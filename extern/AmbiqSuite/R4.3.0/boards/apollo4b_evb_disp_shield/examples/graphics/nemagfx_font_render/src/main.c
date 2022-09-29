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
#include "am_hal_global.h"
#include "string.h"
#include "nema_font.h"
#include "am_util_delay.h"

#ifndef DONT_USE_NEMADC
#include "nema_dc.h"
#include "nema_dc_mipi.h"
#endif

#include "am_devices_nemadc_rm67162.h"
#include "am_devices_dsi_rm67162.h"
#include "nema_programHW.h"

#ifndef USE_1BIT_FONT
#define USE_1BIT_FONT 1
#endif
#ifndef USE_2BIT_FONT
#define USE_2BIT_FONT 1
#endif
#ifndef USE_4BIT_FONT
#define USE_4BIT_FONT 1
#endif
#ifndef USE_8BIT_FONT
#define USE_8BIT_FONT 1
#endif

#define NEMA_FONT_IMPLEMENTATION
#if USE_1BIT_FONT != 0
    #include "DejaVuSerif8pt1b.h"
#endif
#if USE_2BIT_FONT != 0
    #include "DejaVuSerif8pt2b.h"
#endif
#if USE_4BIT_FONT != 0
    #include "DejaVuSerif8pt4b.h"
#endif
#if USE_8BIT_FONT != 0
    #include "DejaVuSerif8pt8b.h"
#endif

#include "simhei16pt1b.h"
#include "simhei16pt2b.h"
#include "simhei16pt4b.h"
#include "simhei16pt8b.h"

#undef NEMA_FONT_IMPLEMENTATION

#define RESX 390
#define RESY 390

static img_obj_t g_sFB = {{0}, RESX, RESY, RESX * 2, 0, NEMA_RGB565, 0};
static nemadc_layer_t g_DCLayer = {(void *)0, 0, RESX, RESY, RESX * 2, 0, 0, RESX, RESY, 0xff, NEMADC_BL_SRC, 0, NEMADC_RGB565, 0, 0, 0, 0, 0, 0, 0};

static void
load_objects(void)
{
    //
    // Load memory objects
    //
    g_sFB.bo = nema_buffer_create(g_sFB.stride * (int32_t)g_sFB.h);
    (void)nema_buffer_map(&g_sFB.bo);

    g_DCLayer.baseaddr_phys = g_sFB.bo.base_phys;
    g_DCLayer.baseaddr_virt = g_sFB.bo.base_virt;

#if USE_1BIT_FONT != 0
    g_sDejaVuSerif8pt1b.bo = nema_buffer_create(g_sDejaVuSerif8pt1b.bitmap_size);
    (void)nema_buffer_map(&g_sDejaVuSerif8pt1b.bo);
    (void)nema_memcpy(g_sDejaVuSerif8pt1b.bo.base_virt, g_sDejaVuSerif8pt1b.bitmap, (size_t)g_sDejaVuSerif8pt1b.bitmap_size);
#endif

#if USE_2BIT_FONT != 0
    g_sDejaVuSerif8pt2b.bo = nema_buffer_create(g_sDejaVuSerif8pt2b.bitmap_size);
    (void)nema_buffer_map(&g_sDejaVuSerif8pt2b.bo);
    (void)nema_memcpy(g_sDejaVuSerif8pt2b.bo.base_virt, g_sDejaVuSerif8pt2b.bitmap, (size_t)g_sDejaVuSerif8pt2b.bitmap_size);
#endif

#if USE_4BIT_FONT != 0
    g_sDejaVuSerif8pt4b.bo = nema_buffer_create(g_sDejaVuSerif8pt4b.bitmap_size);
    (void)nema_buffer_map(&g_sDejaVuSerif8pt4b.bo);
    (void)nema_memcpy(g_sDejaVuSerif8pt4b.bo.base_virt, g_sDejaVuSerif8pt4b.bitmap, (size_t)g_sDejaVuSerif8pt4b.bitmap_size);
#endif

#if USE_8BIT_FONT != 0
    g_sDejaVuSerif8pt8b.bo = nema_buffer_create(g_sDejaVuSerif8pt8b.bitmap_size);
    (void)nema_buffer_map(&g_sDejaVuSerif8pt8b.bo);
    (void)nema_memcpy(g_sDejaVuSerif8pt8b.bo.base_virt, g_sDejaVuSerif8pt8b.bitmap, (size_t)g_sDejaVuSerif8pt8b.bitmap_size);
#endif
    simhei16pt1b.bo = nema_buffer_create(simhei16pt1b.bitmap_size);
    (void)nema_memcpy(simhei16pt1b.bo.base_virt, simhei16pt1b.bitmap, (size_t)simhei16pt1b.bitmap_size);

    simhei16pt2b.bo = nema_buffer_create(simhei16pt2b.bitmap_size);
    (void)nema_memcpy(simhei16pt2b.bo.base_virt, simhei16pt2b.bitmap, (size_t)simhei16pt2b.bitmap_size);

    simhei16pt4b.bo = nema_buffer_create(simhei16pt4b.bitmap_size);
    (void)nema_memcpy(simhei16pt4b.bo.base_virt, simhei16pt4b.bitmap, (size_t)simhei16pt4b.bitmap_size);

    simhei16pt8b.bo = nema_buffer_create(simhei16pt8b.bitmap_size);
    (void)nema_memcpy(simhei16pt8b.bo.base_virt, simhei16pt8b.bitmap, (size_t)simhei16pt8b.bitmap_size);

}

