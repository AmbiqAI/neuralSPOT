//*****************************************************************************
//
//! @file port_systick_stimer.c
//!
//! @brief This is the port file for systick based tick generation and Ambiq
//!        STimer tickless features.
//!
//! Set the configOVERRIDE_DEFAULT_TICK_CONFIGURATION = 0 to use the SYSTICK
//! based tick generation, and set configUSE_TICKLESS_IDLE = 2 to enable STIMER
//! based tickless.
//!
//! The following configuration is valid:
//! 1. configOVERRIDE_DEFAULT_TICK_CONFIGURATION = 0 and configUSE_TICKLESS_IDLE = 0
//!     SYSTICK is used for tick generation, disable tickless feature
//! 2. configOVERRIDE_DEFAULT_TICK_CONFIGURATION = 0 and configUSE_TICKLESS_IDLE = 1
//!     SYSTICK is used for tick generation, 
//!     SYSTICK is used for tickless feature (normal sleep only)
//! 3. configOVERRIDE_DEFAULT_TICK_CONFIGURATION = 0 and configUSE_TICKLESS_IDLE = 2
//!     SYSTICK is used for tick generation, STIMER is used for tickless feature
//! 4. configOVERRIDE_DEFAULT_TICK_CONFIGURATION = 1 and configUSE_TICKLESS_IDLE = 0
//!     STIMER is used for tick generation, disable tickless feature
//! 5. configOVERRIDE_DEFAULT_TICK_CONFIGURATION = 1 and configUSE_TICKLESS_IDLE = 2
//!     STIMER is used for tick generation, STIMER is used for tickless feature
//!
//! The following configuration is invalid:
//!    configOVERRIDE_DEFAULT_TICK_CONFIGURATION = 1 and configUSE_TICKLESS_IDLE = 1
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2025, Ambiq Micro, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision release_sdk5p2-040c7863bb of the AmbiqSuite Development Package.
//
//*****************************************************************************

/* hardware includes */
#include "am_mcu_apollo.h"
#include "am_bsp.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* MPU wrappers includes. */
#include "mpu_wrappers.h"

/* Portasm includes. */
#include "portasm.h"

// Check to make sure the FreeRTOSConfig.h options are consistent per the implementation
#if configOVERRIDE_DEFAULT_TICK_CONFIGURATION == 0

/**
 * @brief Constants required to manipulate the NVIC.
 */
#define portNVIC_SYSTICK_CTRL_REG             ( *( ( volatile uint32_t * ) 0xe000e010 ) )
#define portNVIC_SYSTICK_LOAD_REG             ( *( ( volatile uint32_t * ) 0xe000e014 ) )
#define portNVIC_SYSTICK_CURRENT_VALUE_REG    ( *( ( volatile uint32_t * ) 0xe000e018 ) )
#define portNVIC_SHPR3_REG                    ( *( ( volatile uint32_t * ) 0xe000ed20 ) )
#define portNVIC_SYSTICK_ENABLE_BIT           ( 1UL << 0UL )
#define portNVIC_SYSTICK_INT_BIT              ( 1UL << 1UL )
#define portNVIC_SYSTICK_CLK_BIT              ( 1UL << 2UL )
#define portNVIC_SYSTICK_COUNT_FLAG_BIT       ( 1UL << 16UL )
#define portNVIC_PEND_SYSTICK_CLEAR_BIT       ( 1UL << 25UL )
#define portNVIC_PEND_SYSTICK_SET_BIT         ( 1UL << 26UL )
#define portMIN_INTERRUPT_PRIORITY            ( 255UL )
#define portNVIC_PENDSV_PRI                   ( portMIN_INTERRUPT_PRIORITY << 16UL )
#define portNVIC_SYSTICK_PRI                  ( portMIN_INTERRUPT_PRIORITY << 24UL )

/**
 * @brief The maximum 24-bit number.
 *
 * It is needed because the systick is a 24-bit counter.
 */
#define portMAX_24_BIT_NUMBER       ( 0xffffffUL )

