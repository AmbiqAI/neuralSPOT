/*
    File : core_portme.c
*/
/*
    Author : Shay Gal-On, EEMBC
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "coremark.h"
#include "am_mcu_apollo.h"

//
// This variable stores the value of the loop iteration
//
//
volatile e_loop_iter g_eAmbClkSpd;

#if VALIDATION_RUN
    volatile ee_s32 seed1_volatile=0x3415;
    volatile ee_s32 seed2_volatile=0x3415;
    volatile ee_s32 seed3_volatile=0x66;
#endif
#if PERFORMANCE_RUN
    volatile ee_s32 seed1_volatile=0x0;
    volatile ee_s32 seed2_volatile=0x0;
    volatile ee_s32 seed3_volatile=0x66;
#endif
#if PROFILE_RUN
    volatile ee_s32 seed1_volatile=0x8;
    volatile ee_s32 seed2_volatile=0x8;
    volatile ee_s32 seed3_volatile=0x8;
#endif
    volatile ee_s32 seed4_volatile=ITERATIONS;
    volatile ee_s32 seed5_volatile=0;
#if AM_PRINT_RESULTS
#define PRTBUFSIZE  1024
    char am_prtbuf[PRTBUFSIZE];
    char *am_pcBuf;
    volatile unsigned am_bufcnt=0;
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
#define MYTIMEDIFF(fin,ini) ((fin)-(ini))
#define SAMPLE_TIME_IMPLEMENTATION 1
#if 1   // Defined in core_portme.h
#define NSECS_PER_SEC          AM_CORECLK_HZ
#define NSECS_PER_SEC_96MHZ   (AM_CORECLK_HZ * 2)
#ifdef TIME_64
#define GETMYTIME (0x00FFFFFF - am_hal_systick_count() + ((CORETIMETYPE)g_ui32SysTickWrappedTime*0x1000000))
#else
#define GETMYTIME (0x00FFFFFF - am_hal_systick_count() + g_ui32SysTickWrappedTime)
#endif
#define START_PA_DUMP (*((volatile ee_u32 *)0x4ffff014))
#define TIMER_RES_DIVIDER 1
#define EE_TICKS_PER_SEC           (NSECS_PER_SEC / TIMER_RES_DIVIDER)
#define EE_TICKS_PER_SEC_96MHZ    (NSECS_PER_SEC_96MHZ / TIMER_RES_DIVIDER)
#endif

/** Define Host specific (POSIX), or target specific global time variables. */
static CORETIMETYPE start_time_val, stop_time_val;

