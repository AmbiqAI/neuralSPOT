//*****************************************************************************
//
//! @file am_devices_spipsram.h
//!
//! @brief General SPI PSRAM driver.
//!
//! @addtogroup spipsram SPI PSRAM Driver
//! @ingroup devices
//! @{
//
//**************************************************************************

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

#ifndef AM_DEVICES_SPIPSRAM_H
#define AM_DEVICES_SPIPSRAM_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Global definitions for psram commands
//
//*****************************************************************************
#define AM_DEVICES_SPIPSRAM_WRITE             0x02
#define AM_DEVICES_SPIPSRAM_READ              0x03
#define AM_DEVICES_SPIPSRAM_FAST_READ         0x0B
#define AM_DEVICES_SPIPSRAM_QUAD_MODE_ENTER   0x35
#define AM_DEVICES_SPIPSRAM_QUAD_WRITE        0x38
#define AM_DEVICES_SPIPSRAM_RESET_ENABLE      0x66
#define AM_DEVICES_SPIPSRAM_RESET_MEMORY      0x99
#define AM_DEVICES_SPIPSRAM_READ_ID           0x9F
#define AM_DEVICES_SPIPSRAM_HALF_SLEEP_ENTER  0xC0
#define AM_DEVICES_SPIPSRAM_QUAD_READ         0xEB
#define AM_DEVICES_SPIPSRAM_QUAD_MODE_EXIT    0xF5

//
// The following definitions are typically specific to a multibit spi psram device.
// They should be tailored
//
//*****************************************************************************
//
// Device specific identification.
//
//*****************************************************************************
#define AM_DEVICES_SPIPSRAM_KGD_PASS          0x5D0D
#define AM_DEVICES_SPIPSRAM_KGD_FAIL          0x550D

// Page size - limits the bust write/read
#define AM_DEVICES_SPIPSRAM_PAGE_SIZE         1024

extern uint32_t g_APS6404LCS;

// According to APS6404L tCEM restriction, we define maximum bytes for each speed empirically
#define AM_DEVICES_SPIPSRAM_48MHZ_MAX_BYTES   32
#define AM_DEVICES_SPIPSRAM_24MHZ_MAX_BYTES   16
#define AM_DEVICES_SPIPSRAM_16MHZ_MAX_BYTES   10
#define AM_DEVICES_SPIPSRAM_12MHZ_MAX_BYTES   6
#define AM_DEVICES_SPIPSRAM_8MHZ_MAX_BYTES    3

#define AM_DEVICES_APS6404L_MAX_DEVICE_NUM    8

//*****************************************************************************
//
// Global type definitions.
//
//*****************************************************************************
typedef enum
{
    AM_DEVICES_SPIPSRAM_STATUS_SUCCESS,
    AM_DEVICES_SPIPSRAM_STATUS_ERROR
} am_devices_spipsram_status_t;

typedef struct
{
    uint32_t ui32ClockFreq;
    uint32_t *pNBTxnBuf;
    uint32_t ui32NBTxnBufLength;
} am_devices_spipsram_config_t;

//*****************************************************************************
//
// External function definitions.
//
//*****************************************************************************
extern uint32_t am_devices_spipsram_init(uint32_t ui32Module, am_devices_spipsram_config_t *pDevConfig, void **ppHandle, void **ppIomHandle);

extern uint32_t am_devices_spipsram_init_no_check(uint32_t ui32Module,
                                                  am_devices_spipsram_config_t *pDevConfig,
                                                  void **ppHandle,
                                                  void **ppIomHandle);


extern uint32_t am_devices_spipsram_term(void *pHandle);

extern uint32_t am_devices_spipsram_read_id(void *pHandle, uint32_t *pDeviceID);

extern uint32_t am_devices_spipsram_reset(void *pHandle);

extern uint32_t am_devices_spipsram_read(void *pHandle, uint8_t *pui8RxBuffer,
                                           uint32_t ui32ReadAddress,
                                           uint32_t ui32NumBytes,
                                           bool bWaitForCompletion);
extern uint32_t am_devices_spipsram_read_adv(void *pHandle, uint8_t *pui8RxBuffer,
                                           uint32_t ui32ReadAddress,
                                           uint32_t ui32NumBytes,
                                           uint32_t ui32PauseCondition,
                                           uint32_t ui32StatusSetClr,
                                           am_hal_iom_callback_t pfnCallback,
                                           void *pCallbackCtxt);

extern uint32_t am_devices_spipsram_write(void *pHandle, uint8_t *ui8TxBuffer,
                                            uint32_t ui32WriteAddress,
                                            uint32_t ui32NumBytes,
                                            bool bWaitForCompletion);

extern uint32_t am_devices_spipsram_write_adv(void *pHandle, uint8_t *puiTxBuffer,
                                           uint32_t ui32WriteAddress,
                                           uint32_t ui32NumBytes,
                                           uint32_t ui32PauseCondition,
                                           uint32_t ui32StatusSetClr,
                                           am_hal_iom_callback_t pfnCallback,
                                           void *pCallbackCtxt);

extern uint32_t am_devices_spipsram_nonblocking_read(void *pHandle, uint8_t *pui8RxBuffer,
                           uint32_t ui32ReadAddress,
                           uint32_t ui32NumBytes,
                           am_hal_iom_callback_t pfnCallback,
                           void *pCallbackCtxt);

extern uint32_t am_devices_spipsram_nonblocking_write(void *pHandle, uint8_t *pui8TxBuffer,
                           uint32_t ui32WriteAddress,
                           uint32_t ui32NumBytes,
                           am_hal_iom_callback_t pfnCallback,
                           void *pCallbackCtxt);

extern uint32_t am_devices_spipsram_blocking_write(void *pHandle, uint8_t *pui8TxBuffer,
                           uint32_t ui32WriteAddress,
                           uint32_t ui32NumBytes);

extern uint32_t am_devices_spipsram_blocking_read(void *pHandle, uint8_t *pui8RxBuffer,
                           uint32_t ui32ReadAddress,
                           uint32_t ui32NumBytes);

extern uint32_t am_devices_spipsram_command_write(void *pHandle, bool bHiPrio, uint32_t ui32InstrLen, uint64_t ui64Instr,
                           uint32_t *pData, uint32_t ui32NumBytes, bool bContinue);

extern uint32_t am_devices_spipsram_command_read(void *pHandle, bool bHiPrio, uint32_t ui32InstrLen, uint64_t ui64Instr,
                           uint32_t *pData, uint32_t ui32NumBytes, bool bContinue);


#ifdef __cplusplus
}
#endif

#endif // AM_DEVICES_SPIPSRAM_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

