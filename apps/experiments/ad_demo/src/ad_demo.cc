/**
 * @file ad01_int8_aligned_power_example.cc
 * @author Carlos Morales
 * @brief Minimal application instantiating a TFLM model, feeding it
 * a test input tensor, and checking the result
 * @version 0.1
 * @date 2023-02-28
 *
 * @copyright Copyright (c) 2023
 *
 */

#if defined(AM_PART_APOLLO5B) || defined(AM_PART_APOLLO510L)
#define NS_PROFILER_PMU_EVENT_0 PMU_EVENT0_NA
#define NS_PROFILER_PMU_EVENT_1 PMU_EVENT1_NA
#define NS_PROFILER_PMU_EVENT_2 PMU_EVENT2_NA
#define NS_PROFILER_PMU_EVENT_3 PMU_EVENT3_NA
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
// #if defined(AM_PART_APOLLO5B) || defined(AM_PART_APOLLO510L)
// #include "ns_pp.h"
// #else
#include "ns_power_profile.h"
// #include "am_hal_spotmgr.h"
// #endif
#if 0 == 1
#include "ad01_int8_aligned_model.h"
#include "ad01_int8_aligned_platform.h"         // platform macros
#include "ad01_int8_aligned_tensors.h"          // tensor sizes
#include "ad01_int8_aligned_context.h"          // model context
#include "ad01_int8_aligned_power_aot_api.h"
#include "ad01_int8_aligned_power_aot_example_tensors.h"
// Memory locations for model and arena
#define NS_AD_TCM   0
#define NS_AD_PSRAM 1
#define NS_AD_SRAM  2
#define NS_AD_MRAM  3
#define NS_AD_NVM   4   // External MSPI flash (e.g., IS25WX064)
#else
#include "ad01_int8_aligned_power_api.h"
#include "ad01_int8_aligned_power_example_tensors.h"
#include "ns_model.h"
#endif

#if (NS_AD_TCM == NS_AD_PSRAM) or (NS_AD_TCM == NS_AD_PSRAM)
#include "ns_peripherals_psram.h"
#endif

#if (NS_AD_TCM == NS_AD_NVM)
#include "ns_nvm.h"
#endif

ns_timer_config_t tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};

#if 0 == 1

// PMU Map has 71 entries - hardcoded since sizeof() doesn't work with extern arrays
#define NS_PMU_MAP_ENTRIES NS_PMU_MAP_SIZE

static uint32_t aot_matrix_store[0 * NS_PMU_MAP_SIZE];
static ns_pmu_accm_t aot_matrix;

// PMU Accumulator for AOT Models (each layer has NS_PMU_MAP_ENTRIES counters, and different counters are captured per run)
// typedef struct {
//     uint32_t pmu_events[NS_PMU_MAP_ENTRIES];
//     uint32_t pmu_events_index; // index of the next pmu event to capture
// } pmu_accumulator_t;

// static pmu_accumulator_t pmu_accumulator[0]; // one accumulator per layer
static ns_pmu_config_t pmu_cfg;

static bool aot_characterizating = false;
static void aot_callback(int32_t op,
                         ad01_int8_aligned_operator_state_t state,
                         int32_t status,
                         void *user_data)
{
    if (!aot_characterizating) {
        return;
    }

    if (state == ad01_int8_aligned_op_state_run_started) {
        ns_pmu_accm_op_begin(aot_matrix, op);
    }

    if (state == ad01_int8_aligned_op_state_run_finished) {
        ns_pmu_accm_op_end(aot_matrix, op);

    }
}

static ad01_int8_aligned_model_context_t model = {
    // .input_data = {ad01_int8_aligned_power_example_input_tensors},
    // .output_data = {ad01_int8_aligned_power_output_tensors},
    #if 1 == 0
    .callback = aot_callback, //aot_callback,
    #else
    .callback = NULL,
    #endif
    .user_data = NULL
};
#else
static ns_model_state_t model;
#endif // 0 == 1


volatile int example_status = 0; // Prevent the compiler from optimizing out while loops

typedef enum { WAITING_TO_RUN, SIGNAL_START_TO_JS, RUNNING, SIGNAL_END_TO_JS } myState_e;

// Button global - will be set by neuralSPOT button helper
static int volatile joulescopeTrigger = 0;

#ifdef NS_MLPROFILE
    #if defined(AM_PART_APOLLO5B) || defined(AM_PART_APOLLO510L)
    extern ns_pmu_config_t ns_microProfilerPMU;
        #if 0 == 0
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
    .eAIPowerMode = NS_MAXIMUM_PERF,
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
#if (NS_AD_TCM == NS_AD_PSRAM) or (NS_AD_TCM == NS_AD_PSRAM)
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
#if (NS_AD_TCM == NS_AD_NVM)
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
#endif // NS_AD_TCM == NS_AD_NVM

