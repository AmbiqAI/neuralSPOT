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
#include "ns_peripherals_power.h"
#include "NS_AT_CONTAINER.h"
#include "unity.h"

int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed.\n");

    ns_itm_printf_enable();

    // Pre-test hook
    NS_AT_NAME_pre_test_hook();

    // Run tests
    NS_AT_RUN_TEST_LIST

    // Post-test hook
    NS_AT_NAME_post_test_hook();

    while (1)
        ;
}
