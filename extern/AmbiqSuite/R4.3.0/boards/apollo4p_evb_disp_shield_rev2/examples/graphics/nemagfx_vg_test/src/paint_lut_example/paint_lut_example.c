//*****************************************************************************
//
//! @file paint_lut_example.h
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

#include "nemagfx_vg_test.h"

#ifdef RUN_PAINT_LUT_EXAMPLE

#include "nema_core.h"
#include "nema_utils.h"
#include "nema_vg.h"

#include "ThinkSilicon_indices.16.h"
#include "ThinkSilicon_indices.256.h"
#include "ThinkSilicon_indices.2.h"
#include "ThinkSilicon_indices.4.h"
#include "ThinkSilicon_palette.16.h"
#include "ThinkSilicon_palette.256.h"
#include "ThinkSilicon_palette.2.h"
#include "ThinkSilicon_palette.4.h"


#ifndef DONT_USE_NEMADC
#include "nema_dc.h"
#endif

#define RESX 452
#define RESY 452

#define TEXX 400
#define TEXY 400

//#define CIRCLE
#define STAR
//#define ELLIPSE
//#define RING

nema_img_obj_t fb = {{0}, RESX, RESY, RESX*4, 0, NEMA_RGBA8888, 0};

nema_img_obj_t palette8 = {{0}, 16, 16, -1, 0, NEMA_RGBA8888, 0};
nema_img_obj_t palette4 = {{0}, 4,   4, -1, 0, NEMA_RGBA8888, 0};
nema_img_obj_t palette2 = {{0}, 2,   2, -1, 0, NEMA_RGBA8888, 0};
nema_img_obj_t palette1 = {{0}, 1,   2, -1, 0, NEMA_RGBA8888, 0};

nema_img_obj_t indices8 = {{0}, TEXX, TEXY, -1, 0, NEMA_L8, NEMA_TEX_CLAMP};
nema_img_obj_t indices4 = {{0}, TEXX, TEXY, -1, 0, NEMA_L4, NEMA_TEX_CLAMP};
nema_img_obj_t indices2 = {{0}, TEXX, TEXY, -1, 0, NEMA_L2, NEMA_TEX_CLAMP};
nema_img_obj_t indices1 = {{0}, TEXX, TEXY, -1, 0, NEMA_L1, NEMA_TEX_CLAMP};

uint8_t cmds_polygon[] = {NEMA_VG_PRIM_MOVE, NEMA_VG_PRIM_POLYGON};

float star_coords[] = {100.0f, 10.0f,
                        8.0f  ,
                        40.0f, 198.0f,
                        190.0f, 78.0f,
                        10.0f, 78.0f,
                        160.0f, 198.0f};

#ifndef DONT_USE_NEMADC
nemadc_layer_t dc_layer = {(void *)0, 0, RESX, RESY, -1, 0, 0, RESX, RESY, 0xff, NEMADC_BL_SRC, 0, NEMADC_RGBA8888, 0, 0, 0, 0, 0, 0, 0};
#endif

