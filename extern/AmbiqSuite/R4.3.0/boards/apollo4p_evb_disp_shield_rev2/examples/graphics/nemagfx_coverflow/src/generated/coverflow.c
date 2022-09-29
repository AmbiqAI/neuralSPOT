//*****************************************************************************
//
//! @file coverflow.c
//! @brief coverflow example.
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
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "nema_core.h"
#include "nema_utils.h"
#include "nema_dc.h"

#include "nema_programHW.h"
#include "nema_easing.h"
#include "nema_event.h"
#include "nema_raster.h"
#include "am_util_stdio.h"

#include "nema_dc.h"
#include "nema_dc_mipi.h"

#include "am_devices_dsi_rm67162.h"
#include "am_devices_nemadc_rm67162.h"
#include "coverflow.h"
#include "im01_128x128_rgba.h"
#include "im02_128x128_rgba.h"
#include "im03_128x128_rgba.h"
#include "im04_128x128_rgba.h"

#ifndef AM_PART_APOLLO4P

#else
//#include "am_hal_stimer.h"
//#include "apollo4p.h"
#include "am_mcu_apollo.h"
#endif
//*****************************************************************************
//
//! *Porting notes
//! * 1)define RESX, RESY, FRAME_BUFFERS, IMG_SIZE, IMG_FORMAT, IMAGE_COUNT
//! * 2)MODE_INTERRACTIVE: coverflow works using mouse input, otherwise draw frames continuously
//! * 3)Modify "load_mult_objects" function (frame-buffer format, stride, input images path)
//
//*****************************************************************************
#define COUNT_FPS
// #define MODE_INTERRACTIVE
#ifndef RESX
#define RESX 320
#endif

#ifndef RESY
#define RESY 320
#endif

#ifndef DEBUG_OVERDRAWS
#define DEBUG_OVERDRAWS 0
#endif

#define FRAME_BUFFERS 2

#define IMG_SIZE      128

#define IMAGE_COUNT   4
#define M_PI          3.14159265358979323846

#define IMAGE_ADDRESS1 (MSPI_XIP_BASE_ADDRESS + ui8Img128x128RGBA01Length)
#define IMAGE_ADDRESS2 (MSPI_XIP_BASE_ADDRESS + ui8Img128x128RGBA01Length + ui8Img128x128RGBA02Length)
#define IMAGE_ADDRESS3 (MSPI_XIP_BASE_ADDRESS + ui8Img128x128RGBA01Length + ui8Img128x128RGBA02Length + ui8Img128x128RGBA03Length)

#ifdef BAREMETAL

    #define FB_GPU  0
    #define FB_DC   1
    #define FB_FREE 2

    #define NUM_LAYERS 1U

    static uintptr_t triple_fbs[NUM_LAYERS][2];

    uintptr_t
    nema_init_triple_fb(int layer, uintptr_t fb0_phys, uintptr_t fb1_phys, uintptr_t fb2_phys)
    {
        //actually doing always 2 framebuffers
        //g_sFB2_phys is ignored

        triple_fbs[layer][FB_GPU]  = fb0_phys;
        triple_fbs[layer][FB_DC]   = fb1_phys;
        // triple_fbs[layer][FB_FREE] = fb2_phys;

        return triple_fbs[layer][FB_GPU];
    }

    uintptr_t
    nema_swap_fb(int layer)
    {
        if (layer < 0)
        {
            layer = 0;
        }

        {
            uintptr_t tmp = triple_fbs[layer][FB_DC];
            triple_fbs[layer][FB_DC] = triple_fbs[layer][FB_GPU];
            triple_fbs[layer][FB_GPU]  = tmp;

            // nemadc_wait_vsync();
            nemadc_set_layer_addr(layer, triple_fbs[layer][FB_DC]);
        }
        return triple_fbs[layer][FB_GPU];
    }
#endif

typedef struct _quad_t
{
    float x0, y0, z0, w0;
    float x1, y1, z1, w1;
    float x2, y2, z2, w2;
    float x3, y3, z3, w3;
}
Quad_t;

static img_obj_t g_sFB[FRAME_BUFFERS];
static img_obj_t g_sImgsRGBA[IMAGE_COUNT];

static nemadc_layer_t g_sLayer[FRAME_BUFFERS] = {{0}};

