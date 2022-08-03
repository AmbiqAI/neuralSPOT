//*****************************************************************************
//
//! @file am_ai_audio.h
//!
//! @brief NeuralSPOT Audio API
//!
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
// This is part of revision release_sdk_4_0_1-bef824fa27 of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef AI_AUDIO
#define AI_AUDIO

#ifdef __cplusplus
extern "C"
{
#endif

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "am_app_utils_ring_buffer.h"

// ******** new API
typedef enum {
    AM_AI_AUDIO_API_CALLBACK,
    AM_AI_AUDIO_API_RINGBUFFER,
    AM_AI_AUDIO_API_TASK,
} am_ai_audio_api_mode_e;

typedef enum {
    AM_AI_AUDIO_SOURCE_AUDADC
} am_ai_audio_source_e;

struct am_ai_acfg;
typedef void (*am_ai_audio_callback_cb)(struct am_ai_acfg*, uint16_t);

//typedef uint16_t audio_buffer_t[][];

typedef struct am_ai_acfg {
    // API Config
    am_ai_audio_api_mode_e  eAudioApiMode;

    // IPC
    am_ai_audio_callback_cb callback;
    void                   *audioBuffer;

    // Audio Config
    am_ai_audio_source_e    eAudioSource;
    uint8_t                 numChannels; // 1 or 2
    uint16_t                numSamples;  // Samples collected per callback
    uint16_t                sampleRate;  // Hz

    // Internals
    void                    *audioSystemHandle; // filled by init
    am_app_utils_ring_buffer_t *bufferHandle; // filled by init

} am_ai_audio_config_t;

extern am_ai_audio_config_t am_ai_audio_config;
extern void audio_init(am_ai_audio_config_t*);

#ifdef __cplusplus
}
#endif

#endif // AI_AUDIO