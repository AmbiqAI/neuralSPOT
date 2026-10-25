//*****************************************************************************
//
//! @file webusb_controller.h
//!
//! @brief Connect MCU and Audio Tool via WebUSB.
//!
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

#ifndef WEBUSB_TX_TIMEOUT_US
// Upper bound on each TX FIFO wait, in microseconds. A host can stop reading IN
// data without disconnecting (for example, a closed browser tab), and the FIFO
// then never drains. Override with a module DEFINE.
#define WEBUSB_TX_TIMEOUT_US 1000000
#endif

typedef void (*webusb_rx_cb)(const uint8_t *buf, uint32_t buf_len, void *param);

/**
 * @brief Blocking USB Send Data
 *
 * Waits for the TX FIFO to drain before writing, and on some parts waits again
 * after flushing. Each wait gives up after WEBUSB_TX_TIMEOUT_US. After a wait
 * times out, later calls return 0 immediately until the FIFO drains or the host
 * reconnects.
 *
 * @param buf Pointer to buffer with data to be sent
 * @param bufsize Requested number of bytes
 * @return uint32_t The amount of data written to the ring-buffer. 0 if no WebUSB
 *         host is connected, buf is NULL, or the FIFO did not drain in time.
 */
uint32_t webusb_send_data(uint8_t *buf, uint32_t bufsize);

/**
 * @brief Wait until the TX FIFO has room for at least needed bytes
 *
 * @param needed Number of free TX FIFO bytes to wait for. A value above
 *        CFG_TUD_VENDOR_TX_BUFSIZE can never be met and always times out.
 * @param poll_us Delay between checks, in microseconds. 0 is treated as 1.
 * @return uint32_t 1 if the space is available, 0 if the host disconnected or
 *         WEBUSB_TX_TIMEOUT_US elapsed first
 */
uint32_t webusb_wait_tx_space(uint32_t needed, uint32_t poll_us);

/**
 * @brief Init tinyusb.
 *
 */
void webusb_init(void);

/**
 * @brief tinyusb device task.
 *
 */
void webusb_task(void);

/**
 * @brief Register message callback function and pass to callback pointer
 *
 * @param cd Callback function
 * @param param Pointer to user parameter which will be passed to callback
 *
 */
void webusb_register_msg_cb(webusb_rx_cb cb, void *param);

/**
 * @brief Register raw callback function and pass to callback pointer
 *
 * @param cd Callback function
 * @param param Pointer to user parameter which will be passed to callback
 *
 */
void webusb_register_raw_cb(webusb_rx_cb cb, void *param);

/**
 * @brief Check if USB is connected
 *
 * @return uint32_t 1 if connected, 0 if not
 */
uint32_t webusb_is_connected(void);

#if defined __cplusplus
}
#endif

#endif // WEBUSB_CONTROLLER_H
