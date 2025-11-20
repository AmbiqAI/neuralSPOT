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

#if defined(AM_PART_APOLLO5B) || defined(AM_PART_APOLLO330P_510L)
#define NS_PROFILER_PMU_EVENT_0 NS_AD_PMU_EVENT_0
#define NS_PROFILER_PMU_EVENT_1 NS_AD_PMU_EVENT_1
#define NS_PROFILER_PMU_EVENT_2 NS_AD_PMU_EVENT_2
#define NS_PROFILER_PMU_EVENT_3 NS_AD_PMU_EVENT_3
#endif
#if defined(__ARM_FEATURE_MVE) || defined(__ARM_FEATURE_MVE_FP)
extern "C++" {
#include <arm_mve.h>
}
#endif
#include <string.h>
#include "ns_core.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_energy_monitor.h"
#include "ns_peripherals_power.h"
#include "ns_peripherals_button.h"
#include "ns_pmu_utils.h"
#include "ns_pmu_map.h"
#include "ns_pmu_accumulator.h"
#include "ns_timer.h"
// #if defined(AM_PART_APOLLO5B) || defined(AM_PART_APOLLO330P_510L)
// #include "ns_pp.h"
// #else
#include "ns_power_profile.h"
#include "am_hal_spotmgr.h"
// #endif
#if NS_AD_AOT == 1
#include "NS_AD_NAME_AOT_model.h"
#include "NS_AD_NAME_AOT_platform.h"         // platform macros
#include "NS_AD_NAME_AOT_tensors.h"          // tensor sizes
#include "NS_AD_NAME_AOT_context.h"          // model context
#include "NS_AD_NAME_aot_api.h"
#include "NS_AD_NAME_aot_example_tensors.h"
// Memory locations for model and arena
#define NS_AD_TCM   0
#define NS_AD_PSRAM 1
#define NS_AD_SRAM  2
#define NS_AD_MRAM  3
#define NS_AD_NVM   4   // External MSPI flash (e.g., IS25WX064)
#else
#include "NS_AD_NAME_api.h"
#include "NS_AD_NAME_example_tensors.h"
#include "ns_model.h"
#endif

#if (NS_AD_MODEL_LOCATION == NS_AD_PSRAM) or (NS_AD_ARENA_LOCATION == NS_AD_PSRAM)
#include "ns_peripherals_psram.h"
#endif

#if (NS_AD_MODEL_LOCATION == NS_AD_NVM)
#include "ns_nvm.h"
#endif

ns_timer_config_t tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};

#if NS_AD_AOT == 1

// PMU Map has 71 entries - hardcoded since sizeof() doesn't work with extern arrays
#define NS_PMU_MAP_ENTRIES NS_PMU_MAP_SIZE

static uint32_t aot_matrix_store[NS_AD_AOT_LAYERS * NS_PMU_MAP_SIZE];
static ns_pmu_accm_t aot_matrix;

// PMU Accumulator for AOT Models (each layer has NS_PMU_MAP_ENTRIES counters, and different counters are captured per run)
// typedef struct {
//     uint32_t pmu_events[NS_PMU_MAP_ENTRIES];
//     uint32_t pmu_events_index; // index of the next pmu event to capture
// } pmu_accumulator_t;

// static pmu_accumulator_t pmu_accumulator[NS_AD_AOT_LAYERS]; // one accumulator per layer
static ns_pmu_config_t pmu_cfg;

static bool aot_characterizating = false;
static void aot_callback(int32_t op,
                         NS_AD_NAME_AOT_operator_state_t state,
                         int32_t status,
                         void *user_data)
{
    if (!aot_characterizating) {
        return;
    }

    if (state == NS_AD_NAME_AOT_op_state_run_started) {
        ns_pmu_accm_op_begin(aot_matrix, op);
    }

    if (state == NS_AD_NAME_AOT_op_state_run_finished) {
        ns_pmu_accm_op_end(aot_matrix, op);

    }
}

static NS_AD_NAME_AOT_model_context_t model = {
    // .input_data = {NS_AD_NAME_example_input_tensors},
    // .output_data = {NS_AD_NAME_output_tensors},
    #if NS_AD_JS_PRESENT == 0
    .callback = aot_callback, //aot_callback,
    #else
    .callback = NULL,
    #endif
    .user_data = NULL
};
#else
static ns_model_state_t model;
#endif // NS_AD_AOT == 1


volatile int example_status = 0; // Prevent the compiler from optimizing out while loops

typedef enum { WAITING_TO_RUN, SIGNAL_START_TO_JS, RUNNING, SIGNAL_END_TO_JS } myState_e;

// Button global - will be set by neuralSPOT button helper
static int volatile joulescopeTrigger = 0;

