//*****************************************************************************
//
//! @file binary_counter.c
//!
//! @brief Example that displays the timer count on the LEDs.
//!
//! Purpose: This example increments a variable on every timer interrupt. The global
//! variable is used to set the state of the LEDs. The example sleeps otherwise.
//!
//! Printing takes place over the SWO/ITM at 1M Baud.
//!
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2022, Ambiq Micro, Inc.
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
// This is part of revision release_sdk_4_3_0-0ca7d78a2b of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

//*****************************************************************************
//
// Macros
//
//*****************************************************************************
#ifdef AM_BSP_NUM_LEDS
#define MAX_COUNT       ((1 << AM_BSP_NUM_LEDS) - 1)
#else
#define NUMLEDS         3       // Default to the expected number of LEDs,
                                // e.g. number found in apollo4b_evb
#define MAX_COUNT       ((1 << NUMLEDS) - 1)
#endif

#define TIMER_NUM       0       // Timer number to be used in the example
#define PRINTF_DISABLE  0       // Nonzero inhibits all SWO printing (LEDs only)

#if PRINTF_DISABLE
#define BC_PRINTF(...)
#else
#define BC_PRINTF(...)  am_util_stdio_printf(__VA_ARGS__)
#endif

//*****************************************************************************
//
// Globals
//
//*****************************************************************************
volatile uint32_t g_ui32TimerCount = 0;

//*****************************************************************************
//
// Function to initialize the timer to interrupt once every second.
//
//*****************************************************************************
void
timer_init(void)
{
    am_hal_timer_config_t   TimerConfig;

    //
    // Set up the desired TIMER.
    // The default config parameters include:
    //  eInputClock = AM_HAL_TIMER_CLOCK_HFRC_DIV16
    //  eFunction = AM_HAL_TIMER_FN_EDGE
    //  Compare0 and Compare1 maxed at 0xFFFFFFFF
    //
    am_hal_timer_default_config_set(&TimerConfig);

    //
    // Modify the default parameters.
    // Configure the timer to a 1s period via ui32Compare1.
    //
    TimerConfig.eFunction        = AM_HAL_TIMER_FN_UPCOUNT;
    TimerConfig.ui32Compare1     = 6000000;

    //
    // Configure the timer
    //
    if ( am_hal_timer_config(TIMER_NUM, &TimerConfig) != AM_HAL_STATUS_SUCCESS )
    {
        BC_PRINTF("Failed to configure TIMER%d.\n", TIMER_NUM);
    }

    //
    // Clear the timer and its interrupt
    //
    am_hal_timer_clear(TIMER_NUM);
    am_hal_timer_interrupt_clear(AM_HAL_TIMER_MASK(TIMER_NUM, AM_HAL_TIMER_COMPARE1));

} // timer_init()

//*****************************************************************************
//
// Timer Interrupt Service Routine (ISR)
//
//*****************************************************************************
void
am_ctimer_isr(void)
{
    //
    // Clear the timer Interrupt (write to clear).
    //
    am_hal_timer_interrupt_clear(AM_HAL_TIMER_MASK(TIMER_NUM, AM_HAL_TIMER_COMPARE1));
    am_hal_timer_clear(TIMER_NUM);

    //
    // Increment count and set limit based on the number of LEDs available.
    //
    g_ui32TimerCount = ( g_ui32TimerCount >= MAX_COUNT ) ? 0 : g_ui32TimerCount + 1;

} // am_ctimer_isr()

