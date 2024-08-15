#include "unity/unity.h"
#include "ns_peripherals_power.h"
#include "ns_core.h"

static ns_power_config_t custom_cfg;

static ns_core_config_t core_cfg = {
        .api = &ns_core_V1_0_0,
    };
static void reset_custom_cfg() {
    custom_cfg.eAIPowerMode = NS_MAXIMUM_PERF;
    custom_cfg.api = &ns_power_V1_0_0;
    custom_cfg.bNeedAudAdc = true;
    custom_cfg.bNeedSharedSRAM = true;
    custom_cfg.bNeedCrypto = false;
    custom_cfg.bNeedBluetooth = true;
    custom_cfg.bNeedUSB = true;
    custom_cfg.bNeedIOM = true;
    custom_cfg.bNeedAlternativeUART = true;
    custom_cfg.b128kTCM = false;
    custom_cfg.bEnableTempCo = false;
    custom_cfg.bNeedITM = true;
}

void ns_power_tests_pre_test_hook() {
    reset_custom_cfg();
}

void ns_power_tests_post_test_hook() {
    // post hook if needed
}

void ns_power_config_null_test() {
    ns_core_init(&core_cfg);
    uint32_t status = ns_power_config(NULL);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_HANDLE, status);
}

// Init each predefined power setting
void ns_power_config_test() {
    int status = ns_power_config(&ns_development_default);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);

    status = ns_power_config(&ns_debug_default);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);

    status = ns_power_config(&ns_good_default);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);

    status = ns_power_config(&ns_mlperf_mode1);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);

    status = ns_power_config(&ns_mlperf_mode2);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);

    status = ns_power_config(&ns_mlperf_mode3);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);

    status = ns_power_config(&ns_audio_default);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);

}


void ns_power_config_invalid_api_test() {
    custom_cfg.api = NULL;
    uint32_t status = ns_power_config(&custom_cfg);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_VERSION, status);
    ns_core_api_t invalid_api = {
        .apiId = 0,
        .version = NS_CORE_V0_0_1,
    };

    custom_cfg.api = &invalid_api;
    status = ns_power_config(&custom_cfg);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_VERSION, status);
}

// Test each power mode configuration including invalid one
void ns_power_config_power_mode_test() {
    reset_custom_cfg();
    custom_cfg.eAIPowerMode = 0;
    ns_lp_printf("what\n");
    uint32_t status = ns_power_config(&custom_cfg);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);

    custom_cfg.eAIPowerMode = 1;
    status = ns_power_config(&custom_cfg);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);

    custom_cfg.eAIPowerMode = 2;
    status = ns_power_config(&custom_cfg);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);

    // Not a valid power mode
    custom_cfg.eAIPowerMode = 3;
    status = ns_power_config(&custom_cfg);
    TEST_ASSERT_NOT_EQUAL(AM_HAL_STATUS_SUCCESS, status);

}

void ns_power_config_all_true_test() {
    reset_custom_cfg();
    custom_cfg.bNeedAudAdc = true;
    custom_cfg.bNeedSharedSRAM = true;
    custom_cfg.bNeedCrypto = true;
    custom_cfg.bNeedBluetooth = true;
    custom_cfg.bNeedUSB = true;
    custom_cfg.bNeedIOM = true;
    custom_cfg.bNeedAlternativeUART = true;
    custom_cfg.b128kTCM = true;
    custom_cfg.bEnableTempCo = true;
    custom_cfg.bNeedITM = true;

    uint32_t status = ns_power_config(&custom_cfg);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);
}

void ns_power_config_all_false_test() {
    reset_custom_cfg();
    custom_cfg.bNeedAudAdc = false;
    custom_cfg.bNeedSharedSRAM = false;
    custom_cfg.bNeedCrypto = false;
    custom_cfg.bNeedBluetooth = false;
    custom_cfg.bNeedUSB = false;
    custom_cfg.bNeedIOM = false;
    custom_cfg.bNeedAlternativeUART = false;
    custom_cfg.b128kTCM = false;
    custom_cfg.bEnableTempCo = false;
    custom_cfg.bNeedITM = false;

    uint32_t status = ns_power_config(&custom_cfg);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);
}