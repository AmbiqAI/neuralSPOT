@page NS-Audio NeuralSPOT Audio Library
@tableofcontents

# NeuralSPOT Audio Library Subsystem
This library helps implement audio-based AI features on Ambiq's platform by simplifying common operations such as:
1. Reading audio data from peripherals such as the low-power analog microphone input (AUDADC) or certain digital mics (PDM)
2. Calculating common input features such as Mel Spectograms
3. Transferring data between the EVB and a PC (with the help of NeuralSPOT RPC)

NS-audio includes the following components:
1. `ns_audio`: this is the front-end to the audio sampling subsystem, and provides multiple mechanisms for accessing the collected data
2. `ns_audadc`: facilities for initializing and operating the AUDADC. Developers shouldn't have to access this system directly
3. `ns_pdm`: facilities for initializing and operating the PDM. Developers shouldn't have to access this system directly
4. `ns_mfcc`: implements an optimized, C-based MFCC calculator.

### Version 2.0.0 Release Notes

Version 2.0.0 adds features and fixes minor bugs. Taking advantage of these changes requires an API change, but backwards compatibility has been preserved via the API version feature.

New features include:

1. **PDM support**: PDM is now supported as an audio input source. Note that the EVB does not include digital microphones. PDM support has been tested with the VoS kit digital microphones.
2. **Dual-channel support**: 2 channel audio can now be captured for both AUDADC and PDM sources

To take advantage of the above features, several application layer changes are required. In summary:

1. The API version must be updated to 2.0.0
2. The various audio buffer declarations must be updated, and a new working buffer must be allocated
3. The audio interrupt callback may optionally be updated to save stack space.

To illustrate the needed changes, the [basic_tf_stub](../../examples/basic_tf_stub) example identifies the changed code with a compile switch, `AUDIO_LEGACY`.

## Using NS Audio to Sample Audio
Briefly, sampling audio is a matter of initializing this library, hooking into its IPC, and collecting data when it is available.