static void
load_mult_objects(void)
{
    //
    // Allocate Framebuffers
    //
    for (int32_t i = 0; i < FRAME_BUFFERS; ++i)
    {
        g_sFB[i].w      = RESX;
        g_sFB[i].h      = RESY;
        g_sFB[i].format = NEMA_RGB565;
        g_sFB[i].stride = nema_stride_size(g_sFB[i].format, 0, g_sFB[i].w);
        g_sFB[i].bo     = nema_buffer_create(nema_texture_size(g_sFB[i].format, 0, g_sFB[i].w, g_sFB[i].h));
        nema_buffer_map(&g_sFB[i].bo);

        g_sLayer[i].format        = NEMADC_RGB565;
        g_sLayer[i].sizex         = g_sLayer[i].resx = g_sFB[i].w;
        g_sLayer[i].sizey         = g_sLayer[i].resy = g_sFB[i].h;
        g_sLayer[i].stride        = g_sFB[i].stride;
        g_sLayer[i].blendmode     = NEMADC_BL_SRC;
        g_sLayer[i].baseaddr_phys = g_sFB[i].bo.base_phys;
        g_sLayer[i].baseaddr_virt = g_sFB[i].bo.base_virt;
        g_sLayer[i].flipx_en      = 0;
        g_sLayer[i].flipy_en      = 0;

        am_util_stdio_printf("FB: V:%p P:0x%08x\n", (void *)g_sFB[i].bo.base_virt, g_sFB[i].bo.base_phys);
    }

    nemadc_set_bgcolor(0x40404040);
    uint32_t ui32Color = 0x50505050;

    // Texture is already loaded to memory, use it directly
    // No need to used nema_buffer_create()
    // nema_buffer_create() - like malloc() - only allocates an empty buffer
    // When using nema_buffer_create, make sure to memcpy the original texture to the
    //     allocated buffer
#ifdef LOAD_FROM_PSRAM
    g_sImgsRGBA[0].bo.base_virt = (void *)MSPI_XIP_BASE_ADDRESS;
    g_sImgsRGBA[1].bo.base_virt = (void *)IMAGE_ADDRESS1;
    g_sImgsRGBA[2].bo.base_virt = (void *)IMAGE_ADDRESS2;
    g_sImgsRGBA[3].bo.base_virt = (void *)IMAGE_ADDRESS3;

    g_sImgsRGBA[0].bo.base_phys = (uintptr_t)g_sImgsRGBA[0].bo.base_virt;
    g_sImgsRGBA[1].bo.base_phys = (uintptr_t)g_sImgsRGBA[1].bo.base_virt;
    g_sImgsRGBA[2].bo.base_phys = (uintptr_t)g_sImgsRGBA[2].bo.base_virt;
    g_sImgsRGBA[3].bo.base_phys = (uintptr_t)g_sImgsRGBA[3].bo.base_virt;
#else
    g_sImgsRGBA[0].bo = nema_buffer_create(sizeof(ui8Img128x128RGBA01));
    g_sImgsRGBA[1].bo = nema_buffer_create(sizeof(ui8Img128x128RGBA02));
    g_sImgsRGBA[2].bo = nema_buffer_create(sizeof(ui8Img128x128RGBA03));
    g_sImgsRGBA[3].bo = nema_buffer_create(sizeof(ui8Img128x128RGBA04));
#endif
    //g_sImgsRGBA[0].bo.base_phys = (uintptr_t)&ui8Img128x128RGBA01[0];

    memcpy(g_sImgsRGBA[0].bo.base_virt, (void *)ui8Img128x128RGBA01, sizeof(ui8Img128x128RGBA01));
    g_sImgsRGBA[0].format = NEMA_RGBA8888;
    g_sImgsRGBA[0].w      = IMG_SIZE;
    g_sImgsRGBA[0].h      = IMG_SIZE;
    g_sImgsRGBA[0].stride = nema_stride_size(g_sImgsRGBA[0].format, 0, g_sImgsRGBA[0].w);
    g_sImgsRGBA[0].sampling_mode =  0;
    nemadc_set_bgcolor(ui32Color);
    ui32Color += 0x10101010;

    //g_sImgsRGBA[1].bo.base_phys = (uintptr_t)&ui8Img128x128RGBA02[0];

    memcpy(g_sImgsRGBA[1].bo.base_virt, (void *)ui8Img128x128RGBA02, sizeof(ui8Img128x128RGBA02));
    g_sImgsRGBA[1].format = NEMA_RGBA8888;
    g_sImgsRGBA[1].w      = IMG_SIZE;
    g_sImgsRGBA[1].h      = IMG_SIZE;
    g_sImgsRGBA[1].stride = nema_stride_size(g_sImgsRGBA[1].format, 0, g_sImgsRGBA[1].w);
    g_sImgsRGBA[1].sampling_mode =  0;
    nemadc_set_bgcolor(ui32Color);
    ui32Color += 0x10101010;

    //g_sImgsRGBA[2].bo.base_phys = (uintptr_t)&ui8Img128x128RGBA03[0];

    memcpy(g_sImgsRGBA[2].bo.base_virt, (void *)ui8Img128x128RGBA03, sizeof(ui8Img128x128RGBA03));
    g_sImgsRGBA[2].format = NEMA_RGBA8888;
    g_sImgsRGBA[2].w      = IMG_SIZE;
    g_sImgsRGBA[2].h      = IMG_SIZE;
    g_sImgsRGBA[2].stride = nema_stride_size(g_sImgsRGBA[2].format, 0, g_sImgsRGBA[2].w);
    g_sImgsRGBA[2].sampling_mode =  0;
    nemadc_set_bgcolor(ui32Color);
    ui32Color += 0x10101010;

    //g_sImgsRGBA[3].bo.base_phys = (uintptr_t)&ui8Img128x128RGBA04[0];

    memcpy(g_sImgsRGBA[3].bo.base_virt, (void *)ui8Img128x128RGBA04, sizeof(ui8Img128x128RGBA04));
    g_sImgsRGBA[3].format = NEMA_RGBA8888;
    g_sImgsRGBA[3].w      = IMG_SIZE;
    g_sImgsRGBA[3].h      = IMG_SIZE;
    g_sImgsRGBA[3].stride = nema_stride_size(g_sImgsRGBA[3].format, 0, g_sImgsRGBA[3].w);
    g_sImgsRGBA[3].sampling_mode =  0;
    nemadc_set_bgcolor(ui32Color);
    ui32Color += 0x10101010;
}

