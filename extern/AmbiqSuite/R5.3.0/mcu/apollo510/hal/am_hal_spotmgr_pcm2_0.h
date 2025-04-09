//*****************************************************************************
//
//! @file am_hal_spotmgr_pcm2_0.h
//!
//! @brief SPOT manager functions that manage power states for PCM 1.0, PCM 1.1,
//! and PCM2.0 parts.
//!
//! @addtogroup spotmgr5b SPOTMGR - SPOT Manager
//! @ingroup apollo510_hal
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
#ifndef AM_HAL_SPOTMGR_PCM2_0_H
#define AM_HAL_SPOTMGR_PCM2_0_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! Defines
//
//*****************************************************************************
//
// The CORELDOACTIVETRIM trim code reduced in simobuck init for both
// PCM1.1 (B0 trim version 3) and PCM2.0 (B1 trim version 1)
//
#define CORELDOACTIVETRIM_REDUCE_IN_SIMOBUCK_INIT_B0TRIM3_B1TRIM1  7

//*****************************************************************************
//
//! Globals
//
//*****************************************************************************

//*****************************************************************************
//
//! @brief Power states update for PCM 1.0, PCM 1.1, and PCM2.0
//!
//! @param eStimulus - Stimilus for power states transition. For GPU state/power
//!                    changes, please use AM_HAL_SPOTMGR_STIM_GPU_STATE but not
//!                    AM_HAL_SPOTMGR_STIM_DEVPWR.
//! @param bOn       - Only needs to be set to true/false when turning on/off
//!                    peripherals or memories included in DEVPWRSTATUS,
//!                    AUDSSPWRSTATUS, MEMPWRSTATUS and SSRAMPWRST. It is
//!                    ignored when updating temperature, CPU state and GPU state.
//! @param pArgs     - Pointer to arguments for power states update, assign it
//!                    to NULL if not needed.
//!
//! When eStimulus is AM_HAL_SPOTMGR_STIM_CPU_STATE,
//! bOn is ignored, and pArgs must point to a am_hal_spotmgr_cpu_state_e enum.
//!
//! When eStimulus is AM_HAL_SPOTMGR_STIM_GPU_STATE,
//! bOn is ignored, and pArgs must point to a am_hal_spotmgr_gpu_state_e enum.
//!
//! When eStimulus is AM_HAL_SPOTMGR_STIM_TEMP,
//! bOn is ignored, and pArgs must point to a am_hal_spotmgr_tempco_param_t struct.
//!
//! When eStimulus is AM_HAL_SPOTMGR_STIM_DEVPWR,
//! bOn must be set to true when turning on a peripheral,
//! bOn must be set to false when turning off a peripheral,
//! and pArgs must point to the DEVPWRSTATUS_MASK of the peripheral to be opened
//! when turning on a peripheral, pArgs is ignored when turning off a peripheral.
//!
//! When eStimulus is AM_HAL_SPOTMGR_STIM_AUDSSPWR,
//! bOn must be set to true when turning on a peripheral,
//! bOn must be set to false when turning off a peripheral,
//! and pArgs must point to the AUDSSPWRSTATUS_MASK of the peripheral to be opened
//! when turning on a peripheral, pArgs is ignored when turning off a peripheral.
//!
//! When eStimulus is AM_HAL_SPOTMGR_STIM_MEMPWR,
//! bOn is ignored, and pArgs must point to the entire MEMPWRSTATUS.
//!
//! When eStimulus is AM_HAL_SPOTMGR_STIM_SSRAMPWR,
//! bOn must be set to true when turning on partial or entire SSRAM,
//! bOn must be set to false when turning off partial or entire SSRAM,
//! and pArgs must point to the expected SSRAMPWRST when turning on,
//! pArgs is ignored when turning off.
//!
//! @return SUCCESS or other Failures.
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_pcm2_0_power_state_update(am_hal_spotmgr_stimulus_e eStimulus, bool bOn, void *pArgs);

//*****************************************************************************
//
//! @brief Reset power state to POR default for PCM 1.0, PCM 1.1, and PCM2.0
//!
//! @return SUCCESS or other Failures.
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_pcm2_0_default_reset(void);

//*****************************************************************************
//
//! @brief SPOT manager init for PCM 1.0, PCM 1.1, and PCM2.0
//!
//! @return SUCCESS or other Failures.
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_pcm2_0_init(void);

//*****************************************************************************
//
//! @brief Postpone tempco handling for PCM 1.0, PCM 1.1, and PCM2.0
//!
//! Set flag to postpone tempco handling till the function
//! am_hal_spotmgr_pcm2_0_tempco_pending_handler() is invoked
//!
//! @return SUCCESS or other Failures.
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_pcm2_0_tempco_postpone(void);

//*****************************************************************************
//
//! @brief Tempco handling for PCM 1.0, PCM 1.1, and PCM2.0
//!
//! Check for pending tempco operation, execute it if pending, and clear
//! the blocking flag.
//!
//! @return SUCCESS or other Failures.
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_pcm2_0_tempco_pending_handle(void);

//*****************************************************************************
//
//! @brief SIMOBUCK initialziation handling at stage just before enabling
//!        SIMOBUCK for PCM 1.0, PCM 1.1, and PCM2.0
//!
//! @return SUCCESS or other Failures.
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_pcm2_0_simobuck_init_bfr_enable(void);

//*****************************************************************************
//
//! @brief SIMOBUCK initialziation handling at stage just after enabling
//!        SIMOBUCK for PCM 1.0, PCM 1.1, and PCM2.0
//!
//! @return SUCCESS or other Failures.
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_pcm2_0_simobuck_init_aft_enable(void);

#if NO_TEMPSENSE_IN_DEEPSLEEP
//*****************************************************************************
//
//! @brief Prepare SPOT manager for suspended tempco during deep sleep for
//!        PCM 1.0, PCM 1.1, and PCM2.0
//!
//! @return SUCCESS or other Failures.
//!
//! Note: This handling also handle tempco suspend for versions before PCM2.0
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_pcm2_0_tempco_suspend(void);
#endif

#if AM_HAL_PWRCTRL_SIMOLP_AUTOSWITCH
//*****************************************************************************
//
//! @brief Initialize registers for SIMOBUCK LP auto switch for PCM 1.0, PCM 1.1,
//! and PCM2.0.
//!
//! @return SUCCESS or other Failures.
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_pcm2_0_simobuck_lp_autosw_init(void);

//*****************************************************************************
//
//! @brief Enable SIMOBUCK LP auto switch for PCM 1.0, PCM 1.1, and PCM2.0
//!
//! @return SUCCESS or other Failures.
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_pcm2_0_simobuck_lp_autosw_enable(void);

//*****************************************************************************
//
//! @brief Disable SIMOBUCK LP auto switch for PCM 1.0, PCM 1.1, and PCM2.0
//!
//! @return SUCCESS or other Failures.
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_pcm2_0_simobuck_lp_autosw_disable(void);
#endif

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_SPOTMGR_PCM2_0_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************


