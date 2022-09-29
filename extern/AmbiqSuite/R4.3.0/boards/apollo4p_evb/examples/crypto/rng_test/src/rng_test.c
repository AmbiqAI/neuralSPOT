//*****************************************************************************
//
//! @file rng_test.c
//!
//! @brief A simple example to demonstrate use of the mbedtls RNG.
//!
//! This example initialized the mbedTLS crypto library and uses it to produce a
//! stream of random numbers.
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

#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/memory_buffer_alloc.h"
#include "mbedtls_cc_mng.h"

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
CCRndContext_t RndContext;
CCRndWorkBuff_t RndWorkBuff;
mbedtls_ctr_drbg_context RndState;
mbedtls_entropy_context MbedtlsEntropy;

//
// Output buffer for random numbers.
//
#define NUM_RANDOM_BYTES 128
uint8_t pui8RandomBytes[NUM_RANDOM_BYTES];

int
main(void)
{
    uint32_t ui32Error;

    /* init Rnd context's inner member */
    RndContext.rndState = &RndState;
    RndContext.entropyCtx = &MbedtlsEntropy;

    //
    // Enable printing
    //
    am_bsp_debug_printf_enable();
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("RNG Demo Example!\n\n");

    //
    // This will also power down crypto
    //
    am_bsp_low_power_init();

    //
    // One time library initialization - to be done at the beginning of program
    //
    am_util_stdio_printf("Powering On Crypto\n");

    //
    // Enable the Crypto module
    //
    am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_CRYPTO);

    //
    // Initiailize MbedTLS
    //
    mbedtls_memory_buffer_alloc_init((uint8_t*)g_ui32MbedTLSHeap, AM_MBEDTLS_HEAP_SIZE);
    CC_LibInit(&RndContext, &RndWorkBuff);

    //
    // Call the mbedTLS API to generate random bytes. If successful, print the
    // bytes to ITM. Otherwise, print the relevant error message.
    //
    ui32Error = mbedtls_ctr_drbg_random(RndContext.rndState, pui8RandomBytes, NUM_RANDOM_BYTES);
    if (ui32Error)
    {
        am_util_stdio_printf("mbedTLS error: 0x%08X\n", ui32Error);
    }
    else
    {
        am_util_stdio_printf("Retrieved random bytes:\n");
        for (uint32_t i = 0; i < NUM_RANDOM_BYTES; i++)
        {
            am_util_stdio_printf("0x%02X\n", pui8RandomBytes[i]);
        }
    }

    am_util_stdio_printf("Example Done.\n");
    while (1)
    {
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_NORMAL);
    }
}
