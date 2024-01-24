//*****************************************************************************
//
//! @file apollo3_secbl.c
//!
//! @brief A simple secondary bootloader program example template for Apollo3
//!
//! Purpose: This program is an example template for a secondary bootloader program for Apollo3.
//! It demonstrates how to access info0 key area. It demonstrates how to use the Ambiq SBL OTA
//! framework for customer specific OTAs, e.g. to support external flash, or to support more
//! advanced auth/enc schemes. It demonstrates how to validate & transfer control to the real
//! main program image (assumed to be at address specified by MAIN_PROGRAM_ADDR_IN_FLASH in flash)
//! after locking the info0 area before exiting
//!
//! This example only demonstrates nonsecure boot and nonsecure OTA.
//!
//! Printing takes place over the ITM at 1M Baud.
//!
//! Additional Information:
//! To exercise this program:
//! Flash the main program at 0x10000 (MAIN_PROGRAM_ADDR_IN_FLASH)
//! Link this program at the address suitable for SBL nonsecure (0xC000) or secure (0xC100)
//! configuration
//! To test OTA - construct images using magic numbers in the range matching
//! AM_IMAGE_MAGIC_CUST
//! To test INFO0 key area access - need to keep INFO0->Security->PLONEXIT as 0
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

#include <string.h>
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

#define INFO0_SECURITY_ADDR             (AM_HAL_FLASH_INFO_ADDR + 0x10)
#define INFO0_SECURITY_MASK_PLONEXIT    0x800
#define MAIN_PROGRAM_ADDR_IN_FLASH  0x10000 // This would normally come from info0

//*****************************************************************************
//
//! @brief  Wrapper for main flash page erase. Erases one page
//!
//! @param  addr Any address within the desired flash page
//!
//! @return Returns 0 on success
//
//*****************************************************************************
int
flash_page_erase(uint32_t addr)
{
    uint32_t ui32Page;
    uint32_t ui32Block;
    //
    // Calculate the flash page number.
    //
    ui32Page = AM_HAL_FLASH_ADDR2PAGE((uintptr_t)addr);
    ui32Block = AM_HAL_FLASH_ADDR2INST((uint32_t)addr);
    return am_hal_flash_page_erase(AM_HAL_FLASH_PROGRAM_KEY, ui32Block, ui32Page);
}

