//*****************************************************************************
//
//! @file apollo3_secbl_info0.c
//!
//! @brief info0 access functions
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

#include "apollo3_secbl.h"
#include "apollo3_secbl_info0.h"

// Get the Authentication key
// It checks for the key index being revoked, and if not - extracts the corresponding
// key from Info0
bool ssbl_info0_get_auth_key(uint32_t authKey, uint8_t *pAuthKey)
{
#if AUTH_ALGO == AUTH_DEFAULT_SHA256_HMAC_ALGO
    uint32_t revokeMask;
    // Each 256b HMAC key takes two slots in the key space
    uint32_t keyMask = 0xC0000000;
    if (!SSBL_RANGE_CHECK(authKey, SSBL_MIN_KEYIDX_INFO0, (SSBL_MAX_KEYIDX_INFO0 + 1)) ||
        (authKey & 0x1) // 256b AuthKey needs to be an even number
        )
    {
        return false;
    }
    revokeMask = AM_REGVAL(INFO0_AREVTRACK_ADDR);
    keyMask >>= (authKey - SSBL_MIN_KEYIDX_INFO0);
    if ((revokeMask & keyMask) != keyMask)
    {
        am_util_stdio_printf("Auth Key # %d revoked mask 0x%x!!\n", authKey, revokeMask);
        return false;
    }

    memcpy(pAuthKey, (uint8_t *)(INFO0_AUTHKEY_ADDR + (authKey - SSBL_MIN_KEYIDX_INFO0) * SSBL_KEYIDX_BYTES), SHA256_SIZE);
    return true;
#else
#error "please return specifc auth keys from info0 here"
#endif
}

// Get the Key Encryption key
// It checks for the key index being revoked, and if not - extracts the corresponding
// key from Info0
bool ssbl_info0_get_kek(uint32_t kek, uint8_t *pKek)
{
#if CRYPT_ALGO == CRYPT_DEFAULT_AES128
    uint32_t revokeMask;
    // Each 256b HMAC key takes two slots in the key space
    uint32_t keyMask = 0x80000000;
    if (!SSBL_RANGE_CHECK(kek, SSBL_MIN_KEYIDX_INFO0, (SSBL_MAX_KEYIDX_INFO0 + 1)))
    {
        return false;
    }
    revokeMask = AM_REGVAL(INFO0_KREVTRACK_ADDR);
    keyMask >>= (kek - SSBL_MIN_KEYIDX_INFO0);
    if ((revokeMask & keyMask) != keyMask)
    {
        am_util_stdio_printf("KEK Key # %d revoked mask 0x%x!!\n", kek, revokeMask);
        return false;
    }
    memcpy(pKek, (uint8_t *)(INFO0_KEK_ADDR + (kek - SSBL_MIN_KEYIDX_INFO0) * SSBL_KEYIDX_BYTES), AES128_SIZE);
    return true;
#else
#error "please return the specific kek from the info0 here"
#endif
}


