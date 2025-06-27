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

typedef void (*webusb_rx_cb)(const uint8_t *buf, uint32_t buf_len, void *param);

/**
 * @brief Blocking USB Send Data
 *
 * @param buf Pointer to buffer with data to be sent
 * @param bufsize Requested number of bytes
 * @return uint32_t The amount of data written to the ring-buffer.
 */
uint32_t webusb_send_data(uint8_t *buf, uint32_t bufsize);

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
