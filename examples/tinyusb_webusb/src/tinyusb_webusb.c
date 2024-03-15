//*****************************************************************************
//
//! @file tinyusb_webusb.c
//!
//! @brief tinyusb webusb example.
//!
//! This example demonstrates WebUSB as web serial with browser with WebUSB support (e.g Chrome).
//! After enumerated successfully, browser will pop-up notification
//! with URL to landing page, click on it to test
//! - Click "Connect" and select device, When connected the on-board LED will litted up.
//! - Any charters received from either webusb/Serial will be echo back to webusb and Serial
//!
//! Note:
//! - The WebUSB landing page notification is currently disabled in Chrome
//! on Windows due to Chromium issue 656702 (https://crbug.com/656702). You have to
//! go to landing page (below) to test
//!
//! - On Windows 7 and prior: You need to use Zadig tool to manually bind the
//! WebUSB interface with the WinUSB driver for Chrome to access. From windows 8 and 10, this
//!  is done automatically by firmware.
//! - On Linux/macOS, udev permission may need to be updated by
//!   - copying '/examples/device/99-tinyusb.rules' file to /etc/udev/rules.d/ then
//!   - run 'sudo udevadm control --reload-rules && sudo udevadm trigger'
//!
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2022, Ambiq Micro, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision release_sdk_4_1_0-8020bdf229 of the AmbiqSuite Development Package.
//
//*****************************************************************************

/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

/* This example demonstrates WebUSB as web serial with browser with WebUSB support (e.g Chrome).
 * After enumerated successfully, browser will pop-up notification
 * with URL to landing page, click on it to test
 *  - Click "Connect" and select device, When connected the on-board LED will litted up.
 *  - Any charters received from either webusb/Serial will be echo back to webusb and Serial
 *
 * Note:
 * - The WebUSB landing page notification is currently disabled in Chrome
 * on Windows due to Chromium issue 656702 (https://crbug.com/656702). You have to
 * go to landing page (below) to test
 *
 * - On Windows 7 and prior: You need to use Zadig tool to manually bind the
 * WebUSB interface with the WinUSB driver for Chrome to access. From windows 8 and 10, this
 * is done automatically by firmware.
 *
 * - On Linux/macOS, udev permission may need to be updated by
 *   - copying '/examples/device/99-tinyusb.rules' file to /etc/udev/rules.d/ then
 *   - run 'sudo udevadm control --reload-rules && sudo udevadm trigger'
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "am_util_stdio.h"

#include "ns_core.h"
#include "ns_peripherals_button.h"
#include "ns_peripherals_power.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_usb.h"

#define MY_RX_BUFSIZE 4096
#define MY_TX_BUFSIZE 4096
uint8_t my_rx_ff_buf[MY_RX_BUFSIZE];
uint8_t my_tx_ff_buf[MY_TX_BUFSIZE];

static ns_usb_config_t webUsbConfig = {
    .api = &ns_usb_V1_0_0,
    .deviceType = NS_USB_VENDOR_DEVICE,
    // .deviceType = NS_USB_CDC_DEVICE,
    .rx_buffer = NULL,
    .rx_bufferLength = 0,
    .tx_buffer = NULL,
    .tx_bufferLength = 0,
    .rx_cb = NULL,
    .tx_cb = NULL,
    .service_cb = NULL};

void msgReceived(const uint8_t *buffer, uint32_t length, void *args) {
    // Do something with the received message
    ns_lp_printf("Received %d bytes: %s\n", length, buffer);
}

char msgBuf[120];
void sendMessage(const char *fmt, ...) {
    va_list args;
    memset(msgBuf, 0, sizeof(msgBuf));
    va_start(args, fmt);
    am_util_stdio_vsnprintf(msgBuf, sizeof(msgBuf), fmt, args);
    am_util_stdio_vprintf(fmt, args);
    va_end(args);
    webusb_send_data((uint8_t *)msgBuf, strlen(msgBuf));
    // ns_ble_send_value(&webbleMessage, NULL); // Send over BLE
}

int main(void) {
    usb_handle_t usb_handle = NULL;
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    volatile int g_intButtonPressed = 0;
    ns_button_config_t button_config = {
        .api = &ns_button_V1_0_0,
        .button_0_enable = true,
        .button_1_enable = false,
        .button_0_flag = &g_intButtonPressed,
        .button_1_flag = NULL};
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\b");
    ns_power_config(&ns_development_default);
    ns_itm_printf_enable();
    NS_TRY(ns_peripheral_button_init(&button_config), "Button init failed\n");

    // Initialize USB
    webUsbConfig.rx_buffer = my_rx_ff_buf;
    webUsbConfig.rx_bufferLength = MY_RX_BUFSIZE;
    webUsbConfig.tx_buffer = my_tx_ff_buf;
    webUsbConfig.tx_bufferLength = MY_TX_BUFSIZE;
    NS_TRY(ns_usb_init(&webUsbConfig, &usb_handle), "USB Init Failed\n");
    ns_lp_printf("USB Init Success\n");
    // WebUSB doesn't use rx_cb or tx_cb, so we don't need to set them
    // Instead, it uses its own callbacks, registered below
    webusb_register_msg_cb(msgReceived, NULL);
    ns_lp_printf("WebUSB Init Success\n");

    // Wait for button before sending a message
    ns_lp_printf("Waiting for button press\n");
    while (g_intButtonPressed == 0) {
        ns_delay_us(1000);
    }

    sendMessage("Hello, WebUSB!\n");
    ns_lp_printf("Sent message\n");

    while (1) {
        ns_deep_sleep();
    }
}
