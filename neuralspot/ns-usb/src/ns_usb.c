/**
 * @file ns-usb.c
 * @author Carlos Morales
 * @brief neuralSPOT USB Utilities
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
#include "ns_core.h"
#include "ns_timer.h"
#include "tusb.h"

const ns_core_api_t ns_usb_V0_0_1 = {.apiId = NS_USB_API_ID, .version = NS_USB_V0_0_1};

const ns_core_api_t ns_usb_V1_0_0 = {.apiId = NS_USB_API_ID, .version = NS_USB_V1_0_0};

const ns_core_api_t ns_usb_oldest_supported_version = {
    .apiId = NS_USB_API_ID, .version = NS_USB_V0_0_1};

const ns_core_api_t ns_usb_current_version = {.apiId = NS_USB_API_ID, .version = NS_USB_V1_0_0};

ns_usb_config_t usb_config = {
    .api = &ns_usb_V1_0_0,
    .deviceType = NS_USB_CDC_DEVICE,
    .rx_buffer = NULL,
    .rx_bufferLength = 0,
    .tx_buffer = NULL,
    .tx_bufferLength = 0,
    .rx_cb = NULL,
    .tx_cb = NULL,
    .service_cb = NULL,
    .desc_url = NULL};

volatile uint8_t gGotUSBRx = 0;

// volatile const void *pTUSB_WeakFcnPointers[] =
// {
//     (void *)tud_mount_cb,
// #ifndef TUSB_ADDED_FUNCTIONS
//     (void *)tud_umount_cb,
// #endif
//     (void *)tud_suspend_cb,
//     (void *)tud_resume_cb,
//     (void *)tud_cdc_line_state_cb,
//     (void *)tud_cdc_rx_cb,
//     (void *)tud_vendor_rx_cb,
//     (void *)tud_vendor_control_xfer_cb,
//     (void *)tud_descriptor_bos_cb,
//     (void *)tud_descriptor_device_cb,
//     (void *)tud_descriptor_configuration_cb,
//     (void *)tud_descriptor_string_cb,
// };

bool ns_usb_data_available(usb_handle_t handle) { return (gGotUSBRx == 1); }

uint32_t ns_get_cdc_rx_bufferLength() { return usb_config.rx_bufferLength; }
uint32_t ns_get_cdc_tx_bufferLength() { return usb_config.tx_bufferLength; }

uint8_t *ns_usb_get_rx_buffer() { return usb_config.rx_buffer; }

uint8_t *ns_usb_get_tx_buffer() { return usb_config.tx_buffer; }

static void ns_usb_service_callback(ns_timer_config_t *c) {
    // Invoked in ISR context
    // ns_lp_printf("U");
    tud_task();
    if (usb_config.service_cb != NULL) {
        usb_config.service_cb(gGotUSBRx);
        ns_lp_printf("got usb rx %d\n", gGotUSBRx);
    }
}

ns_timer_config_t g_ns_usbTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_USB,
    .enableInterrupt = true,
    .periodInMicroseconds = 1000,
    .callback = ns_usb_service_callback};

uint32_t ns_usb_init(ns_usb_config_t *cfg, usb_handle_t *h) {
    ns_lp_printf("ns_usb_init\n");
#ifndef NS_DISABLE_API_VALIDATION
    if (cfg == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }

    if (ns_core_check_api(cfg->api, &ns_usb_oldest_supported_version, &ns_usb_current_version)) {
        return NS_STATUS_INVALID_VERSION;
    }

    if ((cfg->rx_buffer == NULL) || (cfg->tx_buffer == NULL)) {
        return NS_STATUS_INVALID_CONFIG;
    }
#endif
    ns_lp_printf("ns_usb_init\n");
    usb_config.deviceType = cfg->deviceType;
    usb_config.rx_buffer = cfg->rx_buffer;
    usb_config.rx_bufferLength = cfg->rx_bufferLength;
    usb_config.tx_buffer = cfg->tx_buffer;
    usb_config.tx_bufferLength = cfg->tx_bufferLength;
    usb_config.rx_cb = cfg->rx_cb;
    usb_config.tx_cb = cfg->tx_cb;
    usb_config.service_cb = cfg->service_cb;
    usb_config.desc_url = cfg->desc_url;
    *h = (void *)&usb_config;
    tusb_init();

    // Set up a timer to service usb

    NS_TRY(ns_timer_init(&g_ns_usbTimer), "Timer Init Failed.\n");

    return NS_STATUS_SUCCESS;
}

void ns_usb_register_callbacks(usb_handle_t handle, ns_usb_rx_cb rxcb, ns_usb_tx_cb txcb) {
    ((ns_usb_config_t *)handle)->rx_cb = rxcb;
    ((ns_usb_config_t *)handle)->tx_cb = txcb;
}

// Blocking read from USB. Will loop until we get the full buffer or timeout
uint32_t ns_usb_recieve_data(usb_handle_t handle, void *buffer, uint32_t bufsize) {

    // USB reads one block at a time, loop until we get full
    // request
    uint32_t bytes_rx = 0;
    uint32_t retries = 100000;
    uint32_t block_retries = 15; // number of rx blocks we'll retry

    if (gGotUSBRx == 0)
        // ns_lp_printf("Kicking off read of %d, have %d, sem %d \n", bufsize, tud_cdc_available(),
        //         gGotUSBRx);
        ns_delay_us(100);
    // uint32_t before = tud_cdc_available();
    // uint8_t before_sem = gGotUSBRx;
    // ns_delay_us(10);
    // uint32_t after = tud_cdc_available();
    // uint8_t after_sem = gGotUSBRx;
    while (tud_cdc_available() < bufsize) {
        // If there isn't enough data to satisfy request, wait for a while

        // ns_lp_printf("Mystery path after %d %d %d\n", after, after_sem, gGotUSBRx);

        // We only care abot gGotUSBRx in order to count 'blocks' (i.e. USB RX interrupts)
        ns_interrupt_master_disable(); // critical region
        if (tud_cdc_available() < bufsize) {
            gGotUSBRx = 0; // set to 1 in IRQ context, need to disable IRQs for a bit
        }
        ns_interrupt_master_enable();

        // Wait for a block to come in
        while ((gGotUSBRx == 0) && (tud_cdc_available() < bufsize)) {
            ns_delay_us(750);
            // tud_task(); // process USB events
            retries--;
            if (retries == 0) {
                ns_lp_printf("[ERROR] ns_usb_recieve_data exhausted wait for sem\n");
                break;
            }
        };

        // Incoming blocks may be less than needed bytes, try up to 5 times
        if (block_retries == 0) {
            ns_lp_printf("[ERROR] ns_usb_recieve_data exhausted block retries\n");
            break;
        }
        block_retries--;
    }
    // uint32_t after2 = tud_cdc_available();
    // uint8_t after2_sem = gGotUSBRx;
    gGotUSBRx = 0;
    ns_interrupt_master_disable(); // critical region
    tud_task(); // process USB events
    bytes_rx = tud_cdc_read((void *)buffer, bufsize);
    ns_interrupt_master_enable();
    // if (retries != 10000)
    //     ns_lp_printf("rx_data ask %d got %d retries %d before cnt, sem: %d,%d, after cnt, sem: %d, %d, af2 cnt,sem: %d, %d\n",
    //         bufsize, bytes_rx, retries, before, before_sem, after, after_sem, after2,
    //         after2_sem);
    // ns_lp_printf("Got bytes %d\n", bytes_rx);
     ns_delay_us(200);

    // dontoptimizeme = after + after_sem + before + before_sem + after2 + after2_sem;
    if (bytes_rx != bufsize) {
        ns_lp_printf("[ERROR] RX error, asked for %d, got %d\n", bufsize, bytes_rx);
    }
    return bytes_rx;
}

void ns_usb_handle_read_error(usb_handle_t h) {
    int i;
    for (i = 0; i < 100; i++) {
        ns_delay_us(10000);
    }
    ns_lp_printf("In error after wait\n");
    tud_cdc_read_flush();
    gGotUSBRx = 0; // may be set by final RX
}

uint32_t ns_usb_send_data(usb_handle_t handle, void *buffer, uint32_t bufsize) {

    uint32_t bytes_tx = 0;
    // ns_lp_printf("NS USB  asked to send %d from 0x%x, \n", bufsize, (uint32_t)buffer);
    // Make sure there is no pending data in the USB TX buffer
    // This is a blocking call, so we can be sure that the buffer is flushed
    while (tud_cdc_write_available() < usb_config.tx_bufferLength) {
        // ns_lp_printf("NS USB flushing before of send %d, avail %d\n", bufsize, tud_cdc_write_available());
        ns_delay_us(1000);
        ns_interrupt_master_disable(); // critical region
        tud_cdc_write_flush(); // flush the write buffer
        tud_task(); // flush the write buffer
        ns_interrupt_master_enable();
    }

    while (bytes_tx < bufsize) {
        ns_interrupt_master_disable(); // critical region
        bytes_tx += tud_cdc_write((void *)(buffer + bytes_tx), bufsize - bytes_tx); // blocking
        tud_cdc_write_flush();
        tud_task(); // process USB events
        ns_interrupt_master_enable();
        // tud_task(); // flush the write buffer
        ns_delay_us(1000);
        // ns_lp_printf("NS USB  asked to send %d, sent %d bytes\n", bufsize, bytes_tx);
    }
    #if defined(AM_PART_APOLLO5B) || defined(AM_PART_APOLLO510L) || defined(AM_PART_APOLLO330P) || defined(NS_AMBIQSUITE_VERSION_R4_5_0)
    tud_cdc_write_flush();
    // tud_task(); // flush the write buffer
    // ns_lp_printf("NS _USB avail %d\n", tud_cdc_write_available());
    #endif

    // uint32_t retval =  tud_cdc_write(buffer, bufsize);
    // tud_cdc_write_flush();
    return bytes_tx;
}

ns_tusb_desc_webusb_url_t * ns_get_desc_url() {
    return usb_config.desc_url;
}
