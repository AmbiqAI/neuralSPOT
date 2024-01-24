//*****************************************************************************
//
//! @file apollo3_secbl_image.c
//!
//! @brief ota image and boot images' validation functions
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
#include "apollo3_secbl_image.h"
#include "apollo3_secbl_info0.h"

#ifdef SECURE_BOOT

//
// Boot image validation function
//
bool ssbl_validate_boot_image(am_image_hdr_common_t *pComHdr, am_ssbl_flash_t *pFlash)
{
    uint8_t authKey[DIGEST_SIZE];
    uint8_t digest[DIGEST_SIZE];
    uint32_t crc;

    bool bAuthEnforce = ssbl_info0_get_AuthEnforce();

    if (bAuthEnforce && !pComHdr->w2.s.authBoot)
    {
        // Security policy violation
        am_util_stdio_printf("No Boot Authentication for image\n");
        return false;
    }

    // Do signature verification & CRC, if enabled
    if (pComHdr->w2.s.authBoot)
    {
        // Check Auth Key
        if ((pComHdr->w2.s.authAlgo != AUTH_ALGO) || !ssbl_info0_get_auth_key(pComHdr->w2.s.authKeyIdx, authKey))
        {
            return false;
        }
        ssbl_auth_start(authKey);
        ssbl_auth_update(((uint32_t)pComHdr + offsetof(am_image_hdr_common_t, addrWord)),
                    pComHdr->w0.s.blobSize - offsetof(am_image_hdr_common_t, addrWord));
        ssbl_auth_finish(digest);
        if (!ssbl_verify_signature((uint8_t *)&pComHdr->signatureClear, digest, authKey))
        {
            am_util_stdio_printf("Boot Authentication failed for image\n");
            return false;
        }
    }

    if (pComHdr->w2.s.crcBoot)
    {
        uint32_t crcBytes;
        uint32_t crcSize = pComHdr->w0.s.blobSize - offsetof(am_image_hdr_common_t, w2);
        uint8_t  *pCrcStartAddr = (uint8_t *)&(pComHdr->w2);

        am_hal_crc32_init();

        crc = 0;

        if (pFlash->bInternal)
        {
            if (am_hal_crc32_accum((uint32_t)pCrcStartAddr, crcSize, &crc) != AM_HAL_STATUS_SUCCESS)
            {
                am_util_stdio_printf("CRC calculation failed for image\n");
                return false;
            }
        }
        else
        {
            while (crcSize > 0)
            {
                crcBytes = crcSize > TEMP_BUF_SIZE ? TEMP_BUF_SIZE : crcSize;
                memcpy(tempBuf, pCrcStartAddr, crcBytes);
                if (am_hal_crc32_accum((uint32_t)tempBuf, crcBytes, &crc) != AM_HAL_STATUS_SUCCESS)
                {
                    am_util_stdio_printf("CRC calculation failed for image\n");
                    return false;
                }
                crcSize -= crcBytes;
                pCrcStartAddr += crcBytes;
            }
        }

        if (crc != pComHdr->crc)
        {
            am_util_stdio_printf("CRC verification failed for image\n");
            return false;
        }
    }
    return true;
}

#endif

