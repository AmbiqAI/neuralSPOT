

# File ns\_audio.h

[**File List**](files.md) **>** [**includes-api**](dir_b70d46c064802b213244316ef6218d52.md) **>** [**ns\_audio.h**](ns__audio_8h.md)

[Go to the documentation of this file](ns__audio_8h.md)

```C++


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

typedef enum {
    NS_AUDIO_API_CALLBACK,   
    NS_AUDIO_API_RINGBUFFER, 
    NS_AUDIO_API_TASK,       
} ns_audio_api_mode_e;

typedef enum {
    NS_AUDIO_SOURCE_AUDADC, 
    NS_AUDIO_SOURCE_PDM,    
} ns_audio_source_e;

typedef enum {
    NS_CLKSEL_XTHS,
    NS_CLKSEL_HFXTAL,
    NS_CLKSEL_HFRC,
    NS_CLKSEL_HFRC2,
    NS_CLKSEL_HFRC2_ADJ
} ns_audio_clksel_e;

typedef struct {
    ns_audio_clksel_e clock;
    bool low_power_mode;
    bool repeating_trigger_mode;
    bool dcmp_enable;
} ns_audadc_cfg_t;

typedef enum {
    NS_AUDIO_PDM_CLK_750KHZ,
    NS_AUDIO_PDM_CLK_1_5MHZ,
} ns_audio_pdm_clock_e;

typedef enum {
    NS_AUDIO_PDM_MICBOARD_0 = 0, // value corresponds to PDM Module
    NS_AUDIO_PDM_MICBOARD_1 = 2,
    NS_AUDIO_PDM_MICBOARD_2 = 1,
} ns_audio_pdm_micsel_e;

typedef struct {
    ns_audio_clksel_e clock;
    ns_audio_pdm_clock_e clock_freq;
    ns_audio_pdm_micsel_e mic; 
    uint8_t numBytes;          // size of sample word in bytes
} ns_pdm_cfg_t;

// Forward declaration to get around using it in cb
struct ns_audio_cfg;

typedef void (*ns_audio_callback_cb)(struct ns_audio_cfg *, uint16_t);

typedef struct ns_audio_cfg {
    const ns_core_api_t *api;          
    ns_audio_api_mode_e eAudioApiMode; 
    ns_audio_callback_cb callback; 
    void *audioBuffer;             

    ns_audio_source_e eAudioSource; 
    uint32_t *sampleBuffer;         

    #ifndef NS_AUDADC_PRESENT
    void *workingBuffer; 
    #else
    am_hal_audadc_sample_t *workingBuffer; 
    #endif

    uint8_t numChannels; 
    uint16_t numSamples; 
    uint16_t sampleRate; 

    ns_audadc_cfg_t *audadc_config;

    ns_pdm_cfg_t *pdm_config;
    am_hal_pdm_transfer_t sTransfer;

    void *audioSystemHandle;            
    ns_ipc_ring_buffer_t *bufferHandle; 
    float fLGAdB;

    #if defined(NS_AMBIQSUITE_VERSION_R4_1_0) || defined(AM_PART_APOLLO4L) ||                      \
        defined(AM_PART_APOLLO3P) || defined(AM_PART_APOLLO3)
    void *sOffsetCalib;
    #else
    am_hal_offset_cal_coeffs_array_t *sOffsetCalib;
    #endif
} ns_audio_config_t;

extern ns_audio_config_t *g_ns_audio_config;

extern uint32_t ns_audio_init(ns_audio_config_t *);


extern uint32_t ns_start_audio(ns_audio_config_t *);


extern uint32_t ns_end_audio(ns_audio_config_t *);


extern void ns_audio_getPCM(int16_t *pcm, uint32_t *raw, int16_t len);

extern void ns_audio_getPCM_v2(ns_audio_config_t *config, void *pcm);

    #ifdef __cplusplus
}
    #endif
#endif // NS_AUDIO

```

