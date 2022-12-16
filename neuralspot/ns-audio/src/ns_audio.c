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

const ns_core_api_t ns_audio_V0_0_1 = {.apiId = NS_AUDIO_API_ID, .version = NS_AUDIO_V0_0_1};

const ns_core_api_t ns_audio_V1_0_0 = {.apiId = NS_AUDIO_API_ID, .version = NS_AUDIO_V1_0_0};

const ns_core_api_t ns_audio_oldest_supported_version = {
    .apiId = NS_AUDIO_API_ID, .version = NS_AUDIO_OLDEST_SUPPORTED_VERSION};

const ns_core_api_t ns_audio_current_version = {.apiId = NS_AUDIO_API_ID,
                                                .version = NS_AUDIO_CURRENT_VERSION};

//
// Enable dumping audio to RTT via the AUDIODEBUG flag.
//
// Requires SSRAM to be enabled, so not compatible with some power modes
//
#ifdef NS_RTT_AUDIODEBUG
    #include "SEGGER_RTT.h"
    #define TIMEOUT 400000 // RTT streaming timeout loop count
    #define AUDIO_SAMPLE_TO_RTT (64 * 512)
    #define RTT_BUFFER_LENGTH (AUDIO_SAMPLE_TO_RTT * 2)
uint8_t g_rttRecorderBuffer[RTT_BUFFER_LENGTH];
AM_SHARED_RW int16_t g_in16SampleToRTT[AUDIO_SAMPLE_TO_RTT];
uint32_t g_ui32SampleToRTT = 0;
#endif

/**
 * @brief Audio Configuration and State
 *
 * This is populated here with some sane results, but it
 * isn't valid until futher initialized by ns_audio_init
 *
 */
ns_audio_config_t *g_ns_audio_config = NULL;

uint32_t
ns_audio_init(ns_audio_config_t *cfg) {

#ifndef NS_DISABLE_API_VALIDATION
    //
    // Check the handle.
    //
    if (cfg == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }

    // check API version
    if (ns_core_check_api(cfg->api, &ns_audio_oldest_supported_version,
                          &ns_audio_current_version)) {
        return NS_STATUS_INVALID_VERSION;
    }

    if ((cfg->callback == NULL) || (cfg->audioBuffer == NULL) || (cfg->sampleBuffer == NULL)) {
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
        ns_ipc_ringbuff_setup_t setup = {.indx = 0,
                                         .pData = g_ns_audio_config->audioBuffer,
                                         .ui32ByteSize = g_ns_audio_config->numSamples * 2 * 2};

        ns_ipc_ring_buffer_init(cfg->bufferHandle, setup);
        g_ns_audio_config->bufferHandle = cfg->bufferHandle;
    }

    if (g_ns_audio_config->eAudioSource == NS_AUDIO_SOURCE_AUDADC) {
        if (audadc_init()) {
            return NS_STATUS_INIT_FAILED;
        }
    }

#ifdef NS_RTT_AUDIODEBUG
    SEGGER_RTT_Init();
    SEGGER_RTT_ConfigUpBuffer(1, "DataLogger", g_rttRecorderBuffer, RTT_BUFFER_LENGTH,
                              SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    am_util_stdio_printf("RTT Control Block Address:  0x%08X\n", (uint32_t)&_SEGGER_RTT);
#endif

    return AM_HAL_STATUS_SUCCESS;
}

void
ns_audio_getPCM(int16_t *pcm, uint32_t *raw, int16_t len) {
    for (int i = 0; i < len; i++) {
        pcm[i] = (int16_t)(raw[i] & 0x0000FFF0);
        if (i == 4) {
            // Workaround for AUDADC sample glitch, here while it is root caused
            pcm[3] = (pcm[2] + pcm[4]) / 2;
        }
    }
}
