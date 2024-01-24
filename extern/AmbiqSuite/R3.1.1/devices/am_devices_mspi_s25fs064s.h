//*****************************************************************************
//
//! @file am_devices_mspi_s25fs064s.h
//!
//! @brief General Multibit SPI S25fs064s driver.
//!
//! @addtogroup mspi_s25fs064s S25FS064S MSPI Display Driver
//! @ingroup devices
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2023, Ambiq Micro, Inc.
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
// This is part of revision release_sdk_3_1_1-10cda4b5e0 of the AmbiqSuite Development Package.
//
//*****************************************************************************

#ifndef AM_DEVICES_MSPI_S25FS064S_H
#define AM_DEVICES_MSPI_S25FS064S_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! @name Global definitions for s25fs064s commands
//! @{
//
//*****************************************************************************
#define AM_DEVICES_MSPI_S25FS064S_WRITE_STATUS      0x01
#define AM_DEVICES_MSPI_S25FS064S_PAGE_PROGRAM      0x02
#define AM_DEVICES_MSPI_S25FS064S_READ              0x03
#define AM_DEVICES_MSPI_S25FS064S_WRITE_DISABLE     0x04
#define AM_DEVICES_MSPI_S25FS064S_READ_STATUS       0x05
#define AM_DEVICES_MSPI_S25FS064S_WRITE_ENABLE      0x06
#define AM_DEVICES_MSPI_S25FS064S_FAST_READ         0x0B
#define AM_DEVICES_MSPI_S25FS064S_READ_4B           0x13
#define AM_DEVICES_MSPI_S25FS064S_SUBSECTOR_ERASE   0x20
#define AM_DEVICES_MSPI_S25FS064S_DUAL_READ         0x3B
#define AM_DEVICES_MSPI_S25FS064S_DUAL_IO_READ      0xBB
#define AM_DEVICES_MSPI_S25FS064S_WRITE_ENHVOL_CFG  0x61
#define AM_DEVICES_MSPI_S25FS064S_RESET_ENABLE      0x66
#define AM_DEVICES_MSPI_S25FS064S_QUAD_READ         0x6B
#define AM_DEVICES_MSPI_S25FS064S_WRITE_VOL_CFG     0x81
#define AM_DEVICES_MSPI_S25FS064S_RESET_MEMORY      0x99
#define AM_DEVICES_MSPI_S25FS064S_READ_ID           0x9F
#define AM_DEVICES_MSPI_S25FS064S_ENTER_4B          0xB7
#define AM_DEVICES_MSPI_S25FS064S_BULK_ERASE        0xC7
#define AM_DEVICES_MSPI_S25FS064S_SECTOR_ERASE      0xD8
#define AM_DEVICES_MSPI_S25FS064S_EXIT_4B           0xE9
#define AM_DEVICES_MSPI_S25FS064S_QUAD_IO_READ      0xEB
#define AM_DEVICES_MSPI_S25FS064S_READ_QUAD_4B      0xEC
//! @}

//*****************************************************************************
//
//! @name Global definitions for the s25fs064s status register
//! @{
//
//*****************************************************************************
#define AM_DEVICES_MSPI_S25FS064S_WEL         0x00000002        // Write enable latch
#define AM_DEVICES_MSPI_S25FS064S_WIP         0x00000001        // Write in progress
//! @}

//*****************************************************************************
//
//! @name Device specific identification.
//! @{
//
//*****************************************************************************
#define AM_DEVICES_MSPI_S25FS064S_ID        0x00170201
#define AM_DEVICES_MSPI_S25FS064S_ID_MASK   0x00FFFFFF
//! @}

//*****************************************************************************
//
//! @name Device specific definitions for s25fs064s commands
//! @{
//
//*****************************************************************************
#define AM_DEVICES_MSPI_S25FS064S_READ_STATUS2              0x07
#define AM_DEVICES_MSPI_S25FS064S_WRITE_ANY_REG             0x71
#define AM_DEVICES_MSPI_S25FS064S_EVAL_ERASE_STATUS         0xD0
//! @}

