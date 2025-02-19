//*****************************************************************************
//
//! @file am_devices_sdio_rs9116.h
//!
//! @brief SiliconLabs RS9116 Wi-Fi device driver.
//!
//! @addtogroup RS9116 SDIO Wi-Fi Driver
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
// This is part of revision release_sdk5p2-040c7863bb of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef AM_DEVICES_SDIO_RS9116_H
#define AM_DEVICES_SDIO_RS9116_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Global definitions
//
//*****************************************************************************
extern am_hal_card_host_t *pSdhcCardHost;
extern am_hal_card_t       RS9116SdioCard;
extern uint32_t            ui32SdioModule;
extern uint8_t             sdio_init_done;

//*****************************************************************************
//
// Global type definitions.
//
//*****************************************************************************
typedef enum
{
    AM_DEVICES_SDIO_RS9116_STATUS_SUCCESS = 0,
    AM_DEVICES_SDIO_RS9116_STATUS_ERROR  = -1,
} am_devices_sdio_rs9116_status_t;

//*****************************************************************************
//
// External function definitions.
//
//*****************************************************************************
extern int16_t rsi_sdio_write_multiple(uint8_t *tx_data, uint32_t Addr, uint32_t no_of_blocks);
extern int8_t rsi_sdio_read_multiple(uint8_t *read_buff, uint32_t Addr, uint32_t no_of_blocks);
extern int8_t sdio_reg_writeb(uint32_t Addr, uint8_t *dBuf);
extern int8_t sdio_reg_readb(uint32_t Addr, uint8_t *dBuf);
extern int16_t rsi_sdio_readb(uint32_t addr, uint16_t len, uint8_t *dBuf);
extern int16_t rsi_sdio_writeb(uint32_t addr, uint16_t len, uint8_t *dBuf);
extern int32_t rsi_mcu_sdio_init(void);
extern int32_t rsi_sdio_apply_timing(uint8_t ui8TxRxDelays[2]);
extern int32_t rsi_sdio_timing_scan(am_hal_host_inst_index_e eIndex,
                           am_hal_host_uhs_mode_e eUHSMode,
                           uint32_t ui32Clock,
                           am_hal_host_bus_width_e eBusWidth,
                           uint8_t *ui8CalibBuf,
                           uint32_t ui32StartAddr,
                           uint32_t ui32BlockCnt,
                           am_hal_host_bus_voltage_e eIoVoltage,
                           uint8_t ui8TxRxDelays[2],
                           am_hal_sdio_card_reset_func pSdioCardReset);

#ifdef __cplusplus
}
#endif

#endif // AM_DEVICES_SDIO_RS9116_H

//*****************************************************************************
// End Doxygen group.
//! @}
//
//*****************************************************************************

