//*****************************************************************************
//
//! @file rtc_print.c
//!
//! @brief Example using the internal RTC.
//!
//! This example demonstrates how to interface with the RTC and prints the
//! time over SWO.
//!
//! The example works by configuring a timer interrupt which will periodically
//! wake the core from deep sleep. After every interrupt, it prints the current
//! RTC time.
//!
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


#define TIMER_NUM       0

//
// String arrays to index Days and Months with the values returned by the RTC.
//
char *pcWeekday[] =
{
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Invalid day"
};

//
//
// The hardware treats the month as 1-12 indexed so the first entry in the month is an invalid one.
//
//
char *pcMonth[] =
{
    "Invalid entry",
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December",
    "Invalid month"
};

am_hal_rtc_time_t hal_time;
uint32_t    g_LastSecond = 0;
uint32_t    g_TestCount = 0;

//*****************************************************************************
//
// Init function for Timer A0.
//
//*****************************************************************************
void timer_init(void)
{
    am_hal_timer_config_t       TimerConfig;


    //
    // Enable the LFRC.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_LFRC_START, 0);

    //
    // Setup Timer #0
    //
    am_hal_timer_default_config_set(&TimerConfig);
    TimerConfig.eInputClock = AM_HAL_TIMER_CLOCK_HFRC_DIV16;
    TimerConfig.eFunction = AM_HAL_TIMER_FN_UPCOUNT;
    TimerConfig.ui32PatternLimit = 0;
    TimerConfig.ui32Compare1 = AM_HAL_CLKGEN_FREQ_MAX_HZ / 16;
#ifdef APOLLO4_FPGA
    // We'll wake about every second like the silicon config.
    // However, expect the RTC to run much faster on the FPGA.
    TimerConfig.ui32Compare1 = (APOLLO4_FPGA * 1000000) / 16;
#endif
    am_hal_timer_config(TIMER_NUM, &TimerConfig);
    am_hal_timer_clear(TIMER_NUM);
    am_hal_timer_interrupt_enable(AM_HAL_TIMER_MASK(TIMER_NUM, AM_HAL_TIMER_COMPARE1));

}

//*****************************************************************************
//
// Timer Interrupt Service Routine (ISR)
//
//*****************************************************************************
void
am_ctimer_isr(void)
{
    //
    // Clear TimerA0 Interrupt (write to clear).
    //
    am_hal_timer_interrupt_clear(AM_HAL_TIMER_MASK(TIMER_NUM, AM_HAL_TIMER_COMPARE1));

    am_hal_rtc_time_get(&hal_time);

    am_hal_timer_clear(TIMER_NUM);


}

//*****************************************************************************
//
// Support function:
// toVal() converts a string to an ASCII value.
//
//*****************************************************************************
int
toVal(char *pcAsciiStr)
{
    int iRetVal = 0;
    iRetVal += pcAsciiStr[1] - '0';
    iRetVal += pcAsciiStr[0] == ' ' ? 0 : (pcAsciiStr[0] - '0') * 10;
    return iRetVal;
}

//*****************************************************************************
//
// Support function:
// mthToIndex() converts a string indicating a month to an index value.
// The return value is a value 0-12, with 0-11 indicating the month given
// by the string, and 12 indicating that the string is not a month.
//
//*****************************************************************************
int
mthToIndex(char *pcMon)
{
    int idx;
    for (idx = 0; idx < 12; idx++)
    {
        if ( am_util_string_strnicmp(pcMonth[idx], pcMon, 3) == 0 )
        {
            return idx;
        }
    }
    return 12;
}

//*****************************************************************************
//
// Main
//
//*****************************************************************************
int
main(void)
{

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
    // Initialize the printf interface for ITM/SWO output.
    //
    am_bsp_itm_printf_enable();

    //
    // Enable the ITM.
    //
    am_hal_itm_enable();

    //
    // Set the RTC time for this example.
    // WARNING this will destroy any time epoch currently in the RTC.
    //
#if defined(__GNUC__)  ||  defined(__ARMCC_VERSION)  ||  defined(__IAR_SYSTEMS_ICC__)
   //
   // The RTC is initialized from the date and time strings that are
   // obtained from the compiler at compile time.
   //
   hal_time.ui32Hour = toVal(&__TIME__[0]);
   hal_time.ui32Minute = toVal(&__TIME__[3]);
   hal_time.ui32Second = toVal(&__TIME__[6]);
   hal_time.ui32Hundredths = 00;
   hal_time.ui32Weekday = am_util_time_computeDayofWeek(2000 + toVal(&__DATE__[9]), mthToIndex(&__DATE__[0]) + 1, toVal(&__DATE__[4]) );
   hal_time.ui32DayOfMonth = toVal(&__DATE__[4]);
   hal_time.ui32Month = mthToIndex(&__DATE__[0]);
   hal_time.ui32Year = toVal(&__DATE__[9]);
   hal_time.ui32Century = 0;
#else
   //
   // The RTC is initialized from an arbitrary date.
   //
    hal_time.ui32Hour = 9;
    hal_time.ui32Minute = 00;
    hal_time.ui32Second = 00;
    hal_time.ui32Hundredths = 00;
    hal_time.ui32Weekday = 2;
    hal_time.ui32DayOfMonth = 26;
    hal_time.ui32Month = 4;
    hal_time.ui32Year = 22;
    hal_time.ui32Century = 21;
#endif

    //
    // Check the validity of the input values
    //
    if (am_hal_rtc_time_set(&hal_time))
    {
        am_util_stdio_printf("Invalid Input Value");
        return 1;
    }

    //
    // TimerA0 init.
    //
    timer_init();

    //
    // Enable the timer interrupt in the NVIC.
    //
    NVIC_SetPriority(TIMER_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(TIMER_IRQn);
    am_hal_interrupt_master_enable();

    //
    // Enable the timer.
    //
    am_hal_timer_start(TIMER_NUM);

    //
    // Loop forever, printing when awakened by the timer.
    //
    while (1)
    {
        //
        // Enable debug printf messages using ITM on SWO pin
        //
        am_bsp_debug_printf_enable();

        //
        // Clear the terminal.
        //
        //am_util_stdio_terminal_clear();

        //
        // Print the banner.
        //
        am_util_stdio_printf("\n\n\n\n\n\nRTC Print Example\n");
        am_util_stdio_printf("This example was built on %s at %s.\n\n", __DATE__, __TIME__);

        //
        // Print RTC time.
        //
        am_hal_rtc_time_get(&hal_time);
        am_util_stdio_printf("\tIt is now ");
        am_util_stdio_printf("%d : ", hal_time.ui32Hour);
        am_util_stdio_printf("%02d : ", hal_time.ui32Minute);
        am_util_stdio_printf("%02d.", hal_time.ui32Second);
        am_util_stdio_printf("%02d ", hal_time.ui32Hundredths);
        am_util_stdio_printf(pcWeekday[hal_time.ui32Weekday]);
        am_util_stdio_printf(" ");
        am_util_stdio_printf(pcMonth[hal_time.ui32Month]);
        am_util_stdio_printf(" ");
        am_util_stdio_printf("%d, ", hal_time.ui32DayOfMonth);
        am_util_stdio_printf("20%02d", hal_time.ui32Year);

        //
        // We are done printing. Disable debug printf messages on ITM.
        //
        am_bsp_debug_printf_disable();

        //
        // Go to Deep Sleep and wait for a wake up.
        //
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
    }
}
