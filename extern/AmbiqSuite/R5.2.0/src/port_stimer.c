//*****************************************************************************
//
//! @file port_stimer.c
//!
//! @brief This is the port file for Ambiq STimer based system tick generation 
//!        and tickless features.
//!
//! This is the port file for Ambiq STIMER based system tick generation 
//! and tickless features.
//! Set the configOVERRIDE_DEFAULT_TICK_CONFIGURATION = 1 to enable the STIMER
//! based tick generation.
//! Set the configUSE_TICKLESS_IDLE = 2 to enable tickless based on STIMER.
//!
//! The following configuration is valid:
//! 1. configOVERRIDE_DEFAULT_TICK_CONFIGURATION = 0 and configUSE_TICKLESS_IDLE = 0
//!     SYSTICK is used for tick generation, disable tickless feature
//! 2. configOVERRIDE_DEFAULT_TICK_CONFIGURATION = 0 and configUSE_TICKLESS_IDLE = 1
//!     SYSTICK is used for tick generation, systick is used for tickless feature
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
#if configOVERRIDE_DEFAULT_TICK_CONFIGURATION == 1

    #if configUSE_TICKLESS_IDLE == 1
        #error "configOVERRIDE_DEFAULT_TICK_CONFIGURATION == 1 supported only for configUSE_TICKLESS_IDLE = 2"
    #endif

    //Set the default stimer clock source and clock value.
    #ifdef APOLLO5_FPGA
        #define DEFAULT_STIMER_CLOCK_HZ   ((APOLLO5_FPGA*1000000UL)/16)
    #ifdef AM_PART_APOLLO510L
        #define DEFAULT_STIMER_CLOCK      AM_HAL_STIMER_HFRC_6MHZ
    #else
        #define DEFAULT_STIMER_CLOCK      AM_HAL_STIMER_XTAL_32KHZ
    #endif
    #else
        #define DEFAULT_STIMER_CLOCK_HZ   32768
        #define DEFAULT_STIMER_CLOCK      AM_HAL_STIMER_XTAL_32KHZ
    #endif

    #define portMAX_32_BIT_NUMBER		( 0xffffffffUL )

	// Keeps the snapshot of the STimer corresponding to last tick update
	static uint32_t g_lastSTimerVal = 0;

    /*
     * @brief The number of SysTick increments that make up one tick period.
     */
	static uint32_t ulTimerCountsForOneTick = 0;

    #if configUSE_TICKLESS_IDLE == 2
        /*
        * The maximum number of tick periods that can be suppressed is limited by the
        * resolution of the Tick timer.
        */
	    static uint32_t xMaximumPossibleSuppressedTicks = 0;
    #endif



//*****************************************************************************
//
// Events associated with STimer CMP0 Interrupt
//
//  This is the FreeRTOS System Timer
//
//  Real Time events must be controlled by FreeRTOS as the Stimer is also used for sleep functions.
//  At any time the Stimer->Cmp0 interrupt can be a regular Tick interrupt or
//  an interrupt from a long Deep_Sleep().
//  The Deep Sleep in entered in Port.c->vPortSuppressTicksAndSleep() which is
//  entered from the IDLE task.
//  If no tasks are READY to run, vPortSuppressTicksAndSleep() is called.
//  See tasks.c-> portTASK_FUNCTION(...)
//
//
//
//
//*****************************************************************************
void
xPortStimerTickHandler(void)
{
    uint32_t remainder = 0;

    BaseType_t ctxtSwitchReqd = pdFALSE;

    // Calculate elapsed tick

    uint32_t curSTimer = am_hal_stimer_counter_get();
    uint32_t elapsed_count = (curSTimer < g_lastSTimerVal) ? \
                             ((portMAX_32_BIT_NUMBER - g_lastSTimerVal) + curSTimer) :
                             (curSTimer - g_lastSTimerVal);
    uint32_t elapsed_tick = elapsed_count / ulTimerCountsForOneTick;

    //Update the timer record value.
    remainder = elapsed_count % ulTimerCountsForOneTick;
    g_lastSTimerVal = curSTimer - remainder;

    // Configure the STIMER->COMPARE_0  
    //The remainder may accumulate to a big value, we need to fine adjust the 
    //delta value to compensate this accumulative error.
    am_hal_stimer_compare_delta_set(0, (ulTimerCountsForOneTick - remainder));


    //
    // This is a timer a0 interrupt, perform the necessary functions
    // for the tick ISR.
    //
    (void) portSET_INTERRUPT_MASK_FROM_ISR();
    {
        //
        // Increment RTOS tick
        // Allowing for need to increment the tick more than one... to avoid accumulation of
        // error in case of interrupt latencies
        //
        while (elapsed_tick--)
        {
            ctxtSwitchReqd = (( xTaskIncrementTick() != pdFALSE ) ? pdTRUE : ctxtSwitchReqd);
        }
        if ( ctxtSwitchReqd != pdFALSE )
        {
            //
            // A context switch is required.  Context switching is
            // performed in the PendSV interrupt. Pend the PendSV
            // interrupt.
            //
            portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;
        }
    }
    portCLEAR_INTERRUPT_MASK_FROM_ISR(0);
}


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

        //
        // Run handlers for the various possible timer events.
        //
        xPortStimerTickHandler();
    }
}