>*NOTE* The code snippets below are for documentation purposes only, and are copied from the [basic_tf_stub](https://github.com/AmbiqAI/neuralSPOT/tree/main/examples/basic_tf_stub) example. See [basic_audio.h](https://github.com/AmbiqAI/neuralSPOT/blob/main/examples/basic_tf_stub/src/basic_audio.h) for audio init, [basic_mfcc.h](https://github.com/AmbiqAI/neuralSPOT/blob/main/examples/basic_tf_stub/src/basic_mfcc.h) for mfcc init, and basic_tf_stub.cc for how an application uses the libraries.

There are several IPC methods available (see the IPC section below for more details). The following example starts sampling audio when the user presses button 0 on the EVB, and uses the NS_AUDIO_API_CALLBACK IPC method:

```c
#include "ns_audio.h"

bool    static g_audioRecording = false;
bool    static g_audioReady     = false;
#if NUM_CHANNELS == 1
int16_t static audioDataBuffer[SAMPLES_IN_FRAME]; // incoming PCM audio data
#else
int32_t static audioDataBuffer[SAMPLES_IN_FRAME]; // 32b for stereo
#endif

alignas(16) uint32_t static dmaBuffer[SAMPLES_IN_FRAME * NUM_CHANNELS * 2]; // DMA target
am_hal_audadc_sample_t static sLGSampleBuffer[SAMPLES_IN_FRAME * NUM_CHANNELS]; // working buffer used by AUDADC

#ifndef NS_AMBIQSUITE_VERSION_R4_1_0
am_hal_offset_cal_coeffs_array_t sOffsetCalib;
#endif

void
audio_frame_callback(ns_audio_config_t *config, uint16_t bytesCollected) {
    if (g_audioRecording) {
        if (g_audioReady) {
            ns_lp_printf("Overflow!\n");
        }
        ns_audio_getPCM_v2(config, audioDataBuffer);
        g_audioReady = true;
    }
}

ns_audio_config_t audioConfig = {
    .api = &ns_audio_V2_0_0,
    .eAudioApiMode = NS_AUDIO_API_CALLBACK,
    .callback = audio_frame_callback,
    .audioBuffer = (void *)&audioDataBuffer,
    .eAudioSource = NS_AUDIO_SOURCE_PDM, // or NS_AUDIO_SOURCE_AUDADC
    .sampleBuffer = dmaBuffer,
    .workingBuffer = sLGSampleBuffer, // only needed by ADC
    .numChannels = NUM_CHANNELS, // 1 or 2
    .numSamples = SAMPLES_IN_FRAME,
    .sampleRate = SAMPLE_RATE,
    .audioSystemHandle = NULL,     // filled in by init
    .bufferHandle = NULL,          // only for ringbuffer mode
#ifndef NS_AMBIQSUITE_VERSION_R4_1_0
    .sOffsetCalib = &sOffsetCalib,
#endif
};

main(void) {
    ns_audio_init(&audio_config);
    ns_start_audio(&audio_config);
    while (1) {
        if (g_intButtonPressed == 1) { // See NS-peripherals for button info
            g_audioRecording = true;   // Tell callback to start collecting audio
        }

        if (g_audioReady) {
            // The callback sets g_audioReady when g_in16AudioDataBuffer has a sample
            // Do something with it...
            ns_mfcc_compute(audioDataBuffer, &mfcc_buffer[mfcc_buffer_head]);
            ns_mfcc_compute(&mfcc_config, g_in16AudioDataBuffer,
                            &mfcc_buffer[mfcc_buffer_head]);
            mfcc_buffer_head += MY_MFCC_NUM_MFCC_COEFFS; // advance one frame
            g_audioReady = false;      // Tell callback we're ready for more
        }
    }
}
```

## Audio Inter-process Communication (IPC)
Capturing audio is an asynchronous task, partially handled by hardware peripherals. AI features which sample audio need a way to run in parallel with the capturing process - NeuralSPOT Audio leverages NeuralSPOT IPC to enable a few ways of doing this:
1. **Callback-based**: Invokes an IRQ-context callback defined by the application developer. The developer is responsible for buffer management.
2. **Ringbuffer-based**: adds a ping-ponging ringbuffer allowing the application to run while the next audio buffer is acquired. It builds on the callback approach, preserving flexibility.

### Audio IPC Modes
Audio IPC behavior is controlled by the ns_audio_config_t configuration parameter passed into `ns_audio_init`. The above example illustrates the callback approach, while the following example shows the `ringbuffer` approach.

```c
#include "ns_audio.h"

bool static g_audioRecording = false;
bool static g_audioReady     = false;
int16_t static g_in16AudioDataBuffer_irq[SAMPLES_IN_FRAME * 2];

// Ringbuffer-specific globals
ns_ipc_ring_buffer_t audioBuf[1];
static uint8_t pui8AudioBuff[SAMPLES_IN_FRAME * 2 * 2]; // two buffers, 2 bytes/entry
int16_t static g_in16AudioDataBuffer_app[SAMPLES_IN_FRAME * 2];
uint32_t static audadcSampleBuffer[SAMPLES_IN_FRAME * 2 + 3];

// This will be called by an IRQ handler
void audio_frame_callback(ns_audio_config_t *config, uint16_t bytesCollected) {

    if (g_audioRecording) {
        if (g_audioReady) {
            ns_lp_printf("Overflow!\n");
        }
        ns_audio_getPCM_v2(config, audioDataBuffer);


        // This is the different part of the callback.
        ns_ipc_ring_buffer_push(&(config->bufferHandle[0]), in16AudioDataBuffer,
                                (config->numSamples * 2), // in bytes
                                false);

        g_audioReady = true;

    }
}

// Desired Audio Configuration
ns_audio_config_t audio_config = {
    .eAudioApiMode = NS_AUDIO_API_RINGBUFFER, // Different from callback IPC
    .callback = audio_frame_callback,
    .audioBuffer = (void *)&pui8AudioBuff,    // Different from callback IPC
    .eAudioSource = NS_AUDIO_SOURCE_AUDADC,
    .numChannels = NUM_CHANNELS,
    .numSamples = SAMPLES_IN_FRAME,
    .sampleRate = SAMPLE_RATE,
    .audioSystemHandle = NULL,
    .bufferHandle = audioBuf                 // Different from callback IPC
};

main(void) {
    ns_audio_init(&audio_config);
    ns_start_audio(&audio_config);
    while (1) {
        if (g_intButtonPressed == 1) { // See NS-peripherals for button info
            g_audioRecording = true;   // Tell callback to start collecting audio
        }

        if (g_audioReady) {
            // The callback sets g_audioReady when g_in16AudioDataBuffer has a sample
            // Do something with it...
            ns_ipc_ring_buffer_pop(audioBuf,
                            &g_in16AudioDataBuffer_app,
                            audio_config.numSamples * 2);

            ns_mfcc_compute(&mfcc_config, g_in16AudioDataBuffer,
                            &mfcc_buffer[mfcc_buffer_head]);
            mfcc_buffer_head += MY_MFCC_NUM_MFCC_COEFFS; // advance one frame
            g_audioReady = false;      // Tell callback we're ready for more
        }
    }
}
```

# MFCC
Using the mel spectrogram feature calculator requires allocation of a memory arena and configuration of the library. The size of the arena is shown in the example code below.

```c
// MFCC Config
#define MY_MFCC_FRAME_LEN_POW2  512 // Next power of two size after SAMPLES_IN_FRAME
#define MY_MFCC_NUM_FBANK_BINS  40
#define MY_MFCC_NUM_MFCC_COEFFS 13

// Allocate memory for MFCC calculations
#define MFCC_ARENA_SIZE  32*(MY_MFCC_FRAME_LEN_POW2*2 + MY_MFCC_NUM_FBANK_BINS*(NS_MFCC_SIZEBINS+MY_MFCC_NUM_MFCC_COEFFS))
static uint8_t mfccArena[MFCC_ARENA_SIZE];
int16_t static g_in16AudioDataBuffer[SAMPLES_IN_FRAME * 2];

// Initialize the configuration structure
ns_mfcc_cfg_t mfcc_config = {
    .arena = mfccArena,
    .sample_frequency = SAMPLE_RATE,
    .num_fbank_bins = MY_MFCC_NUM_FBANK_BINS,
    .low_freq = 20,
    .high_freq = 8000,
    .num_frames = NUM_FRAMES,
    .num_coeffs = MY_MFCC_NUM_MFCC_COEFFS,
    .num_dec_bits = 4,
    .frame_shift_ms = 30,
    .frame_len_ms = 30,
    .frame_len = SAMPLES_IN_FRAME,
    .frame_len_pow2 = MY_MFCC_FRAME_LEN_POW2
};

main(void) {
    float mfcc_buffer[NUM_FRAMES * MY_MFCC_NUM_MFCC_COEFFS];

    ns_mfcc_init(&mfcc_config);

    // See ns_audio examples above to see how to invoke mfcc as part of sample process
}
```

### Version 2.1.0 Release Notes

Version 2.1.0 adds the ability to dynamically switch between PDM and AUDADC sources. Taking advantage of this feature requires an API change, but backwards compatibility has been preserved via the API version feature.

``` c
ns_audio_config_t audioConfig = {
    .api = &ns_audio_V2_1_0,
    .eAudioApiMode = NS_AUDIO_API_CALLBACK,
    .callback = audio_frame_callback,
    .audioBuffer = (void *)&audioDataBuffer,
    .eAudioSource = NS_AUDIO_SOURCE_PDM
    .sampleBuffer = dmaBuffer,
    .workingBuffer = sLGSampleBuffer, // only needed by ADC
    .numChannels = NUM_CHANNELS, // 1 or 2
    .numSamples = SAMPLES_IN_FRAME,
    .sampleRate = SAMPLE_RATE,
    .audioSystemHandle = NULL,     // filled in by init
    .bufferHandle = NULL,          // only for ringbuffer mode
#ifndef NS_AMBIQSUITE_VERSION_R4_1_0
    .sOffsetCalib = &sOffsetCalib,
#endif
};

main(void) {
    // init and start audio
    ns_audio_init(&audio_config);
    ns_start_audio(&audio_config);
    // switch from pdm to audadc
    ns_end_audio(&audio_config);
    audio_config.eAudioSource = NS_AUDIO_SOURCE_AUDADC;
    ns_start_audio(&audio_config);
}
```