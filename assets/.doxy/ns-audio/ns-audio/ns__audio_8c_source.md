

# File ns\_audio.c

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-audio**](dir_45211a8475460839574f71aa108f4957.md) **>** [**src**](dir_e70eef2d5115541d1d6cb7ad27f30382.md) **>** [**ns\_audio.c**](ns__audio_8c.md)

[Go to the documentation of this file](ns__audio_8c.md)

```C++


//*****************************************************************************
//
// ${copyright}
//
// This is part of revision ${version} of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include "ns_audio.h"
#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"

#ifndef AM_PART_APOLLO4L
    #include "ns_audadc.h"
#endif

#include "ns_ipc_ring_buffer.h"
#include "ns_pdm.h"

const ns_core_api_t ns_audio_V0_0_1 = {.apiId = NS_AUDIO_API_ID, .version = NS_AUDIO_V0_0_1};
const ns_core_api_t ns_audio_V1_0_0 = {.apiId = NS_AUDIO_API_ID, .version = NS_AUDIO_V1_0_0};
const ns_core_api_t ns_audio_V2_0_0 = {.apiId = NS_AUDIO_API_ID, .version = NS_AUDIO_V2_0_0};
const ns_core_api_t ns_audio_oldest_supported_version = {
    .apiId = NS_AUDIO_API_ID, .version = NS_AUDIO_OLDEST_SUPPORTED_VERSION};
const ns_core_api_t ns_audio_current_version = {
    .apiId = NS_AUDIO_API_ID, .version = NS_AUDIO_CURRENT_VERSION};

#ifdef NS_RTT_AUDIODEBUG
    #include "SEGGER_RTT.h"
    #define TIMEOUT 400000 // RTT streaming timeout loop count
    #define AUDIO_SAMPLE_TO_RTT (64 * 512)
    #define RTT_BUFFER_LENGTH (AUDIO_SAMPLE_TO_RTT * 2)
uint8_t g_rttRecorderBuffer[RTT_BUFFER_LENGTH];
AM_SHARED_RW int16_t g_in16SampleToRTT[AUDIO_SAMPLE_TO_RTT];
uint32_t g_ui32SampleToRTT = 0;
#endif

ns_audio_config_t *g_ns_audio_config = NULL;

uint32_t ns_audio_init(ns_audio_config_t *cfg) {
#ifndef NS_DISABLE_API_VALIDATION
    // Check the handle.
    if (cfg == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }

    // check API version
    if (ns_core_check_api(
            cfg->api, &ns_audio_oldest_supported_version, &ns_audio_current_version)) {
        return NS_STATUS_INVALID_VERSION;
    }

    if ((cfg->callback == NULL) || (cfg->audioBuffer == NULL) || (cfg->sampleBuffer == NULL)) {
        return NS_STATUS_INVALID_CONFIG;
    }

    #ifdef AM_PART_APOLLO4L
    if (cfg->eAudioSource == NS_AUDIO_SOURCE_AUDADC) {
        return NS_STATUS_INVALID_CONFIG;
    }

    if (cfg->numChannels != 1) {
        return NS_STATUS_INVALID_CONFIG; // APOLLO4L only supports 1 channel
    }

    #endif

    if ((cfg->api->version.major != 1) && (cfg->eAudioSource == NS_AUDIO_SOURCE_AUDADC) &&
        (cfg->workingBuffer == NULL)) {
        return NS_STATUS_INVALID_CONFIG;
    }
    if (sizeof(*(cfg->sampleBuffer)) > cfg->numSamples * 2) {
        return NS_STATUS_INVALID_CONFIG;
    }
#endif

    // cfg->api->initialized = true;

    g_ns_audio_config = cfg;

    if (g_ns_audio_config->eAudioApiMode == NS_AUDIO_API_RINGBUFFER) {
        // init a ringbuffer
        ns_ipc_ringbuff_setup_t setup = {
            .indx = 0,
            .pData = g_ns_audio_config->audioBuffer,
            .ui32ByteSize = g_ns_audio_config->numSamples * 2};

        ns_ipc_ring_buffer_init(g_ns_audio_config->bufferHandle, setup);
        g_ns_audio_config->bufferHandle = g_ns_audio_config->bufferHandle;
    }

    if (g_ns_audio_config->eAudioSource == NS_AUDIO_SOURCE_AUDADC) {
#ifdef AM_PART_APOLLO4L
        am_util_stdio_printf("Error - Trying to init non-existant AUDADC on Apollo4 Lite\n");
        return NS_STATUS_INIT_FAILED;
#else
        if (g_ns_audio_config->audadc_config == NULL) {
            g_ns_audio_config->audadc_config = &ns_audadc_default;
        }

        if (audadc_init(cfg)) {
            return NS_STATUS_INIT_FAILED;
        }

        // Trigger the AUDADC sampling for the first time manually.
        if (AM_HAL_STATUS_SUCCESS !=
            am_hal_audadc_sw_trigger(g_ns_audio_config->audioSystemHandle)) {
            am_util_stdio_printf("Error - triggering the AUDADC failed.\n");
            return NS_STATUS_INIT_FAILED;
        }
#endif
    } else if (g_ns_audio_config->eAudioSource == NS_AUDIO_SOURCE_PDM) {
        if (g_ns_audio_config->pdm_config == NULL) {
            g_ns_audio_config->pdm_config = &ns_pdm_default;
        }

        if (pdm_init(g_ns_audio_config)) {
            return NS_STATUS_INIT_FAILED;
        }

    } else {
        return NS_STATUS_INVALID_CONFIG;
    }

#ifdef NS_RTT_AUDIODEBUG
    SEGGER_RTT_Init();
    SEGGER_RTT_ConfigUpBuffer(
        1, "DataLogger", g_rttRecorderBuffer, RTT_BUFFER_LENGTH, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    am_util_stdio_printf("RTT Control Block Address:  0x%08X\n", (uint32_t)&_SEGGER_RTT);
#endif

    return AM_HAL_STATUS_SUCCESS;
}

// static uint32_t synthData = 0;
// static void
// gen_synthetic_audadc(ns_audio_config_t *config, uint32_t cnt) {
//     uint8_t channel = 0;
//     for (int i = 0; i < cnt; i++) {
//         // Generate synthetic ADC data
//         config->sampleBuffer[i] = (synthData & 0xFFF) << 4;
//         config->sampleBuffer[i] |= channel << 19;
//         synthData++;
//         channel = (channel + 1) % 2;
//     }
// }

void ns_audio_getPCM(int16_t *pcm, uint32_t *raw, int16_t len) {
    if (g_ns_audio_config->api->version.major < 2) {
        for (int i = 0; i < len; i++) {
            pcm[i] = (int16_t)(raw[i] & 0x0000FFF0);
            if (i == 4) {
                // Workaround for AUDADC sample glitch, here while it is root caused
                pcm[3] = (pcm[2] + pcm[4]) / 2;
            }
        }
    } else {
        ns_audio_getPCM_v2(g_ns_audio_config, pcm);
    }
}

void ns_audio_getPCM_v2(ns_audio_config_t *config, void *pcm) {
#ifdef AM_PART_APOLLO4L
    // Apollo4 Lite only supports PDM, and there is nothing to do for PDM here
    return;
#else
    uint32_t ui32PcmSampleCnt = config->numSamples * config->numChannels;

    if (config->eAudioSource == NS_AUDIO_SOURCE_AUDADC) {
        uint32_t LeftChCount = 0;
        uint32_t RightChCount = 0;

        uint32_t *pcm32 = (uint32_t *)pcm;
        uint16_t *pcm16 = (uint16_t *)pcm;

        // gen_synthetic_audadc(config, ui32PcmSampleCnt);
    #ifdef NS_AMBIQSUITE_VERSION_R4_1_0
        am_hal_audadc_samples_read(
            config->audioSystemHandle, config->sampleBuffer, &ui32PcmSampleCnt, true,
            config->workingBuffer, false, NULL);
    #else
        am_hal_audadc_samples_read(
            config->audioSystemHandle, config->sampleBuffer, &ui32PcmSampleCnt, true,
            config->workingBuffer, false, NULL, config->sOffsetCalib);
    #endif
        for (int i = 0; i < ui32PcmSampleCnt; i++) {
            if (config->numChannels == 1) {
                pcm16[i] = config->workingBuffer[i].int16Sample;
            } else {
                if (config->workingBuffer[i].ui16AudChannel == 0) {
                    // Low gain samples (MIC0) data to left channel.
                    pcm32[LeftChCount++] = (config->workingBuffer[i].int16Sample & 0x0000FFFF);
                } else {
                    // right channel (MIC2, MIC3) data
                    pcm32[RightChCount++] |= ((config->workingBuffer[i].int16Sample) << 16);
                }
            }
        }
    } else if (config->eAudioSource == NS_AUDIO_SOURCE_PDM) {
        // ISR current does the work, do nothing here
        // uint8_t *pcm8 = (uint8_t *)pcm;

        // for ( uint32_t i = 0; i < ui32PcmSampleCnt; i++ ){
        //     pcm8[2 * i] = (config->sampleBuffer[i] & 0xFF00) >> 8U;
        //     pcm8[2 * i + 1] = (config->sampleBuffer[i] & 0xFF0000) >> 16U;
        // }
    }
#endif
}

```
