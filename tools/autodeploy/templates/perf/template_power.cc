/**
 * @file NS_AD_NAME_example.cc
 * @author Carlos Morales
 * @brief Minimal application instantiating a TFLM model, feeding it
 * a test input tensor, and checking the result
 * @version 0.1
 * @date 2023-02-28
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifdef AM_PART_APOLLO5B
#define NS_PROFILER_PMU_EVENT_0 NS_AD_PMU_EVENT_0
#define NS_PROFILER_PMU_EVENT_1 NS_AD_PMU_EVENT_1
#define NS_PROFILER_PMU_EVENT_2 NS_AD_PMU_EVENT_2
#define NS_PROFILER_PMU_EVENT_3 NS_AD_PMU_EVENT_3
#endif

#include "ns_core.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_energy_monitor.h"
#include "ns_peripherals_power.h"
#include "ns_peripherals_button.h"
#include "ns_pmu_utils.h"
#include "ns_pmu_map.h"
#include "ns_power_profile.h"
#include "NS_AD_NAME_api.h"
#include "NS_AD_NAME_example_tensors.h"
#include "ns_model.h"

#if (NS_AD_MODEL_LOCATION == NS_AD_PSRAM) or (NS_AD_ARENA_LOCATION == NS_AD_PSRAM)
#include "ns_peripherals_psram.h"
#endif

// TFLM Config
static ns_model_state_t model;
volatile int example_status = 0; // Prevent the compiler from optimizing out while loops

typedef enum { WAITING_TO_RUN, SIGNAL_START_TO_JS, RUNNING, SIGNAL_END_TO_JS } myState_e;

// Button global - will be set by neuralSPOT button helper
static int volatile joulescopeTrigger = 0;

#ifdef NS_MLPROFILE
#ifdef AM_PART_APOLLO5B
extern ns_pmu_config_t ns_microProfilerPMU;
extern ns_profiler_sidecar_t ns_microProfilerSidecar;
#endif
#endif

// Button Peripheral Config Struct
ns_button_config_t joulescopeTrigger_config = {
    .api = &ns_button_V1_0_0,
    .button_0_enable = false,
    .button_1_enable = false,
    .joulescope_trigger_enable = true,
    .button_0_flag = NULL,
    .button_1_flag = NULL,
    .joulescope_trigger_flag = &joulescopeTrigger};

const ns_power_config_t ns_power_measurement = {
    .api = &ns_power_V1_0_0,
    .eAIPowerMode = NS_AD_CPU_MODE,
    .bNeedAudAdc = false,
    .bNeedSharedSRAM = true,
    .bNeedCrypto = false,
    .bNeedBluetooth = false,
    .bNeedUSB = false,
    .bNeedIOM = false,
    .bNeedAlternativeUART = false,
    .b128kTCM = false,
    .bEnableTempCo = false,
    .bNeedITM = false};

// PSRAM Stuff
#if (NS_AD_MODEL_LOCATION == NS_AD_PSRAM) or (NS_AD_ARENA_LOCATION == NS_AD_PSRAM)
ns_psram_config_t psram_cfg = {
    .api = &ns_psram_V0_0_1,
    .psram_enable = true,
    .psram_type = PSRAM_TYPE_HEX,
    // .psram_type = PSRAM_TYPE_OCT,
    .psram_block = 3,
    .psram_size = 0, // filled in by init
    .psram_base_address = 0, // filled in by init
};
#endif

int tf_invoke() {
    model.interpreter->Invoke();
    return 0;
}

int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    uint32_t num_layers = 0;
    char name[50];
    uint32_t pmu_profile_start_layer = 0;
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");

    myState_e state = WAITING_TO_RUN;

    // Initialize the model, get handle if successful
    ns_init_power_monitor_state();
    ns_set_power_monitor_state(NS_IDLE);

    NS_TRY(ns_power_config(&ns_power_measurement), "Power Init Failed.\n");
        // NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed\n");

    ns_itm_printf_enable();
    // am_bsp_itm_printf_enable();

    ns_interrupt_master_enable();

    NS_TRY(ns_set_performance_mode(NS_AD_CPU_MODE), "Set CPU Perf mode failed.");
    NS_TRY(ns_peripheral_button_init(&joulescopeTrigger_config), "Button initialization failed.\n");
    
    #if (NS_AD_MODEL_LOCATION == NS_AD_PSRAM) or (NS_AD_ARENA_LOCATION == NS_AD_PSRAM)
        NS_TRY(ns_psram_init(&psram_cfg), "PSRAM Init Failed\n");
    #endif

    int status = NS_AD_NAME_minimal_init(&model); // model init with minimal defaults
    if (status == NS_AD_NAME_STATUS_FAILURE) {
        while (1)
            ns_lp_printf("Model init failed.\n");
        example_status = NS_AD_NAME_STATUS_INIT_FAILED; // hang
    }
    ns_lp_printf("Model init successful.\n");

    // At this point, the model is ready to use - init and allocations were successful
    // Note that the model handle is not meant to be opaque, the structure is defined
    // in ns_model.h, and contains state, config details, and model structure information

    // Get data about input and output tensors
    int numInputs = model.numInputTensors;

    // Initialize input tensors
    int offset = 0;
    for (int i = 0; i < numInputs; i++) {
        memcpy(
            model.model_input[i]->data.int8, ((char *)NS_AD_NAME_example_input_tensors) + offset,
            model.model_input[i]->bytes);
        offset += model.model_input[i]->bytes;
    }
    ns_lp_printf("Input tensors initialized.\n");

#if NS_AD_JS_PRESENT == 0
    // If no Joulescope, start running the model without waiting for a trigger
    // ns_lp_printf("Current power and performance register settings:\n");
    // ns_pp_ap5_snapshot(false, 0, false);
    // ns_pp_ap5_snapshot(false, 0, true);

    joulescopeTrigger = true;
    ns_lp_printf("No Joulescope, starting model without trigger.\n");
#endif

    // Event loop
    while (1) {
        switch (state) {

        case WAITING_TO_RUN:
            if (joulescopeTrigger) {
                ns_delay_us(100000);
                state = SIGNAL_START_TO_JS;
                joulescopeTrigger = false;
            }
            break;

        case SIGNAL_START_TO_JS:
            ns_lp_printf("Starting...\n");
            ns_set_power_monitor_state(3);
            ns_delay_us(100000);
            state = RUNNING;
            break;

        case RUNNING:
            ns_lp_printf("Running...\n");
            ns_set_power_monitor_state(0);
#if NS_AD_JS_PRESENT != 0
            for (int i = 0; i < NS_AD_POWER_RUNS; i++) {
#else
            for (int i = 0; i < 1; i++) { // Only 1 run needed for onboard_perf mode
#endif // NS_AD_JS_PRESENT
            // for (int i = 0; i < NS_AD_POWER_RUNS; i++) {
                model.interpreter->Invoke();
#ifdef NS_MLPROFILE
                if (i == 0) {
                    ns_stop_perf_profiler();
                    model.profiler->LogCsv(); // prints and also captures events in a buffer
                    // Remember the number of events, treat that as the number of layers
                    num_layers = ns_microProfilerSidecar.captured_event_num;
                    // ns_lp_printf("Number of layers: %d\n", num_layers);
                }
#endif // NS_MLPROFILE
            // ns_lp_printf("Model run %d complete.\n", i);
            }
            // ns_lp_printf("Model run complete.\n");
            // ns_delay_us(1100000);
            state = SIGNAL_END_TO_JS;
            break;

        case SIGNAL_END_TO_JS:
            ns_lp_printf("Ending...\n");
            ns_set_power_monitor_state(2);
            ns_delay_us(100000);
#if NS_AD_JS_PRESENT == 0
            // Characterize and break out of event loop
#ifdef AM_PART_APOLLO5B

            // Run the model repeatedly, capturing different PMU every time. The results
            // will accumulate in the events array (ns_profiler_event_stats_t). Print those 
            // prettily to the console, up until the limit of the event buffer (4096
            // events). 
            //
            // IMPORTANT: this assumes that every run is identical, which is true for most models,
            // but not all. Specifically, some models include a CALL_ONCE layer that will only run
            // the first time the model is invoked. That 'first run' is above, and is already
            // captured in the event buffer. If the model has a CALL_ONCE layer, the number of layers
            // will be different for the runs below, so the mapping of PMU events to layers must be 
            // adjusted.

            ns_characterize_model(tf_invoke);
            ns_parse_pmu_stats(num_layers, model.rv_count);

#endif // AM_PART_APOLLO5B
            while (1);   // hang
#endif // NS_AD_JS_PRESENT == 0
            state = WAITING_TO_RUN;
            break;
        }
    }

    while (1) {
        // Success!
        example_status = NS_AD_NAME_STATUS_SUCCESS;
    }
}
