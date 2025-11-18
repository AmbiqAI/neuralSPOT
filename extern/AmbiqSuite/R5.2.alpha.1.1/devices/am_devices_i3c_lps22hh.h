//*****************************************************************************
//
//! @file am_devices_i3c_lps22hh.c
//!
//! @brief ST Microelectronics LPS22HH Pressure and Temperature Sensor Driver with I3C Interface.
//!
//! @addtogroup devices_i3c_lps22hh LPS22HH I3C Pressure/Temperature Sensor Driver
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

#ifndef AM_DEVICES_I3C_LPS22HH_H
#define AM_DEVICES_I3C_LPS22HH_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! @name Global definitions for the lps22hh sensor
//! @{
//
//*****************************************************************************
#define AM_DEVICES_LPS22HH_MAX_DEVICE_NUM    1

#define AM_DEVICES_LPS22HH_DEVICE_ID         0xB3
#define AM_DEVICES_LPS22HH_STATIC_ADDRESS_1  0x5C
#define AM_DEVICES_LPS22HH_STATIC_ADDRESS_2  0x5D
#define AM_DEVICES_LPS22HH_DYNAMIC_ADDRESS   0x22
#define AM_DEVICES_LPS22HH_MAX_WRITE_LENGTH  0x40FF
#define AM_DEVICES_LPS22HH_MAX_READ_LENGTH   0x2020FF
#define AM_DEVICES_LPS22HH_DMA_DELAY_US      100

#define AM_DEVICES_LPS22HH_WHO_AMI_I         0x0F
#define AM_DEVICES_LPS22HH_CTRL_REG1         0x10U
#define AM_DEVICES_LPS22HH_CTRL_REG2         0x11U
#define AM_DEVICES_LPS22HH_CTRL_REG3         0x12U
#define AM_DEVICES_LPS22HH_FIFO_CTRL         0x13U
#define AM_DEVICES_LPS22HH_FIFO_WTM          0x14U
#define AM_DEVICES_LPS22HH_INT_SOURCE        0x24U
#define AM_DEVICES_LPS22HH_FIFO_STATUS1      0x25U
#define AM_DEVICES_LPS22HH_FIFO_STATUS2      0x26U
#define AM_DEVICES_LPS22HH_STATUS            0x27U
#define AM_DEVICES_LPS22HH_PRESS_OUT_XL      0x28U
#define AM_DEVICES_LPS22HH_PRESS_OUT_L       0x29U
#define AM_DEVICES_LPS22HH_PRESS_OUT_H       0x2AU
#define AM_DEVICES_LPS22HH_TEMP_OUT_L        0x2BU
#define AM_DEVICES_LPS22HH_TEMP_OUT_H        0x2CU

#define AM_DEVICES_LPS22HH_RESET_MSK         0x4U
#define AM_DEVICES_LPS22HH_BLOCK_DATA_MSK    0x2U

#define AM_DEVICES_LPS22HH_PRESSURE_DATA_AVALIABLE 0x1U
#define AM_DEVICES_LPS22HH_TEMP_DATA_AVALIABLE     0x2U
#define AM_DEVICES_LPS22HH_TRANSFER_DELAY_TIME     200
#define AM_DEVICES_LPS22HH_DMA_TIMEOUT_CNT         1000
//! @}

//*****************************************************************************
//
// Global type definitions.
//
//*****************************************************************************
typedef enum
{
    AM_DEVICES_LPS22HH_STATUS_SUCCESS,
    AM_DEVICES_LPS22HH_STATUS_ERROR,
    AM_DEVICES_LPS22HH_STATUS_INVALID_HANDLE,
    AM_DEVICES_LPS22HH_STATUS_IN_USE,
    AM_DEVICES_LPS22HH_STATUS_TIMEOUT,
    AM_DEVICES_LPS22HH_STATUS_OUT_OF_RANGE,
    AM_DEVICES_LPS22HH_STATUS_INVALID_ARG,
} am_devices_i3c_lps22hh_status_t;

typedef struct
{
    uint32_t *pCmdRingBuf;
    uint32_t ui32CmdRingBufLen;
    uint32_t *pRespRingBuf;
    uint32_t ui32RespRingBufLen;

    uint32_t (*pfnI3cTimeoutCallback)(bool *pDMAComplete);
    bool *pDMAComplete;
} am_devices_i3c_lps22hh_dma_config_t;

typedef struct
{
    // Host config
    am_hal_i3c_xfer_mode_e   eTransferMode;
    am_hal_i3c_speed_mode_e  eSpeedMode;

    uint8_t  ui8DynamicAddr;
    am_devices_i3c_lps22hh_dma_config_t sI3cDmaCfg;
} am_devices_i3c_lps22hh_config_t;

