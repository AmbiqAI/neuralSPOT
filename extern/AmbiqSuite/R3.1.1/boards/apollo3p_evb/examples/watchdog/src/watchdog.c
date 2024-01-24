//*****************************************************************************
//
//! @file watchdog.c
//!
//! @brief Example of a basic configuration of the watchdog.
//!
//! Purpose: This example shows a simple configuration of the watchdog. It will print
//! a banner message, configure the watchdog for both interrupt and reset
//! generation, and immediately start the watchdog timer.
//! The watchdog ISR provided will 'pet' the watchdog four times, printing
//! a notification message from the ISR each time.
//! On the fifth interrupt, the watchdog will not be pet, so the 'reset'
//! action will eventually be allowed to occur.
//! On the sixth timeout event, the WDT should issue a system reset, and the
//! program should start over from the beginning.
//!
//! Printing takes place over the ITM at 1M Baud.
//!
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2023, Ambiq Micro, Inc.
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
// This is part of revision release_sdk_3_1_1-10cda4b5e0 of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

//*****************************************************************************
//
// Global Variables
//
//*****************************************************************************
uint8_t g_ui8NumWatchdogInterrupts = 0;
uint32_t g_ui32ResetStatus = 0;

am_hal_wdt_config_t g_sWatchdogConfig =
{
    //
    // Select the Apollo 1 Clock Rate.
    //
#ifdef AM_PART_APOLLO
    .ui32Config = AM_HAL_WDT_ENABLE_RESET | AM_HAL_WDT_ENABLE_INTERRUPT,
#endif
#if defined(AM_PART_APOLLO2) || defined(AM_PART_APOLLO3) || defined(AM_PART_APOLLO3P)
    .ui32Config =
        _VAL2FLD(WDT_CFG_CLKSEL, WDT_CFG_CLKSEL_128HZ)  |
        AM_HAL_WDT_ENABLE_RESET                         |
        AM_HAL_WDT_ENABLE_INTERRUPT,
#endif
    //
    // Set WDT interrupt timeout for 3/4 second.
    //
    .ui16InterruptCount = 128 * 3 / 4,

    //
    // Set WDT reset timeout for 1.5 seconds.
    //
    .ui16ResetCount = 128 * 3 / 2
};

//*****************************************************************************
//
// Interrupt handler for the watchdog.
//
//*****************************************************************************
void
am_watchdog_isr(void)
{
    //
    // Clear the watchdog interrupt.
    //
    am_hal_wdt_int_clear();

    //
    // Catch the first four watchdog interrupts, but let the fifth through
    // untouched.
    //
    if ( g_ui8NumWatchdogInterrupts < 4 )
    {
        //
        // Restart the watchdog.
        //
        am_hal_wdt_restart();
    }

    //
    // Enable debug printf messages using ITM on SWO pin
    //
    am_bsp_debug_printf_enable();

    //
    // Send a status message and give it some time to print.
    //
    am_util_stdio_printf("Interrupt #: %d\n", g_ui8NumWatchdogInterrupts);
    am_util_delay_ms(100);

    //
    // Increment the number of watchdog interrupts.
    //
    g_ui8NumWatchdogInterrupts++;

} // am_watchdog_isr()

//*****************************************************************************
//
// Main function.
//
//*****************************************************************************
int
main(void)
{
    am_hal_reset_status_t sStatus;

    //
    // Set the clock frequency.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);

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
    // Initialize device drivers for the LEDs on the board.
    //
    am_devices_led_array_init(am_bsp_psLEDs, AM_BSP_NUM_LEDS);
#endif

    //
    // Initialize the printf interface for ITM/SWO output.
    //
    am_bsp_itm_printf_enable();

    //
    // Clear the terminal screen, and print a quick message to show that we're
    // alive.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("Watchdog Example.\n");

    //
    // Print out reset status register upon entry.
    //
    am_hal_reset_status_get(&sStatus);
    g_ui32ResetStatus = sStatus.eStatus;

    am_util_stdio_printf("Reset Status Register = 0x%x\n",
                          g_ui32ResetStatus);

    //
    // Clear reset status register for next time we reset.
    //
    am_hal_reset_control(AM_HAL_RESET_CONTROL_STATUSCLEAR, 0);

    //
    // LFRC has to be turned on for this example because the watchdog only
    // runs off of the LFRC.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_LFRC_START, 0);

    //
    // Configure the watchdog.
    //
    am_hal_wdt_init(&g_sWatchdogConfig);

    //
    // Enable the interrupt for the watchdog in the NVIC.
    //
    NVIC_EnableIRQ(WDT_IRQn);
    am_hal_interrupt_master_enable();

    //
    // Enable the watchdog.
    //
    am_hal_wdt_start();

    //
    // Loop forever.
    //
    while (1)
    {
        //
        // We are done printing. Disable debug printf messages on ITM.
        //
        am_bsp_debug_printf_disable();

        //
        // Disable interrupts.
        //
        am_hal_interrupt_master_disable();

#ifdef AM_BSP_NUM_LEDS
        //
        // Turn OFF the indicator LED.
        //
        am_devices_led_off(am_bsp_psLEDs, 0);
#endif

        //
        // Go to deep sleep.
        //
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);

#ifdef AM_BSP_NUM_LEDS
        //
        // Turn ON the indicator LED.
        //
        am_devices_led_on(am_bsp_psLEDs, 0);
#endif

        //
        // An interrupt woke us up so now enable them and take it.
        //
        am_hal_interrupt_master_enable();
    }
}

