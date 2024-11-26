/**
 *
 *  @file ns_audio.c
 *
 *  @brief Implementation of the NeuralSPOT ns-audio API
 *
 *  Purpose: A single point of entry for capturing Ambiq Audio
 *
 **/

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
#include "ns_audadc.h"
#include "ns_ipc_ring_buffer.h"
#include "ns_pdm.h"

const ns_core_api_t ns_audio_V0_0_1 = {.apiId = NS_AUDIO_API_ID, .version = NS_AUDIO_V0_0_1};
const ns_core_api_t ns_audio_V1_0_0 = {.apiId = NS_AUDIO_API_ID, .version = NS_AUDIO_V1_0_0};
const ns_core_api_t ns_audio_V2_0_0 = {.apiId = NS_AUDIO_API_ID, .version = NS_AUDIO_V2_0_0};
const ns_core_api_t ns_audio_V2_1_0 = {.apiId = NS_AUDIO_API_ID, .version = NS_AUDIO_V2_1_0};
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
static bool audio_initialized = false;

/**
 * @brief Audio Configuration and State
 *
 */
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

    if ((cfg->callback == NULL) || (cfg->audioBuffer == NULL) || (cfg->sampleBuffer == NULL) || ((uintptr_t)cfg->audioBuffer % 2 != 0)) {
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
    
    if ((cfg->numChannels > 2) || ((cfg->sampleRate  != 8000) && cfg->sampleRate != 16000)) {
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
#ifndef NS_AUDADC_PRESENT
        am_util_stdio_printf("Error - Trying to init non-existent AUDADC\n");
        return NS_STATUS_INIT_FAILED;
#endif
        // If api doesn't support dynamic audio source, initialize audadc here
        if (ns_core_check_api(g_ns_audio_config->api, &ns_audio_oldest_supported_version, &ns_audio_V2_0_0) == NS_STATUS_SUCCESS) {
            // initialize audadc
            if(ns_start_audio(g_ns_audio_config) != NS_STATUS_SUCCESS) {
                return NS_STATUS_INIT_FAILED;
            }
        }
    }
    else if (g_ns_audio_config->eAudioSource == NS_AUDIO_SOURCE_PDM) {
        // If api doesn't support dynamic audio source, initialize pdm here
        if (ns_core_check_api(g_ns_audio_config->api, &ns_audio_oldest_supported_version, &ns_audio_V2_0_0) == NS_STATUS_SUCCESS) {
            // initialize pdm
            if(ns_start_audio(g_ns_audio_config) != NS_STATUS_SUCCESS) {
                return NS_STATUS_INIT_FAILED;
            }
        }
    }
    else {
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

uint32_t ns_start_audio(ns_audio_config_t *cfg) {
    if (g_ns_audio_config->eAudioSource == NS_AUDIO_SOURCE_AUDADC) {
#ifdef NS_AUDADC_PRESENT
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
#else
        return NS_STATUS_INIT_FAILED;
#endif
    } 
    else {
        if (g_ns_audio_config->pdm_config == NULL) {
            g_ns_audio_config->pdm_config = &ns_pdm_default;
        }
        if (pdm_init(g_ns_audio_config)) {
            return NS_STATUS_INIT_FAILED;
        }
    }
    audio_initialized = true;
    return NS_STATUS_SUCCESS;
}

uint32_t ns_end_audio(ns_audio_config_t *cfg) {
    if (g_ns_audio_config->eAudioSource == NS_AUDIO_SOURCE_AUDADC) {
       audadc_deinit(cfg);
       return NS_STATUS_SUCCESS;
    } else if (g_ns_audio_config->eAudioSource == NS_AUDIO_SOURCE_PDM) {
        pdm_deinit(cfg);
        return NS_STATUS_SUCCESS;
    }
    return NS_STATUS_INVALID_CONFIG;
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
#ifndef NS_AUDADC_PRESENT
    // Platform only supports PDM, and there is nothing to do for PDM here
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

uint32_t ns_audio_set_gain(am_hal_pdm_gain_e left_gain, am_hal_pdm_gain_e right_gain) {
    if (g_ns_audio_config->eAudioSource == NS_AUDIO_SOURCE_AUDADC) {
// #ifndef NS_AUDADC_PRESENT
//         return NS_STATUS_INVALID_CONFIG;
// #endif
//         // deinitialize audadc first        
//         ns_end_audio(g_ns_audio_config);
        return;
    }
    else if (g_ns_audio_config->eAudioSource == NS_AUDIO_SOURCE_PDM) {
        // check if we have already started the audio before. 
        if(audio_initialized) {
            ns_end_audio(g_ns_audio_config);
            g_ns_audio_config->pdm_config->left_gain = left_gain;
            g_ns_audio_config->pdm_config->right_gain = right_gain;
        }
        else {
            return NS_STATUS_FAILURE;
        }
        pdm_init(g_ns_audio_config);
    }
    return NS_STATUS_SUCCESS;
}


