/**
 * @file ns-usb.h
 * @author Carlos Morales
 * @brief API definition for NeuralSPOT USB
 * @version 0.1
 * @date 2022-08-18
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef NS_USB
#define NS_USB

#ifdef __cplusplus
extern "C" {
#endif
#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "tusb.h"

typedef void *usb_handle_t;

typedef enum {
    NS_USB_CDC_DEVICE, ///< CDC (uart-like) device
    NS_USB_HID_DEVICE, ///< Human Interface Device (not supported)
    NS_USB_MSC_DEVICE  ///< Mass Storage Device (not supported)
} ns_usb_device_type_e;

typedef struct {
    usb_handle_t handle;
    void *buffer;
    uint8_t status;
    uint8_t itf;
    bool dtr;
    bool rts;
} ns_usb_transaction_t;

typedef void (*ns_usb_rx_cb)(ns_usb_transaction_t *);
typedef void (*ns_usb_tx_cb)(ns_usb_transaction_t *);

typedef struct {
    ns_usb_device_type_e deviceType;
    void *buffer;
    uint16_t bufferLength;
    ns_usb_rx_cb rx_cb;
    ns_usb_tx_cb tx_cb;
} ns_usb_config_t;

extern usb_handle_t
ns_usb_init(ns_usb_config_t *);

extern void ns_usb_register_callbacks(usb_handle_t, ns_usb_rx_cb, ns_usb_tx_cb);

extern uint32_t
ns_usb_recieve_data(usb_handle_t handle, void *buffer, uint32_t bufsize);

extern uint32_t
ns_usb_send_data(usb_handle_t handle, void *buffer, uint32_t bufsize);

#ifdef __cplusplus
}
#endif
#endif