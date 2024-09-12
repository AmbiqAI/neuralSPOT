/**
 * @file Vision.cc
 * @author Ambiq
 * @brief This demo is intended to work with the vision WebBLE client
 * See README.md for more details
 * @version 0.1
 * @date 2022-11-09
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "arm_math.h"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
// neuralSPOT
#include "ns_ambiqsuite_harness.h"
#include "ns_debug_log.h"
#include "ns_energy_monitor.h"
#include "ns_malloc.h"
#include "ns_peripherals_button.h"
#include "ns_peripherals_power.h"
#include "ns_timer.h"
#include "ns_usb.h"
#include "ns_core.h"
#include "tusb.h"

// Locals
#include "ns_camera.h"
#include "vision.h"
#include "model.h"

// #if (configAPPLICATION_ALLOCATED_HEAP == 1)
// // RPC uses malloc internally, so we need to declare it here
// uint8_t ucHeap[NS_RPC_MALLOC_SIZE_IN_K * 1024] __attribute__((aligned(4)));
// #endif

#define MY_RX_BUFSIZE 4096
#define MY_TX_BUFSIZE 4096
static uint8_t my_rx_ff_buf[MY_RX_BUFSIZE];
static uint8_t my_tx_ff_buf[MY_TX_BUFSIZE];
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
    .service_cb = NULL};

// USB Data sent to WebUSB client
typedef enum {
    FIRST_CHUNK = 0x0,
    MIDDLE_CHUNK = 0x1,
    LAST_CHUNK = 0x2,
} usb_data_descriptor_e;

#define MAX_WEBUSB_FRAME 512
#define WEBUSB_HEADER_SIZE 2
#define MAX_WEBUSB_CHUNK (MAX_WEBUSB_FRAME - WEBUSB_HEADER_SIZE)

typedef struct usb_data {
    // Jpeg Image
    uint8_t descriptor;
    uint8_t mode; // 0 RGB, 1 JPG
    uint8_t buffer[MAX_WEBUSB_FRAME];
} usb_data_t;

// static app_state_e state = IDLE_STATE;
// static float32_t modelResults[NUM_CLASSES] = {0};
// static int modelResult = 0;
// static bool usbAvailable = false;
// static int volatile sensorCollectBtnPressed = false;
// static int volatile clientCollectBtnPressed = false;
// static data_collect_mode_e collectMode = SENSOR_DATA_COLLECT;
static uint8_t camBuffer[CAM_BUFF_SIZE];
// static AM_SHARED_RW uint8_t usbXmitBuffer[CAM_BUFF_SIZE];
// extern img_t *imgBuffer;

const ns_power_config_t ns_pwr_config = {
    .api = &ns_power_V1_0_0,
    .eAIPowerMode = NS_MINIMUM_PERF,
    .bNeedAudAdc = false,
    .bNeedSharedSRAM = true,
    .bNeedCrypto = true,
    .bNeedBluetooth = false,
    .bNeedUSB = true,
    .bNeedIOM = true,
    .bNeedAlternativeUART = false,
    .b128kTCM = false};

static uint32_t elapsedTime = 0;
ns_timer_config_t tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};

// ns_button_config_t button_config = {
//     .api = &ns_button_V1_0_0,
//     .button_0_enable = true,
//     .button_1_enable = true,
//     .button_0_flag = &sensorCollectBtnPressed,
//     .button_1_flag = &clientCollectBtnPressed};

ns_camera_config_t camera_config = {
    .api = &ns_camera_V1_0_0,
    .spiSpeed = AM_HAL_IOM_8MHZ,
    .cameraHw = NS_ARDUCAM,
    .imageMode = CAM_IMAGE_MODE,
    .imagePixFmt = NS_CAM_IMAGE_PIX_FMT_JPEG,
    // .imagePixFmt = NS_CAM_IMAGE_PIX_FMT_RGB565,
    .spiConfig = {.iom = 1},
};

void tic() { elapsedTime = ns_us_ticker_read(&tickTimer); }
uint32_t toc() { return ns_us_ticker_read(&tickTimer) - elapsedTime; }

static uint32_t bufferOffset = 0;

static void perform_capture() {
    uint32_t camLength =
        ns_transfer_picture(&camera_config, camBuffer, &bufferOffset, CAM_BUFF_SIZE);

    // Max xfer is 512, so we have to chunk anything bigger
    int remaining = camLength;
    // int offset = 0;
    int offset = bufferOffset;
    while (remaining > 0) {
        usb_data_t data;
        if (offset == 0) {
            // ns_lp_printf("First chunk\n");
            data.descriptor = FIRST_CHUNK;
        } else {
            data.descriptor = MIDDLE_CHUNK;
        };

        data.mode = camera_config.imagePixFmt == NS_CAM_IMAGE_PIX_FMT_JPEG ? 1 : 0;

        int chunkSize = remaining > MAX_WEBUSB_CHUNK ? MAX_WEBUSB_CHUNK : remaining;
        memcpy(data.buffer, &camBuffer[offset], chunkSize);
        remaining -= chunkSize;
        offset += chunkSize;
        if (remaining == 0) {
            data.descriptor = LAST_CHUNK;
        }

        webusb_send_data((uint8_t *)&data, chunkSize + WEBUSB_HEADER_SIZE);
        while (tud_vendor_write_available() < MAX_WEBUSB_FRAME) {
            ns_delay_us(200);
        }
    }
    tud_vendor_write_flush();
}

void msgReceived(const uint8_t *buffer, uint32_t length, void *args) {
    // Do something with the received message
    ns_lp_printf("Received %d bytes: %s\n", length, buffer);
}

int main(void) {
    // int err;
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    usb_handle_t usb_handle = NULL;

    // Power configuration (mem, cache, peripherals, clock)
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");
    NS_TRY(ns_power_config(&ns_pwr_config), "Power config failed.\n");
    ns_itm_printf_enable();
    ns_interrupt_master_enable();

    elapsedTime = 0;
    NS_TRY(ns_timer_init(&tickTimer), "Timer Init Failed\n");
    // NS_TRY(ns_peripheral_button_init(&button_config), "Button Init Failed\n");

    // WebUSB Setup
    webusb_register_msg_cb(msgReceived, NULL);
    webUsbConfig.rx_buffer = my_rx_ff_buf;
    webUsbConfig.rx_bufferLength = MY_RX_BUFSIZE;
    webUsbConfig.tx_buffer = my_tx_ff_buf;
    webUsbConfig.tx_bufferLength = MY_TX_BUFSIZE;
    NS_TRY(ns_usb_init(&webUsbConfig, &usb_handle), "USB Init Failed\n");
    ns_lp_printf("USB Init Success\n");

    // Camera Setup
    NS_TRY(ns_camera_init(&camera_config), "Camera Stop Failed\n");
    ns_lp_printf("Camera Init Success\n");
    ns_stop_camera(&camera_config);

    ns_lp_printf("📸 TinyVision Demo\n\n");

    // Send camera images to webusb clients
    ns_start_camera(&camera_config);
    ns_delay_us(10000);
    ns_take_picture(&camera_config);
    while (1) {
        perform_capture();
        ns_take_picture(&camera_config);
        ns_deep_sleep();
    }
}
