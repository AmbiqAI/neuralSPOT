// #include "tflite.h"
#include <stdint.h>
#include "ns_core.h"
#include "am_util_stdio.h"
#include "ns_peripherals_button.h"
#include "ns_peripherals_power.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_audio.h"
#include "ae_api.h"

#include "ae_api.h"
#include "ns_ble.h"
#include "FreeRTOS.h"
#include "task.h"
#include "arm_math.h"

#define LEN_FFT_NNSP 512
#define LEN_STFT_WIN_COEFF 480
#define LEN_STFT_HOP 160
#define NUM_MELBANKS 40
#define NUM_FEATURE_CONTEXT 6
#define MAX_SIZE_FEATURE 80
#define DIMEMSION_FEATURE NUM_MELBANKS
#define SAMPLING_RATE 16000
#define NUM_CHANNELS 1

alignas(16) unsigned char static encodedDataBuffer[80]; // Opus encoder output length is hardcoded
                                                        // to 80 bytes
alignas(16) unsigned char static dummyBuffer[80];       // Opus encoder output length is hardcoded
                                                        // to 80 bytes
bool enableSE = false; // Flip between SE and Raw audio when button 0 is pressed
uint32_t seLatency = 0;
uint32_t opusLatency = 0;

#include "audio_webble.h"

#if (configAPPLICATION_ALLOCATED_HEAP == 1)
size_t ucHeapSize = NS_BLE_DEFAULT_MALLOC_K * 4 * 1024;
uint8_t ucHeap[NS_BLE_DEFAULT_MALLOC_K * 4 * 1024] __attribute__((aligned(4)));
#endif

// Custom power mode for BLE+Audio
const ns_power_config_t ns_power_ble = {
    .api = &ns_power_V1_0_0,
    .eAIPowerMode = NS_MAXIMUM_PERF,
    .bNeedAudAdc = false,
    .bNeedSharedSRAM = false,
    .bNeedCrypto = false,
    .bNeedBluetooth = true,
    .bNeedUSB = false,
    .bNeedIOM = false,
    .bNeedAlternativeUART = false,
    .b128kTCM = false,
    .bEnableTempCo = false,
    .bNeedITM = false,
    .bNeedXtal = true};

// FreeRTOS Tasks
TaskHandle_t radio_task_handle;  // Services WSF for BLE
TaskHandle_t encode_task_handle; // Encodes audio and sends it over BLE
TaskHandle_t my_xSetupTask;      // Starts the other tasks and suspends itself

bool volatile static g_audioReady = false;

// Audio Task
uint32_t seStart, seEnd;
uint32_t opusStart, opusEnd;
uint32_t seLatencyCapturePeriod = 10;  // measure every 100 frames (1s)
uint32_t opusLatencyCapturePeriod = 5; // measure every 100 frames (1s)
uint32_t currentSESample = 0;
uint32_t currentOpusSample = 0;

alignas(16) int16_t static sinWave[320];

// Encode and Xfer Task
int16_t static g_in16AudioDataBuffer[LEN_STFT_HOP << 1];
int16_t static pingAddress = 0;
int16_t static collectedBuffer[LEN_STFT_HOP * 2];

void encodeAndXferTask(void *pvParameters) {
    uint32_t i = 0;
    uint32_t framesSent = 0;
    while (1) {
        // memcpy(encodedDataBuffer+0, sinWave+i, sizeof(encodedDataBuffer));
        if (g_audioReady) {
            // audio_enc_encode_frame(collectedBuffer, 320, dummyBuffer);
            ns_ble_send_value(&webbleOpusAudio, NULL); // Send the encoded audio over BLE
            framesSent++;
            g_audioReady = false;
            if ((framesSent % 500) == 0) {
                ns_lp_printf("Sent %d frames\n", framesSent);
            }
        }
        i = (i + 1) % 20;

        // ns_lp_printf("%d", i);
        // }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void setup_task(void *pvParameters) {
    ns_lp_printf("Setting up BLE FreeRTOS Tasks\n");
    ns_ble_pre_init(); // Set NVIC priorities
    xTaskCreate(RadioTask, "RadioTask", 512, 0, 3, &radio_task_handle);
    xTaskCreate(encodeAndXferTask, "encodeAndXferTask", 4096, 0, 3, &encode_task_handle);
    vTaskSuspend(NULL);
    while (1)
        ;
}

/// Audio Config
bool volatile static g_audioRecording = false;
uint32_t static audadcSampleBuffer[(LEN_STFT_HOP << 1) + 3];
#ifdef USE_AUDADC
am_hal_audadc_sample_t static workingBuffer[SAMPLES_IN_FRAME * NUM_CHANNELS]; // working buffer
                                                                              // used by AUDADC
#endif
#if !defined(NS_AMBIQSUITE_VERSION_R4_1_0) && defined(NS_AUDADC_PRESENT)
am_hal_offset_cal_coeffs_array_t sOffsetCalib;
#endif

void audio_frame_callback(ns_audio_config_t *config, uint16_t bytesCollected) {
    if (pingAddress == 0) {
        memcpy(collectedBuffer, g_in16AudioDataBuffer, LEN_STFT_HOP * sizeof(int16_t));
        pingAddress = LEN_STFT_HOP;
    } else {
        memcpy(
            &collectedBuffer[LEN_STFT_HOP], g_in16AudioDataBuffer, LEN_STFT_HOP * sizeof(int16_t));
        pingAddress = 0;
        g_audioReady = true;
    }
}

ns_audio_config_t audio_config = {
    .api = &ns_audio_V2_1_0,
    .eAudioApiMode = NS_AUDIO_API_CALLBACK,
    .callback = audio_frame_callback,
    .audioBuffer = (void *)&g_in16AudioDataBuffer,
#ifdef USE_AUDADC
    .eAudioSource = NS_AUDIO_SOURCE_AUDADC,
#else
    .eAudioSource = NS_AUDIO_SOURCE_PDM,
#endif
    .sampleBuffer = audadcSampleBuffer,
#if USE_AUDADC
    .workingBuffer = workingBuffer,
#else
    .workingBuffer = NULL,
#endif
    .numChannels = NUM_CHANNELS,
    .numSamples = LEN_STFT_HOP,
    .sampleRate = SAMPLING_RATE,
    .audioSystemHandle = NULL, // filled in by init
    .bufferHandle = NULL,      // only for ringbuffer mode
#if !defined(NS_AMBIQSUITE_VERSION_R4_1_0) && defined(NS_AUDADC_PRESENT)
    .sOffsetCalib = &sOffsetCalib,
#endif
};

ns_timer_config_t basic_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};

int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\b");

    ns_power_config(&ns_power_ble);

    NS_TRY(ns_timer_init(&basic_tickTimer), "Timer init failed.\n");

    // Only turn HP while doing codec and AI
    NS_TRY(ns_set_performance_mode(NS_MAXIMUM_PERF), "Set CPU Perf mode failed. ");
    ns_audio_init(&audio_config);
    ns_start_audio(&audio_config);
    audio_enc_init(0);

    // non-zero test pattern
    for (int i = 0; i < 320; i++) {
        sinWave[i] = (int16_t)(sin(2 * 3.14159 * 400 * i / 16000) * 32767);
    }

    for (int i = 0; i < 80; i++) {
        encodedDataBuffer[i] = 0;
    }

    ns_itm_printf_enable();

    xTaskCreate(setup_task, "Setup", 512, 0, 3, &my_xSetupTask);
    vTaskStartScheduler();
    while (1) {
    };
}
