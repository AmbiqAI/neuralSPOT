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

static ns_usb_config_t usb_config = {.deviceType = NS_USB_CDC_DEVICE,
                                     .buffer = NULL,
                                     .bufferLength = 0,
                                     .rx_cb = NULL,
                                     .tx_cb = NULL};

usb_handle_t
ns_usb_init(ns_usb_config_t *cfg) {

    usb_config.deviceType = cfg->deviceType;
    usb_config.buffer = cfg->buffer;
    usb_config.bufferLength = cfg->bufferLength;
    usb_config.rx_cb = cfg->rx_cb;
    usb_config.tx_cb = cfg->tx_cb;

    tusb_init();

    return (void *)&usb_config; // TODO make this a better handle
}

void
ns_usb_register_callbacks(usb_handle_t handle, ns_usb_rx_cb rxcb,
                          ns_usb_tx_cb txcb) {
    ((ns_usb_config_t *)handle)->rx_cb = rxcb;
    ((ns_usb_config_t *)handle)->tx_cb = txcb;
}

uint8_t static gGotUSBRx = 0;

// Invoked when CDC interface received data from host
void
tud_cdc_rx_cb(uint8_t itf) {
    (void)itf;
    ns_usb_transaction_t rx;
    if (usb_config.rx_cb != NULL) {
        rx.handle = &usb_config;
        rx.buffer = usb_config.buffer;
        rx.status = AM_HAL_STATUS_SUCCESS;
        rx.itf = itf;
        usb_config.rx_cb(&rx);
    }
    gGotUSBRx = 1;
}

void
tud_cdc_tx_complete_cb(uint8_t itf) {
    (void)itf;
    ns_usb_transaction_t rx;
    if (usb_config.tx_cb != NULL) {
        rx.handle = &usb_config;
        rx.buffer = usb_config.buffer;
        rx.status = AM_HAL_STATUS_SUCCESS;
        rx.itf = itf;
        usb_config.tx_cb(&rx);
    }
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
    uint32_t retries = 10150;

    tud_task();

    // ns_printf("Kicking off read of %d, have %d, sem %d \n", bufsize, tud_cdc_available(), gGotUSBRx);
    if (tud_cdc_available() < bufsize) {
        while (gGotUSBRx == 0) {
            tud_task();
            ns_delay_us(100);
            retries--;
            if (retries == 0) {break;} 
        };
    }
    gGotUSBRx = 0;
    bytes_rx = tud_cdc_read((void*)buffer, bufsize);
    // ns_printf("Got bytes %d\n", bytes_rx);
    return bytes_rx;
    // while (bytes_rx < bufsize) {
    //     tud_task(); // tinyusb device task
    //     ns_delay_us(100);

    //     old_rx = bytes_rx;
    //     bytes_rx += tud_cdc_read((void*)(buffer+bytes_rx), bufsize - bytes_rx);
    //     if (bytes_rx == old_rx) {
    //         retries--;
    //     }
    //     if (retries == 0) {break;} 
    // }
    // // if (retries == 0)
    // //     ns_printf("Got retries %d\n", retries);
    // // else
    //      ns_printf("Got bytes %d\n", bytes_rx);

    // return bytes_rx;
}

/**
 * @brief Flushes the USB RX fifo after a delay, resets ns_usb rx state
 * 
 * @param h handle
 */
void ns_usb_handle_read_error(usb_handle_t h) {
    int i;
    for (i=0;i<100;i++) {
        ns_delay_us(10000); ns_printf(".");
        tud_task(); // tinyusb device task
    }
    // ns_printf("after wait\n");
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
        tud_task(); // tinyusb device task
        bytes_tx += tud_cdc_write((void*)(buffer+bytes_tx), bufsize - bytes_tx); // blocking
        tud_cdc_write_flush();
        // ns_printf("NS USB  asked to send %d, sent %d bytes\n", bufsize, bytes_tx);
    }

    // uint32_t retval =  tud_cdc_write(buffer, bufsize);
    // tud_cdc_write_flush();
    return bytes_tx;
}