//*****************************************************************************
//
//! @file apollo3_secbl.c
//!
//! @brief A simple secondary bootloader program example for Apollo3p
//!
//! Purpose: This program is an example template for a secondary bootloader program for Apollo3p.
//! It demonstrates how to access info0 key area. It demonstrates how to use the Ambiq SBL OTA
//! framework for customer specific OTAs, e.g. to support external flash, or to support more
//! advanced auth/enc schemes. It demonstrates how to validate & transfer control to the real
//! main program image (assumed to be at address specified by MAIN_PROGRAM_ADDR_IN_FLASH in flash)
//! after locking the info0 area before exiting
//!
//! Printing takes place over the ITM at 1M Baud.
//!
//! Additional Information:
//! To exercise this program:
//! Flash the main program at 0x80000  if executing in the internal flash
//! Flash the main program at 0x40000000 if excuting in the MSPI1 XIP flash
//! Link this program at the address suitable for SBL nonsecure (0xC000) or secure (0xC100)
//! configuration
//! To test OTA - construct images using magic numbers in the range matching
//! AM_IMAGE_MAGIC_CUST
//! To test INFO0 key area access - need to keep INFO0->Security->PLONEXIT as 0
//! apollo3p_evb + cygnus
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
#include "apollo3_secbl.h"

uint8_t tempBuf[TEMP_BUF_SIZE];

//
// Program an image from one place to another - doing optional decryption along with
// The function expects the image to be in the common format defined by Ambiq
//
bool
install_ota_image(am_ssbl_flash_t *pSrcFlash, am_ssbl_flash_t *pDstFlash, uint32_t pSrc, uint32_t pDst, uint32_t ui32NumBytes, ssbl_decrypt_info_t *pDecryptInfo)
{
    // clearSize = offsetof(am_image_hdr_common_t, signatureClear)
    uint32_t clearSize = pDecryptInfo->clearSize;

    //
    // Erase the entire destination flash area firstly
    //
    if (!pDstFlash->flash_erase || pDstFlash->flash_erase(pDst, ui32NumBytes))
    {
        return false;
    }

    if (pDecryptInfo->bDecrypt)
    {
        ssbl_crypt_init(pDecryptInfo->key);
    }

    while (ui32NumBytes)
    {
        uint32_t blockSize = (ui32NumBytes > sizeof(tempBuf)) ? sizeof(tempBuf): ui32NumBytes;
        if (pSrcFlash->flash_read(tempBuf, pSrc, blockSize))
        {
            return false;
        }

        if (pDecryptInfo->bDecrypt)
        {
            // Do in place decryption
            ssbl_crypt_decrypt((uint8_t *)tempBuf + clearSize, (uint8_t *)tempBuf + clearSize,
                         blockSize - clearSize, pDecryptInfo->iv);
            clearSize = 0;
        }

        if (pDstFlash->flash_write(tempBuf, pDst, blockSize))
        {
            return false;
        }

        pSrc += sizeof(tempBuf);
        pDst += sizeof(tempBuf);
        ui32NumBytes -= blockSize;
    }

    return true;
}

