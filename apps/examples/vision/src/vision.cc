/**
 * @file Vision.cc
 * @author Ambiq
 * @brief This demo is intended to work with an Arducam and the vision WebUSB client
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

#include "ns_camera.h"
#include "vision.h"

//
// WebUSB Configuration and Datatypes
//
#define MY_RX_BUFSIZE 4096
#define MY_TX_BUFSIZE 4096

static uint8_t my_rx_ff_buf[MY_RX_BUFSIZE] __attribute__((aligned(16)));
static uint8_t my_tx_ff_buf[MY_TX_BUFSIZE] __attribute__((aligned(16)));
// WebUSB URL
static ns_tusb_desc_webusb_url_t vision_url;
static ns_usb_config_t webUsbConfig = {
    .api = &ns_usb_V1_0_0,
    .deviceType = NS_USB_VENDOR_DEVICE,
    .rx_buffer = NULL,
    .rx_bufferLength = 0,
    .tx_buffer = NULL,
    .tx_bufferLength = 0,
    .rx_cb = NULL,
    .tx_cb = NULL,
    .service_cb = NULL,
    .desc_url = &vision_url // Filled in at runtime
};

// USB Data sent to WebUSB client (used to communicated to webusb client)
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

//
// Camera Configuration - put jpgBuffer in SRAM because it's too big for TCM
//
#define JPG_MODE
#ifndef JPG_MODE
static uint8_t rgbBuffer[RGB_BUFF_SIZE] __attribute__((aligned(16)));
#else
static uint8_t jpgBuffer[JPG_BUFF_SIZE] __attribute__((aligned(16)));
#endif
void picture_dma_complete(ns_camera_config_t *cfg); 
void picture_taken_complete(ns_camera_config_t *cfg);

// Camera needs to handle interrupt from the corrent ISR
#define iom_isr am_iom_isrx(CAM_SPI_IOM)
#define am_iom_isrx(n) am_iom_isr(n)
#define am_iom_isr(n) am_iomaster##n##_isr
extern "C" void iom_isr(void) {
    ns_spi_handle_iom_isr();
}

ns_camera_config_t camera_config = {
    .api = &ns_camera_V1_0_0,
    .spiSpeed = CAM_SPI_SPEED,
    .cameraHw = NS_ARDUCAM,
#ifdef JPG_MODE
    .imageMode = NS_CAM_IMAGE_MODE_320X320,
    .imagePixFmt = NS_CAM_IMAGE_PIX_FMT_JPEG,
#else
    .imageMode = NS_CAM_IMAGE_MODE_96X96,
    .imagePixFmt = NS_CAM_IMAGE_PIX_FMT_RGB565,
#endif
    .spiConfig = {.iom = CAM_SPI_IOM}, 
    .dmaCompleteCb = picture_dma_complete,
    .pictureTakenCb = picture_taken_complete,
};

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

//
// State and IPC for DMA and Camera Polling Background Tasks
//
volatile bool dmaComplete = false;  // Set by callback, monitored and reset by main loop
volatile bool pictureTaken = false; // Set by callback, monitored and reset by main loop
uint32_t buffer_length = 0;         // set when DMA is started
static uint32_t bufferOffset = 0;   // set when DMA is started

void picture_dma_complete(ns_camera_config_t *cfg) {
    // ns_lp_printf("DMA Complete CB\n");
    dmaComplete = true;
}

void picture_taken_complete(ns_camera_config_t *cfg) {
    pictureTaken = true;
    // ns_lp_printf("Picture taken CB\n");
}

// Helper Functions

void tic() {
    uint32_t oldTime = elapsedTime;
    elapsedTime = ns_us_ticker_read(&tickTimer);
    if (elapsedTime == oldTime) {
        // We've saturated the timer, reset it
        ns_timer_clear(&tickTimer);
    }
}

uint32_t toc() { return ns_us_ticker_read(&tickTimer) - elapsedTime; }

void press_jpg_shutter_button(ns_camera_config_t *cfg) {
    camera_config.imageMode = NS_CAM_IMAGE_MODE_320X320;
    camera_config.imagePixFmt = NS_CAM_IMAGE_PIX_FMT_JPEG;
    // ns_lp_printf("Taking JPG picture\n");
    ns_press_shutter_button(cfg);
}

void press_rgb_shutter_button(ns_camera_config_t *cfg) {
    camera_config.imageMode = NS_CAM_IMAGE_MODE_96X96;
    camera_config.imagePixFmt = NS_CAM_IMAGE_PIX_FMT_RGB565;
    // ns_lp_printf("Taking RGB picture\n");
    ns_press_shutter_button(cfg);
}

#ifndef JPG_MODE
static uint32_t start_rgb_dma() {
    uint32_t camLength = ns_start_dma_read(&camera_config, rgbBuffer, &bufferOffset, RGB_BUFF_SIZE);
    return camLength;
}
#else // JPG_MODE
static uint32_t start_jpg_dma() {
    uint32_t camLength = ns_start_dma_read(&camera_config, jpgBuffer, &bufferOffset, JPG_BUFF_SIZE);
    return camLength;
}
#endif

/**
 * @brief Sends the image to the WebUSB client in chunks
 *
 * @param camLength Total length of the image
 * @param buff Buffer containing the image
 */
