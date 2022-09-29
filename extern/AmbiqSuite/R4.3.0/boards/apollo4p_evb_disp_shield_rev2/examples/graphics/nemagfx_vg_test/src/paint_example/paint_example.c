//*****************************************************************************
//
//! @file paint_example.c
//!
//! @brief Demo to show different paint types.
//!
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
#include "nemagfx_vg_test.h"

#ifdef RUN_PAINT_EXAMPLE

#include "paint.h"
#include "ref.rgba.h"

//*****************************************************************************
//
// Macro definitions
//
//*****************************************************************************
//! Screen resolution
#define RESX 452
#define RESY 452

//*****************************************************************************
//
// Global Variables
//
//*****************************************************************************
//! framebuffrer
static nema_img_obj_t g_sFramebuffer =
{
    {0}, RESX, RESY, RESX*4, 0, NEMA_RGBA8888, 0
};

//! texture
static nema_img_obj_t g_sRefImage =
{
    {0}, REF_IMG_W, REF_IMG_H, REF_IMG_W*4, 0, NEMA_RGBA8888, NEMA_FILTER_BL
};

//! DC layer
static nemadc_layer_t g_sDCLayer =
{
    (void *)0, 0, RESX, RESY, -1, 0, 0, RESX, RESY, 0xff,
    NEMADC_BL_SRC, 0, NEMADC_RGBA8888, 0, 0, 0, 0, 0, 0, 0
};

//Matrix
nema_matrix3x3_t m_path;
nema_matrix3x3_t m_paint;

float star_x_off = 10.f, star_y_off = 10.f;
float star_length = 80.f;
float star_height = 100.f;

//*****************************************************************************
//
//! @brief Create buffer, load image from MRAM to ssram
//!
//
//*****************************************************************************
static void bufferCreate(void)
{
    //Load memory objects
    g_sFramebuffer.bo = nema_buffer_create( g_sFramebuffer.stride*g_sFramebuffer.h);
    nema_buffer_map(&g_sFramebuffer.bo);
    printf("FB: V:%p P:0x%08x\n", (void *)g_sFramebuffer.bo.base_virt, g_sFramebuffer.bo.base_phys);

    g_sRefImage.bo = nema_buffer_create(ref_rgba_len);
    memcpy(g_sRefImage.bo.base_virt, ref_rgba, ref_rgba_len);

    //set dc layer memory
    g_sDCLayer.baseaddr_phys = g_sFramebuffer.bo.base_phys;
    g_sDCLayer.baseaddr_virt = g_sFramebuffer.bo.base_virt;

    printf("FB: V:%p P:0x%08x\n", (void *)g_sFramebuffer.bo.base_virt, g_sFramebuffer.bo.base_phys);
}

//*****************************************************************************
//
//! @brief Destroy buffers
//!
//
//*****************************************************************************
static void bufferDestroy(void)
{
    //Destroy memory objects
    nema_buffer_destroy(&g_sRefImage.bo);

    nema_buffer_unmap(&g_sFramebuffer.bo);
    nema_buffer_destroy(&g_sFramebuffer.bo);
}

//*****************************************************************************
//
//! @brief calculate matrix
//!
//
//*****************************************************************************
static void
matrixCalculate(nema_matrix3x3_t m, float angle_degrees, float x, float y)
{
    float cosa = nema_cos(angle_degrees);
    float sina = nema_sin(angle_degrees);

    m[0][0] = cosa;
    m[0][1] = sina;
    m[0][2] = x - (x*cosa) - (y*sina);
    m[1][0] = -sina;
    m[1][1] = cosa;
    m[1][2] = y + (x*sina) - (y*cosa);
}

//*****************************************************************************
//
//! @brief draw texture
//!
//
//*****************************************************************************
static void
draw_star_texture(NEMA_VG_PATH_HANDLE path, NEMA_VG_PAINT_HANDLE paint, float x_pos, float y_pos, float degree)
{
    float texture_center_x = REF_IMG_W / 2;
    float texture_center_y = REF_IMG_H / 2;

    nema_mat3x3_load_identity(m_path);
    nema_mat3x3_translate(m_path, x_pos, y_pos);
    nema_vg_path_set_matrix(path, m_path);

    nema_mat3x3_load_identity(m_paint);
    matrixCalculate(m_paint, degree, texture_center_x, texture_center_y);
    nema_mat3x3_translate(m_paint, x_pos + star_x_off, y_pos + star_y_off);
    nema_vg_paint_set_tex_matrix(paint, m_paint);

    nema_vg_draw_path(path, paint);
}

