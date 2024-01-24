//*****************************************************************************
//
//! @file ctimer_multi_bidirectional_stepper.c
//!
//! @brief CTimer multiple bidirectional stepper motors Example,
//!
//! Purpose: This example demonstrates how to create arbitrary patterns on multiple
//! CTimers.  TMR6 A is used to create base timing for the patterns.  TMR0 B
//! TMR1 A and TMR1 B are configured to dual edge trigger on TMR6 with separate
//! counting patterns. All timers are configured to run and then synchronized
//! off of the global timer enable.
//!
//! Printing takes place over the ITM at 1M Baud.
//!
//! Additional Information:
//! The patterns are output as follows:
//!     Pin12 - TMR6 A dual edge trigger pulse
//!     Pin13 - TMR0 B positive pattern
//!     Pin18   TMR1 A negative pattern
//!     Pin19   TMR1 B inactive pattern
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

#define TRIG_GPIO      12
#define POSITIVE_GPIO  13
#define NEGATIVE_GPIO  18
#define INACTIVE_GPIO  19

//*****************************************************************************
//
// Stepper Pattern helper functions.
//
//*****************************************************************************
void
initialize_trigger_counter(void)
{
    //
    // Set up timer A6.
    //
    am_hal_ctimer_clear(6, AM_HAL_CTIMER_TIMERA);
    am_hal_ctimer_config_single(6, AM_HAL_CTIMER_TIMERA,
                              (AM_HAL_CTIMER_FN_PWM_REPEAT    |
                               AM_HAL_CTIMER_LFRC_512HZ));

    am_hal_ctimer_config_trigger(6, AM_HAL_CTIMER_TIMERA, 1 << CTIMER_AUX6_TMRA6EN23_Pos);

    //
    // Set the A6 Timer period
    //
    am_hal_ctimer_period_set(6, AM_HAL_CTIMER_TIMERA, 1000, 500);

    //
    // Configure timer A6 OUT2 on pin #12.
    //
    am_hal_ctimer_output_config(6, AM_HAL_CTIMER_TIMERA, TRIG_GPIO,
                              AM_HAL_CTIMER_OUTPUT_SECONDARY,
                              AM_HAL_GPIO_PIN_DRIVESTRENGTH_12MA);

    //
    // Start the timer.
    //
    am_hal_ctimer_start(6, AM_HAL_CTIMER_TIMERA);
}

void
initialize_pattern_counter(uint32_t ui32TimerNumber,
                           uint32_t ui32TimerSegment,
                           uint64_t ui64Pattern,
                           uint32_t ui32PatternLen,
                           uint32_t ui32Trigger,
                           uint32_t ui32OutputPin,
                           uint32_t ui32PatternClock,
                           uint32_t ui32Invert)
{
    //
    // Set up timer.
    //
    am_hal_ctimer_clear(ui32TimerNumber, ui32TimerSegment);

    am_hal_ctimer_config_single(ui32TimerNumber, ui32TimerSegment,
                              (AM_HAL_CTIMER_FN_PTN_ONCE    |
                               ui32PatternClock) );

    //
    // Set the pattern in the CMPR registers.
    //
    am_hal_ctimer_compare_set(ui32TimerNumber, ui32TimerSegment, 0,
                            (uint32_t)(ui64Pattern & 0xFFFF));
    am_hal_ctimer_compare_set(ui32TimerNumber, ui32TimerSegment, 1,
                            (uint32_t)((ui64Pattern >> 16) & 0xFFFF));
    //
    // Set the timer trigger and pattern length.
    //
    am_hal_ctimer_config_trigger(ui32TimerNumber, ui32TimerSegment,
                               ( (ui32PatternLen << CTIMER_AUX0_TMRA0LMT_Pos) |
                                 ( ui32Trigger << CTIMER_AUX0_TMRA0TRIG_Pos) |
                                 (ui32Invert << CTIMER_AUX0_TMRA0TINV_Pos)) );

    //
    // Configure timer output pin.
    //
    am_hal_ctimer_output_config(ui32TimerNumber, ui32TimerSegment, ui32OutputPin,
                              AM_HAL_CTIMER_OUTPUT_NORMAL,
                              AM_HAL_GPIO_PIN_DRIVESTRENGTH_12MA);

    //
    // Start the timer.
    //
    am_hal_ctimer_start(ui32TimerNumber, ui32TimerSegment);
}

void
global_disable(void)
{
    CTIMER->GLOBEN = 0x0;
}

void
global_enable(void)
{
    CTIMER->GLOBEN = 0xffff;
}
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
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_LFRC_START, 0);

    //
    // Initialize the printf interface for ITM/SWO output.
    //
    am_bsp_itm_printf_enable();

    //
    // Clear the terminal and print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("CTimer multiple bidirectional stepper motor example\n");

    //
    // We are done printing. Disable debug printf messages on ITM.
    //
    am_bsp_debug_printf_disable();

    //
    // Disable all the counters.
    //
    global_disable();

    //
    // Set up the base counter controlling the pattern overall period (1Hz).
    //
    initialize_trigger_counter();

    //
    // Set B0 CTimer with 1st pattern
    //
    initialize_pattern_counter(0, AM_HAL_CTIMER_TIMERB, 0x333,
                               31, CTIMER_AUX0_TMRB0TRIG_A6OUT2DUAL, POSITIVE_GPIO,
                               AM_HAL_CTIMER_LFRC_32HZ, 1);

    //
    // Set A1 CTimer with 2nd pattern.
    //
    initialize_pattern_counter(1, AM_HAL_CTIMER_TIMERA, 0x33300,
                               31, CTIMER_AUX1_TMRA1TRIG_A6OUT2DUAL, NEGATIVE_GPIO,
                               AM_HAL_CTIMER_LFRC_32HZ, 1);

    //
    // Set B1 CTimer with 3rd pattern.
    //
    initialize_pattern_counter(1, AM_HAL_CTIMER_TIMERB, 0,
                               31, CTIMER_AUX1_TMRB1TRIG_A6OUT2DUAL, INACTIVE_GPIO,
                               AM_HAL_CTIMER_LFRC_32HZ, 1);

    //
    // Enable all the counters.
    //
    global_enable();

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

