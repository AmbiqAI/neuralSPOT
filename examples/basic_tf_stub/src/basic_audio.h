/**
 * @file audio.h
 * @author Carlos Morales
 * @brief Typical Audio config
 * @version 0.1
 * @date 2022-10-26
 *
 * @copyright Copyright (c) 2022
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
int16_t static in16AudioDataBuffer[SAMPLES_IN_FRAME * 2];
uint32_t static audadcSampleBuffer[SAMPLES_IN_FRAME * 2 + 3];

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
    uint32_t *pui32_buffer = (uint32_t *)am_hal_audadc_dma_get_buffer(config->audioSystemHandle);

    if (audioRecording) {
        // if (audioReady) {
        //     ns_printf("Warning - audio buffer wasnt consumed in time\n");
        // }

        // Raw PCM data is 32b (14b/channel) - here we only care about one
        // channel For ringbuffer mode, this loop may feel extraneous, but it is
        // needed because ringbuffers are treated a blocks, so there is no way
        // to convert 32b->16b
        for (int i = 0; i < config->numSamples; i++) {
            in16AudioDataBuffer[i] = (int16_t)(pui32_buffer[i] & 0x0000FFF0);
            if (i == 4) {
                // Workaround for AUDADC sample glitch, here while it is root caused
                in16AudioDataBuffer[3] = (in16AudioDataBuffer[2] + in16AudioDataBuffer[4]) / 2;
            }
        }

#ifdef RINGBUFFER_MODE
        ns_ipc_ring_buffer_push(&(config->bufferHandle[0]), in16AudioDataBuffer,
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
static ns_audio_config_t audio_config = {.prefix = {0},
#ifdef RINGBUFFER_MODE
                                         .eAudioApiMode = NS_AUDIO_API_RINGBUFFER,
                                         .callback = audio_frame_callback,
                                         .audioBuffer = (void *)&pui8AudioBuff,
#else
                                         .eAudioApiMode = NS_AUDIO_API_CALLBACK,
                                         .callback = audio_frame_callback,
                                         .audioBuffer = (void *)&in16AudioDataBuffer,
#endif
                                         .eAudioSource = NS_AUDIO_SOURCE_AUDADC,
                                         .sampleBuffer = audadcSampleBuffer,
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