//*****************************************************************************
//
// Main function.
//
//*****************************************************************************
int
main(void)
{
    uint32_t ui32Ret;

    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    //
    // Configure the board for low power operation.
    //
    am_bsp_low_power_init();

#ifdef AM_BSP_NUM_LEDS
    //
    // Initialize the LED array
    //
    am_devices_led_array_init(am_bsp_psLEDs, AM_BSP_NUM_LEDS);
#endif

    //
    // Initialize the count so that the counting begins with 0
    // after the first trip to the ISR.
    //
    g_ui32TimerCount = MAX_COUNT;

#if PRINTF_DISABLE
    //
    // Initialize timer.
    //
    timer_init();
#else
    //
    // Initialize the printf interface for SWO/ITM output
    //
    am_bsp_debug_printf_enable();

    //
    // Clear the terminal and print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("Binary Counter Example for %s\n", AM_HAL_DEVICE_NAME);
    am_util_stdio_printf("---------------------------------------------------- \n");
    am_util_stdio_printf("The Timer clock source is HFRC_DIV16.\n");
    am_util_stdio_printf("The Timer wakes once per second to show a binary \n");
    am_util_stdio_printf(" value on the LEDs and a decimal value on SWO.\n");
    am_util_stdio_printf("---------------------------------------------------- \n");

    //
    // Initialize timer.
    //
    timer_init();

    //
    // Disable debug printf messages on SWO.
    //
    am_bsp_debug_printf_disable();
#endif // !PRINTF_DISABLE

    //
    // Enable the timer Interrupt.
    //
    am_hal_timer_interrupt_enable(AM_HAL_TIMER_MASK(TIMER_NUM, AM_HAL_TIMER_COMPARE1));

    //
    // Enable the timer interrupt in the NVIC.
    //
    NVIC_SetPriority(TIMER_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(TIMER_IRQn);
    am_hal_interrupt_master_enable();

    //
    // Start the timer
    //
    am_hal_timer_start(TIMER_NUM);

    //
    // Loop forever.
    //
    while (1)
    {
        //
        // Go to Deep Sleep.
        //
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);

#ifdef AM_BSP_NUM_LEDS
        //
        // Set the LEDs.
        //
        am_devices_led_array_out(am_bsp_psLEDs, AM_BSP_NUM_LEDS, g_ui32TimerCount);
#endif // AM_BSP_NUM_LEDS

#if !PRINTF_DISABLE
        //
        // Enable debug printf messages using SWO.
        //
        am_bsp_debug_printf_enable();
#endif // !PRINTF_DISABLE

        //
        // Output current count.
        //
        BC_PRINTF("%2d ", g_ui32TimerCount);

        if ( g_ui32TimerCount >= MAX_COUNT )
        {
            am_hal_pwrctrl_mcu_mode_e eCurrentPowerMode;

            //
            // Take this opportunity to toggle TurboSPOT mode.
            //
            ui32Ret = am_hal_pwrctrl_mcu_mode_status(&eCurrentPowerMode);
            if ( ui32Ret != AM_HAL_STATUS_SUCCESS )
            {
                BC_PRINTF("\n\nERROR (%d) getting current power mode status\n", ui32Ret);
            }

            if ( eCurrentPowerMode == AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE )
            {
                ui32Ret = am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_LOW_POWER);
                if ( ui32Ret == AM_HAL_STATUS_SUCCESS )
                {
                    BC_PRINTF("\n\nSwitched to LP Low Power Mode (%dMHZ)\n", AM_HAL_CLKGEN_FREQ_MAX_MHZ);
                }
                else
                {
                    BC_PRINTF("\n\nERROR (%d) while switching from TurboSPOT mode to Low Power.\n", ui32Ret);
                }
            }
            else if ( eCurrentPowerMode == AM_HAL_PWRCTRL_MCU_MODE_LOW_POWER )
            {
                ui32Ret = am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE);
                if ( ui32Ret == AM_HAL_STATUS_SUCCESS )
                {
                    BC_PRINTF("\n\nSwitched to HP High Performance TurboSPOT mode (%dMHz)\n", AM_HAL_CLKGEN_FREQ_MAX_MHZ * 2);
                }
                else
                {
                    BC_PRINTF("\n\nERROR (%d) while switching from Low Performance mode to TurboSPOT mode.\n", ui32Ret);
                }
            }
            else
            {
                BC_PRINTF("\n\nERROR, invalid current power mode = (%d).\n", (uint32_t)eCurrentPowerMode);
            }
        }

#if !PRINTF_DISABLE
        //
        // Disable debug printf messages on SWO.
        //
        am_bsp_debug_printf_disable();
#endif // !PRINTF_DISABLE

    } // while()

} // main()
