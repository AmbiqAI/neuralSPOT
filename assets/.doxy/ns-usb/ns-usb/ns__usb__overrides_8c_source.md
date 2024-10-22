

# File ns\_usb\_overrides.c

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-usb**](dir_450d7ce7daa9d29b9b3b5cb7d00f16f9.md) **>** [**src**](dir_f255d6a194767e484966de6b2584c9a6.md) **>** [**overrides**](dir_9c7a8f6708b262eb1b0dd6b85c35f66f.md) **>** [**ns\_usb\_overrides.c**](ns__usb__overrides_8c.md)

[Go to the documentation of this file](ns__usb__overrides_8c.md)

```C++

#include "ns_usb.h"

// Mini-rant: these two functions which override a WEAK function in TinyUSB
// are declared here to get around armlink's overzealous (and inconsistent)
// removal - even with this, the linker will insists on removing them unless
// we force a --keep in the linker flags. I can't even...

extern volatile uint8_t gGotUSBRx;
extern ns_usb_config_t usb_config;

// Invoked when CDC interface received data from host
void tud_cdc_rx_cb(uint8_t itf) {
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

void tud_cdc_tx_complete_cb(uint8_t itf) {
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

```