int32_t
font_render()
{
     load_objects();

    // (void)nema_event_init(0, 0, 0, 0, 0);

    //Format        | Pixclock | RESX | FP | SYNC | BP | RESY | FP | SYNC | BP
    //800x600, 60Hz | 40.000   | 800  | 40 | 128  | 88 | 600  | 1  | 4    | 23
    //nemadc_timing(800, 40, 128, 88, 600, 1, 4, 23);
    //nemadc_set_layer(0, &g_DCLayer);

    nema_cmdlist_t sCL;
    //
    // Create Command Lists
    //
    sCL = nema_cl_create();
    //
    // Bind Command List
    //
    nema_cl_bind(&sCL);
    //
    // Bind Framebuffer
    //
    nema_bind_dst_tex(g_sFB.bo.base_phys, g_sFB.w, g_sFB.h, g_sFB.format, g_sFB.stride);
    //
    // Set Clipping Rectangle
    //
    nema_set_clip(0, 0, RESX, RESY);
    nema_set_blend_fill(NEMA_BL_SRC);
    //
    // Fill Rectangle with Color
    //
    nema_fill_rect(0, 0, RESX, RESY, 0x10101010);

    char str[] = "0 bpp - Hello World!!!\n|-------------------------------|\nThe quick brown fox jumps     over the lazy dog!\n\
ThisIsAVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryLongString\nH NemaGFX υποστηρίζει ΚΑΙ Unicode χαρακτήρες!!!";

    int32_t w, h;

#if   USE_1BIT_FONT != 0
    nema_bind_font(&g_sDejaVuSerif8pt1b);
#elif USE_2BIT_FONT != 0
    nema_bind_font(&g_sDejaVuSerif8pt2b);
#elif USE_4BIT_FONT != 0
    nema_bind_font(&g_sDejaVuSerif8pt4b);
#elif USE_8BIT_FONT != 0
    nema_bind_font(&g_sDejaVuSerif8pt8b);
#endif

    const int32_t spacing = 5;

    (void)nema_string_get_bbox(str, (int *)(&w), (int *)(&h), RESX / 2 - spacing * 3, 1);
    h += spacing;
    w += spacing;

    int32_t x, y;

    const int32_t xs[4] = {spacing, w + 2 * spacing, spacing, w + 2 * spacing};
    const int32_t ys[4] = {spacing, spacing, h + 2 * spacing, h + 2 * spacing};
    int32_t idx = 0;

#if USE_1BIT_FONT != 0
    x = xs[idx];
    y = ys[idx];
    ++idx;

    str[0] = '1';

    nema_set_blend_fill(NEMA_BL_SRC);
    nema_fill_rect(x, y, w, h, 0x70707070U);
    nema_bind_font(&g_sDejaVuSerif8pt1b);
    nema_print(str, x, y, w, h, 0xff00ff80U, NEMA_ALIGNX_LEFT | NEMA_TEXT_WRAP | NEMA_ALIGNY_TOP);

    nema_cl_submit(&sCL);
    (void)nema_cl_wait(&sCL);
    nemadc_set_layer(0, &g_DCLayer);
    if (g_sDispCfg[g_eDispType].eInterface == IF_DSI)
    {
        dsi_send_frame_single(NEMADC_OUTP_OFF);
    }
    else
    {
        nemadc_send_frame_single();
    }

    nema_cl_rewind(&sCL);
#endif

#if USE_2BIT_FONT != 0
    x = xs[idx];
    y = ys[idx];
    ++idx;

    str[0] = '2';

    nema_set_blend_fill(NEMA_BL_SRC);
    nema_fill_rect(x, y, w, h, 0x70707070U);
    nema_bind_font(&g_sDejaVuSerif8pt2b);
    nema_print(str, x, y, w, h, 0xff00ffffU, NEMA_ALIGNX_RIGHT | NEMA_TEXT_WRAP | NEMA_ALIGNY_BOTTOM);

    nema_cl_submit(&sCL);
    (void)nema_cl_wait(&sCL);
    nemadc_set_layer(0, &g_DCLayer);
    if (g_sDispCfg[g_eDispType].eInterface == IF_DSI)
    {
        dsi_send_frame_single(NEMADC_OUTP_OFF);
    }
    else
    {
        nemadc_send_frame_single();
    }

    nema_cl_rewind(&sCL);
#endif

#if USE_4BIT_FONT != 0
    x = xs[idx];
    y = ys[idx];
    ++idx;

    str[0] = '4';

    nema_set_blend_fill(NEMA_BL_SRC);
    nema_fill_rect(x, y, w, h, 0x70707070U);
    nema_bind_font(&g_sDejaVuSerif8pt4b);
    nema_print(str, x, y, w, h, 0xff0080ffU, NEMA_ALIGNX_CENTER | NEMA_TEXT_WRAP | NEMA_ALIGNY_CENTER);

    nema_cl_submit(&sCL);
    (void)nema_cl_wait(&sCL);
    nemadc_set_layer(0, &g_DCLayer);
    if (g_sDispCfg[g_eDispType].eInterface == IF_DSI)
    {
        dsi_send_frame_single(NEMADC_OUTP_OFF);
    }
    else
    {
        nemadc_send_frame_single();
    }
    nema_cl_rewind(&sCL);
#endif

#if USE_8BIT_FONT != 0

    x = xs[idx];
    y = ys[idx];
    ++idx;

    str[0] = '8';

    nema_set_blend_fill(NEMA_BL_SRC);
    nema_fill_rect(x, y, w, h, 0x70707070U);
    nema_bind_font(&g_sDejaVuSerif8pt8b);
    nema_print(str, x, y, w, h, 0x808ff08fU, NEMA_ALIGNX_JUSTIFY | NEMA_TEXT_WRAP | NEMA_ALIGNY_JUSTIFY);

    nema_cl_submit(&sCL);
    (void)nema_cl_wait(&sCL);
    nemadc_set_layer(0, &g_DCLayer);
    if (g_sDispCfg[g_eDispType].eInterface == IF_DSI)
    {
        dsi_send_frame_single(NEMADC_OUTP_OFF);
    }
    else
    {
        nemadc_send_frame_single();
    }

#endif

    am_util_delay_ms(5000);

// start for chinese font display
    nema_clear(0x00000000);

    char str_chinese[] = "这是汉字库图形的例程\
                          这是汉字库图形的例程\
                          这是汉字库图形的例程\
                          这是汉字库图形的例程\
                          这是汉字库图形的例程\
                          这是汉字库图形的例程\
                          这是汉字库图形的例程\
                          这是汉字库图形的例程\n";
    idx = 0;
/* first */
    x = xs[idx];
    y = ys[idx];
    ++idx;

    nema_set_blend_fill(NEMA_BL_SRC);
    nema_fill_rect(x, y, w, h, 0x70707070U);
    nema_bind_font(&simhei16pt1b);
    nema_print(str_chinese, x, y, w, h, 0xff00ff80U, NEMA_ALIGNX_RIGHT | NEMA_TEXT_WRAP | NEMA_ALIGNY_BOTTOM);

    nema_cl_submit(&sCL);
    (void)nema_cl_wait(&sCL);
    nemadc_set_layer(0, &g_DCLayer);
    if (g_sDispCfg[g_eDispType].eInterface == IF_DSI)
    {
        dsi_send_frame_single(NEMADC_OUTP_OFF);
    }
    else
    {
        nemadc_send_frame_single();
    }

    nema_cl_rewind(&sCL);

/* second         */
    x = xs[idx];
    y = ys[idx];
    ++idx;

    nema_set_blend_fill(NEMA_BL_SRC);
    nema_fill_rect(x, y, w, h, 0x70707070U);
    nema_bind_font(&simhei16pt2b);
    nema_print(str_chinese, x, y, w, h, 0xff00ffffU, NEMA_ALIGNX_RIGHT | NEMA_TEXT_WRAP | NEMA_ALIGNY_BOTTOM);

    nema_cl_submit(&sCL);
    (void)nema_cl_wait(&sCL);
    nemadc_set_layer(0, &g_DCLayer);
    if (g_sDispCfg[g_eDispType].eInterface == IF_DSI)
    {
        dsi_send_frame_single(NEMADC_OUTP_OFF);
    }
    else
    {
        nemadc_send_frame_single();
    }

    nema_cl_rewind(&sCL);

/* third         */
    x = xs[idx];
    y = ys[idx];
    ++idx;

    nema_set_blend_fill(NEMA_BL_SRC);
    nema_fill_rect(x, y, w, h, 0x70707070U);
    nema_bind_font(&simhei16pt4b);
    nema_print(str_chinese, x, y, w, h, 0xff0080ffU, NEMA_ALIGNX_RIGHT  | NEMA_TEXT_WRAP | NEMA_ALIGNY_BOTTOM);

    nema_cl_submit(&sCL);
    (void)nema_cl_wait(&sCL);
    nemadc_set_layer(0, &g_DCLayer);
    if (g_sDispCfg[g_eDispType].eInterface == IF_DSI)
    {
        dsi_send_frame_single(NEMADC_OUTP_OFF);
    }
    else
    {
        nemadc_send_frame_single();
    }

    nema_cl_rewind(&sCL);

/* fourth         */
    x = xs[idx];
    y = ys[idx];
    ++idx;

    nema_set_blend_fill(NEMA_BL_SRC);
    nema_fill_rect(x, y, w, h, 0x70707070U);
    nema_bind_font(&simhei16pt8b);
    nema_print(str_chinese, x, y, w, h, 0x808ff08fU, NEMA_ALIGNX_RIGHT  | NEMA_TEXT_WRAP | NEMA_ALIGNY_BOTTOM);

    nema_cl_submit(&sCL);
    (void)nema_cl_wait(&sCL);
    nemadc_set_layer(0, &g_DCLayer);
    if (g_sDispCfg[g_eDispType].eInterface == IF_DSI)
    {
        dsi_send_frame_single(NEMADC_OUTP_OFF);
    }
    else
    {
        nemadc_send_frame_single();
    }

    nema_cl_rewind(&sCL);

    nema_cl_destroy(&sCL);
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


    font_render();

    return 0;
}