//*****************************************************************************
//
//! @name Device specific definitions for the Configuration register(s)
//! @{
//
//*****************************************************************************
#define AM_DEVICES_MSPI_S25FS064S_QUAD_MODE           (0x02)
#define AM_DEVICES_MSPI_S25FS064S_DUAL_MODE           (0x00)
#define AM_DEVICES_MSPI_S25FS064S_SERIAL_MODE         (0x00)
#define AM_DEVICES_MSPI_S25FS064S_CR1V                (0x00020080)
#define AM_DEVICES_MSPI_S25FS064S_CR1V_VALUE(n)       (((uint32_t)(n) << 24) & 0xFF000000)
#define AM_DEVICES_MSPI_S25FS064S_CR2V                (0x00030080)
#define AM_DEVICES_MSPI_S25FS064S_CR2V_VALUE(n)       (((uint32_t)(n) << 24) & 0xFF000000)
#define AM_DEVICES_MSPI_S25FS064S_ADDR_QUAD_EXT_DELAY (2)
#define AM_DEVICES_MSPI_S25FS064S_ADDR_DUAL_EXT_DELAY (4)
#define AM_DEVICES_MSPI_S25FS064S_CR3V                (0x00040080)
#define AM_DEVICES_MSPI_S25FS064S_CR4V                (0x00050080)
#define AM_DEVICES_MSPI_S25FS064S_CR3V_VALUE          (((uint32_t)(0x08) << 24) & 0xFF000000)
#define AM_DEVICES_MSPI_S25FS064S_CR4V_VALUE          (((uint32_t)(0x10) << 24) & 0xFF000000)
#define AM_DEVICES_MSPI_S25FS064S_ERASE_SUCCESS       (0x04)
//! @}

//*****************************************************************************
//
//! @name Device specific definitions for the s25fs064s size information
//! @{
//
//*****************************************************************************
#define AM_DEVICES_MSPI_S25FS064S_PAGE_SIZE       0x100    //256 bytes, minimum program unit
#define AM_DEVICES_MSPI_S25FS064S_SUBSECTOR_SIZE  0x1000   //4K bytes
#define AM_DEVICES_MSPI_S25FS064S_SECTOR_SIZE     0x10000  //64K bytes.
#define AM_DEVICES_MSPI_S25FS064S_MAX_SECTORS     128      // Sectors within 3-byte address range.

//! @}
//*****************************************************************************
//
//! @name Global definitions for the MSPI instance to use.
//! @{
//
//*****************************************************************************
#define AM_DEVICES_MSPI_S25FS064S_MSPI_INSTANCE     0
#define AM_DEVICES_MSPI_S25FS064S_MAX_DEVICE_NUM    1
//! @}

//*****************************************************************************
//
// Global type definitions.
//
//*****************************************************************************
typedef enum
{
    AM_DEVICES_MSPI_S25FS064S_STATUS_SUCCESS,
    AM_DEVICES_MSPI_S25FS064S_STATUS_ERROR
} am_devices_mspi_s25fs064s_status_t;

typedef struct
{
    am_hal_mspi_device_e eDeviceConfig;
    am_hal_mspi_clock_e eClockFreq;
    uint32_t *pNBTxnBuf;
    uint32_t ui32NBTxnBufLength;
    uint32_t ui32ScramblingStartAddr;
    uint32_t ui32ScramblingEndAddr;
} am_devices_mspi_s25fs064s_config_t;

//*****************************************************************************
//
// External function definitions.
//
//*****************************************************************************
//*****************************************************************************
//
//! @brief Initialize the mspi_s25fs064s driver.
//!
//! @param ui32Module
//! @param psMSPISettings
//! @param ppHandle
//! @param ppMspiHandle
//!
//! @return
//
//*****************************************************************************
extern uint32_t am_devices_mspi_s25fs064s_init(uint32_t ui32Module,
                                           am_devices_mspi_s25fs064s_config_t *psMSPISettings,
                                           void **ppHandle, void **ppMspiHandle);

//*****************************************************************************
//
//! @brief De-Initialization the mspi_s25fs064s driver.
//!
//! @param pHandle  - Pointer to device driver handle
//!
//! @return
//
//*****************************************************************************
extern uint32_t am_devices_mspi_s25fs064s_deinit(void *pHandle);

//*****************************************************************************
//
//! @brief Reads the ID of the external s25fs064s and returns the value.
//!
//! @param pHandle  - Pointer to device driver handle
//
//*****************************************************************************
extern uint32_t am_devices_mspi_s25fs064s_id(void *pHandle);

//*****************************************************************************
//
//! @brief Reads the current status of the external s25fs064s
//!
//! @param pHandle  - Pointer to device driver handle
//
//*****************************************************************************
extern uint32_t am_devices_mspi_s25fs064s_reset(void *pHandle);

//*****************************************************************************
//
//! @brief Reads the current status of the external s25fs064s
//!
//! @param pHandle  - Pointer to device driver handle
//! @param pStatus
//!
//! @return
//
//*****************************************************************************
extern uint32_t am_devices_mspi_s25fs064s_status(void *pHandle, uint32_t *pStatus);

