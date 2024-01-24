//*****************************************************************************
//
//! @file cache_monitor.c
//!
//! @brief Example to show the performance of cache.
//!
//! Purpose: This example provides a demonstration of the cache monitor to check
//! the cache hit rate and cache miss number.
//!
//! Additional Information:
//! If the fireball device card is used, this example can work on:
//! Apollo3p_evb + Cygnus
//! Recommend to use 3.3V power supply voltage.
//!
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

#include "am_devices_mspi_psram_aps6404l.h"
#include <string.h>

//#define         XIP_UNCACHED
#define         TEMP_BUFFER_SIZE        AM_DEVICES_MSPI_PSRAM_PAGE_SIZE // ((AM_HAL_IOM_MAX_TXNSIZE_SPI + 256) & 0xFFFFFF00)
uint32_t        g_TempBuf[2][TEMP_BUFFER_SIZE / 4];
#define         INITIAL_DATA            0x12

#define         DATA_STEP               0x11
#define         SIZE_STEP               2
uint32_t        g_ui32CurData = INITIAL_DATA;
uint32_t        g_iteration = 0;
#define         ITERATION_NUM           10
// 4 pixel worth of data
#define         DATA_4P(data)           ((data) | ((data) << 8) | ((data) << 16) | ((data) << 24))
uint32_t        g_data_size = 120 * 120;
uint32_t        g_prime_num[ITERATION_NUM] = {10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000};
uint32_t        g_exp_prime[ITERATION_NUM] = {4, 8, 15, 25, 46, 95, 168, 303, 669, 1229};

uint32_t        DMATCBBuffer[4096];
void            *g_MSPIDevHdl;
void            *g_MSPIHdl;

uint32_t        g_DMON[4] = {0};
uint32_t        g_IMON[4] = {0};

#define MSPI_XIP_BASE_ADDRESS   0x04000000
#define PSRAM_XIP_BASE          MSPI_XIP_BASE_ADDRESS
#define PSRAM_XIP_OFFSET        (PSRAM_XIP_BASE - MSPI_XIP_BASE_ADDRESS)
#define PSRAM_XIP_SIZE          (1024*1024)

#define XIP_REFRESH_RATE        10

#define MSPI_TEST_MODULE        1

typedef uint32_t (*mspi_xip_test_function_t)(uint32_t, uint32_t, uint32_t);

/*  File automatically generated with
    "BIN2C prime_mpi.bin"
    BIN2C (C) JRVV, ELECSAN S.A., 2016
    This program is freeware
*/
#define SZ_PRIME_MPI    98
const unsigned char Kc_PRIME_MPI[SZ_PRIME_MPI] =
{
    0x70, 0xB4, 0x04, 0x46, 0x00, 0x20, 0x89, 0x1C, 0x8C, 0x42, 0x28, 0xDB, 0x45, 0x1C, 0x02, 0x26,
    0x8E, 0x42, 0x20, 0xDA, 0x91, 0xFB, 0xF6, 0xF3, 0x06, 0xFB, 0x13, 0x13, 0xD3, 0xB1, 0x76, 0x1C,
    0x8E, 0x42, 0x18, 0xDA, 0x91, 0xFB, 0xF6, 0xF3, 0x06, 0xFB, 0x13, 0x13, 0x93, 0xB1, 0x76, 0x1C,
    0x8E, 0x42, 0x10, 0xDA, 0x91, 0xFB, 0xF6, 0xF3, 0x06, 0xFB, 0x13, 0x13, 0x53, 0xB1, 0x76, 0x1C,
    0x8E, 0x42, 0x08, 0xDA, 0x91, 0xFB, 0xF6, 0xF3, 0x06, 0xFB, 0x13, 0x13, 0x00, 0x2B, 0x18, 0xBF,
    0x76, 0x1C, 0xDD, 0xD1, 0x05, 0x46, 0x51, 0x18, 0x8C, 0x42, 0x28, 0x46, 0xD6, 0xDA, 0x70, 0xBC,
    0x70, 0x47,
};

