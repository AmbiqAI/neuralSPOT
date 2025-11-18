//*****************************************************************************
//
//! @file am_devices_i3c_mb85rc1mt.h
//!
//! @brief Fujitsu MB85RC1MT 128K FRAM Memory Driver with I3C Interface.
//!
//! @addtogroup devices_i3c_mb85rc1mt MB85RC1MT I3C FRAM Memory Driver
//! @ingroup devices
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2025, Ambiq Micro, Inc.
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
// This is part of revision release_sdk5_2_a_1_1-c2486c8ef of the AmbiqSuite Development Package.
//
//*****************************************************************************

#ifndef AM_DEVICES_I3C_MB85RC1MT_H
#define AM_DEVICES_I3C_MB85RC1MT_H

#include "am_bsp.h"

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! @name Global definitions for MB85RC1MT
//! @{
//
//*****************************************************************************
#define AM_DEVICES_MB85RC1MT_MAX_DEVICE_NUM  1
#define AM_DEVICES_MB85RC1MT_SLAVE_ID        0x7C
#define AM_DEVICES_MB85RC1MT_STATIC_ADDRESS  0x50
#define AM_DEVICES_MB85RC1MT_DEVICEID_ADDR   0xA0
#define AM_DEVICES_MB85RC1MT_DEVICE_ID       0x58A700

#define AM_DEVICES_MB85RC1MT_CMD_RING_LENGTH   256
#define AM_DEVICES_MB85RC1MT_RESP_RING_LENGTH  256
#define AM_DEVICES_MB85RC1MT_SCL_LOW_CNT(x)    16*x
#define AM_DEVICES_MB85RC1MT_SCL_HIGH_CNT(x)   16*x

#define AM_DEVICES_MB85RC1MT_DMA_DELAY_US      100
#define AM_DEVICES_MB85RC1MT_DMA_TIMEOUT_CNT   10000
//! @}

//*****************************************************************************
//
// Global type definitions.
//
//*****************************************************************************
typedef enum
{
    AM_DEVICES_MB85RC1MT_STATUS_SUCCESS,
    AM_DEVICES_MB85RC1MT_STATUS_ERROR,
    AM_DEVICES_MB85RC1MT_STATUS_INVALID_HANDLE,
    AM_DEVICES_MB85RC1MT_STATUS_IN_USE,
    AM_DEVICES_MB85RC1MT_STATUS_TIMEOUT,
    AM_DEVICES_MB85RC1MT_STATUS_OUT_OF_RANGE,
    AM_DEVICES_MB85RC1MT_STATUS_INVALID_ARG,
} am_devices_i3c_mb85rc1mt_status_e;

typedef struct
{
    uint32_t *pCmdRingBuf;
    uint32_t ui32CmdRingBufLen;
    uint32_t *pRespRingBuf;
    uint32_t ui32RespRingBufLen;

    uint32_t (*pfnI3cTimeoutCallback)(bool *pDMAComplete);
    bool *pDMAComplete;
} am_devices_i3c_mb85rc1mt_dma_config_t;
typedef struct
{
    // Host config
    am_hal_i3c_xfer_mode_e   eTransferMode;
    am_hal_i3c_speed_mode_e  eSpeedMode;

    //Device config
    uint8_t                  ui8DynamicAddr;
    am_devices_i3c_mb85rc1mt_dma_config_t sI3cDmaCfg;
} am_devices_i3c_mb85rc1mt_config_t;

//
//  Typedef for I3C device callback function
//
typedef uint32_t (*am_devices_i3c_cb_t)(void *pCallbackCtxt);

//*****************************************************************************
//
// External function definitions.
//
//*****************************************************************************