static inline void
bind_img(img_obj_t *psImg)
{
    nema_set_tex_color(0);
    nema_bind_src_tex(psImg->bo.base_phys, psImg->w, psImg->h, psImg->format, psImg->stride, NEMA_FILTER_BL | NEMA_TEX_BORDER);
}

static float FoV    = 30.f;
static float y_orig = 0;
static float x_orig = 0;

static nema_matrix4x4_t proj_matrix;
//
// Transform coordinates from [-1,1] to [0, RESX] and [0, RESY]
//
static void
transform_quad_coords(Quad_t *psQ)
{
    nema_mat4x4_obj_to_win_coords(proj_matrix, x_orig, y_orig, RESX, RESY, -1, 1, &psQ->x0, &psQ->y0, &psQ->z0, &psQ->w0);
    nema_mat4x4_obj_to_win_coords(proj_matrix, x_orig, y_orig, RESX, RESY, -1, 1, &psQ->x1, &psQ->y1, &psQ->z1, &psQ->w1);
    nema_mat4x4_obj_to_win_coords(proj_matrix, x_orig, y_orig, RESX, RESY, -1, 1, &psQ->x2, &psQ->y2, &psQ->z2, &psQ->w2);
    nema_mat4x4_obj_to_win_coords(proj_matrix, x_orig, y_orig, RESX, RESY, -1, 1, &psQ->x3, &psQ->y3, &psQ->z3, &psQ->w3);
}

static void
calc_circle(float *x_off, float *z_off, float angle, float r)
{
    *x_off = nema_cos(angle)*r;
    *z_off = nema_sin(angle)*r;
}

const float img_width     = 0.2f;
const float img_angle_deg = 90.f;
const float back_rail_z   = 0.98f;
const float front_img_z   = 0.7f;

static float front_rail_z;
static float shift_par0, shift_par3;
static float r_big, r_small;
static float centre_right_x, img_x_proj;

