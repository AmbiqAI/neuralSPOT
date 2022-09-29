//*****************************************************************************
//
//! @file main.c
//!
//! @brief NemaGFX example.
//! this example use two frame buffer demonstrate a digital Quartz clock,
//! with Nema GPU support, the shader effect continue shows while timer passing
//! need a timer to get the accurate time past.
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
#include "nema_event.h"
#include "nema_dc.h"
#include "watch.h"
#include "nema_dc_mipi.h"
#include "am_devices_dsi_rm67162.h"
#include "am_devices_nemadc_rm67162.h"

#define FRAME_BUFFERS 2
#define FB_GPU  0
#define FB_DC   1
#define NUM_LAYERS 1U

static uintptr_t triple_fbs[NUM_LAYERS][3];

img_obj_t fb[FRAME_BUFFERS];
nemadc_layer_t layer[FRAME_BUFFERS] = {{0}};
static nema_cmdlist_t cl;
static int cur_fb = 1;

float start_time;
float end_time;

uintptr_t
nema_init_triple_fb(int layer, uintptr_t fb0_phys, uintptr_t fb1_phys, uintptr_t fb2_phys)
{
    //actually doing always 2 framebuffers
    //fb2_phys is ignored

    triple_fbs[layer][FB_GPU]  = fb0_phys;
    triple_fbs[layer][FB_DC]   = fb1_phys;

    return triple_fbs[layer][FB_GPU];
}

uintptr_t
nema_swap_layer(int layer)
{
    if (layer < 0)
    {
        layer = 0;
    }

    {
        uintptr_t tmp = triple_fbs[layer][FB_DC];
        triple_fbs[layer][FB_DC] = triple_fbs[layer][FB_GPU];
        triple_fbs[layer][FB_GPU]  = tmp;

        nemadc_set_layer_addr(layer, triple_fbs[layer][FB_DC]);
    }
    return triple_fbs[layer][FB_GPU];
}

static void
swap_fb(void)
{
    cur_fb = (cur_fb + 1) % FRAME_BUFFERS;
}

void flush_screen(void)
{
    if (g_sDispCfg[g_eDispType].eInterface == IF_DSI)
    {
        dsi_send_frame_single(NEMADC_OUTP_OFF);
    }
    else
    {
        nemadc_send_frame_single();
    }

}

static void
load_objects(void)
{
    for (int i = 0; i < FRAME_BUFFERS; ++i)
    {
        fb[i].w = RESX;
        fb[i].h = RESY;
        fb[i].format = NEMA_RGB565;
        fb[i].stride = RESX * 2;
        fb[i].bo = nema_buffer_create(fb[i].stride*fb[i].h);
        (void)nema_buffer_map(&fb[i].bo);

        layer[i].sizex = layer[i].resx = fb[i].w;
        layer[i].sizey = layer[i].resy = fb[i].h;
        layer[i].stride = fb[i].stride;
        layer[i].format = NEMADC_RGB565;
        layer[i].blendmode = NEMADC_BL_SRC;
        layer[i].baseaddr_phys = fb[i].bo.base_phys;
        layer[i].baseaddr_virt = fb[i].bo.base_virt;
        layer[i].flipx_en      = 0;
        layer[i].flipy_en      = 0;

        // printf("FB: V:%p P:0x%08x\n", (void *)fb[i].bo.base_virt, fb[i].bo.base_phys);
    }
}

uintptr_t cur_fb_base_phys;

