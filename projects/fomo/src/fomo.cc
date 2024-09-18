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

// #include "arm_math.h"
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
#include "ns_camera.h"
// #include "ArducamCamera.h"

// EdgeImpulse Includes
#include "edge-impulse/edge-impulse-sdk/porting/ei_classifier_porting.h"
#include "edge-impulse/edge-impulse-sdk/classifier/ei_run_classifier.h"
// #include "edge-impulse/ingestion-sdk-platform/sensor/ei_analog_mic.h"
#include "edge-impulse/model/model-parameters/model_metadata.h"
#include "edge-impulse/model/model-parameters/model_variables.h"

// Include the model

// Locals
#include "vision.h"

// #define GRAYSCALE
// extern ArducamCamera camera; // Arducam driver assumes this is a global, so :shrug:

#if (configAPPLICATION_ALLOCATED_HEAP == 1)
size_t ucHeapSize = (NS_MALLOC_HEAP_SIZE_IN_K + 5) * 1024;
uint8_t ucHeap[(NS_MALLOC_HEAP_SIZE_IN_K + 5) * 1024] __attribute__((aligned(4)));
#endif

#define MY_RX_BUFSIZE 4096
#define MY_TX_BUFSIZE 4096
static uint8_t my_rx_ff_buf[MY_RX_BUFSIZE];
static uint8_t my_tx_ff_buf[MY_TX_BUFSIZE];
static ns_usb_config_t webUsbConfig = {
    .api = &ns_usb_V1_0_0,
    .deviceType = NS_USB_VENDOR_DEVICE,
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
#define MAX_RESULTS 15

typedef struct usb_data {
    // Jpeg Image
    uint8_t descriptor;
    uint8_t mode; // 0 RGB, 1 JPG
    uint8_t buffer[MAX_WEBUSB_FRAME];
} usb_data_t;

typedef struct {
    char id[20]; // 20 chars max
    uint8_t x;
    uint8_t y;
    uint8_t confidence;  
} result_t;

typedef struct usb_metadata {
    // Jpeg Image
    uint8_t descriptor; // 0/1/2 for first/middle/last chunk, 3 for metadata
    uint8_t cpu_load;
    uint8_t inference_latency;
    uint8_t num_results;
    result_t results[MAX_RESULTS];
} usb_metadata_t;


static int volatile sensorCollectBtnPressed = false;
static int volatile clientCollectBtnPressed = false;
static AM_SHARED_RW uint8_t camBuffer[CAM_BUFF_SIZE];
// static uint8_t grayScaleBuffer[CAM_CH_SIZE];

const ns_power_config_t ns_pwr_config = {
    .api = &ns_power_V1_0_0,
    .eAIPowerMode = NS_MAXIMUM_PERF,
    .bNeedAudAdc = false,
    .bNeedSharedSRAM = true,
    .bNeedCrypto = true,
    .bNeedBluetooth = false,
    .bNeedUSB = true,
    .bNeedIOM = true,
    .bNeedAlternativeUART = false,
    .b128kTCM = false};

static uint32_t elapsedTime = 0;
ns_timer_config_t ei_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};

// ns_timer_config_t ei_tickTimer;

// ns_button_config_t button_config = {
//     .api = &ns_button_V1_0_0,
//     .button_0_enable = true,
//     .button_1_enable = true,
//     .button_0_flag = &sensorCollectBtnPressed,
//     .button_1_flag = &clientCollectBtnPressed};

// DMA Callback
volatile bool dmaComplete = false;
volatile bool pictureTaken = false;
// uint32_t bufferOffset = 0;
uint32_t buffer_length = 0;
static uint32_t bufferOffset = 0;

void picture_dma_complete(ns_camera_config_t *cfg) {
    // ns_lp_printf("DMA Complete CB\n");
    dmaComplete = true;
}

void picture_taken_complete(ns_camera_config_t *cfg) {
    pictureTaken = true;
    // ns_lp_printf("Picture taken CB\n");
}

