//*****************************************************************************
//
//! @file am_devices_dsi_rm67162.h
//!
//! @brief Generic Raydium OLED display driver.
//!
//! @addtogroup dsi_rm67162 RM67162 DSI Display Device Driver
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


#ifndef AM_DEVICES_DSI_RM67162_H
#define AM_DEVICES_DSI_RM67162_H

#include "stdint.h"
#include "nema_dc.h"
#include "nema_dc_hal.h"
#include "nema_dc_mipi.h"
#include "nema_dc_regs.h"
#include "nema_dc_intern.h"
#include "nema_dc_dsi.h"

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Global definitions for the commands
//
//*****************************************************************************
#define TB_LCDPANEL_MIPI_DBIB      14

//*****************************************************************************
//
// Global type definitions.
//
//*****************************************************************************

//
//!
//
typedef enum
{
    FMT_RGB565 = 0,
    FMT_RGB888,
    FMT_NUM
} pixel_format_t;

//
//!  callback typedef
//
typedef void (*CallbackFun)(void *contex);

//*****************************************************************************
//
// External function definitions.
//
//*****************************************************************************
//*****************************************************************************
//
//! @brief Initialize the rm67162 dsi driver.
//!
//! @param  ui32PixelFormat     - Pixel color format
//! @param  ui16ResX, ui16ResY  - Resolution of frame
//! @param  ui16MinX, ui16MinY  - Start point of panel region to be updated
//!
//! @note This function should be called before any other am_devices_rm67162
//! functions. It is used to set tell the other functions how to communicate
//! with the OLED display hardware.
//!
//! @return Status.
//
//*****************************************************************************
extern uint32_t am_devices_dsi_rm67162_init(
    uint32_t ui32PixelFormat,
    uint16_t ui16ResX,
    uint16_t ui16ResY,
    uint16_t ui16MinX,
    uint16_t ui16MinY);

//*****************************************************************************
//
//! @brief start sending single frame.
//!
//! @param ui32Mode
//!
//! This function sends frame to rm67162.
//
//****************************************************************************
extern void dsi_send_frame_single_start(uint32_t ui32Mode);

//*****************************************************************************
//
//! @brief Function to dsi_send_frame_single_start_none_block_te
//!
//! This function send start TE without block
//
//*****************************************************************************
extern void dsi_send_frame_single_start_none_block_te(void);

//*****************************************************************************
//
//! @brief Function to dsi_send_frame_single_start_none_block_vsync
//!
//! This function send start flush the screen without block
//!
//! @param ui32Mode - NEMADC mode select
//
//*****************************************************************************
extern void dsi_send_frame_single_start_none_block_vsync(uint32_t ui32Mode);

//*****************************************************************************
//
//! @brief Configurations after completing single frame.
//!
//! This function should be called after completing single frame.
//
//****************************************************************************
extern void dsi_send_frame_single_end(void);

//*****************************************************************************
//
//! @brief send single frame.
//!
//! @param ui32Mode
//!
//! This function sends frame to rm67162.
//
//****************************************************************************
extern void dsi_send_frame_single(uint32_t ui32Mode);

//*****************************************************************************
//
//! @brief Set the rm67162 updated region.
//!
//! @param  ui16ResX, ui16ResY    - resolution of frame
//! @param  ui16MinX, ui16MinY    - start point of panel region to be updated
//!
//! This is used to set rm67162 updated region.
//!
//! @return Status.
//
//*****************************************************************************
extern uint32_t am_devices_dsi_rm67162_set_region(
    uint16_t ui16ResX,
    uint16_t ui16ResY,
    uint16_t ui16MinX,
    uint16_t ui16MinY);

//*****************************************************************************
//
//! @brief hardware reset rm67162.
//!
//! This function send a RESX signal to the display
//
//*****************************************************************************
void send_reset_signal(void);

//*****************************************************************************
//
//! @brief send DSI DCS command.
//!
//! @param ui8Cmd
//! @param pui8Data
//! @param ui8Len
//! @param bHS
//!
//! This function sends DSI commands to rm67162.
//
//****************************************************************************
void dsi_dcs_write(uint8_t ui8Cmd, uint8_t* pui8Data, uint8_t ui8Len, bool bHS);

//*****************************************************************************
//
//! @brief DSI read.
//!
//! @param cmd
//! @param n_data
//! @param bHS
//!
//! @return
//
//****************************************************************************
uint32_t dsi_dcs_read(uint8_t cmd, uint8_t n_data, bool bHS);

//*****************************************************************************
//
//! @brief send DSI generic command.
//!
//! This function sends DSI commands to rm67162.
//
//****************************************************************************
void dsi_generic_write(uint8_t* pui8Data, uint8_t ui8Len, bool bHS);

//*****************************************************************************
//
//! @brief DSI generic read command.
//!
//! @param cmd
//! @param n_para
//! @param n_data
//! @param bHS
//!
//! This function reads parameters from rm67162.
//!
//! @return
//
//****************************************************************************
uint32_t dsi_generic_read(uint8_t* cmd, uint8_t n_para, uint8_t n_data, bool bHS);

#ifdef __cplusplus
}
#endif

#endif // AM_DEVICES_DSI_RM67162_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

