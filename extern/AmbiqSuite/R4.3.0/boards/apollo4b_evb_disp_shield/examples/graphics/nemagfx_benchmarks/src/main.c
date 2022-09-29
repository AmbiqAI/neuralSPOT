//*****************************************************************************
//
//! @file main.c
//!
//! @brief NemaGFX example.
//! this example demonstrate the Nema GPU and CPU performance use Nema GPU's
//! basic characteristics, we should care about the FPS after each individual
//! test.
//! need a timer to get the accurate time past.
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

// -----------------------------------------------------------------------------
// Copyright (c) 2019 Think Silicon S.A.
// Think Silicon S.A. Confidential Proprietary
// -----------------------------------------------------------------------------
//     All Rights reserved - Unpublished -rights reserved under
//         the Copyright laws of the European Union
//
//  This file includes the Confidential information of Think Silicon S.A.
//  The receiver of this Confidential Information shall not disclose
//  it to any third party and shall protect its confidentiality by
//  using the same degree of care, but not less than a reasonable
//  degree of care, as the receiver uses to protect receiver's own
//  Confidential Information. The entire notice must be reproduced on all
//  authorised copies and copies may only be made to the extent permitted
//  by a licensing agreement from Think Silicon S.A..
//
//  The software is provided 'as is', without warranty of any kind, express or
//  implied, including but not limited to the warranties of merchantability,
//  fitness for a particular purpose and noninfringement. In no event shall
//  Think Silicon S.A. be liable for any claim, damages or other liability, whether
//  in an action of contract, tort or otherwise, arising from, out of or in
//  connection with the software or the use or other dealings in the software.
//
//
//                    Think Silicon S.A.
//                    http://www.think-silicon.com
//                    Patras Science Park
//                    Rion Achaias 26504
//                    Greece
// -----------------------------------------------------------------------------
#include "am_bsp.h"
#include "nema_core.h"
#include "nema_utils.h"

#ifndef DONT_USE_NEMADC
#include "nema_dc.h"
#include "nema_dc_mipi.h"
#endif

#include "utils.h"
#include "bench.h"
#include "am_devices_dsi_rm67162.h"
#include "am_devices_nemadc_rm67162.h"

int bench_fill_quad(int i32Blend);
int bench_fill_rect(int i32Blend);
int bench_fill_tri(int i32Blend);

int bench_draw_circle(int blendmode);
int bench_draw_line(int blendmode);
int bench_draw_rect(int blendmode);
int bench_draw_string(int blendmode);

int bench_blit(int blendmode, int rotation);
int bench_stretch_blit(int blendmode, float scale, int i32NemaTexMode);
int bench_stretch_blit_rotate(int blendmode, float scale, int i32NemaTexMode);

int bench_textured_tri(int blendmode, int i32NemaTexMode);
int bench_textured_quad(int blendmode, int i32NemaTexMode);

#ifndef DEFAULT_EXEC_MODE
#define DEFAULT_EXEC_MODE CPU_GPU
#endif

TLS_VAR AM_SHARED_RW nema_cmdlist_t *g_psCLCur, g_sCL0, g_sCL1, g_sContextCL;
ExecutionMode_e eExecMode = DEFAULT_EXEC_MODE;

extern float start_wall, stop_wall;

static int
i32RenderFrame()
{
  return 0;
}

static int
test_irq(int i32Mode)
{

    g_sCL0 = nema_cl_create();
    nema_cl_bind(&g_sCL0);
    //
    // just some dummy instructions
    //
    nema_set_blend_fill(NEMA_BL_SRC);

    start_wall = nema_get_time();
    stop_wall  = start_wall;

    int i32CLCount = 0;

    do
    {
        do
        {
            nema_cl_submit(&g_sCL0);
            if (i32Mode == 0)
            {
                nema_cl_wait(&g_sCL0);
            }
            ++i32CLCount;
        }
        while (i32CLCount % 1000 != 0);

        stop_wall = nema_get_time();
    }
    while ((stop_wall - start_wall) < TIMEOUT_S);

    nema_cl_destroy(&g_sCL0);

    return i32CLCount;
}