//
// Ota image validation function
// If Encrypted, it populates the pDecryptInfo with required information for decryption later
// Also - it copies the first 256B of the image in clear to the local SRAM provided (pMainHdr)
bool ssbl_validate_ota_image(uint32_t ui32OtaImageAddr, am_ssbl_flash_t *pFlash, ssbl_decrypt_info_t *pDecryptInfo, am_main_image_hdr_t *pMainHdr)
{
    uint8_t authKey[DIGEST_SIZE];
    uint8_t digest[DIGEST_SIZE];
    uint8_t iv[CRYPT_SIZE];
    uint32_t crc;
    uint32_t bytesRemain;
    uint32_t pSrc;
    uint8_t *pBuf;

    // Need to enforce security policy
    // e.g. this is where one can mandate authentication
    bool bAuthEnforce = ssbl_info0_get_AuthEnforce();
    bool bEncEnforce = ssbl_info0_get_EncEnforce();
    am_image_hdr_common_t *pComHdr = (am_image_hdr_common_t *)pMainHdr;

    // Read the headers
    if (pFlash->flash_read((uint8_t *)pComHdr, ui32OtaImageAddr, sizeof(*pMainHdr)) != AM_HAL_STATUS_SUCCESS)
    {
        return false;
    }

    if (bAuthEnforce && !pComHdr->w2.s.authInstall)
    {
        // Security policy violation
        am_util_stdio_printf("No Install Authentication for image\n");
        return false;
    }

#ifdef SECURE_BOOT
    if (bAuthEnforce && !pComHdr->w2.s.authBoot)
    {
        // Security policy violation
        am_util_stdio_printf("No Boot Authentication for image\n");
        return false;
    }
#endif

    if (bEncEnforce && !pComHdr->w0.s.encrypted)
    {
        // Security policy violation
        am_util_stdio_printf("No Encryption for image\n");
        return false;
    }

    // If enabled - check for install authentication
    // Thereafter check for boot authentication & CRC after decryption
    if (pComHdr->w2.s.authInstall || pComHdr->w2.s.authBoot)
    {
        // Check Auth Key
        if ((pComHdr->w2.s.authAlgo != AUTH_ALGO) || !ssbl_info0_get_auth_key(pComHdr->w2.s.authKeyIdx, authKey))
        {
            return false;
        }
    }

    if (pComHdr->w2.s.authInstall)
    {
        // Signature is computed over rest of the image
        ssbl_auth_start(authKey);
        ssbl_auth_update(((uint32_t)pComHdr + offsetof(am_image_hdr_common_t, iv)),
                    sizeof(*pMainHdr) - offsetof(am_image_hdr_common_t, iv));
        // Now iterate over the image body
        bytesRemain = pComHdr->w0.s.blobSize - sizeof(*pMainHdr);
        pSrc = ui32OtaImageAddr + sizeof(*pMainHdr);
        pBuf = tempBuf;
        while (bytesRemain)
        {
            uint32_t bytesRead = (bytesRemain > sizeof(tempBuf) ? sizeof(tempBuf) : bytesRemain);
            // Read
            pFlash->flash_read(tempBuf, pSrc, bytesRead);
            ssbl_auth_update((uint32_t)pBuf, bytesRead);
            bytesRemain -= bytesRead;
            pSrc += bytesRead;
        }
        ssbl_auth_finish(digest);

        if (!ssbl_verify_signature((uint8_t *)&pComHdr->signature, digest, authKey))
        {
            am_util_stdio_printf("Install Authentication failed for image\n");
            return false;
        }
    }

    if (!pComHdr->w2.s.crcBoot && !pComHdr->w2.s.authBoot)
    {
        // Nothing more needed
        return true;
    }

    // Now we know we need to iterate over whole blob
    if (pComHdr->w0.s.encrypted)
    {
        uint8_t kek[CRYPT_SIZE];
        if ((pComHdr->w2.s.encAlgo != CRYPT_ALGO) || !ssbl_info0_get_kek(pComHdr->w2.s.keyIdx, kek))
        {
            return false;
        }
        // Decrypt the key first
        memset(iv, 0, CRYPT_SIZE);
        ssbl_crypt_init((uint8_t *)kek);
        ssbl_crypt_decrypt((uint8_t *)pComHdr->kek, pDecryptInfo->key, CRYPT_SIZE, (uint8_t *)iv);
        // Now decrypt the image
        pDecryptInfo->bDecrypt = true;
        pDecryptInfo->clearSize =
            (pComHdr->w0.s.magicNum == AM_IMAGE_MAGIC_NONSECURE) ? 0 : offsetof(am_image_hdr_common_t, signatureClear);

        memcpy(iv, IMG_CRYPT_IV(pComHdr), CRYPT_SIZE);
        memcpy(pDecryptInfo->iv, IMG_CRYPT_IV(pComHdr), CRYPT_SIZE);
        ssbl_crypt_init(pDecryptInfo->key);
        // Do in place decryption
        ssbl_crypt_decrypt((uint8_t *)pComHdr->signatureClear, (uint8_t *)pComHdr->signatureClear,
                     sizeof (*pMainHdr) - offsetof(am_image_hdr_common_t, signatureClear), iv);
    }

    if (pComHdr->w2.s.crcBoot)
    {
        am_hal_crc32_init();
        am_hal_crc32_accum((uint32_t)&(pComHdr->w2), (sizeof (*pMainHdr) - offsetof(am_image_hdr_common_t, w2)), &crc);
    }

    if (pComHdr->w2.s.authBoot)
    {
        ssbl_auth_start(authKey);
        ssbl_auth_update((uint32_t)&pComHdr->addrWord, sizeof (*pMainHdr) - offsetof(am_image_hdr_common_t, addrWord));
    }

    // Now iterate over the image body
    bytesRemain = pComHdr->w0.s.blobSize - sizeof(*pMainHdr);
    pSrc = ui32OtaImageAddr + sizeof(*pMainHdr);
    pBuf = tempBuf;
    while (bytesRemain)
    {
        uint32_t bytesRead = (bytesRemain > sizeof(tempBuf) ? sizeof(tempBuf) : bytesRemain);
        // Read
        pFlash->flash_read(tempBuf, pSrc, bytesRead);
        if (pComHdr->w0.s.encrypted)
        {
            ssbl_crypt_decrypt((uint8_t *)pBuf, (uint8_t *)pBuf,
                          bytesRead, iv);
        }
        if (pComHdr->w2.s.authBoot)
        {
            ssbl_auth_update((uint32_t)pBuf, bytesRead);
        }
        if (pComHdr->w2.s.crcBoot)
        {
            am_hal_crc32_accum((uint32_t)pBuf, bytesRead, &crc);
        }
        bytesRemain -= bytesRead;
        pSrc += bytesRead;
    }

    if (pComHdr->w2.s.authBoot)
    {
        ssbl_auth_finish(digest);

        if (!ssbl_verify_signature((uint8_t *)&pComHdr->signatureClear, digest, authKey))
        {
            am_util_stdio_printf("Install Authentication failed for image\n");
            return false;
        }
    }

    if (pComHdr->w2.s.crcBoot & (crc != pComHdr->crc))
    {
        am_util_stdio_printf("CRC verification failed for image\n");
        return false;
    }

    return true;
}


