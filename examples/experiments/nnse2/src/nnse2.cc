// #include "tflite.h"
#include <stdint.h>
#include "ambiq_nnsp_debug.h"
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
#include "FreeRTOS.h"
#include "task.h"
#include <arm_math.h>
#if ARM_OPTIMIZED == 3
#include <arm_mve.h>
#endif
ns_perf_counters_t pp;
alignas(16) unsigned char static encodedDataBuffer[80]; // Opus encoder output length is hardcoded
                                                        // to 80 bytes
bool enableSE = false; // Flip between SE and Raw audio when button 0 is pressed
uint32_t seLatency = 0;
uint32_t opusLatency = 0;

// extern uint32_t g_ui32TrimVer;
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
// ns_perf_counters_t start, end, delta;
// int8_t do_it_once = 1;
ns_timer_config_t basic_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};

// Audio Task
uint32_t seStart, seEnd;
uint32_t opusStart, opusEnd;
uint32_t seLatencyCapturePeriod = 10;  // measure every 100 frames (1s)
uint32_t opusLatencyCapturePeriod = 5; // measure every 100 frames (1s)
uint32_t currentSESample = 0;
uint32_t currentOpusSample = 0;

#define NUM_GROUPS 100
int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\b");

    ns_power_config(&ns_power_ble);

    // Only turn HP while doing codec and AI

    ns_itm_printf_enable();

    ns_audio_init(&audio_config);
    ns_peripheral_button_init(&button_config_nnsp);
    

    // Initialize the Opus encoder
    audio_enc_init(0);
    ns_lp_printf("Opus Encoder Initialized\n");
    NS_TRY(ns_timer_init(&basic_tickTimer), "Timer init failed.\n");

    // Generate a 400hz sin wave (for debugging)
    for (int i = 0; i < 320; i++) {
        sinWave[i] = (int16_t)(sin(2 * 3.14159 * 400 * i / SAMPLING_RATE) * 32767);
    }

    NS_TRY(ns_set_performance_mode(NS_MAXIMUM_PERF), "Set CPU Perf mode failed.");
    seCntrlClass_init(&cntrl_inst);
    seCntrlClass_reset(&cntrl_inst);
#ifdef DEF_ACC32BIT_OPT
    ns_lp_printf("You are using \"32bit\" accumulator.\n");
#else
    ns_lp_printf("You are using \"64bit\" accumulator.\n");
#endif
    
    seCntrlClass_exec(&cntrl_inst, sinWave, xmitBuffer);
    // seCntrlClass_exec(&cntrl_inst, sinWave, xmitBuffer);
    // seCntrlClass_exec(&cntrl_inst, sinWave, xmitBuffer);
    ns_lp_printf("SE Run 3 times\n");

    ns_lp_printf("MCPS estimation\n");
    ns_init_perf_profiler();
    ns_reset_perf_counters();
    ns_start_perf_profiler();
    
    for (int i = 0; i < 100; i++) 
        seCntrlClass_exec(&cntrl_inst, sinWave, xmitBuffer);
    ns_stop_perf_profiler();
    ns_capture_perf_profiler(&pp);
    ns_print_perf_profile(&pp);

    // ns_lp_printf("cpu freq=%d\n", g_ui32TrimVer);
    while(1);    
}
