//*****************************************************************************
//
//  am_hal_utils.h
//! @file
//!
//! @brief HAL Utility Functions
//!
//! @addtogroup utils4 Utils
//! @ingroup apollo4hal
//! @{
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
// This is part of revision release_sdk_4_1_0-8020bdf229 of the AmbiqSuite Development Package.
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

extern void     am_hal_delay_us(uint32_t ui32us);
extern uint32_t am_hal_delay_us_status_change(uint32_t ui32usMaxDelay, uint32_t ui32Address,
                                 uint32_t ui32Mask, uint32_t ui32Value);
extern uint32_t am_hal_delay_us_status_check(uint32_t ui32usMaxDelay, uint32_t ui32Address,
                                uint32_t ui32Mask, uint32_t ui32Value,
                                bool bIsEqual);
extern uint32_t am_hal_load_ui32(uint32_t *pui32Address);
extern void     am_hal_store_ui32(uint32_t *pui32Address, uint32_t ui32Data);

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
