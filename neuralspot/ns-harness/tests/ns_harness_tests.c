#include "unity/unity.h"
#include "ns_core.h"
#include <stdio.h>


void ns_harness_tests_pre_test_hook() {

}
void ns_harness_tests_post_test_hook() {
}


// Test that itm_printf_enable() enables ITM printf
void ns_itm_printf_enable_test() {
    ns_itm_printf_disable();
    ns_itm_printf_enable();
    TEST_ASSERT_TRUE(g_ns_state.itmPrintCurrentlyEnabled);
    TEST_ASSERT_TRUE(g_ns_state.itmPrintWantsToBeEnabled);
}

// Test that itm_printf_disable() disables ITM printf
void ns_itm_printf_disable_test() {
    ns_itm_printf_disable;
    TEST_ASSERT_FALSE(g_ns_state.itmPrintCurrentlyEnabled);
}

// Ns_lp_printf: Neither itm nor uart enabled
void ns_lp_printf_no_itm_uart_test() {
    ns_itm_printf_disable();
    ns_uart_printf_disable();
    TEST_ASSERT_FALSE(g_ns_state.uartPrintCurrentlyEnabled);
    TEST_ASSERT_FALSE(g_ns_state.itmPrintCurrentlyEnabled);
    TEST_ASSERT_FALSE(g_ns_state.uartPrintWantsToBeEnabled);
    TEST_ASSERT_FALSE(g_ns_state.itmPrintWantsToBeEnabled);
}

// Both uart and itm enabled
void itm_uart_enabled_test() {
    ns_itm_printf_enable();
    ns_uart_printf_enable();
    TEST_ASSERT_TRUE(g_ns_state.uartPrintCurrentlyEnabled);
}

// Only uart enabled
void uart_enabled_test() {
    ns_itm_printf_disable;
    ns_uart_printf_enable();
    TEST_ASSERT_TRUE(g_ns_state.uartPrintCurrentlyEnabled);
}
