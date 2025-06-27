#include "ns_core.h"
#include "unity/unity.h"
#define NS_CORE_INVALID                                                                         \
        { .major = 5, .minor = 5, .revision = 5 }

void ns_core_tests_pre_test_hook() {
    // pre hook if needed
}

void ns_core_tests_post_test_hook() {
    // post hook if needed
}

// Basic test should pass
void ns_core_test_init() {
    ns_core_config_t cfg = {
        .api = &ns_core_V1_0_0
    };
    uint32_t status = ns_core_init(&cfg);
    TEST_ASSERT_EQUAL(NS_STATUS_SUCCESS, status);
}

// Test null config
void ns_core_test_null_cfg() {
    uint32_t status = ns_core_init(NULL);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_HANDLE, status);
}

// Test different invalid api versions
void ns_core_test_invalid_api() {
    ns_core_config_t cfg = {
        .api = NULL
    };
    uint32_t status = ns_core_init(&cfg);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_VERSION, status);
    // Invalid API id
    ns_core_api_t invalid_api = {.apiId = 0xCCCCCC, .version = NS_CORE_V0_0_1};
    cfg.api = &invalid_api;
    uint32_t status2 = ns_core_init(&cfg);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_VERSION, status2);

    // Invalid API version
    ns_core_api_t invalid_api2 = {.apiId = 0xCA0000, .version = NS_CORE_INVALID};
    cfg.api = &invalid_api;
    uint32_t status3 = ns_core_init(&cfg);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_VERSION, status3);
}