//*****************************************************************************
//
//! @brief paint example
//!
//
//*****************************************************************************
int paint_example(void)
{
    //Initialize NemaGFX
    if ( nema_init() != 0 )
    {
        return -1;
    }

    //Init NemaVG
    nema_vg_init(RESX, RESY);

    //Initialize NemaDC
    if ( display_setup(RESX, RESY) != 0 )
    {
        return -2;
    }

    //Create buffer
    bufferCreate();

    //Set layer
    nemadc_set_layer(0, &g_sDCLayer);

#if 0
    // Use circular cl
    nema_cmdlist_t cl  = nema_cl_create_sized(1024);
    nema_cl_bind_circular(&cl);
#else
    nema_cmdlist_t cl  = nema_cl_create();
    nema_cl_bind(&cl);
#endif

    //clear framebuffer
    nema_bind_dst_tex(g_sFramebuffer.bo.base_phys, g_sFramebuffer.w, g_sFramebuffer.h, g_sFramebuffer.format, g_sFramebuffer.stride);
    nema_set_clip(0, 0, RESX, RESY);
    nema_clear(nema_rgba(0xff, 0xff, 0x00, 0xff)); //yellow

    //Create path and set path, this path will be reused with different offset.
    NEMA_VG_PATH_HANDLE path = nema_vg_path_create();
    nema_vg_path_set_shape(path, 6, cmds_star, 12, data_star_small);

    //Create paint
    NEMA_VG_PAINT_HANDLE paint = nema_vg_paint_create();

    //Create gradient and set gradient, this gradient will be reused by different paint.
    NEMA_VG_GRAD_HANDLE gradient = nema_vg_grad_create();
    nema_vg_grad_set(gradient, COLOR_STOPS, stops, colors);

    //Set up VG context
    nema_vg_set_fill_rule(NEMA_VG_FILL_EVEN_ODD);
    nema_vg_set_blend(NEMA_BL_SRC_OVER);

    //Path offset
    float x_pos;
    float y_pos;

    /**********************Paint: linear gradient******************************************/
    //Set paint to linear gradient
    nema_vg_paint_set_type(paint, NEMA_VG_PAINT_GRAD_LINEAR);

    // Use the same paint with different gradient cordinate.
    // use the same path with different offset.

    for ( int i = 0; i < STARS_IN_ONE_LINE ; i++ )
    {

        x_pos = STAR_DIST_X * i;

        y_pos = 0.f;
        nema_mat3x3_load_identity(m_path);
        nema_mat3x3_translate(m_path, x_pos, y_pos);
        nema_vg_path_set_matrix(path, m_path);

        nema_vg_paint_set_grad_linear(paint, gradient, grad[i][0] + x_pos, grad[i][1] + y_pos, grad[i][2] + x_pos, grad[i][3] + y_pos, NEMA_TEX_CLAMP | NEMA_FILTER_BL);
        nema_vg_set_fill_rule(NEMA_VG_FILL_EVEN_ODD);
        nema_vg_draw_path(path, paint);

        y_pos = STAR_DIST_Y;
        nema_mat3x3_load_identity(m_path);
        nema_mat3x3_translate(m_path, x_pos, y_pos);
        nema_vg_path_set_matrix(path, m_path);

        nema_vg_paint_set_grad_linear(paint, gradient, grad[i][0] + x_pos, grad[i][1] + y_pos, grad[i][2] + x_pos, grad[i][3] + y_pos, NEMA_TEX_CLAMP | NEMA_FILTER_BL);
        nema_vg_set_fill_rule(NEMA_VG_FILL_NON_ZERO);
        nema_vg_draw_path(path, paint);
    }

    /**********************Paint:texture**********************************************/

    nema_vg_paint_set_type(paint, NEMA_VG_PAINT_TEXTURE);
    nema_vg_paint_set_tex(paint, &g_sRefImage);
    float degree[STARS_IN_ONE_LINE] = {
                                         0.f,   45.f,
                                         90.f, 135.f,
                                        180.f, 225.f,
                                        270.f, 315.f
                                      };

    for ( int i = 0; i < STARS_IN_ONE_LINE ; i++ )
    {
        x_pos = STAR_DIST_X * i;

        y_pos = STAR_DIST_Y * 2;
        nema_vg_set_fill_rule(NEMA_VG_FILL_EVEN_ODD);
        draw_star_texture(path, paint, x_pos, y_pos, degree[i]);

        y_pos = STAR_DIST_Y*3;
        nema_vg_set_fill_rule(NEMA_VG_FILL_NON_ZERO);
        draw_star_texture(path, paint, x_pos, y_pos, degree[i]);
    }

    y_pos = STAR_DIST_Y*4;

    /**********************Paint: CONIC GRADIENT**************************************/
    nema_vg_paint_set_type(paint, NEMA_VG_PAINT_GRAD_CONICAL);

    float x_star_center;
    float y_star_center;

    //calculate and set path matrix
    x_pos = 0.f;
    x_star_center = x_pos + star_length / 2 + star_x_off;
    y_star_center = y_pos + star_height / 2 + star_y_off + 12.f;
    nema_mat3x3_load_identity(m_path);
    nema_mat3x3_translate(m_path, x_pos, y_pos);
    nema_vg_path_set_matrix(path, m_path);

    nema_vg_set_fill_rule(NEMA_VG_FILL_EVEN_ODD);

    //set conical cordinate
    nema_vg_paint_set_grad_conical(paint, gradient, x_star_center, y_star_center, NEMA_TEX_CLAMP  | NEMA_FILTER_BL);

    //Draw path
    nema_vg_draw_path(path, paint);

    //Calculate and set path matrix
    x_pos = STAR_DIST_X;
    x_star_center = x_pos + star_length / 2 + star_x_off;
    y_star_center = y_pos + star_height / 2 + star_y_off + 12.f;
    nema_mat3x3_load_identity(m_path);
    nema_mat3x3_translate(m_path, x_pos, y_pos);
    nema_vg_path_set_matrix(path, m_path);

    nema_vg_set_fill_rule(NEMA_VG_FILL_NON_ZERO);

    //set conical cordinate
    nema_vg_paint_set_grad_conical(paint, gradient, x_star_center, y_star_center, NEMA_TEX_CLAMP | NEMA_FILTER_BL);

    //Draw path
    nema_vg_draw_path(path, paint);

    /**********************Paint: RADIAL GRADIENT***************************************/
    //We doesn't support this gradient now.

    /**********************Paint: COLOR FILL********************************************/
    nema_vg_paint_set_type(paint, NEMA_VG_PAINT_FILL);

    //Calculate and set path matrix
    x_pos = STAR_DIST_X*4;
    nema_mat3x3_load_identity(m_path);
    nema_mat3x3_translate(m_path, x_pos, y_pos);
    nema_vg_path_set_matrix(path, m_path);

    nema_vg_set_fill_rule(NEMA_VG_FILL_EVEN_ODD);

    //set paint color
    nema_vg_paint_set_paint_color(paint, nema_rgba(0x00, 0xff, 0x000, 0xff)); // green

    //draw path
    nema_vg_draw_path(path, paint);

    //Calculate and set path matrix
    x_pos = STAR_DIST_X*5;
    nema_mat3x3_load_identity(m_path);
    nema_mat3x3_translate(m_path, x_pos, y_pos);
    nema_vg_path_set_matrix(path, m_path);

    nema_vg_set_fill_rule(NEMA_VG_FILL_NON_ZERO);

    //set paint color
    nema_vg_paint_set_paint_color(paint, nema_rgba(0xff, 0x00, 0x000, 0xff)); // red

    //draw path
    nema_vg_draw_path(path, paint);

    /**********************Paint: DRAW OUTLINE********************************************/
    nema_vg_paint_set_type(paint, NEMA_VG_PAINT_FILL);

    //Calculate and set path matrix
    x_pos = STAR_DIST_X*6;
    nema_mat3x3_load_identity(m_path);
    nema_mat3x3_translate(m_path, x_pos, y_pos);
    nema_vg_path_set_matrix(path, m_path);

    //Use this fill rule to draw outline
    nema_vg_set_fill_rule(NEMA_VG_FILL_DRAW);

    //Set outline color
    nema_vg_paint_set_paint_color(paint, nema_rgba(0xff, 0x00, 0x000, 0xff)); // red

    //Draw path
    nema_vg_draw_path(path, paint);

    /**********************Paint: draw outline with width*******************************/
    nema_vg_paint_set_type(paint, NEMA_VG_PAINT_FILL);

    //Calculate and set path matrix
    x_pos = STAR_DIST_X*7;
    nema_mat3x3_load_identity(m_path);
    nema_mat3x3_translate(m_path, x_pos, y_pos);
    nema_vg_path_set_matrix(path, m_path);

    //Use NEMA_VG_FILL_DRAW and set a stroke >1
    nema_vg_set_fill_rule(NEMA_VG_FILL_DRAW);
    nema_vg_paint_set_stroke_width(paint, 5.f);

    //Set outline color
    nema_vg_paint_set_paint_color(paint, nema_rgba(0xff, 0x00, 0x000, 0xff)); // red

    //draw path
    nema_vg_draw_path(path, paint);

    //submit cl.
    nema_cl_submit(&cl);
    nema_cl_wait(&cl);

    //Start DC
    display_refresh_start();
    //Wait DC complete interrupt.
    nemadc_wait_vsync();
    //Do follow-up operations required by hardware.
    display_refresh_end();

    //Release resource
    nema_vg_paint_destroy(paint);
    nema_vg_path_destroy(path);
    nema_vg_grad_destroy(gradient);

    nema_vg_deinit();
    bufferDestroy();

    return 0;
}

#endif
