// #include "tflite.h"
#include <stdint.h>
#include "seCntrlClass.h"
#include "am_util_stdio.h"
#include "ns_peripherals_button.h"
#include "ns_peripherals_power.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_audio.h"
#include "ambiq_nnsp_const.h"
#include "ns_timer.h"
#include "ns_energy_monitor.h"
#include "nn_speech.h"
#include "ns_perf_profile.h"

#include "ae_api.h"
#include "ns_ble.h"
#include "FreeRTOS.h"
#include "task.h"
#include "arm_math.h"

alignas(16) unsigned char static encodedDataBuffer[80]; // Opus encoder output length is hardcoded
                                                        // to 80 bytes
#include "audio_webble.h"

#ifdef DEF_GUI_ENABLE
    #include "ns_rpc_generic_data.h"
#endif

#define NUM_CHANNELS 1

// #define USE_AUDADC // Uncomment this to use the AUDADC instead of the PDM

/// Button Peripheral Config
int volatile g_intButtonPressed = 0;
ns_button_config_t button_config_nnsp = {
    .api = &ns_button_V1_0_0,
    .button_0_enable = true,
    .button_1_enable = false,
    .button_0_flag = &g_intButtonPressed,
    .button_1_flag = NULL};

/// Audio Config
bool volatile static g_audioRecording = false;
bool volatile static g_audioReady = false;
int16_t static g_in16AudioDataBuffer[LEN_STFT_HOP << 1];
uint32_t static audadcSampleBuffer[(LEN_STFT_HOP << 1) + 3];
#ifdef USE_AUDADC
am_hal_audadc_sample_t static workingBuffer[SAMPLES_IN_FRAME * NUM_CHANNELS]; // working buffer
                                                                              // used by AUDADC
#endif
#if !defined(NS_AMBIQSUITE_VERSION_R4_1_0) && defined(NS_AUDADC_PRESENT)
am_hal_offset_cal_coeffs_array_t sOffsetCalib;
#endif

// Use this to generate a sinwave for debugging instead
// of using the microphone
alignas(16) int16_t static sinWave[320];

void audio_frame_callback(ns_audio_config_t *config, uint16_t bytesCollected) {
    if (g_audioRecording) {
        ns_audio_getPCM_v2(config, g_in16AudioDataBuffer);
        g_audioReady = true;
    }
}

ns_audio_config_t audio_config = {
    .api = &ns_audio_V2_0_0,
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
    .bNeedITM = false};

// Speech Enhancement Control Structure
seCntrlClass cntrl_inst;

// Transmit Buffer
// Opus frames are 20ms, so we need 320 samples at 16kHz
// AudioTask writes clean audio to xmitBuffer
// EndodeAndXferTask watches xmitBuffer, and encodes it when there are 320 samples available (and
// sends that)
#define XBUFSIZE 640
alignas(16) int16_t static xmitBuffer[XBUFSIZE];
uint16_t static xmitWritePtr = 0;
uint16_t static xmitReadPtr = 0;
uint16_t static xmitAvailable = 0;

// FreeRTOS Tasks
TaskHandle_t audio_task_handle;  // Collects audio and de-noises it
TaskHandle_t radio_task_handle;  // Services WSF for BLE
TaskHandle_t encode_task_handle; // Encodes audio and sends it over BLE
TaskHandle_t my_xSetupTask;      // Starts the other tasks and suspends itself

// Timer and structs for performance profiling
ns_perf_counters_t start, end, delta;
int8_t do_it_once = 1;
ns_timer_config_t basic_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};

