//*****************************************************************************
//!
//! @file apollo3_secbl_secure.h
//!
//! @brief the header file of secondary bootloader secure functions
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

#ifndef APOLLO3_SECBL_SECURE_H
#define APOLLO3_SECBL_SECURE_H

#include "apollo3_secbl.h"

#define SHA256_SIZE 32
#define AES128_SIZE 16

#if AUTH_ALGO == AUTH_DEFAULT_SHA256_HMAC_ALGO
#define DIGEST_SIZE SHA256_SIZE
#else
#error "please specify the authentication digest length"
#endif

#if CRYPT_ALGO == CRYPT_DEFAULT_AES128
#define CRYPT_SIZE  AES128_SIZE
#define IMG_CRYPT_IV(pComHdr) (void *)&pComHdr->iv
#else
#error "please specify the crypt key length & crypt initial vector"
#endif

typedef struct
{
    uint32_t                clearSize; // clear text size
    bool                    bDecrypt;  // if the image is encrypted
    uint8_t                 key[CRYPT_SIZE]; // encryption key size
    uint8_t                 iv[CRYPT_SIZE]; // initial vector of the encryption algorithm
} ssbl_decrypt_info_t;

//*****************************************************************************
//
//! @brief initialize the authentication
//!
//! @param pAuthKey - the authentication key
//!
//! @return None
//
//*****************************************************************************
void ssbl_auth_start(uint8_t *pAuthKey);

//*****************************************************************************
//
//! @brief update the authentication
//!
//! @param bufAddr - buffer address of the data which needs to be authenticated
//! @param length - length of the data which needs to be authenticated
//!
//! @return None
//
//*****************************************************************************
void ssbl_auth_update(uint32_t bufAddr, uint32_t length);

//*****************************************************************************
//
//! @brief get the final authentication digital signature
//!
//! @param pDigest - the final authentication digital signature
//!
//! @return None
//
//*****************************************************************************
void ssbl_auth_finish(uint8_t *pDigest);

//*****************************************************************************
//
//! @brief Initialize the MBED SW security engine
//!        Note: default use MBED TLS security engine
//!
//! @param None
//!
//! @return None
//
//*****************************************************************************
void ssbl_secure_init(void);

//*****************************************************************************
//
//! @brief Initialize the MBED AES128 crypt algorithm
//!        Note: default use MBED TLS AES128 algorithm
//!
//! @param pKey - the AES128 cryption key
//!
//! @return None
//
//*****************************************************************************
void ssbl_crypt_init(uint8_t *pKey);

//*****************************************************************************
//
//! @brief decrypt the information in the 'cipherText' and put it in the 'plainText'
//!
//! @param ciperText - the crypted data
//! @param plainText - the decrypted data
//! @param size - size of the data
//! @param pIV - initial vector for decryption
//!
//! @return None
//
//*****************************************************************************
void ssbl_crypt_decrypt(uint8_t *cipherText, uint8_t *plainText, uint32_t size, uint8_t *pIv);

//*****************************************************************************
//
//! @brief Compare the signature with the digital signature calculated from the image
//!
//! @param signature - signaure in the image header
//! @param digest - the digital signature calculated from the image
//! @param authKey - autheKey may be used for some authentication algorithms
//!
//! @return true if signature matches
//
//*****************************************************************************
bool ssbl_verify_signature(uint8_t *signature, uint8_t *digest, uint8_t *authKey);

#endif // APOLLO3_SECBL_SECURE_H
