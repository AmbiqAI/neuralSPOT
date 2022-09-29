//*****************************************************************************
//
//! @file mpu_mspi_ddr_octal_psram_example.c
//!
//! @brief Example of using MPU protection as workaround for DSP RAM issue.
//!
//! Purpose: This example demonstrates how to use MPU as workaround for DSP RAM
//! issue.
//!
//! Additional Information:
//! To enable debug printing, add the following project-level macro definitions.
//!
//! AM_DEBUG_PRINTF
//!
//! When defined, debug messages will be sent over ITM/SWO at 1M Baud.
//!
//! Note that when these macros are defined, the device will never achieve deep
//! sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.
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
#include <string.h>
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_devices_mspi_psram_aps12808l.h"
#include "am_util.h"
#include "regions.h"

#define WAKE_INTERVAL_IN_MS     1000
#define XT_PERIOD               32768
#define WAKE_INTERVAL           XT_PERIOD * WAKE_INTERVAL_IN_MS * 1e-3

#define TEST_COUNT              100
#define DSPRAM_TEST_ADDR        0x10170000
#define PSRAM_TEST_ADDR         0x14040000
#define SSRAM_TEST_ADDR         0x100E0000
#define PSRAM_DUMMY_READ_ADDR   0x14000000
#define SSRAM_DUMMY_READ_ADDR   0x10060000

#define DSPRAM_START_ADDR       0x10160000
#define DSPRAM_END_ADDR         0x101D8000
#define SSRAM_START_ADDR        0x10060000
#define SSRAM_END_ADDR          0x10160000
#define PSRAM_START_ADDR        0x14000000
#define PSRAM_END_ADDR          0x18000000

uint32_t        DMATCBBuffer[2560];
void            *g_pDevHandle;
void            *g_pHandle;

uint32_t g_timer_isr_count = 0;
uint32_t *g_pTestCount1;
uint32_t *g_pTestCount2;

uint8_t testBuffer[6] = {0x00, 0x55, 0xAA, 0x5A, 0xA5, 0xFF};
uint8_t testBuffer2[6] = {0x0F, 0x5F, 0xA0, 0x05, 0x0A, 0xAF};

am_hal_pwrctrl_dsp_memory_config_t DSPRAM_Cfg =
{
    .bEnableICache      = false,
    .bRetainCache       = false,
    .bEnableRAM         = true,
    .bActiveRAM         = false,
    .bRetainRAM         = true,
};

am_devices_mspi_psram_config_t MSPI_PSRAM_OctalCE0MSPIConfig =
{
    .eDeviceConfig            = AM_HAL_MSPI_FLASH_OCTAL_DDR_CE0,
    .eClockFreq               = AM_HAL_MSPI_CLK_96MHZ,
    .ui32NBTxnBufLength       = sizeof(DMATCBBuffer) / sizeof(uint32_t),
    .pNBTxnBuf                = DMATCBBuffer,
    .ui32ScramblingStartAddr  = 0,
    .ui32ScramblingEndAddr    = 0,
};

am_devices_mspi_psram_config_t MSPI_PSRAM_OctalCE1MSPIConfig =
{
    .eDeviceConfig            = AM_HAL_MSPI_FLASH_OCTAL_DDR_CE1,
    .eClockFreq               = AM_HAL_MSPI_CLK_96MHZ,
    .ui32NBTxnBufLength       = sizeof(DMATCBBuffer) / sizeof(uint32_t),
    .pNBTxnBuf                = DMATCBBuffer,
    .ui32ScramblingStartAddr  = 0,
    .ui32ScramblingEndAddr    = 0,
};

#define MSPI_TEST_MODULE              0

#if (MSPI_TEST_MODULE == 0)
#define MSPI_XIP_BASE_ADDRESS 0x14000000
#elif (MSPI_TEST_MODULE == 1)
#define MSPI_XIP_BASE_ADDRESS 0x18000000
#elif (MSPI_TEST_MODULE == 2)
#define MSPI_XIP_BASE_ADDRESS 0x1C000000
#endif // #if (MSPI_TEST_MODULE == 0)

//! MSPI interrupts.
static const IRQn_Type mspi_interrupts[] =
{
    MSPI0_IRQn,
    MSPI1_IRQn,
    MSPI2_IRQn,
};

