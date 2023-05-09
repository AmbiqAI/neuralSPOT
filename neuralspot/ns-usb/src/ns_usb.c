/**
 * @file ns-usb.c
 * @author Carlos Morales
 * @brief NeuralSPOT USB Utilities
 *
 * @version 0.1
 * @date 2022-08-18
 *
 * Helper library for neuralspot features using USB
 * @copyright Copyright (c) 2022
 *
 */

#include "ns_usb.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_core.h"
#include "ns_timer.h"

const ns_core_api_t ns_usb_V0_0_1 = {.apiId = NS_USB_API_ID, .version = NS_USB_V0_0_1};

const ns_core_api_t ns_usb_V1_0_0 = {.apiId = NS_USB_API_ID, .version = NS_USB_V1_0_0};

const ns_core_api_t ns_usb_oldest_supported_version = {.apiId = NS_USB_API_ID,
                                                       .version = NS_USB_V0_0_1};

const ns_core_api_t ns_usb_current_version = {.apiId = NS_USB_API_ID, .version = NS_USB_V1_0_0};

static ns_usb_config_t usb_config = {.api = &ns_usb_V1_0_0,
                                     .deviceType = NS_USB_CDC_DEVICE,
                                     .rx_buffer = NULL,
                                     .rx_bufferLength = 0,
                                     .tx_buffer = NULL,
                                     .tx_bufferLength = 0,
                                     .rx_cb = NULL,
                                     .tx_cb = NULL,
                                     .service_cb = NULL};

volatile static uint8_t gGotUSBRx = 0;

bool
ns_usb_data_available(usb_handle_t handle) {
    return (gGotUSBRx == 1);
}

uint32_t
ns_get_cdc_rx_bufferLength() {
    return usb_config.rx_bufferLength;
}
uint32_t
ns_get_cdc_tx_bufferLength() {
    return usb_config.tx_bufferLength;
}

uint8_t *
ns_usb_get_rx_buffer() {
    return usb_config.rx_buffer;
}

uint8_t *
ns_usb_get_tx_buffer() {
    return usb_config.tx_buffer;
}

static void
ns_usb_service_callback(ns_timer_config_t *c) {
    // Invoked in ISR context
    tud_task();
    if (usb_config.service_cb != NULL)
        usb_config.service_cb(gGotUSBRx);
}

ns_timer_config_t g_ns_usbTimer = {.api = &ns_timer_V1_0_0,
                                   .timer = NS_TIMER_USB,
                                   .enableInterrupt = true,
                                   .periodInMicroseconds = 1000,
                                   .callback = ns_usb_service_callback};

uint32_t
ns_usb_init(ns_usb_config_t *cfg, usb_handle_t *h) {

#ifndef NS_DISABLE_API_VALIDATION
    if (cfg == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }

    if (ns_core_check_api(cfg->api, &ns_usb_oldest_supported_version, &ns_usb_current_version)) {
        return NS_STATUS_INVALID_VERSION;
    }

    if ((cfg->rx_buffer == NULL) || (cfg->tx_buffer == NULL)) {
        return NS_STATUS_INVALID_CONFIG;
    }
#endif

    usb_config.deviceType = cfg->deviceType;
    usb_config.rx_buffer = cfg->rx_buffer;
    usb_config.rx_bufferLength = cfg->rx_bufferLength;
    usb_config.tx_buffer = cfg->tx_buffer;
    usb_config.tx_bufferLength = cfg->tx_bufferLength;
    usb_config.rx_cb = cfg->rx_cb;
    usb_config.tx_cb = cfg->tx_cb;
    usb_config.service_cb = cfg->service_cb;
    *h = (void *)&usb_config;
    tusb_init();

    // Set up a timer to service usb

    NS_TRY(ns_timer_init(&g_ns_usbTimer), "Timer Init Failed.\n");

    return NS_STATUS_SUCCESS;
}

void
ns_usb_register_callbacks(usb_handle_t handle, ns_usb_rx_cb rxcb, ns_usb_tx_cb txcb) {
    ((ns_usb_config_t *)handle)->rx_cb = rxcb;
    ((ns_usb_config_t *)handle)->tx_cb = txcb;
}

// Invoked when CDC interface received data from host
void
tud_cdc_rx_cb(uint8_t itf) {
    (void)itf;
    ns_usb_transaction_t rx;
    if (usb_config.rx_cb != NULL) {
        rx.handle = &usb_config;
        rx.rx_buffer = usb_config.rx_buffer;
        rx.tx_buffer = usb_config.tx_buffer;
        rx.status = AM_HAL_STATUS_SUCCESS;
        rx.itf = itf;
        usb_config.rx_cb(&rx);
    }
    gGotUSBRx = 1;
    // ns_lp_printf("---rx---\n");
}

