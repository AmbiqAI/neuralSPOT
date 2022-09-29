//*****************************************************************************
//
//! @file rotating_crate.c
//!
//! @brief NemaGFX example.
//! this example shows a rotating crate with texture rending support. with Nema
//! GPU support, it can significantly reduce the general CPU effort to calculate
//! the data inside the frame buffer
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
#include "nema_matrix4x4.h"
#include "nema_math.h"
#include "nema_utils.h"

#ifndef DONT_USE_NEMADC
#include "nema_dc.h"
#endif

#include "am_devices_dsi_rm67162.h"
#include "am_devices_nemadc_rm67162.h"

#ifndef RESX
#define RESX 320
#endif

#ifndef RESY
#define RESY 320
#endif

#ifndef COLOR_CUBE
#define COLOR_CUBE 0
#endif

typedef enum
{
    PRIM_LINE,
    PRIM_RECT,
    PRIM_TRI,
    PRIM_QUAD,
    PRIM_MAX
}
Primitive_e;

#define FRAME_BUFFERS 2
img_obj_t g_sFB[FRAME_BUFFERS];

#ifndef DONT_USE_NEMADC
nemadc_layer_t g_sLayer[FRAME_BUFFERS] = {{0}};
#endif

#define CRATE_FORMAT NEMA_RGB565

#include "crate_320x320_rgba565.h"

img_obj_t g_sCrate = {{0}, RESX, RESY, -1, 0, CRATE_FORMAT, 0};

void
load_objects(void)
{
    int32_t i;
    for (i = 0; i < FRAME_BUFFERS; ++i)
    {
        g_sFB[i].w = RESX;
        g_sFB[i].h = RESY;
        g_sFB[i].format = NEMA_RGB565;
        g_sFB[i].stride = RESX * 2;
        g_sFB[i].bo = nema_buffer_create(g_sFB[i].stride * g_sFB[i].h);
        nema_buffer_map(&g_sFB[i].bo);

#ifndef DONT_USE_NEMADC
        g_sLayer[i].sizex = g_sLayer[i].resx = g_sFB[i].w;
        g_sLayer[i].sizey = g_sLayer[i].resy = g_sFB[i].h;
        g_sLayer[i].stride = g_sFB[i].stride;
        g_sLayer[i].format = NEMADC_RGB565;
        g_sLayer[i].blendmode = NEMADC_BF_ONE;
        g_sLayer[i].baseaddr_phys = g_sFB[i].bo.base_phys;
        g_sLayer[i].baseaddr_virt = g_sFB[i].bo.base_virt;
        g_sLayer[i].flipx_en      = 0;
        g_sLayer[i].flipy_en      = 0;
#endif
    }

    // g_sCrate.bo = nema_load_file(CRATE_PATH, -1, (void *)0);

    g_sCrate.bo = nema_buffer_create(g_i32Crate320x320RGBA565Length);
    nema_memcpy(g_sCrate.bo.base_virt, g_ui8Crate320x320RGBA565, g_i32Crate320x320RGBA565Length);
}

//int32_t mouse_x = 0;
//int32_t mouse_y = 0;

void
draw_cube_side(float *v, int32_t v0, int32_t v1, int32_t v2, int32_t v3, uint32_t col)
{
#if COLOR_CUBE
    //fill with color
    nema_fill_quad(v[v0 * 3], v[v0 * 3 + 1],
                   v[v1 * 3], v[v1 * 3 + 1],
                   v[v2 * 3], v[v2 * 3 + 1],
                   v[v3 * 3], v[v3 * 3 + 1], col);
#else
    //blit
    nema_blit_quad_fit(v[v0 * 3], v[v0 * 3 + 1],
                   v[v1 * 3], v[v1 * 3 + 1],
                   v[v2 * 3], v[v2 * 3 + 1],
                   v[v3 * 3], v[v3 * 3 + 1]);
#endif
}

