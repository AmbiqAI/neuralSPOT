//*****************************************************************************
//
//! @file am_devices_i3c_lsm6dso.c
//!
//! @brief ST Microelectronics LSM6DSO IMU Sensor Driver with I3C Interface.
//!
//! @addtogroup devices_i3c_lsm6dso LSM6DSO I3C IMU Sensor Driver
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

#ifndef AM_DEVICES_I3C_LSM6DSO_H
#define AM_DEVICES_I3C_LSM6DSO_H

#include "am_bsp.h"

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! @name Global definitions for the lsm6dso sensor
//! @{
//
//*****************************************************************************
#define AM_DEVICES_LSM6DSO_MAX_DEVICE_NUM    1

#define AM_DEVICES_LSM6DSO_STATIC_ADDR1      0x6A
#define AM_DEVICES_LSM6DSO_STATIC_ADDR2      0x6B

#define AM_DEVICES_LSM6DSO_DEVICE_ID         0x6C
#define AM_DEVICES_LSM6DSO_STATIC_ADDRESS    0x6A

#define AM_DEVICES_LSM6DSO_WHO_AMI_I         0x0F
#define AM_DEVICES_LSM6DSO_CTRL_REG1         0x10U
typedef struct
{
    uint8_t not_used_01              : 1;
    uint8_t lpf2_xl_en               : 1;
    uint8_t fs_xl                    : 2;
    uint8_t odr_xl                   : 4;
} lsm6dso_ctrl1_xl_t;

#define AM_DEVICES_LSM6DSO_CTRL_REG2         0x11U
typedef struct
{
    uint8_t not_used_01              : 1;
    uint8_t fs_g                     : 3; /* fs_125 + fs_g */
    uint8_t odr_g                    : 4;
} lsm6dso_ctrl2_g_t;

#define AM_DEVICES_LSM6DSO_CTRL_REG3         0x12U
typedef struct
{
    uint8_t sw_reset                 : 1;
    uint8_t not_used_01              : 1;
    uint8_t if_inc                   : 1;
    uint8_t sim                      : 1;
    uint8_t pp_od                    : 1;
    uint8_t h_lactive                : 1;
    uint8_t bdu                      : 1;
    uint8_t boot                     : 1;
} lsm6dso_ctrl3_c_t;

#define AM_DEVICES_LSM6DSO_FIFO_STATUS1      0x3AU
#define AM_DEVICES_LSM6DSO_FIFO_STATUS2      0x3BU
#define AM_DEVICES_LSM6DSO_STATUS            0x1EU
typedef struct
{
    uint8_t xlda                     : 1;
    uint8_t gda                      : 1;
    uint8_t tda                      : 1;
    uint8_t not_used_01              : 5;
} lsm6dso_status_reg_t;

#define AM_DEVICES_LSM6DSO_OUT_TEMP_L        0x20U
#define AM_DEVICES_LSM6DSO_OUT_TEMP_H        0x21U
#define AM_DEVICES_LSM6DSO_OUTX_L_G          0x22U
#define AM_DEVICES_LSM6DSO_OUTX_H_G          0x23U
#define AM_DEVICES_LSM6DSO_OUTY_L_G          0x24U
#define AM_DEVICES_LSM6DSO_OUTY_H_G          0x25U
#define AM_DEVICES_LSM6DSO_OUTZ_L_G          0x26U
#define AM_DEVICES_LSM6DSO_OUTZ_H_G          0x27U
#define AM_DEVICES_LSM6DSO_OUTX_L_A          0x28U
#define AM_DEVICES_LSM6DSO_OUTX_H_A          0x29U
#define AM_DEVICES_LSM6DSO_OUTY_L_A          0x2AU
#define AM_DEVICES_LSM6DSO_OUTY_H_A          0x2BU
#define AM_DEVICES_LSM6DSO_OUTZ_L_A          0x2CU
#define AM_DEVICES_LSM6DSO_OUTZ_H_A          0x2DU

#define AM_DEVICES_LSM6DSO_DMA_TIMEOUT_CNT   10000
//! @}

