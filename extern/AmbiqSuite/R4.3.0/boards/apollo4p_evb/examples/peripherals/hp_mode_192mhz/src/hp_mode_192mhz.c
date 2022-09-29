//*****************************************************************************
//
//! @file hp_mode_192mhz.c
//!
//! @brief Example demonstrates the usage of High Performance Mode(192MHz) HAL.
//!
//! Purpose: This example sets the Apollo4 into High Power Mode(192MHz), then
//! times a calculation of prime numbers, displaying the elapsed time.
//! Next, it switches the Apollo4 into Low Performance Mode(96MHz), performs
//! the same calculation, then displays the elapsed time, which should be
//! roughly double the time of Low Power Mode.
//!
//! The entire test takes around 30s to run on Apollo4.
//!
//! Printing takes place over the ITM at 1M Baud.
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
// Macro definitions
//
//*****************************************************************************
//#define NUM_OF_PRIMES_IN 10000
//#define EXP_PRIMES       1229   //  Expected with NUM_OF_PRIMES_IN = 10000
#define NUM_OF_PRIMES_IN (10000 * 5)
#define EXP_PRIMES       5133   // Expected with NUM_OF_PRIMES_IN = 50000

//*****************************************************************************
//
// Globals
//
//*****************************************************************************
uint32_t
timer_init(uint32_t ui32TimerNum)
{
    am_hal_timer_config_t       TimerConfig;
    uint32_t ui32Status         = AM_HAL_STATUS_SUCCESS;

    //
    // Set the timer configuration
    // The default timer configuration is HFRC_DIV16, EDGE, compares=0, no trig.
    //
    am_hal_timer_default_config_set(&TimerConfig);
    ui32Status = am_hal_timer_config(ui32TimerNum, &TimerConfig);
    if ( ui32Status != AM_HAL_STATUS_SUCCESS )
    {
        am_util_stdio_printf("Failed to configure TIMER%d, return value=%d\n", ui32TimerNum, ui32Status);
        return ui32Status;
    }

    //
    // Stop and clear the timer.
    //
    am_hal_timer_clear(ui32TimerNum);
    am_hal_timer_stop(ui32TimerNum);

    //
    // Timer interrupt not needed for this purpose.
    //

    return ui32Status;

} // timer_init()

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
// Main function.
//
//*****************************************************************************
int
main(void)
{
    uint32_t ui32retval, ui32Result, ui32TimerCount, ui32TimerNum, ui32Errcnt;

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
    // Clear the terminal and print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("Apollo4 192MHz High Performance Mode Example\n\n");

    //
    // Select timer to use.
    //
    ui32TimerNum = 5;

    ui32Errcnt = 0;
    am_util_stdio_printf("Set High Performance (HP) Mode.");
    ui32retval = am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE);
    if ( ui32retval != AM_HAL_STATUS_SUCCESS )
    {
        am_util_stdio_printf("Error with am_hal_pwrctrl_mcu_mode_select(), returned %d.\n", ui32retval);
        while(1);
    }

    am_util_stdio_printf(" Start prime calculations...\n");

    //
    // Initialize the timer.
    //
    timer_init(ui32TimerNum);

    //
    // Start the timer as close to the prime calculations as possible.
    //
    am_hal_timer_clear(ui32TimerNum);   // The clear function also starts the timer

    //
    // Run prime in HP mode.
    //
    ui32Result = prime_number(NUM_OF_PRIMES_IN);
    am_hal_timer_stop(ui32TimerNum);

    ui32TimerCount = am_hal_timer_read(ui32TimerNum);
    am_util_stdio_printf("  Computations took %d.%03ds (ticks=%d).\n",
                        ui32TimerCount / 6000000,
                        (ui32TimerCount % 6000000) / 10000, // Limit to 3 decimals
                        ui32TimerCount);

    if ( ui32Result == EXP_PRIMES )
    {
        am_util_stdio_printf("  Pass - Number of primes for %d is %d.\n", NUM_OF_PRIMES_IN, ui32Result);
    }
    else
    {
        am_util_stdio_printf("  ERROR - Invalid result. Expected %d, got %d.\n", EXP_PRIMES, ui32Result);
        ui32Errcnt++;
    }

    am_util_stdio_printf("\n");
    am_util_stdio_printf("Set Low Power (LP) Mode.");
    ui32retval = am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_LOW_POWER);
    if ( ui32retval != AM_HAL_STATUS_SUCCESS )
    {

        am_util_stdio_printf("Error with am_hal_pwrctrl_mcu_mode_select(), returned %d.\n", ui32retval);
        while(2);
    }

    am_util_stdio_printf(" Start prime calculations...\n");

    //
    // Start the timer as close to the prime calculations as possible.
    //
    am_hal_timer_clear(ui32TimerNum);   // The clear function also starts the timer

    //
    // Run prime in LP mode.
    //
    ui32Result = prime_number(NUM_OF_PRIMES_IN);
    am_hal_timer_stop(ui32TimerNum);

    ui32TimerCount = am_hal_timer_read(ui32TimerNum);
    am_util_stdio_printf("  Computations took %d.%03ds (ticks=%d).\n",
                        ui32TimerCount / 6000000,
                        (ui32TimerCount % 6000000) / 10000, // Limit to 3 decimals
                        ui32TimerCount);

    if ( ui32Result == EXP_PRIMES )
    {
        am_util_stdio_printf("  Pass - Number of primes for %d is %d.\n", NUM_OF_PRIMES_IN, ui32Result);
    }
    else
    {
        am_util_stdio_printf("  ERROR - Invalid result. Expected %d, got %d.\n", EXP_PRIMES, ui32Result);
        ui32Errcnt++;
    }

    //
    // End of example.
    //
    am_util_stdio_printf("\n");
    if ( ui32Errcnt == 0 )
    {
        am_util_stdio_printf("HP and LP tests PASSED.\n");
    }
    else
    {
        am_util_stdio_printf("Failure detected during testing.\n");
    }
    am_util_stdio_printf("192MHz High Performance Mode Example Complete.\n");

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


