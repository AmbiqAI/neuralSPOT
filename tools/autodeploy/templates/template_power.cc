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

#include "ns_core.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_energy_monitor.h"
#include "ns_peripherals_power.h"
#include "ns_peripherals_button.h"

#include "NS_AD_NAME_api.h"
#include "NS_AD_NAME_example_tensors.h"
#include "ns_model.h"

// TFLM Config
static ns_model_state_t model;
volatile int example_status = 0; // Prevent the compiler from optimizing out while loops

typedef enum { WAITING_TO_RUN, SIGNAL_START_TO_JS, RUNNING, SIGNAL_END_TO_JS } myState_e;

// Button global - will be set by neuralSPOT button helper
static int volatile joulescopeTrigger = 0;

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
    .bNeedSharedSRAM = false,
    .bNeedCrypto = false,
    .bNeedBluetooth = false,
    .bNeedUSB = false,
    .bNeedIOM = false,
    .bNeedAlternativeUART = false,
    .b128kTCM = false,
    .bEnableTempCo = false,
    .bNeedITM = false};

int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");

    myState_e state = WAITING_TO_RUN;

    // Initialize the model, get handle if successful
    ns_init_power_monitor_state();
    ns_set_power_monitor_state(NS_IDLE);

    NS_TRY(ns_power_config(&ns_power_measurement), "Power Init Failed.\n");
    // ns_itm_printf_enable();
    NS_TRY(ns_set_performance_mode(NS_AD_CPU_MODE), "Set CPU Perf mode failed.");
    NS_TRY(ns_peripheral_button_init(&joulescopeTrigger_config), "Button initialization failed.\n");

    int status = NS_AD_NAME_minimal_init(&model); // model init with minimal defaults
    ns_interrupt_master_enable();
    if (status == NS_AD_NAME_STATUS_FAILURE) {
        while (1)
            // ns_lp_printf("Model init failed.\n");
            example_status = NS_AD_NAME_STATUS_INIT_FAILED; // hang
    }
    // ns_lp_printf("Model init successful.\n");

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

    // Event loop
    while (1) {
        switch (state) {

        case WAITING_TO_RUN:
            if (joulescopeTrigger) {
                state = SIGNAL_START_TO_JS;
                joulescopeTrigger = false;
            }
            break;

        case SIGNAL_START_TO_JS:
            // ns_lp_printf("Starting...\n");
            ns_set_power_monitor_state(3);

            ns_delay_us(100000);
            state = RUNNING;
            break;

        case RUNNING:
            // ns_lp_printf("Running...\n");
            ns_set_power_monitor_state(1);
            for (int i = 0; i < NS_AD_POWER_RUNS; i++) {
                model.interpreter->Invoke();
            }
            // ns_delay_us(1100000);
            state = SIGNAL_END_TO_JS;
            break;

        case SIGNAL_END_TO_JS:
            // ns_lp_printf("Ending...\n");
            ns_set_power_monitor_state(2);
            ns_delay_us(100000);

            state = WAITING_TO_RUN;
            break;
        }
    }

    while (1) {
        // Success!
        example_status = NS_AD_NAME_STATUS_SUCCESS;
    }
}
