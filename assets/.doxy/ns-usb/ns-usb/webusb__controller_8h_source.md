

# File webusb\_controller.h

[**File List**](files.md) **>** [**includes-api**](dir_a2a61ebab6bf7d344fb1c36a75cc33e9.md) **>** [**webusb\_controller.h**](webusb__controller_8h.md)

[Go to the documentation of this file](webusb__controller_8h.md)

```C++

//*****************************************************************************
//
//*****************************************************************************

//*****************************************************************************
//
// ${copyright}
//
// This is part of revision ${version} of the AmbiqSuite Development Package.
//
//*****************************************************************************

#ifndef WEBUSB_CONTROLLER_H
#define WEBUSB_CONTROLLER_H


#if defined __cplusplus
extern "C" {
#endif

typedef void (*webusb_rx_cb)(const uint8_t *buf, uint32_t buf_len, void *param);

uint32_t webusb_send_data(uint8_t *buf, uint32_t bufsize);

void webusb_init(void);

void webusb_task(void);

void webusb_register_msg_cb(webusb_rx_cb cb, void *param);

void webusb_register_raw_cb(webusb_rx_cb cb, void *param);




#if defined __cplusplus
}
#endif

#endif // WEBUSB_CONTROLLER_H

```

