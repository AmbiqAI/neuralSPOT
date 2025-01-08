/*
 * Copyright 2020 NXP
 * Copyright 2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "erpc_manually_constructed.hpp"
#include "erpc_transport_setup.h"
#include "erpc_uart_transport.hpp"

using namespace erpc;

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

ERPC_MANUALLY_CONSTRUCTED(UartTransport, s_uart_transport);

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

erpc_transport_t erpc_transport_uart_init(ns_uart_handle_t handle)
{
    erpc_transport_t transport;

    s_uart_transport.construct(handle);
    if (s_uart_transport->init() == kErpcStatus_Success)
    {
        transport = reinterpret_cast<erpc_transport_t>(s_uart_transport.get());
    }
    else
    {
        transport = NULL;
    }

    return transport;
}
