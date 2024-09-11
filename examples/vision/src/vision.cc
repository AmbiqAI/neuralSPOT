/**
 * @file main.cc
 * @author Adam Page (adam.page@ambiq.com)
 * @brief
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
// #include "ns_rpc_generic_data.h"
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

typedef struct usb_data {
    // Jpeg Image
    uint8_t descriptor;
    uint8_t buffer[512 - 1]; // assume 66% compression (so no x3 for RGB)
} usb_data_t;

static app_state_e state = IDLE_STATE;
static float32_t modelResults[NUM_CLASSES] = {0};
static int modelResult = 0;
static bool usbAvailable = false;
static int volatile sensorCollectBtnPressed = false;
static int volatile clientCollectBtnPressed = false;
static data_collect_mode_e collectMode = SENSOR_DATA_COLLECT;
static uint8_t camBuffer[CAM_BUFF_SIZE];
static AM_SHARED_RW uint8_t usbXmitBuffer[CAM_BUFF_SIZE];
extern img_t *imgBuffer;

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

ns_button_config_t button_config = {
    .api = &ns_button_V1_0_0,
    .button_0_enable = true,
    .button_1_enable = true,
    .button_0_flag = &sensorCollectBtnPressed,
    .button_1_flag = &clientCollectBtnPressed};

// Handle TinyUSB events
// void
// tud_mount_cb(void) {
//     usbAvailable = true;
// }
// void
// tud_resume_cb(void) {
//     usbAvailable = true;
// }
// void
// tud_umount_cb(void) {
//     usbAvailable = false;
// }
// void
// tud_suspend_cb(bool remote_wakeup_en) {
//     usbAvailable = false;
// }

void trigger_camera() {
    if (collectMode == CLIENT_DATA_COLLECT) {
        // Nothing to do
    } else {
        trigger_camera_capture();
    }
}

uint32_t toc(); //

void perform_capture() {
    // if (collectMode == CLIENT_DATA_COLLECT) {
    //     img_t *imgPtr = &imgBuffer[0];
    //     uint32_t numSamples;
    //     for (size_t c = 0; c < IMG_NUM_CH; c++) {
    //         for (size_t y = 0; y < IMG_HEIGHT; y++) {
    //             numSamples = fetch_image_from_pc(imgPtr, IMG_WIDTH);
    //             imgPtr += numSamples;
    //         }
    //     }
    // } else {
    while (is_camera_capturing()) {
        ns_delay_us(50000);
    }
    ns_lp_printf("Toc 2 = %u\n", toc());
    uint32_t camLength = camera_capture(camBuffer, CAM_BUFF_SIZE);
    memcpy(usbXmitBuffer, camBuffer, camLength);

    // // To debug, just print first 20 bytes
    // for (int i = 00; i < 20; i++) {
    //     ns_lp_printf("0x%02x ", camBuffer[i]);
    //     // if (i % 16 == 0) {
    //     //     ns_lp_printf("\n");
    //     // }
    // }
    // for (int i = 00; i < 20; i++) {
    //     ns_lp_printf("0xx%02x ", usbXmitBuffer[i]);
    //     // if (i % 16 == 0) {
    //     //     ns_lp_printf("\n");
    //     // }
    // }
    // ns_lp_printf("\n");

    // Max xfer is 512, so we have to chunk anything bigger
    int remaining = camLength;
    int offset = 0;
    while (remaining > 0) {
        usb_data_t data;
        if (offset == 0) {
            ns_lp_printf("First chunk\n");
            data.descriptor = 0;
        } else {
            data.descriptor = MIDDLE_CHUNK;
        };
        int chunkSize = remaining > 511 ? 511 : remaining;
        memcpy(data.buffer, &usbXmitBuffer[offset], chunkSize);
        remaining -= chunkSize;
        offset += chunkSize;
        if (remaining == 0) {
            data.descriptor = LAST_CHUNK;
        }

        webusb_send_data((uint8_t *)&data, chunkSize + 1);
        while (tud_vendor_write_available() < 512) {
            ns_delay_us(200);
        }
    }
    tud_vendor_write_flush();

    // int rem = webusb_send_data((uint8_t *)&usbXmitBuffer, camLength);
    // ns_lp_printf("Sent %d bytes\n", rem);

    // Send via webusb

    // ns_lp_printf("Toc 3 = %u\n", toc());
    // camera_decode_image(camBuffer, camLength, imgBuffer, IMG_WIDTH, IMG_HEIGHT, IMG_SCALE);
    // ns_lp_printf("Toc 4 = %u\n", toc());
    // img_t *imgPtr = &imgBuffer[0];
    // for (size_t c = 0; c < IMG_NUM_CH; c++) {
    //     for (size_t y = 0; y < IMG_HEIGHT; y++) {
    //         send_image_to_pc(imgPtr, IMG_WIDTH);
    //         imgPtr += IMG_WIDTH;
    //     }
    // }
    // }
}

void preprocess_image() {
    /**
     * @brief Preprocess image
     *
     */
    if (collectMode == CLIENT_DATA_COLLECT) {
        // Just need to normalize
    } else {
    }
}

