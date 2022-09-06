@page NS-Audio NeuralSPOT Audio Library
@tableofcontents

# NeuralSPOT Audio Library Subsystem
This library helps implement audio-based AI features on Ambiq's platform by simplifying common operations such as:
1. Reading audio data from peripherals such as the low-power analog microphone input (AUDADC)
2. Calculating common input features such as Mel Spectograms
3. Transferring data between the EVB and a PC (with the help of NeuralSPOT RPC)

NS-audio includes the following components:
1. `ns_audio`: this is the front-end to the audio sampling subsystem, and provides multiple mechanisms for accessing the collected data
2. `ns_audadc`: facilities for initializing and operating the AUDADC. Developers shouldn't have to access this system directly
3. `ns_mfcc`: implements an optimized, C-based MFCC calculator.

## Using NS Audio to Sample Audio
Briefly, sampling audio is a matter of initializing this library, hooking into its IPC, and collecting data when it is available.

There are several IPC methods available (see the IPC section below for more details). The following example starts sampling audio when the user presses button 0 on the EVB, and uses the NS_AUDIO_API_CALLBACK IPC method:

```c
#include "ns_audio.h"

bool    static g_audioRecording = false;
bool    static g_audioReady     = false;
int16_t static g_in16AudioDataBuffer[SAMPLES_IN_FRAME * 2];

// This will be called by an IRQ handler
void audio_frame_callback(ns_audio_config_t *config, uint16_t bytesCollected) {
    uint32_t *buffer = (uint32_t *)am_hal_audadc_dma_get_buffer(config->audioSystemHandle);

    if (g_audioRecording) {
        for (int i = 0; i < config->numSamples; i++) {
            g_in16AudioDataBuffer[i] = (int16_t)(pui32_buffer[i] & 0x0000FFF0);
        }
        g_audioReady = true;
    }
}

// Desired Audio Configuration 
ns_audio_config_t audio_config = {
    .eAudioApiMode = NS_AUDIO_API_CALLBACK,
    .callback = audio_frame_callback, // declared above
    .audioBuffer = (void *)&g_in16AudioDataBuffer,
    .eAudioSource = NS_AUDIO_SOURCE_AUDADC,
    .numChannels = NUM_CHANNELS,
    .numSamples = SAMPLES_IN_FRAME,
    .sampleRate = SAMPLE_RATE,
    .audioSystemHandle = NULL,        // filled in by audio_init() below
    .bufferHandle = NULL
};

main(void) {
    ns_audio_init(&audio_config);

    while (1) {
        if (g_intButtonPressed == 1) { // See NS-peripherals for button info
            g_audioRecording = true;   // Tell callback to start collecting audio
        }

        if (g_audioReady) {
            // The callback sets g_audioReady when g_in16AudioDataBuffer has a sample
            // Do something with it...
            ns_mfcc_compute(g_in16AudioDataBuffer, &mfcc_buffer[mfcc_buffer_head]);
            g_audioReady = false;      // Tell callback we're ready for more
        }
    }
}
```

## Audio Inter-process Communication (IPC)
Capturing audio is an asynchronous task, partially handled by hardware peripherals. AI features which sample audio need a way to run in parallel with the capturing process - NeuralSPOT Audio leverages NeuralSPOT IPC to enable a few ways of doing this:
1. Simple: defines a simple mailbox system where the application only needs to access a couple of global variables to receive audio buffers. This is the simplest IPC, but least flexible. It doesn't ping-pong audio buffers, so the application must consume the buffer before the next sample interrupt occurs. (NOTE: not implemented yet!)
2. Callback-based: Invokes an IRQ-context callback defined by the application developer. The developer is responsible for buffer management.
3. Ringbuffer-based: adds a ping-ponging ringbuffer allowing the application to run while the next audio buffer is acquired. It builds on the callback approach, preserving flexibility.

### Choosing IPC Mode
Audio IPC behavior is controlled by the ns_audio_config_t configuration parameter passed into `ns_audio_init'. The above example illustrates the callback approach, while the following example shows the ringbuffer approach.

```c
#include "ns_audio.h"

bool static g_audioRecording = false;
bool static g_audioReady     = false;
int16_t static g_in16AudioDataBuffer_irq[SAMPLES_IN_FRAME * 2];

// Ringbuffer-specific globals
ns_ipc_ring_buffer_t audioBuf[1];
static uint8_t pui8AudioBuff[SAMPLES_IN_FRAME * 2 * 2]; // two buffers, 2 bytes/entry
int16_t static g_in16AudioDataBuffer_app[SAMPLES_IN_FRAME * 2];

// This will be called by an IRQ handler
void audio_frame_callback(ns_audio_config_t *config, uint16_t bytesCollected) {
    uint32_t *buffer = (uint32_t *)am_hal_audadc_dma_get_buffer(config->audioSystemHandle);

    if (g_audioRecording) {
        for (int i = 0; i < config->numSamples; i++) {
            g_in16AudioDataBuffer_irq[i] = (int16_t)(pui32_buffer[i] & 0x0000FFF0);
        }

        // This is the different part of the callback.
        ns_ipc_ring_buffer_push(&(config->bufferHandle[0]),
                                    g_in16AudioDataBuffer_irq,
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
            ns_mfcc_compute(g_in16AudioDataBuffer, &mfcc_buffer[mfcc_buffer_head]);
            g_audioReady = false;      // Tell callback we're ready for more
        }
    }
}
```