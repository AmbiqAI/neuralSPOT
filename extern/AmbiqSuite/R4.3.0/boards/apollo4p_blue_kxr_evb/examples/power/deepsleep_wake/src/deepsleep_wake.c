//*****************************************************************************
//
//! @file deepsleep_wake.c
//!
//! @brief Example that goes to deepsleep and wakes from either the RTC or GPIO.
//!
//! Purpose: This example configures the device to go into a deep sleep mode. Once in
//! deep sleep the RTC peripheral will wake the device every second, check to
//! see if 5 seconds has elapsed and then toggle LED1.
//!
//! Alternatively, it will awake when button 0 is pressed and toggle LED0.
//!
//! The example begins by printing out a banner annoucement message through
//! the UART, which is then completely disabled for the remainder of execution.
//!
//! Text is output to the UART at 115,200 BAUD, 8 bit, no parity.
//! Please note that text end-of-line is a newline (LF) character only.
//! Therefore, the UART terminal must be set to simulate a CR/LF.
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

// RTC time structure.
am_hal_rtc_time_t g_sTime;


#if AM_BSP_NUM_LEDS == 0
// Define PRINT_WAKE_LOOP for debug only, will use more power.
//#define PRINT_WAKE_LOOP

#ifdef PRINT_WAKE_LOOP
uint32_t ui32PntCnt = 0;
#endif
#endif

//*****************************************************************************
//
// GPIO ISR
//
//*****************************************************************************
void
am_gpio0_001f_isr(void)
{
    uint32_t ui32IntStatus;

    //
    // Delay for button debounce.
    //
    am_hal_delay_us(20000);

    //
    // Clear the GPIO Interrupt (write to clear).
    //
    AM_CRITICAL_BEGIN
    am_hal_gpio_interrupt_irq_status_get(GPIO0_001F_IRQn, true, &ui32IntStatus);
    am_hal_gpio_interrupt_irq_clear(GPIO0_001F_IRQn, ui32IntStatus);
    AM_CRITICAL_END

    //
    // Toggle LED 0.
    //
#if (AM_BSP_NUM_LEDS > 0)
    am_devices_led_toggle(am_bsp_psLEDs, 0);
#endif
}

//*****************************************************************************
//
// RTC ISR
//
//*****************************************************************************
static uint32_t g_RTCseconds = 0;

void
am_rtc_isr(void)
{
    //
    // Clear the RTC alarm interrupt.
    //
    am_hal_rtc_interrupt_clear(AM_HAL_RTC_INT_ALM);

    //
    // Check the desired number of seconds until LED is toggled.
    //
    if ( ++g_RTCseconds >= 5 )
    {
        //
        // Reset the seconds counter.
        //
        g_RTCseconds = 0;

#if (AM_BSP_NUM_LEDS > 0)
        //
        // Toggle LED 1.
        //
        am_devices_led_toggle(am_bsp_psLEDs, 1);
#else
#ifdef PRINT_WAKE_LOOP
        am_bsp_uart_printf_enable();
        am_util_stdio_printf("%2d", ui32PntCnt + 1);
        if ( (++ui32PntCnt % 20) == 0 )
        {
            ui32PntCnt = 0;
            am_util_stdio_printf("\nwake: ");
        }
        am_bsp_uart_printf_disable();
#endif
#endif
    }
}

//*****************************************************************************
//
// Main function.
//
//*****************************************************************************
int
main(void)
{
    am_hal_pwrctrl_mcu_memory_config_t McuMemCfg =
    {
        .eCacheCfg    = AM_HAL_PWRCTRL_CACHE_ALL,
        .bRetainCache = true,
        .eDTCMCfg     = AM_HAL_PWRCTRL_DTCM_8K,
        .eRetainDTCM  = AM_HAL_PWRCTRL_DTCM_8K,
        .bEnableNVM0  = true,
        .bRetainNVM0  = false
    };

    am_hal_pwrctrl_sram_memcfg_t SRAMMemCfg =
    {
        .eSRAMCfg           = AM_HAL_PWRCTRL_SRAM_NONE,
        .eActiveWithMCU     = AM_HAL_PWRCTRL_SRAM_NONE,
        .eActiveWithGFX     = AM_HAL_PWRCTRL_SRAM_NONE,
        .eActiveWithDISP    = AM_HAL_PWRCTRL_SRAM_NONE,
        .eActiveWithDSP     = AM_HAL_PWRCTRL_SRAM_NONE,
        .eSRAMRetain        = AM_HAL_PWRCTRL_SRAM_NONE
    };

    const am_hal_cachectrl_config_t am_hal_cachectrl_benchmark =
    {
        .bLRU                       = 0,
        .eDescript                  = AM_HAL_CACHECTRL_DESCR_1WAY_128B_512E,
        .eMode                      = AM_HAL_CACHECTRL_CONFIG_MODE_INSTR,
    };

    //
    // Set the cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_benchmark);
    am_hal_cachectrl_enable();

    //
    // Configure the board for low power operation.
    //
    am_bsp_low_power_init();

    //
    // Power down Crypto.
    //
#if defined (AM_PART_APOLLO4B)
    am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_CRYPTO_POWERDOWN, 0);