//*****************************************************************************
//
// Global type definitions.
//
//*****************************************************************************
typedef enum
{
    AM_DEVICES_LSM6DSO_STATUS_SUCCESS,
    AM_DEVICES_LSM6DSO_STATUS_ERROR,
    AM_DEVICES_LSM6DSO_STATUS_INVALID_HANDLE,
    AM_DEVICES_LSM6DSO_STATUS_IN_USE,
    AM_DEVICES_LSM6DSO_STATUS_TIMEOUT,
    AM_DEVICES_LSM6DSO_STATUS_OUT_OF_RANGE,
    AM_DEVICES_LSM6DSO_STATUS_INVALID_ARG,
} am_devices_i3c_lsm6dso_status_t;

typedef struct
{

    uint32_t       *pCmdRingBuf;
    uint32_t       ui32CmdRingBufLen;
    uint32_t       *pRespRingBuf;
    uint32_t       ui32RespRingBufLen;

    uint32_t (*pfnI3cTimeoutCallback)(bool *pDMAComplete);

    bool *pDMAComplete;

} am_devices_i3c_lsm6dso_dma_config_t;

typedef struct
{
    // Host config
    am_hal_i3c_xfer_mode_e              eTransferMode;
    am_hal_i3c_speed_mode_e             eSpeedMode;

    //Device config
    uint8_t                             ui8DynamicAddr;

    am_devices_i3c_lsm6dso_dma_config_t sI3cDmaCfg;


} am_devices_i3c_lsm6dso_config_t;

typedef enum
{
    AM_DEVICES_LSM6DSO_ODR_OFF    = 0,
    AM_DEVICES_LSM6DSO_ODR_12Hz5  = 1,
    AM_DEVICES_LSM6DSO_ODR_26Hz   = 2,
    AM_DEVICES_LSM6DSO_ODR_52Hz   = 3,
    AM_DEVICES_LSM6DSO_ODR_104Hz  = 4,
    AM_DEVICES_LSM6DSO_ODR_208Hz  = 5,
    AM_DEVICES_LSM6DSO_ODR_417Hz  = 6,
    AM_DEVICES_LSM6DSO_ODR_833Hz  = 7,
    AM_DEVICES_LSM6DSO_ODR_1667Hz = 8,
    AM_DEVICES_LSM6DSO_ODR_3333Hz = 9,
    AM_DEVICES_LSM6DSO_ODR_6667Hz = 10,
    AM_DEVICES_LSM6DSO_ODR_1Hz6   = 11,  //In high performance is 12Hz5
} am_devices_i3c_lsm6dso_odr_t;

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
//! @brief Initialize the lsm6dso sensor.
//!
//! @param ui32Module  - I3C instance
//! @param pI3cHandle  - I3C controller handle
//! @param ppDevHandle - Device handle which needs to be return
//! @param pDevConfig  - I3C device structure describing the target sensor
//!
//! @note This function should be called before any other am_devices_i3c_lsm6dso
//! functions. It is used to tell the other functions how to communicate
//! with the I3C sensors.
//!
//! @return Status.
//
//*****************************************************************************
extern uint32_t am_devices_i3c_lsm6dso_init(uint32_t ui32Module,
                                            void *pI3cHandle,
                                            void **ppDevHandle,
                                            am_devices_i3c_lsm6dso_config_t *pDevConfig);

//*****************************************************************************
//
//! @brief De-Initialize the lsm6dso driver.
//!
//! @param pHandle     - Pointer to device handle
//!
//! This function reverses the initialization
//!
//! @return Status.
//
//*****************************************************************************
extern uint32_t am_devices_i3c_lsm6dso_term(void *pHandle);

//*****************************************************************************
//
//! @brief Send I3C Common Command Codes
//!
//! @param pHandle     - Pointer to device handle
//! @param pui32Data   - Buffer to config sensor or save current configuration
//! @param ui8CccId    - Common command code index
//!
//! This function sends I3C CCCs to sensor to configure or get sensor information
//
//! @return 32-bit status
//
//*****************************************************************************
extern uint32_t am_devices_i3c_lsm6dso_send_cccs(void *pHandle,
                                                 uint32_t *pui32Data,
                                                 uint8_t ui8CccId);


