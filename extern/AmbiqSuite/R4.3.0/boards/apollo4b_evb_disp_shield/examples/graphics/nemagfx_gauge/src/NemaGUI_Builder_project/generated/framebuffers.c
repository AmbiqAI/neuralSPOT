//*****************************************************************************
//
//! @file framebuffers.c
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

#define RESX        456
#define RESY        456

#ifndef FRAME_BUFFERS
#define FRAME_BUFFERS 2
#endif

#ifndef BACK_BUFFERS
#define BACK_BUFFERS 2
#endif

#ifndef FRAME_BUFFER_FORMAT
#define FRAME_BUFFER_FORMAT NEMA_TSC4
#endif

#ifndef BACK_BUFFER_FORMAT
#define BACK_BUFFER_FORMAT NEMA_TSC4
#endif

#if FRAME_BUFFERS < 1
#error 0 && "FRAME_BUFFERS need to be at least 1"
#endif

#include "framebuffers.h"
#include "nema_dc.h"
#include "nema_core.h"
#include "nema_utils.h"
#include "ng_globals.h"

static img_obj_t fb[FRAME_BUFFERS];
static nemadc_layer_t layer[FRAME_BUFFERS];

static img_obj_t back_buffer[BACK_BUFFERS];

static inline int
format_to_dc(int format)
{
    switch(format)
    {
        case NEMA_RGBA5551: return NEMADC_RGBA5551;
        case NEMA_ABGR8888: return NEMADC_ABGR8888;
        case NEMA_RGB332  : return NEMADC_RGB332;
        case NEMA_RGB565  : return NEMADC_RGB565;
        case NEMA_BGRA8888: return NEMADC_BGRA8888;
        case NEMA_L8      : return NEMADC_L8;
        case NEMA_ARGB8888: return NEMADC_ARGB8888;
        case NEMA_TSC4    : return NEMADC_TSC4;
        case NEMA_TSC6    : return NEMADC_TSC6;
        default:
        case NEMA_RGBA8888: return NEMADC_RGBA8888;
    }
}

void load_framebuffers(void)
{
    // load frame buffers
    for (int id = 0; id < FRAME_BUFFERS; ++id)
    {
        fb[id].stride = nema_stride_size(FRAME_BUFFER_FORMAT, 0, RESX);
        fb[id].w = RESX;
        fb[id].h = RESY;
        fb[id].format = FRAME_BUFFER_FORMAT;

        if (FRAME_BUFFER_FORMAT == NEMA_TSC4)
        {
            fb[id].bo = nema_buffer_create(RESX * RESY / 2);
        }
        else
        {
            fb[id].bo = nema_buffer_create(fb[id].stride * fb[id].h);
        }

        nema_buffer_map(&fb[id].bo);

        layer[id].sizex         = layer[id].resx = fb[id].w;
        layer[id].sizey         = layer[id].resy = fb[id].h;
        layer[id].stride        = -1;
        layer[id].format        = format_to_dc(FRAME_BUFFER_FORMAT);
        layer[id].blendmode     = NEMADC_BL_SRC;
        layer[id].baseaddr_phys = fb[id].bo.base_phys;
        layer[id].baseaddr_virt = fb[id].bo.base_virt;

    }

    // load back buffers
    for (int id = 0; id < BACK_BUFFERS; ++id)
    {
        back_buffer[id].stride = nema_stride_size(BACK_BUFFER_FORMAT, 0, RESX);
        back_buffer[id].w      = RESX;
        back_buffer[id].h      = RESY;
        back_buffer[id].format = BACK_BUFFER_FORMAT;

        if ( BACK_BUFFER_FORMAT == NEMA_TSC4 )
        {
            back_buffer[id].bo = nema_buffer_create(RESX * RESY / 2);
        }
        else if ( BACK_BUFFER_FORMAT == NEMA_TSC6 )
        {
            back_buffer[id].bo = nema_buffer_create(6 * RESX * RESY / 8);
        }
        else
        {
            back_buffer[id].bo = nema_buffer_create(back_buffer[id].stride * back_buffer[id].h);
        }

        nema_buffer_map(&back_buffer[id].bo);
    }

    ng_globals_register_framebuffers(FRAME_BUFFERS, fb, BACK_BUFFERS, back_buffer, layer);
    ng_globals_set_resolution(RESX, RESY);
}