void
run_bench(int i32TestNo)
{
    suite_init();

    int i32Result = 0;

    AM_REGVAL(0x40090090) = 0x00000002;

    switch (i32TestNo)
    {
        case 37:
            bench_start(i32TestNo);
            i32Result = test_irq(0);
            bench_stop(i32TestNo, i32Result);
            break;
        case 38:
            bench_start(i32TestNo);
            i32Result = test_irq(1);
            bench_stop(i32TestNo, i32Result);
            break;

        case 1:
            bench_start(i32TestNo);
            i32Result = bench_fill_tri(0);
            bench_stop(i32TestNo, i32Result);
            break;

        case 2:
            bench_start(i32TestNo);
            i32Result = bench_fill_tri(1);
            bench_stop(i32TestNo, i32Result);
            break;

        case 3:
            bench_start(i32TestNo);
            i32Result = bench_fill_rect(0);
            bench_stop(i32TestNo, i32Result);
            break;

        case 4:
            bench_start(i32TestNo);
            i32Result = bench_fill_rect(1);
            bench_stop(i32TestNo, i32Result);
            break;

        case 5:
            bench_start(i32TestNo);
            i32Result = bench_fill_quad(0);
            bench_stop(i32TestNo, i32Result);
            break;

        case 6:
            bench_start(i32TestNo);
            i32Result = bench_fill_quad(1);
            bench_stop(i32TestNo, i32Result);
            break;

        case 7:
            bench_start(i32TestNo);
            i32Result = bench_draw_string(NEMA_BL_SRC);
            bench_stop(i32TestNo, i32Result);
            break;

        case 8:
            bench_start(i32TestNo);
            i32Result = bench_draw_line(0);
            bench_stop(i32TestNo, i32Result);
            break;

        case 9:
            bench_start(i32TestNo);
            i32Result = bench_draw_line(1);
            bench_stop(i32TestNo, i32Result);
            break;

        case 10:
            bench_start(i32TestNo);
            i32Result = bench_draw_rect(0);
            bench_stop(i32TestNo, i32Result);
            break;

        case 11:
            bench_start(i32TestNo);
            i32Result = bench_draw_rect(1);
            bench_stop(i32TestNo, i32Result);
            break;

        case 12:
            bench_start(i32TestNo);
            i32Result = bench_blit(NEMA_BL_SRC, NEMA_ROT_000_CCW);
            bench_stop(i32TestNo, i32Result);
            break;

        case 13:
            bench_start(i32TestNo);
            i32Result = bench_blit(NEMA_BL_SRC | NEMA_BLOP_MODULATE_RGB, NEMA_ROT_000_CCW);
            bench_stop(i32TestNo, i32Result);
            break;

        case 14:
            bench_start(i32TestNo);
            i32Result = bench_blit(NEMA_BL_SIMPLE, NEMA_ROT_000_CCW);
            bench_stop(i32TestNo, i32Result);
            break;

        case 15:
            bench_start(i32TestNo);
            i32Result = bench_blit(NEMA_BL_SIMPLE | NEMA_BLOP_MODULATE_RGB, NEMA_ROT_000_CCW);
            bench_stop(i32TestNo, i32Result);
            break;

        case 16:
            bench_start(i32TestNo);
            i32Result = bench_blit(NEMA_BL_SRC, NEMA_ROT_090_CW);
            bench_stop(i32TestNo, i32Result);
            break;

        case 17:
            bench_start(i32TestNo);
            i32Result = bench_blit(NEMA_BL_SRC, NEMA_ROT_180_CW);
            bench_stop(i32TestNo, i32Result);
            break;

        case 18:
            bench_start(i32TestNo);
            i32Result = bench_blit(NEMA_BL_SRC, NEMA_ROT_270_CW);
            bench_stop(i32TestNo, i32Result);
            break;

        case 19:
            bench_start(i32TestNo);
            i32Result = bench_blit(NEMA_BL_SRC, NEMA_MIR_VERT);
            bench_stop(i32TestNo, i32Result);
            break;

        case 20:
            bench_start(i32TestNo);
            i32Result = bench_blit(NEMA_BL_SRC, NEMA_MIR_HOR);
            bench_stop(i32TestNo, i32Result);
            break;

        case 21:
            bench_start(i32TestNo);
            i32Result = bench_blit(NEMA_BL_SRC | NEMA_BLOP_SRC_CKEY, NEMA_ROT_000_CCW);
            bench_stop(i32TestNo, i32Result);
            break;

        case 22:
            bench_start(i32TestNo);
            i32Result = bench_blit(NEMA_BL_SRC | NEMA_BLOP_DST_CKEY, NEMA_ROT_000_CCW);
            bench_stop(i32TestNo, i32Result);
            break;

        case 23:
            bench_start(i32TestNo);
            i32Result = bench_stretch_blit(NEMA_BL_SRC, 1.5, NEMA_FILTER_PS);
            bench_stop(i32TestNo, i32Result);
            break;

        case 24:
            bench_start(i32TestNo);
            i32Result = bench_stretch_blit(NEMA_BL_SIMPLE, 1.5, NEMA_FILTER_PS);
            bench_stop(i32TestNo, i32Result);
            break;

        case 25:
            bench_start(i32TestNo);
            i32Result = bench_stretch_blit(NEMA_BL_SRC, 1.5, NEMA_FILTER_BL);
            bench_stop(i32TestNo, i32Result);
            break;

        case 26:
            bench_start(i32TestNo);
            i32Result = bench_stretch_blit(NEMA_BL_SIMPLE, 1.5, NEMA_FILTER_BL);
            bench_stop(i32TestNo, i32Result);
            break;

        case 27:
            bench_start(i32TestNo);
            i32Result = bench_stretch_blit_rotate(NEMA_BL_SRC, 0.75, NEMA_FILTER_PS);
            bench_stop(i32TestNo, i32Result);
            break;

        case 28:
            bench_start(i32TestNo);
            i32Result = bench_stretch_blit_rotate(NEMA_BL_SRC, 0.75, NEMA_FILTER_BL);
            bench_stop(i32TestNo, i32Result);
            break;

        case 29:
            bench_start(i32TestNo);
            i32Result = bench_textured_tri(NEMA_BL_SRC, NEMA_FILTER_PS);
            bench_stop(i32TestNo, i32Result);
            break;

        case 30:
            bench_start(i32TestNo);
            i32Result = bench_textured_tri(NEMA_BL_SIMPLE, NEMA_FILTER_PS);
            bench_stop(i32TestNo, i32Result);
            break;

        case 31:
            bench_start(i32TestNo);
            i32Result = bench_textured_tri(NEMA_BL_SRC, NEMA_FILTER_BL);
            bench_stop(i32TestNo, i32Result);
            break;

        case 32:
            bench_start(i32TestNo);
            i32Result = bench_textured_tri(NEMA_BL_SIMPLE, NEMA_FILTER_BL);
            bench_stop(i32TestNo, i32Result);
            break;

        case 33:
            bench_start(i32TestNo);
            i32Result = bench_textured_quad(NEMA_BL_SRC, NEMA_FILTER_PS);
            bench_stop(i32TestNo, i32Result);
            break;

        case 34:
            bench_start(i32TestNo);
            i32Result = bench_textured_quad(NEMA_BL_SIMPLE, NEMA_FILTER_PS);
            bench_stop(i32TestNo, i32Result);
            break;

        case 35:
            bench_start(i32TestNo);
            i32Result = bench_textured_quad(NEMA_BL_SRC, NEMA_FILTER_BL);
            bench_stop(i32TestNo, i32Result);
            break;

        case 36:
            bench_start(i32TestNo);
            i32Result = bench_textured_quad(NEMA_BL_SIMPLE, NEMA_FILTER_BL);
            bench_stop(i32TestNo, i32Result);
            break;
        default:
            return;
    }

    suite_terminate();
}

