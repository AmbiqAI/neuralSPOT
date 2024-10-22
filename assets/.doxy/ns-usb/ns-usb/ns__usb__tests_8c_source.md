

# File ns\_usb\_tests.c

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-usb**](dir_450d7ce7daa9d29b9b3b5cb7d00f16f9.md) **>** [**tests**](dir_c776a9f5367fd77ebb6bd7a429b2b06e.md) **>** [**ns\_usb\_tests.c**](ns__usb__tests_8c.md)

[Go to the documentation of this file](ns__usb__tests_8c.md)

```C++

#include "ns_usb.h"
#include "unity/unity.h"
#include "ns_core.h"

 uint8_t rx_buf[4096];
 uint8_t tx_buf[4096];
 ns_usb_config_t cfg;
 usb_handle_t handle = NULL;

 void reset_usb_cfg() {
    cfg.api = &ns_usb_V1_0_0;
    cfg.deviceType = NS_USB_VENDOR_DEVICE;
    cfg.rx_buffer = &rx_buf;
    cfg.rx_bufferLength = 4096;
    cfg.tx_buffer = &tx_buf;
    cfg.tx_bufferLength = 4096;
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
    uint32_t status = ns_usb_init(&cfg, &handle);
    TEST_ASSERT_EQUAL(NS_STATUS_SUCCESS, status);
}

// Null config should fail
void ns_usb_test_init_null_config() {
    uint32_t status = ns_usb_init(NULL, &handle);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_HANDLE, status);
}

// Null rx and tx buffer should fail
void ns_usb_test_init_null_buffers() {
    reset_usb_cfg();
    cfg.rx_buffer = NULL;
    cfg.tx_buffer = NULL;
    uint32_t status = ns_usb_init(&cfg, &handle);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_CONFIG, status);
}

// Invalid API
void ns_usb_test_init_invalid_api() {
    reset_usb_cfg();
    ns_core_api_t invalid_api = {.apiId = 0xFFFFF, .version = NS_USB_V0_0_1};
    cfg.api = &invalid_api;
    uint32_t status = ns_usb_init(&cfg, &handle);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_VERSION, status);
}



```