//
// Process OTA descriptor list
//
bool
process_ota_desc_list(uint32_t *pOtaDesc)
{
    static uint32_t ui32RecursiveCount = 0;
    uint32_t i = 0;
    bool bOtaSuccess = false;
    bool bNoRollback = false;
    am_image_hdr_common_t *pComHdr, ComHdr;
    am_main_image_hdr_t mainHdr;
    am_ssbl_flash_t *pSrcFlash;
    am_ssbl_flash_t *pDstFlash;
    ssbl_decrypt_info_t decryptInfo;
    uint32_t size = 0;
    uint32_t pSrc;
    uint32_t pDst;

    //
    // Prevent potential dead loop by wrong ota list or attack
    //
    if (ui32RecursiveCount++ > 1)
    {
        return false;
    }

    uint32_t otaImagePtr = pOtaDesc[0];

    // CAUTION: We can reprogram a bit in flash to 0 only once...so make sure we do not re-clear bits

    while (otaImagePtr != AM_HAL_SECURE_OTA_OTA_LIST_END_MARKER)
    {
        if (!AM_HAL_SECURE_OTA_OTA_IS_VALID(otaImagePtr))
        {
            // This OTA has already been invalidated...Skip
            goto move_to_next_ota_image;
        }

        //
        // This template assumes OTA images using the same formal as used by Main.
        // Users can select any image format - as long as the first byte (magic#) is kept the same.
        //

        otaImagePtr = AM_HAL_SECURE_OTA_OTA_GET_BLOB_PTR(otaImagePtr);

        //
        // Only check if the image header is within the flash
        // since we don't know the length of image blob yet
        //
        if (!ssbl_find_flash_of_image(otaImagePtr, sizeof(am_image_hdr_common_t), &pSrcFlash))
        {
            goto invalidate_ota_image;
        }

        pComHdr = &ComHdr;

        if (pSrcFlash->flash_read((uint8_t *)pComHdr, otaImagePtr, sizeof(am_image_hdr_common_t)) != AM_HAL_STATUS_SUCCESS)
        {
            goto invalidate_ota_image;
        }

        //
        // Check if whole image blob is within the flash
        //
        if (!ssbl_find_flash_of_image(otaImagePtr, pComHdr->w0.s.blobSize, &pSrcFlash))
        {
            // Invalidate this OTA for subsequent processing
            am_util_stdio_printf("Found bad OTA pointing to: image address=0x%x, size 0x%x\n", otaImagePtr, pComHdr->w0.s.blobSize);
            goto invalidate_ota_image;
        }

        memset((void *)&decryptInfo, 0x0, sizeof(decryptInfo));

        switch (pComHdr->w0.s.magicNum)
        {
           case MAGIC_CUST_PROP:
                    // customer specific image formatting and processing
                    bOtaSuccess = process_cust_prop_ota(otaImagePtr);
                    break;

            case MAGIC_CUST_SECOTADESC:
                // Special case
                // The OTA image blob(s) is in external memory
                // This OTA blob just contains a list of the images
                am_util_stdio_printf("Cust Specific OTA Available - OTA Desc @0x%x\n", otaImagePtr);
                bOtaSuccess = process_ota_desc_list(&((ssbl_cust_otadesc_t *)otaImagePtr)->ota[0]);
                break;

#ifndef SECURE_BOOT
            case AM_IMAGE_MAGIC_NONSECURE:
                // We only install the blob inside
                size = pComHdr->w0.s.blobSize - sizeof(am_thirdparty_image_hdr_t);
                pSrc = otaImagePtr + sizeof(am_thirdparty_image_hdr_t);

                //
                // Process OTA
                am_util_stdio_printf("Found OTA @ 0x%x magic 0x%x - size 0x%x\n", otaImagePtr, pComHdr->w0.s.magicNum, size);
                // We can perform any necessary verification/decryption here before installing a valid OTA image
                bOtaSuccess = ssbl_validate_ota_image(otaImagePtr, pSrcFlash, &decryptInfo, &mainHdr);
                break;
#else
            case AM_IMAGE_MAGIC_CHILD:
                // We install the whole blob
                size = pComHdr->w0.s.blobSize;
                pSrc = otaImagePtr;

                //
                // Process OTA
                am_util_stdio_printf("Found OTA @ 0x%x magic 0x%x - size 0x%x\n", otaImagePtr, pComHdr->w0.s.magicNum, size);
                // We can perform any necessary verification/decryption here before installing a valid OTA image
                bOtaSuccess = ssbl_validate_ota_image(otaImagePtr, pSrcFlash, &decryptInfo, &mainHdr);
                break;
            case AM_IMAGE_MAGIC_MAIN:
                // We install the whole blob
                size = pComHdr->w0.s.blobSize;
                pSrc = otaImagePtr;

                //
                // Process OTA
                am_util_stdio_printf("Found OTA @ 0x%x magic 0x%x - size 0x%x\n", otaImagePtr, pComHdr->w0.s.magicNum, size);
                // We can perform any necessary verification/decryption here before installing a valid OTA image
                if (ssbl_validate_ota_image(otaImagePtr, pSrcFlash, &decryptInfo, &mainHdr))
                {
                    // Need to validate all the child images as well
                    bOtaSuccess = ssbl_validate_child_images(mainHdr.childPtr);
                }
                break;
#endif
        }

        if (!bOtaSuccess || !size )
        {
            goto invalidate_ota_image;
        }

        pDst = AM_IMAGE_GET_LOADADDR(&mainHdr);
        am_util_stdio_printf("To be installed at 0x%x\n", pDst);

        //
        // Install ota image
        //
        if (!ssbl_find_flash_of_image(pDst, size, &pDstFlash) || !pDstFlash->bXip)
        {
            bOtaSuccess = false;
            goto invalidate_ota_image;
        }

#ifdef SECURE_BOOT
        bNoRollback = ssbl_info0_get_NoRollback();
        if (bNoRollback)
        {
            am_image_hdr_common_t curImage;

            if (pDstFlash->flash_read((void *)&curImage, pDst, sizeof(curImage)) != AM_HAL_STATUS_SUCCESS)
            {
                goto invalidate_ota_image;
            }

            if (curImage.w0.s.magicNum == mainHdr.common.w0.s.magicNum)
            {
                // We assume that in this case there is an existing image - on which version check can be performed
                if (curImage.versionKeyWord.s.version >= mainHdr.common.versionKeyWord.s.version)
                {
                    am_util_stdio_printf("Version rollback check failed\n");
                    bOtaSuccess = false;
                    goto invalidate_ota_image;
                }
            }
        }
#endif

        if (!install_ota_image(pSrcFlash, pDstFlash, pSrc, pDst, size, &decryptInfo))
        {
            am_util_stdio_printf("Failed to install OTA @ 0x%x\n", pDst);
            bOtaSuccess = false;
        }
        else
        {
            am_util_stdio_printf("Successfully installed OTA @ 0x%x\n", pDst);
        }

invalidate_ota_image:
        // Provide OTA Feedback to application
        am_hal_flash_clear_bits(AM_HAL_FLASH_PROGRAM_KEY, &pOtaDesc[i],
            bOtaSuccess ? AM_HAL_SECURE_OTA_OTA_DONE_SUCCESS_CLRMASK : AM_HAL_SECURE_OTA_OTA_DONE_FAILURE_CLRMASK);

move_to_next_ota_image:
        if (i++ == AM_HAL_SECURE_OTA_MAX_OTA)
        {
            am_util_stdio_printf("Exceeded maximum OTAs\n", i);
            return false;
        }
        otaImagePtr = pOtaDesc[i];
    }

    return true;
}

