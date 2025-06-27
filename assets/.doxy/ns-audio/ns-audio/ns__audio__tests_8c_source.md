

# File ns\_audio\_tests.c

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-audio**](dir_45211a8475460839574f71aa108f4957.md) **>** [**tests**](dir_ebce5d9663c103d379a321152159c42b.md) **>** [**ns\_audio\_tests.c**](ns__audio__tests_8c.md)

[Go to the documentation of this file](ns__audio__tests_8c.md)


```C++
#include "unity/unity.h"

#include "ns_audio.h"
#include <stdint.h>
#define NUM_CHANNELS 1
#define NUM_FRAMES 100
#define SAMPLES_IN_FRAME 480
#define SAMPLE_RATE 16000
volatile bool static g_audioReady = false;
volatile bool static g_audioRecording = false;
int16_t static audioDataBuffer[SAMPLES_IN_FRAME]; // incoming PCM audio data
uint32_t static dmaBuffer[SAMPLES_IN_FRAME * NUM_CHANNELS * 2];     // DMA target
am_hal_audadc_sample_t static sLGSampleBuffer[SAMPLES_IN_FRAME * NUM_CHANNELS]; // working buffer
am_hal_offset_cal_coeffs_array_t sOffsetCalib;
ns_audio_config_t audioConfig;
static void audio_frame_callback(ns_audio_config_t *config, uint16_t bytesCollected) {
    if (g_audioRecording) {
        if (g_audioReady) {
            ns_lp_printf("Overflow!\n");
        }
        ns_audio_getPCM_v2(config, audioDataBuffer);
        g_audioReady = true;
    }
}
// Initialization function
static void initialize_audio_config() {
    audioConfig.api = &ns_audio_V2_0_0;
    audioConfig.eAudioApiMode = NS_AUDIO_API_CALLBACK;
    audioConfig.callback = audio_frame_callback;
    audioConfig.audioBuffer = (void *)&audioDataBuffer;
    audioConfig.eAudioSource = NS_AUDIO_SOURCE_PDM;
    audioConfig.sampleBuffer = dmaBuffer;
    audioConfig.workingBuffer = sLGSampleBuffer;
    audioConfig.numChannels = NUM_CHANNELS;
    audioConfig.numSamples = SAMPLES_IN_FRAME;
    audioConfig.sampleRate = SAMPLE_RATE;
    audioConfig.audioSystemHandle = NULL;
    audioConfig.bufferHandle = NULL;
#ifndef NS_AMBIQSUITE_VERSION_R4_1_0
    audioConfig.sOffsetCalib = &sOffsetCalib;
#endif
}

void ns_audio_tests_pre_test_hook() {

}

void ns_audio_tests_post_test_hook() {

}


void ns_switch_audio_test() {
#ifdef NS_AUDADC_PRESENT
    initialize_audio_config();
    int status = ns_audio_init(&audioConfig);
    status = ns_start_audio(&audioConfig);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);
    ns_end_audio(&audioConfig);
    audioConfig.eAudioSource = NS_AUDIO_SOURCE_AUDADC;
    status = ns_start_audio(&audioConfig);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);
#endif
}
// Basic init test
void ns_audio_init_test() {
    initialize_audio_config();
    int status = ns_audio_init(&audioConfig);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);
}

// Api test
void ns_audio_api_test() {
    initialize_audio_config();
    const ns_core_api_t invalid_api = {.apiId = 0xFFFFF, .version = NS_AUDIO_V0_0_1};
    audioConfig.api = &invalid_api;
    int status = ns_audio_init(&audioConfig);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_VERSION, status);
}
// NULL handle
void ns_audio_null_handle_test() {
    int status = ns_audio_init(NULL);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_HANDLE, status);
}

void ns_audio_null_config_test() {
    initialize_audio_config();
    // Null callback
    audioConfig.callback = NULL;
    int status = ns_audio_init(&audioConfig);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_CONFIG, status);
    
    initialize_audio_config();
    // Null audioBuffer
    audioConfig.audioBuffer = NULL;
    status = ns_audio_init(&audioConfig);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_CONFIG, status);

    initialize_audio_config();
    // Null sampleBuffer
    audioConfig.sampleBuffer = NULL;
    status = ns_audio_init(&audioConfig);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_CONFIG, status);
}

// Ensure both audio sources work
void ns_audio_audioSource_test() {
    initialize_audio_config();

    audioConfig.eAudioSource = NS_AUDIO_SOURCE_PDM;
    int status = ns_audio_init(&audioConfig);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);
}

void ns_audio_num_samples_test() {
    initialize_audio_config();
    // Should fail with 0 samples
    audioConfig.numSamples = 0;
    int status = ns_audio_init(&audioConfig);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_CONFIG, status);
    audioConfig.numSamples = SAMPLES_IN_FRAME;

    initialize_audio_config();
    // Should pass with large numsamples
    status = ns_audio_init(&audioConfig);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);
}

void ns_audio_num_channels_greater_than_2_test() {
    initialize_audio_config();
    // Should fail with 3 channels
    audioConfig.numChannels = 3;
    int status = ns_audio_init(&audioConfig);
    TEST_ASSERT_NOT_EQUAL(AM_HAL_STATUS_SUCCESS, status);

    initialize_audio_config();
    // Should pass with 1 channel
    audioConfig.numChannels = 1;
    status = ns_audio_init(&audioConfig);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);
}

void ns_audio_negative_sample_rate_test() {
    initialize_audio_config();
    audioConfig.sampleRate = -2;
    int status = ns_audio_init(&audioConfig);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_CONFIG, status);
}

void ns_audio_pdm_config_test() {
    initialize_audio_config();
    ns_pdm_cfg_t pdmConfig;
    audioConfig.pdm_config = &pdmConfig;
    pdmConfig.mic = NS_AUDIO_PDM_MICBOARD_0;
    pdmConfig.clock_freq = NS_AUDIO_PDM_CLK_750KHZ;
    for(int clock = 0; clock <= NS_CLKSEL_HFRC2_ADJ; clock++) {
        pdmConfig.clock = clock;
        ns_audio_init(&audioConfig);
        int status = ns_start_audio(&audioConfig);
        if(clock == NS_CLKSEL_XTHS || clock == NS_CLKSEL_HFRC2) {
            TEST_ASSERT_EQUAL(NS_STATUS_INIT_FAILED, status);
        }
        else {
            TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);
        }
    }
}
```


