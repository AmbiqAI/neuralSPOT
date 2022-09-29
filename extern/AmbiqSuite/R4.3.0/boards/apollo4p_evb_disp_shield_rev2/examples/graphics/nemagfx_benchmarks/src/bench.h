//*****************************************************************************
//
//! @file bench.h
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

#ifndef __BENCH_H__
#define __BENCH_H__

#include "am_hal_global.h"

#include "nema_hal.h"
#include "nema_cmdlist.h"
#include "nema_utils.h"

#include "utils.h"

#ifndef ITEMS_PER_CL
#define ITEMS_PER_CL 90
#endif

extern ExecutionMode_e eExecMode;
extern TLS_VAR img_obj_t g_sAmbiqLogo;
extern TLS_VAR nema_cmdlist_t *g_psCLCur, g_sCL0, g_sCL1, g_sContextCL;

extern float start_wall, stop_wall;

static int i32RenderFrame(void);

#ifdef DEBUG_PRINTF
#define PRINTF am_util_debug_printf
#else
#define PRINTF(...)
#endif

void run_bench(int testno);

#define swap_cmd_lists() do {                   \
    g_psCLCur = (g_psCLCur == &g_sCL0) ? &g_sCL1 : &g_sCL0;    \
} while(0)

static inline int
CL_CHECK_SUBMIT(int i32ItemsPerCL)
{
    int _items_per_cl = i32ItemsPerCL;

    if (_items_per_cl == 0)
    {
        _items_per_cl = ITEMS_PER_CL;
    }

    int items = 0;
    int i32PixCount = 0;
    start_wall = nema_get_time();
    stop_wall = start_wall;

    nema_cl_bind(g_psCLCur);
    nema_cl_rewind(g_psCLCur);
    nema_cl_branch(&g_sContextCL);

    if (eExecMode != GPU_BOUND)
    {
        do
        {
            i32PixCount += i32RenderFrame();

            ++items;
            if ( (items % _items_per_cl) == 0)
            {
                if ( eExecMode != CPU_BOUND )
                {
                    nema_cl_return();
                    nema_cl_submit(g_psCLCur);
                    PRINTF("Submitted CL %d\r\n", g_psCLCur->submission_id);
                }

                swap_cmd_lists();
                PRINTF("Waiting for CL %d\r\n", g_psCLCur->submission_id);
                nema_cl_wait(g_psCLCur);

                nema_cl_bind(g_psCLCur);
                nema_cl_rewind(g_psCLCur);
                nema_cl_branch(&g_sContextCL);

                stop_wall = nema_get_time();
                if ((stop_wall-start_wall) > TIMEOUT_S)
                {
                    break;
                }
            }
        }
        while (1);

        PRINTF("Finishing Test\r\n");
        PRINTF("Waiting for CL %d\r\n", g_psCLCur->submission_id);
        nema_cl_wait(g_psCLCur);
        swap_cmd_lists();

        PRINTF("Waiting for CL %d\r\n", g_psCLCur->submission_id);
        nema_cl_wait(g_psCLCur);
    }
    else
    {
        int i;
        int pix_count_cl = 0;
        for ( i = 0; i < _items_per_cl; i++ )
        {
            pix_count_cl += i32RenderFrame();
        }
        while (1)
        {
            nema_cl_return();
            nema_cl_submit(g_psCLCur);
            nema_cl_wait(g_psCLCur);
            items += _items_per_cl;
            i32PixCount += pix_count_cl;
            stop_wall = nema_get_time();
            if ((stop_wall-start_wall) > TIMEOUT_S)
            {
                break;
            }

        }
    }

    stop_wall = nema_get_time();

    return i32PixCount;
}

#endif