ns_camera_config_t camera_config = {
    .api = &ns_camera_V1_0_0,
    .spiSpeed = AM_HAL_IOM_8MHZ,
    .cameraHw = NS_ARDUCAM,
    .imageMode = CAM_IMAGE_MODE,
    // .imagePixFmt = NS_CAM_IMAGE_PIX_FMT_JPEG,
    .imagePixFmt = NS_CAM_IMAGE_PIX_FMT_RGB565,
    .spiConfig = {.iom = 1},
    .dmaCompleteCb = picture_dma_complete,
    .pictureTakenCb = picture_taken_complete,
};

void tic() { elapsedTime = ns_us_ticker_read(&ei_tickTimer); }
uint32_t toc() { return ns_us_ticker_read(&ei_tickTimer) - elapsedTime; }

static uint32_t start_dma() {
    uint32_t camLength = ns_start_dma_read(&camera_config, camBuffer, &bufferOffset, CAM_BUFF_SIZE);
    return camLength;
}

uint32_t chop_off_trailing_zeros(uint32_t length) {
    uint32_t index;
    for (index = length - 1; index >= 0; index--) {
        if (camBuffer[index] != 0) {
            break;
        }
    }
    return index + 1;
}

static void render_image(uint32_t camLength, ei_impulse_result_t *inf) {
    // Max xfer is 512, so we have to chunk anything bigger
    int remaining = camLength;
    // int offset = 0;
    int offset = bufferOffset;
    // ns_lp_printf("Rendering image len = %d\n",camLength);

    while (remaining > 0) {
        usb_data_t data;
        if (offset == 0) {
            // ns_lp_printf("First chunk\n");
            data.descriptor = FIRST_CHUNK;
        } else {
            // ns_lp_printf("Middle chunk\n");
            data.descriptor = MIDDLE_CHUNK;
        };

        data.mode = camera_config.imagePixFmt == NS_CAM_IMAGE_PIX_FMT_JPEG ? 1 : 0;

        int chunkSize = remaining > MAX_WEBUSB_CHUNK ? MAX_WEBUSB_CHUNK : remaining;
        memcpy(data.buffer, &camBuffer[offset], chunkSize);
        remaining -= chunkSize;
        offset += chunkSize;
        if (remaining == 0) {
            // ns_lp_printf("Last chunk\n");
            data.descriptor = LAST_CHUNK;
        }

        webusb_send_data((uint8_t *)&data, chunkSize + WEBUSB_HEADER_SIZE);
        while (tud_vendor_write_available() < MAX_WEBUSB_FRAME) {
            ns_delay_us(200);
        }
    }
    tud_vendor_write_flush();

    // Send metadata
    usb_metadata_t metadata;
    metadata.descriptor = 3;
    metadata.cpu_load = 0;
    metadata.inference_latency = inf->timing.classification;
    metadata.num_results = inf->bounding_boxes_count;
    int result_index = 0;

    for (size_t ix = 0; ix < inf->bounding_boxes_count; ix++) {
        auto bb = inf->bounding_boxes[ix];
        if (bb.value < 0.6f) {
            continue;
        }
        else {
            ns_lp_printf("bb.value %f\n", bb.value);
        }
        
        strncpy(metadata.results[result_index].id, bb.label, 20);
        // metadata.id[ix] = 0;
        metadata.results[result_index].x = bb.x;
        metadata.results[result_index].y = bb.y;
        metadata.results[result_index].confidence = bb.value * 100;
        result_index++;
    }
    metadata.num_results = result_index;
    ns_lp_printf("Meta number of results: %d, descriptor %d\n", metadata.num_results, metadata.descriptor);
    // for (size_t ix = 0; ix < metadata.num_results; ix++) {
    //     ns_lp_printf("Meta: %s (%d) [ x: %d, y: %d ]\n", metadata.id[ix], metadata.confidence[ix], metadata.x[ix], metadata.y[ix]);
    // }
    webusb_send_data((uint8_t *)&metadata, sizeof(usb_metadata_t));
    while (tud_vendor_write_available() < MAX_WEBUSB_FRAME) {
        ns_delay_us(200);
    }
    tud_vendor_write_flush();

}