static int
ambiq_init(uint32_t ui32MipiCfg)
{
    uint16_t ui16PanelResX = g_sDispCfg[g_eDispType].ui32PanelResX; //!< panel's max resolution
    uint16_t ui16PanelResY = g_sDispCfg[g_eDispType].ui32PanelResY; //!< panel's max resolution
    uint16_t ui16MinX, ui16MinY;
    //
    // Set the display region to center
    //
    if ( FB_RESX > ui16PanelResX )
    {
        ui16MinX = 0;   //!< set the minimum value to 0
    }
    else
    {
        ui16MinX = (ui16PanelResX - FB_RESX) >> 1;
        ui16MinX = (ui16MinX >> 1) << 1;
    }

    if ( FB_RESY > ui16PanelResY )
    {
        ui16MinY = 0;   //!< set the minimum value to 0
    }
    else
    {
        ui16MinY = (ui16PanelResY - FB_RESY) >> 1;
        ui16MinY = (ui16MinY >> 1) << 1;
    }
    //
    // Initialize the display
    //
    switch (g_sDispCfg[g_eDispType].eInterface)
    {
        case IF_SPI4:
            am_devices_nemadc_rm67162_init(MIPICFG_SPI4, MIPICFG_1RGB565_OPT0, FB_RESX, FB_RESY, ui16MinX, ui16MinY);
            break;
        case IF_DSPI:
            am_devices_nemadc_rm67162_init(MIPICFG_DSPI | MIPICFG_SPI4, MIPICFG_2RGB565_OPT0, FB_RESX, FB_RESY, ui16MinX, ui16MinY);
            break;
        case IF_QSPI:
            am_devices_nemadc_rm67162_init(MIPICFG_QSPI | MIPICFG_SPI4, MIPICFG_4RGB565_OPT0, FB_RESX, FB_RESY, ui16MinX, ui16MinY);
            break;
        case IF_DSI:
            am_devices_dsi_rm67162_init(ui32MipiCfg, FB_RESX, FB_RESY, ui16MinX, ui16MinY);
            break;
        default:
            ; //NOP
    }

    return 0;
}

static int
init(void)
{
    int i32Ret;
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
                if (ui8DbiWidth ==  8)
                {
                    ui32MipiCfg = MIPICFG_8RGB888_OPT0;
                }
                break;

            case FMT_RGB565:
                if (ui8DbiWidth == 16)
                {
                    ui32MipiCfg = MIPICFG_16RGB565_OPT0;
                }
                if (ui8DbiWidth ==  8)
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

    i32Ret = ambiq_init(ui32MipiCfg);
    if (i32Ret != 0)
    {
        return i32Ret;
    }

    return 0;
}

int am_benchmarks()
{
    int i32TestNo = 0;

    init();

    if (i32TestNo != 0)
    {
        run_bench(i32TestNo);
    }
    else
    {
        while (1)
        {
            for (int i32Test = 1; i32Test <= TEST_MAX; ++i32Test)
            {
                run_bench(i32Test);
            }
        }
    }

    return 0;
}