am_devices_mspi_psram_config_t MSPI_PSRAM_QuadCE0MSPIConfig =
{
    .eDeviceConfig            = AM_HAL_MSPI_FLASH_QUAD_CE0,
//    .eDeviceConfig            = AM_HAL_MSPI_PSRAM_QUAD_CE0,
    .eClockFreq               = AM_HAL_MSPI_CLK_8MHZ,
    .ui32NBTxnBufLength       = sizeof(DMATCBBuffer) / sizeof(uint32_t),
    .pNBTxnBuf                = DMATCBBuffer,
    .ui32ScramblingStartAddr  = 0,
    .ui32ScramblingEndAddr    = 0,
};

const am_hal_cachectrl_config_t cache_monitor_cachectrl =
{
    .bLRU                       = 0,
    .eDescript                  = AM_HAL_CACHECTRL_DESCR_1WAY_128B_1024E,
    .eMode                      = AM_HAL_CACHECTRL_CONFIG_MODE_INSTR_DATA,
};

float g_hit_rate = 0.0;
uint32_t g_cache_miss = 0;

//! MSPI interrupts.
static const IRQn_Type mspi_interrupts[] =
{
    MSPI0_IRQn,
#if defined(AM_PART_APOLLO3P)
    MSPI1_IRQn,
    MSPI2_IRQn,
#endif
};

//
// Take over the interrupt handler for whichever MSPI we're using.
//
#define psram_mspi_isr                                                          \
    am_mspi_isr1(MSPI_TEST_MODULE)
#define am_mspi_isr1(n)                                                        \
    am_mspi_isr(n)
#define am_mspi_isr(n)                                                         \
    am_mspi ## n ## _isr

//*****************************************************************************
//
// MSPI ISRs.
//
//*****************************************************************************
void psram_mspi_isr(void)
{
    uint32_t      ui32Status;

    am_hal_mspi_interrupt_status_get(g_MSPIHdl, &ui32Status, false);

    am_hal_mspi_interrupt_clear(g_MSPIHdl, ui32Status);

    am_hal_mspi_interrupt_service(g_MSPIHdl, ui32Status);
}

//*****************************************************************************
//
// Prototype for third-party algorithm.
//
//*****************************************************************************
// n: number of primes, id=process number (), p=number of processes (1)
int prime_number ( int n, int id, int p );

int
mspi_psram_init(void)
{
    uint32_t      ui32Status;

    //
    // Configure the MSPI and PSRAM Device.
    //
    ui32Status = am_devices_mspi_psram_init(MSPI_TEST_MODULE, &MSPI_PSRAM_QuadCE0MSPIConfig, &g_MSPIDevHdl, &g_MSPIHdl);
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to configure the MSPI and PSRAM Device correctly!\n");
        return -1;
    }
    return 0;
}

//*****************************************************************************
//
// Function prototypes
//
//*****************************************************************************
bool
run_mspi_xip(void)
{
    uint32_t ret;
    mspi_xip_test_function_t test_function = (mspi_xip_test_function_t)((PSRAM_XIP_BASE) | 0x00000001); // // Execute a call to the test function in the sector.
    CACHECTRL->CACHECFG |= CACHECTRL_CACHECFG_ENABLE_MONITOR_Msk;
    CACHECTRL->CTRL = CACHECTRL_CTRL_RESET_STAT_Msk;
     // Test the function
    ret = test_function(g_prime_num[g_iteration], 0, 1);
    CACHECTRL->CACHECFG &= ~CACHECTRL_CACHECFG_ENABLE_MONITOR_Msk;
    g_IMON[0] = CACHECTRL->IMON0;
    g_IMON[1] = CACHECTRL->IMON1;
    g_IMON[2] = CACHECTRL->IMON2;
    g_IMON[3] = CACHECTRL->IMON3;
    return (ret == g_exp_prime[g_iteration]);
}

//*****************************************************************************
//
// Timer handling to execute XIP codes
//
//*****************************************************************************
static am_hal_ctimer_config_t g_sTimer =
{
    // Don't link timers.
    0,

    // Set up TimerA.
    (AM_HAL_CTIMER_FN_REPEAT |
     AM_HAL_CTIMER_INT_ENABLE    |
     AM_HAL_CTIMER_HFRC_12KHZ),

    // No configuration for TimerB.
    0,
};

