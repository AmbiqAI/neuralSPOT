//*****************************************************************************
//
//! @file am_hal_utils.h
//!
//! @brief HAL Utility Functions
//!
//! @addtogroup utils4 Utils - HAL Utility Functions
//! @ingroup apollo5b_hal
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
// This is part of revision release_sdk5p2-040c7863bb of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef AM_HAL_UTILS_H
#define AM_HAL_UTILS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

//*****************************************************************************
//
//! @brief Use the bootrom to implement a spin loop.
//!
//! @param ui32us - Number of microseconds to delay.  Must be >=1; the
//! value of 0 will result in an extremely long delay.
//!
//! Use this function to implement a CPU busy waiting spin loop without cache
//! or delay uncertainties.
//!
//! @note Interrupts are not disabled during execution of this function.
//!       Therefore, any interrupt taken will affect the delay timing.
//
//*****************************************************************************
extern void am_hal_delay_us(uint32_t ui32us);

//*****************************************************************************
//
//! @brief Delays for a desired amount of cycles while also waiting for a
//! status to change a value.
//!
//! @param ui32usMaxDelay - Maximum number of ~1uS delay loops.
//! @param ui32Address    - Address of the register for the status change.
//! @param ui32Mask   - Mask for the status change.
//! @param ui32Value  - Target value for the status change.
//!
//! This function will delay for approximately the given number of microseconds
//! while checking for a status change, exiting when either the given time has
//! expired or the status change is detected.
//!
//! @returns AM_HAL_STATUS_SUCCESS = status change detected.
//!          AM_HAL_STATUS_TIMEOUT = timeout.
//
//*****************************************************************************
extern uint32_t am_hal_delay_us_status_change(uint32_t ui32usMaxDelay, uint32_t ui32Address,
                                 uint32_t ui32Mask, uint32_t ui32Value);

//*****************************************************************************
//
//! @brief Delays for a desired amount of cycles while also waiting for a
//! status to equal OR not-equal to a value.
//!
//! @param ui32usMaxDelay - Maximum number of ~1uS delay loops.
//! @param ui32Address    - Address of the register for the status change.
//! @param ui32Mask   - Mask for the status change.
//! @param ui32Value  - Target value for the status change.
//! @param bIsEqual   - Check for equal if true; not-equal if false.
//!
//! This function will delay for approximately the given number of microseconds
//! while checking for a status change, exiting when either the given time has
//! expired or the status change is detected.
//!
//! @returns 0 = timeout.
//!          1 = status change detected.
//
//*****************************************************************************
extern uint32_t am_hal_delay_us_status_check(uint32_t ui32usMaxDelay, uint32_t ui32Address,
                                uint32_t ui32Mask, uint32_t ui32Value,
                                bool bIsEqual);

//*****************************************************************************
//
//!  @brief Read words
//!
//!  @param pSrcAddr The starting address to read from.
//!  @param pDstAddr The starting address to write to.
//!  @param numWords The number of words to read.
//!
//!  The core of this function is located in ITCM, which means instruction fetch bypasses
//!  I-Cache.
//
//*****************************************************************************
extern void am_hal_read_words(uint32_t *pSrcAddr, uint32_t *pDstAddr, uint32_t numWords);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_UTILS_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