static void render_image(uint32_t camLength, uint8_t *buff) {
    // Max WebUSB xfer is 512, so we have to chunk anything bigger
    int remaining = camLength;
    int offset = bufferOffset;
    // ns_lp_printf("Sending image of length %d\n", camLength);

    // Calculate FPS and restart timer
    //  uint32_t elapsed = toc();
    //  ns_lp_printf("FPS: %d\n", 1000000 / elapsed);
    //  tic();

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

        // When Arducam takes a JPG picture, it adds a 0x00 to the beginning for some reason
        // Chop that off. The ending zeros have already been removed in chop_off_trailing_zeros
        // before render_image is called.

        int chunkSize = remaining > MAX_WEBUSB_CHUNK ? MAX_WEBUSB_CHUNK : remaining;
        memcpy(data.buffer, &buff[offset], chunkSize);
        remaining -= chunkSize;
        offset += chunkSize;
        if (remaining == 0) {
            // ns_lp_printf("Last chunk\n");
            data.descriptor = LAST_CHUNK;
        }

        webusb_send_data((uint8_t *)&data, chunkSize + WEBUSB_HEADER_SIZE);
        while (tud_vendor_write_available() < MAX_WEBUSB_FRAME) {
            // USB likes to be lazy about sending the last packet, so force it to
            // ns_lp_printf(".");
            ns_delay_us(400);
        }
        ns_delay_us(400);
    }
    // Only if Ambiqsuite 4.5.0+ or 5.2.0+ is used
    #if defined(NS_AMBIQSUITE_VERSION_R4_5_0) or defined(NS_AMBIQSUITE_VERSION_R5_2_0)
    tud_vendor_write_flush();
    #endif
}

void msgReceived(const uint8_t *buffer, uint32_t length, void *args) {
    // The message contains information about how to set the camera
    ns_camera_adjust_settings(buffer[0], buffer[1], buffer[2]);

    // ns_lp_printf("Received %d bytes: %s\n", length, buffer);
}

