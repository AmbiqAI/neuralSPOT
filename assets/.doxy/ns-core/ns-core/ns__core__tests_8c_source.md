

# File ns\_core\_tests.c

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-core**](dir_7a01d249276e526cbac17daf32597066.md) **>** [**tests**](dir_ae8b3b42ecd578532e898a7057869875.md) **>** [**ns\_core\_tests.c**](ns__core__tests_8c.md)

[Go to the documentation of this file](ns__core__tests_8c.md)

```C++

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

```