//*****************************************************************************
//
//! @brief Reads the contents of the external s25fs064s into a buffer.
//!
//! @param pHandle  - Pointer to device driver handle
//! @param pui8RxBuffer
//! @param ui32ReadAddress
//! @param ui32NumBytes
//! @param bWaitForCompletion
//!
//! @return
//
//*****************************************************************************
extern uint32_t am_devices_mspi_s25fs064s_read(void *pHandle, uint8_t *pui8RxBuffer,
                                           uint32_t ui32ReadAddress,
                                           uint32_t ui32NumBytes,
                                           bool bWaitForCompletion);

//*****************************************************************************
//
//! @brief Programs the given range of s25fs064s addresses.
//!
//! @param pHandle  - Pointer to device driver handle
//! @param pui8TxBuffer
//! @param ui32WriteAddress
//! @param ui32NumBytes
//! @param bWaitForCompletion
//!
//! @return
//
//*****************************************************************************
extern uint32_t am_devices_mspi_s25fs064s_write(void *pHandle, uint8_t *pui8TxBuffer,
                                            uint32_t ui32WriteAddress,
                                            uint32_t ui32NumBytes,
                                            bool bWaitForCompletion);

//*****************************************************************************
//
//! @brief Erases the entire contents of the external s25fs064s
//!
//! @param pHandle  - Pointer to device driver handle
//!
//! @return
//
//*****************************************************************************
extern uint32_t am_devices_mspi_s25fs064s_mass_erase(void *pHandle);

//*****************************************************************************
//
//! @brief Erases the contents of a single sector of s25fs064s
//!
//! @param pHandle  - Pointer to device driver handle
//! @param ui32SectorAddress
//!
//! @return
//
//*****************************************************************************
extern uint32_t am_devices_mspi_s25fs064s_sector_erase(void *pHandle, uint32_t ui32SectorAddress);

//*****************************************************************************
//
//! @brief Sets up the MSPI and external S25FS064S into XIP mode.
//!
//! @param pHandle  - Pointer to device driver handle
//!
//! @return
//
//*****************************************************************************
extern uint32_t am_devices_mspi_s25fs064s_enable_xip(void *pHandle);

//*****************************************************************************
//
//! @brief  Removes the MSPI and external S25FS064S from XIP mode.
//!
//! @param pHandle
//!
//! @return
//
//*****************************************************************************
extern uint32_t am_devices_mspi_s25fs064s_disable_xip(void *pHandle);

//*****************************************************************************
//
//! @brief Sets up the MSPI and external S25FS064S into scrambling mode.
//!
//! @param pHandle
//!
//! @return
//
//*****************************************************************************
extern uint32_t am_devices_mspi_s25fs064s_enable_scrambling(void *pHandle);

//*****************************************************************************
//
//! @brief Removes the MSPI and external S25FS064S from scrambling mode.
//!
//! @param pHandle
//!
//! @return
//
//*****************************************************************************
extern uint32_t am_devices_mspi_s25fs064s_disable_scrambling(void *pHandle);

//*****************************************************************************
//
//! @brief
//!
//! @param pHandle  - Pointer to device driver handle
//! @param pui8RxBuffer
//! @param ui32ReadAddress
//! @param ui32NumBytes
//! @param ui32PauseCondition
//! @param ui32StatusSetClr
//! @param pfnCallback
//! @param pCallbackCtxt
//!
//! @return
//
//*****************************************************************************
extern uint32_t am_devices_mspi_s25fs064s_read_adv(void *pHandle,
                               uint8_t *pui8RxBuffer,
                               uint32_t ui32ReadAddress,
                               uint32_t ui32NumBytes,
                               uint32_t ui32PauseCondition,
                               uint32_t ui32StatusSetClr,
                               am_hal_mspi_callback_t pfnCallback,
                               void *pCallbackCtxt);

//*****************************************************************************
//
//! @brief Reads the contents of the external s25fs064s into a buffer.
//!
//! @param pHandle  - Pointer to device driver handle
//! @param pui8RxBuffer
//! @param ui32ReadAddress
//! @param ui32NumBytes
//! @param bWaitForCompletion
//!
//! @return
//
//*****************************************************************************
extern uint32_t am_devices_mspi_s25fs064s_read_hiprio(void *pHandle,
                           uint8_t *pui8RxBuffer,
                           uint32_t ui32ReadAddress,
                           uint32_t ui32NumBytes,
                           bool bWaitForCompletion);



#ifdef __cplusplus
}
#endif

#endif // AM_DEVICES_MSPI_S25FS064S_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

