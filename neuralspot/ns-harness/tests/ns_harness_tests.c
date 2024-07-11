#include "unity/unity.h"
#include "ns_core.h"
#include <stdio.h>
char output_buffer[1024];


void ns_harness_tests_pre_test_hook() {

}
void ns_harness_tests_post_test_hook() {

}

// Dont call ns_core_init first -- Undefined Behavior
void itm_enable_noCoreInitTest() {
    ns_itm_printf_enable();
    TEST_ASSERT_FALSE(g_ns_state.itmPrintCurrentlyEnabled);
    TEST_ASSERT_FALSE(g_ns_state.itmPrintWantsToBeEnabled);

}
// Test that itm_printf_enable() enables ITM printf
void ns_itm_printf_enable_test() {
    ns_core_config_t cfg = {
        .api = &ns_core_V1_0_0
    };
    uint32_t status = ns_core_init(&cfg);
    ns_core_init(&cfg);
    ns_itm_printf_enable();
    TEST_ASSERT_TRUE(g_ns_state.itmPrintCurrentlyEnabled);
    TEST_ASSERT_TRUE(g_ns_state.itmPrintWantsToBeEnabled);

}

// Test that itm_printf_disable() disables ITM printf
void ns_itm_printf_disable_test() {
    ns_core_config_t cfg = {
        .api = &ns_core_V1_0_0
    };
    uint32_t status = ns_core_init(&cfg);
    ns_core_init(&cfg);
    ns_itm_printf_enable();
    ns_itm_printf_disable;
    TEST_ASSERT_FALSE(g_ns_state.itmPrintCurrentlyEnabled);
}

// Ns_lp_printf: Neither itm nor uart enabled
void ns_lp_printf_no_itm_uart_test() {
    ns_core_config_t cfg = {
        .api = &ns_core_V1_0_0
    };
    uint32_t status = ns_core_init(&cfg);
    ns_core_init(&cfg);
    TEST_ASSERT_FALSE(g_ns_state.uartPrintCurrentlyEnabled);
    TEST_ASSERT_FALSE(g_ns_state.itmPrintCurrentlyEnabled);
    TEST_ASSERT_FALSE(g_ns_state.uartPrintWantsToBeEnabled);
    TEST_ASSERT_FALSE(g_ns_state.itmPrintWantsToBeEnabled);
}

// Both uart and itm enabled
void itm_uart_enabled_test() {
    ns_core_config_t cfg = {
        .api = &ns_core_V1_0_0
    };
    uint32_t status = ns_core_init(&cfg);
    ns_core_init(&cfg);
    ns_itm_printf_enable();
    ns_uart_printf_enable();
    ns_lp_printf("Hello, World!\n");

    // Should probably fail?
    TEST_FAIL_MESSAGE("This test should not pass");
}


// Poorly formatted String -- not sure how this should be handled yet
void poorly_formatted_string_test() {
    ns_core_config_t cfg = {
        .api = &ns_core_V1_0_0
    };
    uint32_t status = ns_core_init(&cfg);
    ns_core_init(&cfg);
    ns_itm_printf_enable();
    int x =  5;
    ns_lp_printf("%s", x);
    TEST_FAIL_MESSAGE("This test should not pass");
}


// Only uart enabled
void uart_enabled_test() {
    ns_itm_printf_disable;
    ns_uart_printf_enable();
    TEST_ASSERT_TRUE(g_ns_state.uartPrintCurrentlyEnabled);
    ns_lp_printf("Hello, World!\n");
}