// Need to allocate 20 Words even though we only need 16, to ensure we have 16 Byte alignment
AM_SHARED_RW uint32_t axiScratchBuf[20];

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

    am_hal_mspi_interrupt_status_get(g_pHandle, &ui32Status, false);

    am_hal_mspi_interrupt_clear(g_pHandle, ui32Status);

    am_hal_mspi_interrupt_service(g_pHandle, ui32Status);
}

//*****************************************************************************
//
// Fault handler for the MPU
//
//*****************************************************************************
void
MemManage_Handler(void)
{
    uint32_t ui32mmfar = *(volatile uint32_t*)AM_REG_SYSCTRL_MMFAR; //Read MMFAR before MMFSR
    uint8_t ui8mmfsr = *(volatile uint8_t*)AM_REG_SYSCTRL_MMFSR;
    *(volatile uint8_t*)AM_REG_SYSCTRL_MMFSR = ui8mmfsr; //Write back MMFSR to clear logged faults
    if ( (ui8mmfsr & 0x80) != 0 )   //MMAR holds a valid fault address if MMARVALID = 1
    {
      if ( ui32mmfar >= DSPRAM_START_ADDR && ui32mmfar < DSPRAM_END_ADDR )
      {
        CPU->DAXICTRL_b.DAXIFLUSHWRITE = 1;
        CPU->DAXICTRL_b.DAXIINVALIDATE = 1;
        am_util_debug_printf("Flush DAXI and Dummy Read\n");

        mpu_region_disable(sDSPRAM_A.ui8RegionNumber);
        mpu_region_disable(sDSPRAM_B.ui8RegionNumber);
        mpu_region_enable(sPSRAM.ui8RegionNumber);
        mpu_region_enable(sSSRAM_A.ui8RegionNumber);
        mpu_region_enable(sSSRAM_B.ui8RegionNumber);
        am_util_debug_printf("Enable MPU for PSRAM and SSRAM\n");
      }
      else if ( ( ui32mmfar >= SSRAM_START_ADDR && ui32mmfar < SSRAM_END_ADDR ) || ( ui32mmfar >= PSRAM_START_ADDR && ui32mmfar < PSRAM_END_ADDR ) )
      {
        mpu_region_disable(sPSRAM.ui8RegionNumber);
        mpu_region_disable(sSSRAM_A.ui8RegionNumber);
        mpu_region_disable(sSSRAM_B.ui8RegionNumber);
        mpu_region_enable(sDSPRAM_A.ui8RegionNumber);
        mpu_region_enable(sDSPRAM_B.ui8RegionNumber);
        am_util_debug_printf("Enable MPU for DSP RAM\n");
      }
    }
}

//*****************************************************************************
//
// Init function for Stimer.
//
//*****************************************************************************
void
stimer_init(void)
{
    //
    // Enable compare A interrupt in STIMER
    //
    am_hal_stimer_int_enable(AM_HAL_STIMER_INT_COMPAREA);

    //
    // Enable the timer interrupt in the NVIC.
    //
    NVIC_SetPriority(STIMER_CMPR0_IRQn, 4);
    NVIC_EnableIRQ(STIMER_CMPR0_IRQn);

    //
    // Configure the STIMER and run
    //
    am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
    am_hal_stimer_compare_delta_set(0, WAKE_INTERVAL);
    am_hal_stimer_config(AM_HAL_STIMER_XTAL_32KHZ |
                         AM_HAL_STIMER_CFG_COMPARE_A_ENABLE);
}

//*****************************************************************************
//
// Timer Interrupt Service Routine (ISR)
//
//*****************************************************************************
void
am_stimer_cmpr0_isr(void)
{
    //
    // Check the timer interrupt status.
    //
    am_hal_stimer_int_clear(AM_HAL_STIMER_INT_COMPAREA);
    am_hal_stimer_compare_delta_set(0, WAKE_INTERVAL);

    uint8_t testByte;
    testByte  = testBuffer2[g_timer_isr_count % 6];
    memset((void *)DSPRAM_TEST_ADDR, testByte, 0x400);
    g_timer_isr_count++;
    am_util_debug_printf("\nWrite DSPRAM in Stimer ISR\n");
    am_util_debug_printf("g_timer_isr_count=%d \n\n", g_timer_isr_count);
}

am_devices_mspi_psram_ddr_timing_config_t MSPIDdrTimingConfig;