void wakeup() {
    am_bsp_itm_printf_enable();
    am_bsp_debug_printf_enable();
    ns_delay_us(50);
}

void deepsleep() {
    am_bsp_itm_printf_disable();
    am_bsp_debug_printf_disable();
    am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
}

void tic() { elapsedTime = ns_us_ticker_read(&tickTimer); }

uint32_t toc() { return ns_us_ticker_read(&tickTimer) - elapsedTime; }

// void sendMessage(
//     uint8_t type, uint8_t classId, uint8_t confidence, uint8_t fps, uint8_t joulesEstimate,
//     uint8_t cpuUtilization) {
//     usb_data_t data = {
//         .type = type,
//         .length = sizeof(usb_data_t),
//         .classId = classId,
//         .confidence = confidence,
//         .fps = fps,
//         .joulesEstimate = joulesEstimate,
//         .cpuUtilization = cpuUtilization};

//     // Send the data to the WebUSB client as an array of bytes
//     webusb_send_data((uint8_t *)&data, sizeof(usb_data_t));
// }

void msgReceived(const uint8_t *buffer, uint32_t length, void *args) {
    // Do something with the received message
    ns_lp_printf("Received %d bytes: %s\n", length, buffer);
}

int main(void) {
    int err;
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    usb_handle_t usb_handle = NULL;

    // Power configuration (mem, cache, peripherals, clock)
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");
    ns_power_config(&ns_pwr_config);
    ns_itm_printf_enable();
    ns_interrupt_master_enable();

    elapsedTime = 0;
    ns_timer_init(&tickTimer);
    ns_peripheral_button_init(&button_config);

    // WebUSB Setup
    webusb_register_msg_cb(msgReceived, NULL);
    webUsbConfig.rx_buffer = my_rx_ff_buf;
    webUsbConfig.rx_bufferLength = MY_RX_BUFSIZE;
    webUsbConfig.tx_buffer = my_tx_ff_buf;
    webUsbConfig.tx_bufferLength = MY_TX_BUFSIZE;
    NS_TRY(ns_usb_init(&webUsbConfig, &usb_handle), "USB Init Failed\n");
    ns_lp_printf("USB Init Success\n");

    // Camera Setup
    am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_IOM1);
    am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_IOM3);
    err = init_camera();
    stop_camera();
    // init_model();

    ns_printf("📸 TinyVision Demo\n\n");
    ns_printf("Please select data collection options:\n\n\t1. BTN1=camera\n\t2. "
              "BTN2=client\n");

    // Application Loop
    while (1) {
        switch (state) {
        case IDLE_STATE:
            if (sensorCollectBtnPressed | clientCollectBtnPressed) {
                collectMode = sensorCollectBtnPressed ? SENSOR_DATA_COLLECT : CLIENT_DATA_COLLECT;
                sensorCollectBtnPressed = false;
                clientCollectBtnPressed = false;
                // wakeup();
                state = START_STATE;
            } else {
                // ns_lp_printf("IDLE_STATE\n");
                // ns_deep_sleep();
                // deepsleep();
            }
            break;

        case START_STATE:
            // print_to_pc("START_STATE\n");
            ns_lp_printf("Start\n");
            start_camera();
            ns_delay_us(10000);
            trigger_camera();
            sensorCollectBtnPressed = false;
            clientCollectBtnPressed = false;
            state = CAPTURE_STATE;
            break;

        case CAPTURE_STATE:
            // print_to_pc("CAPTURE_STATE\n");
            // ns_lp_printf("Capture\n");
            tic();
            perform_capture();
            trigger_camera();
            // ns_printf("Capture took %02u\n", toc());
            // state = DISPLAY_STATE;
            state = CAPTURE_STATE;
            // state = IDLE_STATE;
            break;

        case PREPROCESS_STATE:
            // print_to_pc("PREPROCESS_STATE\n");
            am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE);
            preprocess_image();
            state = CAPTURE_STATE;
            // state = INFERENCE_STATE;
            break;

        case INFERENCE_STATE:
            // print_to_pc("INFERENCE_STATE\n");
            ns_lp_printf("Inference\n");
            tic();
            modelResult = model_inference(modelResults);
            ns_printf("Inference took %02u\n", toc());
            state = modelResult == -1 ? FAIL_STATE : DISPLAY_STATE;
            break;

        case DISPLAY_STATE:
            // print_to_pc("DISPLAY_STATE\n");
            ns_printf("Result=%d\n", modelResult);
            // send_results_to_pc(modelResults, NUM_CLASSES);
            ns_delay_us(1000000);
            am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_LOW_POWER);
            if (sensorCollectBtnPressed | clientCollectBtnPressed) {
                state = STOP_STATE;
            } else {
                state = CAPTURE_STATE;
            }
            break;

        case FAIL_STATE:
            // print_to_pc("FAIL_STATE\n");
            state = STOP_STATE;
            break;

        case STOP_STATE:
            // print_to_pc("STOP_STATE\n");
            stop_camera();
            sensorCollectBtnPressed = false;
            clientCollectBtnPressed = false;
            state = IDLE_STATE;
            break;

        default:
            state = IDLE_STATE;
            break;
        }
    }
}

