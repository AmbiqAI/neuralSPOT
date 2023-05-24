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

#ifdef RINGBUFFER_MODE
    #include "ns_ipc_ring_buffer.h"
#endif
#ifdef AUDIODEBUG
    #include "SEGGER_RTT.h"
#endif

// Audio and IPC Config
// Set by app when it wants to start recording, used by callback
bool volatile static audioRecording = false;

/// Set by callback when audio buffer has been copied, cleared by
/// app when the buffer has been consumed.
bool volatile static audioReady = false;

#ifdef RINGBUFFER_MODE
// Ringbuffer storage
ns_ipc_ring_buffer_t audioBuf[1];
static uint8_t pui8AudioBuff[SAMPLES_IN_FRAME * 2 * 2]; // two buffers, 2 bytes/entry
#endif

// Audio buffers for application
#ifdef AUDIO_LEGACY
int16_t static audioDataBuffer[SAMPLES_IN_FRAME * 2];
uint32_t static dmaBuffer[SAMPLES_IN_FRAME * 2 + 3];
#else
    #if NUM_CHANNELS == 1
int16_t static audioDataBuffer[SAMPLES_IN_FRAME]; // incoming PCM audio data
    #else
int32_t static audioDataBuffer[SAMPLES_IN_FRAME];
    #endif
alignas(16) uint32_t static dmaBuffer[SAMPLES_IN_FRAME * NUM_CHANNELS * 2]; // DMA target
am_hal_audadc_sample_t static workingBuffer[SAMPLES_IN_FRAME *
                                            NUM_CHANNELS]; // working buffer used by AUDADC
#endif

/**
 *
 * @brief Audio Callback (user-defined, executes in IRQ context)
 *
 * When the 'g_audioRecording' flag is set, copy the latest sample to a buffer
 * and set a 'ready' flag. If recording flag isn't set, discard buffer.
 * If 'ready' flag is still set, the last buffer hasn't been consumed yet,
 * print a debug message and overwrite.
 *
 */
static void
audio_frame_callback(ns_audio_config_t *config, uint16_t bytesCollected) {
#ifdef AUDIO_LEGACY
    uint32_t *pui32_buffer = (uint32_t *)am_hal_audadc_dma_get_buffer(config->audioSystemHandle);
#endif

    if (audioRecording) {
// if (audioReady) {
//     ns_printf("Warning - audio buffer wasnt consumed in time\n");
// }

// Raw PCM data is 32b (14b/channel) - convert it to 16b PCM
#ifdef AUDIO_LEGACY
        ns_audio_getPCM(audioDataBuffer, pui32_buffer, config->numSamples);
#else
        ns_audio_getPCM_v2(config, audioDataBuffer);
#endif

#ifdef RINGBUFFER_MODE
        ns_ipc_ring_buffer_push(&(config->bufferHandle[0]), audioDataBuffer,
                                (config->numSamples * 2), // in bytes
                                false);
#endif
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
#ifdef AUDIO_LEGACY
    .api = &ns_audio_V1_0_0,
#else
    .api = &ns_audio_V2_0_0,
#endif
#ifdef RINGBUFFER_MODE
    .eAudioApiMode = NS_AUDIO_API_RINGBUFFER,
    .callback = audio_frame_callback,
    .audioBuffer = (void *)&pui8AudioBuff,
#else
    .eAudioApiMode = NS_AUDIO_API_CALLBACK,
    .callback = audio_frame_callback,
    .audioBuffer = (void *)&in16AudioDataBuffer,
#endif
    .eAudioSource = NS_AUDIO_SOURCE_PDM,
    .sampleBuffer = dmaBuffer,
#ifndef AUDIO_LEGACY
    .workingBuffer = workingBuffer,
#endif
    .numChannels = NUM_CHANNELS,
    .numSamples = SAMPLES_IN_FRAME,
    .sampleRate = SAMPLE_RATE,
    .audioSystemHandle = NULL, // filled in by audio_init()
#ifdef RINGBUFFER_MODE
    .bufferHandle = audioBuf
#else
    .bufferHandle = NULL
#endif
};
