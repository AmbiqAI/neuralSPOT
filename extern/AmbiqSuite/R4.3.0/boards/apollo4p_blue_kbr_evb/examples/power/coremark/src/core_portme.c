//*****************************************************************************
//
// file core_portme.c
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
/*
    File : core_portme.c
*/
/*
    Author : Shay Gal-On, EEMBC
    Legal : TODO!
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "coremark.h"
#include "am_mcu_apollo.h"

#if VALIDATION_RUN
    volatile ee_s32 seed1_volatile = 0x3415;
    volatile ee_s32 seed2_volatile = 0x3415;
    volatile ee_s32 seed3_volatile = 0x66;
#endif
#if PERFORMANCE_RUN
    volatile ee_s32 seed1_volatile = 0x0;
    volatile ee_s32 seed2_volatile = 0x0;
    volatile ee_s32 seed3_volatile = 0x66;
#endif
#if PROFILE_RUN
    volatile ee_s32 seed1_volatile = 0x8;
    volatile ee_s32 seed2_volatile = 0x8;
    volatile ee_s32 seed3_volatile = 0x8;
#endif
    volatile ee_s32 seed4_volatile = ITERATIONS;
    volatile ee_s32 seed5_volatile = 0;
#if AM_PRINT_RESULTS
#define PRTBUFSIZE  768
    char am_prtbuf[PRTBUFSIZE];
    char *am_pcBuf;
    volatile unsigned am_bufcnt = 0;
#endif // AM_PRINT_RESULTS

/* Porting : Timing functions
    How to capture time and convert to seconds must be ported to whatever is supported by the platform.
    e.g. Read value from on board RTC, read value from cpu clock cycles performance counter etc.
    Sample implementation for standard time.h and windows.h definitions included.
*/
/* Define : TIMER_RES_DIVIDER
    Divider to trade off timer resolution and total time that can be measured.

    Use lower values to increase resolution, but make sure that overflow does not occur.
    If there are issues with the return value overflowing, increase this value.
    */
#ifdef TIME_64
#define CORETIMETYPE ee_u64
#else
#define CORETIMETYPE ee_u32
#endif
#define MYTIMEDIFF(fin, ini) ((fin) - (ini))
#define SAMPLE_TIME_IMPLEMENTATION 1
#if 1   // Defined in core_portme.h
#define NSECS_PER_SEC   AM_CORECLK_HZ
#ifdef TIME_64
#define GETMYTIME (0x00FFFFFF - am_hal_systick_count() + ((CORETIMETYPE)g_ui32SysTickWrappedTime * 0x1000000))
#else
#define GETMYTIME (0x00FFFFFF - am_hal_systick_count() + g_ui32SysTickWrappedTime)
#endif
#define START_PA_DUMP (*((volatile ee_u32 *)0x4ffff014))
#define TIMER_RES_DIVIDER 1
#define EE_TICKS_PER_SEC (NSECS_PER_SEC / TIMER_RES_DIVIDER)
#endif

/** Define Host specific (POSIX), or target specific global time variables. */
static CORETIMETYPE start_time_val, stop_time_val;

/* Function : start_time
    This function will be called right before starting the timed portion of the benchmark.

    Implementation may be capturing a system timer (as implemented in the example code)
    or zeroing some system parameters - e.g. setting the cpu clocks cycles to 0.
*/
void start_time(void)
{
    am_hal_systick_load(0x00FFFFFF);
    am_hal_systick_int_enable();
    am_hal_interrupt_master_enable();
    am_hal_systick_start();
    start_time_val = 0; // GETMYTIME could be used - but there should be very small change anyways, as we just started
    START_PA_DUMP = 0x1;
}

/* Function : stop_time
    This function will be called right after ending the timed portion of the benchmark.

    Implementation may be capturing a system timer (as implemented in the example code)
    or other system parameters - e.g. reading the current value of cpu cycles counter.
*/

void stop_time(void)
{
    am_hal_systick_stop();
    stop_time_val = GETMYTIME;
    START_PA_DUMP = 0x0;
}
/* Function : get_time
    Return an abstract "ticks" number that signifies time on the system.

    Actual value returned may be cpu cycles, milliseconds or any other value,
    as long as it can be converted to seconds by <time_in_secs>.
    This methodology is taken to accomodate any hardware or simulated platform.
    The sample implementation returns millisecs by default,
    and the resolution is controlled by <TIMER_RES_DIVIDER>
*/
CORE_TICKS get_time(void)
{
    CORE_TICKS elapsed = (CORE_TICKS)(MYTIMEDIFF(stop_time_val, start_time_val));
    return elapsed;
}

/* Function : time_in_secs
    Convert the value returned by get_time to seconds.

    The <secs_ret> type is used to accomodate systems with no support for floating point.
    Default implementation implemented by the EE_TICKS_PER_SEC macro above.
*/
secs_ret time_in_secs(CORE_TICKS ticks)
{
    secs_ret retval = ((secs_ret)ticks) / (secs_ret)EE_TICKS_PER_SEC;
    return retval;
}

