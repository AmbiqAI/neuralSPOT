/**
 * @file ns-usb.c
 * @author NeuralSPOT USB Utilities
 * @brief Helper library for neuralspot features using USB
 * @version 0.1
 * @date 2022-08-18
 *
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
ns_init_usb(ns_usb_config_t *cfg) {

    usb_config.deviceType = cfg->deviceType;
    usb_config.buffer = cfg->buffer;
    usb_config.bufferLength = cfg->bufferLength;
    usb_config.rx_cb = cfg->rx_cb;
    usb_config.tx_cb = cfg->tx_cb;

    tusb_init();

    return &usb_config; // TODO make this a better handle
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
