//*****************************************************************************
//
//! @file watch.c
//!
//! @brief NemaGFX example.
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
#include "nema_core.h"
#include "nema_utils.h"

#include "watch.h"

#include "0.l8.h"
#include "L0_360x360_l8.h"
#include "1.rgba.h"
#include "2.rgba.h"
#include "3.rgba.h"

static img_obj_t backface   = {{0}, 390, 390, -1, 0, NEMA_L8, NEMA_FILTER_BL};

static const int hour_pivot_x = -11;
static const int hour_pivot_y = -140;
static img_obj_t hour_h     = {{0},  22, 104, -1, 0, NEMA_RGBA8888, NEMA_FILTER_BL};

static const int min_pivot_x = -14;
static const int min_pivot_y = -139;
static img_obj_t min_h      = {{0},  28, 153, -1, 0, NEMA_RGBA8888, NEMA_FILTER_BL};

static const int ssec_pivot_x = -9;
static const int ssec_pivot_y = -148;
static img_obj_t ssec_h     = {{0},  18, 219, -1, 0, NEMA_RGBA8888, NEMA_FILTER_BL};

static nema_buffer_t create_bo_from_pointer(void *ptr, size_t size)
{
    nema_buffer_t bo;

#ifdef UNIFIED_MEMORY
    bo.base_virt = ptr;
    bo.base_phys = (uintptr_t)ptr;
    bo.size      = size;
#else
    bo = nema_buffer_create(size);
    (void)nema_buffer_map(&bo);
    nema_memcpy(bo.base_virt, ptr, size);
#endif

    return bo;
}

static void
load_images(void)
{
    //Load memory objects
    backface.bo = create_bo_from_pointer(__0_l8  , __0_l8_len);
      hour_h.bo = create_bo_from_pointer((void*)__1_rgba, __1_rgba_len);
       min_h.bo = create_bo_from_pointer(__2_rgba, __2_rgba_len);
      ssec_h.bo = create_bo_from_pointer(__3_rgba, __3_rgba_len);
}

static nema_cmdlist_t wf_cl;

static int draw_bf = 0;
static int tex_bf  = 0;
static uint32_t bf_color  = 0;

void
init_watch(int draw_backface, int backface_is_tex, uint32_t backface_color)
{
    load_images();

    draw_bf  = draw_backface;
    tex_bf   = backface_is_tex;
    bf_color = backface_color;

    wf_cl             = nema_cl_create_sized(0x1000);
}

static inline void
draw_needle( img_obj_t *img, float x0, float y0, float angle, int cx, int cy )
{
    float x1 = x0 + img->w,  y1 = y0;
    float x2 = x0 + img->w,  y2 = y0 + img->h;
    float x3 = x0       ,  y3 = y0 + img->h;

    //calculate rotation matrix
    nema_matrix3x3_t m;
    nema_mat3x3_load_identity(m);
    nema_mat3x3_rotate(m, -angle);
    nema_mat3x3_translate(m, cx, cy);

    //rotate points
    nema_mat3x3_mul_vec(m, &x0, &y0);
    nema_mat3x3_mul_vec(m, &x1, &y1);
    nema_mat3x3_mul_vec(m, &x2, &y2);
    nema_mat3x3_mul_vec(m, &x3, &y3);

    //draw needle
    nema_bind_src_tex( img->bo.base_phys, img->w, img->h, img->format, img->stride, NEMA_FILTER_BL);
    nema_blit_quad_fit(x0, y0,
                       x1, y1,
                       x2, y2,
                       x3, y3);
}

nema_cmdlist_t *
draw_watchface(float hour, float min, float sec)
{
    float angle;

    nema_cl_rewind(&wf_cl);
    nema_cl_bind(&wf_cl);

    nema_set_clip(0, 0, RESX, RESY);
    // nema_bind_dst_tex(cur_fb_base_phys, RESX, RESY, NEMA_RGBA8888, -1);

    //draw backface
    if ( draw_bf != 0 )
    {
        if ( tex_bf != 0 )
        {
            nema_bind_src_tex( backface.bo.base_phys, backface.w, backface.h, backface.format, backface.stride, 0 );
            nema_set_blend_blit(NEMA_BLOP_MODULATE_RGB | NEMA_BL_SRC);

            nema_set_const_color(0x80ffa0a0);

            nema_blit(0, 0);
        }
        else
        {
            nema_set_blend_fill(NEMA_BL_SRC);
            nema_fill_rect(0, 0, RESX, RESY, bf_color);
        }
    }

    nema_set_blend_blit(NEMA_BL_SIMPLE);

    //draw hour hand
    angle =  hour / 12.f * 360.f;
    draw_needle( &hour_h  , hour_pivot_x, hour_pivot_y, -angle, RESX / 2, RESY / 2);

    //draw minute hand
    angle =  min / 60.f * 360.f;
    draw_needle( &min_h   , min_pivot_x , min_pivot_y , -angle, RESX / 2, RESY / 2);

    //draw second hand
    angle = (float)sec / 60.f * 360.f;
    draw_needle( &ssec_h  , ssec_pivot_x, ssec_pivot_y, -angle, RESX / 2, RESY / 2);

    nema_cl_return();

    return &wf_cl;
}