// Timer Interrupt Service Routine (ISR)
void
am_ctimer_isr(void)
{
    uint32_t ui32Status;

    ui32Status = am_hal_ctimer_int_status_get(false);
    am_hal_ctimer_int_clear(ui32Status);

    am_hal_ctimer_int_service(ui32Status);
}

void
stop_xip_timer(void)
{
    //
    // Stop timer A0
    //
    am_hal_ctimer_stop(0, AM_HAL_CTIMER_TIMERA);
}

void
start_xip_timer(void)
{
    stop_xip_timer(); // Just in case host died without sending STOP last time
    //
    // Start timer A0
    //
    am_hal_ctimer_start(0, AM_HAL_CTIMER_TIMERA);
}

void
deinit_xip_timer(void)
{
    NVIC_DisableIRQ(CTIMER_IRQn);
    am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA0);
    am_hal_ctimer_int_disable(AM_HAL_CTIMER_INT_TIMERA0);
}

static void
XIP_handler(void)
{
    if (!run_mspi_xip())
    {
        am_util_stdio_printf("Unable to run XIP successfully\n");
        while(1);
    }
    g_cache_miss = g_IMON[1] - g_IMON[2];
    g_hit_rate = (float)g_IMON[2] * 100.0F / (float)g_IMON[1];
    am_util_stdio_printf("*****Iteration%d complete*****\n", g_iteration);
    am_util_stdio_printf("Total fetch number: %d\n", g_IMON[0]);
    am_util_stdio_printf("Look up number: %d\n", g_DMON[1]);
    am_util_stdio_printf("Cache miss number: %d\n", g_cache_miss);
    am_util_stdio_printf("Cache hit rate: %6.2f%%\n", g_hit_rate);
    if ( ++g_iteration >= ITERATION_NUM )
    {
        g_iteration = 0;
        stop_xip_timer();
        deinit_xip_timer();
        am_util_stdio_printf("XIP Cache Performance Demonstration End!\n");
        am_util_stdio_printf("Cache monitor example completes...\n");
    }
}

void
init_xip_timer(void)
{
    uint32_t ui32Period;

    //
    // Set up timer A0.
    //
    am_hal_ctimer_clear(0, AM_HAL_CTIMER_TIMERA);
    am_hal_ctimer_config(0, &g_sTimer);

    ui32Period = 12000 / XIP_REFRESH_RATE;
    am_hal_ctimer_period_set(0, AM_HAL_CTIMER_TIMERA, ui32Period,
                             (ui32Period >> 1));

    //
    // Clear the timer Interrupt
    //
    am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA0);

    //
    // Enable the timer Interrupt.
    //
    am_hal_ctimer_int_register(AM_HAL_CTIMER_INT_TIMERA0,
                               XIP_handler);
    am_hal_ctimer_int_enable(AM_HAL_CTIMER_INT_TIMERA0);

    //
    // Enable the timer interrupt in the NVIC.
    //
    NVIC_EnableIRQ(CTIMER_IRQn);
}

int
mspi_xip_init()
{
    uint32_t ui32Status;
    //
    // Write the executable function into the target sector.
    //
    am_util_stdio_printf("Writing Executable function of %d Bytes to address %d\n", SZ_PRIME_MPI, PSRAM_XIP_BASE);
    ui32Status = am_devices_mspi_psram_write(g_MSPIDevHdl, (uint8_t *)Kc_PRIME_MPI, PSRAM_XIP_OFFSET, SZ_PRIME_MPI, true);
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to write executable function to Flash Device!\n");
        return -1;
    }
    //
    // Set up for XIP operation.
    //
    am_util_stdio_printf("Putting the MSPI and External PSRAM into XIP mode\n");
    ui32Status = am_devices_mspi_psram_enable_xip(g_MSPIDevHdl);
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to put the MSPI into XIP mode!\n");
        return -1;
    }
#ifndef XIP_UNCACHED
    ui32Status = am_hal_cachectrl_control(AM_HAL_CACHECTRL_CONTROL_FLASH_CACHE_INVALIDATE, 0);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to invalidate Cache!\n");
        return -1;
    }
