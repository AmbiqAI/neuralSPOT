//*****************************************************************************
//
//! @file dbi2dsi_test.c
//!
//! @brief DSI example.
//!
//! This example demonstrates how to drive a MIPI DSI panel.
//!
//! 1-lane DSI includes 4 signals,
//!   * Differential clock lane - positive (CLKP)
//!   * Differential clock lane - negative (CLKN)
//!   * Differential data lane 0 - positive (D0P)
//!   * Differential data lane 0 - negative (D0N).
//!
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

#include "am_bsp.h"
#include "nema_hal.h"
#include "nema_dc.h"
#include "nema_dc_mipi.h"
#include "am_util_delay.h"
#include "am_util_stdio.h"
#include "nema_dc_regs.h"
#include "oli_200x200_rgba.h"
#include "tsi_malloc.h"
#include "string.h"
#include "am_devices_dsi_rm67162.h"

#define FB_RESX 200
#define FB_RESY 200

//*****************************************************************************
//
//! @brief Test DBI2DSI interface.
//! @param ui32PixelFormat - Panel pixel format
//! @return None.
//
//*****************************************************************************
void
test_MIPI_DSI(uint32_t ui32PixelFormat)
{
    //! panel's max resolution
    uint16_t ui16PanelResX = g_sDispCfg[g_eDispType].ui32PanelResX;
    //! panel's max resolution
    uint16_t ui16PanelResY = g_sDispCfg[g_eDispType].ui32PanelResY;
    uint16_t ui16MinX, ui16MinY;
    //
    // Set the display region to center of panel
    //
    if (FB_RESX > ui16PanelResX)
    {
        //
        // set the minimum value to 0
        //
        ui16MinX = 0;
    }
    else
    {
        ui16MinX = (ui16PanelResX - FB_RESX) >> 1;
        ui16MinX = (ui16MinX >> 1) << 1;
    }

    if (FB_RESY > ui16PanelResY)
    {
        //
        // set the minimum value to 0
        //
        ui16MinY = 0;
    }
    else
    {
        ui16MinY = (ui16PanelResY - FB_RESY) >> 1;
        ui16MinY = (ui16MinY >> 1) << 1;
    }

    am_devices_dsi_rm67162_init(ui32PixelFormat, FB_RESX, FB_RESY, ui16MinX, ui16MinY);
    //
    // send layer 0 to display via NemaDC and DSI
    //
    nemadc_layer_t sLayer0;
    sLayer0.resx          = FB_RESX;
    sLayer0.resy          = FB_RESY;
    sLayer0.buscfg        = 0;
    sLayer0.format        = NEMADC_RGBA8888;
    sLayer0.blendmode     = NEMADC_BL_SRC;
    sLayer0.stride        = sLayer0.resx * 4;
    sLayer0.startx        = 0;
    sLayer0.starty        = 0;
    sLayer0.sizex         = sLayer0.resx;
    sLayer0.sizey         = sLayer0.resy;
    sLayer0.alpha         = 0xff;
    sLayer0.flipx_en      = 0;
    sLayer0.flipy_en      = 0;
    sLayer0.baseaddr_virt = tsi_malloc(sLayer0.resy * sLayer0.stride);
    sLayer0.baseaddr_phys = (unsigned)(sLayer0.baseaddr_virt);

    memcpy((char*)sLayer0.baseaddr_virt, ui8Oli200x200RGBA, sizeof(ui8Oli200x200RGBA));
    //
    // Program NemaDC Layer0.This function includes layer enable.
    //
    nemadc_set_layer(0, &sLayer0);
    //
    // start frame transfer with DPI disabled
    //
    dsi_send_frame_single(NEMADC_OUTP_OFF);
    tsi_free(sLayer0.baseaddr_virt);
}

//*****************************************************************************
//
//! @brief Test DBI2DSI interface.
//! @return 0 - Pass, Others - Fail.
//
//*****************************************************************************
int32_t
am_dbi2dsi_test(void)
{
    uint8_t ui8DbiWidth = g_sDsiCfg.eDbiWidth;
    pixel_format_t eFormat = FMT_RGB888;
    uint8_t ui8LanesNum = g_sDsiCfg.ui8NumLanes;
    uint32_t ui32MipiCfg = MIPICFG_8RGB888_OPT0;
    uint32_t ui32FreqTrim = g_sDsiCfg.eDsiFreq;

    switch (eFormat)
    {
        case FMT_RGB888:
            if (ui8DbiWidth == 16)
            {
                ui32MipiCfg = MIPICFG_16RGB888_OPT0;
            }
            if (ui8DbiWidth == 8)
            {
                ui32MipiCfg = MIPICFG_8RGB888_OPT0;
            }
            break;

        case FMT_RGB565:
            if (ui8DbiWidth == 16)
            {
                ui32MipiCfg = MIPICFG_16RGB565_OPT0;
            }
            if (ui8DbiWidth == 8)
            {
                ui32MipiCfg = MIPICFG_8RGB565_OPT0;
            }
            break;

        default:
            //
            // invalid color component index
            //
            return -3;
    }

    nema_sys_init();
    //
    // Initialize NemaDC
    //
    if (nemadc_init() != 0)
    {
        return -2;
    }
    //
    // Initialize DSI
    //
    if (am_hal_dsi_para_config(ui8LanesNum, ui8DbiWidth, ui32FreqTrim) != 0)
    {
        return -1;
    }

    test_MIPI_DSI(ui32MipiCfg);

    return 0;
}
