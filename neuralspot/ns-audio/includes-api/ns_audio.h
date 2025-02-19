/**
 * @file ns_audio.h
 * @author Carlos Morales
 * @brief API Definition for NeuralSPOT Audio library
 * @version 0.1
 * @date 2022-08-12
 *
 * @copyright Copyright (c) 2022
 *
 * \addtogroup ns-audio
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
    #include "ns_core.h"
    #include "ns_ipc_ring_buffer.h"

    #define NS_AUDIO_V0_0_1                                                                        \
        { .major = 0, .minor = 0, .revision = 1 }
    #define NS_AUDIO_V1_0_0                                                                        \
        { .major = 1, .minor = 0, .revision = 0 }
    #define NS_AUDIO_V2_0_0                                                                        \
        { .major = 2, .minor = 0, .revision = 0 }
    #define NS_AUDIO_V2_1_0                                                                        \
        { .major = 2, .minor = 1, .revision = 0 }
    #define NS_AUDIO_OLDEST_SUPPORTED_VERSION NS_AUDIO_V0_0_1
    #define NS_AUDIO_CURRENT_VERSION NS_AUDIO_V2_1_0
    #define NS_AUDIO_API_ID 0xCA0001

extern const ns_core_api_t ns_audio_V0_0_1;
extern const ns_core_api_t ns_audio_V1_0_0;
extern const ns_core_api_t ns_audio_V2_0_0;
extern const ns_core_api_t ns_audio_V2_1_0;
extern const ns_core_api_t ns_audio_oldest_supported_version;
extern const ns_core_api_t ns_audio_current_version;

    #ifndef NS_AUDIO_DMA_BUFFER_SIZE
        #define NS_AUDIO_DMA_BUFFER_SIZE 480
    #endif

/// Audio IPC Modes
typedef enum {
    NS_AUDIO_API_CALLBACK,   ///< App-defined callback is invoked when audio is ready
    NS_AUDIO_API_RINGBUFFER, ///< FreeRTOS ringbuffer is used to send audio to app
    NS_AUDIO_API_TASK,       ///< FreeRTOS task event (TODO)
} ns_audio_api_mode_e;

/// Audio Source (current only AUDADC and PDM are supported)
typedef enum {
    NS_AUDIO_SOURCE_AUDADC, ///< Collect data from AUDADC
    NS_AUDIO_SOURCE_PDM,    ///< Collect data from PDM
} ns_audio_source_e;

/// Audio Clock Source
typedef enum {
    NS_CLKSEL_XTHS,
    NS_CLKSEL_HFXTAL,
    NS_CLKSEL_HFRC,
    NS_CLKSEL_HFRC2,
    NS_CLKSEL_HFRC2_ADJ,
    NS_CLKSEL_PLL, // ap5b only
} ns_audio_clksel_e;

/// @brief AUDADC Configuration
typedef struct {
    ns_audio_clksel_e clock;
    bool low_power_mode;
    bool repeating_trigger_mode;
    bool dcmp_enable;
    int left_gain;
    int right_gain;
} ns_audadc_cfg_t;

/// @brief PDM Clock Frequency
typedef enum {
    NS_AUDIO_PDM_CLK_750KHZ,
    NS_AUDIO_PDM_CLK_1_5MHZ,
} ns_audio_pdm_clock_e;

/// @brief  PDM Microphone Selection
typedef enum {
    NS_AUDIO_PDM_MICBOARD_0 = 0, // value corresponds to PDM Module
    NS_AUDIO_PDM_MICBOARD_1 = 2,
    NS_AUDIO_PDM_MICBOARD_2 = 1,
} ns_audio_pdm_micsel_e;

/// @brief PDM Configuration
typedef struct {
    ns_audio_clksel_e clock;
    ns_audio_pdm_clock_e clock_freq;
    ns_audio_pdm_micsel_e mic; ///< VoS Kit breakout board PDM mic slot
    uint8_t numBytes;          // size of sample word in bytes
    am_hal_pdm_gain_e left_gain;
    am_hal_pdm_gain_e right_gain;
} ns_pdm_cfg_t;

// Forward declaration to get around using it in cb
struct ns_audio_cfg;

/// Invoked by IRQ when audio buffer is ready
typedef void (*ns_audio_callback_cb)(struct ns_audio_cfg *, uint16_t);

/// NeuralSPOT Audio API Configuration Struct
///
/// Audio configuration is via this struct, which also serves
/// as a handle after ns_audio_init() has been invoked
///
typedef struct ns_audio_cfg {
    const ns_core_api_t *api;          ///< API prefix
    ns_audio_api_mode_e eAudioApiMode; /**< Defines how the audio system will
                                            interact with the applications */

    /** IPC */
    ns_audio_callback_cb callback; ///< Invoked when there is audio in buffer
    void *audioBuffer;             ///< Where the audio will be located when callback occurs

    /** Audio Config */
    ns_audio_source_e eAudioSource; ///< Choose audio source such as AUDADC
    uint32_t *sampleBuffer;         ///< Where samples are DMA'd to

    #ifndef NS_AUDADC_PRESENT
    void *workingBuffer; ///< Not used for Apollo4 Lite
    #else
    am_hal_audadc_sample_t *workingBuffer; ///< Working buffer used by AUDADC, otherwise NULL
    #endif

    uint8_t numChannels; ///< Number of audio channels, currently 1 or 2
    uint16_t numSamples; ///< Samples collected per callback
    uint16_t sampleRate; ///< In Hz

    /** AUDADC Config - only used by audadc driver */
    ns_audadc_cfg_t *audadc_config;

    /** PDM Config - only used by the pdm driver*/
    ns_pdm_cfg_t *pdm_config;
    am_hal_pdm_transfer_t sTransfer;

    /** Internals */
    void *audioSystemHandle;            ///< Handle, filled by init
    ns_ipc_ring_buffer_t *bufferHandle; ///< Filled by init
    float fLGAdB;

    #if defined(NS_AMBIQSUITE_VERSION_R4_1_0) || defined(AM_PART_APOLLO4L) ||                      \
        defined(AM_PART_APOLLO3P) || defined(AM_PART_APOLLO3)
    void *sOffsetCalib;
    #else
    am_hal_offset_cal_coeffs_array_t *sOffsetCalib;
    #endif
} ns_audio_config_t;

