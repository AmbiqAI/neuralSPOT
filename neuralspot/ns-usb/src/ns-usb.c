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

#include "ns-usb.h"

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

uint32_t
ns_usb_recieve_data(usb_handle_t handle, void *buffer, uint32_t bufsize) {
    return tud_cdc_read(buffer, bufsize);
}

uint32_t
ns_usb_send_data(usb_handle_t handle, void *buffer, uint32_t bufsize) {
    uint32_t retval =  tud_cdc_write(buffer, bufsize);
    tud_cdc_write_flush();
    return retval;
}