#else
    //  Mark XIP as non-cached - to make sure we see its impact
    am_hal_cachectrl_nc_cfg_t ncCfg;
    ncCfg.bEnable = true;
    ncCfg.eNCRegion = AM_HAL_CACHECTRL_NCR0;
    ncCfg.ui32StartAddr = PSRAM_XIP_BASE;
    ncCfg.ui32EndAddr = PSRAM_XIP_BASE + PSRAM_XIP_SIZE;
    ui32Status = am_hal_cachectrl_control(AM_HAL_CACHECTRL_CONTROL_NC_CFG, &ncCfg);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to mark XIP region as non-cacheable\n");
        return -1;
    }
#endif
    return 0;
}


int
flush_mspi_psram_data(void)
{
    uint32_t      ui32Status;

    //DEBUG_PRINT("\nWriting a known pattern to psram!\n");
    for (uint32_t address = 0; address < g_data_size; address += TEMP_BUFFER_SIZE)
    {
        uint32_t i;
        //
        // Generate raw color data into PSRAM frame buffer
        //
        for (i = 0; i < TEMP_BUFFER_SIZE / 4; i++)
        {
            g_TempBuf[0][i] = DATA_4P(g_ui32CurData);
        }

        AM_CRITICAL_BEGIN
        CACHECTRL->CACHECFG |= CACHECTRL_CACHECFG_ENABLE_MONITOR_Msk;
        CACHECTRL->CTRL = CACHECTRL_CTRL_RESET_STAT_Msk;
        AM_CRITICAL_END

        //
        // Write the buffer into the target address in PSRAM
        //
        ui32Status = am_devices_mspi_psram_write(g_MSPIDevHdl, (uint8_t *)g_TempBuf[0], address, TEMP_BUFFER_SIZE, true);

        AM_CRITICAL_BEGIN
        CACHECTRL->CACHECFG &= ~CACHECTRL_CACHECFG_ENABLE_MONITOR_Msk;
        g_DMON[0] += CACHECTRL->DMON0;
        g_DMON[1] += CACHECTRL->DMON1;
        g_DMON[2] += CACHECTRL->DMON2;
        g_DMON[3] += CACHECTRL->DMON3;
        AM_CRITICAL_END

        if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
        {
            am_util_stdio_printf("\nFailed to write buffer to PSRAM Device!\n");
            return -1;
        }
        //
        // Read the data back into the RX buffer.
        //
        ui32Status = am_devices_mspi_psram_read(g_MSPIDevHdl, (uint8_t *)g_TempBuf[1], address, TEMP_BUFFER_SIZE, true);
        if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
        {
            am_util_stdio_printf("\nFailed to read buffer to PSRAM Device!\n");
            return -1;
        }

        //
        // Compare the buffers
        //
        for (uint32_t i = 0; i < TEMP_BUFFER_SIZE / 4; i++)
        {
            if (g_TempBuf[1][i] != g_TempBuf[0][i])
            {
                am_util_stdio_printf("\nTX and RX buffers failed to compare!\n");
                return -1;
            }
        }
    }
    return 0;
}