//*****************************************************************************
//
//! @brief Write generic device register of lsm6dso sensor.
//!
//! @param pHandle       - Pointer to device handle
//! @param ui8WriteAddr  - Address of desired data in sensor
//! @param pui32Data     - Buffer to write the sensor data from
//! @param ui32NumBytes  - Number of bytes to write to sensor
//!
//! This function write data to the given register of sensor, and returns
//! the result as an 32-bit unsigned integer value.
//!
//! @return 32-bit status
//
//*****************************************************************************
extern uint32_t am_devices_i3c_lsm6dso_write_reg(void *pHandle,
                                                 uint8_t ui8WriteAddr,
                                                 uint32_t *pui32Data,
                                                 uint32_t ui32NumBytes);

//*****************************************************************************
//
//! @brief Read generic device register from lsm6dso sensor.
//!
//! @param pHandle        - Pointer to device handle
//! @param ui8ReadAddr    - Address of desired data in sensor
//! @param pui32Data      - Buffer to store the received data from sensor
//! @param ui32NumBytes   - Number of bytes to read from sensor
//! @param pfnCallback    - I3C device call back function
//! @param pCallbackCtxt  - I3C device callback context
//!
//! This function read data from the given register of sensor, and returns
//! the result as an 32-bit unsigned integer value.
//!
//! @return 32-bit status
//
//*****************************************************************************
extern uint32_t am_devices_i3c_lsm6dso_read_reg(void *pHandle,
                                                uint8_t ui8ReadAddr,
                                                uint32_t *pui32Data,
                                                uint32_t ui32NumBytes);

//*****************************************************************************
//
//! @brief Set the maximum write length via direct CCC
//!
//! @param pHandle        - Pointer to device handle
//! @param ui16WriteLen   - maximum write length
//!
//! This function read data from the sensor registers
//!
//! @return 32-bit status
//
//*****************************************************************************
extern uint32_t am_devices_i3c_lsm6dso_write_len_set(void *pHandle,
                                                     uint16_t ui16WriteLen);

//*****************************************************************************
//
//! @brief Set the maximum read length via direct CCC
//!
//! @param pHandle        - Pointer to device handle
//! @param ui16ReadLen    - maximum read length
//!
//! This function read data from the sensor registers
//!
//! @return 32-bit status
//
//*****************************************************************************
extern uint32_t am_devices_i3c_lsm6dso_read_len_set(void *pHandle,
                                                    uint16_t ui16ReadLen);

//*****************************************************************************
//
//! @brief Get the status from lsm6dso sensor.
//!
//! @param pHandle        - Pointer to device handle
//! @param pui32Data      - sensor data ready status
//!
//! This function read data from the sensor registers
//!
//! @return 32-bit status
//
//*****************************************************************************
extern uint32_t am_devices_i3c_lsm6dso_get_status(void *pHandle,
                                                  lsm6dso_status_reg_t *pui32Data);

//*****************************************************************************
//
//! @brief Get the acceleration data from lsm6dso sensor.
//!
//! @param pHandle        - Pointer to device handle
//! @param pui16Data      - Pointer to the 6 byte accelerometer data
//!
//! This function read data from the sensor registers
//!
//! @return 32-bit status
//
//*****************************************************************************
extern uint32_t am_devices_i3c_lsm6dso_acceleration_raw_get(void *pHandle,
                                                            int16_t *pui16Data);

//*****************************************************************************
//
//! @brief Get the angular data from lsm6dso sensor.
//!
//! @param pHandle        - Pointer to device handle
//! @param pui16Data      - Pointer to the 6 byte gyroscope data
//!
//! This function read data from the sensor registers
//!
//! @return 32-bit status
//
//*****************************************************************************
extern uint32_t am_devices_i3c_lsm6dso_angular_rate_raw_get(void *pHandle,
                                                            int16_t *pui16Data);

//*****************************************************************************
//
//! @brief Get the temperature from lsm6dso sensor.
//!
//! @param pHandle        - Pointer to device handle
//! @param pui16Data      - Pointer to the 6 byte gyroscope data
//!
//! This function read data from the sensor registers
//!
//! @return 32-bit status
//
//*****************************************************************************
extern uint32_t am_devices_i3c_lsm6dso_temperature_raw_get(void *pHandle,
                                                           int16_t *pui16Data);

#ifdef __cplusplus
}
#endif

#endif // AM_DEVICES_I3C_LSM6DSO_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
