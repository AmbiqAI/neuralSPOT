//*****************************************************************************
//
//! @file am_devices_dsi_rm67162.h
//!
//! @brief Generic RM67162 DSI driver.
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
// This is part of revision release_sdk_4_1_0-8020bdf229 of the AmbiqSuite Development Package.
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
typedef enum
{
    FMT_RGB565 = 0,
    FMT_RGB888,
    FMT_NUM
} pixel_format_t;

typedef void (*CallbackFun)(void *contex);

//*****************************************************************************
//
// External function definitions.
//
//*****************************************************************************
extern uint32_t
am_devices_dsi_rm67162_init(uint32_t ui32PixelFormat, uint16_t ui16ResX, uint16_t ui16ResY, uint16_t ui16MinX, uint16_t ui16MinY);
extern void
dsi_send_frame_single_start(uint32_t ui32Mode);
extern void
dsi_send_frame_single_end(void);
extern void
dsi_send_frame_single(uint32_t ui32Mode);
extern uint32_t
am_devices_dsi_rm67162_set_region(uint16_t ui16ResX, uint16_t ui16ResY, uint16_t ui16MinX, uint16_t ui16MinY);
void
send_reset_signal(void);
void
dsi_dcs_write(uint8_t ui8Cmd, uint8_t* pui8Data, uint8_t ui8Len, bool bHS);
uint32_t
dsi_dcs_read(uint8_t cmd, uint8_t n_data, bool bHS);
void
dsi_generic_write(uint8_t* pui8Data, uint8_t ui8Len, bool bHS);
uint32_t
dsi_generic_read(uint8_t* cmd, uint8_t n_para, uint8_t n_data, bool bHS);

#ifdef __cplusplus
}
#endif

#endif // AM_DEVICES_DSI_RM67162_H

