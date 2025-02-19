//*****************************************************************************
//
//! @file am_hal_clkgen_private.h
//!
//! @brief Internal api definition for internal clock_gen functions
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
//! @cond CLKGEN_PRIVATE_FUNC
#ifndef AM_HAL_CLKGEN_PRIVATE_H
#define AM_HAL_CLKGEN_PRIVATE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "am_hal_clkgen.h"

//
//! Default HFRC_ADJ that adjusts HFRC from 48MHz to 48MHZ with 32.768KHz
//! XTAL_LS as reference clock
//
#define AM_HAL_CLKGEN_DEFAULT_HFRC_ADJ_CONFIG                           \
    {                                                                   \
        .eRepeat        = AM_HAL_CLKGEN_HFRCADJ_RPT_4_SEC,              \
        .ui32TargetVal  = 0x5B8,                                        \
        .eWarmup        = AM_HAL_CLKGEN_HFRCAJD_XTWARMUP_1SEC,          \
        .eSpeed         = AM_HAL_CLKGEN_HFRCADJ_ATTACK_SPEED_1_IN_2,    \
        .eMaxDelta      = AM_HAL_CLKGEN_HFRCADJ_MAXDELTA_DIS,           \
    }

//
// ! Default HFRC2_ADJ that adjusts HFRC2 from 250MHz to 196.608MHz with 32MHz
// ! XTAL HS as reference clock
//
#define AM_HAL_CLKGEN_DEFAULT_HFRC2_ADJ_CONFIG                          \
    {                                                                   \
        .eRefClkSel   =  AM_HAL_CLKGEN_HFRC2ADJ_REFCLK_MUX_XTAL_HS,\
        .eRefClkDiv   =  AM_HAL_CLKGEN_HFRC2ADJ_REFCLK_DIV_4,\
        .ui32AdjRatio =  0xC49BA,\
    }


// ****************************************************************************
//
//! @brief calculate HFRC2 adjust ratio
//!
//! This function calculates the HFRC2 adjust ratio needed to achieve
//! ui32TargetFreq specified for the eXtalDiv setting.
//!
//! @param ui32RefFreq - HFRC2 ADJ reference frequency in Hz as selected by
//!                      eRefClkSel.
//! @param ui32TargetFreq - HFRC2 target frequency in Hz to tune HFRC2_250MHz to.
//! @param eXtalDiv - HFRC2 ADJ xtal divide settings
//! @param pui32AdjRatio - pointer to uint32 variable to store the result of
//!                        the calculation.
//!
//! @note Avoid using this API directly, use Clock Manager to control HF2ADJ
//!       with automatic configuration for HFRC2 instead.
//!
//! @return status      - generic or interface specific status.
//
// ****************************************************************************
extern uint32_t am_hal_clkgen_hfrc2adj_ratio_calculate(uint32_t ui32RefFreq,
                                                       uint32_t ui32TargetFreq,
                                                       am_hal_clkgen_hfrc2adj_refclk_div_e eXtalDiv,
                                                       uint32_t *pui32AdjRatio);


// ****************************************************************************
//
//! @brief calculate HFRC ADJ target
//!
//! This function calculates the HFRC adjust targeted needed to achieve
//! ui32TargetFreq
//!
//! @param ui32RefFreq - XTAL_LS frequency in Hz.
//! @param ui32TargetFreq - HFRC target frequency in Hz to tune HFRC_48MHz to
//! @param pui32AdjTarget - pointer to uint32 variable to store the result of
//!                         the calculation.
//!
//! @note Avoid using this API directly, use Clock Manager to control HFADJ
//!       with automatic configuration for HFRC instead.
//!
//!
//! @return status      - generic or interface specific status.
//
// ****************************************************************************
extern uint32_t am_hal_clkgen_hfrcadj_target_calculate(uint32_t ui32RefFreq,
                                                       uint32_t ui32TargetFreq,
                                                       uint32_t *pui32AdjTarget);

// ****************************************************************************
//
//! @brief Enable/Disable force-on for HFRC oscillator block.
//!
//! @note This API is inteded for use by HAL only. Do not call this API from
//!       Application/BSP.
//!
//! @param bForceOn - TRUE: force HFRC oscillator ON, FALSE: Hardware managed.
//!
//! @return status  - Execution status as defined in am_hal_status_e.
//
// ****************************************************************************
extern uint32_t am_hal_clkgen_private_hfrc_force_on(bool bForceOn);

// ****************************************************************************
//
//! @brief Configure and Enable HFADJ
//!
//! @note This API is inteded for use by HAL only. Do not call this API from
//!       Application/BSP.
//!
//! @param ui32RegVal - HFADJ configuration register value.
//!
//! @return status    - Execution status as defined in am_hal_status_e.
//
// ****************************************************************************
extern uint32_t am_hal_clkgen_private_hfadj_apply(uint32_t ui32RegVal);

// ****************************************************************************
//
//! @brief Disable HFADJ
//!
//! @note This API is inteded for use by HAL only. Do not call this API from
//!       Application/BSP.
//!
//! @return status    - Execution status as defined in am_hal_status_e.
//
// ****************************************************************************
extern uint32_t am_hal_clkgen_private_hfadj_disable();

// ****************************************************************************
//
//! @brief Enable/Disable force-on for HFRC2 oscillator block.
//!
//! @note This API is inteded for use by HAL only. Do not call this API from
//!       Application/BSP.
//!
//! @param bForceOn - TRUE: force HFRC2 oscillator ON, FALSE: Hardware managed.
//!
//! @return status  - Execution status as defined in am_hal_status_e.
//
// ****************************************************************************
extern uint32_t am_hal_clkgen_private_hfrc2_force_on(bool bForceOn);

// ****************************************************************************
//
//! @brief Configure and Enable HFADJ
//!
//! @note This API is inteded for use by HAL only. Do not call this API from
//!       Application/BSP.
//!
//! @param ui32RegVal - HFADJ configuration register value.
//!
//! @return status    - Execution status as defined in am_hal_status_e.
//
// ****************************************************************************
extern uint32_t am_hal_clkgen_private_hf2adj_apply(am_hal_clkgen_hfrc2adj_t* psHFRC2Adj);

// ****************************************************************************
//
//! @brief Disable HFADJ2
//!
//! @note This API is inteded for use by HAL only. Do not call this API from
//!       Application/BSP.
//!
//! @return status    - Execution status as defined in am_hal_status_e.
//
// ****************************************************************************
extern uint32_t am_hal_clkgen_private_hf2adj_disable();

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_CLKGEN_PRIVATE_H
//! @endcond CLKGEN_PRIVATE_FUNC
