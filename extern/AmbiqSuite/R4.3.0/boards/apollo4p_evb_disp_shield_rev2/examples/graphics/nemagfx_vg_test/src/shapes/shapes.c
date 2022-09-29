//*****************************************************************************
//
//! @file shapes.c
//!
//! @brief Demo to draw the predefined shapes.
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

#ifdef RUN_SHAPE

#include "ref.rgba.h"

//*****************************************************************************
//
// Macro definitions
//
//*****************************************************************************
#define RESX 452
#define RESY 452

#define COLOR_STOPS       5
#define REF_IMG_W         80
#define REF_IMG_H         100

//*****************************************************************************
//
// Global Variables
//
//*****************************************************************************
//! framebuffrer
nema_img_obj_t g_sFrameBuffer =
{
    {0}, RESX, RESY, RESX*4, 0, NEMA_RGBA8888, 0
};

//! texture
nema_img_obj_t g_sRefImage =
{
    {0}, REF_IMG_W, REF_IMG_H, REF_IMG_W*4, 0, NEMA_RGBA8888, NEMA_FILTER_BL
};

//! DC layer
nemadc_layer_t g_sDCLayer =
{
    (void *)0, 0, RESX, RESY, -1, 0, 0, RESX, RESY, 0xff,
    NEMADC_BL_SRC, 0, NEMADC_RGBA8888, 0, 0, 0, 0, 0, 0, 0
};

