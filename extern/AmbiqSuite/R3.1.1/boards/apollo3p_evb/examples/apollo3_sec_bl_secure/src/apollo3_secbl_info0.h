//*****************************************************************************
//!
//! @file apollo3_secbl_info0.h
//!
//! @brief the header file of info0 access functions
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

#ifndef APOLLO3_SECBL_INFO0_H
#define APOLLO3_SECBL_INFO0_H

#include "am_mcu_apollo.h"

//
// These address could be redefined by the customer if they want to
// customize their INFO0
//
#define INFO0_SECURITY_ADDR             (AM_HAL_FLASH_INFO_ADDR + 0x10)
#define INFO0_AREVTRACK_ADDR            (AM_HAL_FLASH_INFO_ADDR + 0x00000c08)
#define INFO0_KREVTRACK_ADDR            (AM_HAL_FLASH_INFO_ADDR + 0x00000c0c)
#define INFO0_AUTHKEY_ADDR              (AM_HAL_FLASH_INFO_ADDR + 0x00001880)
#define INFO0_KEK_ADDR                  (AM_HAL_FLASH_INFO_ADDR + 0x00001800)

#define INFO0_SECURITY_MASK_PLONEXIT    0x800
#define INFO0_SECURITY_MASK_AUTHENFORCE 0x1000000
#define INFO0_SECURITY_MASK_ENCENFORCE  0x2000000
#define INFO0_SECURITY_MASK_NOROLLBACK  0x4000000

// KeyIdx 8 - 15
#define SSBL_MIN_KEYIDX_INFO0     8
#define SSBL_MAX_KEYIDX_INFO0     15

// Number of key bytes corresponding to each index
#define SSBL_KEYIDX_BYTES         16

//
// get the INFO0 rollback information
//
static inline bool ssbl_info0_get_NoRollback(void)
{
    return AM_REGVAL(INFO0_SECURITY_ADDR) & INFO0_SECURITY_MASK_NOROLLBACK;
}

//
// get the INFO authentication enforce information
//
static inline bool ssbl_info0_get_AuthEnforce(void)
{
    return AM_REGVAL(INFO0_SECURITY_ADDR) & INFO0_SECURITY_MASK_AUTHENFORCE;
}

//
// get the INFO0 encrypt enforce information
//
static inline bool ssbl_info0_get_EncEnforce(void)
{
    return AM_REGVAL(INFO0_SECURITY_ADDR) & INFO0_SECURITY_MASK_ENCENFORCE;
}

//*****************************************************************************
//
//! @brief get the authentification key from the INFO0
//!
//! @param autheKey - auth key index
//! @param pAuthKey - the authentification key
//!
//! @return true if the valid auth key can be found
//
//*****************************************************************************
bool ssbl_info0_get_auth_key(uint32_t authKey, uint8_t *pAuthKey);

//*****************************************************************************
//
//! @brief get the key encryption key
//!
//! @param kek - key encryption key index
//! @param pKey - the key encryption key
//!
//! @return true if key encryption key is valid
//
//*****************************************************************************
bool ssbl_info0_get_kek(uint32_t kek, uint8_t *pKek);

#endif
