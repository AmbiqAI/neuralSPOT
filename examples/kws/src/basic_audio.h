/**
 * @file basic_audio.h
 * @author Carlos Morales
 * @brief Typical Audio config
 * @version 2.0
 * @date 2023-05-24
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "basic_tf_stub.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_audio.h"


// Audio and IPC Config
// Set by app when it wants to start recording, used by callback
bool volatile static audioRecording = false;

/// Set by callback when audio buffer has been copied, cleared by
/// app when the buffer has been consumed.
bool volatile static audioReady = false;


// Audio buffers for application

#if NUM_CHANNELS == 1
alignas(16) int16_t static audioDataBuffer[SAMPLES_IN_FRAME]; // incoming PCM audio data
#else
alignas(16) int32_t static audioDataBuffer[SAMPLES_IN_FRAME];
#endif

alignas(16) uint32_t static dmaBuffer[SAMPLES_IN_FRAME * NUM_CHANNELS * 2];   // DMA target
    
#ifndef USE_PDM_MICROPHONE
alignas(16) am_hal_audadc_sample_t static workingBuffer[SAMPLES_IN_FRAME * NUM_CHANNELS]; // working buffer used  // by AUDADC                                                                          
#endif // USE_PDM_MICROPHONE

#if !defined(NS_AMBIQSUITE_VERSION_R4_1_0) && defined(NS_AUDADC_PRESENT)
am_hal_offset_cal_coeffs_array_t sOffsetCalib;
#endif

/**
 *
 * @brief Audio Callback (user-defined, executes in IRQ context)
 *
 * When the 'audioRecording' flag is set, copy the latest sample to a buffer
 * and set a 'ready' flag. If recording flag isn't set, discard buffer.
 * If 'ready' flag is still set, the last buffer hasn't been consumed yet,
 * print a debug message and overwrite.
 *
 */
static void audio_frame_callback(ns_audio_config_t *config, uint16_t bytesCollected) {
   if (audioRecording) {
        ns_audio_getPCM_v2(config, audioDataBuffer);
        audioReady = true;
    }
}

/**
 * @brief NeuralSPOT Audio config struct
 *
 * Populate this struct before calling ns_audio_config()
 *
 */
static ns_audio_config_t audio_config = {

#ifdef DYNAMIC_AUDIO_SOURCE
    .api = &ns_audio_V2_1_0,
#else
    .api = &ns_audio_V2_0_0,
#endif

    .eAudioApiMode = NS_AUDIO_API_CALLBACK,
    .callback = audio_frame_callback,
    .audioBuffer = (void *)&audioDataBuffer,

#ifdef USE_PDM_MICROPHONE
    .eAudioSource = NS_AUDIO_SOURCE_PDM,
#else
    .eAudioSource = NS_AUDIO_SOURCE_AUDADC,
#endif
    .sampleBuffer = dmaBuffer,
#if !defined(AUDIO_LEGACY) && defined(NS_AUDADC_PRESENT) && !defined(USE_PDM_MICROPHONE)
    .workingBuffer = workingBuffer,
#endif
    .numChannels = NUM_CHANNELS,
    .numSamples = SAMPLES_IN_FRAME,
    .sampleRate = SAMPLE_RATE,
    .audioSystemHandle = NULL, // filled in by audio_init()

    .bufferHandle = NULL,
#if !defined(NS_AMBIQSUITE_VERSION_R4_1_0) && defined(NS_AUDADC_PRESENT)
    .sOffsetCalib = &sOffsetCalib,
#endif
};
