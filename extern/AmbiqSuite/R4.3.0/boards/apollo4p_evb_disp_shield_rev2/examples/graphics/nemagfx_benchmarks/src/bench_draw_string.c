//*****************************************************************************
//
//! @file bench_draw_string.c
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

#include <nema_core.h>
#include <nema_font.h>

//#define NEMA_FONT_IMPLEMENTATION
#include "DejaVuSerif12pt8b.h"
//#undef NEMA_FONT_IMPLEMENTATION

#include "bench.h"

static char str[] = "Think Silicon\nUltra-low power | vivid graphics";
static int w, h;

static int
_memcpy(uint8_t* dst, const uint8_t* src, uint32_t nbytes)
{
    uint32_t i;
    int count = 0;
    for (i = 0; i < nbytes; i++)
    {
        dst[i] = src[i];
        count++;
    }

    return count;
}

static int
i32RenderFrame()
{
    int x = 0;
    int y = 0;
    uint32_t col = nema_rand();

    if (FB_RESX > w)
    {
       x = nema_rand() % (FB_RESX - w);
    }

    if (FB_RESY > h)
    {
        y = nema_rand() % (FB_RESY - h);
    }

    nema_print(str, x, y, w, h, col, NEMA_ALIGNX_CENTER | NEMA_TEXT_WRAP | NEMA_ALIGNY_CENTER);

    return sizeof(str) - 1;
}

int
bench_draw_string(int blendmode)
{

    //g_sDejaVuSerif12pt8b.bo = nema_buffer_create_pool(NEMA_MEM_POOL_ASSETS, g_sDejaVuSerif12pt8b.bitmap_size);
    //nema_buffer_map(&g_sDejaVuSerif12pt8b.bo);
    _memcpy(g_sDejaVuSerif12pt8b.bo.base_virt, g_sDejaVuSerif12pt8b.bitmap, g_sDejaVuSerif12pt8b.bitmap_size);
    nema_buffer_flush(&g_sDejaVuSerif12pt8b.bo);

    g_sContextCL = nema_cl_create();
    g_sCL0 = nema_cl_create();
    g_sCL1 = nema_cl_create();
    g_psCLCur = &g_sCL0;

    nema_cl_bind(&g_sContextCL);
    //
    // Bind Framebuffer
    //
    nema_bind_dst_tex(g_sFB.bo.base_phys, g_sFB.w, g_sFB.h, (nema_tex_format_t)(g_sFB.format), -1);
    //
    // Set Clipping Rectangle
    //
    nema_set_clip(0, 0, FB_RESX, FB_RESY);
    //
    // Set Blending Mode
    //
    nema_set_blend_blit(blendmode);
    nema_bind_font(&g_sDejaVuSerif12pt8b);
    nema_string_get_bbox(str, &w, &h, FB_RESX, 1);

    int i32PixCount = 0;
    nema_bind_font(&g_sDejaVuSerif12pt8b);
    i32PixCount += CL_CHECK_SUBMIT(ITEMS_PER_CL <= 5 ? 1 : ITEMS_PER_CL / 5);

    nema_cl_destroy(&g_sContextCL);
    nema_cl_destroy(&g_sCL0);
    nema_cl_destroy(&g_sCL1);

    return i32PixCount;
}