/**
 * @brief A fiddle factor to estimate the number of SysTick counts that would
 * have occurred while the SysTick counter is stopped during tickless idle
 * calculations.
 */
#define portMISSED_COUNTS_FACTOR    ( 94UL )

/**
 * @brief By using the external clock source, systick frequency is fix to 3MHz.
 */
#define configSYSTICK_CLOCK_HZ (AM_HAL_CLKGEN_FREQ_MAX_HZ / 32)

/**
 * @brief Let the user override the default SysTick clock rate.  If defined by the
 * user, this symbol must equal the SysTick clock rate when the CLK bit is 0 in the
 * configuration register.
 */
#ifndef configSYSTICK_CLOCK_HZ
    #define configSYSTICK_CLOCK_HZ             ( configCPU_CLOCK_HZ )
    /* Ensure the SysTick is clocked at the same frequency as the core. */
    #define portNVIC_SYSTICK_CLK_BIT_CONFIG    ( portNVIC_SYSTICK_CLK_BIT )
#else
    /* Select the option to clock SysTick not at the same frequency as the core. */
    #define portNVIC_SYSTICK_CLK_BIT_CONFIG    ( 0 )
#endif

#if ( configUSE_TICKLESS_IDLE == 1 ) || (configUSE_TICKLESS_IDLE == 2)

/**
 * @brief The number of SysTick increments that make up one tick period.
 */
    PRIVILEGED_DATA static uint32_t ulTimerCountsForOneTick = 0;

/**
 * @brief The maximum number of tick periods that can be suppressed is
 * limited by the 24 bit resolution of the SysTick timer.
 */
    PRIVILEGED_DATA static uint32_t xMaximumPossibleSuppressedTicks = 0;

/**
 * @brief Compensate for the CPU cycles that pass while the SysTick is
 * stopped (low power functionality only).
 */
    PRIVILEGED_DATA static uint32_t ulStoppedTimerCompensation = 0;
#endif /* configUSE_TICKLESS_IDLE */

#if configUSE_TICKLESS_IDLE == 2
    //Set the default stimer clock source and clock value.
    #ifdef APOLLO5_FPGA
        #define DEFAULT_STIMER_CLOCK_HZ   ((APOLLO5_FPGA*1000000UL)/16)
    #ifdef AM_PART_APOLLO510L
        #define DEFAULT_STIMER_CLOCK      AM_HAL_STIMER_HFRC_6MHZ
    #else
        #define DEFAULT_STIMER_CLOCK      AM_HAL_STIMER_XTAL_32KHZ
    #endif
    #else
        // #define DEFAULT_STIMER_CLOCK_HZ   32768
        // #define DEFAULT_STIMER_CLOCK      AM_HAL_STIMER_XTAL_32KHZ
        #define DEFAULT_STIMER_CLOCK_HZ   6000000
        #define DEFAULT_STIMER_CLOCK      AM_HAL_STIMER_HFRC_6MHZ
    #endif

    #define portMAX_32_BIT_NUMBER		( 0xffffffffUL )
