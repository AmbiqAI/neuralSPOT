/**
 * @file ic.cc
 * @author Carlos Morales
 * @brief This simple demo classifies images included in examples.h using the
 * the open source MLPerf Tiny Resnet8 - it then sends the result to an accompanying
 * WebUSB client to display the result.
 *
 * @version 0.1
 * @date 2024-03-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "ns_core.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_energy_monitor.h"
#include "ns_peripherals_power.h"
#include "ns_usb.h"

#define MY_RX_BUFSIZE 4096
#define MY_TX_BUFSIZE 4096
uint8_t my_rx_ff_buf[MY_RX_BUFSIZE];
uint8_t my_tx_ff_buf[MY_TX_BUFSIZE];

static ns_usb_config_t webUsbConfig = {
    .api = &ns_usb_V1_0_0,
    .deviceType = NS_USB_VENDOR_DEVICE,
    // .deviceType = NS_USB_CDC_DEVICE,
    .rx_buffer = NULL,
    .rx_bufferLength = 0,
    .tx_buffer = NULL,
    .tx_bufferLength = 0,
    .rx_cb = NULL,
    .tx_cb = NULL,
    .service_cb = NULL,
    .desc_url = {0}};

// WebUSB URL
static ns_tusb_desc_webusb_url_t ic_url;

// Model stuff
#include "ic_bench_api.h"
#include "ic_bench_example_tensors.h"
#include "examples.h"
#include "ns_model.h"

const char *kCategoryLabels[11] = {"airplane", "automobile", "bird", "cat",   "deer",   "dog",
                                   "frog",     "horse",      "ship", "truck", "unknown"};

// TFLM Config
static ns_model_state_t model;
volatile int example_status = 0; // Prevent the compiler from optimizing out while loops

ns_timer_config_t basic_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};

// USB Data sent to WebUSB client
typedef struct usb_data {
    uint8_t type;
    uint8_t length;
    uint8_t classId;
    uint8_t confidence;
    uint8_t fps;
    uint8_t joulesEstimate;
    uint8_t cpuUtilization;
} usb_data_t;

void sendMessage(
    uint8_t type, uint8_t classId, uint8_t confidence, uint8_t fps, uint8_t joulesEstimate,
    uint8_t cpuUtilization) {
    usb_data_t data = {
        .type = type,
        .length = sizeof(usb_data_t),
        .classId = classId,
        .confidence = confidence,
        .fps = fps,
        .joulesEstimate = joulesEstimate,
        .cpuUtilization = cpuUtilization};

    // Send the data to the WebUSB client as an array of bytes
    webusb_send_data((uint8_t *)&data, sizeof(usb_data_t));
}

void msgReceived(const uint8_t *buffer, uint32_t length, void *args) {
    // Do something with the received message
    ns_lp_printf("Received %d bytes: %s\n", length, buffer);
}

int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    usb_handle_t usb_handle = NULL;
    char *source;
    int8_t max, label, j;
    j = 0;
    int invokes = 0;
    uint32_t oldTime = ns_us_ticker_read(&basic_tickTimer);
    uint32_t newTime = oldTime;
    float ips = 0;


    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed.\n");
    NS_TRY(ns_set_performance_mode(NS_MAXIMUM_PERF), "Set CPU Perf mode failed.");
    ns_itm_printf_enable();
    ns_interrupt_master_enable();

    NS_TRY(ns_timer_init(&basic_tickTimer), "Timer init failed.\n");

    webusb_register_msg_cb(msgReceived, NULL);

    // instantiate the URL descriptor
    strcpy(ic_url.url, "ambiqai.github.io/web-ble-dashboards/ic_demo/");
    ic_url.bDescriptorType = 3;
    ic_url.bScheme = 1;
    ic_url.bLength = 3 + sizeof(ic_url.url) - 1;

    // Initialize USB
    webUsbConfig.rx_buffer = my_rx_ff_buf;
    webUsbConfig.rx_bufferLength = MY_RX_BUFSIZE;
    webUsbConfig.tx_buffer = my_tx_ff_buf;
    webUsbConfig.tx_bufferLength = MY_TX_BUFSIZE;
    webUsbConfig.desc_url = ic_url;
    NS_TRY(ns_usb_init(&webUsbConfig, &usb_handle), "USB Init Failed\n");
    ns_lp_printf("USB Init Success\n");

    // Initialize the model, get handle if successful
    int status = ic_bench_minimal_init(&model); // model init with minimal defaults
    if (status == ic_bench_STATUS_FAILURE) {
        ns_lp_printf("Model init failed... status %d\n", status);
        while (1)
            example_status = ic_bench_STATUS_INIT_FAILED; // hang
    }

    // Loop through 100 images
    while (1) {
        source = (char *)&(images[j][0]);
        memcpy(model.model_input[0]->data.int8, source, model.model_input[0]->bytes);

        example_status = model.interpreter->Invoke();
        invokes++;
        newTime = ns_us_ticker_read(&basic_tickTimer);
        if (newTime - oldTime > 1000000) {
            ips = (float)invokes / ((float)(newTime - oldTime) / 1000000.0f);
            invokes = 0;
            oldTime = newTime;
        }

        // Parse the output tensor to find max value
        max = -127;
        label = 0;
        for (uint8_t i = 0; i < model.model_output[0]->bytes; i++) {
            if (model.model_output[0]->data.int8[i] > max) {
                max = model.model_output[0]->data.int8[i];
                label = i;
            }
        }

        // Send the result to WebUSB client
        uint8_t confidence = (uint8_t)((max + 127) * 100 / 255);

        // convert ips to uint8_t
        uint8_t fps = ips;
        sendMessage(0, label, confidence, fps, 0, 0);
        // ns_lp_printf("%d - Label is %d (%s). FPS = %0.2f\n", j, label, kCategoryLabels[label],
        // ips);

        // Wrap at 100
        j = (j + 1) % 100;
    }
}
