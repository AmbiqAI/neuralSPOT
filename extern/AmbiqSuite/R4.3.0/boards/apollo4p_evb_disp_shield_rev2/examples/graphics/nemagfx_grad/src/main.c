//*****************************************************************************
//
//! @file main.c
//!
//! @brief NemaGFX example.
//! In computer graphics, a color gradient specifies a range of position-dependent
//! colors, usually used to fill a region. For example, many window managers
//! allow the screen background to be specified as a gradient. The colors
//! produced by a gradient vary continuously with the position, producing smooth
//! color transitions.

//!
//! AM_DEBUG_PRINTF
//! If enabled, debug messages will be sent over ITM.
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
#include "nema_core.h"
#include "nema_utils.h"
#include "am_hal_global.h"
#include "string.h"

#ifndef DONT_USE_NEMADC
#include "nema_dc.h"
#include "nema_dc_mipi.h"
#endif

#include "am_devices_dsi_rm67162.h"
#include "am_devices_nemadc_rm67162.h"
#include "nema_programHW.h"

static const unsigned long OWHITE    = 0xFFFFFFFF;
static const unsigned long BLACK     = 0x00000000;

#define OPAQUE(color)         ((color) | 0xFF000000 )
#define TRANSP(alpha, color)  ((color & 0x00FFFFFF) | ((alpha & 0xff) << 24) )

#define RESX 256
#define RESY 256

AM_SHARED_RW nema_cmdlist_t g_sCL0;
AM_SHARED_RW nema_cmdlist_t g_sContextCL;
AM_SHARED_RW nema_cmdlist_t g_sClearBlackCL;
AM_SHARED_RW nema_cmdlist_t g_sClearWhiteCL;

nema_buffer_t g_sFBBo = {0};
nema_buffer_t g_sZBufferBo  = {0};

nemadc_layer_t g_sDCLayer = {(void *)0, 0, RESX, RESY, RESX * 2, 0, 0, RESX, RESY, 0xff, NEMADC_BL_SRC, 0, NEMADC_RGB565, 0, 0, 0, 0, 0, 0, 0};

void
suite_init(void)
{
    g_sFBBo = nema_buffer_create(RESX * RESY * 4);
    nema_buffer_map(&g_sFBBo);

    g_sDCLayer.baseaddr_phys = g_sFBBo.base_phys;
    g_sDCLayer.baseaddr_virt = g_sFBBo.base_virt;
    memset(g_sDCLayer.baseaddr_virt, 0, RESX*RESY*4);

    g_sZBufferBo = nema_buffer_create(RESX * RESY * 4);
    nema_buffer_map(&g_sZBufferBo);

    g_sContextCL = nema_cl_create();
    nema_cl_bind(&g_sContextCL);
    nema_set_const_reg(0, 0);
    nema_enable_gradient(0);
    nema_enable_depth(0);

    nema_bind_dst_tex(g_sFBBo.base_phys, RESX, RESY, NEMA_RGB565, -1);
    nema_bind_depth_buffer(g_sZBufferBo.base_phys, RESX, RESY);

    nema_set_clip(0, 0, RESX, RESY);

    nema_cl_unbind();
    //
    // Clear Black CL
    //
    g_sClearBlackCL = nema_cl_create_sized(256);
    nema_cl_bind(&g_sClearBlackCL);
    nema_cl_branch(&g_sContextCL);
    nema_set_matrix_translate(0, 0);
    nema_clear(BLACK);
    nema_cl_unbind();
    //
    // Clear White CL
    //
    g_sClearWhiteCL = nema_cl_create_sized(256);
    nema_cl_bind(&g_sClearWhiteCL);
    nema_cl_branch(&g_sContextCL);
    nema_set_matrix_translate(0, 0);
    nema_clear(OWHITE);
    nema_cl_unbind();
}