// static uint32_t perform_capture() {
//     uint32_t camLength =
//         my_transfer_picture(&camera_config, camBuffer, &bufferOffset, CAM_BUFF_SIZE);

//     #ifdef GRAYSCALE
//     // Create grayscale image and put in grayScaleBuffer
//     for (int i = 0; i < CAM_CH_SIZE; i++) {
//         uint16_t pixel = (camBuffer[i * 2] << 8) | camBuffer[i * 2 + 1];
//         uint8_t r = (pixel & 0xF800) >> 8;
//         uint8_t g = (pixel & 0x07E0) >> 3;
//         uint8_t b = (pixel & 0x001F) << 3;
//         grayScaleBuffer[i] = (r + g + b) / 3;
//     }
//     #endif    
//     return camLength;
// }

// static void render_image(uint32_t camLength) {
//     // Max xfer is 512, so we have to chunk anything bigger
//     int remaining = camLength;
//     int offset = bufferOffset;
//     while (remaining > 0) {
//         usb_data_t data;
//         if (offset == 0) {
//             // ns_lp_printf("First chunk\n");
//             data.descriptor = FIRST_CHUNK;
//         } else {
//             data.descriptor = MIDDLE_CHUNK;
//         };

//         data.mode = camera_config.imagePixFmt == NS_CAM_IMAGE_PIX_FMT_JPEG ? 1 : 0;

//         int chunkSize = remaining > MAX_WEBUSB_CHUNK ? MAX_WEBUSB_CHUNK : remaining;
//         memcpy(data.buffer, &camBuffer[offset], chunkSize);
//         remaining -= chunkSize;
//         offset += chunkSize;
//         if (remaining == 0) {
//             data.descriptor = LAST_CHUNK;
//         }

//         webusb_send_data((uint8_t *)&data, chunkSize + WEBUSB_HEADER_SIZE);
//         while (tud_vendor_write_available() < MAX_WEBUSB_FRAME) {
//             ns_delay_us(200);
//         }
//     }
//     tud_vendor_write_flush();
// }

void msgReceived(const uint8_t *buffer, uint32_t length, void *args) {
    // Do something with the received message (we shouldn't get one, but need the cb)
    ns_lp_printf("Received %d bytes: %s\n", length, buffer);
}

void r565_to_rgb(uint16_t color, uint8_t *r, uint8_t *g, uint8_t *b) {
    *r = (color & 0xF800) >> 8;
    *g = (color & 0x07E0) >> 3;
    *b = (color & 0x1F) << 3;
}

int ei_get_data(size_t offset, size_t length, float *out_ptr) {
    uint32_t bufIdx = 0;
    for (int i = 0; i < length; i++) {
        #ifdef GRAYSCALE
            float pix = (grayScaleBuffer[offset + i]<<16)+(grayScaleBuffer[offset + i]<<8)+grayScaleBuffer[offset + i];
        #else
            // Convert RGB565 to RGB888 to pix
            // Grab the 565 pixel at offset + i
            uint16_t pixel = (camBuffer[(offset + i) * 2] << 8) | camBuffer[(offset + i) * 2 + 1];
            uint8_t r, g, b;
            r565_to_rgb(pixel, &r, &g, &b);
            float pix = (r << 16) + (g << 8) + b;
        #endif
        out_ptr[i] = pix;
    }
    return 0;
}

static bool debug_nn = false;