#ifdef SECURE_BOOT

//
// Ensure the child images referenced for the main image are valid
//
bool ssbl_validate_child_images(uint32_t *pChildPtr)
{
    uint32_t numChild = 0;
    am_ssbl_flash_t *pFlash;
    // Check for child images
    while ((numChild < AM_IMAGE_MAX_CHILD_IMAGE) && (*pChildPtr != 0xFFFFFFFF))
    {
        am_image_hdr_common_t *pHdr = (am_image_hdr_common_t *)pChildPtr;

        if (!ssbl_find_flash_of_image(*pChildPtr, pHdr->w0.s.blobSize, &pFlash) || !pFlash->bXip)
        {
            am_util_stdio_printf("Invalid child image\n");
            return false;
        }
        if (!ssbl_validate_boot_image(pHdr, pFlash))
        {
            am_util_stdio_printf("Validation failed for child image\n");
            return false;
        }
        numChild++;
        pChildPtr++;
    }
    return true;
}
#endif

//
// Verify the main image & any linked child images
// Also applies necessary protections on successful validation
// Returns the Start of the image
//
uint32_t ssbl_validate_main_image(uint32_t **ppVtor)
{
    uint32_t imageAddr;
    uint32_t sp;
    uint32_t reset;
    am_ssbl_flash_t *pFlash;
    am_image_hdr_common_t *pComHdr;

    imageAddr = MAIN_PROGRAM_ADDR_IN_FLASH;
    pComHdr = (am_image_hdr_common_t *)imageAddr;

    if (!ssbl_find_flash_of_image(imageAddr, 0x0, &pFlash) || !pFlash->bXip)
    {
        am_util_stdio_printf("Invalid main image\n");
        return AM_HAL_STATUS_FAIL;
    }

    //
    // Enable the flash XIP mode
    //
    if (pFlash->bXip && pFlash->flash_enable_xip)
    {
        if (pFlash->flash_enable_xip() != AM_HAL_STATUS_SUCCESS)
        {
            return AM_HAL_STATUS_FAIL;
        }
    }

    *ppVtor = 0;

#ifndef SECURE_BOOT
    reset = *((uint32_t *)(imageAddr + 4));
    if (!ssbl_find_flash_of_image(imageAddr, (reset - imageAddr + 4), &pFlash))
    {
        am_util_stdio_printf("Invalid main image\n");
        return AM_HAL_STATUS_FAIL;
    }
#else
    pComHdr = (am_image_hdr_common_t *)imageAddr;

    if (!ssbl_validate_boot_image(pComHdr, pFlash))
    {
        am_util_stdio_printf("Validation failed for main image\n");
        return AM_HAL_STATUS_FAIL;
    }
    // Check for child images
    am_main_image_hdr_t *pMainHdr = (am_main_image_hdr_t *)imageAddr;
    uint32_t *pChildPtr = pMainHdr->childPtr;
    uint32_t numChild = 0;
    while ((numChild < AM_IMAGE_MAX_CHILD_IMAGE) && (*pChildPtr != 0xFFFFFFFF))
    {
        am_image_hdr_common_t *pHdr = (am_image_hdr_common_t *)pChildPtr;

        if (!ssbl_find_flash_of_image((uint32_t)pChildPtr, pHdr->w0.s.blobSize, &pFlash) || !pFlash->bXip)
        {
            am_util_stdio_printf("Invalid child image\n");
            return AM_HAL_STATUS_FAIL;
        }

        if (!ssbl_validate_boot_image(pHdr, pFlash))
        {
            am_util_stdio_printf("Validation failed for child image\n");
            return AM_HAL_STATUS_FAIL;
        }

        numChild++;
        pChildPtr++;
    }
    imageAddr = (uint32_t)(pMainHdr + 1);
    reset = *((uint32_t *)(imageAddr + 4));
    if ((reset + 4) > (imageAddr + pComHdr->w0.s.blobSize - sizeof(*pMainHdr)))
    {
        am_util_stdio_printf("Invalid main image\n");
        return AM_HAL_STATUS_FAIL;
    }

#endif

    sp = *((uint32_t *)imageAddr);
    //
    // Make sure the SP & Reset vector are sane
    // Validate the Stack Pointer
    // Validate the reset vector
    //
    if ((sp < SRAM_BASEADDR) || \
        (sp >= (SRAM_BASEADDR + SRAM_SIZE)))
    {
        am_util_stdio_printf("Invalid main image\n");
        return AM_HAL_STATUS_FAIL;
    }
    else
    {
        am_util_stdio_printf("Found valid main image - SP:0x%x RV:0x%x\n", sp, reset);
        *ppVtor = (uint32_t *)imageAddr;
#ifdef SECURE_BOOT
        // Apply protections
        if (pComHdr->addrWord.s.writeProtect && pFlash->flash_write_protect)
        {
            // Write protect
            pFlash->flash_write_protect(imageAddr, pComHdr->w0.s.blobSize);
        }
        if (pComHdr->addrWord.s.copyProtect && pFlash->flash_copy_protect)
        {
            // Copy protect
            pFlash->flash_copy_protect(imageAddr, pComHdr->w0.s.blobSize);
        }
        // Apply protections for Child images
        pChildPtr = pMainHdr->childPtr;
        while (numChild--)
        {
            am_image_hdr_common_t *pComHdr = (am_image_hdr_common_t *)pChildPtr;

            // Apply protections
            if (pComHdr->addrWord.s.writeProtect && pFlash->flash_write_protect)
            {
                // Write protect
                pFlash->flash_write_protect((uint32_t)pComHdr, pComHdr->w0.s.blobSize);
            }
            if (pComHdr->addrWord.s.copyProtect && pFlash->flash_copy_protect)
            {
                // Copy protect
                pFlash->flash_copy_protect((uint32_t)pComHdr, pComHdr->w0.s.blobSize);
            }
            pChildPtr++;
        }
#endif
        am_util_stdio_printf("Will transfer control over to this image after locking things down\n\n\n\n");

        return AM_HAL_STATUS_SUCCESS;
    }
}