#endif



    void vPortSetupTimerInterrupt( void )
    {
        /* Calculate the constants required to configure the tick interrupt. */
        #if ( configUSE_TICKLESS_IDLE == 1 )
        {
            ulTimerCountsForOneTick = ( configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ );
            xMaximumPossibleSuppressedTicks = portMAX_24_BIT_NUMBER / ulTimerCountsForOneTick;
            ulStoppedTimerCompensation = portMISSED_COUNTS_FACTOR / ( configCPU_CLOCK_HZ / configSYSTICK_CLOCK_HZ );
        }
        #elif ( configUSE_TICKLESS_IDLE == 2 )
        {
            /* Calculate the constants required to configure the tick interrupt. */
            ulTimerCountsForOneTick = ( configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ );
            xMaximumPossibleSuppressedTicks = portMAX_32_BIT_NUMBER / ulTimerCountsForOneTick - 4;
        }
        #endif /* configUSE_TICKLESS_IDLE */

        /* Stop and reset the SysTick. */
        portNVIC_SYSTICK_CTRL_REG = 0UL;
        portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;

        /* Configure SysTick to interrupt at the requested rate. */
        portNVIC_SYSTICK_LOAD_REG = ( configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
        portNVIC_SYSTICK_CTRL_REG = portNVIC_SYSTICK_CLK_BIT_CONFIG | portNVIC_SYSTICK_INT_BIT | portNVIC_SYSTICK_ENABLE_BIT;


        #if (configUSE_TICKLESS_IDLE == 2)
            uint32_t oldCfg;

            //
            // Enable the timer interrupt in the NVIC, making sure to use the
            // appropriate priority level.
            //
            NVIC_SetPriority(STIMER_CMPR0_IRQn, 0xFF);

            //
            // Configure the STIMER
            //
            oldCfg = am_hal_stimer_config(AM_HAL_STIMER_CFG_FREEZE);
            am_hal_stimer_config((oldCfg & ~(AM_HAL_STIMER_CFG_FREEZE | STIMER_STCFG_CLKSEL_Msk)) | DEFAULT_STIMER_CLOCK | AM_HAL_STIMER_CFG_COMPARE_A_ENABLE);

        #endif
    }

#if ( configUSE_TICKLESS_IDLE == 1 )
    void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
    {
        uint32_t ulReloadValue, ulCompleteTickPeriods, ulCompletedSysTickDecrements, ulSysTickDecrementsLeft;
        TickType_t xModifiableIdleTime;

        /* Make sure the SysTick reload value does not overflow the counter. */
        if( xExpectedIdleTime > xMaximumPossibleSuppressedTicks )
        {
            xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
        }

        /* Enter a critical section but don't use the taskENTER_CRITICAL()
         * method as that will mask interrupts that should exit sleep mode. */
        __asm volatile ( "cpsid i" ::: "memory" );
        __asm volatile ( "dsb" );
        __asm volatile ( "isb" );

        /* If a context switch is pending or a task is waiting for the scheduler
         * to be unsuspended then abandon the low power entry. */
        if( eTaskConfirmSleepModeStatus() == eAbortSleep )
        {
            /* Re-enable interrupts - see comments above the cpsid instruction
             * above. */
            __asm volatile ( "cpsie i" ::: "memory" );
        }
        else
        {
            /* Stop the SysTick momentarily.  The time the SysTick is stopped for
             * is accounted for as best it can be, but using the tickless mode will
             * inevitably result in some tiny drift of the time maintained by the
             * kernel with respect to calendar time. */
            portNVIC_SYSTICK_CTRL_REG = ( portNVIC_SYSTICK_CLK_BIT_CONFIG | portNVIC_SYSTICK_INT_BIT );

            /* Use the SysTick current-value register to determine the number of
             * SysTick decrements remaining until the next tick interrupt.  If the
             * current-value register is zero, then there are actually
             * ulTimerCountsForOneTick decrements remaining, not zero, because the
             * SysTick requests the interrupt when decrementing from 1 to 0. */
            ulSysTickDecrementsLeft = portNVIC_SYSTICK_CURRENT_VALUE_REG;

            if( ulSysTickDecrementsLeft == 0 )
            {
                ulSysTickDecrementsLeft = ulTimerCountsForOneTick;
            }

            /* Calculate the reload value required to wait xExpectedIdleTime
             * tick periods.  -1 is used because this code normally executes part
             * way through the first tick period.  But if the SysTick IRQ is now
             * pending, then clear the IRQ, suppressing the first tick, and correct
             * the reload value to reflect that the second tick period is already
             * underway.  The expected idle time is always at least two ticks. */
            ulReloadValue = ulSysTickDecrementsLeft + ( ulTimerCountsForOneTick * ( xExpectedIdleTime - 1UL ) );

            if( ( portNVIC_INT_CTRL_REG & portNVIC_PEND_SYSTICK_SET_BIT ) != 0 )
            {
                portNVIC_INT_CTRL_REG = portNVIC_PEND_SYSTICK_CLEAR_BIT;
                ulReloadValue -= ulTimerCountsForOneTick;
            }

            if( ulReloadValue > ulStoppedTimerCompensation )
            {
                ulReloadValue -= ulStoppedTimerCompensation;
            }

            /* Set the new reload value. */
            portNVIC_SYSTICK_LOAD_REG = ulReloadValue;

            /* Clear the SysTick count flag and set the count value back to
             * zero. */
            portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;

            /* Restart SysTick. */
            portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;

            /* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
             * set its parameter to 0 to indicate that its implementation contains
             * its own wait for interrupt or wait for event instruction, and so wfi
             * should not be executed again.  However, the original expected idle
             * time variable must remain unmodified, so a copy is taken. */
            xModifiableIdleTime = xExpectedIdleTime;

            /* Set PWRCTRL register to use ARM sleep mode. The default sleep mode is Ambiq sleep mode,
             * in this mode, the SYSTICK timer input clock will be gated, so we need to switch to ARM sleep mode
             * to keep the clock active.*/
            PWRCTRL->CPUPWRCTRL_b.SLEEPMODE = 1;

            configPRE_SLEEP_PROCESSING( xModifiableIdleTime );

            if( xModifiableIdleTime > 0 )
            {
                __asm volatile ( "dsb" ::: "memory" );
                __asm volatile ( "wfi" );
                __asm volatile ( "isb" );
            }

            configPOST_SLEEP_PROCESSING( xExpectedIdleTime );

            /* Re-enable interrupts to allow the interrupt that brought the MCU
             * out of sleep mode to execute immediately.  See comments above
             * the cpsid instruction above. */
            __asm volatile ( "cpsie i" ::: "memory" );
            __asm volatile ( "dsb" );
            __asm volatile ( "isb" );

            /* Disable interrupts again because the clock is about to be stopped
             * and interrupts that execute while the clock is stopped will increase
             * any slippage between the time maintained by the RTOS and calendar
             * time. */
            __asm volatile ( "cpsid i" ::: "memory" );
            __asm volatile ( "dsb" );
            __asm volatile ( "isb" );

            /* Disable the SysTick clock without reading the
             * portNVIC_SYSTICK_CTRL_REG register to ensure the
             * portNVIC_SYSTICK_COUNT_FLAG_BIT is not cleared if it is set.  Again,
             * the time the SysTick is stopped for is accounted for as best it can
             * be, but using the tickless mode will inevitably result in some tiny
             * drift of the time maintained by the kernel with respect to calendar
             * time*/
            portNVIC_SYSTICK_CTRL_REG = ( portNVIC_SYSTICK_CLK_BIT_CONFIG | portNVIC_SYSTICK_INT_BIT );

            /* Determine whether the SysTick has already counted to zero. */
            if( ( portNVIC_SYSTICK_CTRL_REG & portNVIC_SYSTICK_COUNT_FLAG_BIT ) != 0 )
            {
                uint32_t ulCalculatedLoadValue;

                /* The tick interrupt ended the sleep (or is now pending), and
                 * a new tick period has started.  Reset portNVIC_SYSTICK_LOAD_REG
                 * with whatever remains of the new tick period. */
                ulCalculatedLoadValue = ( ulTimerCountsForOneTick - 1UL ) - ( ulReloadValue - portNVIC_SYSTICK_CURRENT_VALUE_REG );

                /* Don't allow a tiny value, or values that have somehow
                 * underflowed because the post sleep hook did something
                 * that took too long or because the SysTick current-value register
                 * is zero. */
                if( ( ulCalculatedLoadValue <= ulStoppedTimerCompensation ) || ( ulCalculatedLoadValue > ulTimerCountsForOneTick ) )
                {
                    ulCalculatedLoadValue = ( ulTimerCountsForOneTick - 1UL );
                }

                portNVIC_SYSTICK_LOAD_REG = ulCalculatedLoadValue;

                /* As the pending tick will be processed as soon as this
                 * function exits, the tick value maintained by the tick is stepped
                 * forward by one less than the time spent waiting. */
                ulCompleteTickPeriods = xExpectedIdleTime - 1UL;
            }
            else
            {
                /* Something other than the tick interrupt ended the sleep. */

                /* Use the SysTick current-value register to determine the
                 * number of SysTick decrements remaining until the expected idle
                 * time would have ended. */
                ulSysTickDecrementsLeft = portNVIC_SYSTICK_CURRENT_VALUE_REG;
                #if ( portNVIC_SYSTICK_CLK_BIT_CONFIG != portNVIC_SYSTICK_CLK_BIT )
                {
                    /* If the SysTick is not using the core clock, the current-
                     * value register might still be zero here.  In that case, the
                     * SysTick didn't load from the reload register, and there are
                     * ulReloadValue decrements remaining in the expected idle
                     * time, not zero. */
                    if( ulSysTickDecrementsLeft == 0 )
                    {
                        ulSysTickDecrementsLeft = ulReloadValue;
                    }
                }
                #endif /* portNVIC_SYSTICK_CLK_BIT_CONFIG */

                /* Work out how long the sleep lasted rounded to complete tick
                 * periods (not the ulReload value which accounted for part
                 * ticks). */
                ulCompletedSysTickDecrements = ( xExpectedIdleTime * ulTimerCountsForOneTick ) - ulSysTickDecrementsLeft;

                /* How many complete tick periods passed while the processor
                 * was waiting? */
                ulCompleteTickPeriods = ulCompletedSysTickDecrements / ulTimerCountsForOneTick;

                /* The reload value is set to whatever fraction of a single tick
                 * period remains. */
                portNVIC_SYSTICK_LOAD_REG = ( ( ulCompleteTickPeriods + 1UL ) * ulTimerCountsForOneTick ) - ulCompletedSysTickDecrements;
            }

            /* Restart SysTick so it runs from portNVIC_SYSTICK_LOAD_REG again,
             * then set portNVIC_SYSTICK_LOAD_REG back to its standard value.  If
             * the SysTick is not using the core clock, temporarily configure it to
             * use the core clock.  This configuration forces the SysTick to load
             * from portNVIC_SYSTICK_LOAD_REG immediately instead of at the next
             * cycle of the other clock.  Then portNVIC_SYSTICK_LOAD_REG is ready
             * to receive the standard value immediately. */
            portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;
            portNVIC_SYSTICK_CTRL_REG = portNVIC_SYSTICK_CLK_BIT | portNVIC_SYSTICK_INT_BIT | portNVIC_SYSTICK_ENABLE_BIT;
            #if ( portNVIC_SYSTICK_CLK_BIT_CONFIG == portNVIC_SYSTICK_CLK_BIT )
            {
                portNVIC_SYSTICK_LOAD_REG = ulTimerCountsForOneTick - 1UL;
            }
            #else
            {
                /* The temporary usage of the core clock has served its purpose,
                 * as described above.  Resume usage of the other clock. */
                portNVIC_SYSTICK_CTRL_REG = portNVIC_SYSTICK_CLK_BIT | portNVIC_SYSTICK_INT_BIT;

                if( ( portNVIC_SYSTICK_CTRL_REG & portNVIC_SYSTICK_COUNT_FLAG_BIT ) != 0 )
                {
                    /* The partial tick period already ended.  Be sure the SysTick
                     * counts it only once. */
                    portNVIC_SYSTICK_CURRENT_VALUE_REG = 0;
                }

                portNVIC_SYSTICK_LOAD_REG = ulTimerCountsForOneTick - 1UL;
                portNVIC_SYSTICK_CTRL_REG = portNVIC_SYSTICK_CLK_BIT_CONFIG | portNVIC_SYSTICK_INT_BIT | portNVIC_SYSTICK_ENABLE_BIT;
            }
            #endif /* portNVIC_SYSTICK_CLK_BIT_CONFIG */

            /* Step the tick to account for any tick periods that elapsed. */
            vTaskStepTick( ulCompleteTickPeriods );

            /* Exit with interrupts enabled. */
            __asm volatile ( "cpsie i" ::: "memory" );
        }
    }
#elif (configUSE_TICKLESS_IDLE == 2)
    //*****************************************************************************
    //
    // Interrupt handler for the STIMER module Compare 0.
    //
    //*****************************************************************************
    void
    am_stimer_cmpr0_isr(void)
    {

        //
        // Check the timer interrupt status.
        //
        uint32_t ui32Status = am_hal_stimer_int_status_get(false);
        if (ui32Status & AM_HAL_STIMER_INT_COMPAREA)
        {
            am_hal_stimer_int_clear(AM_HAL_STIMER_INT_COMPAREA);
        }
    }


    void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
    {
        uint32_t ulReloadValue;
        TickType_t xModifiableIdleTime;

        uint32_t uiSTimerCountEnterSleep;
        uint32_t uiSTimerCountExitSleep;
        uint32_t ulSTimerElapsedCount;
        uint32_t ulSTimerDelta;
        uint32_t ulSysTickDecrementsLeft;
        uint32_t ulCompleteTickPeriods;



        /* Make sure the SysTick reload value does not overflow the counter. */
        if( xExpectedIdleTime > xMaximumPossibleSuppressedTicks )
        {
            xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
        }

        /* Enter a critical section but don't use the taskENTER_CRITICAL()
         * method as that will mask interrupts that should exit sleep mode. */
        __asm volatile ( "cpsid i" ::: "memory" );
        __asm volatile ( "dsb" );
        __asm volatile ( "isb" );

        /* If a context switch is pending or a task is waiting for the scheduler
         * to be unsuspended then abandon the low power entry. */
        if( eTaskConfirmSleepModeStatus() == eAbortSleep )
        {
            /* Re-enable interrupts - see comments above the cpsid instruction
             * above. */
            __asm volatile ( "cpsie i" ::: "memory" );
        }
        else
        {
            /*Clear the pending STIMER interrupt and reenable it*/
            NVIC_ClearPendingIRQ(STIMER_CMPR0_IRQn);
            NVIC_EnableIRQ(STIMER_CMPR0_IRQn);
            am_hal_stimer_int_enable(AM_HAL_STIMER_INT_COMPAREA);

            /* Stop the SysTick momentarily.  The time the SysTick is stopped for
             * is accounted for as best it can be, but using the tickless mode will
             * inevitably result in some tiny drift of the time maintained by the
             * kernel with respect to calendar time. */
            portNVIC_SYSTICK_CTRL_REG = ( portNVIC_SYSTICK_CLK_BIT_CONFIG | portNVIC_SYSTICK_INT_BIT );

            /* Get STIMER count, STIMER will be used to wake up CPU*/
            uiSTimerCountEnterSleep = am_hal_stimer_counter_get();

            /* Use the SysTick current-value register to determine the number of
             * SysTick decrements remaining until the next tick interrupt.  If the
             * current-value register is zero, then there are actually
             * ulTimerCountsForOneTick decrements remaining, not zero, because the
             * SysTick requests the interrupt when decrementing from 1 to 0. */
            ulSysTickDecrementsLeft = portNVIC_SYSTICK_CURRENT_VALUE_REG;

            if( ulSysTickDecrementsLeft == 0 )
            {
                ulSysTickDecrementsLeft = ulTimerCountsForOneTick;
            }

            /* Calculate the reload value required to wait xExpectedIdleTime
             * tick periods.  -1 is used because this code normally executes part
             * way through the first tick period.  But if the SysTick IRQ is now
             * pending, then clear the IRQ, suppressing the first tick, and correct
             * the reload value to reflect that the second tick period is already
             * underway.  The expected idle time is always at least two ticks. */
            ulReloadValue = ulSysTickDecrementsLeft + ( ulTimerCountsForOneTick * ( xExpectedIdleTime - 1UL ) );

            if( ( portNVIC_INT_CTRL_REG & portNVIC_PEND_SYSTICK_SET_BIT ) != 0 )
            {
                portNVIC_INT_CTRL_REG = portNVIC_PEND_SYSTICK_CLEAR_BIT;
                ulReloadValue -= ulTimerCountsForOneTick;
            }

            /* TODO: We need to measure the Compensation value at different CPU clock*/
            // if( ulReloadValue > ulStoppedTimerCompensation )
            // {
            //     ulReloadValue -= ulStoppedTimerCompensation;
            // }

            /*Calculated the except sleep time for STIMER*/
            ulSTimerDelta = (uint64_t)ulReloadValue * DEFAULT_STIMER_CLOCK_HZ / configSYSTICK_CLOCK_HZ;


            /*Set for the time to wake up CPU*/
            am_hal_stimer_compare_delta_set(0, ulSTimerDelta);

            /* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
             * set its parameter to 0 to indicate that its implementation contains
             * its own wait for interrupt or wait for event instruction, and so wfi
             * should not be executed again.  However, the original expected idle
             * time variable must remain unmodified, so a copy is taken. */
            xModifiableIdleTime = xExpectedIdleTime;
            configPRE_SLEEP_PROCESSING( xModifiableIdleTime );


            if( xModifiableIdleTime > 0 )
            {
                __asm volatile ( "dsb" ::: "memory" );
                __asm volatile ( "wfi" );
                __asm volatile ( "isb" );
            }

            configPOST_SLEEP_PROCESSING( xExpectedIdleTime );


            /* Re-enable interrupts to allow the interrupt that brought the MCU
             * out of sleep mode to execute immediately.  See comments above
             * the cpsid instruction above. */
            __asm volatile ( "cpsie i" ::: "memory" );
            __asm volatile ( "dsb" );
            __asm volatile ( "isb" );

            /* Disable interrupts again because the clock is about to be stopped
             * and interrupts that execute while the clock is stopped will increase
             * any slippage between the time maintained by the RTOS and calendar
             * time. */
            __asm volatile ( "cpsid i" ::: "memory" );
            __asm volatile ( "dsb" );
            __asm volatile ( "isb" );

            // Any interrupt may have woken us up

            // Before renable interrupts, check how many ticks the processor has been in SLEEP
            uiSTimerCountExitSleep = am_hal_stimer_counter_get();
            ulSTimerElapsedCount = (uiSTimerCountExitSleep < uiSTimerCountEnterSleep) ? \
                            ((portMAX_32_BIT_NUMBER - uiSTimerCountEnterSleep) + uiSTimerCountExitSleep) :
                            (uiSTimerCountExitSleep - uiSTimerCountEnterSleep);

            if(ulSTimerElapsedCount >= ulSTimerDelta)
            {
                /* The STIMER interrupt ended the sleep. */

                /*The wake up procedure may take sometime, so we usually sleep longer than the expected time.
                 Get the exceed count, convert it to systick count. */
                uint32_t ulStimerExceedCount = ulSTimerElapsedCount - ulSTimerDelta;
                uint32_t ulSystickExceedCount = (uint64_t)ulStimerExceedCount * configSYSTICK_CLOCK_HZ / DEFAULT_STIMER_CLOCK_HZ;

                /*The wake up procedure may take sometime, make sure the wake up procedure
                  takes no more than one tick*/
                configASSERT(ulSystickExceedCount < ulTimerCountsForOneTick);

                /* The tick interrupt ended the sleep (or is now pending), and
                 * a new tick period has started.  Reset portNVIC_SYSTICK_LOAD_REG
                 * with whatever remains of the new tick period. */
                uint32_t ulCalculatedLoadValue = ( ulTimerCountsForOneTick - 1UL ) - ulSystickExceedCount;

                /* Don't allow a tiny value, or values that have somehow
                 * underflowed because the post sleep hook did something
                 * that took too long or because the SysTick current-value register
                 * is zero. */
                if(  ulCalculatedLoadValue > ulTimerCountsForOneTick )
                {
                    ulCalculatedLoadValue = ( ulTimerCountsForOneTick - 1UL );
                }

                portNVIC_SYSTICK_LOAD_REG = ulCalculatedLoadValue;

                /* We sleep for enough tick*/
                ulCompleteTickPeriods = xExpectedIdleTime;

            }
            else
            {
                /* Something other than the tick interrupt ended the sleep. */

                /* Calculate the remaining stimer count until the expected idle time would have ended. */
                uint32_t ulStimerCountLeft = ulSTimerDelta - ulSTimerElapsedCount;

                /* Convert this value ti systick count */
                ulSysTickDecrementsLeft = (uint64_t)ulStimerCountLeft * configSYSTICK_CLOCK_HZ / DEFAULT_STIMER_CLOCK_HZ;


                /* Work out how long the sleep lasted rounded to complete tick
                 * periods (not the ulReload value which accounted for part
                 * ticks). */
                uint32_t ulCompletedSysTickDecrements = ( xExpectedIdleTime * ulTimerCountsForOneTick ) - ulSysTickDecrementsLeft;

                /* How many complete tick periods passed while the processor
                 * was waiting? */
                ulCompleteTickPeriods = ulCompletedSysTickDecrements / ulTimerCountsForOneTick;

                /* The reload value is set to whatever fraction of a single tick
                 * period remains. */
                portNVIC_SYSTICK_LOAD_REG = ( ( ulCompleteTickPeriods + 1UL ) * ulTimerCountsForOneTick ) - ulCompletedSysTickDecrements;
            }

            /* Restart SysTick so it runs from portNVIC_SYSTICK_LOAD_REG again,
             * then set portNVIC_SYSTICK_LOAD_REG back to its standard value.  If
             * the SysTick is not using the core clock, temporarily configure it to
             * use the core clock.  This configuration forces the SysTick to load
             * from portNVIC_SYSTICK_LOAD_REG immediately instead of at the next
             * cycle of the other clock.  Then portNVIC_SYSTICK_LOAD_REG is ready
             * to receive the standard value immediately. */
            portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;
            portNVIC_SYSTICK_CTRL_REG = portNVIC_SYSTICK_CLK_BIT | portNVIC_SYSTICK_INT_BIT | portNVIC_SYSTICK_ENABLE_BIT;
            #if ( portNVIC_SYSTICK_CLK_BIT_CONFIG == portNVIC_SYSTICK_CLK_BIT )
            {
                portNVIC_SYSTICK_LOAD_REG = ulTimerCountsForOneTick - 1UL;
            }
            #else
            {
                /* The temporary usage of the core clock has served its purpose,
                 * as described above.  Resume usage of the other clock. */
                portNVIC_SYSTICK_CTRL_REG = portNVIC_SYSTICK_CLK_BIT | portNVIC_SYSTICK_INT_BIT;

                if( ( portNVIC_SYSTICK_CTRL_REG & portNVIC_SYSTICK_COUNT_FLAG_BIT ) != 0 )
                {
                    /* The partial tick period already ended.  Be sure the SysTick
                     * counts it only once. */
                    portNVIC_SYSTICK_CURRENT_VALUE_REG = 0;
                }

                portNVIC_SYSTICK_LOAD_REG = ulTimerCountsForOneTick - 1UL;
                portNVIC_SYSTICK_CTRL_REG = portNVIC_SYSTICK_CLK_BIT_CONFIG | portNVIC_SYSTICK_INT_BIT | portNVIC_SYSTICK_ENABLE_BIT;
            }
            #endif /* portNVIC_SYSTICK_CLK_BIT_CONFIG */

            // Clear the interrupt - to avoid extra tick counting in ISR
            am_hal_stimer_int_disable(AM_HAL_STIMER_INT_COMPAREA);
            am_hal_stimer_int_clear(AM_HAL_STIMER_INT_COMPAREA);
            NVIC_ClearPendingIRQ(STIMER_CMPR0_IRQn);
            NVIC_DisableIRQ(STIMER_CMPR0_IRQn);

            /* Step the tick to account for any tick periods that elapsed. */
            vTaskStepTick( ulCompleteTickPeriods );

            /* Exit with interrupts enabled. */
            __asm volatile ( "cpsie i" ::: "memory" );
        }
    }

    #endif

#endif