ee_u32 default_num_contexts = 1;

static void apollo4_cache_memory_config(void)
{
    am_hal_pwrctrl_sram_memcfg_t SRAMMemCfg =
    {
        .eSRAMCfg           = AM_HAL_PWRCTRL_SRAM_NONE,
        .eActiveWithMCU     = AM_HAL_PWRCTRL_SRAM_NONE,
        .eActiveWithGFX     = AM_HAL_PWRCTRL_SRAM_NONE,
        .eActiveWithDISP    = AM_HAL_PWRCTRL_SRAM_NONE,
        .eActiveWithDSP     = AM_HAL_PWRCTRL_SRAM_NONE,
        .eSRAMRetain        = AM_HAL_PWRCTRL_SRAM_NONE
    };

    am_hal_pwrctrl_mcu_memory_config_t McuMemCfg =
    {
        .eCacheCfg    = AM_HAL_PWRCTRL_CACHEB0_ONLY,
        .bRetainCache = false,
        .eDTCMCfg     = AM_HAL_PWRCTRL_DTCM_8K,
        .eRetainDTCM  = AM_HAL_PWRCTRL_DTCM_8K,
        .bEnableNVM0  = true,
        .bRetainNVM0  = true
    };

    am_hal_pwrctrl_mcu_memory_config(&McuMemCfg);

    //
    // Configure the cache
    //
    CPU->CACHECFG_b.CONFIG = CPU_CACHECFG_CONFIG_W1_128B_2048E; // Direct mapped, 128-bit linesize, 2048 entries (4 SRAMs active)

    CPU->CACHECFG_b.DENABLE = 1; // 0 = disable data cache, 1 = enable data cache
    CPU->CACHECFG_b.IENABLE = 1; // 0 = disable instr cache, 1 = enable instr cache
    CPU->CACHECFG_b.ENABLE  = 1; // 0 = disable cache, 1 = enable cache

    PWRCTRL->MEMPWREN_b.PWRENCACHEB2 = PWRCTRL_MEMPWREN_PWRENCACHEB2_DIS;
    PWRCTRL->MEMPWREN_b.PWRENCACHEB0 = PWRCTRL_MEMPWREN_PWRENCACHEB0_EN;
    PWRCTRL->MEMRETCFG_b.CACHEPWDSLP = PWRCTRL_MEMRETCFG_CACHEPWDSLP_DIS;

    //
    // Configure the MRAM for low power mode
    //
    MCUCTRL->MRAMPWRCTRL_b.MRAMLPREN = 1;
    MCUCTRL->MRAMPWRCTRL_b.MRAMSLPEN = 0;
#if defined(AM_PART_APOLLO4B)
    MCUCTRL->MRAMPWRCTRL_b.MRAMPWRCTRL = 0;
#elif defined(AM_PART_APOLLO4P) // AM_PART_APOLLO4B
    MCUCTRL->MRAMPWRCTRL_b.MRAMPWRCTRL = 1;
#endif

    //
    // Disable SRAM
    //
    am_hal_pwrctrl_sram_config(&SRAMMemCfg);

    MCUCTRL->PWRSW0_b.PWRSWVDDMDSP0DYNSEL = 0;    // Bit 18
    MCUCTRL->PWRSW0_b.PWRSWVDDMDSP1DYNSEL = 0;    // Bit 21
    MCUCTRL->PWRSW0_b.PWRSWVDDMLDYNSEL    = 0;    // Bit 24
} // apollo4_cache_memory_config()

/* Function : portable_init
    Target specific initialization code
    Test for some common mistakes.
*/
void portable_init(core_portable *p, int *argc, char *argv[])
{
    if (sizeof(ee_ptr_int) != sizeof(ee_u8 *))
    {
        ee_printf("ERROR! Please define ee_ptr_int to a type that holds a pointer!\n");
    }

    if (sizeof(ee_u32) != 4)
    {
        ee_printf("ERROR! Please define ee_u32 to a 32b unsigned type!\n");
    }
    p->portable_id = 1;

#if AM_PRINT_RESULTS
    int ix;

    //
    // Initialize our printf buffer.
    //
    for ( ix = 0; ix < PRTBUFSIZE; ix++ )
    {
        am_prtbuf[ix] = 0x00;
    }
    am_pcBuf = am_prtbuf;
    am_bufcnt = 0;
#endif // AM_PRINT_RESULTS

    //
    // Initialize the printf interface for UART output.
    //
    am_bsp_uart_printf_enable();

    //
    // Clear the terminal and print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("Ambiq Micro Coremark test...\n\n");

    //
    // To minimize power during the run, disable the UART.
    //
    am_bsp_uart_printf_disable();

    //
    // Configure the board for low power.
    //
    am_bsp_low_power_init();

#if defined (AM_PART_APOLLO4B)
    //
    // Power down Crypto.
    //
    am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_CRYPTO_POWERDOWN, 0);

    //
    // Disable all peripherals
    //
    am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_DIS_PERIPHS_ALL, 0);

    //
    // Disable XTAL
    //
    am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_XTAL_PWDN_DEEPSLEEP, 0);