#if 0 == 0
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

    ns_itm_printf_enable();


    #if 1 == 0
    ns_itm_printf_enable();
    #endif // 1 == 0
    // ns_itm_printf_enable();
    ns_lp_printf("Power Init Successful.\n");
    // ns_timer_init(&tickTimer);
        // NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed\n");
    #if (NS_AD_TCM == NS_AD_PSRAM) or (NS_AD_TCM == NS_AD_PSRAM)
        NS_TRY(ns_psram_init(&psram_cfg), "PSRAM Init Failed\n");
    #endif
    #if (NS_AD_TCM == NS_AD_NVM)
        // Perform timing scan + init (and enable XIP if requested)
        ns_lp_printf("Initializing NVM...\n");
        NS_TRY(ns_nvm_init(&nvm_cfg), "NVM Init Failed\n");
    #endif

    NS_TRY(ns_set_performance_mode(NS_MAXIMUM_PERF), "Set CPU Perf mode failed.");
    NS_TRY(ns_peripheral_button_init(&joulescopeTrigger_config), "Button initialization failed.\n");

#if 0 == 1
    // Initialize the model
    // Memcpy input and output tensor length arrays to context
    // memcpy(model.outputs[0].data, ad01_int8_aligned_outputs, sizeof(ad01_int8_aligned_outputs));
    // memcpy(model.input_len, ad01_int8_aligned_inputs_len, sizeof(ad01_int8_aligned_inputs_0_size));
    // memcpy(model.output_len, ad01_int8_aligned_outputs_len, sizeof(ad01_int8_aligned_outputs_0_size));
    ns_lp_printf("Initializing AOT model...\n");
    int status = ad01_int8_aligned_model_init(&model); // model init with minimal defaults
    memcpy(model.inputs[0].data, ad01_int8_aligned_power_example_input_tensors, sizeof(ad01_int8_aligned_power_example_input_tensors));

    ns_lp_printf("AOT model init successful.\n");
    ns_lp_printf("AOT input tensor copied to 0x%x\n", model.inputs[0].data);
    pmu_cfg.api = &ns_pmu_V1_0_0;
    ns_pmu_reset_config(&pmu_cfg);

    // reset the accumulator
    // for (int i = 0; i < 0; i++) {
    //     pmu_accumulator[i].pmu_events_index = 0;
    // }
#else
    int status = ad01_int8_aligned_power_minimal_init(&model); // model init with minimal defaults
#endif
    if (status == ad01_int8_aligned_power_STATUS_FAILURE) {
        ns_lp_printf("TFLM Model init failed.\n");
        while (1);
        example_status = ad01_int8_aligned_power_STATUS_INIT_FAILED; // hang
    }
    ns_lp_printf("TFLM Model init successful.\n");

    // At this point, the model is ready to use - init and allocations were successful
    // Note that the model handle is not meant to be opaque, the structure is defined
    // in ns_model.h, and contains state, config details, and model structure information

#if 0 == 0
    // Get data about input and output tensors
    int numInputs = model.numInputTensors;

    // Initialize input tensors
    int offset = 0;
    for (int i = 0; i < numInputs; i++) {

        memcpy(
            model.model_input[i]->data.int8, ((char *)ad01_int8_aligned_power_example_input_tensors) + offset,
            model.model_input[i]->bytes);
        offset += model.model_input[i]->bytes;
    }
    ns_lp_printf("Input tensors initialized.\n");
#endif

    // capture_snapshot(0);
    // // Enable ITM
    // ns_itm_printf_enable();
    // print_snapshot(0, false);

#if 1 == 1
    // If no Joulescope , start running the model without waiting for a trigger
    ns_lp_printf("Current power and performance register settings:\n");
    // ns_delay_us(1000000);
    #if defined(AM_PART_APOLLO5B) || defined(AM_PART_APOLLO510L)
    // Disable ITM
    ns_itm_printf_disable();
    capture_snapshot(0);
    // Enable ITM
    ns_itm_printf_enable();
    print_snapshot(0, false);

    // am_util_pp_snapshot(false, 1, false); // Capture register snapshot 1
    // am_util_pp_snapshot(false, 1, true); // print captured snapshot
    #else
    // ns_pp_ap5_snapshot(false, 0, false);
    // ns_pp_ap5_snapshot(false, 0, true);
    #endif // AM_PART_APOLLO5B
    joulescopeTrigger = true;
    ns_lp_printf("No Joulescope, starting model without trigger.\n");
#endif
    ns_lp_printf("Starting model...\n");
    // Event loop


            
            // Silly Demo - run forever, print a . every 10 runs.
            int i = 0;
            int cr = 0;
            while (1) {
                model.interpreter->Invoke();
                i++;
                if (i % 10 == 0) {
                    ns_lp_printf(".");
                    cr++;
                    if (cr > 100) {
                        ns_lp_printf("\n");
                        cr = 0;
                    }
                }
            }

}
