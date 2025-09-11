/**
 * @file perf_profiling.cc
 * @author Ambiq
 * @brief Demonstrates the use of PMU for performance profiling
 * @version 0.1
 * @date 2025-06-25
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef AM_PART_APOLLO5B
#error "This example is only supported on Apollo5B"
#endif

////////////////////////////////////////////////
// NeuralSPOT Headers
#include "ns_core.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_audio.h"
#include "ns_energy_monitor.h"
#include "ns_peripherals_power.h"
#include "ns_audio_mfcc.h"
#include "ns_debug_log.h"
#include "ns_peripherals_button.h"
#include "ns_timer.h"
#include "ns_pmu_utils.h"
#define __ARM_FEATURE_MVE 1
#define __ARM_FEATURE_MVE_FLOAT 1
#include "arm_mve.h"

#define NS_PROFILER_PMU_EVENT_0 ARM_PMU_MVE_LDST_MULTI_RETIRED
#define NS_PROFILER_PMU_EVENT_1 ARM_PMU_MVE_INT_MAC_RETIRED
#define NS_PROFILER_PMU_EVENT_2 ARM_PMU_INST_RETIRED
#define NS_PROFILER_PMU_EVENT_3 ARM_PMU_MVE_PRED

////////////////////////////////////////////////
// Audio and MFCC Parameters
#define NUM_CHANNELS 1
#define NUM_FRAMES 49 // 20ms frame shift
#define SAMPLES_IN_FRAME 320
#define SAMPLE_RATE 16000

////////////////////////////////////////////////
// MFCC (this is the algorithm that will be profiled)
#define MY_MFCC_FRAME_LEN_POW2 512 // Next power of two size after SAMPLES_IN_FRAME
#define MY_MFCC_NUM_FBANK_BINS 40
#define MY_MFCC_NUM_MFCC_COEFFS 10

// Allocate memory for MFCC calculations
#define MFCC_ARENA_SIZE                                                                            \
    32 * (MY_MFCC_FRAME_LEN_POW2 * 2 +                                                             \
          MY_MFCC_NUM_FBANK_BINS * (NS_MFCC_SIZEBINS + MY_MFCC_NUM_MFCC_COEFFS))
static uint8_t mfccArena[MFCC_ARENA_SIZE];
static ns_mfcc_cfg_t mfcc_config = {.api = &ns_mfcc_V1_0_0,
                             .arena = mfccArena,
                             .sample_frequency = SAMPLE_RATE,
                             .num_fbank_bins = MY_MFCC_NUM_FBANK_BINS,
                             .low_freq = 20,
                             .high_freq = 4000,
                             .num_frames = NUM_FRAMES,
                             .num_coeffs = MY_MFCC_NUM_MFCC_COEFFS,
                             .num_dec_bits = 0,
                             .frame_shift_ms = 20,
                             .frame_len_ms = 30,
                             .frame_len = SAMPLES_IN_FRAME,
                             .frame_len_pow2 = MY_MFCC_FRAME_LEN_POW2};


////////////////////////////////////////////////
// Timer is used for TF profiling
static ns_timer_config_t tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};
static uint32_t elapsedTime = 0;

void tic() {
    uint32_t oldTime = elapsedTime;
    elapsedTime = ns_us_ticker_read(&tickTimer);
    if (elapsedTime == oldTime) {
        // We've saturated the timer, reset it
        ns_timer_clear(&tickTimer);
    }
}

uint32_t toc() { return ns_us_ticker_read(&tickTimer) - elapsedTime; }


#define MAC_INSTRUCTIONS 10000000
int64_t run_mve_mac_benchmark(void) {
    int16x8_t a = vdupq_n_s16(2);
    int16x8_t b = vdupq_n_s16(3);
    int64_t acc = 0;

    for (uint32_t i = 0; i < MAC_INSTRUCTIONS; i++) {
        acc += vmlaldavq_s32(a, b); // Each = 8 MACs (int16 * int16 → int32), reduced to scalar
    }
    // ns_lp_printf("MACs: %d\n", MAC_INSTRUCTIONS);
    volatile int64_t sink = acc;
    return sink;
}




////////////////////////////////////////////////
// PMU config for profiling
ns_pmu_config_t pmu_config;

alignas(16) int16_t static sinWave[320];
float mfcc_buffer[NUM_FRAMES * MY_MFCC_NUM_MFCC_COEFFS];

////////////////////////////////////////////////
// function to be prototyped 
// (MFCC over 49 frames of synthetic audio)
int func_to_be_profiled() {
    // for (int i = 0; i < NUM_FRAMES; i++) {
    //     int32_t mfcc_buffer_head = i * MY_MFCC_NUM_MFCC_COEFFS;
    //     ns_mfcc_compute(&mfcc_config, sinWave, &mfcc_buffer[mfcc_buffer_head]);
    // }
    return (int)(run_mve_mac_benchmark());
    // return 0; // *MUST* return a value, otherwise the compiler may optimize it out
}

/**
 * @brief Main Various profiling passes
 *
 * @return int
 */
int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};

    // Tells callback if it should be recording audio

    // Pile of inits
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed.\n");
    ns_itm_printf_enable();
    ns_interrupt_master_enable();
    NS_TRY(ns_timer_init(&tickTimer), "Timer init failed.\n");
    NS_TRY(ns_mfcc_init(&mfcc_config), "MFCC config failed.\n");

        // PMU config for profiling
    pmu_config.api = &ns_pmu_V1_0_0;
    ns_pmu_reset_config(&pmu_config);

    // Add events
    ns_pmu_event_create(&pmu_config.events[0], NS_PROFILER_PMU_EVENT_0, NS_PMU_EVENT_COUNTER_SIZE_32);
    ns_pmu_event_create(&pmu_config.events[1], NS_PROFILER_PMU_EVENT_1, NS_PMU_EVENT_COUNTER_SIZE_32);
    ns_pmu_event_create(&pmu_config.events[2], NS_PROFILER_PMU_EVENT_2, NS_PMU_EVENT_COUNTER_SIZE_32);
    ns_pmu_event_create(&pmu_config.events[3], NS_PROFILER_PMU_EVENT_3, NS_PMU_EVENT_COUNTER_SIZE_32);   
    ns_pmu_init(&pmu_config); // PMU config passed to model init, which passes it to debugLogInit

    // Generate a 400hz sin wave (dummy data for MFCC)
    for (int i = 0; i < 320; i++) {
        sinWave[i] = (int16_t)(sin(2 * 3.14159 * 400 * i / SAMPLE_RATE) * 32767);
    }

    // User the tick timer to measure a single pass
    tic();
    func_to_be_profiled(); // Call once
    ns_lp_printf("Single pass profiling took %d us.\n", toc());

    // Init PMU and call function to collect events defined above during run
    ns_lp_printf("\nStarting PMU capture - 4 counters, single pass first...\n");

    // Profile it
    ns_pmu_init(&pmu_config); // PMU config passed to model init, which passes it to debugLogInit
    func_to_be_profiled(); // Call once
    ns_pmu_get_counters(&pmu_config);

    // Print the counters
    ns_lp_printf("Single pass profiling done.\n");
    ns_lp_printf("Counter Value, Counter Name, Counter Description\n"); // header
    ns_pmu_print_counters(&pmu_config);

    // Characterize the function by repeatedly calling it and checking different PMU counters every time
    ns_lp_printf("\nStarting full PMU capture - multiple passes...\n");
    // This function prints results as it goes
    ns_pmu_characterize_function(func_to_be_profiled, &pmu_config);

    while(1);                

};