#elif defined (AM_PART_APOLLO4P)
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_CRYPTO);
#endif

    //
    // Set memory configuration to minimum.
    //
    apollo4_cache_memory_config();

#ifndef NOFPU
    //
    // Enable the floating point module, and configure the core for lazy
    // stacking.
    //
    am_hal_sysctrl_fpu_enable();
    am_hal_sysctrl_fpu_stacking_enable(true);
#else
    am_hal_sysctrl_fpu_disable();
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
} // portable_init()

/* Function : portable_fini
    Target specific final code
*/
void portable_fini(core_portable *p)
{
    p->portable_id = 0;

#if AM_PRINT_RESULTS
    int iCnt;
    char *pcBuf;

    //
    // Initialize the printf interface for UART output.
    //
    am_bsp_uart_printf_enable();

    //
    // Clear the terminal.
    //
    am_util_stdio_terminal_clear();

    //
    // Print the banner.
    //
    am_util_stdio_printf("\nAmbiq Micro Coremark run finished!\n\n");

    //
    // Now, let's go parse the buffer and print it out!
    //
    pcBuf = am_prtbuf;
    iCnt = 0;
    while ( (*pcBuf != 0x00)  &&  (iCnt<PRTBUFSIZE) )
    {
        am_util_stdio_printf(pcBuf);
        while ( *pcBuf != 0x00 )
        {
            pcBuf++;
            iCnt++;
        }
        iCnt++;     // Account for the NULL terminator
        pcBuf++;    // Point after the NULL terminator to the next string
    }

    //
    // Disable the UART.
    //
    am_bsp_uart_printf_disable();
#endif // AM_PRINT_RESULTS

#if AM_BSP_NUM_LEDS
void set_leds(uint32_t mask, uint32_t delay);   // Function prototype
    //
    // Now for the grand finale, do a little something with the LEDs.
    //
    am_devices_led_array_init(am_bsp_psLEDs, AM_BSP_NUM_LEDS);

    uint32_t ux, umask, umod;
    for (ux = 0; ux < (AM_BSP_NUM_LEDS * 4); ux++ )
    {
        umod = (ux % (AM_BSP_NUM_LEDS * 2));
        if ( umod < AM_BSP_NUM_LEDS )
        {
            // Walk up the LEDs sequentially.
            umask = 1 << umod;
        }
        else
        {
            // Go the other direction.
            umask = (1 << (AM_BSP_NUM_LEDS - 1)) >> (umod - AM_BSP_NUM_LEDS);
        }
        set_leds(umask, 200);
    }

    //
    // Flash the LED array 3 times.
    //
    for (ux = 0; ux < 3; ux++ )
    {
        set_leds((1 << AM_BSP_NUM_LEDS) - 1, 300);
        set_leds(0x00, 300);
    }
#endif // AM_BSP_NUM_LEDS
} // portable_fini()

#if AM_BSP_NUM_LEDS
void set_leds(uint32_t mask, uint32_t delay)
{
    am_devices_led_array_out(am_bsp_psLEDs, AM_BSP_NUM_LEDS, mask);
    am_util_delay_ms(delay);
}
#endif // AM_BSP_NUM_LEDS

#if AM_PRINT_RESULTS
int am_sprintf(char *pcFmt, ...)
{
    uint32_t ui32NumChars;
    int iRet = 0;

    va_list pArgs;

    if ( am_bufcnt < PRTBUFSIZE )
    {
        va_start(pArgs, pcFmt);
        ui32NumChars = am_util_stdio_vsprintf(am_pcBuf, pcFmt, pArgs);
        va_end(pArgs);

        if ( (am_bufcnt + ui32NumChars) >= PRTBUFSIZE )
        {
            //
            // This string is 40 chars (with the NULL terminator)
            //
            am_util_stdio_sprintf(&am_prtbuf[PRTBUFSIZE - (40 + 1)], "BUFFER OVERFLOWED! Increase PRTBUFSIZE\n");
            am_prtbuf[PRTBUFSIZE - 1] = 0x00;   // Double terminate the buffer
            am_pcBuf = &am_prtbuf[PRTBUFSIZE];  // Don't allow any further printing
            am_bufcnt = PRTBUFSIZE;             //  "
        }
        else
        {
            am_pcBuf += ui32NumChars;
            am_pcBuf++;                 // Skip NULL terminator
            am_bufcnt += ui32NumChars;
            am_bufcnt++;                // Include NULL terminator
            iRet = ui32NumChars;
        }
    } // if (am_bufcnt)

    return iRet;

} // am_sprintf()
#endif // AM_PRINT_RESULTS