#else
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_CRYPTO);
#endif

    //
    // Disable all peripherals
    //
    am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_DIS_PERIPHS_ALL, 0);

#if RTC_CLK_SRC != XT
    //
    // Disable XTAL in deepsleep
    //
    am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_XTAL_PWDN_DEEPSLEEP, 0);
#endif

#ifdef AM_DEVICES_BLECTRLR_RESET_PIN
    //
    // For SiP packages, put the BLE Controller in reset.
    //
    am_hal_gpio_state_write(AM_DEVICES_BLECTRLR_RESET_PIN, AM_HAL_GPIO_OUTPUT_CLEAR);
    am_hal_gpio_pinconfig(AM_DEVICES_BLECTRLR_RESET_PIN,   am_hal_gpio_pincfg_output);
    am_hal_gpio_state_write(AM_DEVICES_BLECTRLR_RESET_PIN, AM_HAL_GPIO_OUTPUT_SET);
    am_hal_gpio_state_write(AM_DEVICES_BLECTRLR_RESET_PIN, AM_HAL_GPIO_OUTPUT_CLEAR);
#endif // AM_DEVICES_BLECTRLR_RESET_PIN

    //
    // Update memory configuration to minimum.
    //
    am_hal_pwrctrl_mcu_memory_config(&McuMemCfg);
    am_hal_pwrctrl_sram_config(&SRAMMemCfg);

    //
    // Enable the XT for the RTC.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_XTAL_START, 0);

    //
    // Select XT for RTC clock source
    //
    am_hal_rtc_osc_select(AM_HAL_RTC_OSC_XT);

    //
    // Enable the RTC.
    //
    am_hal_rtc_osc_enable();

    //
    // Initialize the printf interface for UART output.
    //
    am_bsp_uart_printf_enable();

    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("Deepsleep Wake Example\n");

    //
    // We are done printing.
    // Disable the UART
    //
    am_bsp_uart_printf_disable();

#if (AM_BSP_NUM_BUTTONS > 0)  &&  (AM_BSP_NUM_LEDS > 0)
    uint32_t ui32IntStatus;
    uint32_t ui32BUTTON0GpioNum = AM_BSP_GPIO_BUTTON0;

    //
    // Configure the button pin.
    //
    am_hal_gpio_pinconfig(AM_BSP_GPIO_BUTTON0, g_AM_BSP_GPIO_BUTTON0);

    //
    // Clear the GPIO Interrupt (write to clear).
    //
    AM_CRITICAL_BEGIN
    am_hal_gpio_interrupt_irq_status_get(GPIO0_001F_IRQn, false, &ui32IntStatus);
    am_hal_gpio_interrupt_irq_clear(GPIO0_001F_IRQn, ui32IntStatus);
    AM_CRITICAL_END

    //
    // Enable the GPIO/button interrupt.
    //
    am_hal_gpio_interrupt_control(AM_HAL_GPIO_INT_CHANNEL_0,
                                  AM_HAL_GPIO_INT_CTRL_INDV_ENABLE,
                                  (void *)&ui32BUTTON0GpioNum);

    //
    // Configure the LEDs.
    //
    am_devices_led_array_init(am_bsp_psLEDs, AM_BSP_NUM_LEDS);

    //
    // Turn the LEDs off, but initialize LED1 on so user will see something.
    //
    for (int ix = 0; ix < AM_BSP_NUM_LEDS; ix++)
    {
        am_devices_led_off(am_bsp_psLEDs, ix);
    }

    am_devices_led_on(am_bsp_psLEDs, 1);
#endif // defined(AM_BSP_NUM_BUTTONS)  &&  defined(AM_BSP_NUM_LEDS)

#ifdef PRINT_WAKE_LOOP
    am_bsp_uart_printf_enable();
    am_util_stdio_printf("\n\nwake: ");
    am_bsp_uart_printf_disable();
#endif

    //
    // Set the alarm repeat interval to be every second.
    //
    am_hal_rtc_alarm_interval_set(AM_HAL_RTC_ALM_RPT_SEC);

    //
    // Clear the RTC alarm interrupt.
    //
    am_hal_rtc_interrupt_clear(AM_HAL_RTC_INT_ALM);

    //
    // Enable the RTC alarm interrupt.
    //
    am_hal_rtc_interrupt_enable(AM_HAL_RTC_INT_ALM);

    //
    // Enable GPIO interrupts to the NVIC.
    //
#if (AM_BSP_NUM_BUTTONS > 0)  &&  (AM_BSP_NUM_LEDS > 0)
    NVIC_SetPriority(GPIO0_001F_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(GPIO0_001F_IRQn);
#endif
    NVIC_SetPriority(RTC_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(RTC_IRQn);

    //
    // Enable interrupts to the core.
    //
    am_hal_interrupt_master_enable();

    while (1)
    {
        //
        // Go to Deep Sleep until wakeup.
        //
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
    }
}
