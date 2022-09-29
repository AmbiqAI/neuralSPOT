//*****************************************************************************
//
//! @file mspi_octal_example_fifo_full.c
//!
//! @brief Example of the MSPI operation with Octal SPI Flash.
//!
//! Purpose: This example demonstrates how to detect the MSPI FIFO Full
//! condition in an application
//!
//!
//! Printing takes place over the ITM at 1M Baud.
//!
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
#include "am_devices_mspi_atxp032.h"
#include "am_util.h"

#define MSPI_TARGET_SECTOR      (0)
#define MSPI_BUFFER_SIZE        (4*1024)  // 16K example buffer size.

#define MSPI_TEST_MODULE        AM_BSP_MSPI_FLASH_MODULE

#define DEFAULT_TIMEOUT         10000

#if (MSPI_TEST_MODULE == 0)
#define MSPI_XIP_BASE_ADDRESS 0x14000000
#elif (MSPI_TEST_MODULE == 1)
#define MSPI_XIP_BASE_ADDRESS 0x18000000
#elif (MSPI_TEST_MODULE == 2)
#define MSPI_XIP_BASE_ADDRESS 0x1C000000
#endif

#define MSPI_TEST_FREQ          AM_HAL_MSPI_CLK_24MHZ

uint32_t        DMATCBBuffer[2560];
uint8_t         TestBuffer[2048];
uint8_t         DummyBuffer[1024];
uint8_t         g_SectorTXBuffer[MSPI_BUFFER_SIZE];
uint8_t         g_SectorRXBuffer[MSPI_BUFFER_SIZE];
void            *g_MSPIHdl;
void            *g_FlashHdl;

const am_devices_mspi_atxp032_config_t MSPI_Flash_Config =
{
    .eDeviceConfig = AM_HAL_MSPI_FLASH_OCTAL_CE0,
    .eClockFreq = MSPI_TEST_FREQ,
    .pNBTxnBuf = DMATCBBuffer,
    .ui32NBTxnBufLength = (sizeof(DMATCBBuffer) / sizeof(uint32_t)),
    .ui32ScramblingStartAddr = 0,
    .ui32ScramblingEndAddr = 0,
};

const am_devices_mspi_atxp032_config_t MSPI_Flash_96MHZ_Config =
{
    .eDeviceConfig = AM_HAL_MSPI_FLASH_OCTAL_CE0,
    .eClockFreq = AM_HAL_MSPI_CLK_96MHZ,
//    .eClockFreq = AM_HAL_MSPI_CLK_24MHZ,
    .pNBTxnBuf = DMATCBBuffer,
    .ui32NBTxnBufLength = (sizeof(DMATCBBuffer) / sizeof(uint32_t)),
    .ui32ScramblingStartAddr = 0,
    .ui32ScramblingEndAddr = 0,
};

//! MSPI interrupts.
static const IRQn_Type mspi_interrupts[] =
{
    MSPI0_IRQn,
#if defined(AM_PART_APOLLO3P) || defined(AM_PART_APOLLO4) || defined(AM_PART_APOLLO4B)
    MSPI1_IRQn,
    MSPI2_IRQn,
#endif
};

//
// Take over the interrupt handler for whichever MSPI we're using.
//
#define flash_mspi_isr                                                          \
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
void flash_mspi_isr(void)
{
    uint32_t      ui32Status;

    am_hal_mspi_interrupt_status_get(g_MSPIHdl, &ui32Status, false);

    am_hal_mspi_interrupt_clear(g_MSPIHdl, ui32Status);

    am_hal_mspi_interrupt_service(g_MSPIHdl, ui32Status);
}

