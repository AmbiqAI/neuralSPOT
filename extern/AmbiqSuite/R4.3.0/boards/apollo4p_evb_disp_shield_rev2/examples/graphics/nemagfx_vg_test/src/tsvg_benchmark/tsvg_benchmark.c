//*****************************************************************************
//
//! @file tsvg_benchmark.c
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

#ifdef RUN_TSVG_BENCHMARK

// Uncomment this if NemaDC is not available
// #define DONT_USE_NEMADC

// Uncomment this if SVG needs to be loaded from the
// filesystem (ignored if STANDALONE is not defined)
// #define LOAD_SVG_FROM_FILESYSTEM

// Uncomment this if main needs to be defined here
// If not STANDALONE, LOAD_SVG_FROM_FILESYSTEM is ingored
// #define STANDALONE

#ifndef DEFAULT_FILENAME
#define DEFAULT_FILENAME  "tiger.tsvg.h"
#endif

// #include <stdio.h>
// #include <sys/time.h>

#include "nema_core.h"
#include "nema_vg.h"
#include "nema_utils.h"
#include "nema_vg_tsvg.h"

#ifndef DONT_USE_NEMADC
#include "nema_dc.h"
#endif

#ifndef LOAD_SVG_FROM_FILESYSTEM
#include DEFAULT_FILENAME
unsigned char *tsvg_bin = tiger_tsvg;
#else
char *tsvg_bin;
#endif


// number of frames to be drawn
#ifndef FRAMES_TO_DRAW
#define FRAMES_TO_DRAW 50
#endif

// Color to be used in order to clear the framebuffer
#define CLEAR_COLOR 0x80808080U

#ifndef RESX
#define RESX 452
#endif

#ifndef RESY
#define RESY 452
#endif

// Returns time in seconds
// static float get_time(void)
// {
//     struct timeval tv;
//     float sec = -1;
//     if (gettimeofday(&tv, NULL) == 0) {
//         sec = ((tv.tv_sec % 86400) + tv.tv_usec / 1000000.f);
//     }

//     return sec;
// }

static void *svg_bin = NULL;
static nema_cmdlist_t cl_svg;
static float scale =  1.f;
static float scalex = 1.f;
static float scaley = 1.f;
static float scale2 = 1.f;

#define COLOR_STOPS       4

static float stops[COLOR_STOPS] = {0.0f, 0.5f, 0.65f, 0.75f};
#if 0
static color_var_t colors[COLOR_STOPS] =
                                {
                                    {0  , 0  , 255  , 255}, //blue
                                    {0, 255  , 0  , 255}, //green
                                    {255  , 255, 255  , 255}, //white
                                    {255, 0, 0, 255},
                                    {0,   0, 255, 255} //orange
                                }; //red
#else
static color_var_t colors[COLOR_STOPS] =
                {
                    { 0xff, 0xff, 0xff, 0xff },
                    { 0x80, 0x80, 0x80, 0xff },
                    { 0x20, 0x20, 0x20, 0xff },
                    { 0x00, 0x00, 0x00, 0xff }
                }; //red
#endif

static float svg_w, svg_h;

// Initializes NemaDC
static int init_dc(nema_buffer_t *fb_bo, int img_w, int img_h)
{

#ifndef DONT_USE_NEMADC
    // Initialize Nema|dc
    // -----------------------------------------------------
    int ret = display_setup(RESX, RESY);
    if (ret)
    {
        return ret;
    }
    //Format        | Pixclock | RESX | FP | SYNC | BP | RESY | FP | SYNC | BP
    //800x600, 60Hz | 40.000   | 800  | 40 | 128  | 88 | 600  | 1  | 4    | 23
    //nemadc_timing(800, 40, 128, 88, 600, 1, 4, 23);
    // -----------------------------------------------------

    nemadc_layer_t layer = {0};
    layer.format    = NEMADC_RGBA8888;
    layer.sizex     = layer.resx = img_w;
    layer.sizey     = layer.resy = img_h;
    layer.stride    = -1;
    layer.blendmode = NEMADC_BL_SRC;
    layer.baseaddr_phys = fb_bo->base_phys;
    layer.baseaddr_virt = fb_bo->base_virt;
    layer.flipx_en      = 0;
    layer.flipy_en      = 0;
    nemadc_set_layer(0, &layer);
#endif

    return 0;
}

void
nema_gfx_display_function()
{
    // scale2 = 0.1f;
    static float rot = 0.f;
    // rot = 0.f;
    nema_matrix3x3_t matrix;
    nema_mat3x3_load_identity(matrix);
    nema_mat3x3_translate(matrix, -svg_w * 0.5f, -svg_h * 0.5f);
    nema_mat3x3_scale(matrix, scale * scale2, scale * scale2);
    nema_mat3x3_rotate(matrix, (float)rot);
    nema_mat3x3_translate(matrix, RESX / 2, RESY / 2);

    nema_vg_set_global_matrix(matrix);

    rot += 10.f;
    if (rot >= 360.f)
    {
        rot = 0.f;
    }

#ifdef DO_SCALE
    scale2 *= 1.3f;
    if (scale2 > 100.f)
    {
        scale2 = 0.1f;
    }
#endif

    // Draw the SVG
    nema_cl_bind_circular(&cl_svg);
    nema_cl_rewind(&cl_svg);

    nema_vg_draw_tsvg(tsvg_bin);

    nema_cl_submit(&cl_svg);
    nema_cl_wait(&cl_svg);
}