//****************************************************************************
// Main
//
//*****************************************************************************
int
main(void)
{
    am_hal_mcuctrl_device_t sDevice;


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
    // Enable debug printf messages using ITM on SWO pin
    //
    am_bsp_itm_printf_enable();

    //
    // Clear the terminal and print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("This is Apollo3 Secondary Bootloader Template Program!\r\n\r\n");

    //
    // Get chip specific info
    //
    am_hal_mcuctrl_info_get(AM_HAL_MCUCTRL_INFO_DEVICEID, &sDevice);

    //
    // If INFO0->Security->PLONEXIT was not set, SBL will keep info0 open upon transferring to main image
    // This allows the secondary bootloader to use the keys in info0 to perform any necessary validations
    // of images and/or OTA upgrades
    //
    if ((AM_REGVAL(INFO0_SECURITY_ADDR) & INFO0_SECURITY_MASK_PLONEXIT) == 0)
    {
        uint32_t *pCustKey = (uint32_t *)AM_REG_INFO0_CUSTOMER_KEY0_ADDR;
        uint32_t *pKek = (uint32_t *)AM_REG_INFO0_CUST_KEK_W0_ADDR;
        uint32_t *pAuthKey = (uint32_t *)AM_REG_INFO0_CUST_AUTH_W0_ADDR;

        //
        // PROTLOCK Open
        // This should also mean that Customer key is accessible
        // All the infospace keys are available to be read - and used for OTA and image verification
        // In this template - we'll just print the values to confirm that we've access to it
        //
        am_util_stdio_printf("Customer Key: 0x%08x-0x%08x-0x%08x-0x%08x\r\n", pCustKey[0], pCustKey[1], pCustKey[2], pCustKey[3]);

        //
        // Print KEK & Auth keys
        //
        uint32_t i = 0;
        am_util_stdio_printf("KEK:\n");

        for (i = 0; i < 8; i++)
        {
            am_util_stdio_printf("Key %2d: 0x%08x-0x%08x-0x%08x-0x%08x\r\n", i + 8, pKek[0], pKek[1], pKek[2], pKek[3]);
            pKek += 4;
        }

        am_util_stdio_printf("Auth:\n");

        for (i = 0; i < 8; i++)
        {
            am_util_stdio_printf("Key %2d: 0x%08x-0x%08x-0x%08x-0x%08x\r\n", i + 8, pAuthKey[0], pAuthKey[1], pAuthKey[2], pAuthKey[3]);
            pAuthKey += 4;
        }
    }

    // Process OTA's
    if ( MCUCTRL->OTAPOINTER_b.OTAVALID )
    {
        uint32_t *pOtaDesc = (uint32_t *)(MCUCTRL->OTAPOINTER & ~0x3);
        uint32_t i = 0;

        uint32_t otaImagePtr = pOtaDesc[0];

        // CAUTION: We can reprogram a bit in flash to 0 only once...so make sure we do not re-clear bits

        am_util_stdio_printf("OTA Available - OTA Desc @0x%x\n", pOtaDesc);
        // Make sure the OTA list is valid
        // Whole OTA list is skipped if it is constructed incorrectly
        while (otaImagePtr != AM_HAL_SECURE_OTA_OTA_LIST_END_MARKER)
        {
            if (AM_HAL_SECURE_OTA_OTA_IS_VALID(otaImagePtr))
            {
                //
                // This template assumes OTA images using the same formal as used by Main.
                // Users can select any image format - as long as the first byte (magic#) is kept the same.
                //
                am_image_hdr_common_t *pComHdr;

                otaImagePtr = AM_HAL_SECURE_OTA_OTA_GET_BLOB_PTR(otaImagePtr);
                pComHdr = (am_image_hdr_common_t *)otaImagePtr;

                //
                // Valid OTA image
                // Make sure the image is contained within flash
                if ((otaImagePtr >= sDevice.ui32FlashSize) ||
                    ((otaImagePtr + pComHdr->w0.s.blobSize) > sDevice.ui32FlashSize))
                {
                    // Invalidate this OTA for subsequent processing
                    am_util_stdio_printf("Found bad OTA pointing to: image address=0x%x, size 0x%x\n", otaImagePtr, pComHdr->w0.s.blobSize);
                    // Indicate Failure
                    am_hal_flash_clear_bits(AM_HAL_FLASH_PROGRAM_KEY, &pOtaDesc[i], AM_HAL_SECURE_OTA_OTA_DONE_FAILURE_CLRMASK);

                }
                else
                {
                    if (AM_IMAGE_MAGIC_CUST(pComHdr->w0.s.magicNum))
                    {
                        //
                        // Process OTA
                        // We can perform any necessary verification/decryption here before installing a valid OTA image
                        // install
                        // Operate only in flash page multiples
                        //
                        uint32_t size = pComHdr->w0.s.blobSize - sizeof(am_thirdparty_image_hdr_t);
                        uint32_t numFlashPage = (size + AM_HAL_FLASH_PAGE_SIZE - 1) / AM_HAL_FLASH_PAGE_SIZE;
                        uint32_t *pDst = (uint32_t *)AM_IMAGE_GET_LOADADDR(pComHdr);
                        uint32_t tempBuf[AM_HAL_FLASH_PAGE_SIZE / 4];
                        uint32_t *pSrc = (uint32_t *)((am_thirdparty_image_hdr_t *)pComHdr + 1);

                        am_util_stdio_printf("Found OTA @ 0x%x magic 0x%x - size 0x%x to be installed at 0x%x\n", otaImagePtr, pComHdr->w0.s.magicNum, size, pDst);
                        for (uint32_t i = 0; i < numFlashPage; i++)
                        {
                            memcpy(tempBuf, pSrc, AM_HAL_FLASH_PAGE_SIZE);
                            flash_page_erase((uint32_t)pDst);
                            am_hal_flash_program_main(AM_HAL_FLASH_PROGRAM_KEY, tempBuf, pDst, AM_HAL_FLASH_PAGE_SIZE / 4);
                            pSrc += AM_HAL_FLASH_PAGE_SIZE / 4;
                            pDst += AM_HAL_FLASH_PAGE_SIZE / 4;
                        }

                        //
                        // Indicate Success
                        //
                        am_hal_flash_clear_bits(AM_HAL_FLASH_PROGRAM_KEY, &pOtaDesc[i], AM_HAL_SECURE_OTA_OTA_DONE_SUCCESS_CLRMASK);
                    }
                    else
                    {
                        //
                        // unknown OTA
                        //
                        am_util_stdio_printf("Found unexpected OTA\n");

                        //
                        // Indicate Failure
                        //
                        am_hal_flash_clear_bits(AM_HAL_FLASH_PROGRAM_KEY, &pOtaDesc[i], AM_HAL_SECURE_OTA_OTA_DONE_FAILURE_CLRMASK);
                    }
                }
            }
            else
            {
                //
                // This OTA has already been invalidated...Skip
                //
            }

            if (i++ == AM_HAL_SECURE_OTA_MAX_OTA)
            {
                am_util_stdio_printf("Exceeded maximum OTAs\n", i);
                break;
            }

            if ((uint32_t)&pOtaDesc[i] >= sDevice.ui32FlashSize)
            {
                am_util_stdio_printf("Found Invalid OTA pointer 0x%x\n", (uint32_t)&pOtaDesc[i]);
                break;
            }
            else
            {
                otaImagePtr = pOtaDesc[i];
            }
        }
    }

    //
    // Clear OTA_POINTER
    //
    MCUCTRL->OTAPOINTER = 0;

    //
    // Validate main image
    // This is only a place holder - Assumes raw main image @ 0x10000
    // Users can select any image format
    // Depending on the custom image format - more elaborate validation (including signature verification) can be done
    //
    uint32_t imageAddr = MAIN_PROGRAM_ADDR_IN_FLASH;
    uint32_t sp = *((uint32_t *)imageAddr);
    uint32_t reset = *((uint32_t *)(imageAddr + 4));
    uint32_t *pVtor = 0;

    //
    // Make sure the SP & Reset vector are sane
    // Validate the Stack Pointer
    // Validate the reset vector
    //
    if ((sp < SRAM_BASEADDR)                            || \
        (sp >= (SRAM_BASEADDR + sDevice.ui32SRAMSize))  || \
        (reset < imageAddr)                             || \
        (reset >= sDevice.ui32FlashSize))
    {
        am_util_stdio_printf("Invalid main image\n");
    }
    else
    {
        am_util_stdio_printf("Found valid main image - SP:0x%x RV:0x%x\n", sp, reset);
        pVtor = (uint32_t *)imageAddr;
        am_util_stdio_printf("Will transfer control over to this image after locking things down\n\n\n\n");
    }

    //
    // Lock the assets (if needed) and give control to main
    //
    am_hal_bootloader_exit(pVtor);

    while(1);
}