int32_t
fillrect_grad()
{
    int32_t x1, y1;
    color_var_t col0, col1, col2;

    g_sCL0 = nema_cl_create();
    nema_cl_bind(&g_sCL0);
    nema_cl_branch(&g_sContextCL);

    nema_enable_gradient(1);

    nema_set_blend_fill(NEMA_BL_SRC);
    //
    // Col 1
    // NEMA_P_RAST_EMULATION;
    //
    x1 = 10; y1 = 10;
    col0.r = 0; col0.g = 0; col0.b = 0; col0.a = 0;
    col1.r = 400; col1.g = 400; col1.b = 400; col1.a = 400; //!< x gradient
    col2.r = 100; col2.g = 100; col2.b = 100; col2.a = 100; //!< y gradient
    nema_interpolate_rect_colors(x1, y1, RESX, RESY, &col0, &col1, &col2);
    nema_fill_rect(x1, y1, RESX, RESY, 0);

    nema_cl_submit(&g_sCL0);
    nema_cl_wait(&g_sCL0);
    nemadc_set_layer(0, &g_sDCLayer);
    if (g_sDispCfg[g_eDispType].eInterface == IF_DSI)
    {
        dsi_send_frame_single(NEMADC_OUTP_OFF);
    }
    else
    {
        nemadc_send_frame_single();
    }

    nema_enable_gradient(0);

    nema_cl_unbind();
    nema_cl_destroy(&g_sCL0);

    return 0;
}

int32_t
suite_run(void)
{
    int32_t i32Ret;
    uint32_t ui32MipiCfg = MIPICFG_8RGB888_OPT0; //!< default config
    //
    // Initialize NemaGFX
    //
    i32Ret = nema_init();
    if (i32Ret != 0)
    {
        return i32Ret;
    }
    //
    // Initialize Nema|dc
    //
    i32Ret = nemadc_init();
    if (i32Ret != 0)
    {
        return i32Ret;
    }

    if ((g_sDispCfg[g_eDispType].eInterface == IF_DSI) || (g_sDispCfg[g_eDispType].bUseDPHYPLL == true))
    {
        uint8_t ui8LanesNum = g_sDsiCfg.ui8NumLanes;
        uint8_t ui8DbiWidth = g_sDsiCfg.eDbiWidth;
        uint32_t ui32FreqTrim = g_sDsiCfg.eDsiFreq;
        pixel_format_t eFormat = FMT_RGB888;
        if (am_hal_dsi_para_config(ui8LanesNum, ui8DbiWidth, ui32FreqTrim) != 0)
        {
            return -3;
        }
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
    }

    uint16_t ui16PanelResX = g_sDispCfg[g_eDispType].ui32PanelResX; //!< panel's max resolution
    uint16_t ui16PanelResY = g_sDispCfg[g_eDispType].ui32PanelResY; //!< panel's max resolution

    uint16_t ui16MinX, ui16MinY;
    //
    // Set the display region to center
    //
    if (RESX > ui16PanelResX)
    {
        ui16MinX = 0;   //!< set the minimum value to 0
    }
    else
    {
        ui16MinX = (ui16PanelResX - RESX) >> 1;
        ui16MinX = (ui16MinX >> 1) << 1;
    }

    if (RESY > ui16PanelResY)
    {
        ui16MinY = 0;   //!< set the minimum value to 0
    }
    else
    {
        ui16MinY = (ui16PanelResY - RESY) >> 1;
        ui16MinY = (ui16MinY >> 1) << 1;
    }
    //
    // Initialize the display
    //
    switch (g_sDispCfg[g_eDispType].eInterface)
    {
        case IF_SPI4:
            am_devices_nemadc_rm67162_init(MIPICFG_SPI4, MIPICFG_1RGB565_OPT0, RESX, RESY, ui16MinX, ui16MinY);
            break;
        case IF_DSPI:
            am_devices_nemadc_rm67162_init(MIPICFG_DSPI | MIPICFG_SPI4, MIPICFG_2RGB565_OPT0, RESX, RESY, ui16MinX, ui16MinY);
            break;
        case IF_QSPI:
            am_devices_nemadc_rm67162_init(MIPICFG_QSPI | MIPICFG_SPI4, MIPICFG_4RGB565_OPT0, RESX, RESY, ui16MinX, ui16MinY);
            break;
        case IF_DSI:
            am_devices_dsi_rm67162_init(ui32MipiCfg, RESX, RESY, ui16MinX, ui16MinY);
            break;
        default:
            ; //NOP
    }

    suite_init();

    fillrect_grad();

    return 0;
}