#ifdef NS_MLPROFILE
    #if defined(AM_PART_APOLLO5B) || defined(AM_PART_APOLLO330P_510L)
    extern ns_pmu_config_t ns_microProfilerPMU;
        #if NS_AD_AOT == 0
        extern ns_profiler_sidecar_t ns_microProfilerSidecar;
        #endif
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

// NVM (IS25WX064)
#if (NS_AD_MODEL_LOCATION == NS_AD_NVM)
// Allow build-time overrides; provide sane defaults.
#ifndef NS_AD_NVM_MSPI_MODULE
#define NS_AD_NVM_MSPI_MODULE 1
#endif
#ifndef NS_AD_NVM_CHIP_SELECT
#define NS_AD_NVM_CHIP_SELECT 0   // CE0
#endif
ns_nvm_config_t nvm_cfg = {
    .api              = &ns_nvm_current_version,
    .enable           = true,
    .enable_xip       = true,                  // XIP for model-in-NVM use-cases
    .mspi_module      = NS_AD_NVM_MSPI_MODULE, // 0..3
    .chip_select      = NS_AD_NVM_CHIP_SELECT, // 0 (CE0) or 1 (CE1)
    .iface            = NS_NVM_IF_OCTAL_1_8_8,   // default; override if needed
    .clock_freq       = AM_HAL_MSPI_CLK_96MHZ,
    .nbtxn_buf        = NULL,
    .nbtxn_buf_len    = 0,
    .scrambling_start = 0,
    .scrambling_end   = 0,
    .xip_base_address = 0, // filled by init
    .size_bytes       = 0  // filled by init
};
#endif // NS_AD_MODEL_LOCATION == NS_AD_NVM

#if NS_AD_AOT == 0
int tf_invoke() {
    model.interpreter->Invoke();
    return 0;
}
#endif

uint32_t start_time = 0;
uint32_t end_time = 0;
uint32_t runs = 0;

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


    // ns_itm_printf_enable();

    ns_interrupt_master_enable();
    NS_TRY(ns_power_config(&ns_power_measurement), "Power Init Failed.\n");
    // NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed.\n");

    #if NS_AD_JS_PRESENT == 0
    ns_itm_printf_enable();
    #endif // NS_AD_JS_PRESENT == 0
    // ns_itm_printf_enable();
    ns_lp_printf("Power Init Successful.\n");
    // ns_timer_init(&tickTimer);
        // NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed\n");
    #if (NS_AD_MODEL_LOCATION == NS_AD_PSRAM) or (NS_AD_ARENA_LOCATION == NS_AD_PSRAM)
        NS_TRY(ns_psram_init(&psram_cfg), "PSRAM Init Failed\n");
    #endif
    #if (NS_AD_MODEL_LOCATION == NS_AD_NVM)
        // Perform timing scan + init (and enable XIP if requested)
        ns_lp_printf("Initializing NVM...\n");
        NS_TRY(ns_nvm_init(&nvm_cfg), "NVM Init Failed\n");
    #endif

    NS_TRY(ns_set_performance_mode(NS_AD_CPU_MODE), "Set CPU Perf mode failed.");
    NS_TRY(ns_peripheral_button_init(&joulescopeTrigger_config), "Button initialization failed.\n");

#if NS_AD_AOT == 1
    // Initialize the model
    // Memcpy input and output tensor length arrays to context
    // memcpy(model.outputs[0].data, NS_AD_NAME_AOT_outputs, sizeof(NS_AD_NAME_AOT_outputs));
    // memcpy(model.input_len, NS_AD_NAME_AOT_inputs_len, sizeof(NS_AD_NAME_AOT_inputs_0_size));
    // memcpy(model.output_len, NS_AD_NAME_AOT_outputs_len, sizeof(NS_AD_NAME_AOT_outputs_0_size));
    ns_lp_printf("Initializing AOT model...\n");
    int status = NS_AD_NAME_AOT_model_init(&model); // model init with minimal defaults
    memcpy(model.inputs[0].data, NS_AD_NAME_AOT_power_example_input_tensors, sizeof(NS_AD_NAME_AOT_power_example_input_tensors));

    ns_lp_printf("AOT model init successful.\n");
    ns_lp_printf("AOT input tensor copied to 0x%x\n", model.inputs[0].data);
    pmu_cfg.api = &ns_pmu_V1_0_0;
    ns_pmu_reset_config(&pmu_cfg);

    // reset the accumulator
    // for (int i = 0; i < NS_AD_AOT_LAYERS; i++) {
    //     pmu_accumulator[i].pmu_events_index = 0;
    // }
#else
    int status = NS_AD_NAME_minimal_init(&model); // model init with minimal defaults
#endif
    if (status == NS_AD_NAME_STATUS_FAILURE) {
        ns_lp_printf("TFLM Model init failed.\n");
        while (1);
        example_status = NS_AD_NAME_STATUS_INIT_FAILED; // hang
    }
    ns_lp_printf("TFLM Model init successful.\n");

    // At this point, the model is ready to use - init and allocations were successful
    // Note that the model handle is not meant to be opaque, the structure is defined
    // in ns_model.h, and contains state, config details, and model structure information