#ifdef STANDALONE
int main (int argc, char *argv[])
#else
int tsvg_benchmark (void)
#endif
{
    // Initialize NemaGFX
    int ret = nema_init();
    if (ret)
    {
        return ret;
    }

    // -----------------------------------------------------
    uint32_t svg_width, svg_height;

#ifdef LOAD_SVG_FROM_FILESYSTEM
    nema_buffer_t tsvg_bo = nema_load_file(argv[1], -1, NULL);
    tsvg_bin = (char *)tsvg_bo.base_virt;
#endif

    nema_vg_get_tsvg_resolution(tsvg_bin, &svg_width, &svg_height);

    if ( svg_width != 0U && svg_height != 0U )
    {
        svg_w = (float)svg_width;
        svg_h = (float)svg_height;
    }
    else
    {
        svg_w = 498; //(float)svg_width;
        svg_h = 600; //(float)svg_height;
    }

    scalex = RESX / svg_w;
    scaley = RESY / svg_h;
    scale  = 0.707f * nema_max2(scalex, scaley);

    printf("rasterizing svg %f x %f\n", svg_w, svg_h);
    printf("scale: %f\n", scale);
    // -----------------------------------------------------

    // Allocate framebuffer
    // -----------------------------------------------------
    nema_buffer_t fb_bo = nema_buffer_create( RESX*RESY*4);
    if (fb_bo.base_virt == NULL)
    {
        printf("Could not alloc image buffer.\n");
        goto error_fb;
    }
    // -----------------------------------------------------

    // Initialize NemaDC
    // -----------------------------------------------------
    ret = init_dc(&fb_bo, RESX, RESY);
    if (ret != 0)
    {
        return ret;
    }
    // -----------------------------------------------------

    // Create cl_clear Command List that will clear the
    // framebuffer on each frame
    // -----------------------------------------------------
    nema_cmdlist_t cl_clear  = nema_cl_create();

    nema_cl_bind(&cl_clear);
    nema_cl_rewind(&cl_clear);

    nema_set_clip(0, 0, RESX, RESY);
    // Bind Framebuffer
    nema_bind_dst_tex(fb_bo.base_phys, RESX, RESY, NEMA_RGBA8888, -1);

    nema_vg_init(RESX, RESY);
    // nema_clear(0xffffffff);

    NEMA_VG_PAINT_HANDLE paint = nema_vg_paint_create();
    NEMA_VG_GRAD_HANDLE gradient = nema_vg_grad_create();
    nema_vg_grad_set(gradient, COLOR_STOPS, stops, colors);
    // nema_vg_paint_set_type(paint, NEMA_VG_PAINT_GRAD_CONICAL);
    // nema_vg_paint_set_grad_conical(paint, RESX / 2, RESY / 2, NEMA_TEX_CLAMP  | NEMA_FILTER_BL);
    nema_vg_paint_set_type(paint, NEMA_VG_PAINT_GRAD_RADIAL);
    nema_vg_paint_set_grad_radial(paint, gradient, RESX / 2, RESY / 2, nema_max2(RESX, RESY), NEMA_TEX_CLAMP | NEMA_FILTER_BL);
    nema_vg_draw_rect(0, 0, RESX, RESY, NULL, paint);

    nema_cl_submit(&cl_clear);
    nema_cl_wait(&cl_clear);

    // Draw the SVG
    // -----------------------------------------------------
    cl_svg    = nema_cl_create_sized(8*1024);
    // float sec0 = get_time();

#ifdef RUN_FOREVER
    while(1)
#else
    for (int i = 0; i < FRAMES_TO_DRAW; ++i)
#endif
    {
        // Clear the framebuffer
        // nema_cl_submit(&cl_clear);
        nema_gfx_display_function();
        //nema_calculate_fps();

        //Start DC
        display_refresh_start();
        //Wait DC complete interrupt.
        nemadc_wait_vsync();
        //Do follow-up operations required by hardware.
        display_refresh_end();
    }

    // -----------------------------------------------------

    // Calculate performance
    // -----------------------------------------------------
    // float sec1 = get_time();
    // am_util_stdio_printf("%d frames in %f sec\n", FRAMES_TO_DRAW, (sec1-sec0));
    // am_util_stdio_printf("fps: %f\n", FRAMES_TO_DRAW/(sec1-sec0));
    // -----------------------------------------------------

    // Destroy allocated assets
    // -----------------------------------------------------
    nema_cl_destroy(&cl_svg);
    nema_cl_destroy(&cl_clear);
    nema_vg_grad_destroy(gradient);
    nema_vg_deinit();
    nema_buffer_destroy(&fb_bo);
error_fb:
error_svg_bin:
    // -----------------------------------------------------

    return 0;
}

#endif
