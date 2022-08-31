/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _EMBEDDED_RPC__USB_CDC_TRANSPORT_H_
#define _EMBEDDED_RPC__USB_CDC_TRANSPORT_H_

#include <cstdio>
#include "erpc_config_internal.h"
#if !ERPC_THREADS_IS(NONE)
#include "erpc_threading.h"
#endif

#include "erpc_framed_transport.hpp"

extern "C" {
#include "ns_usb.h"
}

/*!
 * @addtogroup USB_CDC_transport
 * @{
 * @file
 */

////////////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////////////

namespace erpc {
/*!
 * @brief Very basic transport to send/receive messages via virtual USB CDC port.
 *
 * @ingroup USB_CDC_transport
 */
class UsbCdcTransport : public FramedTransport
{
public:
    /*!
     * @brief Constructor.
     *
     * @param[in] serialHandle Pointer to point to a memory space of size #SERIAL_MANAGER_HANDLE_SIZE allocated by the
     * caller, see serial manager header file.
     * @param[in] serialConfig Pointer to user-defined configuration structure allocated by the caller, see serial
     * manager header file.
     * @param[in] usbCdcConfig Pointer to serial port usb config structure allocated by the caller, see serial manager
     * header file.
     * @param[in] usbRingBuffer Pointer to point serial manager ring buffer allocated by the caller, see serial manager
     * header file.
     * @param[in] usbRingBufferLength Serial manager ring buffer size.
     */
    UsbCdcTransport(usb_handle_t usbHandle);

    /*!
     * @brief Destructor.
     */
    virtual ~UsbCdcTransport(void);

    /*!
     * @brief Initialize USB CDC peripheral configuration structure with values specified in UsbCdcTransport
     * constructor.
     *
     * @retval kErpcStatus_InitFailed When USB CDC init function failed.
     * @retval kErpcStatus_Success When USB CDC init function was executed successfully.
     */
    virtual erpc_status_t init(void);

    /*!
     * @brief Function called from Serial Manager Rx Callback to unblock the receive function
     *
     * Unblocks the receive function.
     */
    void rx_cb(void);

    /*!
     * @brief Function called from Serial Manager Tx Callback to unblock the send function
     *
     * Unblocks the send function.
     */
    void tx_cb(void);

protected:
#if !ERPC_THREADS_IS(NONE)
    Semaphore m_rxSemaphore; /*!< Semaphore used by RTOS to block task until the receiving is not complete */
    Semaphore m_txSemaphore; /*!< Semaphore used by RTOS to block task until the sending is not complete */
#endif
private:
    usb_handle_t m_usbHandle;

    /*!
     * @brief Receive data from USB CDC peripheral.
     *
     * @param[inout] data Preallocated buffer for receiving data.
     * @param[in] size Size of data to read.
     *
     * @retval kErpcStatus_ReceiveFailed USB CDC failed to receive data.
     * @retval kErpcStatus_Success Successfully received all data.
     */
    virtual erpc_status_t underlyingReceive(uint8_t *data, uint32_t size);

    /*!
     * @brief Write data to USB CDC peripheral.
     *
     * @param[in] data Buffer to send.
     * @param[in] size Size of data to send.
     *
     * @retval kErpcStatus_Success Always returns success status.
     */
    virtual erpc_status_t underlyingSend(const uint8_t *data, uint32_t size);
};

} // namespace erpc

/*! @} */

#endif /* _EMBEDDED_RPC__USB_CDC_TRANSPORT_H_ */
