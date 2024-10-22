

# File ns\_button\_tests.c

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-peripherals**](dir_62cbd78784261bb0d09981988628a167.md) **>** [**tests**](dir_212d9119d40fadcef4edfdca97975f2d.md) **>** [**ns\_button\_tests.c**](ns__button__tests_8c.md)

[Go to the documentation of this file](ns__button__tests_8c.md)

```C++

#include "ns_peripherals_button.h"
#include "unity/unity.h"
ns_button_config_t cfg;

static void reset_button_config() {
    volatile int g_intButtonPressed = 0;
    cfg.api = &ns_button_V0_0_1;
    cfg.button_0_enable = true;
    cfg.button_1_enable = false;
    cfg.button_0_flag = &g_intButtonPressed;
    cfg.button_1_flag = NULL;
}

void ns_button_tests_pre_test_hook() {
    reset_button_config();
}

void ns_button_tests_post_test_hook() {
    // post hook if needed
}


void ns_button_basic_init_test() {
    int status = ns_peripheral_button_init(&cfg);
    TEST_ASSERT_EQUAL(NS_STATUS_SUCCESS, status);
}
void ns_button_init_null_test() {
    int status = ns_peripheral_button_init(NULL);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_HANDLE, status);
}

void ns_button_invalid_api_test() {
    cfg.api = NULL;
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_VERSION, ns_peripheral_button_init(&cfg));

    const ns_core_api_t invalid_api = {
    .apiId = 0xFFFF, .version = NS_BUTTON_V0_0_1};
    cfg.api = &invalid_api;
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_VERSION, ns_peripheral_button_init(&cfg));
}

void ns_button_init_both_buttons_enable_test() {
    reset_button_config();
    volatile int g_intButtonPressed2 = 0;
    cfg.button_1_enable = true;
    cfg.button_1_flag = &g_intButtonPressed2;
    TEST_ASSERT_EQUAL(NS_STATUS_SUCCESS, ns_peripheral_button_init(&cfg));
}

// Init with both buttons disabled should probably fail?
void ns_button_init_both_buttons_disable_test() {
    reset_button_config();
    cfg.button_0_enable = false;
    cfg.button_1_enable = false;
    TEST_ASSERT_NOT_EQUAL(NS_STATUS_SUCCESS, ns_peripheral_button_init(&cfg));
    
}

void ns_button_init_joulescope_trigger_enable_test() {
    reset_button_config();
    volatile int joulescope_flag = 0;
    cfg.joulescope_trigger_enable = true;
    cfg.joulescope_trigger_flag = &joulescope_flag;
    TEST_ASSERT_EQUAL(NS_STATUS_SUCCESS, ns_peripheral_button_init(&cfg));
}

// Should probably fail, but it doesn't because the flag is not checked
void ns_button_enable_no_flag_test() {
    reset_button_config();
    cfg.button_0_flag = NULL;
    TEST_ASSERT_NOT_EQUAL(NS_STATUS_SUCCESS, ns_peripheral_button_init(&cfg));
}

// Should probably fail, but it doesn't because the flag is not checked
void ns_button_joulescope_enable_no_flag_test() {
    reset_button_config();
    cfg.joulescope_trigger_enable = true;
    cfg.joulescope_trigger_flag = NULL;
    TEST_ASSERT_NOT_EQUAL(NS_STATUS_SUCCESS, ns_peripheral_button_init(&cfg));
}

```