//*****************************************************************************
//
//! @brief Create buffer, load image from MRAM to ssram
//!
//
//*****************************************************************************
static void bufferCreate(void)
{
    //Load memory objects
    g_sFrameBuffer.bo = nema_buffer_create( g_sFrameBuffer.stride*g_sFrameBuffer.h);
    nema_buffer_map(&g_sFrameBuffer.bo);
    printf("FB: V:%p P:0x%08x\n", (void *)g_sFrameBuffer.bo.base_virt, g_sFrameBuffer.bo.base_phys);

    g_sRefImage.bo = nema_buffer_create(ref_rgba_len);
    memcpy(g_sRefImage.bo.base_virt, ref_rgba, ref_rgba_len);

    //set dc layer memory
    g_sDCLayer.baseaddr_phys = g_sFrameBuffer.bo.base_phys;
    g_sDCLayer.baseaddr_virt = g_sFrameBuffer.bo.base_virt;

    printf("FB: V:%p P:0x%08x\n", (void *)g_sFrameBuffer.bo.base_virt, g_sFrameBuffer.bo.base_phys);
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

    nema_buffer_unmap(&g_sFrameBuffer.bo);
    nema_buffer_destroy(&g_sFrameBuffer.bo);
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
//! @brief run the shapes demo
//!
//
//*****************************************************************************
int shapes()
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

    bufferCreate();

    nemadc_set_layer(0, &g_sDCLayer);

#if 0
    // Use circular cl
    nema_cmdlist_t cl  = nema_cl_create_sized(1024);
    nema_cl_bind_circular(&cl);
#else
    nema_cmdlist_t cl  = nema_cl_create();
    nema_cl_bind(&cl);
#endif

    //clear frame buffer
    nema_bind_dst_tex(g_sFrameBuffer.bo.base_phys, g_sFrameBuffer.w, g_sFrameBuffer.h, g_sFrameBuffer.format, g_sFrameBuffer.stride);
    nema_set_clip(0, 0, RESX, RESY);
    nema_clear(nema_rgba(0xff, 0xff, 0x00, 0xff)); //yellow

    //Create and set paint
    NEMA_VG_PAINT_HANDLE paint =  nema_vg_paint_create();
    nema_vg_paint_set_stroke_width(paint, 15.0f);
    nema_vg_set_fill_rule(NEMA_VG_FILL_EVEN_ODD);
    nema_vg_set_blend(NEMA_BL_SRC_OVER);

    //Create and set gradient
    NEMA_VG_GRAD_HANDLE gradient = nema_vg_grad_create();
    float stops[COLOR_STOPS] = {0.0f, 0.25f, 0.50f, 0.75f, 1.0f};
    color_var_t colors[COLOR_STOPS] = {
                                        {0  , 0  , 255  , 255}, //blue
                                        {0, 255  , 0  , 255}, //green
                                        {255  , 255, 255  , 255}, //white
                                        {255  , 165, 0, 255}, //orange
                                        {255, 0, 0, 255} //red
                                      };
    nema_vg_grad_set(gradient, COLOR_STOPS, stops, colors);

    //--------------FILL---------------------------
    float y_start = 55.0f;
    nema_vg_paint_set_type(paint, NEMA_VG_PAINT_FILL);

    nema_vg_paint_set_paint_color(paint, nema_rgba(0xff, 0x00, 0xff, 0x80));

    nema_vg_draw_circle(60.0f, y_start, 50.0f, NULL, paint);

    nema_vg_draw_ellipse(220.0f, y_start, 70.0f, 30.0f, NULL, paint);

    nema_vg_draw_ring(380.0f, y_start, 40.0f, 0.0f, 330.0f, paint);

    nema_vg_draw_line(450.0f, y_start - 40.0f, 550.0f, y_start + 40.0f, NULL, paint);

    nema_vg_draw_rounded_rect(580.0f, y_start - 40.0f, 80.0f, 80.0f, 20.0f, 30.0f, NULL, paint);

    nema_vg_draw_rect(700.0f, y_start - 40.0f, 80.0f, 80.0f, NULL, paint);
    //--------------LINEAR GRADIENT--------------------------

    y_start += 120.0f;

    nema_vg_paint_set_type(paint, NEMA_VG_PAINT_GRAD_LINEAR);
    nema_vg_paint_set_grad_linear(paint, gradient, 100.0f, y_start - 35.0f, 100.0f, y_start + 35.0f, NEMA_TEX_CLAMP | NEMA_FILTER_BL);

    nema_vg_draw_circle(60.0f, y_start, 50.0f, NULL, paint);

    nema_vg_draw_ellipse(220.0f, y_start, 70.0f, 30.0f, NULL, paint);

    nema_vg_draw_ring(380.0f, y_start, 40.0f, 0.0f, 330.0f, paint);

    nema_vg_draw_line(450.0f, y_start - 40.0f, 550.0f, y_start + 40.0f, NULL, paint);

    nema_vg_draw_rounded_rect(580.0f, y_start - 40.0f, 80.0f, 80.0f, 20.0f, 30.0f, NULL, paint);

    nema_vg_draw_rect(700.0f, y_start - 40.0f, 80.0f, 80.0f, NULL, paint);

    //--------------CONIC GRADIENT--------------------------

    y_start += 120.0f;

    nema_vg_paint_set_type(paint, NEMA_VG_PAINT_GRAD_CONICAL);

    nema_vg_paint_set_grad_conical(paint, gradient, 60.0f, y_start, NEMA_TEX_CLAMP  | NEMA_FILTER_BL);
    nema_vg_draw_circle(60.0f, y_start, 50.0f, NULL, paint);

    nema_vg_paint_set_grad_conical(paint, gradient, 220.0f, y_start, NEMA_TEX_CLAMP  | NEMA_FILTER_BL);
    nema_vg_draw_ellipse(220.0f, y_start, 70.0f, 30.0f, NULL, paint);

    nema_vg_paint_set_grad_conical(paint, gradient, 380.0f, y_start, NEMA_TEX_CLAMP  | NEMA_FILTER_BL);
    nema_vg_draw_ring(380.0f, y_start, 40.0f, 0.0f, 330.0f, paint);

    nema_vg_paint_set_grad_conical(paint, gradient, 500.0f, y_start, NEMA_TEX_CLAMP  | NEMA_FILTER_BL);
    nema_vg_draw_line(450.0f, y_start - 40.0f, 550.0f, y_start + 40.0f, NULL, paint);

    nema_vg_paint_set_grad_conical(paint, gradient, 620.0f, y_start, NEMA_TEX_CLAMP  | NEMA_FILTER_BL);
    nema_vg_draw_rounded_rect(580.0f, y_start - 40.0f, 80.0f, 80.0f, 20.0f, 30.0f, NULL, paint);

    nema_vg_paint_set_grad_conical(paint, gradient, 740.0f, y_start, NEMA_TEX_CLAMP  | NEMA_FILTER_BL);
    nema_vg_draw_rect(700.0f, y_start - 40.0f, 80.0f, 80.0f, NULL, paint);

    //--------------RADIAL GRADIENT--------------------------
    //Not supported now!!!!!!!!!!!
    // y_start += 120.0f;

    // nema_vg_paint_set_type(paint, NEMA_VG_PAINT_GRAD_RADIAL);

    // nema_vg_paint_set_grad_radial(paint, gradient, 60.0f, y_start, 50, NEMA_TEX_CLAMP | NEMA_FILTER_BL);
    // nema_vg_draw_circle(60.0f, y_start, 50.0f, NULL, paint);

    // nema_vg_paint_set_grad_radial(paint, gradient, 220.0f, y_start, 50, NEMA_TEX_CLAMP | NEMA_FILTER_BL);
    // nema_vg_draw_ellipse(220.0f, y_start, 70.0f, 30.0f, NULL, paint);

    // nema_vg_paint_set_grad_radial(paint, gradient, 380.0f, y_start, 50, NEMA_TEX_CLAMP | NEMA_FILTER_BL);
    // nema_vg_draw_ring(380.0f, y_start, 40.0f, 0.0f, 330.0f, paint);

    // nema_vg_paint_set_grad_radial(paint, gradient, 500.0f, y_start, 50, NEMA_TEX_CLAMP | NEMA_FILTER_BL);
    // nema_vg_draw_line(450.0f, y_start - 40.0f, 550.0f, y_start + 40.0f, NULL, paint);

    // nema_vg_paint_set_grad_radial(paint, gradient, 620.0f, y_start, 50, NEMA_TEX_CLAMP | NEMA_FILTER_BL);
    // nema_vg_draw_rounded_rect(580.0f, y_start - 40.0f, 80.0f, 80.0f, 20.0f, 30.0f, NULL, paint);

    // nema_vg_paint_set_grad_radial(paint, gradient, 740.0f, y_start, 50, NEMA_TEX_CLAMP | NEMA_FILTER_BL);
    // nema_vg_draw_rect(700.0f, y_start - 40.0f, 80.0f, 80.0f, NULL, paint);

    //--------------TEXTURE--------------------------
    nema_matrix3x3_t m_paint;
    y_start += 120.0f;

    nema_vg_paint_set_type(paint, NEMA_VG_PAINT_TEXTURE);
    nema_vg_paint_set_tex(paint, &g_sRefImage);

    nema_mat3x3_load_identity(m_paint);
    matrixCalculate(m_paint, 0.0f, REF_IMG_W / 2.f, REF_IMG_H / 2.f);
    nema_mat3x3_translate(m_paint, 20.0f, y_start - 50.0f);
    nema_vg_paint_set_tex_matrix(paint, m_paint);
    nema_vg_draw_circle(60.0f, y_start, 50.0f, NULL, paint);

    nema_mat3x3_load_identity(m_paint);
    matrixCalculate(m_paint, 90.0f, REF_IMG_W / 2.f, REF_IMG_H / 2.f);
    nema_mat3x3_translate(m_paint, 180.0f, y_start - 50.0f);
    nema_vg_paint_set_tex_matrix(paint, m_paint);
    nema_vg_draw_ellipse(220.0f, y_start, 70.0f, 30.0f, NULL, paint);

    nema_mat3x3_load_identity(m_paint);
    matrixCalculate(m_paint, 180.0f, REF_IMG_W / 2.f, REF_IMG_H / 2.f);
    nema_mat3x3_translate(m_paint, 330.0f, y_start - 50.0f);
    nema_vg_paint_set_tex_matrix(paint, m_paint);
    nema_vg_draw_ring(380.0f, y_start, 40.0f, 10.0f, 330.0f, paint);

    nema_mat3x3_load_identity(m_paint);
    matrixCalculate(m_paint, 180.0f, REF_IMG_W / 2.f, REF_IMG_H / 2.f);
    nema_mat3x3_translate(m_paint, 440.0f, y_start - 50.0f);
    nema_vg_paint_set_tex_matrix(paint, m_paint);
    nema_vg_draw_line(450.0f, y_start - 40.0f, 550.0f, y_start + 40.0f, NULL, paint);

    nema_mat3x3_load_identity(m_paint);
    matrixCalculate(m_paint, 180.0f, REF_IMG_W / 2.f, REF_IMG_H / 2.f);
    nema_mat3x3_translate(m_paint, 580.0f, y_start - 50.0f);
    nema_vg_paint_set_tex_matrix(paint, m_paint);
    nema_vg_draw_rounded_rect(580.0f, y_start - 40.0f, 80.0f, 80.0f, 20.0f, 30.0f, NULL, paint);

    nema_mat3x3_load_identity(m_paint);
    matrixCalculate(m_paint, 270.0f, REF_IMG_W / 2.f, REF_IMG_H / 2.f);
    nema_mat3x3_translate(m_paint, 700.0f, y_start - 50.0f);
    nema_vg_paint_set_tex_matrix(paint, m_paint);
    nema_vg_draw_rect(700.0f, y_start - 40.0f, 80.0f, 80.0f, NULL, paint);

    nema_cl_submit(&cl);
    nema_cl_wait(&cl);

    //Start DC
    display_refresh_start();
    //Wait DC complete interrupt.
    nemadc_wait_vsync();
    //Do follow-up operations required by hardware.
    display_refresh_end();

    nema_vg_grad_destroy(gradient);
    nema_vg_paint_destroy(paint);
    nema_vg_deinit();
    bufferDestroy();

    return 0;
}

#endif
