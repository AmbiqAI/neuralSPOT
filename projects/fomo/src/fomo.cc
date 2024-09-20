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
#include "ArducamCamera.h"

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
extern ArducamCamera camera; // Arducam driver assumes this is a global, so :shrug:

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

#define JPG_BUFF_SIZE (320*320)
static int volatile sensorCollectBtnPressed = false;
static int volatile clientCollectBtnPressed = false;
static AM_SHARED_RW uint8_t jpgBuffer[JPG_BUFF_SIZE] __attribute__((aligned(4)));
static AM_SHARED_RW uint8_t camBuffer[CAM_BUFF_SIZE] __attribute__((aligned(4)));

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

void press_rgb_shutter_button(ns_camera_config_t *cfg) {
    camera_config.imageMode = CAM_IMAGE_MODE;
    camera_config.imagePixFmt = NS_CAM_IMAGE_PIX_FMT_RGB565;   
    int err = ns_press_shutter_button(cfg);
}

void press_jpg_shutter_button(ns_camera_config_t *cfg) {
    camera_config.imageMode = NS_CAM_IMAGE_MODE_320X320;
    camera_config.imagePixFmt = NS_CAM_IMAGE_PIX_FMT_JPEG; 
    int err = ns_press_shutter_button(cfg);
}

static uint32_t start_rgb_dma() {
    // ns_lp_printf("Starting RGB DMA\n");
    uint32_t camLength = ns_start_dma_read(&camera_config, camBuffer, &bufferOffset, CAM_BUFF_SIZE);
    bufferOffset = 0;
    return CAM_BUFF_SIZE;
}

static uint32_t start_jpg_dma() {
    // ns_lp_printf("Starting JPG DMA\n");
    uint32_t camLength = ns_start_dma_read(&camera_config, jpgBuffer, &bufferOffset, CAM_BUFF_SIZE);
    bufferOffset = 1;
    return camLength;
}

uint32_t chop_off_trailing_zeros(uint32_t length) {
    uint32_t index;
    for (index = length - 1; index >= 0; index--) {
        if (jpgBuffer[index] != 0) {
            break;
        }
    }
    return index + 1;
}

static void render_image(uint32_t camLength, ei_impulse_result_t *inf, uint8_t *buff) {
    // Max xfer is 512, so we have to chunk anything bigger
    int remaining = camLength;
    int offset = bufferOffset;
    ns_lp_printf("Rendering image len = %d inf is 0x%x, offset is %d\n",camLength, inf,offset);

    while (remaining > 0) {
        usb_data_t data;
        if (offset == 0) {
            // ns_lp_printf("First chunk\n");
            data.descriptor = FIRST_CHUNK;
        } else {
            // ns_lp_printf("Middle chunk\n");
            data.descriptor = MIDDLE_CHUNK;
        };

        // data.mode = camera_config.imagePixFmt == NS_CAM_IMAGE_PIX_FMT_JPEG ? 1 : 0;
        if (inf == NULL) {
            data.mode = 0;
        } else {
            data.mode = 1;
        }

        int chunkSize = remaining > MAX_WEBUSB_CHUNK ? MAX_WEBUSB_CHUNK : remaining;
        memcpy(data.buffer, &buff[offset], chunkSize);
        remaining -= chunkSize;
        offset += chunkSize;
        if (remaining == 0) {
            // ns_lp_printf("Last chunk\n");
            data.descriptor = LAST_CHUNK;
        }

        webusb_send_data((uint8_t *)&data, chunkSize + WEBUSB_HEADER_SIZE);
        int numr = 0;
        while (tud_vendor_write_available() < 4096) {
            ns_delay_us(200);
            if (numr++ > 100) {
                // ns_lp_printf("tud_vendor_write_available break %d\n ",tud_vendor_write_available() );
                tud_vendor_write_flush();
                break;
            }

        }
    }
    tud_vendor_write_flush();

    if (inf == NULL) {
        return;
    }
    // Send metadata
    usb_metadata_t metadata;
    metadata.descriptor = 3;
    metadata.cpu_load = 50;
    metadata.inference_latency = inf->timing.classification;
    ns_lp_printf("Inference latency: %d\n", metadata.inference_latency);
    int result_index = 0;

    for (size_t ix = 0; ix < inf->bounding_boxes_count; ix++) {
        auto bb = inf->bounding_boxes[ix];
        if (bb.value == 0.0f) {
            break;
        } 
        else if (ix > MAX_RESULTS) {
            break;
        }
        else {
            ns_lp_printf("bb.value %f\n", bb.value);
        }
        
        strncpy(metadata.results[result_index].id, bb.label, 20);
        metadata.results[result_index].x = bb.x;
        metadata.results[result_index].y = bb.y;
        metadata.results[result_index].confidence = bb.value * 100;
        result_index++;
    }
    metadata.num_results = result_index;
    // ns_lp_printf("Meta number of results: %d, descriptor %d\n", metadata.num_results, metadata.descriptor);

    webusb_send_data((uint8_t *)&metadata, sizeof(usb_metadata_t));
    tud_vendor_write_flush();
    while (tud_vendor_write_available() < MAX_WEBUSB_FRAME) {
        ns_delay_us(200);
    }
}

