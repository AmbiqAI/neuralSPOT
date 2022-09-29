//*****************************************************************************
//
//! @file mspi_octal_example.h
//!
//! @brief Headers file for Example of the MSPI operation with Octal SPI Flash.
//! @{
//!
//!
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
#ifndef _MSPI_OCTAL_EXAMPLE_H
#define _MSPI_OCTAL_EXAMPLE_H

#ifdef __cplusplus
extern "C"
{
#endif

//
//! the abstracted marco for the various mspi devices
//
#if defined(AM_BSP_MSPI_FLASH_DEVICE_ATXP032)
    #define am_abstract_mspi_devices_config_t         am_devices_mspi_atxp032_config_t
    #define am_abstract_mspi_devices_timing_config_t  am_devices_mspi_atxp032_sdr_timing_config_t
    #define AM_ABSTRACT_MSPI_SUCCESS                  AM_DEVICES_MSPI_ATXP032_STATUS_SUCCESS
    #define AM_ABSTRACT_MSPI_ERROR                    AM_DEVICES_MSPI_ATXP032_STATUS_ERROR
    #define AM_DEVICES_MSPI_FLASH_SECTOR_SHIFT        16
#elif defined(AM_BSP_MSPI_FLASH_DEVICE_IS25WX064)
    #define am_abstract_mspi_devices_config_t         am_devices_mspi_is25wx064_config_t
    #define am_abstract_mspi_devices_timing_config_t  am_devices_mspi_is25wx064_timing_config_t
    #define AM_ABSTRACT_MSPI_SUCCESS                  AM_DEVICES_MSPI_IS25WX064_STATUS_SUCCESS
    #define AM_ABSTRACT_MSPI_ERROR                    AM_DEVICES_MSPI_IS25WX064_STATUS_ERROR
    #define AM_DEVICES_MSPI_FLASH_SECTOR_SHIFT        17
#endif

//
//! the universal struct for the various mspi devices struct
//! each internal function pointer members refer to their different drivers
//
typedef struct
{
    uint8_t  devName[30];
    uint32_t (*mspi_init)(uint32_t ui32Module, am_abstract_mspi_devices_config_t *psMSPISettings, void **ppHandle, void **ppMspiHandle);
    uint32_t (*mspi_term)(void *pHandle);

    uint32_t (*mspi_read_id)(void *pHandle);

    uint32_t (*mspi_read)(void *pHandle, uint8_t *pui8RxBuffer,
                           uint32_t ui32ReadAddress,
                           uint32_t ui32NumBytes,
                           bool bWaitForCompletion);

    uint32_t (*mspi_read_adv)(void *pHandle, uint8_t *pui8RxBuffer,
                           uint32_t ui32ReadAddress,
                           uint32_t ui32NumBytes,
                           uint32_t ui32PauseCondition,
                           uint32_t ui32StatusSetClr,
                           am_hal_mspi_callback_t pfnCallback,
                           void *pCallbackCtxt);

    uint32_t (*mspi_read_callback)(void *pHandle, uint8_t *pui8RxBuffer,
                           uint32_t ui32ReadAddress,
                           uint32_t ui32NumBytes);

    uint32_t (*mspi_write)(void *pHandle, uint8_t *ui8TxBuffer,
                           uint32_t ui32WriteAddress,
                           uint32_t ui32NumBytes,
                           bool bWaitForCompletion);

    uint32_t (*mspi_write_adv)(void *pHandle,
                           uint8_t *puiTxBuffer,
                           uint32_t ui32WriteAddress,
                           uint32_t ui32NumBytes,
                           uint32_t ui32PauseCondition,
                           uint32_t ui32StatusSetClr,
                           am_hal_mspi_callback_t pfnCallback,
                           void *pCallbackCtxt);

    uint32_t (*mspi_mass_erase)(void *pHandle);
    uint32_t (*mspi_sector_erase)(void *pHandle, uint32_t ui32SectorAddress);

    uint32_t (*mspi_xip_enable)(void *pHandle);
    uint32_t (*mspi_xip_disable)(void *pHandle);
    uint32_t (*mspi_scrambling_enable)(void *pHandle);
    uint32_t (*mspi_scrambling_disable)(void *pHandle);
    uint32_t (*mspi_init_timing_check)(uint32_t ui32Module,
                                        am_abstract_mspi_devices_config_t *pDevCfg,
                                        am_abstract_mspi_devices_timing_config_t *pDevSdrCfg);
    uint32_t (*mspi_init_timing_apply)(void *pHandle,
                                        am_abstract_mspi_devices_timing_config_t *pDevSdrCfg);
} mspi_device_func_t;

#ifdef __cplusplus
}
#endif

#endif // _MSPI_OCTAL_EXAMPLE_H
//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
