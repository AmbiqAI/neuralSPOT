//*****************************************************************************
//
//! @file rsa_sign_verify.c
//!
//! @brief A simple example to demonstrate using runtime crypto APIs.
//!
//! This example initializes the runtime crypto lib at the beginning.
//! Later it invokes crypto and use it to do SHA and RSA.
//! If ENABLE_CRYPTO_ON_OFF is defined, it also controls Crypto power and keeps
//! it active only while in use.
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2022, Ambiq Micro, Inc.
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
// This is part of revision release_sdk_4_3_0-0ca7d78a2b of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

#include "config-cc312-apollo4-no-os.h"

#include "cc_lib.h"

#include "mbedtls/rsa.h"
#include "mbedtls/sha256.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/memory_buffer_alloc.h"
#include "mbedtls_cc_mng.h"

#include "crypto_test_data.h"
#include "rsa_keys.h"

#define ENABLE_CRYPTO_ON_OFF

//*****************************************************************************
//
// Globals.
//
//*****************************************************************************

//
// Dynamic memory for mbedTLS stack.
//
#define AM_MBEDTLS_HEAP_SIZE (128*1024/4)
uint32_t g_ui32MbedTLSHeap[AM_MBEDTLS_HEAP_SIZE];

//
// Context variables for mbedTLS operations.
//
mbedtls_rsa_context rsa;
CCRndContext_t RndContex;
CCRndWorkBuff_t RndWorkBuff;
mbedtls_ctr_drbg_context RndState;
mbedtls_entropy_context MbedtlsEntropy;

int
main(void)
{
    /* init Rnd context's inner member */
    RndContex.rndState = &RndState;
    RndContex.entropyCtx = &MbedtlsEntropy;

    //
    // Enable printing
    //
    am_bsp_debug_printf_enable();
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("Crypto Demo Example!\n\n");

#ifdef ENABLE_CRYPTO_ON_OFF
    //
    // This will also power down crypto
    //
    am_bsp_low_power_init();
#endif

    //
    // One time library initialization - to be done at the beginning of program
    //
    am_util_stdio_printf("Doing one time Init!\n\n");
#ifdef ENABLE_CRYPTO_ON_OFF
    am_util_stdio_printf("Powering On Crypto\n");
    //
    // Enable the Crypto module
    //
    am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_CRYPTO);
#endif

    //
    // Initiailize MbedTLS
    //
    mbedtls_memory_buffer_alloc_init((uint8_t*)g_ui32MbedTLSHeap, AM_MBEDTLS_HEAP_SIZE);
    CC_LibInit(&RndContex, &RndWorkBuff);

#ifdef ENABLE_CRYPTO_ON_OFF
    //
    // We can power off crypto now
    //
    mbedtls_mng_suspend(0, 0);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_CRYPTO);
    am_util_stdio_printf("Crypto powered down!\n");
#endif
    //
    // Placeholder for application doing other things unrelated to crypto
    //
    am_hal_delay_us(1000);
#ifdef ENABLE_CRYPTO_ON_OFF
    am_util_stdio_printf("Powering On Crypto\n");
    //
    // Power up crypto
    //
    am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_CRYPTO);
    mbedtls_mng_resume(0, 0);
#endif
    //
    // Initialize the RSA module, and import our keys.
    //
    mbedtls_rsa_init(&rsa, MBEDTLS_RSA_PKCS_V15, 0);
    mbedtls_mpi_read_string(&rsa.N, 16, PUBLIC_RSA_N);
    mbedtls_mpi_read_string(&rsa.E, 16, PUBLIC_RSA_E);

    rsa.len = ( mbedtls_mpi_bitlen( &rsa.N ) + 7 ) >> 3;

    if (mbedtls_rsa_check_pubkey(&rsa))
    {
        am_util_stdio_printf("mbedtls_rsa_check_pubkey failed\n");
        while (1);
    }

    //
    // Run SHA256 on our binary, and then compare against the previously calculated hash.
    //
    uint8_t digest[TEST_HASH_SIZE];
    mbedtls_sha256(pui8TestBinary, TEST_BIN_SIZE, digest, false);

    uint32_t i;
    uint32_t ui32HashErrors = 0;
    for (i = 0; i < TEST_HASH_SIZE; i++)
    {
        if (digest[i] != pui8TestHash[i])
        {
            ui32HashErrors++;
        }
    }

    //
    // Print our SHA256 results.
    //
    if (ui32HashErrors == 0)
    {
        am_util_stdio_printf("SHA256 values match.\n");
    }
    else
    {
        am_util_stdio_printf("Error, %d mismatches found in SHA256.\n", ui32HashErrors);
    }

    //
    // Attempt to verify the provided signature using our public key.
    //
    uint32_t result = mbedtls_rsa_pkcs1_verify(&rsa,
                                               NULL,
                                               NULL,
                                               MBEDTLS_RSA_PUBLIC,
                                               MBEDTLS_MD_SHA256,
                                               32,
                                               pui8TestHash,
                                               pui8TestSignature);

    //
    // Release the RSA context. We're done with RSA operations.
    //
    mbedtls_rsa_free(&rsa);

    //
    // Print our RSA results.
    //
    if (result == 0)
    {
        am_util_stdio_printf("RSA signature verified.\n", result);
    }
    else
    {
        am_util_stdio_printf("RSA Error: 0x%08X\n", result);
    }

#ifdef ENABLE_CRYPTO_ON_OFF
    //
    // We can power off crypto now
    //
    mbedtls_mng_suspend(0, 0);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_CRYPTO);
    am_util_stdio_printf("Crypto powered down!\n");
#endif
    am_util_stdio_printf("Example Done\n");
    while (1);
}
