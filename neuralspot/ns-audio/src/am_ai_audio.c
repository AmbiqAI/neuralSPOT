//*****************************************************************************
//
//! @file audio.c
//!
//! @brief A set of libraries for capturing audio from AUDADC.
//!
//! Purpose: Utitlies for initializing the AUDADC and capturing audio to a buffer
//!
//! Additional Information:
//! @RTT Streaming: Run rtt_logger.py to capture pcm raw data via PC.
//!                 Should modify -RTTAddress in rtt_logger.py to _SEGGER_RTT address in audadc_rtt_stream.map
//!                 The data saved as stereo(L:low gain/R: high gain)
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

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "am_app_utils_ring_buffer.h"
#include "am_ai_audio.h"
#include "am_ai_audadc.h"

//
// RTT streaming buffer
//
#ifdef AUDIODEBUG
#include "SEGGER_RTT.h"
#define TIMEOUT                     400000      // RTT streaming timeout loop count
#define AUDIO_SAMPLE_TO_RTT         (64*512)
#define RTT_BUFFER_LENGTH           (AUDIO_SAMPLE_TO_RTT * 2)
uint8_t g_rttRecorderBuffer[RTT_BUFFER_LENGTH];
AM_SHARED_RW int16_t g_in16SampleToRTT[AUDIO_SAMPLE_TO_RTT];
uint32_t g_ui32SampleToRTT = 0;
#endif

// Populate with sane defaults
am_ai_audio_config_t am_ai_audio_config = {
  .eAudioApiMode = AM_AI_AUDIO_API_CALLBACK,
  .callback = NULL,
  .audioBuffer = NULL,
  .eAudioSource  = AM_AI_AUDIO_SOURCE_AUDADC,
  .numChannels = 1,
  .numSamples = 480,
  .sampleRate = 16000,
  .audioSystemHandle = NULL,
  .bufferHandle = NULL
};

//*****************************************************************************
//
// Initialize audio system
//
//*****************************************************************************
void
audio_init(am_ai_audio_config_t *cfg)
{
    // Copy in config
    am_ai_audio_config.eAudioApiMode = cfg->eAudioApiMode;
    am_ai_audio_config.callback = cfg->callback;
    am_ai_audio_config.callback = cfg->callback;
    am_ai_audio_config.audioBuffer = cfg->audioBuffer;
    am_ai_audio_config.numChannels = cfg->numChannels;
    am_ai_audio_config.numSamples = cfg->numSamples;
    am_ai_audio_config.sampleRate = cfg->sampleRate;
    am_ai_audio_config.eAudioSource  = cfg->eAudioSource;

    if (am_ai_audio_config.eAudioApiMode == AM_AI_AUDIO_API_RINGBUFFER) {
        // init a ringbuffer
        am_app_utils_ringbuff_setup_t setup = {
            .indx = 0,
            .pData = am_ai_audio_config.audioBuffer,
            .ui32ByteSize = am_ai_audio_config.numSamples * 2 * 2
        };

        am_app_utils_ring_buffer_init(cfg->bufferHandle, setup);
        am_ai_audio_config.bufferHandle  = cfg->bufferHandle;
    }

    if (am_ai_audio_config.eAudioSource == AM_AI_AUDIO_SOURCE_AUDADC){
        audadc_init();
    }

  #ifdef AUDIODEBUG
    SEGGER_RTT_Init();
    SEGGER_RTT_ConfigUpBuffer(1, "DataLogger", g_rttRecorderBuffer, RTT_BUFFER_LENGTH, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    am_util_stdio_printf("RTT Control Block Address:  0x%08X\n", (uint32_t)&_SEGGER_RTT);
  #endif
}