static void
cover_flow_init(void)
{
    nema_mat4x4_load_perspective(proj_matrix, FoV, (float)RESX / RESY, 1, 100);

    img_x_proj = nema_cos(img_angle_deg) * img_width;
    float img_z_proj = nema_sin(img_angle_deg) * img_width;

    front_rail_z = back_rail_z - img_z_proj;

    r_big   = (back_rail_z  - front_img_z) / nema_sin(img_angle_deg);
    r_small = (front_rail_z - front_img_z) / nema_sin(img_angle_deg);

    float x0_front_x = -img_width / 2;

    centre_right_x = x0_front_x + r_big;

    float right_critical_x0 = centre_right_x - nema_cos(img_angle_deg) * r_big;

    shift_par0 = -(right_critical_x0 + img_x_proj - 1) * 0.5f;
    shift_par3 = 1.f - shift_par0;

#if 0
    float centre_right_z = front_img_z;

    float centre_left_x  = x0_front_x - r_small;
    float centre_left_z  = front_img_z;
    float right_critical_z0 = back_rail_z;
    float right_critical_x1 = centre_right_x-nema_cos(img_angle_deg)*r_small;
    float right_critical_z1 = front_rail_z;

    float x0_front_z = front_img_z;

    float x1_front_x = img_width/2;
    float x1_front_z = front_img_z;
    printf("%f\n", img_width    );
    printf("%f\n", img_angle_deg);
    printf("%f\n", img_x_proj);
    printf("%f\n", img_z_proj);
    printf("%f\n", back_rail_z  );
    printf("%f\n", front_img_z  );
    printf("%f\n", front_rail_z);
    printf("%f\n", r_big  );
    printf("%f\n", r_small);

    printf("\n");

    printf("%f\t%f\n", x0_front_x       , x0_front_z       );
    printf("%f\t%f\n", x1_front_x       , x1_front_z       );
    printf("%f\t%f\n", centre_right_x   , centre_right_z   );
    printf("%f\t%f\n", centre_left_x    , centre_left_z    );
    printf("%f\t%f\n", right_critical_x0, right_critical_z0);
    printf("%f\t%f\n", right_critical_x1, right_critical_z1);

    printf("\n");

    printf("%f\n", shift_par0);
    printf("%f\n", shift_par3);
#endif
}

static void
map_quad_points(Quad_t *psQ, float shift_par)
{
    if (shift_par < shift_par0)
    {
        //0 - 0.3
        psQ->x0 = -2 * shift_par + 1 - img_x_proj;
        psQ->z0 = back_rail_z;

        psQ->x1 = psQ->x0 + img_x_proj;
        psQ->z1 = front_rail_z;
    }
    else if (shift_par < 0.5f)
    {
        //shift_par0 - 0.5
        //angle 71.6 - 0
        float angle = nema_ez( img_angle_deg, 0.f, 0.5f-shift_par0, shift_par-shift_par0, &nema_ez_linear);
        calc_circle(&psQ->x0, &psQ->z0, angle, r_big);

        psQ->x0 = centre_right_x - psQ->x0;
        psQ->z0 = front_img_z + psQ->z0;

        calc_circle(&psQ->x1, &psQ->z1, angle, r_small);

        psQ->x1 = centre_right_x - psQ->x1;
        psQ->z1 = front_img_z + psQ->z1;
    }
    else if (shift_par < shift_par3)
    {
        //0.5 - shift_par3
        //angle 0 - 71.6
        float angle = nema_ez( 0.f, img_angle_deg, shift_par3-0.5f, shift_par-0.5f, &nema_ez_linear);
        calc_circle(&psQ->x0, &psQ->z0, angle, r_small);

        psQ->x0 = -centre_right_x + psQ->x0;
        psQ->z0 = front_img_z + psQ->z0;

        calc_circle(&psQ->x1, &psQ->z1, angle, r_big);

        psQ->x1 = -centre_right_x + psQ->x1;
        psQ->z1 = front_img_z + psQ->z1;
    }
    else
    {
        //0.7 - 1.0
        psQ->x0 = -2 * shift_par + 1;
        psQ->z0 = front_rail_z;

        psQ->x1 = psQ->x0 + img_x_proj;
        psQ->z1 = back_rail_z;
    }

    ///X
    psQ->x2 = psQ->x1;
    psQ->x3 = psQ->x0;

    ///Y
    psQ->y0 = psQ->y1 = -0.5f*img_width;
    psQ->y2 = psQ->y3 =  0.5f*img_width;

    ///Z
    psQ->z2 = psQ->z1;
    psQ->z3 = psQ->z0;

    ///W
    psQ->w0 = psQ->w1 = psQ->w2 = psQ->w3 = 1.f;
}

