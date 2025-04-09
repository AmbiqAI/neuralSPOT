//*****************************************************************************
//
//! @file am_util_crypto.h
//!
//! @brief Utility functions for the crypto module.
//!
//! A few utils to help with crypto functionality
//!
//! @addtogroup crypto Crypto
//! @ingroup utils
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2025, Ambiq Micro, Inc.
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
// This is part of revision release_sdk5p0p0-5f68a8286b of the AmbiqSuite Development Package.
//
//*****************************************************************************

#ifndef AM_UTIL_CRYPTO_H
#define AM_UTIL_CRYPTO_H

#include <time.h>

#if !defined(AM_PART_APOLLO4_API) && !defined(AM_PART_APOLLO5_API)
#error "Error: am_util_crypto.h only supports Apollo4 and Apollo5."
#endif

//
// Memory Pools
//
#define AM_UTIL_CRYPTO_VARIABLE_POOL_SIZE      512
#define AM_UTIL_CRYPTO_FIXED_BUFFER_SIZE       20
#define AM_UTIL_CRYPTO_FIXED_BUFFER_COUNT      1024

//
// LibSWD configuration
//
#define AM_UTIL_CRYPTO_WRITE_BLOCK_SIZE        128
#define AM_UTIL_CRYPTO_READ_BLOCK_SIZE         32

//*****************************************************************************
//
//! @brief Malloc implemtation tailored to LibCrypto's needs.
//!
//! @param size - size of buffers to malloc
//!
//! @return pointer to space malloc'ed
//
//*****************************************************************************
extern void * am_util_crypto_malloc(size_t size);

//*****************************************************************************
//
//! @brief Calloc implementation tailored to LibCrypto's needs.
//!
//! @param num - number of buffers to calloc
//! @param size - size of buffers to calloc
//!
//! @return pointer to space calloc'ed
//
//*****************************************************************************
extern void * am_util_crypto_calloc(size_t num, size_t size);

//*****************************************************************************
//
//! @brief Minimal implementation of Crypto Free.
//
//! @note This implementation of free can only free memory from the fixed-size
//! buffer pool. It ignores all other requests.
//!
//! @param ptr - pointer to buffer to free
//
//*****************************************************************************
extern void am_util_crypto_free(void *ptr);

//*****************************************************************************
//
//! @brief Statistics function for malloc
//
//*****************************************************************************
extern void am_util_crypto_print_malloc_stats(void);

//*****************************************************************************
//
//! @brief Crypto Exit
//!
//! @param exitCode - Exit Code
//
//*****************************************************************************
extern void am_util_crypto_exit(uint32_t exitCode);

//*****************************************************************************
//
//! @brief Crypto Abort
//
//*****************************************************************************
extern void am_util_crypto_abort(void);

//*****************************************************************************
//
//! @brief Crypto Get Time for mbedtls libraries
//!
//! @note To be used with cryptocell library builds only. RTC structure should
//!       be defined in code per crypto_self_test example
//!
//! @param time - pointer to time structure - will be unsued and is a carryover
//!               from the library
//!
//! @return time_t - Calculated time in seconds since 1/1/1970 until current
//!                  RTC time
//
//*****************************************************************************
extern time_t am_util_crypto_get_time( time_t * time );

//*****************************************************************************
//
//! @brief Crypto Set Ambiq RTC structure
//!
//! @note To be used with cryptocell library builds only
//!
//! Use this function to create a local structure in user code to pass for
//! use in am_util_crypto_get_time
//
//*****************************************************************************
extern void am_util_crypto_ambiq_time_structure_set(am_hal_rtc_time_t *atm);

//*****************************************************************************
//
//! @brief Crypto Set gmtime structure
//!
//! @note To be used with cryptocell library builds only
//!
//! Use this function to create a local structure in user code to pass for
//! use in am_util_crypto_get_time
//
//*****************************************************************************
extern void am_util_crypto_gm_time_structure_set(struct tm *tm);

#endif // AM_UTIL_CRYPTO_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

