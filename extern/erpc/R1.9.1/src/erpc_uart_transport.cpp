/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * Copyright 2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "erpc_uart_transport.hpp"
#include "erpc_message_buffer.hpp"
#include "ns_ambiqsuite_harness.h"
#include "ns_uart.h"
#include <cstdio>
#include <string>

using namespace erpc;

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

static volatile bool s_isTransferReceiveCompleted = false;
static volatile bool s_isTransferSendCompleted = false;
static UartTransport *s_uart_instance = NULL;
////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////


static void ERPC_ns_uart_tx_cb(ns_uart_transaction_t *tr)
{
    // ns_printf("NS USB TX CB\n");

    UartTransport *transport = s_uart_instance;
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

static void ERPC_ns_uart_rx_cb(ns_uart_transaction_t *tr)
{
    // ns_printf("NS USB RX CB\n");

    UartTransport *transport = s_uart_instance;
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

UartTransport::UartTransport(ns_uart_handle_t uartHandle)
: m_uartHandle(uartHandle)
#if !ERPC_THREADS_IS(NONE)
, m_rxSemaphore()
, m_txSemaphore()
#endif
{
        s_uart_instance = this;
}

UartTransport::~UartTransport(void)
{
    // serial_close(m_serialHandle);
}

void UartTransport::tx_cb(void)
{
#if !ERPC_THREADS_IS(NONE)
    m_txSemaphore.putFromISR();
#else
    s_isTransferSendCompleted = true;
#endif
}

void UartTransport::rx_cb(void)
{
#if !ERPC_THREADS_IS(NONE)
    m_rxSemaphore.putFromISR();
#else
    s_isTransferReceiveCompleted = true;
#endif
}
erpc_status_t UartTransport::init(void)
{
    erpc_status_t status = kErpcStatus_Success;

    ns_uart_register_callbacks(m_uartHandle, ERPC_ns_uart_rx_cb, ERPC_ns_uart_tx_cb);

    return status;
}

erpc_status_t UartTransport::underlyingSend(const uint8_t *data, uint32_t size)
{   
    if(((ns_uart_config_t*)m_uartHandle)->tx_blocking)
    {
        uint32_t status = ns_uart_blocking_send_data(((ns_uart_config_t*)m_uartHandle), (char *)data, size);
        return (status != AM_HAL_STATUS_SUCCESS) ? kErpcStatus_SendFailed : kErpcStatus_Success;
    }
    else {
        uint32_t status = ns_uart_nonblocking_send_data(((ns_uart_config_t*)m_uartHandle), (char *)data, size);
        return (status != AM_HAL_STATUS_SUCCESS) ? kErpcStatus_SendFailed : kErpcStatus_Success;
    }
}
erpc_status_t UartTransport::underlyingReceive(uint8_t *data, uint32_t size)
{
    if(((ns_uart_config_t*)m_uartHandle)->rx_blocking)
    {
        uint32_t status = ns_uart_blocking_receive_data(((ns_uart_config_t*)m_uartHandle), (char *)data, size);
        return (status != AM_HAL_STATUS_SUCCESS) ? kErpcStatus_ReceiveFailed : kErpcStatus_Success;
    }
    else {
        uint32_t status = ns_uart_nonblocking_receive_data(((ns_uart_config_t*)m_uartHandle), (char *)data, size);
        return (status != AM_HAL_STATUS_SUCCESS) ? kErpcStatus_ReceiveFailed : kErpcStatus_Success;
    }
}