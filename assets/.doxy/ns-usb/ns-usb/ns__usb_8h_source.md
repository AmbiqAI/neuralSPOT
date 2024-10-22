

# File ns\_usb.h

[**File List**](files.md) **>** [**includes-api**](dir_a2a61ebab6bf7d344fb1c36a75cc33e9.md) **>** [**ns\_usb.h**](ns__usb_8h.md)

[Go to the documentation of this file](ns__usb_8h.md)

```C++


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

typedef enum {
    NS_USB_CDC_DEVICE,   
    NS_USB_HID_DEVICE,   
    NS_USB_MSC_DEVICE,   
    NS_USB_VENDOR_DEVICE 
} ns_usb_device_type_e;

typedef struct {
    usb_handle_t handle;
    void *rx_buffer;
    void *tx_buffer;
    uint8_t status;
    uint8_t itf;
    bool dtr;
    bool rts;
} ns_usb_transaction_t;

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

typedef struct {
    const ns_core_api_t *api;        
    ns_usb_device_type_e deviceType; 
    void *rx_buffer; 
    uint16_t rx_bufferLength; 
    void *tx_buffer; 
    uint16_t tx_bufferLength;     
    ns_usb_rx_cb rx_cb;           
    ns_usb_tx_cb tx_cb;           
    ns_usb_service_cb service_cb; 
    ns_tusb_desc_webusb_url_t *desc_url; 
} ns_usb_config_t;


extern uint32_t ns_usb_init(ns_usb_config_t *, usb_handle_t *);

extern void ns_usb_register_callbacks(usb_handle_t, ns_usb_rx_cb, ns_usb_tx_cb);

extern uint32_t ns_usb_recieve_data(usb_handle_t handle, void *buffer, uint32_t bufsize);

extern uint32_t ns_usb_send_data(usb_handle_t handle, void *buffer, uint32_t bufsize);

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

```

