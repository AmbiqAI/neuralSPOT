//*****************************************************************************
//
//! @file hello_world_uart.c
//!
//! @brief A simple "Hello World" example using the UART peripheral.
//!
//! This example prints a "Hello World" message with some device info
//! over UART at 115200 baud.
//! To see the output of this program, run a terminal appl such as
//! Tera Term or PuTTY, and configure the console for UART.
//! The example sleeps after it is done printing.
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2021, Ambiq Micro, Inc.
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
// This is part of revision release_sdk_4_0_1-bef824fa27 of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef MLPERF_POWER
#define MLPERF_POWER

#ifdef __cplusplus
extern "C"
{
#endif

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

typedef enum
{
    AM_AI_MINIMUM_PERF = 0, // slowest clock
    AM_AI_MEDIUM_PERF  = 1, // ~100Mhz clock
    AM_AI_MAXIMUM_PERF = 2  // ~200Mhz clock
} am_ai_power_mode_e;

typedef struct
{
    am_ai_power_mode_e eAIPowerMode;
    bool               bNeedAudAdc;
    bool               bNeedSharedSRAM;
    bool               bNeedCrypto;
    bool               bNeedBluetooth;
    bool               bNeedAlternativeUART; // for EEMBC Power Control Module and similar
} am_ai_power_config_t;

extern const am_ai_power_config_t am_ai_development_default;
extern const am_ai_power_config_t am_ai_good_default;
extern const am_ai_power_config_t am_ai_mlperf_recommended_default;
extern const am_ai_power_config_t am_ai_mlperf_ulp_default;
extern const am_ai_power_config_t am_ai_audio_default;

extern uint32_t am_ai_set_power_mode(const am_ai_power_config_t*);

#ifdef __cplusplus
}
#endif

#endif // MLPERF_POWER