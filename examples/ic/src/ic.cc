/**
 * @file ic_bench_example.cc
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

#include "ic_bench_api.h"
#include "ic_bench_example_tensors.h"
#include "ns_model.h"

// TFLM Config
static ns_model_state_t model;
volatile int example_status = 0; // Prevent the compiler from optimizing out while loops

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

ns_timer_config_t basic_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};

int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");

    // Initialize the model, get handle if successful
    ns_init_power_monitor_state();
    ns_set_power_monitor_state(NS_IDLE);

    NS_TRY(ns_power_config(&ns_power_measurement), "Power Init Failed.\n");
    NS_TRY(ns_set_performance_mode(NS_MAXIMUM_PERF), "Set CPU Perf mode failed.");
    ns_itm_printf_enable();

    NS_TRY(ns_timer_init(&basic_tickTimer), "Timer init failed.\n");

    // ns_lp_printf("About to set CPU speed\n");
    // int s = am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE);
    // int s = am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_LOW_POWER);
    // int s = 0;
    // am_util_debug_printf("Hello World! stat = %d sim 0x%x trim 0x%x gpfreq 0x%x\n",
    //     s, PWRCTRL->VRSTATUS_b.SIMOBUCKST, g_ui32TrimVer, CLKGEN->MISC_b.CPUHPFREQSEL);
    // am_hal_gpio_state_write(74, AM_HAL_GPIO_OUTPUT_CLEAR);
    // am_hal_gpio_pinconfig(74, am_hal_gpio_pincfg_output);

    // am_hal_gpio_state_write(74, AM_HAL_GPIO_OUTPUT_CLEAR);
    // am_hal_gpio_pinconfig(74, am_hal_gpio_pincfg_output);

    // Initialize the model, get handle if successful
    int status = ic_bench_minimal_init(&model); // model init with minimal defaults

    if (status == ic_bench_STATUS_FAILURE) {
        ns_lp_printf("Model init failed... status %d\n", status);
        while (1)
            example_status = ic_bench_STATUS_INIT_FAILED; // hang
    }

    // At this point, the model is ready to use - init and allocations were successful
    // Note that the model handle is not meant to be opaque, the structure is defined
    // in ns_model.h, and contains state, config details, and model structure information

    // Get data about input and output tensors
    int numInputs = model.numInputTensors;
    int numOutputs = model.numOutputTensors;

    // Initialize input tensors
    int offset = 0;
    for (int i = 0; i < numInputs; i++) {
        memcpy(
            model.model_input[i]->data.int8, ((char *)ic_bench_example_input_tensors) + offset,
            model.model_input[i]->bytes);
        offset += model.model_input[i]->bytes;
    }

    // am_hal_gpio_state_write(74, AM_HAL_GPIO_OUTPUT_SET);
    ns_lp_printf("Starting...\n");
    int invokes = 0;
    // for (int i = 0; i < 100; i++) {
    uint32_t oldTime = ns_us_ticker_read(&basic_tickTimer);
    uint32_t newTime = oldTime;
    float ips = 0;
    while (1) {
        example_status = model.interpreter->Invoke();
        invokes++;
        ns_lp_printf(".");
        newTime = ns_us_ticker_read(&basic_tickTimer);
        if (newTime - oldTime > 1000000) {
            ips = (float)invokes / ((float)(newTime - oldTime) / 1000000.0f);
            ns_lp_printf("\nImage Classification FPS: %0.2f\n", ips);
            invokes = 0;
            oldTime = newTime;
        }

        if (invokes > 10) {
            invokes = 0;
            oldTime = newTime;
        }

        // if (example_status != kTfLiteOk) {
        //     ns_lp_printf("Invoke failed...\n");
        //     while (1)
        //         example_status = ic_bench_STATUS_INVALID_CONFIG; // hang
        // }
    }
    ns_lp_printf("Done...\n");
    // am_hal_gpio_state_write(74, AM_HAL_GPIO_OUTPUT_CLEAR);

    // Compare the bytes of the output tensors against expected values
    offset = 0;
    for (int i = 0; i < numOutputs; i++) {
        if (0 != memcmp(
                     model.model_output[i]->data.int8,
                     ((char *)ic_bench_example_output_tensors) + offset,
                     model.model_output[i]->bytes)) {
            while (1)
                example_status = ic_bench_STATUS_INVALID_CONFIG; // miscompare, so hang
        }
        offset += model.model_output[i]->bytes;
    }

    while (1) {
        // Success!
        example_status = ic_bench_STATUS_SUCCESS;
    }
}