typedef enum
{
  AM_DEVICES_LPS22HH_POWER_DOWN      = 0x00,
  AM_DEVICES_LPS22HH_ONE_SHOOT       = 0x08,
  AM_DEVICES_LPS22HH_1_Hz            = 0x01,
  AM_DEVICES_LPS22HH_10_Hz           = 0x02,
  AM_DEVICES_LPS22HH_25_Hz           = 0x03,
  AM_DEVICES_LPS22HH_50_Hz           = 0x04,
  AM_DEVICES_LPS22HH_75_Hz           = 0x05,
  AM_DEVICES_LPS22HH_1_Hz_LOW_NOISE  = 0x11,
  AM_DEVICES_LPS22HH_10_Hz_LOW_NOISE = 0x12,
  AM_DEVICES_LPS22HH_25_Hz_LOW_NOISE = 0x13,
  AM_DEVICES_LPS22HH_50_Hz_LOW_NOISE = 0x14,
  AM_DEVICES_LPS22HH_75_Hz_LOW_NOISE = 0x15,
  AM_DEVICES_LPS22HH_100_Hz          = 0x06,
  AM_DEVICES_LPS22HH_200_Hz          = 0x07,
} am_devices_i3c_lps22hh_odr_t;

//*****************************************************************************
//
// External function definitions.
//
//*****************************************************************************

//*****************************************************************************
//
//! @brief Initialize the lps22hh sensor.
//!
//! @param ui32Module  - I3C instance
//! @param pI3cHandle  - I3C controller handle
//! @param ppDevHandle - Device handle which needs to be return
//! @param pDevConfig  - I3C device structure describing the target sensor
//!
//! @note This function should be called before any other am_devices_i3c_lps22hh
//! functions. It is used to tell the other functions how to communicate
//! with the I3C sensors.
//!
//! @return Status.
//
//*****************************************************************************
extern uint32_t am_devices_i3c_lps22hh_init(uint32_t ui32Module,
                                            void *pI3cHandle,
                                            void **ppDevHandle,
                                            am_devices_i3c_lps22hh_config_t *pDevConfig);

//*****************************************************************************
//
//! @brief De-Initialize the lps22hh driver.
//!
//! @param pHandle     - Pointer to device handle
//!
//! This function reverses the initialization
//!
//! @return Status.
//
//*****************************************************************************
extern uint32_t am_devices_i3c_lps22hh_term(void *pHandle);

//*****************************************************************************
//
//! @brief Send I3C Common Command Codes
//!
//! @param pHandle       - Pointer to device handle
//! @param pui32Data     - Buffer to config sensor or save current configuration
//! @param ui8CccId      - Common command code index
//!
//! This function sends I3C CCCs to sensor to configure or get sensor information
//
//! @return 32-bit status
//
//*****************************************************************************
extern uint32_t am_devices_i3c_lps22hh_send_cccs(void *pHandle,
                                                 uint32_t *pui32Data,
                                                 uint8_t ui8CccId);

//*****************************************************************************
//
//! @brief Write generic device register of lps22hh sensor.
//!
//! @param pHandle        - Pointer to device handle
//! @param ui8SubAddress  - Address of desired data in sensor
//! @param pData          - Buffer to write the sensor data from
//! @param ui32NumBytes   - Number of bytes to write to sensor
//!
//! This function write data to the given register of sensor, and returns
//! the result as an 32-bit unsigned integer value.
//!
//! @return 32-bit status
//
//*****************************************************************************
extern uint32_t am_devices_i3c_lps22hh_write_reg(void *pHandle,
                                                 uint8_t ui8SubAddress,
                                                 uint32_t *pData,
                                                 uint32_t ui32NumBytes);

//*****************************************************************************
//
//! @brief Read generic device register from lps22hh sensor.
//!
//! @param pHandle        - Pointer to device handle
//! @param ui8SubAddress  - Address of desired data in sensor
//! @param pData          - Buffer to store the received data from sensor
//! @param ui32NumBytes   - Number of bytes to read from sensor
//!
//! This function read data from the given register of sensor, and returns
//! the result as an 32-bit unsigned integer value.
//!
//! @return 32-bit status
//
//*****************************************************************************
extern uint32_t am_devices_i3c_lps22hh_read_reg(void *pHandle,
                                                uint8_t ui8SubAddress,
                                                uint32_t *pData,
                                                uint32_t ui32NumBytes);

//*****************************************************************************
//
//! @brief Get the pressure from lps22hh sensor.
//!
//! @param pHandle        - Pointer to device handle
//! @param pData          - The actual pressure data detected by sensor
//!
//! This function read data from the sensor pressure registers, and caculate
//! the pressure data based on registers data.
//!
//! @return 32-bit status
//
//*****************************************************************************
extern uint32_t am_devices_i3c_lps22hh_get_pressure(void *pHandle,
                                                    float32_t *pData);

//*****************************************************************************
//
//! @brief Get the temperature from lps22hh sensor.
//!
//! @param pHandle        - Pointer to device handle
//! @param pData          - The actual temperature data detected by sensor
//!
//! This function read data from the sensor temperature registers, and caculate
//! the temperature data based on registers data.
//!
//! @return 32-bit status
//
//*****************************************************************************
extern uint32_t am_devices_i3c_lps22hh_get_temperature(void *pHandle,
                                                       float32_t *pData);

#ifdef __cplusplus
}
#endif

#endif // AM_DEVICES_I3C_LPS22HH_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

