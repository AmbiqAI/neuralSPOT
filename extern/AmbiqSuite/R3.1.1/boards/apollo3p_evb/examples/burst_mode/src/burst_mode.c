//*****************************************************************************
//
//! @file turbomode.c
//!
//! @brief Example demonstrates the usage of TurboSPOT (aka burst mode) HAL.
//!
//! Purpose: This example shows how to detect if TurboSPOT Mode is available.
//! If so, it sets the Apollo3 into Normal (48MHz) mode, then times a
//! calculation of prime numbers, displaying the elapsed time.  Next, it
//! switches the Apollo3 into Burst mode, performs the same calculation, then
//! displays the elapsed time, which should be roughly 50% of Normal mode.
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

#define NUM_OF_PRIMES_IN 10000
#define EXP_PRIMES 9592


//*****************************************************************************
//
// Init function for the STimer.
//
//*****************************************************************************
void
stimer_init(void)
{
    //
    // Configure the STIMER and run
    //
    am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
    am_hal_stimer_config(AM_HAL_STIMER_XTAL_32KHZ);
}

//*****************************************************************************
//
//  Purpose:
//
//    prime_number() returns the number of primes between 1 and N.
//
//  Discussion:
//
//    A naive algorithm is used.
//
//    Mathematica can return the number of primes less than or equal to N
//    by the command PrimePi[N].
//
//                N  PRIME_NUMBER
//
//                1           0
//               10           4
//              100          25
//            1,000         168
//           10,000       1,229
//          100,000       9,592
//        1,000,000      78,498
//       10,000,000     664,579
//      100,000,000   5,761,455
//    1,000,000,000  50,847,534
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    23 April 2009
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the maximum number to check.
//
//    Output, int PRIME_NUMBER, the number of prime numbers up to N.
//
//*****************************************************************************
uint32_t
prime_number(int32_t i32n)
{
    uint32_t ui32Total, ui32Prime;
    int32_t ix, jx;

    ui32Total = 0;

    for ( ix = 2; ix <= i32n; ix++ )
    {
        ui32Prime = 1;
        for ( jx = 2; jx < ix; jx++ )
        {
            if ( (ix % jx) == 0 )
            {
                ui32Prime = 0;
                break;
            }
        }
        ui32Total += ui32Prime;
    }

    return ui32Total;
}

//*****************************************************************************
//
// Test cases for the main program to run.
//
//*****************************************************************************
int
main(void)
{
    uint32_t                      ui32StartTime, ui32StopTime;
    uint32_t                      ui32BurstModeDelta, ui32NormalModeDelta;
    am_hal_burst_avail_e          eBurstModeAvailable;
    am_hal_burst_mode_e           eBurstMode;

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
    // Initialize the printf interface for ITM/SWO output.
    //
    am_bsp_itm_printf_enable();

    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("%s TurboSPOT Example\n\n", AM_HAL_DEVICE_NAME);

    //
    // Initialize the STimer.
    //
    stimer_init();

    //
    // Check that the TurboSPOT Feature is available.
    //
    if ( am_hal_burst_mode_initialize(&eBurstModeAvailable) == AM_HAL_STATUS_SUCCESS )
    {
        if ( eBurstModeAvailable == AM_HAL_BURST_AVAIL )
        {
            am_util_stdio_printf("TurboSPOT mode is Available\n");

            //
            // It's available, make sure we are in "Normal" mode.
            //
            if ( am_hal_burst_mode_disable(&eBurstMode) == AM_HAL_STATUS_SUCCESS )
            {
                if ( eBurstMode == AM_HAL_NORMAL_MODE )
                {
                    am_util_stdio_printf("Operating in Normal mode (%dMHz)\n", AM_HAL_CLKGEN_FREQ_MAX_MHZ);
                }
            }
            else
            {
                am_util_stdio_printf("Failed to Disable TurboSPOT mode operation\n");
            }
        }
        else
        {
            am_util_stdio_printf("TurboSPOT mode is Not Available\n");
        }
    }
    else
    {
        am_util_stdio_printf("Failed to Initialize for TurboSPOT mode operation\n");
    }

    //
    // Execute the example algorithm.
    //
    am_util_stdio_printf("\nStarted calculating primes in Normal Mode\n");

    //
    // Capture the start time.
    //
    ui32StartTime = am_hal_stimer_counter_get();

    am_util_stdio_printf("\nNumber of Primes: %d\n", prime_number(NUM_OF_PRIMES_IN));

    //
    // Stop the timer and calculate the elapsed time.
    //
    ui32StopTime = am_hal_stimer_counter_get();

    //
    // Calculate the TurboSPOT mode delta time.
    //
    ui32NormalModeDelta = ui32StopTime - ui32StartTime;
    am_util_stdio_printf("Normal Mode Delta: %d\n", ui32NormalModeDelta);

    //
    // Put the MCU into TurboSPOT mode.
    //
    if ( eBurstModeAvailable == AM_HAL_BURST_AVAIL )
    {
        if ( am_hal_burst_mode_enable(&eBurstMode) == AM_HAL_STATUS_SUCCESS )
        {
            if ( eBurstMode == AM_HAL_BURST_MODE )
            {
                am_util_stdio_printf("%s operating in TurboSPOT mode (%dMHz)\n",
                                     AM_HAL_DEVICE_NAME, AM_HAL_CLKGEN_FREQ_MAX_MHZ * 2);
            }
        }
        else
        {
            am_util_stdio_printf("Failed to Enable TurboSPOT mode operation\n");
        }
    }

    //
    // Execute the example algorithm.
    //
    if ( eBurstModeAvailable == AM_HAL_BURST_AVAIL )
    {
        am_util_stdio_printf("\nStarted calculating primes in TurboSPOT mode\n");
    }
    else
    {
        am_util_stdio_printf("\nTurboSPOT mode not available, started calculating primes in Normal mode.\n");
    }

    //
    // Capture the start time.
    //
    ui32StartTime = am_hal_stimer_counter_get();

    am_util_stdio_printf("\nNumber of Primes: %d\n", prime_number(NUM_OF_PRIMES_IN));

    //
    // Stop the timer and calculate the elapsed time.
    //
    ui32StopTime = am_hal_stimer_counter_get();

    //
    // Disable TurboSPOT Mode.
    //
    if ( eBurstModeAvailable == AM_HAL_BURST_AVAIL )
    {
        if ( am_hal_burst_mode_disable(&eBurstMode) == AM_HAL_STATUS_SUCCESS )
        {
            if ( AM_HAL_NORMAL_MODE != eBurstMode )
            {
                am_util_stdio_printf("%s failed to return to Normal Mode (%dMHz)\n",
                                     AM_HAL_DEVICE_NAME, AM_HAL_CLKGEN_FREQ_MAX_MHZ);
            }
        }
        else
        {
            am_util_stdio_printf("Failed to Disable TurboSPOT mode operation\n");
        }
    }

    //
    // Calculate the TurboSPOT mode delta time.
    //
    ui32BurstModeDelta = ui32StopTime - ui32StartTime;
    am_util_stdio_printf("TurboSPOT mode Delta: %d\n", ui32BurstModeDelta);

    //
    // End of example.
    //
    am_util_stdio_printf("\n");
    am_util_stdio_printf("TurboSPOT mode Example Complete.");

    //
    // Loop forever while sleeping.
    //
    while (1)
    {
        //
        // Go to Deep Sleep.
        //
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
    }
}




