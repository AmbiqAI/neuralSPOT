#include "ns_usb.h"
#include "unity/unity.h"
#include "ns_core.h"

static uint8_t rx_buf[100];
static uint8_t tx_buf[100];
static ns_usb_config_t cfg;

static void reset_usb_cfg() {
    cfg.api = &ns_usb_V1_0_0;
    cfg.deviceType = NS_USB_VENDOR_DEVICE;
    cfg.rx_buffer = rx_buf;
    cfg.rx_bufferLength = 10;
    cfg.tx_buffer = tx_buf;
    cfg.tx_bufferLength = 10;
    cfg.rx_cb = NULL;
    cfg.tx_cb = NULL;
    cfg.service_cb = NULL;
}

void ns_usb_tests_pre_test_hook() {
    reset_usb_cfg();
}

void ns_usb_tests_post_test_hook() {
    // post hook if needed
}

// Basic init should pass
void ns_usb_test_init() {
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
    reset_usb_cfg();
    cfg.rx_buffer = NULL;
    cfg.tx_buffer = NULL;
    usb_handle_t * handle;
    uint32_t status = ns_usb_init(&cfg, handle);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_CONFIG, status);
}

// Invalid API
void ns_usb_test_init_invalid_api() {
    reset_usb_cfg();
    usb_handle_t * handle;
    uint32_t status = ns_usb_init(&cfg, handle);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_HANDLE, status);
}