/* Function : start_time
    This function will be called right before starting the timed portion of the benchmark.

    Implementation may be capturing a system timer (as implemented in the example code)
    or zeroing some system parameters - e.g. setting the cpu clocks cycles to 0.
*/
void start_time(void) {
    am_hal_systick_stop();
    am_hal_systick_reset();
    am_hal_systick_load(0x00FFFFFF);
    am_hal_systick_int_enable();
    am_hal_interrupt_master_enable();
    am_hal_systick_start();
    start_time_val = GETMYTIME; // GETMYTIME could be used - but there should be very small change anyways, as we just started
    START_PA_DUMP = 0x1;
}
/* Function : stop_time
    This function will be called right after ending the timed portion of the benchmark.

    Implementation may be capturing a system timer (as implemented in the example code)
    or other system parameters - e.g. reading the current value of cpu cycles counter.
*/
void stop_time(void) {
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
CORE_TICKS get_time(void) {
    CORE_TICKS elapsed=(CORE_TICKS)(MYTIMEDIFF(stop_time_val, start_time_val));
    return elapsed;
}
/* Function : time_in_secs
    Convert the value returned by get_time to seconds.

    The <secs_ret> type is used to accomodate systems with no support for floating point.
    Default implementation implemented by the EE_TICKS_PER_SEC macro above.
*/
secs_ret time_in_secs(CORE_TICKS ticks)
{
    secs_ret retval = 0;

    if (g_eAmbClkSpd == AM_CLK_SPEED_96MHz)
    {
        retval = ((secs_ret)ticks) / (secs_ret)EE_TICKS_PER_SEC_96MHZ;
    }
    else
    {
        retval = ((secs_ret)ticks) / (secs_ret)EE_TICKS_PER_SEC;
    }

    return retval;
}

ee_u32 default_num_contexts=1;

/* Function : portable_init
    Target specific initialization code
    Test for some common mistakes.
*/

am_hal_burst_mode_e eBurstMode;

void portable_init(core_portable *p, int *argc, char *argv[])
{
#if AM_PRINT_RESULTS
    int i;
#endif // AM_PRINT_RESULTS

    if (sizeof(ee_ptr_int) != sizeof(ee_u8 *)) {
        ee_printf("ERROR! Please define ee_ptr_int to a type that holds a pointer!\n");
    }
    if (sizeof(ee_u32) != 4) {
        ee_printf("ERROR! Please define ee_u32 to a 32b unsigned type!\n");
    }
    p->portable_id=1;

#if AM_PRINT_RESULTS
    // Initialize our printf buffer.
    for(i = 0; i < PRTBUFSIZE; i++)
    {
        am_prtbuf[i] = 0x00;
    }
    am_pcBuf = am_prtbuf;
    am_bufcnt = 0;
#endif // AM_PRINT_RESULTS

    //
    // Set the system clock to maximum frequency.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);

    //
    // Set the default cache configuration
    //
const am_hal_cachectrl_config_t am_hal_cachectrl_benchmark =
{
    .bLRU                       = 0,
    .eDescript                  = AM_HAL_CACHECTRL_DESCR_1WAY_128B_512E,
    .eMode                      = AM_HAL_CACHECTRL_CONFIG_MODE_INSTR,
};
    am_hal_cachectrl_config(&am_hal_cachectrl_benchmark);
    am_hal_cachectrl_enable();

    //
    // Enable the cache for LPMMODE and aggressive settings.
    // This must be done after am_hal_cachectrl_enable().
    //
    if ( am_hal_cachectrl_control(AM_HAL_CACHECTRL_CONTROL_LPMMODE_AGGRESSIVE, 0) )
    {
        am_util_stdio_printf("Failed to set cache into LPMMODE_AGGRESSIVE.\n");
        while(1);
    }

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

#if (defined(AM_PART_APOLLO3) || defined(AM_PART_APOLLO3P))
    //
    // Configure the board for low power.
    //
    am_bsp_low_power_init();

    //
    // Initialize the printf interface for UART output.
    //
    am_bsp_uart_printf_enable();

    if (g_eAmbClkSpd == AM_CLK_SPEED_96MHz)
    {
        am_util_stdio_printf("\n\n\n\nAmbiq Micro Coremark 96MHz test...\n\n");
    }
    else
    {
        am_util_stdio_printf("\n\n\n\nAmbiq Micro Coremark 48MHz test...\n\n");
    }

    //
    // Turn off unneeded flash
    //
    if ( am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEM_FLASH_MIN) )
    {
        am_util_stdio_printf("Failed to reconfigure Flash for minimum.\n");
        while(1);
    }

    //
    // Turn off unneeded SRAM
    //
    if ( am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEM_SRAM_8K_DTCM) )
    {
      am_util_stdio_printf("Failed to reconfigure SRAM.\n");
      while(1);
    }

    am_hal_gpio_pinconfig(COREMARK_GPIO, g_AM_HAL_GPIO_OUTPUT);

    if (g_eAmbClkSpd == AM_CLK_SPEED_96MHz)
    {
        am_hal_burst_avail_e          eBurstModeAvailable;

        //
        // Check that the Burst Feature is available.
        //
        am_util_delay_ms(2000);
        if (AM_HAL_STATUS_SUCCESS == am_hal_burst_mode_initialize(&eBurstModeAvailable))
        {
            if (AM_HAL_BURST_AVAIL == eBurstModeAvailable)
            {
                am_util_stdio_printf("Apollo3 Burst Mode is Available\n");
            }
            else
            {
                am_util_stdio_printf("Apollo3 Burst Mode is Not Available\n");
            }
        }
        else
        {
            am_util_stdio_printf("Failed to Initialize for Burst Mode operation\n");
        }

        //
        // Put the MCU into "Burst" mode.
        //
        if ( am_hal_burst_mode_enable(&eBurstMode) == AM_HAL_STATUS_SUCCESS )
        {
            if (AM_HAL_BURST_MODE == eBurstMode)
            {
                am_util_stdio_printf("Apollo3 operating in Burst Mode (96MHz)\n");
            }
        }
        else
        {
            am_util_stdio_printf("Failed to Enable Burst Mode operation\n");
        }
    }

    am_hal_gpio_state_write(COREMARK_GPIO,AM_HAL_GPIO_OUTPUT_SET);
    am_hal_flash_delay(FLASH_CYCLES_US(10));
    am_hal_gpio_state_write(COREMARK_GPIO,AM_HAL_GPIO_OUTPUT_CLEAR);
#endif // AM_PART_APOLLO3 || AM_PART_APOLLO3P
    //
    // To minimize power during the run, disable the UART.
    //
    am_bsp_uart_printf_disable();
}

/* Function : portable_fini
    Target specific final code
*/
void portable_fini(core_portable *p)
{
    p->portable_id=0;

#if defined(AM_PART_APOLLO3)
    am_hal_gpio_state_write(COREMARK_GPIO, AM_HAL_GPIO_OUTPUT_SET);
    am_hal_flash_delay(FLASH_CYCLES_US(10));
    am_hal_gpio_state_write(COREMARK_GPIO, AM_HAL_GPIO_OUTPUT_CLEAR);
    //
    // Now that we're done, turn all memory back on.
    //
    if ( am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEM_ALL) )
    {
      am_util_stdio_printf("Failed to re-enable all memory\n");
      while(1);
    }
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();
#endif // AM_PART_APOLLO3

#if AM_PRINT_RESULTS
    int iCnt;
    char *pcBuf;

    //
    // Initialize the printf interface for UART output.
    //
    am_bsp_uart_printf_enable();

    //
    // Print the banner.
    //
    am_util_stdio_printf("\n\n\n\nAmbiq Micro Coremark run finished!\n\n");

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
}

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

        if ( (am_bufcnt+ui32NumChars) >= PRTBUFSIZE )
        {
            //
            // This string is 40 chars (with the NULL terminator)
            //
            am_util_stdio_sprintf(&am_prtbuf[PRTBUFSIZE-(40+1)], "BUFFER OVERFLOWED! Increase PRTBUFSIZE\n");
            am_prtbuf[PRTBUFSIZE-1] = 0x00;     // Double terminate the buffer
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
