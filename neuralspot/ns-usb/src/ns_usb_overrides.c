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