//*****************************************************************************
//
//! @file ctimer_pwm_output.c
//!
//! @brief Breathing LED example.
//!
//! Purpose: This example shows one way to vary the brightness of an LED using a timer
//! in PWM mode.  The timer can be clocked from either the XTAL (default) or
//! the LFRC, selectable by a define, USE_XTAL.
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
// Macros
//
//*****************************************************************************
//
// The default is to use the XTAL as the clock source.
// Select XTAL or LFRC via the USE_XTAL define.
//
#define USE_XTAL    1
#if USE_XTAL
#define BC_CLKSRC   "XTAL"
#define PWM_CLK     AM_HAL_CTIMER_XT_2_048KHZ
#else
// Note that LFRC runs 4x slower than XTAL and will look more choppy.
#define BC_CLKSRC   "LFRC"
#define PWM_CLK     AM_HAL_CTIMER_LFRC_512HZ
#endif

//*****************************************************************************
//
// LED brightness profile
//
// A simple Python script for generating the sine-wave values.
// import math
// iperiod = 64
// halfpi = math.pi / 2.0
// period = float(iperiod)
// ampl = period
// period = period / 2.0
// a = halfpi / period
// for ix in range(0, iperiod):
//     print(str(ix) + ": " + str(int(round(math.sin(a) * ampl - 1))))
//     a = a + (halfpi / period)
//
//*****************************************************************************
volatile uint32_t g_ui32Index = 0;

#if USE_XTAL
#define PERIOD  64
const uint8_t g_pui8Brightness[PERIOD] =
{
     2,  5,  8, 11, 15, 18, 21, 23,
    26, 29, 32, 35, 37, 40, 42, 44,
    46, 48, 50, 52, 54, 55, 57, 58,
    59, 60, 61, 62, 62, 63, 63, 63,
    63, 63, 62, 62, 61, 60, 59, 58,
    57, 55, 54, 52, 50, 48, 46, 44,
    42, 40, 37, 35, 32, 29, 26, 23,
    21, 18, 15, 11,  8,  5,  2,  1
};
#else // Use LFRC
#define PERIOD  16
const uint8_t g_pui8Brightness[PERIOD] =
{
     2,  5,  8, 10, 12, 14, 15, 15,
    15, 14, 12, 10,  8,  5,  2,  1
};
#endif

//*****************************************************************************
//
// Timer Interrupt Serive Routine (ISR)
//
//*****************************************************************************
void
am_ctimer_isr(void)
{
    uint32_t ui32OnTime;

    //
    // Clear the interrupt that got us here.
    //
    am_hal_ctimer_int_clear(AM_BSP_PWM_LED_TIMER_INT);

    ui32OnTime = g_pui8Brightness[g_ui32Index];

    if ( !(g_ui32Index & 1) )
    {
        // This is a CMPR0 interrupt.
        // The CMPR2 interrupt will come later, but while we're here go ahead
        // and update the CMPR0 period.
        //
        am_hal_ctimer_period_set(AM_BSP_PWM_LED_TIMER, AM_BSP_PWM_LED_TIMER_SEG,
                                 PERIOD, ui32OnTime);
    }
    else
    {
        //
        // This is a CMPR2 interrupt.  Update the CMPR2 period.
        //
        am_hal_ctimer_aux_period_set(AM_BSP_PWM_LED_TIMER, AM_BSP_PWM_LED_TIMER_SEG,
                                     PERIOD, ui32OnTime);
    }

    //
    // Set up the LED duty cycle for the next pulse.
    //
    g_ui32Index = (g_ui32Index + 1) % PERIOD;

} // am_ctimer_isr()

//*****************************************************************************
//
// Main function.
//
//*****************************************************************************
int
main(void)
{
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

    //
    // Among other things, am_bsp_low_power_init() stops the XT oscillator,
    // which is needed for this example.
    //
#if USE_XTAL
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_XTAL_START, 0);
#else
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_LFRC_START, 0);
#endif

    //
    // Initialize the printf interface for ITM/SWO output.
    //
    am_bsp_itm_printf_enable();

    //
    // Clear the terminal and print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("CTimer PWM Output example\n");
    am_util_stdio_printf("Clock source is:" BC_CLKSRC ".\n");
    am_util_stdio_printf("LED on pin: %d\n\n", AM_BSP_PIN_PWM_LED);

    //
    // We are done printing. Disable debug printf messages on ITM.
    //
    am_bsp_debug_printf_disable();

    //
    // Configure the output pin.
    //
    am_hal_ctimer_output_config(AM_BSP_PWM_LED_TIMER,
                                AM_BSP_PWM_LED_TIMER_SEG,
                                AM_BSP_PIN_PWM_LED,
                                AM_HAL_CTIMER_OUTPUT_NORMAL,
                                AM_HAL_GPIO_PIN_DRIVESTRENGTH_12MA);

    //
    // Configure a timer to drive the LED.
    //
    am_hal_ctimer_config_single(AM_BSP_PWM_LED_TIMER,               // ui32TimerNumber
                                AM_BSP_PWM_LED_TIMER_SEG,           // ui32TimerSegment
                                (AM_HAL_CTIMER_FN_PWM_REPEAT    |   // ui32ConfigVal
                                 PWM_CLK                        |
                                 AM_HAL_CTIMER_INT_ENABLE) );

    //
    // Set up initial timer periods.
    //
    am_hal_ctimer_period_set(AM_BSP_PWM_LED_TIMER,
                             AM_BSP_PWM_LED_TIMER_SEG, PERIOD, 1);
    am_hal_ctimer_aux_period_set(AM_BSP_PWM_LED_TIMER,
                                 AM_BSP_PWM_LED_TIMER_SEG, PERIOD, 1);

    //
    // Enable interrupts for the Timer we are using on this board.
    //
    am_hal_ctimer_int_enable(AM_BSP_PWM_LED_TIMER_INT);
    NVIC_EnableIRQ(CTIMER_IRQn);
    am_hal_interrupt_master_enable();

    //
    // Start the timer.
    //
    am_hal_ctimer_start(AM_BSP_PWM_LED_TIMER, AM_BSP_PWM_LED_TIMER_SEG);

    //
    // Sleep forever while waiting for an interrupt.
    //
    while (1)
    {
        //
        // Go to Deep Sleep.
        //
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
    }

} // main()
