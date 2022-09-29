//*****************************************************************************
//
//! @file main.c
//!
//! @brief NemaGFX example.
//! Nemagfx_tsc_fb is a demo of TSC frame-buffer compression. The program uses
//! TSC4-compressed frame-buffer during run-time. It saves frame-buffer space
//! in RAM in a scale of 1:4 also.The demo use example NEMADC_TSC4 frame buffer
//! shows a 400x400 TSC4 image on the screen, it will significantly save RAM use.
//! Note:  the width and height of the frame-buffer should be 4-pixels aligned

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
#include "am_bsp.h"
#include "nema_core.h"
#include "nema_utils.h"
#include "am_hal_global.h"

#ifndef DONT_USE_NEMADC
#include "nema_dc.h"
#include "nema_dc_mipi.h"
#endif

#include "oli_400x400_tsc4.h"
#include "am_devices_dsi_rm67162.h"
#include "am_devices_nemadc_rm67162.h"

#define RESX 390
#define RESY 390

static img_obj_t g_sFB = {{0}, RESX, RESY, -1, 0, NEMA_TSC4, 0};
img_obj_t g_sTSiLogo = {{0}, 400, 400, -1, 0, NEMA_TSC4, 0};

#ifndef DONT_USE_NEMADC
nemadc_layer_t g_sDCLayer = {(void *)0, 0, RESX, RESY, -1, 0, 0, RESX, RESY, 0xff, NEMADC_BL_SRC, 0, NEMADC_TSC4, 0, 0, 0, 0, 0, 0, 0};
#endif

void
load_objects(void)
{
    //!< Load memory objects
    g_sFB.bo = nema_buffer_create(g_sFB.stride * g_sFB.h);
    nema_buffer_map(&g_sFB.bo);
    printf("FB: V:%p P:0x%08x\n", (void *)g_sFB.bo.base_virt, g_sFB.bo.base_phys);

#ifndef DONT_USE_NEMADC
    g_sDCLayer.baseaddr_phys = g_sFB.bo.base_phys;
    g_sDCLayer.baseaddr_virt = g_sFB.bo.base_virt;
#endif

    printf("FB: V:%p P:0x%08x\n", (void *)g_sFB.bo.base_virt, g_sFB.bo.base_phys);

    g_sTSiLogo.bo.base_phys = (uintptr_t)(g_ui8Oli400x400Tsc4);
    g_sTSiLogo.bo.base_virt = (void*)(g_sTSiLogo.bo.base_phys);
}

AM_SHARED_RW nema_cmdlist_t g_sCL;
int32_t
tsc4_image_show()
{
    int32_t i32Ret;
    uint32_t ui32MipiCfg = MIPICFG_8RGB888_OPT0; //!< default config
    //!< Initialize NemaGFX
    i32Ret = nema_init();
    if (i32Ret != 0)
    {
        return i32Ret;
    }
    //!< Initialize Nema|dc
    i32Ret = nemadc_init();
    if (i32Ret != 0)
    {
        return i32Ret;
    }

    if ((g_sDispCfg[g_eDispType].eInterface == IF_DSI) || (g_sDispCfg[g_eDispType].bUseDPHYPLL == true))
    {
        uint8_t ui8LanesNum = g_sDsiCfg.ui8NumLanes;
        uint8_t ui8DbiWidth = g_sDsiCfg.eDbiWidth;
        uint32_t ui32FreqTrim = g_sDsiCfg.eDsiFreq;
        pixel_format_t eFormat = FMT_RGB888;
        if (am_hal_dsi_para_config(ui8LanesNum, ui8DbiWidth, ui32FreqTrim) != 0)
        {
            return -3;
        }
        switch (eFormat)
        {
            case FMT_RGB888:
                if (ui8DbiWidth == 16)
                {
                    ui32MipiCfg = MIPICFG_16RGB888_OPT0;
                }
                if (ui8DbiWidth == 8)
                {
                    ui32MipiCfg = MIPICFG_8RGB888_OPT0;
                }
                break;

            case FMT_RGB565:
                if (ui8DbiWidth == 16)
                {
                    ui32MipiCfg = MIPICFG_16RGB565_OPT0;
                }
                if (ui8DbiWidth == 8)
                {
                    ui32MipiCfg = MIPICFG_8RGB565_OPT0;
                }
                break;

            default:
                //
                // invalid color component index
                //
                return -3;
        }
    }

    uint16_t ui16PanelResX = g_sDispCfg[g_eDispType].ui32PanelResX; //!< panel's max resolution
    uint16_t ui16PanelResY = g_sDispCfg[g_eDispType].ui32PanelResY; //!< panel's max resolution

    uint16_t ui16MinX, ui16MinY;

    //!< Set the display region to center
    if (RESX > ui16PanelResX)
    {
        ui16MinX = 0;   //!< set the minimum value to 0
    }
    else
    {
        ui16MinX = (ui16PanelResX - RESX) >> 1;
        ui16MinX = (ui16MinX >> 1) << 1;
    }

    if (RESY > ui16PanelResY)
    {
        ui16MinY = 0;   //!< set the minimum value to 0
    }
    else
    {
        ui16MinY = (ui16PanelResY - RESY) >> 1;
        ui16MinY = (ui16MinY >> 1) << 1;
    }

    //!< Initialize the display
    switch (g_sDispCfg[g_eDispType].eInterface)
    {
        case IF_SPI4:
            am_devices_nemadc_rm67162_init(MIPICFG_SPI4, MIPICFG_1RGB565_OPT0, RESX, RESY, ui16MinX, ui16MinY);
            break;
        case IF_DSPI:
            am_devices_nemadc_rm67162_init(MIPICFG_DSPI | MIPICFG_SPI4, MIPICFG_2RGB565_OPT0, RESX, RESY, ui16MinX, ui16MinY);
            break;
        case IF_QSPI:
            am_devices_nemadc_rm67162_init(MIPICFG_QSPI | MIPICFG_SPI4, MIPICFG_4RGB565_OPT0, RESX, RESY, ui16MinX, ui16MinY);
            break;
        case IF_DSI:
            am_devices_dsi_rm67162_init(ui32MipiCfg, RESX, RESY, ui16MinX, ui16MinY);
            break;
        default:
            ; //NOP
    }

    load_objects();

    g_sCL = nema_cl_create();

    nema_cl_bind(&g_sCL);

    nema_bind_dst_tex(g_sFB.bo.base_phys, g_sFB.w, g_sFB.h, (nema_tex_format_t)(g_sFB.format), g_sFB.stride);
    nema_bind_src_tex(g_sTSiLogo.bo.base_phys, g_sTSiLogo.w, g_sTSiLogo.h, (nema_tex_format_t)(g_sTSiLogo.format), -1, NEMA_FILTER_BL);

    nema_set_clip(0, 0, RESX, RESY);
    nema_set_blend_blit(NEMA_BL_SRC);

    nema_blit(0, 0);
    nema_set_blend_fill(NEMA_BL_SRC);

    nema_cl_unbind();
    nema_cl_submit(&g_sCL);
    nema_cl_wait(&g_sCL);
    nemadc_set_layer(0, &g_sDCLayer);
    if (g_sDispCfg[g_eDispType].eInterface == IF_DSI)
    {
        dsi_send_frame_single(NEMADC_OUTP_OFF);
    }
    else
    {
        nemadc_send_frame_single();
    }

    nema_cl_destroy(&g_sCL);

    return 0;
}

