#include "ns_usb.h"
#include "unity/unity.h"

void ns_usb_tests_pre_test_hook() {
    // pre hook if needed
}

void ns_usb_tests_post_test_hook() {
    // post hook if needed
}

// Basic init should pass
void ns_usb_test_init() {
    uint8_t rx_buf[100];
    uint8_t tx_buf[100];
    ns_usb_config_t cfg = {
        .api = &ns_usb_V1_0_0,
        .deviceType = NS_USB_CDC_DEVICE,
        .rx_buffer = rx_buf,
        .rx_bufferLength = 10,
        .tx_buffer = tx_buf,
        .tx_bufferLength = 10,
        .rx_cb = NULL,
        .tx_cb = NULL,
        .service_cb = NULL
    };
    usb_handle_t * handle;
    uint32_t status = ns_usb_init(&cfg, handle);
    TEST_ASSERT_EQUAL(NS_STATUS_SUCCESS, status);
}

// Null config should fail
void ns_usb_test_init_null_config() {
    usb_handle_t * handle;
    uint32_t status = ns_usb_init(NULL, handle);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_HANDLE, status);
}

// Null rx and tx buffer should fail
void ns_usb_test_init_null_buffers() {
    ns_usb_config_t cfg = {
        .api = &ns_usb_V1_0_0,
        .deviceType = NS_USB_CDC_DEVICE,
        .rx_buffer = NULL,
        .rx_bufferLength = 10,
        .tx_buffer = NULL,
        .tx_bufferLength = 10,
        .rx_cb = NULL,
        .tx_cb = NULL,
        .service_cb = NULL
    };
    usb_handle_t * handle;
    uint32_t status = ns_usb_init(&cfg, handle);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_HANDLE, status);
}

// Invalid API
void ns_usb_test_init_invalid_api() {
    uint8_t rx_buf[100];
    uint8_t tx_buf[100];
    // use timer api instead of usb
    ns_core_api_t invalid_usb_api = {.apiId = NS_TIMER_API_ID, .version = NS_TIMER_V1_0_0};
    ns_usb_config_t cfg = {
        .api = &invalid_usb_api,
        .deviceType = NS_USB_CDC_DEVICE,
        .rx_buffer = rx_buf,
        .rx_bufferLength = 10,
        .tx_buffer = tx_buf,
        .tx_bufferLength = 10,
        .rx_cb = NULL,
        .tx_cb = NULL,
        .service_cb = NULL
    };
    usb_handle_t * handle;
    uint32_t status = ns_usb_init(&cfg, handle);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_HANDLE, status);
}