static float
map_to_ratio(float val, float break_point, float laska)
{
    float ret_val = 0.f;

    if (val <= break_point)
    {
        return val / (1.f + laska);
    }

    ret_val += break_point;
    val     -= break_point;

    float before_laska = (0.5f - break_point) * 2.f;
    float after_laska  = (0.5f - break_point) * 2.f + laska;

    if (val < before_laska)
    {
        ret_val += val * after_laska / before_laska;
        return ret_val / (1.f + laska);
    }

    ret_val += after_laska;

    val     -= before_laska;
    ret_val += val;

    return ret_val / (1.f + laska);
}

#ifdef STANDALONE
int
main(void)
#else
int32_t
cover_flow()
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

#ifndef BAREMETAL
    nema_event_init(1, 0, 0, RESX, RESY);
#endif

    load_mult_objects();

    nemadc_layer_disable(1);
    nemadc_layer_disable(2);
    nemadc_cursor_enable(0);
    nemadc_set_layer(0, &g_sLayer[1]);

    uintptr_t cur_fb_base_phys = nema_init_triple_fb(0, g_sLayer[0].baseaddr_phys,
                                                  g_sLayer[1].baseaddr_phys,
                                                  0);
    cover_flow_init();

    nema_cmdlist_t sCL0 = nema_cl_create();
    nema_cmdlist_t sCL1 = nema_cl_create();

    nema_cmdlist_t* psCLs[2] = {&sCL0, &sCL1};
    nema_cmdlist_t* psCurCL = psCLs[0];

    nema_cl_bind(&sCL0);
    nema_set_clip(0, 0, RESX, RESY);
//****************************************************************
//
// Background
//
//****************************************************************
#if defined(BAREMETAL) && defined(APOLLO4_FPGA) && defined(AM_PART_APOLLO4P)
    am_hal_stimer_config(AM_HAL_STIMER_HFRC_6MHZ | AM_HAL_STIMER_CFG_RUN);
#endif
    for (int32_t i = 0; i < FRAME_BUFFERS; ++i)
    {
        nema_set_blend_fill(NEMA_BL_SRC);
        nema_bind_dst_tex(g_sFB[i].bo.base_phys, g_sFB[i].w, g_sFB[i].h, g_sFB[i].format, g_sFB[i].stride);
        nema_fill_rect(0, 0, RESX, RESY, 0);

//
//        #if RESX == 800 && RESY == 600 && !defined(PLATFORM_SYNOPSYS)
//            nema_set_blend_blit(NEMA_BL_SRC);
//            nema_bind_src_tex(TSi_logo.bo.base_phys, TSi_logo.w, TSi_logo.h, TSi_logo.format, TSi_logo.stride, NEMA_FILTER_BL);
//            nema_blit_rect_fit(RESX/2 - TSi_logo.w/4, 30, TSi_logo.w/2, TSi_logo.h/2);
//        #endif
//
    }
    nema_cl_submit(&sCL0);
    nema_cl_wait(&sCL0);
//****************************************************************
//
// Coverflow
//
//****************************************************************

    float shift_par;
#ifdef COUNT_FPS
    float sec0 = nema_get_time();
#endif

    int32_t ui32Frames = 0;
    double secs = 0.5;
    int32_t i32CurImg = 0;

    nema_cmdlist_t sCLBg = nema_cl_create();
    nema_cl_bind(&sCLBg);

#if (DEBUG_OVERDRAWS == 0)
    nema_enable_gradient(0);
#endif
    nema_set_blend_fill(NEMA_BL_SRC);

    uint32_t ui32ColStep = 0x20000;
    uint32_t ui32ColInit = 0x080000;
    nema_set_gradient_fx(ui32ColInit, ui32ColInit, ui32ColInit, 0xff0000,
                         0, ui32ColStep,
                         0, ui32ColStep,
                         0, ui32ColStep,
                         0, 0);

    nema_fill_rect(0, RESY>>1, RESX, RESY>>1, 0x0);
    nema_enable_gradient(0);
    nema_fill_rect(0, 0, RESX, RESY>>1, 0x0);

    float sec_init     = 0.f;
    double cur_sec      = 0.f;