//*****************************************************************************
//
// Main Function.
//
//*****************************************************************************
int
main(void)
{
    int iRet;
    uint32_t ui32Result;

    //
    // Set the clock frequency.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);

    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&cache_monitor_cachectrl);
    am_hal_cachectrl_enable();

    //
    // Configure the board for low power operation.
    //
    am_bsp_low_power_init();

    //
    // Enable the ITM print interface.
    //
    am_bsp_itm_printf_enable();

    //
    // Clear the terminal and print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("Ambiq Micro cache monitor example.\n\n");

    //
    // Print the compiler version.
    //
    am_util_stdio_printf("App Compiler:    %s\n", COMPILER_VERSION);
    am_util_stdio_printf("HAL Compiler:    %s\n", g_ui8HALcompiler);
    am_util_stdio_printf("HAL SDK version: %d.%d.%d\n",
                         g_ui32HALversion.s.Major,
                         g_ui32HALversion.s.Minor,
                         g_ui32HALversion.s.Revision);
    am_util_stdio_printf("HAL compiled with %s-style registers\n",
                         g_ui32HALversion.s.bAMREGS ? "AM_REG" : "CMSIS");

    // Initialize the MSPI PSRAM
    iRet = mspi_psram_init();
    if (iRet)
    {
        am_util_stdio_printf("Unable to initialize MSPI psram\n");
        while(1);
    }
    NVIC_EnableIRQ(mspi_interrupts[MSPI_TEST_MODULE]);

    am_hal_interrupt_master_enable();
    am_util_stdio_printf("DCACHE Performance Demonstration Start!\n");
    while(1)
    {
        memset(g_DMON, 0, sizeof(uint32_t) * 4);
        iRet = flush_mspi_psram_data();
        if (iRet)
        {
            am_util_stdio_printf("Unable to flush MSPI psram\n");
            while(1);
        }
        g_cache_miss = g_DMON[1] - g_DMON[2];
        g_hit_rate = (float)g_DMON[2] / (float)g_DMON[1] * 100.0F;
        am_util_stdio_printf("*****Iteration%d complete*****\n", g_iteration);
        am_util_stdio_printf("Total fetch number: %d\n", g_DMON[0]);
        am_util_stdio_printf("Look up number: %d\n", g_DMON[1]);
        am_util_stdio_printf("Cache miss number: %d\n", g_cache_miss);
        am_util_stdio_printf("Cache hit rate: %6.2f%%\n", g_hit_rate);
        if ( ++g_iteration >= ITERATION_NUM )
        {
            g_iteration = 0;
            break;
        }
        g_ui32CurData += DATA_STEP;
        g_data_size *= SIZE_STEP;
    }
    am_util_stdio_printf("DCACHE Performance Demonstration End!\n");

    am_util_stdio_printf("ICACHE Performance Demonstration Start!\n");
    while(1)
    {
        memset(g_IMON, 0, sizeof(uint32_t) * 4);

        AM_CRITICAL_BEGIN
        CACHECTRL->CACHECFG |= CACHECTRL_CACHECFG_ENABLE_MONITOR_Msk;
        CACHECTRL->CTRL = CACHECTRL_CTRL_RESET_STAT_Msk;
        AM_CRITICAL_END
        //
        // Determine the number of primes for the given value.
        //
        ui32Result = prime_number(g_prime_num[g_iteration], 0, 1);

        AM_CRITICAL_BEGIN
        CACHECTRL->CACHECFG &= ~CACHECTRL_CACHECFG_ENABLE_MONITOR_Msk;
        g_IMON[0] = CACHECTRL->IMON0;
        g_IMON[1] = CACHECTRL->IMON1;
        g_IMON[2] = CACHECTRL->IMON2;
        g_IMON[3] = CACHECTRL->IMON3;
        AM_CRITICAL_END

        if ( ui32Result != g_exp_prime[g_iteration] )
        {
            am_util_stdio_printf("ERROR: Invalid result. Expected %d, got %d.\n", g_prime_num[g_iteration], ui32Result);
            while(1);
        }
        g_cache_miss = g_IMON[1] - g_IMON[2];
        g_hit_rate = (float)g_IMON[2] * 100.0F / (float)g_IMON[1];
        am_util_stdio_printf("*****Iteration%d complete*****\n", g_iteration);
        am_util_stdio_printf("Total fetch number: %d\n", g_IMON[0]);
        am_util_stdio_printf("Look up number: %d\n", g_DMON[1]);
        am_util_stdio_printf("Cache miss number: %d\n", g_cache_miss);
        am_util_stdio_printf("Cache hit rate: %6.2f%%\n", g_hit_rate);
        if ( ++g_iteration >= ITERATION_NUM )
        {
            g_iteration = 0;
            break;
        }
    }
    am_util_stdio_printf("ICACHE Performance Demonstration End!\n");

    iRet = mspi_xip_init();
    if (iRet)
    {
        am_util_stdio_printf("Unable to init XIP\n");
        while(1);
    }
    // Configure a Timer to trigger XIP
    init_xip_timer();
    start_xip_timer();
    am_util_stdio_printf("XIP Cache Performance Demonstration Start!\n");
    while(1);
}