int paint_lut_example()
{
    // Initialize NemaGFX
    if ( nema_init() != 0 )
    {
        return -1;
    }

#ifndef DONT_USE_NEMADC
    //Initialize NemaDC
    if ( display_setup(RESX, RESY) != 0 )
    {
        return -2;
    }
#endif
    // Initialize memory objects
    fb.bo = nema_buffer_create(fb.stride*fb.h);
    nema_buffer_map(&fb.bo);

    //palette8.bo = nema_load_file("ThinkSilicon_palette.256.rgba", -1, NULL);
    palette8.bo = nema_buffer_create(sizeof(ThinkSilicon_palette_256));
    memcpy(palette8.bo.base_virt, ThinkSilicon_palette_256, sizeof(ThinkSilicon_palette_256));
    //indices8.bo  = nema_load_file("ThinkSilicon_indices.256.gray", -1, NULL);
    indices8.bo = nema_buffer_create(sizeof(ThinkSilicon_indices_256));
    memcpy(indices8.bo.base_virt, ThinkSilicon_indices_256, sizeof(ThinkSilicon_indices_256));
    //palette4.bo = nema_load_file("ThinkSilicon_palette.16.rgba", -1, NULL);
    palette4.bo = nema_buffer_create(sizeof(ThinkSilicon_palette_16));
    memcpy(palette4.bo.base_virt, ThinkSilicon_palette_16, sizeof(ThinkSilicon_palette_16));
    //indices4.bo  = nema_load_file("ThinkSilicon_indices.16.gray", -1, NULL);
    indices4.bo = nema_buffer_create(sizeof(ThinkSilicon_indices_16));
    memcpy(indices4.bo.base_virt, ThinkSilicon_indices_16, sizeof(ThinkSilicon_indices_16));
    //palette2.bo = nema_load_file("ThinkSilicon_palette.4.rgba", -1, NULL);
    palette2.bo = nema_buffer_create(sizeof(ThinkSilicon_palette_4));
    memcpy(palette2.bo.base_virt, ThinkSilicon_palette_4, sizeof(ThinkSilicon_palette_4));
    //indices2.bo  = nema_load_file("ThinkSilicon_indices.4.gray", -1, NULL);
    indices2.bo = nema_buffer_create(sizeof(ThinkSilicon_indices_4));
    memcpy(indices2.bo.base_virt, ThinkSilicon_indices_4, sizeof(ThinkSilicon_indices_4));
    //palette1.bo = nema_load_file("ThinkSilicon_palette.2.rgba", -1, NULL);
    palette1.bo = nema_buffer_create(sizeof(ThinkSilicon_palette_2));
    memcpy(palette1.bo.base_virt, ThinkSilicon_palette_2, sizeof(ThinkSilicon_palette_2));
    //indices1.bo  = nema_load_file("ThinkSilicon_indices.2.gray", -1, NULL);
    indices1.bo = nema_buffer_create(sizeof(ThinkSilicon_indices_2));
    memcpy(indices1.bo.base_virt, ThinkSilicon_indices_2, sizeof(ThinkSilicon_indices_2));

#ifndef DONT_USE_NEMADC
    dc_layer.baseaddr_phys = fb.bo.base_phys;
    dc_layer.baseaddr_virt = fb.bo.base_virt;
#endif

#ifndef DONT_USE_NEMADC
    //Format        | Pixclock | RESX | FP | SYNC | BP | RESY | FP | SYNC | BP
    //800x600, 60Hz | 40.000   | 800  | 40 | 128  | 88 | 600  | 1  | 4    | 23
    //nemadc_timing(800, 40, 128, 88, 600, 1, 4, 23);
    nemadc_set_layer(0, &dc_layer);
#endif

    nema_cmdlist_t cl  = nema_cl_create();
    nema_cl_bind(&cl);

    // Clear fb
    nema_bind_dst_tex(fb.bo.base_phys, fb.w, fb.h, fb.format, fb.stride);
    nema_set_clip(0, 0, RESX, RESY);

    nema_clear(nema_rgba(0xff, 0xff, 0xff, 0xff));
    //Set Tex Color for NEMA_TEX_BORDER sampling type (first Color of the Palette)
    nema_set_tex_color(0U);

    nema_matrix3x3_t m_paint, m_path;

    // Init VG library
    nema_vg_init(RESX, RESY);

    // Create paint
    NEMA_VG_PAINT_HANDLE paint = nema_vg_paint_create();
    nema_vg_paint_set_type(paint, NEMA_VG_PAINT_TEXTURE);
    nema_vg_set_fill_rule(NEMA_VG_FILL_EVEN_ODD);

#ifdef STAR
    // Create path and set it to a star using polygon segment
    NEMA_VG_PATH_HANDLE path_polygon = nema_vg_path_create();
    nema_vg_path_set_shape(path_polygon, 2, cmds_polygon, 11, star_coords);
#endif

#ifdef RECT
    //Draw Rect
    nema_mat3x3_load_identity(m_paint);
    //nema_mat3x3_scale(m_paint, 0.5, 0.5);
    nema_mat3x3_translate(m_paint, 50, 50);
    nema_vg_paint_set_tex_matrix(paint, m_paint);
    nema_vg_paint_set_lut_tex(paint, &palette1, &indices1);
    nema_vg_draw_rect(50.0f, 50.0f, 400.0f, 400.0f, NULL, paint);
#endif

#ifdef CIRCLE
    //Draw Circle
    nema_mat3x3_load_identity(m_paint);
    //nema_mat3x3_scale(m_paint, 0.5, 0.5);
    //nema_mat3x3_translate(m_paint, 100, 100);
    nema_vg_paint_set_tex_matrix(paint, m_paint);
    nema_vg_paint_set_lut_tex(paint, &palette8, &indices8);
    nema_vg_draw_circle(RESX / 2, RESY / 2, RESX / 2, NULL, paint);
#endif

#ifdef ELLIPSE
    //Draw Ellipse
    nema_mat3x3_load_identity(m_paint);
    nema_mat3x3_scale(m_paint, 0.5, 0.5);
    nema_mat3x3_translate(m_paint, 56, 0);
    nema_vg_paint_set_tex_matrix(paint, m_paint);
    nema_vg_paint_set_lut_tex(paint, &palette2, &indices2);
    nema_vg_draw_ellipse(250, 100, 100, 50, NULL, paint);
#endif

#ifdef RING
    //Draw Ring
    nema_mat3x3_load_identity(m_paint);
    nema_vg_paint_set_stroke_width(paint, 80.f);
    nema_mat3x3_scale(m_paint, 0.5, 0.5);
    //nema_mat3x3_translate(m_paint, 155, 390);
    nema_vg_paint_set_tex_matrix(paint, m_paint);
    nema_vg_paint_set_lut_tex(paint, &palette8, &indices8);
    nema_vg_draw_ring(RESX / 2, 200, 200, 0, 180, paint);
#endif

#ifdef STAR
    // Draw Star Path
    nema_vg_set_fill_rule(NEMA_VG_FILL_NON_ZERO);
    nema_mat3x3_load_identity(m_path);
    nema_mat3x3_load_identity(m_paint);
    nema_mat3x3_scale(m_path, 2, 2);
    nema_mat3x3_translate(m_path, 20, 20);
   //ma_mat3x3_scale(m_paint, 0.5, 0.5);
   //ma_mat3x3_translate(m_paint, 515, 315);
    nema_vg_paint_set_tex_matrix(paint, m_paint);
    nema_vg_paint_set_lut_tex(paint, &palette8, &indices8);
    nema_vg_path_set_matrix(path_polygon, m_path);
    nema_vg_draw_path(path_polygon, paint);
#endif

    // Submit command list
    nema_cl_submit(&cl);
    nema_cl_wait(&cl);

    //Start DC
    display_refresh_start();
    //Wait DC complete interrupt.
    nemadc_wait_vsync();
    //Do follow-up operations required by hardware.
    display_refresh_end();

    //nema_save_file("fb.rgba",RESX*RESY*4, fb.bo.base_virt);

    // Destroy memory object (Deallocate memory)
    nema_vg_paint_destroy(paint);
    nema_vg_deinit();

    nema_buffer_unmap(&fb.bo);
    nema_buffer_destroy(&fb.bo);
    nema_buffer_destroy(&palette8.bo);
    nema_buffer_destroy(&indices8.bo);
    nema_buffer_destroy(&palette4.bo);
    nema_buffer_destroy(&indices4.bo);
    nema_buffer_destroy(&palette2.bo);
    nema_buffer_destroy(&indices2.bo);
    nema_buffer_destroy(&palette1.bo);
    nema_buffer_destroy(&indices1.bo);

    nema_cl_destroy(&cl);

    return 0;
}

#endif
