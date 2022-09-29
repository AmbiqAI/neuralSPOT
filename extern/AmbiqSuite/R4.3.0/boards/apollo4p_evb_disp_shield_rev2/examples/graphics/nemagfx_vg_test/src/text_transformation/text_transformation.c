//*****************************************************************************
//
//! @file text_transformation.c
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

#ifdef RUN_TEXT_TRANSFORMATION

#include "nema_core.h"
#include "nema_dc.h"
#include "nema_vg.h"
#include "nema_vg_font.h"
#include "DejaVuSans_ttf_kern.h"
#include "nema_font.h"
#include "nema_event.h"
#include "nema_utils.h"

#define VECTOR_FONT DejaVuSans_ttf_kern

#define FRAME_BUFFERS 1

#define RESX 452
#define RESY 452

static img_obj_t fb[FRAME_BUFFERS];
static nemadc_layer_t layer[FRAME_BUFFERS] = {{0}};

static char str[]  = "Sample text 0\nSample text 1";

void load_objects(void)
{
    int i;
    for (i = 0; i < FRAME_BUFFERS; ++i)
    {
        fb[i].w      = RESX;
        fb[i].h      = RESY;
        fb[i].format = NEMA_RGBA8888;
        fb[i].stride = RESX*4;
        fb[i].bo     = nema_buffer_create(fb[i].stride*fb[i].h);
        nema_buffer_map(&fb[i].bo);

#ifndef DONT_USE_NEMADC
        layer[i].sizex         = layer[i].resx = fb[i].w;
        layer[i].sizey         = layer[i].resy = fb[i].h;
        layer[i].stride        = fb[i].stride;
        layer[i].format        = NEMADC_RGBA8888;
        layer[i].blendmode     = NEMADC_BF_ONE;
        layer[i].baseaddr_phys = fb[i].bo.base_phys;
        layer[i].baseaddr_virt = fb[i].bo.base_virt;
        layer[i].flipx_en      = 0;
        layer[i].flipy_en      = 0;
#endif

    }

    nema_buffer_t font_data = nema_buffer_create(DejaVuSans_ttf_kern.data_length*4);
    memcpy(font_data.base_virt, DejaVuSans_ttf_kern.data, DejaVuSans_ttf_kern.data_length*4);
    DejaVuSans_ttf_kern.data = font_data.base_virt;
}

int cur_fb = 0;
uintptr_t cur_fb_base_phys = 0;

// void
// swap_buffers(void) {

// #ifndef DONT_USE_NEMADC
//     nemadc_wait_vsync();
//     //Do follow-up operations required by hardware.
//     display_refresh_end();
//     nemadc_set_layer(0, &layer[cur_fb]);
// #endif

//     cur_fb = (cur_fb+1)%FRAME_BUFFERS;
//     cur_fb_base_phys = fb[cur_fb].bo.base_phys;
// }

int text_transformation()
{
    //Initialize NemaGFX
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

    load_objects();

#ifndef DONT_USE_NEMADC
    //Format        | Pixclock | RESX | FP | SYNC | BP | RESY | FP | SYNC | BP
    //800x600, 60Hz | 40.000   | 800  | 40 | 128  | 88 | 600  | 1  | 4    | 23
    //nemadc_timing(800, 40, 128, 88, 600, 1, 4, 23);
    nemadc_set_layer(0, &layer[0]);
#endif

    //initialize nema_vg
    //Put VG init operation in front of any rendering operations, or it will
    //cause odd issue
    nema_vg_init(RESX, RESY);

    cur_fb_base_phys = fb[cur_fb].bo.base_phys;

    //Create Command List
    nema_cmdlist_t cl  = nema_cl_create_sized(10*1024);
    //Bind Command List
    nema_cl_bind_circular(&cl);

    //Bind Framebuffer
    nema_bind_dst_tex(cur_fb_base_phys, fb[0].w, fb[0].h, fb[0].format, fb[0].stride);
    //Set Clipping Rectangle
    nema_set_clip(0, 0, RESX, RESY);

    //Set Blending Mode
    nema_set_blend_fill(NEMA_BL_SRC);
    //Fill Rectangle with Black (Clear)
    nema_fill_rect(0, 0, RESX, RESY, 0);

    nema_cl_submit(&cl);
    nema_cl_wait(&cl);

    nema_cl_rewind(&cl);

    // //initialize nema_vg
    // nema_vg_init(RESX, RESY);

    NEMA_VG_PAINT_HANDLE paint =  nema_vg_paint_create();
    nema_vg_paint_set_type(paint, NEMA_VG_PAINT_FILL);

    float font_size = 32.f;
    uint32_t rgba8888 = nema_rgba(255, 255, 0, 255);
    nema_vg_paint_set_paint_color(paint, rgba8888);

    nema_vg_bind_font(&VECTOR_FONT);
    nema_vg_set_font_size(font_size);

    int w, h;
    nema_vg_string_get_bbox(str, &w, &h, RESX, NEMA_ALIGNX_LEFT);

    int x_off = (RESX - w) / 2;
    int y_off = (RESY - h) / 2;
    //transformation origin point is the center location in the string's bound box
    float x_transform = x_off + 0.5f*w;
    float y_transform = y_off + 0.5f*h;

    float time = 0;

    while(1)
    {
        time += 0.05;
        float sec  = time;
        float min  = time / 60;
        float hour = time / 60 / 60;

        //calculate text angle based on current time
        float angle = sec*5.f / 60.f * 360.f;

        nema_cl_rewind(&cl);

        nema_bind_dst_tex(cur_fb_base_phys, fb[0].w, fb[0].h, fb[0].format, fb[0].stride);
        nema_clear(nema_rgba(0, 0, 0, 0xff));

        //compute the transformation matrix that
        //rotates text around transformation origin point (x_transform, y_transform)
        nema_matrix3x3_t mat;
        nema_mat3x3_load_identity(mat);
        nema_mat3x3_translate(mat, -x_transform, -y_transform);
        nema_mat3x3_rotate(mat, angle);
        nema_mat3x3_translate(mat, x_transform, y_transform);
        nema_vg_print(paint, str, x_off, y_off, RESX, RESY, NEMA_ALIGNX_LEFT, mat);

        //draw transformation origin point
        nema_set_blend_fill(NEMA_BL_SRC);
        nema_fill_rect(x_transform, y_transform, 2, 2, 0xff0000ff);
        nema_cl_submit(&cl);
        nema_cl_wait(&cl);

        //Start DC
        display_refresh_start();
        //Wait DC complete interrupt.
        nemadc_wait_vsync();
        //Do follow-up operations required by hardware.
        display_refresh_end();

        //swap_buffers();
        // nema_calculate_fps();
    }

    nema_cl_destroy(&cl);

    nema_vg_paint_destroy(paint);
    nema_vg_deinit();

    return 0;
}

#endif