#if NS_AD_AOT == 0
    // Get data about input and output tensors
    int numInputs = model.numInputTensors;

    // Initialize input tensors
    int offset = 0;
    for (int i = 0; i < numInputs; i++) {

        memcpy(
            model.model_input[i]->data.int8, ((char *)NS_AD_NAME_AOT_power_example_input_tensors) + offset,
            model.model_input[i]->bytes);
        offset += model.model_input[i]->bytes;
    }
    ns_lp_printf("Input tensors initialized.\n");
#endif

    // capture_snapshot(0);
    // // Enable ITM
    // ns_itm_printf_enable();
    // print_snapshot(0, false);

#if NS_AD_JS_PRESENT == 0
    // If no Joulescope, start running the model without waiting for a trigger
    ns_lp_printf("Current power and performance register settings:\n");
    // ns_delay_us(1000000);
    #if defined(AM_PART_APOLLO5B) || defined(AM_PART_APOLLO330P_510L)
    // Disable ITM
    ns_itm_printf_disable();
    capture_snapshot(0);
    // Enable ITM
    ns_itm_printf_enable();
    print_snapshot(0, false);

    // am_util_pp_snapshot(false, 1, false); // Capture register snapshot 1
    // am_util_pp_snapshot(false, 1, true); // print captured snapshot
    #else
    ns_pp_ap5_snapshot(false, 0, false);
    ns_pp_ap5_snapshot(false, 0, true);
    #endif // AM_PART_APOLLO5B
    joulescopeTrigger = true;
    ns_lp_printf("No Joulescope, starting model without trigger.\n");
#endif
    ns_lp_printf("Starting model...\n");
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
                            // On first, and %100 runs, capture the start time
            // if ((runs <= 5) || (runs == 10) || (runs % 100 == 0)) {
            //     start_time = ns_us_ticker_read(&tickTimer);
            //     ns_lp_printf("Start time: %d us\n", start_time);
            // }
#else
            for (int i = 0; i < 1; i++) { // Only 1 run needed for onboard_perf mode
#endif // NS_AD_JS_PRESENT
            // for (int i = 0; i < NS_AD_POWER_RUNS; i++) {
#if NS_AD_AOT == 0
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
#else
                // ns_lp_printf("Running AOT model...\n");
                NS_AD_NAME_AOT_model_run(&model);
                // ns_lp_printf("AOT model run complete.\n");

#endif
                // if ((runs <= 5) || (runs == 10) || (runs % 100 == 0)) {
                //     end_time = ns_us_ticker_read(&tickTimer);
                //     ns_lp_printf("Model run %d complete. Time: %d us\n", runs, end_time - start_time);
                // }
                // runs++;
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
#if defined(AM_PART_APOLLO5B) || defined(AM_PART_APOLLO330P_510L)

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

#if NS_AD_AOT == 0
            ns_characterize_model(tf_invoke);
            ns_lp_printf("Model characterization complete.\n");
            ns_parse_pmu_stats(num_layers, model.rv_count);
            ns_lp_printf("Model pmu print complete.\n");
#else
            aot_characterizating = true;
            ns_lp_printf("Creating AOT matrix...\n");
            aot_matrix = ns_pmu_accm_create(NS_AD_AOT_LAYERS, NS_PMU_MAP_SIZE, aot_matrix_store);

            do {
                ns_lp_printf("Running AOT model...\n");
                ns_pmu_accm_inference_begin(aot_matrix);
                NS_AD_NAME_AOT_model_run(&model);
                ns_pmu_accm_inference_end(aot_matrix);
            } while (!ns_pmu_accm_complete(aot_matrix));
            ns_lp_printf("AOT model run complete.\n");
            uint32_t *m; ns_pmu_accm_get(aot_matrix, &m);
            ns_lp_printf("AOT matrix complete.\n");
            ns_pmu_accmprint_matrix(aot_matrix, m, NS_AD_AOT_LAYERS, NS_PMU_MAP_SIZE);
            ns_lp_printf("AOT matrix print complete.\n");
            // dump_csv(m, NS_AD_AOT_LAYERS, NS_PMU_MAP_SIZE);
            ns_pmu_accm_destroy(aot_matrix);
            aot_characterizating = false;

            ns_lp_printf("AOT model characterization complete.\n");
            // // print the pmu stats
            // for (int op = 0; op < NS_AD_AOT_LAYERS; op++) {
            //     ns_lp_printf("Layer %d: ", op);
            //     for (int i = 0; i < NS_PMU_MAP_ENTRIES; i++) {
            //         ns_lp_printf("%d ", pmu_accumulator[op].pmu_events[i]);
            //     }
            //     ns_lp_printf("\n");
            // }
#endif

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
