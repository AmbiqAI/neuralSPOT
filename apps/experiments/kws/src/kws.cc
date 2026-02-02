/**
 @file har.cc

 @brief Human Activity Recognition using neuralSPOT

 HAR is a model trained using the scripts found here:
https://github.com/AmbiqAI/Human-Activity-Recognition

**/

// #define ENERGY_MONITOR_ENABLE
// #define LOWEST_POWER_MODE

// ARM perf requires ITM to be enabled, impacting power measurements.
// For profiling measurements to work, example must be compiled using the MLPROFILE=1 make parameter
#ifdef NS_MLPROFILE
    #define MEASURE_ARM_PERF true
#else
    #define MEASURE_ARM_PERF false
#endif



/// neuralSPOT Includes
#include "ns_ambiqsuite_harness.h"
#include "ns_core.h"
#include "ns_energy_monitor.h"
#include "ns_perf_profile.h"
#include "ns_peripherals_power.h"
#include "arm_nnfunctions.h"   // CMSIS-NN functions
#include "arm_math.h"          // CMSIS-DSP/MVE intrinsics if needed
#include "ds_cnn_data.h"       // Declarations for weights, biases, and quant params

extern void ds_cnn_infer(const int8_t *input, int8_t *output);


/**
 * @brief Main HAR - infinite loop listening and inferring
 *
 * @return int
 */
int main(void) {
    float tmp = 0;
    float output[kCategoryCount];
    uint8_t output_max = 0;
    float max_val = 0.0;
    bool measure_first_inference = MEASURE_ARM_PERF;
    ns_perf_counters_t pp;
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    myState_e state = WAITING_TO_RECORD;

    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");

#ifdef ENERGY_MONITOR_ENABLE
    ns_init_power_monitor_state();
#endif
    ns_set_power_monitor_state(NS_IDLE); // no-op if ns_init_power_monitor_state not called
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed.\n");
    NS_TRY(ns_set_performance_mode(NS_MINIMUM_PERF), "Set CPU Perf mode failed.");

#ifndef LOWEST_POWER_MODE
    ns_itm_printf_enable();
#endif
ns_lp_printf("Starting model...\n");
#ifdef NS_MLPROFILE
    NS_TRY(ns_timer_init(&basic_tickTimer), "Timer init failed.\n");
#endif

    int8_t output[FC_OUT_FEATURES];
    ds_cnn_infer(test_input, output);
    ns_lp_printf("Inference done\n");
    // Print out output
    for (int i = 0; i < FC_OUT_FEATURES; i++) {
        ns_lp_printf("Output[%d] = %d\n", i, output[i]);
    }

}