int main(void) {
    // int err;
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    usb_handle_t usb_handle = NULL;

    // Power configuration (mem, cache, peripherals, clock)
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");
    NS_TRY(ns_power_config(&ns_pwr_config), "Power config failed.\n");
    // am_bsp_low_power_init();
    #if defined(AM_PART_APOLLO510L) || defined(AM_PART_APOLLO330P)
    am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE2);
    #else
    am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE);
    #endif

    ns_itm_printf_enable();
    ns_interrupt_master_enable();

    #if defined(AM_PART_APOLLO5B) || defined(AM_PART_APOLLO510B)
    ns_perf_enable_pcsamp();
    ns_lp_printf("DWT ITM enable\n");
    #endif



    elapsedTime = 0;

    // Initialize the URL descriptor
    strcpy(vision_url.url, "ambiqai.github.io/web-ble-dashboards/vision_demo/");
    vision_url.bDescriptorType = 3;
    vision_url.bScheme = 1;
    vision_url.bLength = 3 + sizeof(vision_url.url) - 1;

    // WebUSB Setup
    webusb_register_raw_cb(msgReceived, NULL);
    webUsbConfig.rx_buffer = my_rx_ff_buf;
    webUsbConfig.rx_bufferLength = MY_RX_BUFSIZE;
    webUsbConfig.tx_buffer = my_tx_ff_buf;
    webUsbConfig.tx_bufferLength = MY_TX_BUFSIZE;

    NS_TRY(ns_usb_init(&webUsbConfig, &usb_handle), "USB Init Failed\n");
    ns_lp_printf("USB Init Success\n");

    // Camera Setup
    if (ns_camera_init(&camera_config) != NS_STATUS_SUCCESS) {
        ns_lp_printf(
            "Camera Init Failed, please ensure the camera is present and connected correctly\n");
        ns_core_fail_loop();
    }
    ns_lp_printf("Camera Init Success\n");
    ns_stop_camera(&camera_config);

    ns_lp_printf("📸 Arducam SPI Camera Demo\n\n");
    ns_lp_printf("🔌 Connect to the WebUSB client at "
                 "https://ambiqai.github.io/web-ble-dashboards/vision_demo\n");

    // Send camera images to webusb clients
    ns_start_camera(&camera_config);
    ns_delay_us(10000);

    // Take the first picture
#ifdef JPG_MODE
    press_jpg_shutter_button(&camera_config);
#else
    press_rgb_shutter_button(&camera_config);
#endif

    // Note: this demo only takes JPG pictures, but the code is in here to take RGB pictures.
    // See the FOMO example for more elaborate camera use cases.

    // There are two camera interactions here:
    //   - one that starts the action of taking a picture. The time this takes depends on shutter
    //   speed
    //   - one that starts the DMA transfer of the picture once the camera is done taking it
    //
    // Both of these tasks are done in by interrupt handlers
    //   - Once a picture is started ("shutter button pressed"), the camera needs to be polled for
    //   completion
    //     This is done by a timer interrupt within ns_camera.c, which calls the
    //     picture_taken_complete callback
    //   - The image data must then be transfered over SPI to the MCU. This is done by DMAing
    //   chunks. ns_camera.c
    //     takes care of monitoring progress and requesting the next chunk. Once the image is fully
    //     transfered, the picture_dma_complete callback is called.

    // Now sit in a loop triggering pics and DMA, then sending them to the WebUSB client
    while (1) {
        // ns_delay_us(1000);
        if (pictureTaken) {
            // ns_lp_printf("Picture taken\n");
            tic();
#ifdef JPG_MODE
            buffer_length = start_jpg_dma();
            if (buffer_length > JPG_BUFF_SIZE) {
                // We set the jpg buff size to less than the absolute max assuming jpg
                // will be smaller than that. If it's not, we have a problem.
                ns_lp_printf("JPG Bigger than expected: %d\n", buffer_length);
            }
#else
            buffer_length = start_rgb_dma();
#endif

            pictureTaken = false;
        }
        if (dmaComplete) {
// ns_lp_printf("DMA Complete %d\n", toc());
#ifdef JPG_MODE
            buffer_length = ns_chop_off_trailing_zeros(
                jpgBuffer, buffer_length); // Remove trailing zeros, calc new length
            press_jpg_shutter_button(
                &camera_config); // Start the next picture while current is xfering
            render_image(buffer_length, jpgBuffer);
#else
            press_rgb_shutter_button(&camera_config);
            render_image(buffer_length, rgbBuffer);
#endif
            dmaComplete = false;
        }
        // ns_deep_sleep();
    }
}
