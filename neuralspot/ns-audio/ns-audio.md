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

bool static g_audioRecording = false;
bool static g_audioReady     = false;

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