void msgReceived(const uint8_t *buffer, uint32_t length, void *args) {
    // Do something with the received message (we shouldn't get one, but need the cb)
    ns_lp_printf("Received %d bytes: %s\n", length, buffer);
}

void r565_to_rgb(uint16_t color, uint8_t *r, uint8_t *g, uint8_t *b) {
    uint8_t r5 = (color & 0xF800) >> 11;
    uint8_t g6 = (color & 0x07E0) >> 5;
    uint8_t b5 = (color & 0x001F);
    *r = ( r5 * 527 + 23 ) >> 6;
    *g = ( g6 * 259 + 33 ) >> 6;
    *b = ( b5 * 527 + 23 ) >> 6;
}

static bool debug_nn = false;

// #define FEAT_TEST
int ei_get_data(size_t offset, size_t length, float *out_ptr) {
    uint32_t outindex = 0;
    for (int i = offset; i < offset+length; i++) {
        #ifdef FEAT_TEST
            uint32_t intpix = features[offset + i];
            float cpix = (intpix&0xff0000) + (intpix&0x00ff00) + (intpix&0x0000ff);
            float pix = features[offset + i];
            if (i < 10) {
                ns_lp_printf("Feature %d: orig 0x%x, 0x%x, %f, cpix %f\n", i, intpix, pix, pix, cpix);
            }
        #else
        #ifdef GRAYSCALE
            float pix = (grayScaleBuffer[offset + i]<<16)+(grayScaleBuffer[offset + i]<<8)+grayScaleBuffer[offset + i];
        #else
            // Convert RGB565 to RGB888 to pix
            // Grab the 565 pixel at offset + i
            uint16_t pixel = (camBuffer[i * 2 +1] << 8) | camBuffer[i * 2];
            uint8_t r, g, b;
            r565_to_rgb(pixel, &r, &g, &b);
            float pix = (r << 16) + (g << 8) + b;
            // if (i<10) {
            //     ns_lp_printf("Pixel %d: 0x%x, r %d, g %d, b %d, pix %f\n", i, pixel, r, g, b, pix);
            // }
        #endif
        #endif
        out_ptr[outindex++] = pix;
    }
    return 0;
}

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
    
    // signal_fomo.total_length = impulse_517398_0.input_width * impulse_517398_0.input_height;
    signal_fomo.total_length = impulse_522036_0.input_width * impulse_522036_0.input_height;
    signal_fomo.get_data = &ei_get_data;
    ei_impulse_result_t result_fomo = { 0 };
    uint32_t camLength = 0;

    // Test inference on canned data
    // EI_IMPULSE_ERROR err = run_classifier(&impulse_handle_522036_0, &signal_fomo, &result_fomo, debug_nn);
    // if (err != EI_IMPULSE_OK) {
    //     ns_lp_printf("ERR: Failed to run classifier (%d)\n", err);
    //     // return;
    // } else {
    //         // print the predictions
    //     ns_lp_printf("Camera Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
    //                 result_fomo.timing.dsp, result_fomo.timing.classification, result_fomo.timing.anomaly);

    //     bool bb_found = result_fomo.bounding_boxes[0].value > 0;
    //     for (size_t ix = 0; ix < result_fomo.bounding_boxes_count; ix++) {
    //         ns_lp_printf("hello\n");
    //         auto bb = result_fomo.bounding_boxes[ix];
    //         ns_lp_printf("bb.value: %f\n", bb.value);
    //         // if (bb.value == 0) {
    //         //     continue;
    //         // }
    //         ns_lp_printf("    %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\n", bb.label, bb.value, bb.x, bb.y, bb.width, bb.height);
    //     }
    //     if (!bb_found) {
    //         ns_lp_printf("    No objects found\n");
    //     }
    // } 
    // while(1) {
    //     ns_deep_sleep();
    // }


    ns_lp_printf("📸 TinyVision Demo\n\n");

    ns_start_camera(&camera_config);
    setBrightness(&camera, CAM_BRIGHTNESS_LEVEL_2);
    setEV(&camera, CAM_EV_LEVEL_3);
    setAutoExposure(&camera, true); 
    setContrast(&camera, CAM_CONTRAST_LEVEL_3);
    ns_delay_us(100000);

    press_rgb_shutter_button(&camera_config);

    app_state_e state = TAKING_RGB_IMAGE;
    EI_IMPULSE_ERROR err;
    bool bb_found;
    ei_impulse_result_bounding_box_t bb;

    while (1) {
        ns_delay_us(1000);
        switch (state) {
        case TAKING_RGB_IMAGE:
            if (pictureTaken) {
                // ns_lp_printf("RGB Picture taken\n");
                buffer_length = start_rgb_dma();
                pictureTaken = false;
                state = TRANSFERRING_RGB_IMAGE;
            }
            break;
        case TRANSFERRING_RGB_IMAGE:
            if (dmaComplete) {
                // Image is in camBuffer, start taking jpg
                // ns_lp_printf("RGB DMA Complete\n");
                // render_image(buffer_length, NULL, camBuffer);

                press_jpg_shutter_button(&camera_config);
                dmaComplete = false;
                state = INFERENCING;
            }
            break;
        case INFERENCING:
            // Infer on cambuf while jpg is being captured
            err = run_classifier(&impulse_handle_522036_0, &signal_fomo, &result_fomo, debug_nn);
            if (err != EI_IMPULSE_OK) {
                ns_lp_printf("ERR: Failed to run classifier (%d)\n", err);
            } 
            else {
                // ns_lp_printf("Inference complete\n");
                // bb_found = result_fomo.bounding_boxes[0].value > 0;
                // for (size_t ix = 0; ix < result_fomo.bounding_boxes_count; ix++) {
                //     // ns_lp_printf("hello\n");
                //     bb = result_fomo.bounding_boxes[ix];
                //     // ns_lp_printf("bb.value: %f\n", bb.value);
                //     if (bb.value == 0) {
                //         continue;
                //     }
                //     ns_lp_printf("    %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\n", bb.label, bb.value, bb.x, bb.y, bb.width, bb.height);
                // }
                // if (!bb_found) {
                //     ns_lp_printf("    No objects found\n");
                // }
            }
            // render_image(buffer_length, &result_fomo, camBuffer);
            // if (bb_found) {
            //     state = INFERENCING;
            // } else {
                state = TAKING_JPG_IMAGE;
            // }
            break;
        case TAKING_JPG_IMAGE:
            if (pictureTaken) {
                // ns_lp_printf("JPG Picture taken\n");
                buffer_length = start_jpg_dma();
                pictureTaken = false;
                state = TRANSFERRING_JPG_IMAGE;
            }
            break;
        case TRANSFERRING_JPG_IMAGE:
            if (dmaComplete) {
                // ns_lp_printf("JPG DMA Complete\n");
                // We have an inference result and a jpg image, render it
                dmaComplete = false;

                // Image is in jpgBuffer, render the result
                // if (camera_config.imagePixFmt == NS_CAM_IMAGE_PIX_FMT_JPEG) {
                    buffer_length = chop_off_trailing_zeros(buffer_length); // Remove trailing zeros, calc new length
                // }
                press_rgb_shutter_button(&camera_config); // hide capture behind rendering
                render_image(buffer_length, &result_fomo, jpgBuffer);
                dmaComplete = false;
                state = TAKING_RGB_IMAGE;
            }
            break;
        }
        ns_deep_sleep();
    }




    // while (1) {
    //     ns_delay_us(1000);
    //     if (pictureTaken) {
    //         // The picture has been taken, start the DMA transfer
    //         // ns_lp_printf("Picture taken\n");
    //         buffer_length = start_dma();
    //         pictureTaken = false;
    //     }    
    //     if (dmaComplete) {
    //         // Image is in camBuffer, run the classifier and render the result
    //         // ns_lp_printf("DMA Complete\n");
    //         if (camera_config.imagePixFmt == NS_CAM_IMAGE_PIX_FMT_JPEG) {
    //             buffer_length =
    //                 chop_off_trailing_zeros(buffer_length); // Remove trailing zeros, calc new length
    //             // ns_lp_printf("Rendering image new len %d\n", buffer_length);
    //         }
    //         // EI_IMPULSE_ERROR err = run_classifier(&impulse_handle_517398_0, &signal_fomo, &result_fomo, debug_nn);
    //         EI_IMPULSE_ERROR err = run_classifier(&impulse_handle_522036_0, &signal_fomo, &result_fomo, debug_nn);
    //         if (err != EI_IMPULSE_OK) {
    //             ns_lp_printf("ERR: Failed to run classifier (%d)\n", err);
    //         } else {
    //                 // print the predictions
    //             // ns_lp_printf("Camera Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
    //             //             result_fomo.timing.dsp, result_fomo.timing.classification, result_fomo.timing.anomaly);

    //             // bool bb_found = result_fomo.bounding_boxes[0].value > 0;
    //             // for (size_t ix = 0; ix < result_fomo.bounding_boxes_count; ix++) {
    //             //     auto bb = result_fomo.bounding_boxes[ix];
    //             //     if (bb.value == 0) {
    //             //         continue;
    //             //     }
    //             //     ns_lp_printf("    %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\n", bb.label, bb.value, bb.x, bb.y, bb.width, bb.height);
    //             // }
    //             // if (!bb_found) {
    //             //     ns_lp_printf("    No objects found\n");
    //             // }
    //         }
    //         render_image(buffer_length, &result_fomo);

    //         // ns_lp_printf("Image rendered\n");
    //         setBrightness(&camera, CAM_BRIGHTNESS_LEVEL_DEFAULT);
    //         // setColorEffect(&camera, CAM_COLOR_FX_BW);

    //         setAutoExposure(&camera, true);
    //         ns_press_shutter_button(&camera_config);
    //         dmaComplete = false;
    //         // ns_lp_printf("DMA started, len %d\n", buffer_length);
    //     }




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
    //     ns_deep_sleep();
    // }
}