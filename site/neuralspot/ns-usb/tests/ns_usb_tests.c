#include "tusb.h"
#include "ns_usb.h"
#include "unity/unity.h"
#include "ns_core.h"
#include <string.h>
#define MY_RX_BUFSIZE 4096
#define MY_TX_BUFSIZE 4096

static uint8_t my_rx_ff_buf[MY_RX_BUFSIZE] __attribute__((aligned(16)));
static uint8_t my_tx_ff_buf[MY_TX_BUFSIZE] __attribute__((aligned(16)));
uint8_t rx_buf[4096];
uint8_t tx_buf[4096];
ns_usb_config_t cfg;
usb_handle_t handle = NULL;

ns_timer_config_t basic_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};

typedef struct usb_data {
    // USB CRC test struct -- first 4 member variables are 32-bit integers for easier alignment on javascript side.
    uint32_t data_type; // 1 or 2 for message vs raw data
    uint32_t data_size; // in bytes
    uint32_t second_crc; // crc that combines test data and first crc
    uint32_t first_crc; // first crc of test data
    uint8_t data[];
} usb_data_t;

static uint32_t received_second_crc = 0;
static bool crc_verified = false;
static ns_tusb_desc_webusb_url_t usb_test_url;

void crc_verification_raw_cb(const uint8_t *data, uint32_t length, void *param) {
    // Extract the packet
    memcpy(&received_second_crc, data + 6, sizeof(uint32_t)); // add 6 to account for padding
    crc_verified = true;

}

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
    cfg.desc_url = &usb_test_url;
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

void ns_usb_test_crc() {
    uint32_t calculated_second_crc;
    usb_data_t packet;
    ns_interrupt_master_enable();
    ns_timer_init(&basic_tickTimer);
    webusb_register_raw_cb(crc_verification_raw_cb, NULL);
    reset_usb_cfg();
    strcpy(usb_test_url.url, "ambiqai.github.io/web-ble-dashboards/vision_demo/");
    usb_test_url.bDescriptorType = 3;
    usb_test_url.bScheme = 1;
    usb_test_url.bLength = 3 + sizeof(usb_test_url.url) - 1;
    cfg.rx_buffer = my_rx_ff_buf;
    cfg.rx_bufferLength = MY_RX_BUFSIZE;
    cfg.tx_buffer = my_tx_ff_buf;
    cfg.tx_bufferLength = MY_TX_BUFSIZE;
    uint32_t status = ns_usb_init(&cfg, &handle);
    TEST_ASSERT_EQUAL(NS_STATUS_SUCCESS, status);

    // Test data
    for(int i = 0; i < 20; i++) {
        uint8_t test_data[480];
        // Different variations of test data
        for (int j = 0; j < 300; j++) {
            test_data[j] = (uint8_t)((i + j) % 256);
        }
        // calculate size of test data
        uint8_t chunk_size = sizeof(test_data) - 1; // exlude null terminator
        
        // calculate CRC of test data
        uint32_t crc = CalcCrc32(0xFFFFFFFF, chunk_size, (uint8_t *)test_data);

        // fill packet struct
        memcpy(packet.data, (uint8_t*)test_data, chunk_size);
        packet.data_size = chunk_size;
        packet.first_crc = crc;
        packet.second_crc  = 0; // initialize to 0
        packet.data_type = 0x02; // raw message
        size_t packet_size = chunk_size + 4 * sizeof(uint32_t); 

        // ns_lp_printf("Sending %d bytes\n", packet_size);

        //send packet
        webusb_send_data((usb_data_t *)&packet, packet_size);
        tud_vendor_write_flush();
        // calculate second crc to see if it matches what is sent back by the browser
        calculated_second_crc = CalcCrc32(0xFFFFFFFF, sizeof(packet.first_crc) + chunk_size, (uint8_t *)&packet.first_crc);

        ns_delay_us(200000);

        // ns_lp_printf("calculated crc: %u", calculated_second_crc);

        // only check if the crc was actually copied in the cb function
        if(crc_verified) {
            TEST_ASSERT_EQUAL(received_second_crc, calculated_second_crc);
        }
        
    }


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

