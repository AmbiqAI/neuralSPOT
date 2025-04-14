//*****************************************************************************
//
//! @file am_hal_sysctrl_ton_config.h
//!
//! @brief Internal api definition for Ton Control
//!
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
//! @cond SYSCTRL_TON_CONFIG
#ifndef AM_HAL_SYSCTRL_TON_CONFIG_H
#define AM_HAL_SYSCTRL_TON_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! Selection of SIMOBUCK scheme for TON configuration
//
//*****************************************************************************
#define AM_SIMOBUCK_SCHEME_HIGH_EFFICIENCY 0

//*****************************************************************************
//
//! Definition of TON Value Levels
//
//*****************************************************************************
typedef enum
{
    SYSCTRL_TON_LEVEL_NA = 0,
    SYSCTRL_TON_LEVEL_LOW,
    SYSCTRL_TON_LEVEL_HIGH,
} am_hal_sysctrl_ton_levels_t;

//*****************************************************************************
//
//! Definition of CPU/GPU Power State enumeration for SIMOBUCK TON control
//
//*****************************************************************************
typedef enum
{
    SYSCTRL_GPU_TON_POWER_STATE_OFF,
    SYSCTRL_GPU_TON_POWER_STATE_LP,
    SYSCTRL_GPU_TON_POWER_STATE_HP,
    SYSCTRL_GPU_TON_POWER_STATE_MAX,
} am_hal_sysctrl_gpu_ton_power_state;

typedef enum
{
    SYSCTRL_CPU_TON_POWER_STATE_OFF = 0,
    SYSCTRL_CPU_TON_POWER_STATE_LP,
    SYSCTRL_CPU_TON_POWER_STATE_HP,
    SYSCTRL_CPU_TON_POWER_STATE_MAX,
} am_hal_sysctrl_cpu_ton_power_state;

#ifdef __cplusplus
}
#endif

#endif //AM_HAL_SYSCTRL_TON_CONFIG_H

//! @endcond SYSCTRL_TON_CONFIG
