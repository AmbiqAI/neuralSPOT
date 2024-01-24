//*****************************************************************************
//!
//! @file apollo3_secbl.h
//!
//! @brief the header file of second bootloader main program
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

#ifndef APOLLO3_SECBL_H
#define APOLLO3_SECBL_H

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

#include "apollo3_secbl_info0.h"
#include "apollo3_secbl_image.h"
#include "apollo3_secbl_flash.h"
#include "apollo3_secbl_mpsi_flash.h"
#include "apollo3_secbl_secure.h"

#define SECURE_BOOT

#define AUTH_DEFAULT_SHA256_HMAC_ALGO 1
#define AUTH_ALGO                     AUTH_DEFAULT_SHA256_HMAC_ALGO
#define CRYPT_DEFAULT_AES128          1
#define CRYPT_ALGO                    CRYPT_DEFAULT_AES128

#define MAIN_PROGRAM_ADDR_IN_FLASH    0x80000 // This could be hardcoded, or derived from INFO0 (preferred)

// Test the XIP from MSPI1
// #define MAIN_PROGRAM_ADDR_IN_FLASH      0x4000000 // This could be hardcoded, or derived from INFO0 (preferred)

// Customer Proprietary image in internal flash
#define MAGIC_CUST_PROP                 0xC2

// Proprietary Image format to convey Customer Specific secondary OTA descriptor
// This could be used for images resident out of internal flash for example
#define MAGIC_CUST_SECOTADESC           0xC1

// min <= val < max
#define SSBL_RANGE_CHECK(val, min, max)   ((!(min) || ((val) >= (min))) && ((val) < (max)))

typedef struct
{
    // First 4 bytes to be same as am_image_hdr_common_t
    union
    {
        uint32_t ui32;
        struct
        {
            uint32_t blobSize : 21;
            uint32_t resvd    : 3;
            uint32_t magicNum : 8;
        } s;
    } w0;
    uint32_t ota[AM_HAL_SECURE_OTA_MAX_OTA + 1];
} ssbl_cust_otadesc_t;

static inline bool validate_cust_prop_image(am_image_hdr_common_t *pComHdr, uint32_t **ppVtor)
{
    return false;
}

static inline bool process_cust_prop_ota(uint32_t otaImagePtr)
{
    return false;
}

#define TEMP_BUF_SIZE  AM_HAL_FLASH_PAGE_SIZE

extern uint8_t tempBuf[TEMP_BUF_SIZE];

#endif // APOLLO3_SECBL_H