void
innercube(int32_t i32AngleX, int32_t i32AngleY, int32_t i32AngleZ)
{
    float box_size_2 = 0.2f;
    float FoV = 28.0724869359f;
    float v[] =
    {               //x     y    z
                    -box_size_2, -box_size_2,  box_size_2,  //0  0
                     box_size_2, -box_size_2,  box_size_2,  //1  3
                     box_size_2,  box_size_2,  box_size_2,  //2  6
                    -box_size_2,  box_size_2,  box_size_2,  //3  9
                    -box_size_2, -box_size_2, -box_size_2,  //4  12
                     box_size_2, -box_size_2, -box_size_2,  //5  15
                     box_size_2,  box_size_2, -box_size_2,  //6  18
                    -box_size_2,  box_size_2, -box_size_2   //7  21
    };

    //!< projection
    nema_matrix4x4_t mvp;

    nema_mat4x4_load_perspective(mvp, FoV, (float)RESX / RESY, 0.2f, 100.f);

    nema_matrix4x4_t proj;
    nema_mat4x4_load_identity(proj);
    nema_mat4x4_rotate_X(proj, i32AngleX);
    nema_mat4x4_rotate_Y(proj, i32AngleY);
    nema_mat4x4_rotate_Z(proj, i32AngleZ);
    nema_mat4x4_translate(proj, 0, 0, 2.f-box_size_2);

    nema_mat4x4_mul(mvp, mvp, proj);

    float w = 1.f;
    int32_t i;

    for (i = 0; i < 24; i += 3)
    {
        nema_mat4x4_obj_to_win_coords(mvp, 0.f, 0.f, RESX, RESY,
                                      1.f, 100.f,
                                      &v[i  ], &v[i + 1], &v[i + 2], &w);
    }

    //!< blend color with background
#if COLOR_CUBE
    nema_set_blend_fill(NEMA_BL_SIMPLE);
#else
    nema_set_blend_blit(NEMA_BL_SRC);
#endif
    nema_bind_src_tex(g_sCrate.bo.base_phys, g_sCrate.w, g_sCrate.h, g_sCrate.format, g_sCrate.stride, NEMA_FILTER_BL);

    //!< remove this to draw back sides also
    nema_tri_cull(NEMA_CULL_CW);
    draw_cube_side(v, 0, 1, 2, 3, 0x60ffffff); //!< front
    draw_cube_side(v, 4, 0, 3, 7, 0x600000ff); //!< left
    draw_cube_side(v, 1, 5, 6, 2, 0x60ff00ff); //!< right
    draw_cube_side(v, 4, 5, 1, 0, 0x60ff0000); //!< top
    draw_cube_side(v, 3, 2, 6, 7, 0x6000ff00); //!< bottom
    draw_cube_side(v, 5, 4, 7, 6, 0x60808080); //!< back
    nema_tri_cull(NEMA_CULL_NONE);
}

int32_t i32LastFB = -1;
int32_t i32CurFB = 0;
uintptr_t cur_fb_base_phys = 0;

void
swap_buffers(void)
{
    i32LastFB = i32CurFB;
    i32CurFB = (i32CurFB + 1) % FRAME_BUFFERS;
    cur_fb_base_phys = g_sFB[i32CurFB].bo.base_phys;
}

nema_cmdlist_t g_sCL;

void
nema_gfx_display_function()
{
    static int32_t i32AngleX = 0.f;
    static int32_t i32AngleY = 0.f;
    static int32_t i32AngleZ = 80.f;

    i32AngleX = (i32AngleX + 1) % 360;
    i32AngleY = (i32AngleY + 2) % 360;
    i32AngleZ = (i32AngleZ + 2) % 360;

    nema_cl_bind(&g_sCL);
    nema_cl_rewind(&g_sCL);

    nema_set_clip(0, 0, RESX, RESY);
    nema_bind_dst_tex(cur_fb_base_phys, g_sFB[0].w, g_sFB[0].h, g_sFB[0].format, g_sFB[0].stride);

    nema_clear(0);
    innercube(i32AngleX, i32AngleY, i32AngleZ);

    nema_cl_submit(&g_sCL);

    if (i32LastFB >= 0)
    {
//        nemadc_set_layer(0, &empty_layer);
//#ifdef ENABLE_DSI
//        dsi_send_frame_single(NEMADC_OUTP_OFF);
//#else
//        nemadc_send_frame_single();
//#endif

#ifndef DONT_USE_NEMADC
        nemadc_set_layer(0, &g_sLayer[i32LastFB]);
#endif

        if (g_sDispCfg[g_eDispType].eInterface == IF_DSI)
        {
            dsi_send_frame_single(NEMADC_OUTP_OFF);
        }
        else
        {
            nemadc_send_frame_single();
        }

    }

    nema_cl_wait(&g_sCL);
    swap_buffers();
}

#ifdef STANDALONE
int
main(void)
#else
int32_t
rotating_crate()
#endif
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

    //!< Set the display region to center
    if ( RESX > ui16PanelResX )
    {
        ui16MinX = 0;   //!< set the minimum value to 0
    }
    else
    {
        ui16MinX = (ui16PanelResX - RESX) >> 1;
        ui16MinX = (ui16MinX >> 1) << 1;
    }

    if ( RESY > ui16PanelResY )
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

    load_objects();

    cur_fb_base_phys = g_sFB[i32CurFB].bo.base_phys;

    //!< Create Command Lists
    g_sCL  = nema_cl_create();
#if defined(BAREMETAL) && defined(APOLLO4_FPGA) && defined(AM_PART_APOLLO4P)
    am_hal_stimer_config(AM_HAL_STIMER_HFRC_6MHZ | AM_HAL_STIMER_CFG_RUN);
#endif
    while (1)
    {
        nema_gfx_display_function();
        nema_calculate_fps();
    }
}
