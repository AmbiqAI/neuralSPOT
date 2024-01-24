//*****************************************************************************
//
//! @file apollo3_secbl_secure.c
//!
//! @brief security related functions of the secondary bootloader
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
#include "mbedtls/config.h"
#include "mbedtls/aes.h"
#include "mbedtls/md.h"
#include "mbedtls/memory_buffer_alloc.h"

mbedtls_md_context_t sha_ctx;
mbedtls_aes_context aes_ctx;

static unsigned char g_mbed_buf[8192];

void
ssbl_secure_init(void)
{
    // Initialize the MBED SW security engine
    mbedtls_memory_buffer_alloc_init(g_mbed_buf, sizeof(g_mbed_buf));
}

void ssbl_auth_start(uint8_t *pAuthKey)
{
#if AUTH_ALGO == AUTH_DEFAULT_SHA256_HMAC_ALGO
    mbedtls_md_init(&sha_ctx);
    mbedtls_md_setup(&sha_ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1);
    mbedtls_md_hmac_starts(&sha_ctx, pAuthKey, 32);
#else
#error "please add the your specifc authentication algorithm here"
#endif
};

void ssbl_auth_update(uint32_t bufAddr, uint32_t length)
{
#if AUTH_ALGO == AUTH_DEFAULT_SHA256_HMAC_ALGO
    mbedtls_md_hmac_update(&sha_ctx, (uint8_t *)bufAddr, length);
#else
#error "please add the your specifc authentication algorithm here"
#endif
};


void ssbl_auth_finish(uint8_t *pDigest)
{
#if AUTH_ALGO == AUTH_DEFAULT_SHA256_HMAC_ALGO
    mbedtls_md_hmac_finish(&sha_ctx, pDigest);
    mbedtls_md_free(&sha_ctx);
#else
#error "please add the your specifc authentication algorithm here"
#endif
};

void ssbl_crypt_init(uint8_t *pKey)
{
#if CRYPT_ALGO == CRYPT_DEFAULT_AES128
    mbedtls_aes_init(&aes_ctx);
    mbedtls_aes_setkey_dec(&aes_ctx, pKey, AES128_SIZE*8);
#else
#error "please add the your specifc cryto algorithm here"
#endif
}

void ssbl_crypt_decrypt(uint8_t *cipherText, uint8_t *plainText, uint32_t size, uint8_t *pIv)
{
#if CRYPT_ALGO == CRYPT_DEFAULT_AES128
    mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_DECRYPT,
                      size, pIv,
                      cipherText, plainText);
#else
#error "please add the your specifc cryto algorithm here"
#endif
}

bool ssbl_verify_signature(uint8_t *signature, uint8_t *digest, uint8_t *authKey)
{
    //
    // if using RSA algorithm, it needs a public key to verify the signature
    //
#if AUTH_ALGO == AUTH_DEFAULT_SHA256_HMAC_ALGO
    (void)authKey;
    if (memcmp(signature, digest, SHA256_SIZE))
    {
        am_util_stdio_printf("mismatched signature\n");
        return false;
    }
    return true;
#else
#error "please add the your specifc signature comparasion here"
#endif
}