//*****************************************************************************
//
// MPU Example Main.
//
//*****************************************************************************
int
main(void)
{
    //
    // Initialize the Extended (DSP0/1) RAM for usage.
    //
    am_hal_pwrctrl_dsp_memory_config(AM_HAL_DSP0, &DSPRAM_Cfg);
    am_hal_pwrctrl_dsp_memory_config(AM_HAL_DSP1, &DSPRAM_Cfg);

    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

#if !defined(AM_PART_APOLLO4P)
    //
    // Set up scratch AXI buf (needs 64B - aligned to 16 Bytes)
    //
    am_hal_daxi_control(AM_HAL_DAXI_CONTROL_AXIMEM, (uint8_t *)((uint32_t)(axiScratchBuf + 3) & ~0xF));
#endif

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
    am_util_stdio_printf("Apollo4 MPU Example\n\n");

    am_util_debug_printf("Starting MSPI DDR Timing Scan: \n");
    if ( AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS == am_devices_mspi_psram_aps12808l_ddr_init_timing_check(MSPI_TEST_MODULE, &MSPI_PSRAM_OctalCE0MSPIConfig, &MSPIDdrTimingConfig) )
    {
        am_util_debug_printf("==== Scan Result: RXDQSDELAY0 = %d \n", MSPIDdrTimingConfig.ui32Rxdqsdelay);
    }
    else
    {
        am_util_debug_printf("==== Scan Result: Failed, no valid setting.  \n");
    }

    //
    // Configure the MSPI and PSRAM Device.
    //
    uint32_t ui32Status = am_devices_mspi_psram_aps12808l_ddr_init(MSPI_TEST_MODULE, &MSPI_PSRAM_OctalCE0MSPIConfig, &g_pDevHandle, &g_pHandle);
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to configure the MSPI and PSRAM Device correctly!\n");
    }
    NVIC_SetPriority(mspi_interrupts[MSPI_TEST_MODULE], 5);
    NVIC_EnableIRQ(mspi_interrupts[MSPI_TEST_MODULE]);

    am_hal_interrupt_master_enable();

    //
    //  Set the DDR timing from previous scan.
    //
    ui32Status = am_devices_mspi_psram_aps12808l_apply_ddr_timing(g_pDevHandle, &MSPIDdrTimingConfig);
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to apply DDR timing in the MSPI!\n");
    }

    //
    // Enable XIP mode.
    //
    ui32Status = am_devices_mspi_psram_aps12808l_ddr_enable_xip(g_pDevHandle);
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to enable XIP mode in the MSPI!\n");
    }

    //
    // Read MPU TYPE register and check for MPU presence.
    //
    uint32_t mpuType = mpu_type_get();
    if ( mpuType == 0x00000800 )
    {
        am_util_stdio_printf("MPU is present\n");
    }
    else
    {
        am_util_stdio_printf("ERROR: MPU NOT PRESENT\r\n");
        return -1;
    }

    //
    // Set the MPU regions.
    //
    mpu_region_configure(&sSSRAM_A, false);
    mpu_region_configure(&sSSRAM_B, false);
    mpu_region_configure(&sPSRAM, false);
    mpu_region_configure(&sDSPRAM_A, true);
    mpu_region_configure(&sDSPRAM_B, true);

    //
    // Enable the MPU
    //
    mpu_global_configure(true, true, false);

    //
    // Set high priority for MPU ISR
    //
    NVIC_SetPriority(MemoryManagement_IRQn, 1);
    NVIC_EnableIRQ(MemoryManagement_IRQn);

    //
    // Trigger MPU ISR by access DSP RAM firstly
    //
    g_pTestCount1 = (uint32_t *)0x10162A40; //ADDR = 0x10162A40 located in DSP_RAM0
    g_pTestCount2 = (uint32_t *)0x101C1B00; //ADDR = 0x101C1B00 located in DSP_RAM1
    *g_pTestCount1 = 0;
    *g_pTestCount2 = 0;

    //
    // STIMER init.
    //
    stimer_init();

    //
    // Switch between DSPRAM and PSRAM/SSRAM
    //
    uint8_t testByte;
    while ( (*g_pTestCount1) < TEST_COUNT )
    {
        (*g_pTestCount1)++;
        testByte  = testBuffer[(*g_pTestCount1) % 6];
        memset((void *)PSRAM_TEST_ADDR, testByte, 0x400000);
        (*g_pTestCount2)++;
        testByte  = testBuffer[(*g_pTestCount2) % 6];
        memset((void *)SSRAM_TEST_ADDR, testByte, 0x1000);
    }

    am_hal_interrupt_master_disable();
    //
    //  End banner.
    //
    am_util_stdio_printf("\nApollo4 MPU Example Complete\n");

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

