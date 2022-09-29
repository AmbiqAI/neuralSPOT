//*****************************************************************************
//
//! @file bench_textured_quad.c
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
#include "nema_matrix4x4.h"

#include "bench.h"

static int
i32RenderFrame()
{
    int i32BBoxW = nema_rand() % (FB_RESX - 50) + 50;
    int i32BBoxH = nema_rand() % (FB_RESY - 50) + 50;
    int i32BBoxX = nema_rand() % (FB_RESX - i32BBoxW);
    int i32BBoxY = nema_rand() % (FB_RESY - i32BBoxH);

    int x0 = (int)(i32BBoxX + (float)(nema_rand() % (100)) / 100.f * i32BBoxW);
    int y0 = i32BBoxY;
    int x1 = i32BBoxX + i32BBoxW;
    int y1 = (int)(i32BBoxY + (float)(nema_rand() % (100)) / 100.f * i32BBoxH);
    int x2 = (int)(i32BBoxX + (float)(nema_rand() % (100)) / 100.f * i32BBoxW);
    int y2 = i32BBoxY + i32BBoxH;
    int x3 = i32BBoxX;
    int y3 = (int)(i32BBoxY + (float)(nema_rand() % (100)) / 100.f * i32BBoxH);

    nema_blit_quad_fit(x0, y0, x1, y1, x2, y2, x3, y3);

    int area = (int)(0.5f*nema_abs(x0 * (y1 - y3) +
                             x1 * (y2 - y0) +
                             x2 * (y3 - y1) +
                             x3 * (y0 - y2) ));
    return area;
}

int
bench_textured_quad(int i32BlendMode, int i32NemaTexMode)
{
    g_sContextCL = nema_cl_create();
    g_sCL0 = nema_cl_create();
    g_sCL1 = nema_cl_create();
    g_psCLCur = &g_sCL0;

    nema_cl_bind(&g_sContextCL);
    //
    // Bind Framebuffer
    //
    nema_bind_dst_tex(g_sFB.bo.base_phys, g_sFB.w, g_sFB.h, (nema_tex_format_t)(g_sFB.format), -1);
    nema_bind_src_tex(g_sAmbiqLogo.bo.base_phys, g_sAmbiqLogo.w, g_sAmbiqLogo.h, (nema_tex_format_t)(g_sAmbiqLogo.format), g_sAmbiqLogo.stride, (nema_tex_mode_t)i32NemaTexMode);
    //
    // Set Clipping Rectangle
    //
    nema_set_clip(0, 0, FB_RESX, FB_RESY);
    //
    // Set Blending Mode
    //
    nema_set_blend_blit(i32BlendMode);
    //-----------------------------------------------------------------------

    int i32PixCount = 0;
    i32PixCount += CL_CHECK_SUBMIT(0);

    nema_cl_destroy(&g_sContextCL);
    nema_cl_destroy(&g_sCL0);
    nema_cl_destroy(&g_sCL1);

    return i32PixCount;
}