// Audio Task
bool enableSE = false; // Flip between SE and Raw audio when button 0 is pressed
void audioTask(void *pvParameters) {
    while (1) {
        if (g_intButtonPressed) {
            // Flip modes between SE and Raw audio
            enableSE = !enableSE;
            g_intButtonPressed = 0;
        }
        if (g_audioReady) { // Every time audio frame is available (160 samples, 10ms)

            NS_TRY(ns_set_performance_mode(NS_MAXIMUM_PERF), "Set CPU Perf mode failed. ");

            // SE Model is stateful, so you have to call it with every frame
            // even if you don't want SE's output (otherwise it'll sound wierd for a bit at
            // the beginning)
            seCntrlClass_exec(&cntrl_inst, g_in16AudioDataBuffer, xmitBuffer + xmitWritePtr);

            if (!enableSE) {
                // Overwrite the SE output with the original audio
                memcpy(
                    xmitBuffer + xmitWritePtr, g_in16AudioDataBuffer,
                    LEN_STFT_HOP * sizeof(int16_t));
            }

            NS_TRY(ns_set_performance_mode(NS_MINIMUM_PERF), "Set CPU Perf mode failed. ");

            // Add it to our simple ring buffer
            xmitAvailable += 160;
            xmitWritePtr = (xmitWritePtr + 160) % XBUFSIZE;
            g_audioReady = false;
        }
        vTaskDelay(pdMS_TO_TICKS(1)); // this could be probably be slower, but we want to keep the
                                      // audio latency low
    }
}

// Encode and Xfer Task
void encodeAndXferTask(void *pvParameters) {
    uint32_t ret;

    while (1) {
        if (xmitAvailable >= 320) {
            NS_TRY(ns_set_performance_mode(NS_MAXIMUM_PERF), "Set CPU Perf mode failed. ");
            ret = audio_enc_encode_frame(xmitBuffer + xmitReadPtr, 320, encodedDataBuffer);
            NS_TRY(ns_set_performance_mode(NS_MINIMUM_PERF), "Set CPU Perf mode failed. ");

            if (ret >= 0) {
                ns_ble_send_value(&webbleOpusAudio, NULL); // Send the encoded audio over BLE
            } else {
                ns_lp_printf("Error encoding %d\n", ret);
            }
            xmitAvailable -= 320;
            xmitReadPtr = (xmitReadPtr + 320) % XBUFSIZE;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void setup_task(void *pvParameters) {
    ns_lp_printf("Setting up BLE FreeRTOS Tasks\n");
    ns_ble_pre_init(); // Set NVIC priorities
    xTaskCreate(RadioTask, "RadioTask", 512, 0, 3, &radio_task_handle);
    xTaskCreate(audioTask, "AudioTask", 2048, 0, 3, &audio_task_handle);
    xTaskCreate(encodeAndXferTask, "encodeAndXferTask", 4096, 0, 3, &encode_task_handle);
    vTaskSuspend(NULL);
    while (1)
        ;
}

int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\b");

    ns_power_config(&ns_power_ble);

    // Only turn HP while doing codec and AI
    NS_TRY(ns_set_performance_mode(NS_MINIMUM_PERF), "Set CPU Perf mode failed. ");

    ns_itm_printf_enable();

    ns_audio_init(&audio_config);
    ns_peripheral_button_init(&button_config_nnsp);
    ns_init_perf_profiler();
    ns_start_perf_profiler();

    // Initialize the Opus encoder
    audio_enc_init(0);

    NS_TRY(ns_timer_init(&basic_tickTimer), "Timer init failed.\n");

    // Generate a 400hz sin wave (for debugging)
    for (int i = 0; i < 320; i++) {
        sinWave[i] = (int16_t)(sin(2 * 3.14159 * 400 * i / SAMPLING_RATE) * 32767);
    }

    // initialize neural nets controller
    seCntrlClass_init(&cntrl_inst);
#ifdef DEF_ACC32BIT_OPT
    ns_lp_printf("You are using \"32bit\" accumulator.\n");
#else
    ns_lp_printf("You are using \"64bit\" accumulator.\n");
#endif

    // BLE is FreeRTOS-driven, everything happens in the tasks set up by setup_task()
    // Audio notifications will start immediately, no waiting for button presses
    g_audioRecording = true;
    xTaskCreate(setup_task, "Setup", 512, 0, 3, &my_xSetupTask);
    vTaskStartScheduler();
    while (1) {
    };
}