#ifdef MODE_INTERRACTIVE
    nema_event_t event = {0};
#endif

    while (1)
    {

        psCurCL = psCLs[ui32Frames & 0x1];

        nema_cl_bind(psCurCL);
        nema_cl_rewind(psCurCL);

        nema_enable_aa(1, 1, 1, 1);
        //
        // Optimize clip for known resolutions
        //
        if (RESX == 800 && RESY == 600)
        {
            nema_set_clip(0, 140, RESX, RESY-140);
        }
        else
        {
            nema_set_clip(0, 0, RESX, RESY);
        }
        //
        // Bind Framebuffer
        //
        nema_bind_dst_tex(cur_fb_base_phys, g_sFB[0].w, g_sFB[0].h, g_sFB[0].format, g_sFB[0].stride);
        //
        // Clear screen before printing items on updated positions
        //
        nema_cl_branch(&sCLBg);

#if (DEBUG_OVERDRAWS != 0)
    nema_debug_overdraws(1U);
#endif
        //
        // Redraw gradient on Synopsys, when using the gyroscope
        //
#if defined(MODE_INTERRACTIVE) && defined(PLATFORM_SYNOPSYS)
        nema_set_blend_fill(NEMA_BL_SRC);
        nema_enable_gradient(0);
        nema_set_gradient_fx(ui32ColInit, ui32ColInit, ui32ColInit, 0xff0000,
                             0, ui32ColStep,
                             0, ui32ColStep,
                             0, ui32ColStep,
                             0, 0 );

        nema_fill_rect(0, (RESY >> 1) - (int)roll, RESX, (RESY >> 1) + (int)roll, 0x0);
        nema_enable_gradient(0);
        nema_fill_rect(0, 0, RESX, (RESY>>1)-(int)roll, 0x0);
#endif

        nema_set_const_color( nema_rgba(50, 50, 50, 0x80) );
        //
        // STOP_COUNT: number of images inside frustum
        //
#define STOP_COUNT (40)

        const float break_point = 0.5f - 1.f / STOP_COUNT;
        float clip_left  = RESX;
        float clip_right = 0;

        for (int32_t i32Stop = STOP_COUNT-1; i32Stop >= 0; --i32Stop)
        {
            int i32StopIdx = (i32Stop & 0x1) ? STOP_COUNT - ((i32Stop + 1) >> 1) : i32Stop >> 1 ;
            int img_idx  = (i32CurImg + i32StopIdx + IMAGE_COUNT * 10 - (STOP_COUNT >> 1)) % IMAGE_COUNT;

            if (i32StopIdx > STOP_COUNT >> 1)
            {
                if (clip_right >= RESX)
                {
                    continue;
                }
                //
                // right side pictures
                //
                nema_set_clip((int32_t)clip_right, 0, (uint32_t)(RESX - clip_right), RESY);
            }
            else
            { //if ( i32StopIdx > STOP_COUNT>>1)
                if (clip_left < 0)
                {
                    continue;
                }
                //
                // left side pictures
                //
                nema_set_clip(0, 0, (uint32_t)clip_left, RESY);
            }

            float shift_target, shift_prev;

            shift_target = nema_ez(.5f, 0.f, STOP_COUNT >> 1, i32StopIdx,     nema_ez_linear) + 0.5f;
            shift_prev   = nema_ez(.5f, 0.f, STOP_COUNT >> 1, i32StopIdx + 1, nema_ez_linear) + 0.5f;

            shift_target = map_to_ratio(shift_target, break_point, 0.19f);

            shift_prev   = map_to_ratio(shift_prev, break_point, 0.19f);

            shift_par    = nema_ez( shift_prev, shift_target, 1.0, cur_sec, &nema_ez_linear);

            Quad_t q1;
            map_quad_points(&q1, shift_par);

            transform_quad_coords(&q1);
            //
            // make sides of quads integers so that clipping is done right
            //
            q1.x0 = q1.x3 = (int32_t) q1.x0;
            q1.x1 = q1.x2 = (int32_t) q1.x1;
            //
            // Reflections
            //
            nema_set_blend_blit(NEMA_BL_SIMPLE | NEMA_BLOP_MODULATE_A);
            bind_img(&g_sImgsRGBA[(img_idx) % IMAGE_COUNT]);

            float screen_img_height_0 = q1.y3 - q1.y0;
            float screen_img_height_1 = q1.y2 - q1.y1;

            nema_blit_quad_fit(q1.x0, q1.y0 + 2.f * screen_img_height_0,
                               q1.x1, q1.y1 + 2.f * screen_img_height_1,
                               q1.x2, q1.y2,
                               q1.x3, q1.y3);
            //
            // Images
            //
            nema_set_blend_blit(NEMA_BL_SIMPLE);
            nema_blit_quad_fit(q1.x0, q1.y0,
                               q1.x1, q1.y1,
                               q1.x2, q1.y2,
                               q1.x3, q1.y3);

            if (i32StopIdx > STOP_COUNT >> 1)
            {
                //
                // right side pictures
                //
                clip_right = q1.x1;
            }
            else if (i32StopIdx < STOP_COUNT >> 1)
            {
                //
                // left side pictures
                //
                clip_left = q1.x0;
            }
            else
            {
                clip_left  = q1.x0;
                clip_right = q1.x1;
            }

            if (clip_right >= RESX && clip_left < 0)
            {
                break;
            }

        }

        nema_cl_submit(psCurCL);

#ifdef COUNT_FPS
        ++ui32Frames;
        if (ui32Frames % 100 == 0)
        {
            float sec1 = nema_get_time();
            am_util_stdio_printf("sec1 = %f, fps: %.2f\n", sec1, (100.f / (sec1 - sec0)));
            sec0 = nema_get_time();
        }
#endif

#ifdef MODE_INTERRACTIVE
        static float d_cur_sec = 0.f;
        static float d_FoV     = 0.f;

        nema_event_wait(&event, 0);

        //Fixed-numders tuned for Synopsys platform
        if ( event.mouse_event == MOUSE_EVENT_NONE && (event.mouse_state == MOUSE_STATE_LEFT_CLICKED))
        {
            d_cur_sec -= event.mouse_dx / 90.f;
            if (d_cur_sec > 0.9f)
            {
                d_cur_sec = 0.9f;
            }
            else if (d_cur_sec < -0.9f)
            {
                d_cur_sec = -0.9f;
            }

        }
        else if (event.mouse_event == MOUSE_EVENT_SCROLL_UP)
        {
            d_FoV -= 1.f;
        }
        else if (event.mouse_event == MOUSE_EVENT_SCROLL_DOWN)
        {
             d_FoV += 1.f;
        }

        cur_sec   += d_cur_sec;
        d_cur_sec *= 0.8f;

        FoV   += d_FoV;

        //Swipe momentum
        d_FoV *= 0.8f;

        if (FoV < 20.f)
        {
            FoV   = 20.f;
            d_FoV = 0.f;
        }
        else if (FoV > 35.f)
        {
            FoV   = 35.f;
            d_FoV = 0.f;
        }

        nema_mat4x4_load_perspective(proj_matrix, FoV, (float)RESX / RESY, 1, 100);

        //Continuous mode (not interractive)
#else
        //
        // Small cur_sec => slow transition (does NOT affect the actual fps)
        //
        cur_sec += 0.05;
#endif

        if (cur_sec > secs + 0.5)
        {
            cur_sec  -= secs + 0.5;
            sec_init += secs + 0.5;
            i32CurImg   = (i32CurImg + 1) % IMAGE_COUNT;
        }
        else if (cur_sec < 0.f)
        {
            cur_sec  += secs + 0.5;
            sec_init -= secs + 0.5;
            i32CurImg   = (i32CurImg + IMAGE_COUNT - 1) % IMAGE_COUNT;
        }

        //nema_cl_wait(psCurCL);

        if (g_sDispCfg[g_eDispType].eInterface == IF_DSI)
        {
            dsi_send_frame_single(NEMADC_OUTP_OFF);
        }
        else
        {
            nemadc_send_frame_single();
        }

        nema_cl_wait(psCurCL);

        cur_fb_base_phys = nema_swap_fb(0);
    }
    // #endif
}