// void
// setup() {
//     /**
//      * @brief Application setup
//      *
//      */
//     int err;
//     ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};

//     // Power configuration (mem, cache, peripherals, clock)
//     NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");

//     ns_power_config(&ns_pwr_config);
//     ns_itm_printf_enable();

//     am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_IOM1);
//     am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_IOM3);
//     // Enable Interrupts
//     am_hal_interrupt_master_enable();
//     // Enable SWO/USB
//     wakeup();
//     // Initialize blocks
//     elapsedTime = 0;
//     ns_timer_init(&tickTimer);
//     // init_rpc();
//     err = init_camera();
//     stop_camera();
//     init_model();
//     ns_peripheral_button_init(&button_config);
//     ns_printf("📸 TinyVision Demo\n\n");
//     ns_printf("Please select data collection options:\n\n\t1. BTN1=camera\n\t2. "
//               "BTN2=client\n");
// }

// void
// loop() {
//     switch (state) {
//     case IDLE_STATE:
//         if (sensorCollectBtnPressed | clientCollectBtnPressed) {
//             collectMode = sensorCollectBtnPressed ? SENSOR_DATA_COLLECT : CLIENT_DATA_COLLECT;
//             wakeup();
//             state = START_STATE;
//         } else {
//             ns_printf("IDLE_STATE\n");
//             deepsleep();
//         }
//         break;

//     case START_STATE:
//         // print_to_pc("START_STATE\n");
//         start_camera();
//         ns_delay_us(10000);
//         trigger_camera();
//         sensorCollectBtnPressed = false;
//         clientCollectBtnPressed = false;
//         state = CAPTURE_STATE;
//         break;

//     case CAPTURE_STATE:
//         // print_to_pc("CAPTURE_STATE\n");
//         tic();
//         perform_capture();
//         trigger_camera();
//         ns_lp_printf("Capture took %02u\n", toc());
//         // state = DISPLAY_STATE;
//         state = PREPROCESS_STATE;
//         break;

//     case PREPROCESS_STATE:
//         // print_to_pc("PREPROCESS_STATE\n");
//         am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE);
//         preprocess_image();
//         state = CAPTURE_STATE;
//         // state = INFERENCE_STATE;
//         break;

//     case INFERENCE_STATE:
//         // print_to_pc("INFERENCE_STATE\n");
//         tic();
//         modelResult = model_inference(modelResults);
//         ns_printf("Inference took %02u\n", toc());
//         state = modelResult == -1 ? FAIL_STATE : DISPLAY_STATE;
//         break;

//     case DISPLAY_STATE:
//         // print_to_pc("DISPLAY_STATE\n");
//         ns_printf("Result=%d\n", modelResult);
//         // send_results_to_pc(modelResults, NUM_CLASSES);
//         ns_delay_us(1000000);
//         am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_LOW_POWER);
//         if (sensorCollectBtnPressed | clientCollectBtnPressed) {
//             state = STOP_STATE;
//         } else {
//             state = CAPTURE_STATE;
//         }
//         break;

//     case FAIL_STATE:
//         // print_to_pc("FAIL_STATE\n");
//         state = STOP_STATE;
//         break;

//     case STOP_STATE:
//         // print_to_pc("STOP_STATE\n");
//         stop_camera();
//         sensorCollectBtnPressed = false;
//         clientCollectBtnPressed = false;
//         state = IDLE_STATE;
//         break;

//     default:
//         state = IDLE_STATE;
//         break;
//     }
// }

// int
// main(void) {
//     /**
//      * @brief Main function
//      * @return int
//      */
//     setup();
//     while (1) {
//         loop();
//     }
// }
