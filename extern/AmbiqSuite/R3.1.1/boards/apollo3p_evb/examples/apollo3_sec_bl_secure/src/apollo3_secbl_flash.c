//*****************************************************************************
//!
//! @file apollo3_secbl_flash.c
//!
//! @brief apollo3 SSBL flash interface management & internal flash access
//!        function implementation
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

#include <string.h>
#include "am_mcu_apollo.h"
#include "am_util.h"

#include "apollo3_secbl_flash.h"

static uint32_t internal_flash_erase(uint32_t ui32Addr, uint32_t ui32NumBytes)
{
    uint32_t ui32Page;
    uint32_t ui32Block;

    uint32_t numFlashPage = (ui32NumBytes + AM_HAL_FLASH_PAGE_SIZE - 1) / AM_HAL_FLASH_PAGE_SIZE;

    if (ui32Addr & (AM_HAL_FLASH_PAGE_SIZE - 1))
    {
        // Starting address needs to be aligned
        return AM_HAL_STATUS_FAIL;
    }

    for (uint32_t i = 0; i < numFlashPage; i++)
    {
        //
        // Calculate the flash page number.
        //
        ui32Block = AM_HAL_FLASH_ADDR2INST((uint32_t)ui32Addr);
        ui32Page = AM_HAL_FLASH_ADDR2PAGE((uintptr_t)ui32Addr);

        if (am_hal_flash_page_erase(AM_HAL_FLASH_PROGRAM_KEY, ui32Block, ui32Page) != AM_HAL_STATUS_SUCCESS)
        {
            return AM_HAL_STATUS_FAIL;
        }

        ui32Addr += AM_HAL_FLASH_PAGE_SIZE;
    }

    return AM_HAL_STATUS_SUCCESS;
}

static uint32_t internal_flash_write(uint8_t *pui8WrBuf, uint32_t ui32WriteAddr, uint32_t ui32NumBytes)
{
    // Operate only in flash page multiples
    uint32_t numFlashPage = (ui32NumBytes + AM_HAL_FLASH_PAGE_SIZE - 1) / AM_HAL_FLASH_PAGE_SIZE;

    if (ui32WriteAddr & (AM_HAL_FLASH_PAGE_SIZE - 1))
    {
        // Starting address needs to be flash page aligned
        return AM_HAL_STATUS_FAIL;
    }

    for (uint32_t i = 0; i < numFlashPage; i++)
    {
        am_hal_flash_program_main(AM_HAL_FLASH_PROGRAM_KEY, (uint32_t *)pui8WrBuf, (uint32_t *)ui32WriteAddr, AM_HAL_FLASH_PAGE_SIZE / 4);
        pui8WrBuf     += AM_HAL_FLASH_PAGE_SIZE;
        ui32WriteAddr += AM_HAL_FLASH_PAGE_SIZE;
    }
    return AM_HAL_STATUS_SUCCESS;
}

static uint32_t internal_flash_read(uint8_t *pui8RdBuf, uint32_t ui32ReadAddress, uint32_t ui32NumBytes)
{
    memcpy((void *)pui8RdBuf, (void *)ui32ReadAddress, ui32NumBytes);
    return AM_HAL_STATUS_SUCCESS;
}

static uint32_t internal_flash_protect(uint32_t ui32StartAddr, uint32_t ui32Size, uint32_t volatile *pProtRegAddr)
{
    if (MCUCTRL->BOOTLOADER_b.PROTLOCK)
    {
        // Cannot apply further protections
        return AM_HAL_STATUS_FAIL;
    }

    if (ui32StartAddr & (AM_HAL_FLASH_PAGE_SIZE - 1))
    {
        // Starting address needs to be aligned
        return AM_HAL_STATUS_FAIL;
    }

    uint32_t startChunk = (ui32StartAddr) >> 14; // 16384 (flash block protect size)
    uint32_t endChunk   = (ui32StartAddr + ui32Size - 1) >> 14; // 6384 (flash block protect size)
    for ( int i = startChunk; i <= endChunk; i++ )
    {
        pProtRegAddr[i / 32] &= ~(1 << (i % 32));
    }
    return AM_HAL_STATUS_SUCCESS;
}

static uint32_t internal_flash_write_protect(uint32_t ui32StartAddr, uint32_t ui32Size)
{
    return internal_flash_protect(ui32StartAddr, ui32Size, &MCUCTRL->FLASHWPROT0);
}

static uint32_t internal_flash_copy_protect(uint32_t ui32StartAddr, uint32_t ui32Size)
{
    return internal_flash_protect(ui32StartAddr, ui32Size, &MCUCTRL->FLASHRPROT0);
}

static am_ssbl_flash_t internal_flash_info =
{
    .devName = "ap3 internal flash",
    .ui32BaseAddr = 0,
    .bXip = true,
    .bInternal = true,
    .flash_erase = internal_flash_erase,
    .flash_write = internal_flash_write,
    .flash_read = internal_flash_read,
    .flash_write_protect = internal_flash_write_protect,
    .flash_copy_protect = internal_flash_copy_protect,
};

static uint32_t gNumFlashDev = 0;
static am_ssbl_flash_t *gpFlashDev[MAX_FLASH_DEV];

//
// Lookup for the address range in flash address map for the device
// Return the driver for the corresponding memory
// returns false if invalid flash memory
//
bool ssbl_find_flash_of_image(uint32_t otaImagePtr, uint32_t ui32Size, am_ssbl_flash_t **ppFlashIf)
{
    bool bRet = false;
    for (uint32_t i = 0; i < gNumFlashDev; i++)
    {
        if ((otaImagePtr >= gpFlashDev[i]->ui32BaseAddr) &&
            ((otaImagePtr + ui32Size) < (gpFlashDev[i]->ui32BaseAddr + gpFlashDev[i]->ui32Size)))
        {
            if (ppFlashIf)
            {
                *ppFlashIf = gpFlashDev[i];
            }
            bRet = true;
            break;
        }
    }
    return bRet;
}

void ssbl_flash_init(void)
{
    //
    // Add the internal flash firstly
    //
    am_hal_mcuctrl_device_t sDevice;
    am_hal_mcuctrl_info_get(AM_HAL_MCUCTRL_INFO_DEVICEID, &sDevice);
    internal_flash_info.ui32Size = sDevice.ui32FlashSize;

    gpFlashDev[gNumFlashDev++] = &internal_flash_info;
}

uint32_t ssbl_flash_add(am_ssbl_flash_t *pFlashIf)
{
    if (gNumFlashDev > MAX_FLASH_DEV)
    {
        am_util_stdio_printf("exceed the maximum flash device number %d\n", MAX_FLASH_DEV);
        return SSBL_FLASH_INIT_FAIL;
    }

    if (!pFlashIf)
    {
        return SSBL_FLASH_INIT_FAIL;
    }

    if (pFlashIf->flash_init)
    {
        if (pFlashIf->flash_init() != AM_HAL_STATUS_SUCCESS)
        {
            return SSBL_FLASH_INIT_FAIL;
        }
    }

    if (pFlashIf->flash_disable_xip)
    {
        if (pFlashIf->flash_disable_xip() != AM_HAL_STATUS_SUCCESS)
        {
            return SSBL_FLASH_INIT_FAIL;
        }
    }

    gpFlashDev[gNumFlashDev++] = pFlashIf;

    return SSBL_FLASH_INIT_SUCCEED;
}

