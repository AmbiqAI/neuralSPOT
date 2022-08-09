//*****************************************************************************
//
//! @file audio.c
//!
//! @brief A set of libraries for capturing audio from AUDADC.
//!
//! Purpose: Utitlies for initializing the AUDADC and capturing audio to a
//! buffer
//!
//! Additional Information:
//! @RTT Streaming: Run rtt_logger.py to capture pcm raw data via PC.
//!                 Should modify -RTTAddress in rtt_logger.py to _SEGGER_RTT
//!                 address in audadc_rtt_stream.map The data saved as
//!                 stereo(L:low gain/R: high gain)
//!
//! @pcm_to_wav.py:  Convert pcm raw data to wav file.
//!                  Save mono left channel to destination file.
//!
//
//*****************************************************************************

//*****************************************************************************
//
// ${copyright}
//
// This is part of revision ${version} of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "ns_audadc.h"
#include "ns_audio.h"
#include "ns_ipc_ring_buffer.h"

//
// RTT streaming buffer
//
#ifdef AUDIODEBUG
    #include "SEGGER_RTT.h"
    #define TIMEOUT 400000 // RTT streaming timeout loop count
    #define AUDIO_SAMPLE_TO_RTT (64 * 512)
    #define RTT_BUFFER_LENGTH (AUDIO_SAMPLE_TO_RTT * 2)
uint8_t g_rttRecorderBuffer[RTT_BUFFER_LENGTH];
AM_SHARED_RW int16_t g_in16SampleToRTT[AUDIO_SAMPLE_TO_RTT];
uint32_t g_ui32SampleToRTT = 0;
#endif

// Populate with sane defaults
ns_audio_config_t g_ns_audio_config = {.eAudioApiMode = NS_AUDIO_API_CALLBACK,
                                       .callback = NULL,
                                       .audioBuffer = NULL,
                                       .eAudioSource = NS_AUDIO_SOURCE_AUDADC,
                                       .numChannels = 1,
                                       .numSamples = 480,
                                       .sampleRate = 16000,
                                       .audioSystemHandle = NULL,
                                       .bufferHandle = NULL};

//*****************************************************************************
//
// Initialize audio system
//
//*****************************************************************************
void
ns_audio_init(ns_audio_config_t *cfg) {
    // Copy in config
    g_ns_audio_config.eAudioApiMode = cfg->eAudioApiMode;
    g_ns_audio_config.callback = cfg->callback;
    g_ns_audio_config.callback = cfg->callback;
    g_ns_audio_config.audioBuffer = cfg->audioBuffer;
    g_ns_audio_config.numChannels = cfg->numChannels;
    g_ns_audio_config.numSamples = cfg->numSamples;
    g_ns_audio_config.sampleRate = cfg->sampleRate;
    g_ns_audio_config.eAudioSource = cfg->eAudioSource;

    if (g_ns_audio_config.eAudioApiMode == NS_AUDIO_API_RINGBUFFER) {
        // init a ringbuffer
        am_app_utils_ringbuff_setup_t setup = {
            .indx = 0,
            .pData = g_ns_audio_config.audioBuffer,
            .ui32ByteSize = g_ns_audio_config.numSamples * 2 * 2};

        am_app_utils_ring_buffer_init(cfg->bufferHandle, setup);
        g_ns_audio_config.bufferHandle = cfg->bufferHandle;
    }

    if (g_ns_audio_config.eAudioSource == NS_AUDIO_SOURCE_AUDADC) {
        audadc_init();
    }

#ifdef AUDIODEBUG
    SEGGER_RTT_Init();
    SEGGER_RTT_ConfigUpBuffer(1, "DataLogger", g_rttRecorderBuffer,
                              RTT_BUFFER_LENGTH, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    am_util_stdio_printf("RTT Control Block Address:  0x%08X\n",
                         (uint32_t)&_SEGGER_RTT);
#endif
}