//*****************************************************************************
//
// MSPI Example Main.
//
//*****************************************************************************
int
main(void)
{
    uint32_t      ui32Status;

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
    am_util_stdio_printf("Apollo4 Octal MSPI Example\n\n");

    //
    // Configure the MSPI and Flash Device.
    //
    ui32Status = am_devices_mspi_atxp032_init(MSPI_TEST_MODULE, (void*)&MSPI_Flash_Config, &g_FlashHdl, &g_MSPIHdl);
    if (AM_DEVICES_MSPI_ATXP032_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to configure the MSPI and Flash Device correctly!\n");
    }

    NVIC_SetPriority(mspi_interrupts[MSPI_TEST_MODULE], AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(mspi_interrupts[MSPI_TEST_MODULE]);
    am_hal_interrupt_master_enable();

    //
    // Generate data into the Sector Buffer
    //
    for (uint32_t i = 0; i < MSPI_BUFFER_SIZE; i++)
    {
        g_SectorTXBuffer[i] = (i & 0xFF);
    }

    //
    // Erase the target sector.
    //
    am_util_stdio_printf("Erasing Sector %d\n", MSPI_TARGET_SECTOR);
    ui32Status = am_devices_mspi_atxp032_sector_erase(g_FlashHdl, MSPI_TARGET_SECTOR << 16);
    if (AM_DEVICES_MSPI_ATXP032_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to erase Flash Device sector!\n");
    }

    //
    // Make sure we aren't in XIP mode.
    //
    ui32Status = am_devices_mspi_atxp032_disable_xip(g_FlashHdl);
    if (AM_DEVICES_MSPI_ATXP032_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to disable XIP mode in the MSPI!\n");
    }

    //
    // Write the TX buffer into the target sector.
    //
    am_util_stdio_printf("Writing %d Bytes to Sector %d\n", MSPI_BUFFER_SIZE, MSPI_TARGET_SECTOR);
    ui32Status = am_devices_mspi_atxp032_write(g_FlashHdl, g_SectorTXBuffer, MSPI_TARGET_SECTOR << 16, MSPI_BUFFER_SIZE, true);
    if (AM_DEVICES_MSPI_ATXP032_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to write buffer to Flash Device!\n");
    }

    ui32Status = am_devices_mspi_atxp032_deinit(g_FlashHdl);
    if (AM_DEVICES_MSPI_ATXP032_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to shutdown the MSPI and Flash Device!\n");
    }

    ui32Status = am_devices_mspi_atxp032_init(MSPI_TEST_MODULE, (void*)&MSPI_Flash_96MHZ_Config, &g_FlashHdl, &g_MSPIHdl);
    if (AM_DEVICES_MSPI_ATXP032_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to configure the MSPI and Flash Device correctly!\n");
    }

    // TODO: This should be moved to Driver
    am_hal_mspi_interrupt_enable(g_MSPIHdl, MSPI0_INTEN_RXF_Msk);

    //
    // Read the data back into the RX buffer.
    //
    am_util_stdio_printf("Read %d Bytes from Sector %d\n", MSPI_BUFFER_SIZE, MSPI_TARGET_SECTOR);
    ui32Status = am_devices_mspi_atxp032_read(g_FlashHdl, g_SectorRXBuffer, MSPI_TARGET_SECTOR << 16, MSPI_BUFFER_SIZE, true);
    if ((AM_DEVICES_MSPI_ATXP032_STATUS_SUCCESS != ui32Status) && (AM_HAL_MSPI_FIFO_FULL_CONDITION != ui32Status))
    {
        am_util_stdio_printf("Failed to read buffer to Flash Device!\n");
    }
    if (AM_HAL_MSPI_FIFO_FULL_CONDITION == ui32Status)
    {
        am_util_stdio_printf("FIFO Full Condition Detected\n");
    }

    //
    // Compare the buffers
    //
    uint32_t ui32BadDataCount = 0;
    am_util_stdio_printf("Comparing the TX and RX Buffers\n");
    for (uint32_t i = 0; i < MSPI_BUFFER_SIZE; i++)
    {
        if (g_SectorRXBuffer[i] != g_SectorTXBuffer[i])
        {
            ui32BadDataCount++;
        }

        if ( (g_SectorRXBuffer[i] != g_SectorTXBuffer[i]) && (i > 1) &&
             ((g_SectorRXBuffer[i] == g_SectorRXBuffer[i - 1])  ||
              (g_SectorRXBuffer[i] == g_SectorRXBuffer[i + 1])) )
        {
            am_util_stdio_printf("Detected FIFO Full Data Pattern in RX Buffer\n");
            break;
        }
    }

    //
    //  End banner.
    //
    am_util_stdio_printf("Apollo4 MSPI Example Complete\n");

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

