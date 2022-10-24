/**
 * @file ns_audio.h
 * @author Carlos Morales
 * @brief API Definition for NeuralSPOT Audio library
 * @version 0.1
 * @date 2022-08-12
 *
 * @copyright Copyright (c) 2022
 *
 * \addtogroup NeuralSPOT-Audio
 *  @{
 */

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
// This is part of revision release_sdk_4_0_1-bef824fa27 of the AmbiqSuite
// Development Package.
//
//*****************************************************************************
#ifndef NS_AUDIO
#define NS_AUDIO

#ifdef __cplusplus
extern "C" {
#endif

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "ns_ipc_ring_buffer.h"

/// Audio IPC Modes
typedef enum {
    NS_AUDIO_API_CALLBACK,   ///< App-defined callback is invoked when audio is ready
    NS_AUDIO_API_RINGBUFFER, ///< FreeRTOS ringbuffer is used to send audio to app
    NS_AUDIO_API_TASK,       ///< FreeRTOS task event (TODO)
} ns_audio_api_mode_e;

/// Audio Source (current only AUDADC is supported)
typedef enum {
    NS_AUDIO_SOURCE_AUDADC  ///< Collect data from AUDADC
} ns_audio_source_e;

// Forward declaration to get around using it in cb
struct am_ai_acfg;

/// Invoked by IRQ when audio buffer is ready
typedef void (*ns_audio_callback_cb)(struct am_ai_acfg *, uint16_t);

/// NeuralSPOT Audio API Configuration Struct
/// 
/// Audio configuration is via this struct, which also serves
/// as a handle after ns_audio_init() has been invoked
/// 
typedef struct am_ai_acfg {
    /** API Config */
    ns_audio_api_mode_e eAudioApiMode; /**< Defines how the audio system will
                                          interact with the applications */

    /** IPC */
    ns_audio_callback_cb callback; ////< Invoked when there is audio in buffer
    void *audioBuffer; ///< Where the audio will be located when callback occurs

    /** Audio Config */
    ns_audio_source_e eAudioSource; ///< Choose audio source such as AUDADC
    uint8_t numChannels; ///< Number of audio channels, currently 1 or 2
    uint16_t numSamples; ///< Samples collected per callback
    uint16_t sampleRate; ///< In Hz

    /** Internals */
    void *audioSystemHandle;                  ///< Handle, filled by init
    ns_ipc_ring_buffer_t *bufferHandle; ///< Filled by init

} ns_audio_config_t;

extern ns_audio_config_t g_ns_audio_config;

/**
 * @brief Initialize NeuralSPOT audio capture library
 *
 * @param cfg : desired confiugration
 */
extern void ns_audio_init(ns_audio_config_t *);

#ifdef __cplusplus
}
#endif
/** @}*/
#endif // NS_AUDIO