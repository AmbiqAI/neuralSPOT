#include "ns_core.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_audio.h"
#include "ns_peripherals_power.h"
#include "ae_api.h"
#include "ns_timer.h"
#include "arm_math.h"

#define NUM_CHANNELS 1
#define SAMPLES_IN_FRAME 320
#define SAMPLING_RATE 16000

alignas(16) int16_t static audioBuf[SAMPLES_IN_FRAME * 2];
alignas(16) uint32_t static sampleBuffer[SAMPLES_IN_FRAME * 2];
uint32_t audioInterruptCount = 0;

void audio_frame_callback(ns_audio_config_t *config, uint16_t bytesCollected) {
    ns_audio_getPCM_v2(config, audioBuf);
    audioInterruptCount++;
    if (audioInterruptCount % 100 == 0) {
        ns_lp_printf(
            "audioInterruptCount: %d (%d seconds)\n", audioInterruptCount,
            audioInterruptCount / 50);
    }
}

ns_audio_config_t audio_config = {
    .api = &ns_audio_V2_0_0,
    .eAudioApiMode = NS_AUDIO_API_CALLBACK,
    .callback = audio_frame_callback,
    .audioBuffer = (void *)&audioBuf,
    .eAudioSource = NS_AUDIO_SOURCE_PDM,
    .sampleBuffer = sampleBuffer,
    .workingBuffer = NULL,
    .numChannels = NUM_CHANNELS,
    .numSamples = SAMPLES_IN_FRAME,
    .sampleRate = SAMPLING_RATE,
    .audioSystemHandle = NULL, // filled in by init
    .bufferHandle = NULL,      // only for ringbuffer mode
};

alignas(16) unsigned char static encodedDataBuffer[80]; // Opus encoder output length is hardcoded
alignas(16) int16_t static sinWave[320];

ns_timer_config_t basic_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};

int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    ns_core_init(&ns_core_cfg);
    uint32_t opusBegin, opusEnd;

    ns_itm_printf_enable();

    ns_power_config(&ns_development_default);
    // am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    // am_hal_cachectrl_enable();
    // NS_TRY(ns_set_performance_mode(NS_MINIMUM_PERF), "Set CPU Perf mode failed. ");

    ns_audio_init(&audio_config);
    NS_TRY(ns_timer_init(&basic_tickTimer), "Timer init failed.\n");
    audio_enc_init(0);

    // Generate a 400hz sin wave (for debugging)
    for (int i = 0; i < 320; i++) {
        sinWave[i] = (int16_t)(sin(2 * 3.14159 * 400 * i / 16000) * 32767);
    }

    opusBegin = ns_us_ticker_read(&basic_tickTimer);
    ns_lp_printf("opusbegintime: %d\n", opusBegin);
    // run the audio encoder 100 times and measure the time
    for (int i = 0; i < 100; i++) {
        audio_enc_encode_frame(sinWave, 320, encodedDataBuffer);
    }
    opusEnd = ns_us_ticker_read(&basic_tickTimer);
    ns_lp_printf("opusendtime: %d\n", opusEnd);
    ns_lp_printf("opustime: %d\n", opusEnd - opusBegin);

    ns_lp_printf("Hello World!\n");

    // Wait 10 seconds
    ns_delay_us(10000000);
    ns_lp_printf("Goodbye World!\n");
    ns_lp_printf("audioInterruptCount: %d\n", audioInterruptCount);

    while (1)
        ;
}
