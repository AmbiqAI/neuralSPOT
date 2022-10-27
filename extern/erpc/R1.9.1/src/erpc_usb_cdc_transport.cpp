/*
 * Copyright 2020-2021 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>
#include "erpc_usb_cdc_transport.hpp"
#include "ns_usb.h"
#include "ns_ambiqsuite_harness.h"

using namespace erpc;

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

static volatile bool s_isTransferReceiveCompleted = false;
static volatile bool s_isTransferSendCompleted = false;
static UsbCdcTransport *s_usbcdc_instance = NULL;

// SDK_ALIGN(static uint8_t s_serialWriteHandleBuffer[SERIAL_MANAGER_WRITE_HANDLE_SIZE], 4);
// static serial_write_handle_t s_serialWriteHandle = &s_serialWriteHandleBuffer[0]; /*!< serial manager write handle */

// SDK_ALIGN(static uint8_t s_serialReadHandleBuffer[SERIAL_MANAGER_READ_HANDLE_SIZE], 4);
// static serial_read_handle_t s_serialReadHandle = &s_serialReadHandleBuffer[0]; /*!< serial manager read handle */
////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////


static void ERPC_ns_usb_tx_cb(ns_usb_transaction_t *tr)
{
    // ns_printf("NS USB TX CB\n");

    UsbCdcTransport *transport = s_usbcdc_instance;
    if (NULL != tr)
    {
        if (AM_HAL_STATUS_SUCCESS == tr->status)
        {
            transport->tx_cb();
        }
        else
        {
            /* Handle other status if needed */
        }
    }
}

static void ERPC_ns_usb_rx_cb(ns_usb_transaction_t *tr)
{
    // ns_printf("NS USB RX CB\n");

    UsbCdcTransport *transport = s_usbcdc_instance;
    if (NULL != tr)
    {
        if (AM_HAL_STATUS_SUCCESS == tr->status)
        {
            transport->rx_cb();
        }
        else
        {
            /* Handle other status if needed */
        }
    }
}

void UsbCdcTransport::tx_cb(void)
{
#if !ERPC_THREADS_IS(NONE)
    m_txSemaphore.putFromISR();
#else
    s_isTransferSendCompleted = true;
#endif
}

void UsbCdcTransport::rx_cb(void)
{
#if !ERPC_THREADS_IS(NONE)
    m_rxSemaphore.putFromISR();
#else
    s_isTransferReceiveCompleted = true;
#endif
}

UsbCdcTransport::UsbCdcTransport(usb_handle_t usbHandle)
: m_usbHandle(usbHandle)
#if !ERPC_THREADS_IS(NONE)
, m_rxSemaphore()
, m_txSemaphore()
#endif
{
    s_usbcdc_instance = this;
}

UsbCdcTransport::~UsbCdcTransport(void)
{
    /* Cleanup */
    // SerialManager_CloseWriteHandle(s_serialWriteHandle);
    // SerialManager_CloseReadHandle(s_serialReadHandle);
    // SerialManager_Deinit(m_serialHandle);
}

erpc_status_t UsbCdcTransport::init(void)
{
    erpc_status_t status = kErpcStatus_Success;

    ns_usb_register_callbacks(m_usbHandle, ERPC_ns_usb_rx_cb, ERPC_ns_usb_tx_cb);

    return status;
}

erpc_status_t UsbCdcTransport::underlyingReceive(uint8_t *data, uint32_t size)
{
    erpc_status_t status = kErpcStatus_Success;

    s_isTransferReceiveCompleted = false;

    //ns_printf("NS USB asked for %d\n", size);

    uint32_t bytes_rx = 0;

    bytes_rx = ns_usb_recieve_data(m_usbHandle, data, size);
    // ns_printf("NS USB RX asked for %d, Rec %d bytes\n", size, bytes_rx);
    if (bytes_rx < size) {
        // ns_printf("NS USB ERROR: asked for %d, Rec %d bytes\n", size, bytes_rx);
        status = kErpcStatus_ReceiveFailed;
        ns_usb_handle_read_error(m_usbHandle);
    } // else { ns_printf("#");}

    return status;
}

erpc_status_t UsbCdcTransport::underlyingSend(const uint8_t *data, uint32_t size)
{
    erpc_status_t status = kErpcStatus_Success;

    s_isTransferSendCompleted = false;

    uint32_t bytes_tx = 0;
    
    while (bytes_tx < size) {
        bytes_tx += ns_usb_send_data(m_usbHandle, (uint8_t*)(data+bytes_tx), size - bytes_tx); // blocking
        tud_cdc_write_flush();

        // ns_printf("NS USB TX asked to send %d, sent %d bytes\n", size, bytes_tx);
    }

    if (bytes_tx != size) {
        ns_printf("NS USB ERROR: asked to send %d, sent %d bytes\n", size, bytes_tx);
        status = kErpcStatus_ReceiveFailed;
    }
    tud_cdc_write_flush();

    return status;
}
