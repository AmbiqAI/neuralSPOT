// #include "tflite.h"
#include <stdint.h>
#include "seCntrlClass.h"
#include "am_util_stdio.h"
#include "ns_peripherals_button.h"
#include "ns_peripherals_power.h"
#include "ns_ambiqsuite_harness.h"
#include "ambiq_nnsp_const.h"
#include "ns_timer.h"
#include "ns_energy_monitor.h"
#include "nn_speech.h"
#include "ns_perf_profile.h"
#include "ns_energy_monitor.h"


#include "arm_math.h"

uint32_t seLatency = 0;
uint32_t opusLatency = 0;

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


// Use this to generate a sinwave for debugging instead
// of using the microphone
alignas(16) int16_t static sinWave[1280];


// Custom power mode for BLE+Audio
const ns_power_config_t ns_power_measure = {
    .api = &ns_power_V1_0_0,
    .eAIPowerMode = NS_MAXIMUM_PERF,
    .bNeedAudAdc = false,
    .bNeedSharedSRAM = false,
    .bNeedCrypto = false,
    .bNeedBluetooth = false,
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


int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\b");

    ns_power_config(&ns_power_measure);

    // Only turn HP while doing codec and AI
    NS_TRY(ns_set_performance_mode(NS_MINIMUM_PERF), "Set CPU Perf mode failed. ");

    // ns_itm_printf_enable();
    ns_init_power_monitor_state();
    ns_set_power_monitor_state(NS_IDLE);


    // NS_TRY(ns_timer_init(&basic_tickTimer), "Timer init failed.\n");

    // initialize neural nets controller
    seCntrlClass_init(&cntrl_inst);

#ifdef DEF_ACC32BIT_OPT
    ns_lp_printf("You are using \"32bit\" accumulator.\n");
#else
    ns_lp_printf("You are using \"64bit\" accumulator.\n");
#endif

    // Generate a 400hz sin wave (for debugging)
    for (int i = 0; i < 1280; i++) {
        sinWave[i] = (int16_t)(sin(2 * 3.14159 * 400 * i / SAMPLING_RATE) * 32767);
    }

    // Invoke NNSE 1000 times, measure power
    ns_lp_printf("Starting NNSE\n");

    // Warmup
    for (int i = 0; i < 5; i++) {
        seCntrlClass_exec(&cntrl_inst, sinWave, xmitBuffer);
    }
    ns_lp_printf("Starting power measurements\n");

    // Run 1000 times
    ns_set_power_monitor_state(NS_INFERING);
    for (int i = 0; i < 1000; i++) {
        seCntrlClass_exec(&cntrl_inst, sinWave, xmitBuffer);
    }
    ns_set_power_monitor_state(NS_IDLE);
    ns_lp_printf("Ending power measurements\n");

    while (1) {
        ns_deep_sleep();
    };
}