//*****************************************************************************
//
//! @brief Initialize the mb85rc1mt device.
//!
//! @param ui32Module  - I3C instance
//! @param pI3cHandle  - I3C controller handle
//! @param ppDevHandle - Device handle which needs to be return
//! @param pDevConfig  - I3C device structure describing the target device
//!
//! @note This function should be called before any other am_devices_i3c_mb85rc1mt
//! functions. It is used to tell the other functions how to communicate
//! with the I3C devices.
//!
//! @return Status.
//
//*****************************************************************************
extern uint32_t am_devices_i3c_mb85rc1mt_init(uint32_t ui32Module,
                                              void *pI3cHandle,
                                              void **ppDevHandle,
                                              am_devices_i3c_mb85rc1mt_config_t *pDevConfig);

//*****************************************************************************
//
//! @brief De-Initialize the mb85rc1mt driver.
//!
//! @param pHandle     - Pointer to device handle
//!
//! This function reverses the initialization
//!
//! @return Status.
//
//*****************************************************************************
extern uint32_t am_devices_i3c_mb85rc1mt_term(void *pHandle);

//*****************************************************************************
//
//! @brief Programs data into mb85rc1mt device based on sub address.
//!
//! @param pHandle        - Pointer to device handle
//! @param ui16WriteAddr  - Address of desired data in device
//! @param pData          - Buffer to write the device data from
//! @param ui32NumBytes   - Number of bytes to write to device
//!
//! This function writes data to the given sub address of device, and returns
//! the result as an 32-bit unsigned integer value.
//!
//! @return 32-bit status
//
//*****************************************************************************
extern uint32_t am_devices_i3c_mb85rc1mt_blocking_write(void *pHandle,
                                                        uint16_t ui16WriteAddr,
                                                        uint32_t *pData,
                                                        uint32_t ui32NumBytes);

//*****************************************************************************
//
//! @brief Read data from mb85rc1mt device based on sub address.
//!
//! @param pHandle        - Pointer to device handle
//! @param ui16SubAddr    - Address of desired data in device
//! @param pData          - Buffer to store the received data from device
//! @param ui32NumBytes   - Number of bytes to read from device
//!
//! This function reads data from the sub address of device, and returns
//! the result as an 32-bit unsigned integer value.
//!
//! @return 32-bit status
//
//*****************************************************************************
extern uint32_t am_devices_i3c_mb85rc1mt_blocking_read(void *pHandle,
                                                       uint16_t ui16SubAddr,
                                                       uint32_t *pData,
                                                       uint32_t ui32NumBytes);

//*****************************************************************************
//
//! @brief Programs data into mb85rc1mt device based on sub address.
//!
//! @param pHandle        - Pointer to device handle
//! @param ui32SubAddr    - Address of desired data in device
//! @param pData          - Buffer to write the device data from
//! @param ui32NumBytes   - Number of bytes to write to device
//!
//! This function writes data to the given sub address of device via DMA mode.
//! and returns the result as an 32-bit unsigned integer value.
//!
//! @return 32-bit status
//
//*****************************************************************************
extern uint32_t am_devices_i3c_mb85rc1mt_nonblocking_write(void *pHandle,
                                                           uint32_t ui32SubAddr,
                                                           uint32_t *pData,
                                                           uint32_t ui32NumBytes);

//*****************************************************************************
//
//! @brief Read data from mb85rc1mt device based on sub address.
//!
//! @param pHandle        - Pointer to device handle
//! @param ui32SubAddr    - Address of desired data in device
//! @param pData          - Buffer to store the received data from device
//! @param ui32NumBytes   - Number of bytes to read from device
//!
//! This function reads data from the sub address of device via DMA mode,
//! and returns the result as an 32-bit unsigned integer value.
//!
//! @return 32-bit status
//
//*****************************************************************************
extern uint32_t am_devices_i3c_mb85rc1mt_nonblocking_read(void *pHandle,
                                                          uint32_t ui32SubAddr,
                                                          uint32_t *pData,
                                                          uint32_t ui32NumBytes);

#ifdef __cplusplus
}
#endif

#endif // AM_DEVICES_I3C_MB85RC1MT_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
