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
// Global Variables
//
//*****************************************************************************
uint8_t g_ui8NumWatchdogInterrupts = 0;
uint32_t g_ui32ResetStatus = 0;

am_hal_wdt_config_t g_sWatchdogConfig =
{
    // Set the clock for 16HZ
    .eClockSource = AM_HAL_WDT_16HZ,

    // Enable the interrupt, and set it for 3/4 of a second.
    .bInterruptEnable = true,
    .ui32InterruptValue = 16 * 3 / 4,

    // Enable the reset, and set it for 15 seconds.
    .bResetEnable = true,
    .ui32ResetValue = 16 * 15,

    // No DSP.
    .bAlertOnDSPReset = false,
};

//*****************************************************************************
//
// Interrupt handler for the watchdog.
//
//*****************************************************************************
void
am_watchdog_isr(void)
{
    uint32_t ui32Status;

    //
    // Read and clear the interrupt status.
    //
    am_hal_wdt_interrupt_status_get(AM_HAL_WDT_MCU, &ui32Status, true);
    am_hal_wdt_interrupt_clear(AM_HAL_WDT_MCU, ui32Status);

    //
    // Send a status message and give it some time to print.
    //
    am_util_stdio_printf("Interrupt #: %d\n", g_ui8NumWatchdogInterrupts);

    //
    // Catch the first four watchdog interrupts, but let the fifth through
    // untouched.
    //
    if ( g_ui8NumWatchdogInterrupts < 4 )
    {
        //
        // Restart the watchdog.
        //
        am_hal_wdt_restart(AM_HAL_WDT_MCU);
    }
    else
    {
        am_util_stdio_printf("\nAllowing watchdog to keep running.\n");
        am_util_stdio_printf("Resetting MCU when watchdog expires...\n");
    }

    am_util_delay_ms(100);

    //
    // Increment the number of watchdog interrupts.
    //
    g_ui8NumWatchdogInterrupts++;

} // am_watchdog_isr()

void *g_ComUART;

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
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    //
    // Configure the board for low power operation.
    //
    am_bsp_low_power_init();

#if AM_BSP_NUM_LEDS > 0
    //
    // Initialize device drivers for the LEDs on the board.
    //
    am_devices_led_array_init(am_bsp_psLEDs, AM_BSP_NUM_LEDS);
#endif

    //
    // Initialize the printf interface for ITM/SWO output.
    //
    // am_hal_uart_initialize(0, &g_ComUART);
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
    // LFRC has to be turned on for this example because the watchdog only
    // runs off of the LFRC.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_LFRC_START, 0);

    //
    // Configure the watchdog.
    //
    am_hal_wdt_config(AM_HAL_WDT_MCU, &g_sWatchdogConfig);
    am_hal_wdt_interrupt_enable(AM_HAL_WDT_MCU, AM_HAL_WDT_INTERRUPT_MCU);

    //
    // Enable the interrupt for the watchdog in the NVIC.
    //
    NVIC_SetPriority(WDT_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(WDT_IRQn);
    am_hal_interrupt_master_enable();

    //
    // Enable the watchdog.
    //
    am_hal_wdt_start(AM_HAL_WDT_MCU, false);

    //
    // Loop forever.
    //
    while (1)
    {
        //
        // We are done printing. Disable debug printf messages on ITM.
        //
        //am_bsp_debug_printf_disable();

        //
        // Disable interrupts.
        //
        am_hal_interrupt_master_disable();

#if AM_BSP_NUM_LEDS > 0
        //
        // Turn OFF the indicator LED.
        //
        am_devices_led_off(am_bsp_psLEDs, 0);
#endif

        //
        // Go to deep sleep.
        //
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);

#if AM_BSP_NUM_LEDS > 0
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
