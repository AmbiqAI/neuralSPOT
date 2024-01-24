//*****************************************************************************
//
//! @file apollo3_secbl_mspi_flash.c
//!
//! @brief a flash interface for atxp032/128 on the cygnus board
//!
//! Purpose: implement a secure bootloader flash interface for atxp032/128
//!
//! Printing takes place over the ITM at 1M Baud.
//!
//! Additional Information:
//! Needs cygnus board with atxp032/128 flash
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

#include <string.h>
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

#include "apollo3_secbl_mpsi_flash.h"
#include "am_devices_mspi_atxp032.h"

#define MSPI_TEST_MODULE      1

#define MSPI_XIP_BASE_ADDRESS 0x04000000

static uint32_t DMATCBBuffer[2560];
static void *g_FlashHdl;
static void *g_MSPIHdl;

static const am_devices_mspi_atxp032_config_t MSPI_Flash_Config =
{
    .eDeviceConfig = AM_HAL_MSPI_FLASH_QUAD_CE0,
    .eClockFreq = AM_HAL_MSPI_CLK_24MHZ,
    .pNBTxnBuf = DMATCBBuffer,
    .ui32NBTxnBufLength = (sizeof(DMATCBBuffer) / sizeof(uint32_t)),
    .ui32ScramblingStartAddr = 0,
    .ui32ScramblingEndAddr = 0,
};

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
#define flash_mspi_isr                                                         \
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
    uint32_t ui32Status;

    am_hal_mspi_interrupt_status_get(g_MSPIHdl, &ui32Status, false);

    am_hal_mspi_interrupt_clear(g_MSPIHdl, ui32Status);

    am_hal_mspi_interrupt_service(g_MSPIHdl, ui32Status);
}

static uint32_t mspi_atxp032_flash_init(void)
{
    uint32_t ui32Status;
    //
    // Configure the MSPI and Flash Device.
    //
    ui32Status = am_devices_mspi_atxp032_init(MSPI_TEST_MODULE, (void*)&MSPI_Flash_Config, &g_FlashHdl, &g_MSPIHdl);
    if (AM_DEVICES_MSPI_ATXP032_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to configure the MSPI and Flash Device correctly!\n");
        return AM_HAL_STATUS_FAIL;
    }
    NVIC_EnableIRQ(mspi_interrupts[MSPI_TEST_MODULE]);

    am_hal_interrupt_master_enable();

    return AM_HAL_STATUS_SUCCESS;
}

static uint32_t mspi_atxp032_flash_erase(uint32_t ui32StartAddr, uint32_t ui32NumBytes)
{
    uint32_t ui32Status;
    uint32_t ui32SectorNum;

    if (ui32StartAddr < ATXP032_FLASH_BASE_ADDR)
    {
        return AM_HAL_STATUS_FAIL;
    }

    ui32SectorNum = (ui32NumBytes + AM_DEVICES_MSPI_ATXP032_SECTOR_SIZE - 1) / AM_DEVICES_MSPI_ATXP032_SECTOR_SIZE;

    ui32StartAddr -= ATXP032_FLASH_BASE_ADDR;
    if (ui32StartAddr & (AM_DEVICES_MSPI_ATXP032_SECTOR_SIZE - 1) &&
       (ui32StartAddr >= ATXP032_FLASH_SIZE))
    {
        return AM_HAL_STATUS_FAIL;
    }

    for (uint32_t i = 0; i < ui32SectorNum; i++)
    {
        ui32Status = am_devices_mspi_atxp032_sector_erase(g_FlashHdl, ui32StartAddr);
        if (AM_DEVICES_MSPI_ATXP032_STATUS_SUCCESS != ui32Status)
        {
            am_util_stdio_printf("Failed to erase Flash Device sector!\n");
            return AM_HAL_STATUS_FAIL;
        }
        ui32StartAddr += AM_DEVICES_MSPI_ATXP032_SECTOR_SIZE;
    }

    return AM_HAL_STATUS_SUCCESS;
}

static uint32_t mspi_atxp032_flash_write(uint8_t *pui8WrBuf, uint32_t ui32WriteAddr, uint32_t ui32NumBytes)
{
    uint32_t ui32Status;

    if (ui32WriteAddr < ATXP032_FLASH_BASE_ADDR)
    {
        return AM_HAL_STATUS_FAIL;
    }

    ui32WriteAddr -= ATXP032_FLASH_BASE_ADDR;
    if (ui32WriteAddr & (AM_DEVICES_MSPI_ATXP032_SECTOR_SIZE - 1) &&
       (ui32WriteAddr + ui32NumBytes) > ATXP032_FLASH_SIZE)
    {
        return AM_HAL_STATUS_FAIL;
    }

    am_util_stdio_printf("Writing %d Bytes to Sector %d\n", ui32NumBytes, ui32WriteAddr >> 16);
    ui32Status = am_devices_mspi_atxp032_write(g_FlashHdl, pui8WrBuf, ui32WriteAddr, ui32NumBytes, true);
    if (AM_DEVICES_MSPI_ATXP032_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to write buffer to Flash Device!\n");
        return AM_HAL_STATUS_FAIL;
    }

    return AM_HAL_STATUS_SUCCESS;
}

static uint32_t mspi_atxp032_flash_read(uint8_t *pui8RdBuf, uint32_t ui32ReadAddr, uint32_t ui32NumBytes)
{
    uint32_t ui32Status;
    //
    // Read the data back into the RX buffer.
    //
    if (ui32ReadAddr < ATXP032_FLASH_BASE_ADDR)
    {
        return AM_HAL_STATUS_FAIL;
    }

    ui32ReadAddr -= ATXP032_FLASH_BASE_ADDR;
    am_util_stdio_printf("Read %d Bytes from Sector %d\n", ui32NumBytes, ui32ReadAddr >> 16);
    ui32Status = am_devices_mspi_atxp032_read(g_FlashHdl, (uint8_t *)pui8RdBuf, ui32ReadAddr, ui32NumBytes, true);
    if (AM_DEVICES_MSPI_ATXP032_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to read buffer to Flash Device!\n");
        return AM_HAL_STATUS_FAIL;
    }

    return AM_HAL_STATUS_SUCCESS;
}

static uint32_t mspi_atxp032_flash_enable_xip(void)
{
    return am_devices_mspi_atxp032_enable_xip(g_FlashHdl);
}

static uint32_t mspi_atxp032_flash_disable_xip(void)
{
    return am_devices_mspi_atxp032_disable_xip(g_FlashHdl);
}

am_ssbl_flash_t mspi_atxp032_flash_info =
{
    .devName = "mspi atxp032 flash",
    .ui32BaseAddr = ATXP032_FLASH_BASE_ADDR,
    .bXip = true,
    .bInternal = false,
    .ui32Size = ATXP032_FLASH_SIZE,
    .flash_erase = mspi_atxp032_flash_erase,
    .flash_enable_xip = mspi_atxp032_flash_enable_xip,
    .flash_disable_xip = mspi_atxp032_flash_disable_xip,
    .flash_init = mspi_atxp032_flash_init,
    .flash_write = mspi_atxp032_flash_write,
    .flash_read = mspi_atxp032_flash_read,
};