int main(void) {
    // int err;
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    usb_handle_t usb_handle = NULL;

    // Power configuration (mem, cache, peripherals, clock)
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");
    NS_TRY(ns_power_config(&ns_pwr_config), "Power config failed.\n");
    am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE);

    ns_itm_printf_enable();
    ns_interrupt_master_enable();

    elapsedTime = 0;
    NS_TRY(ns_timer_init(&ei_tickTimer), "Timer Init Failed\n");

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

    // EI Init
    signal_t signal_fomo;
    signal_fomo.total_length = impulse_517398_0.input_width * impulse_517398_0.input_height;
    signal_fomo.get_data = &ei_get_data;
    ei_impulse_result_t result_fomo = { 0 };
    uint32_t camLength = 0;

    ns_lp_printf("📸 TinyVision Demo\n\n");

    ns_start_camera(&camera_config);
    ns_delay_us(10000);

    ns_press_shutter_button(&camera_config);
    // ns_take_picture(&camera_config);
    while (1) {
        ns_delay_us(1000);
        if (pictureTaken) {
            // The picture has been taken, start the DMA transfer
            // ns_lp_printf("Picture taken\n");
            buffer_length = start_dma();
            pictureTaken = false;
        }    
        if (dmaComplete) {
            // Image is in camBuffer, run the classifier and render the result
            // ns_lp_printf("DMA Complete\n");
            if (camera_config.imagePixFmt == NS_CAM_IMAGE_PIX_FMT_JPEG) {
                buffer_length =
                    chop_off_trailing_zeros(buffer_length); // Remove trailing zeros, calc new length
                // ns_lp_printf("Rendering image new len %d\n", buffer_length);
            }
            EI_IMPULSE_ERROR err = run_classifier(&impulse_handle_517398_0, &signal_fomo, &result_fomo, debug_nn);
            if (err != EI_IMPULSE_OK) {
                ns_lp_printf("ERR: Failed to run classifier (%d)\n", err);
                // return;
            } else {
                    // print the predictions
                // ns_lp_printf("Camera Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
                //             result_fomo.timing.dsp, result_fomo.timing.classification, result_fomo.timing.anomaly);

                bool bb_found = result_fomo.bounding_boxes[0].value > 0.5f;
                for (size_t ix = 0; ix < result_fomo.bounding_boxes_count; ix++) {
                    auto bb = result_fomo.bounding_boxes[ix];
                    if (bb.value == 0) {
                        continue;
                    }
                    // ns_lp_printf("    %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\n", bb.label, bb.value, bb.x, bb.y, bb.width, bb.height);
                }
                if (!bb_found) {
                    // ns_lp_printf("    No objects found\n");
                }
            }
            render_image(buffer_length, &result_fomo);

            // ns_lp_printf("Image rendered\n");
            ns_press_shutter_button(&camera_config);
            dmaComplete = false;
            // ns_lp_printf("DMA started, len %d\n", buffer_length);
        }




        // tic();
        // camLength = perform_capture();
        // ns_lp_printf("Captured image in %d ms\n", toc()); tic();
        // render_image(camLength);
        // ns_lp_printf("Rendered image in %d ms\n", toc()); tic();
        // ns_take_picture(&camera_config);
        // ns_lp_printf("Took picture in %d ms\n", toc()); tic();
        // // ns_lp_printf("Captured image\n");
        // EI_IMPULSE_ERROR err = run_classifier(&impulse_handle_517398_0, &signal_fomo, &result_fomo, debug_nn);
        // ns_lp_printf("Ran classifier in %d ms\n", toc());
        // if (err != EI_IMPULSE_OK) {
        //     ns_lp_printf("ERR: Failed to run classifier (%d)\n", err);
        //     // return;
        // } else {
        //           // print the predictions
        //     // ns_lp_printf("Camera Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
        //     //             result_fomo.timing.dsp, result_fomo.timing.classification, result_fomo.timing.anomaly);

        //     bool bb_found = result_fomo.bounding_boxes[0].value > 0.5f;
        //     for (size_t ix = 0; ix < result_fomo.bounding_boxes_count; ix++) {
        //         auto bb = result_fomo.bounding_boxes[ix];
        //         if (bb.value == 0) {
        //             continue;
        //         }
        //         ns_lp_printf("    %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\n", bb.label, bb.value, bb.x, bb.y, bb.width, bb.height);
        //     }
        //     if (!bb_found) {
        //         // ns_lp_printf("    No objects found\n");
        //     }
        // }
        ns_deep_sleep();
    }
}