void vPortSetupTimerInterrupt( void )
{

    uint32_t oldCfg;

    /* Calculate the constants required to configure the tick interrupt. */
    ulTimerCountsForOneTick = (DEFAULT_STIMER_CLOCK_HZ /configTICK_RATE_HZ) ;

    #if configUSE_TICKLESS_IDLE == 2
    {
        xMaximumPossibleSuppressedTicks = portMAX_32_BIT_NUMBER / ulTimerCountsForOneTick - 4;
    }
    #endif /* configUSE_TICKLESS_IDLE */

    //
    // Enable the COMPAREA interrupt in STIMER.
    //
    am_hal_stimer_int_enable(AM_HAL_STIMER_INT_COMPAREA);


    //
    // Enable the timer interrupt in the NVIC, making sure to use the
    // appropriate priority level.
    //
    NVIC_SetPriority(STIMER_CMPR0_IRQn, 0xFF);
    NVIC_EnableIRQ(STIMER_CMPR0_IRQn);


    //
    // Configure the STIMER
    //
    oldCfg = am_hal_stimer_config(AM_HAL_STIMER_CFG_FREEZE);
    g_lastSTimerVal = am_hal_stimer_counter_get();
    am_hal_stimer_compare_delta_set(0, ulTimerCountsForOneTick);

    am_hal_stimer_config((oldCfg & ~(AM_HAL_STIMER_CFG_FREEZE | STIMER_STCFG_CLKSEL_Msk)) | DEFAULT_STIMER_CLOCK | AM_HAL_STIMER_CFG_COMPARE_A_ENABLE);
}

#if (configUSE_TICKLESS_IDLE == 2)
void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
{
    uint32_t ulReloadValue;
    TickType_t xModifiableIdleTime;

    uint32_t ulCurrentSTimerCount;
    uint32_t ulElapsedCount;
    uint32_t ulElapsedTick;
    uint32_t ulElapsedCountRemainder;


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
        // Adjust for the time already elapsed
        ulCurrentSTimerCount = am_hal_stimer_counter_get();
        ulElapsedCount = (ulCurrentSTimerCount < g_lastSTimerVal) ? \
                        ((portMAX_32_BIT_NUMBER - g_lastSTimerVal) + ulCurrentSTimerCount) :
                        (ulCurrentSTimerCount - g_lastSTimerVal);


        /* Calculate the compare value required to wait xExpectedIdleTime
         * tick periods.   The expected idle time is always at least two ticks. */
        ulReloadValue = ( ulTimerCountsForOneTick * xExpectedIdleTime ) - ulElapsedCount;


        // Initialize new timeout value
        am_hal_stimer_compare_delta_set(0, ulReloadValue);

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

        // Any interrupt may have woken us up

        // Before renable interrupts, check how many ticks the processor has been in SLEEP
        // Adjust xTickCount via vTaskStepTick( Delta_Sleep )
        // to keep xTickCount up to date, as if ticks have been running all along
        ulCurrentSTimerCount = am_hal_stimer_counter_get();
        ulElapsedCount = (ulCurrentSTimerCount < g_lastSTimerVal) ? \
                        ((portMAX_32_BIT_NUMBER - g_lastSTimerVal) + ulCurrentSTimerCount) :
                        (ulCurrentSTimerCount - g_lastSTimerVal);
        ulElapsedTick =  ulElapsedCount / ulTimerCountsForOneTick;

        //Update recorded stimer counter value 
        ulElapsedCountRemainder = ulElapsedCount % ulTimerCountsForOneTick;               
        g_lastSTimerVal = ulCurrentSTimerCount - ulElapsedCountRemainder;

        /* Restart System Tick */

        // Clear the interrupt - to avoid extra tick counting in ISR
        am_hal_stimer_int_clear(AM_HAL_STIMER_INT_COMPAREA);
        NVIC_ClearPendingIRQ(STIMER_CMPR0_IRQn);

        //Set the new stimer interrupt delta value
        am_hal_stimer_compare_delta_set(0, (ulTimerCountsForOneTick - ulElapsedCountRemainder) );

 
        /* Step the tick to account for any tick periods that elapsed. */
        vTaskStepTick( ulElapsedTick );

        /* Exit with interrupts enabled. */
        __asm volatile ( "cpsie i" ::: "memory" );
    }
}
#endif

#endif