void
tud_cdc_tx_complete_cb(uint8_t itf) {
    (void)itf;
    ns_usb_transaction_t rx;
    if (usb_config.tx_cb != NULL) {
        rx.handle = &usb_config;
        rx.rx_buffer = usb_config.rx_buffer;
        rx.tx_buffer = usb_config.tx_buffer;
        rx.status = AM_HAL_STATUS_SUCCESS;
        rx.itf = itf;
        usb_config.tx_cb(&rx);
    }
    // ns_lp_printf("---tx---\n");
}

/**
 * @brief Blocking USB Receive Data
 *
 * @param handle USB handle
 * @param buffer Pointer to buffer where data will be placed
 * @param bufsize Requested number of bytes
 * @return uint32_t
 */
uint32_t
ns_usb_recieve_data(usb_handle_t handle, void *buffer, uint32_t bufsize) {

    // USB reads one block at a time, loop until we get full
    // request
    uint32_t bytes_rx = 0;
    uint32_t retries = 10000;
    uint32_t block_retries = 3; // number of rx blocks we'll retry

    // if (gGotUSBRx == 0)
    //     ns_lp_printf("Kicking off read of %d, have %d, sem %d \n", bufsize, tud_cdc_available(),
    //             gGotUSBRx);
    // uint32_t before = tud_cdc_available();
    // uint8_t before_sem = gGotUSBRx;
    // // ns_delay_us(10);
    // uint32_t after = tud_cdc_available();
    // uint8_t after_sem = gGotUSBRx;
    while (tud_cdc_available() < bufsize) {
        ns_interrupt_master_disable(); // critical region
        gGotUSBRx = 0;                 // set to 1 in IRQ context, need to disable IRQs for a bit
        // ns_lp_printf("Mystery path after %d %d %d\n", after, after_sem, gGotUSBRx);

        // Wait for a block to come in
        while (gGotUSBRx == 0) {
            ns_interrupt_master_enable();
            ns_delay_us(150);
            retries--;
            if (retries == 0) {
                ns_lp_printf("[ERROR] ns_usb_recieve_data exhausted wait for sem\n");
                break;
            }
        };

        // Incoming blocks may be less than needed bytes, try up to 3 times
        if (block_retries == 0) {
            ns_lp_printf("[ERROR] ns_usb_recieve_data exhausted block retries\n");
            break;
        }
        block_retries--;
    }
    // uint32_t after2 = tud_cdc_available();
    // uint8_t after2_sem = gGotUSBRx;
    gGotUSBRx = 0;
    bytes_rx = tud_cdc_read((void *)buffer, bufsize);
    // if (retries != 10000)
    //     ns_lp_printf("rx_data ask %d got %d retries %d before cnt, sem: %d,%d, after cnt, sem:
    //     %d, %d, af2 cnt,sem: %d, %d\n",
    //         bufsize, bytes_rx, retries, before, before_sem, after, after_sem, after2,
    //         after2_sem);
    // ns_lp_printf("Got bytes %d\n", bytes_rx);
    //  ns_delay_us(100);

    // dontoptimizeme = after + after_sem + before + before_sem + after2 + after2_sem;
    return bytes_rx;
}

/**
 * @brief Flushes the USB RX fifo after a delay, resets ns_usb rx state
 *
 * @param h handle
 */
void
ns_usb_handle_read_error(usb_handle_t h) {
    int i;
    for (i = 0; i < 100; i++) {
        ns_delay_us(10000);
    }
    ns_printf("after wait\n");
    tud_cdc_read_flush();
    gGotUSBRx = 0; // may be set by final RX
}

/**
 * @brief Blocking USB Send Data
 *
 * @param handle USB handle
 * @param buffer Pointer to buffer with data to be sent
 * @param bufsize Requested number of bytes
 * @return uint32_t
 */
uint32_t
ns_usb_send_data(usb_handle_t handle, void *buffer, uint32_t bufsize) {

    uint32_t bytes_tx = 0;
    while (bytes_tx < bufsize) {
        bytes_tx += tud_cdc_write((void *)(buffer + bytes_tx), bufsize - bytes_tx); // blocking
        tud_cdc_write_flush();
        // ns_lp_printf("NS USB  asked to send %d, sent %d bytes\n", bufsize, bytes_tx);
    }

    // uint32_t retval =  tud_cdc_write(buffer, bufsize);
    // tud_cdc_write_flush();
    return bytes_tx;
}