#define DC_LAYER_TO_USE 0
int watchface()
{
    int ret;
    uint32_t ui32MipiCfg = MIPICFG_8RGB888_OPT0; // default config
    //Initialize NemaGFX
    ret = nema_init();
    if (ret != 0)
    {
        return ret;
    }
    //Initialize Nema|dc
    ret = nemadc_init();
    if (ret != 0)
    {
        return ret;
    }

    if ((g_sDispCfg[g_eDispType].eInterface == IF_DSI) || (g_sDispCfg[g_eDispType].bUseDPHYPLL == true))
    {
        uint8_t ui8LanesNum = g_sDsiCfg.ui8NumLanes;
        uint8_t ui8DbiWidth = g_sDsiCfg.eDbiWidth;
        uint32_t ui32FreqTrim = g_sDsiCfg.eDsiFreq;
        pixel_format_t eFormat = FMT_RGB888;
        if ( am_hal_dsi_para_config(ui8LanesNum, ui8DbiWidth, ui32FreqTrim) != 0 )
        {
            return -3;
        }
        switch ( eFormat )
        {
            case FMT_RGB888:
                if ( ui8DbiWidth == 16 )
                {
                    ui32MipiCfg = MIPICFG_16RGB888_OPT0;
                }
                if ( ui8DbiWidth ==  8 )
                {
                    ui32MipiCfg = MIPICFG_8RGB888_OPT0;
                }
                break;

            case FMT_RGB565:
                if ( ui8DbiWidth == 16 )
                {
                    ui32MipiCfg = MIPICFG_16RGB565_OPT0;
                }
                if ( ui8DbiWidth ==  8 )
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

    uint16_t panel_resx = g_sDispCfg[g_eDispType].ui32PanelResX; //panel's max resolution
    uint16_t panel_resy = g_sDispCfg[g_eDispType].ui32PanelResY; //panel's max resolution

    uint16_t minx, miny;

    //Set the display region to center
    if ( RESX > panel_resx )
    {
        minx = 0; // set the minimum value to 0
    }
    else
    {
        minx = (panel_resx - RESX) >> 1;
        minx = (minx >> 1) << 1;
    }

    if ( RESY > panel_resy )
    {
        miny = 0; // set the minimum value to 0
    }
    else
    {
        miny = (panel_resy - RESY) >> 1;
        miny = (miny >> 1) << 1;
    }

    //Initialize the display
    switch (g_sDispCfg[g_eDispType].eInterface)
    {
        case IF_SPI4:
            am_devices_nemadc_rm67162_init(MIPICFG_SPI4, MIPICFG_1RGB565_OPT0, RESX, RESY, minx, miny);
            break;
        case IF_DSPI:
            am_devices_nemadc_rm67162_init(MIPICFG_DSPI | MIPICFG_SPI4, MIPICFG_2RGB565_OPT0, RESX, RESY, minx, miny);
            break;
        case IF_QSPI:
            am_devices_nemadc_rm67162_init(MIPICFG_QSPI | MIPICFG_SPI4, MIPICFG_4RGB565_OPT0, RESX, RESY, minx, miny);
            break;
        case IF_DSI:
            am_devices_dsi_rm67162_init(ui32MipiCfg, RESX, RESY, minx, miny);
            break;
        default:
            ; //NOP
    }

    load_objects();

    cur_fb_base_phys = nema_init_triple_fb(DC_LAYER_TO_USE, layer[0].baseaddr_phys,
                                                            layer[1].baseaddr_phys,
                                                            0);

    init_watch(
        1,          //draw backface (bf)
        1,          //bf is texture
        0x10204080  //if (bf) isn't texture, fill with this color
        );

    cl = nema_cl_create_sized(0x100);
#if defined(BAREMETAL) && defined(APOLLO4_FPGA) && defined(AM_PART_APOLLO4P)
    am_hal_stimer_config(AM_HAL_STIMER_HFRC_6MHZ | AM_HAL_STIMER_CFG_RUN);
#endif
    while(1)
    {
        float time = nema_get_time();

        float sec  = time;
        float min  = time / 60;
        float hour = time / 60 / 60;

        //draw_watchface returns a CL pointer that draws the watchface
        //the destination texture needs to be bound beforehand
        // am_util_stdio_printf("hour = %.02f, min = %.02f, sec = %.02f\r\n", hour, min, sec);

/*         start_time = nema_get_time(); */
        nema_cmdlist_t *wf_cl = draw_watchface(hour, min, sec);
/*
        end_time = nema_get_time();
        am_util_stdio_printf("used %f s\n", end_time - start_time);
*/

        //rewind and bind the CL
        nema_cl_rewind(&cl);
        nema_cl_bind(&cl);

        //bind the destination buffer
        nema_bind_dst_tex(cur_fb_base_phys, RESX, RESY, fb[cur_fb].format, RESX*2);

        //branch to the watchface CL
        nema_cl_branch(wf_cl);

        //submit and wait for the CL
        nema_cl_submit(&cl);
        nemadc_set_layer(0, &layer[cur_fb]);
        flush_screen();

        nema_cl_wait(&cl);

        //swap frame buffer
        cur_fb_base_phys = nema_swap_layer(DC_LAYER_TO_USE);

        swap_fb();

        // nemadc_set_layer_with_color_convert(0, &layer[0]);
        nema_calculate_fps();
    }

}