void dump_info_keys(void)
{
#ifdef DUMP_INFO0_KEYS
    //
    // If INFO0->Security->PLONEXIT was not set, SBL will keep info0 open upon transferring to main image
    // This allows the secondary bootloader to use the keys in info0 to perform any necessary validations
    // of images and/or OTA upgrades
    //
    // The code segment below is just demonstrating a way to access those keys
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
#endif
}

//****************************************************************************
// Main
//
//*****************************************************************************
int
main(void)
{
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
    am_util_stdio_printf("This is Apollo3 Secondary Bootloader Reference Example!\n\n\n");

    //
    // Add internal & external flash interfaces
    //
    ssbl_flash_init();

    if (ssbl_flash_add(&mspi_atxp032_flash_info) == SSBL_FLASH_INIT_FAIL)
    {
        am_util_stdio_printf("atxp032 flash initilization failed!\n");
        while (1);
    }

    ssbl_secure_init();

    dump_info_keys();

    // Process OTA's
    // This reference example builds on top of the OTA framework already used by Ambiq SBL
    // Optionally - OEM may define their own means to communicate OTA information
    if ( MCUCTRL->OTAPOINTER_b.OTAVALID )
    {
        uint32_t *pOtaDesc = (uint32_t *)(MCUCTRL->OTAPOINTER & ~0x3);
        process_ota_desc_list(pOtaDesc);
    }

    //
    // Clear OTA_POINTER
    //
    MCUCTRL->OTAPOINTER_b.OTAVALID = 0;

    uint32_t *pVtor;
    //
    // Validate main image
    //
    ssbl_validate_main_image(&pVtor);

    //
    // Lock the assets (if needed) and give control to main
    //
    am_hal_bootloader_exit(pVtor);

    while(1);
}
