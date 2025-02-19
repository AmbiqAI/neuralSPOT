//*****************************************************************************
//
//! @file am_hal_clkmgr_private.h
//!
//! @brief Internal api definition for internal clock manager functions
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
//! @cond CLKMGR_PRIVATE_FUNC
#ifndef AM_HAL_CLKMGR_PRIVATE_H
#define AM_HAL_CLKMGR_PRIVATE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "am_hal_clkmgr.h"

//*****************************************************************************
//
//! @brief Handle Call to Legacy CLKGEN AM_HAL_CLKGEN_CONTROL_HFADJ_ENABLE
//!        This function will convert and call HFRC clock configuration.
//!
//! @note This API is inteded for use by HAL only. Do not call this API from
//!       Application/BSP.
//!
//! @param pArgs - Argument passed to the AM_HAL_CLKGEN_CONTROL_HFADJ_ENABLE
//!                for custom HFADJ configuration
//!
//! @return status - Status for the operation, as defined in am_hal_status_e
//
//*****************************************************************************
extern uint32_t am_hal_clkmgr_private_clkgen_hfadj_apply(void* pArgs);

//*****************************************************************************
//
//! @brief Handle Call to Legacy CLKGEN AM_HAL_CLKGEN_CONTROL_HFADJ_DISABLE
//!        This function will convert and call HFRC clock configuration.
//!
//! @note This API is inteded for use by HAL only. Do not call this API from
//!       Application/BSP.
//!
//! @return status - Status for the operation, as defined in am_hal_status_e
//
//*****************************************************************************
extern uint32_t am_hal_clkmgr_private_clkgen_hfadj_disable();

//*****************************************************************************
//
//! @brief Handle Call to Legacy CLKGEN AM_HAL_CLKGEN_CONTROL_HF2ADJ_ENABLE
//!        This function will convert and call HFRC2 clock configuration.
//!
//! @note This API is inteded for use by HAL only. Do not call this API from
//!       Application/BSP.
//!
//! @return status - Status for the operation, as defined in am_hal_status_e
//
//*****************************************************************************
extern uint32_t am_hal_clkmgr_private_clkgen_hf2adj_apply();

//*****************************************************************************
//
//! @brief Handle Call to Legacy CLKGEN AM_HAL_CLKGEN_CONTROL_HF2ADJ_DISABLE
//!        This function will convert and call HFRC2 clock configuration.
//!
//! @note This API is inteded for use by HAL only. Do not call this API from
//!       Application/BSP.
//!
//! @return status - Status for the operation, as defined in am_hal_status_e
//
//*****************************************************************************
extern uint32_t am_hal_clkmgr_private_clkgen_hf2adj_disable();

#ifdef __cplusplus
}
#endif

#endif //AM_HAL_CLKMGR_PRIVATE_H
//! @endcond CLKMGR_PRIVATE_FUNC