extern ns_audio_config_t *g_ns_audio_config;

/**
 * @brief Initialize NeuralSPOT audio capture library
 *
 * @param cfg : desired configuration
 */
extern uint32_t ns_audio_init(ns_audio_config_t *);

/**
 * @brief Start audio capture, must be called after ns_audio_init
 *
 * @param cfg : desired configuration
 */
extern uint32_t ns_start_audio(ns_audio_config_t *);

/**
 * @brief Stop audio capture
 *
 * @param cfg : desired configuration
 */
extern uint32_t ns_end_audio(ns_audio_config_t *);

/**
 * @brief Extract int16 PCM from data collected by ADC
 *
 * @param pcm - resulting PCM data
 * @param raw - incoming data from ADC engine
 * @param len - number of sample words to convert
 */
extern void ns_audio_getPCM(int16_t *pcm, uint32_t *raw, int16_t len);

/**
 * @brief Extract int16 PCM from AUDADC or PDM sources
 *
 * @param config - ns audio config
 * @param pcm - resulting PCM data
 */
extern void ns_audio_getPCM_v2(ns_audio_config_t *config, void *pcm);

/**
 * @brief Set gain of audio source
 *
 * @param left_gain - left channel gain
 * @param right_gain - right channel gain
 */
extern uint32_t ns_audio_set_gain(int left_gain, int right_gain);
    #ifdef __cplusplus
}
    #endif
#endif // NS_AUDIO
       /** @}*/
