//*****************************************************************************
//
//! @file am_devices_mspi_psram_aps12808l.h
//!
//! @brief Micron Serial SPI PSRAM driver.
//!
//! @addtogroup mspi_psram_12808l APS12808L MSPI PSRAM Driver
//! @ingroup devices
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2022, Ambiq Micro, Inc.
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
// This is part of revision release_sdk_4_3_0-0ca7d78a2b of the AmbiqSuite Development Package.
//
//*****************************************************************************

#ifndef AM_DEVICES_MSPI_PSRAM_APS12808L_H
#define AM_DEVICES_MSPI_PSRAM_APS12808L_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Global definitions for psram commands
//
//*****************************************************************************
#define AM_DEVICES_MSPI_PSRAM_OCTAL_DDR_GLOBAL_RESET 0xFFFF
#define AM_DEVICES_MSPI_PSRAM_OCTAL_DDR_READ        0x2020
#define AM_DEVICES_MSPI_PSRAM_OCTAL_DDR_WRITE       0xA0A0
#define AM_DEVICES_MSPI_PSRAM_OCTAL_DDR_READ_REGISTER 0x4040
#define AM_DEVICES_MSPI_PSRAM_OCTAL_DDR_WRITE_REGISTER 0xC0C0

//
// The following definitions are typically specific to a multibit spi psram device.
// They should be tailored
//
//*****************************************************************************
//
// Device specific identification.
//
//*****************************************************************************
// Page size - limits the bust write/read
#define AM_DEVICES_MSPI_PSRAM_PAGE_SIZE         1024
#define AM_DEVICES_MSPI_PSRAM_TEST_BLOCK_SIZE   8*1024

//*****************************************************************************
//
// Global definitions for the MSPI instance to use.
//
//*****************************************************************************
#define AM_DEVICES_MSPI_PSRAM_MAX_DEVICE_NUM    2

//*****************************************************************************
//
// Global type definitions.
//
//*****************************************************************************
typedef enum
{
    AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS,
    AM_DEVICES_MSPI_PSRAM_STATUS_ERROR
} am_devices_mspi_psram_status_t;

typedef struct
{
    am_hal_mspi_device_e eDeviceConfig;
    am_hal_mspi_clock_e eClockFreq;
    uint32_t *pNBTxnBuf;
    uint32_t ui32NBTxnBufLength;
    uint32_t ui32ScramblingStartAddr;
    uint32_t ui32ScramblingEndAddr;
} am_devices_mspi_psram_config_t;

typedef struct
{
    uint32_t ui32Turnaround;
    uint32_t ui32Rxneg;
    uint32_t ui32Rxdqsdelay;
    uint32_t ui32Txdqsdelay;
} am_devices_mspi_psram_ddr_timing_config_t;

//*****************************************************************************
//
// External function definitions.
//
//*****************************************************************************
extern uint32_t am_devices_mspi_psram_aps12808l_ddr_init(uint32_t ui32Module,
                                                   am_devices_mspi_psram_config_t *pDevCfg,
                                                   void **ppHandle,
                                                   void **pMspiHandle);

extern uint32_t am_devices_mspi_psram_aps12808l_ddr_deinit(void *pHandle);

extern uint32_t am_devices_mspi_psram_aps12808l_ddr_id(void *pHandle);

extern uint32_t am_devices_mspi_psram_aps12808l_ddr_reset(void *pHandle);

extern uint32_t am_devices_mspi_psram_aps12808l_ddr_read(void *pHandle,
                                           uint8_t *pui8RxBuffer,
                                           uint32_t ui32ReadAddress,
                                           uint32_t ui32NumBytes,
                                           bool bWaitForCompletion);

extern uint32_t am_devices_mspi_psram_aps12808l_ddr_read_adv(void *pHandle,
                                               uint8_t *pui8RxBuffer,
                                               uint32_t ui32ReadAddress,
                                               uint32_t ui32NumBytes,
                                               uint32_t ui32PauseCondition,
                                               uint32_t ui32StatusSetClr,
                                               am_hal_mspi_callback_t pfnCallback,
                                               void *pCallbackCtxt);

extern uint32_t am_devices_mspi_psram_aps12808l_ddr_write(void *pHandle,
                                            uint8_t *ui8TxBuffer,
                                            uint32_t ui32WriteAddress,
                                            uint32_t ui32NumBytes,
                                            bool bWaitForCompletion);

extern uint32_t am_devices_mspi_psram_aps12808l_ddr_write_adv(void *pHandle,
                                                uint8_t *puiTxBuffer,
                                                uint32_t ui32WriteAddress,
                                                uint32_t ui32NumBytes,
                                                uint32_t ui32PauseCondition,
                                                uint32_t ui32StatusSetClr,
                                                am_hal_mspi_callback_t pfnCallback,
                                                void *pCallbackCtxt);

extern uint32_t am_devices_mspi_psram_aps12808l_ddr_enable_xip(void *pHandle);

extern uint32_t am_devices_mspi_psram_aps12808l_ddr_disable_xip(void *pHandle);

extern uint32_t am_devices_mspi_psram_aps12808l_ddr_enable_scrambling(void *pHandle);

extern uint32_t am_devices_mspi_psram_aps12808l_ddr_disable_scrambling(void *pHandle);

extern uint32_t am_devices_mspi_psram_aps12808l_ddr_read_hiprio(void *pHandle, uint8_t *pui8RxBuffer,
                           uint32_t ui32ReadAddress,
                           uint32_t ui32NumBytes,
                           am_hal_mspi_callback_t pfnCallback,
                           void *pCallbackCtxt);

extern uint32_t am_devices_mspi_psram_aps12808l_ddr_nonblocking_read(void *pHandle, uint8_t *pui8RxBuffer,
                           uint32_t ui32ReadAddress,
                           uint32_t ui32NumBytes,
                           am_hal_mspi_callback_t pfnCallback,
                           void *pCallbackCtxt);


extern uint32_t am_devices_mspi_psram_aps12808l_ddr_write_hiprio(void *pHandle, uint8_t *pui8TxBuffer,
                           uint32_t ui32WriteAddress,
                           uint32_t ui32NumBytes,
                           am_hal_mspi_callback_t pfnCallback,
                           void *pCallbackCtxt);

extern uint32_t am_devices_mspi_psram_aps12808l_ddr_nonblocking_write(void *pHandle, uint8_t *pui8TxBuffer,
                           uint32_t ui32WriteAddress,
                           uint32_t ui32NumBytes,
                           am_hal_mspi_callback_t pfnCallback,
                           void *pCallbackCtxt);


extern uint32_t am_devices_mspi_psram_aps12808l_ddr_init_timing_check(uint32_t module,
                                            am_devices_mspi_psram_config_t *pDevCfg,
                                            am_devices_mspi_psram_ddr_timing_config_t *pDevDdrCfg);

extern uint32_t am_devices_mspi_psram_aps12808l_apply_ddr_timing(void *pHandle,
                                        am_devices_mspi_psram_ddr_timing_config_t *pDevDdrCfg);

#ifdef __cplusplus
}
#endif

#endif // AM_DEVICES_MSPI_PSRAM_APS12808L_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

