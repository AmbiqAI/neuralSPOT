/**
 * @file NS_AD_NAME_test.cc
 * @author Carlos Morales
 * @brief Neuralspot Test Runner
 * @version 0.1
 * @date 2024-6-14
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ns_core.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_peripherals_power.h"
#include "NS_AT_CONTAINER.h"
#include "unity/unity.h"

int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed.\n");

    ns_itm_printf_enable();
    // ns_lp_printf("Starting tests\n");

    UnityBegin("NS_AT_MAIN");
    // Pre-test hook
    NS_AT_NAME_pre_test_hook();

    // AM_INIT_TEST();

    // Run tests
    NS_AT_RUN_TEST_LIST

    // Post-test hook
    NS_AT_NAME_post_test_hook();
    UnityEnd();

    ns_lp_printf("Tests complete\n");

    while (1)
        ;
}
