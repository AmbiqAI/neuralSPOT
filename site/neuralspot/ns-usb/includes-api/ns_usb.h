/**
 * @file ns-usb.h
 * @author Carlos Morales
 * @brief API definition for NeuralSPOT USB
 * @version 0.1
 * @date 2022-08-18
 *
 * @copyright Copyright (c) 2022
 *
 * \addtogroup ns-usb
 * @{
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
    #include "ns_core.h"
    #include "webusb_controller.h"
    #include "crc32.h"

    #define NS_USB_V0_0_1                                                                          \
        { .major = 0, .minor = 0, .revision = 1 }
    #define NS_USB_V1_0_0                                                                          \
        { .major = 1, .minor = 0, .revision = 0 }

    #define NS_USB_OLDEST_SUPPORTED_VERSION NS_USB_V0_0_1
    #define NS_USB_CURRENT_VERSION NS_USB_V1_0_0
    #define NS_USB_API_ID 0xCA0006
    #define MAX_URL_LENGTH 100

extern const ns_core_api_t ns_usb_V0_0_1;
extern const ns_core_api_t ns_usb_V1_0_0;
extern const ns_core_api_t ns_usb_oldest_supported_version;
extern const ns_core_api_t ns_usb_current_version;

typedef void *usb_handle_t;

/// @brief USB Device Type
typedef enum {
    NS_USB_CDC_DEVICE, ///< CDC (uart-like) device
    NS_USB_HID_DEVICE, ///< Human Interface Device (not supported)
    NS_USB_MSC_DEVICE, ///< Mass Storage Device (not supported)
    NS_USB_VENDOR_DEVICE ///< Vendor Device (e.g. for WebUSB)
} ns_usb_device_type_e;

/// @brief USB Transaction Control Stucture
typedef struct {
    usb_handle_t handle;
    void *rx_buffer;
    void *tx_buffer;
    uint8_t status;
    uint8_t itf;
    bool dtr;
    bool rts;
} ns_usb_transaction_t;

/// @brief URL buffer Stucture
typedef struct TU_ATTR_PACKED
{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bScheme;
  char    url[MAX_URL_LENGTH];
} ns_tusb_desc_webusb_url_t;
typedef void (*ns_usb_rx_cb)(ns_usb_transaction_t *);
typedef void (*ns_usb_tx_cb)(ns_usb_transaction_t *);
typedef void (*ns_usb_service_cb)(uint8_t);

/**
 * @brief USB Configuration Struct
 *
 */
typedef struct {
    const ns_core_api_t *api;        ///< API prefix
    ns_usb_device_type_e deviceType; ///< Device type, only CDC is currently supported
    void *rx_buffer; ///< Pointer to allocated buffer which USB CDC will use for rx transfers
    uint16_t rx_bufferLength; ///< Length of rx buffer
    void *tx_buffer; ///< Pointer to allocated buffer which USB CDC will use for tx transfers
    uint16_t tx_bufferLength;     ///< Length of tx buffer
    ns_usb_rx_cb rx_cb;           ///< Callback for rx events
    ns_usb_tx_cb tx_cb;           ///< Callback for tx events
    ns_usb_service_cb service_cb; ///< Callback for service events
    ns_tusb_desc_webusb_url_t *desc_url; ///< WebUSB URL descriptor

} ns_usb_config_t;

/**
 * @brief Initialize the USB system
 *
 * @param cfg
 * @param handle
 * @return uint32_t Status
 */
extern uint32_t ns_usb_init(ns_usb_config_t *, usb_handle_t *);

/**
 * @brief Register callbacks for USB events, should be called after ns_usb_init
 *
 * @param handle
 * @param rx_cb
 * @param tx_cb
 */
extern void ns_usb_register_callbacks(usb_handle_t, ns_usb_rx_cb, ns_usb_tx_cb);

/**
 * @brief Blocking USB Receive Data
 *
 * @param handle USB handle
 * @param buffer Pointer to buffer where data will be placed
 * @param bufsize Requested number of bytes
 * @return uint32_t
 */
extern uint32_t ns_usb_recieve_data(usb_handle_t handle, void *buffer, uint32_t bufsize);

/**
 * @brief Blocking USB Send Data
 *
 * @param handle USB handle
 * @param buffer Pointer to buffer with data to be sent
 * @param bufsize Requested number of bytes
 * @return uint32_t
 */
extern uint32_t ns_usb_send_data(usb_handle_t handle, void *buffer, uint32_t bufsize);

/**
 * @brief Flushes the USB RX fifo after a delay, resets ns_usb rx state
 *
 * @param h handle
 */
extern void ns_usb_handle_read_error(usb_handle_t h);

extern bool ns_usb_data_available(usb_handle_t handle);

extern uint8_t *ns_usb_get_rx_buffer();

extern uint8_t *ns_usb_get_tx_buffer();

extern uint32_t ns_get_cdc_rx_bufferLength();

extern uint32_t ns_get_cdc_tx_bufferLength();

extern ns_tusb_desc_webusb_url_t * ns_get_desc_url();

    #ifdef __cplusplus
}
    #endif
#endif
/** @}*/
