//*****************************************************************************
//
//! @file main.c
//!
//! @brief NemaGFX example.
//! this example use one frame buffer demonstrate two picture trasition effect,
//! with Nema GPU support, the effect include
//! NEMA_TRANS_LINEAR_H,
//! NEMA_TRANS_CUBE_H,
//! NEMA_TRANS_INNERCUBE_H,
//! NEMA_TRANS_STACK_H,
//! NEMA_TRANS_LINEAR_V,
//! NEMA_TRANS_CUBE_V,
//! NEMA_TRANS_INNERCUBE_V,
//! NEMA_TRANS_STACK_V,
//! NEMA_TRANS_FADE,
//! NEMA_TRANS_FADE_ZOOM,
//! NEMA_TRANS_MAX,
//! NEMA_TRANS_NONE
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

#include <string.h>

#include "am_bsp.h"
#include "am_util_delay.h"
#include "am_devices_dsi_rm67162.h"
#include "am_devices_nemadc_rm67162.h"

#include "nema_core.h"
#include "nema_utils.h"
#include "nema_dc.h"
#include "nema_dc_mipi.h"

#include "nema_transitions.h"
#include "nema_utils.h"
#include "nemagfx_transition_effects.h"

#include "im00.rgba565.h"
#include "im01.rgba565.h"

//
//#define PANEL_RESX 360              //!< panel's max resolution
//#define PANEL_RESY 360              //!< panel's max resolution
//
#define PANEL_RESX (g_sDispCfg[g_eDispType].ui32PanelResX/4*4)
#define PANEL_RESY (g_sDispCfg[g_eDispType].ui32PanelResY/4*4)

#define RESX 256
#define RESY 256

#define PANEL_OFFSET                ((PANEL_RESX-RESX)/2)

#define SMALLFB
#define SMALLFB_STRIPES 4

static img_obj_t g_sScreen0 = {
                        {.size = IM00_RGBA565_LEN,
                         .base_virt = (void *)g_ui8Im00RGBA565,
                         .base_phys = (uintptr_t)g_ui8Im00RGBA565},
                         256, 256, -1, 0, (uint8_t)NEMA_RGB565, 0};
static img_obj_t g_sScreen1 = {
                        {.size = IM01_RGBA565_LEN,
                         .base_virt = (void *)g_ui8Im01RGBA565,
                         .base_phys = (uintptr_t)g_ui8Im01RGBA565},
                         256, 256, -1, 0, (uint8_t)NEMA_RGB565, 0};

#define FRAME_BUFFERS 1
static img_obj_t g_sFB[FRAME_BUFFERS];
static nemadc_layer_t g_sLayer[3] = {{0}};

static void
load_objects(void)
{
    for (int32_t i = 0; i < FRAME_BUFFERS; ++i)
    {
        g_sFB[i].w = RESX;
        g_sFB[i].h = RESY;
        g_sFB[i].format = NEMA_RGB565;
        g_sFB[i].stride = RESX * 2;

#ifdef SMALLFB
        g_sFB[i].bo = nema_buffer_create(g_sFB[i].stride * g_sFB[i].h / SMALLFB_STRIPES);
#else
        g_sFB[i].bo = nema_buffer_create(g_sFB[i].stride * g_sFB[i].h);
#endif

        (void)nema_buffer_map(&g_sFB[i].bo);

        g_sLayer[i].sizex = g_sLayer[i].resx = g_sFB[i].w;
        g_sLayer[i].sizey = g_sLayer[i].resy = g_sFB[i].h;
        g_sLayer[i].stride = g_sFB[i].stride;
        g_sLayer[i].format = NEMADC_RGB565;
        g_sLayer[i].blendmode = NEMADC_BL_SRC;
        g_sLayer[i].baseaddr_phys = g_sFB[i].bo.base_phys;
        g_sLayer[i].baseaddr_virt = g_sFB[i].bo.base_virt;
        g_sLayer[i].flipx_en      = 0;
        g_sLayer[i].flipy_en      = 0;
    }

    //
    // Preload the textures from MRAM to SSRAM
    // Set the textures address in SSRAM for GPU
    //

    g_sScreen0.bo = nema_buffer_create(IM00_RGBA565_LEN);
    memcpy((void*)g_sScreen0.bo.base_phys, g_ui8Im00RGBA565, IM00_RGBA565_LEN);
    g_sScreen1.bo = nema_buffer_create(IM01_RGBA565_LEN);
    memcpy((void*)g_sScreen1.bo.base_phys, g_ui8Im01RGBA565, IM01_RGBA565_LEN);

}

static nema_cmdlist_t g_sCLDrawEntireScene;
static nema_cmdlist_t g_sCL;
static int32_t g_i32CurFB = 0;

static int32_t
init(void)
{
    int32_t i32Ret;
    uint32_t ui32MipiCfg = MIPICFG_8RGB888_OPT0; //!< default config
    //
    // Initialize NemaGFX
    //
    i32Ret = nema_init();
    if (i32Ret != 0)
    {
        return i32Ret;
    }
    //
    // Initialize Nema|dc
    //
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

#ifdef SMALLFB
    nemadc_timing(RESX, 4, 10, 10,
                  RESY / SMALLFB_STRIPES, 10, 50, 10);
#else
    nemadc_timing(RESX, 4, 10, 10,
                  RESY, 10, 50, 10);
#endif

    load_objects();

    g_sCLDrawEntireScene  = nema_cl_create();
    g_sCL  = nema_cl_create();

    return 0;
}

static nema_transition_t g_eEffect = NEMA_TRANS_LINEAR_H;

static void
display(float step)
{
    nema_cl_bind(&g_sCLDrawEntireScene);
    nema_cl_rewind(&g_sCLDrawEntireScene);

    nema_bind_tex(NEMA_TEX1, g_sScreen0.bo.base_phys,  \
        g_sScreen0.w, g_sScreen0.h, g_sScreen0.format, g_sScreen0.stride, NEMA_FILTER_BL | NEMA_TEX_BORDER);
    nema_bind_tex(NEMA_TEX2, g_sScreen1.bo.base_phys,  \
        g_sScreen1.w, g_sScreen1.h, g_sScreen1.format, g_sScreen1.stride, NEMA_FILTER_BL | NEMA_TEX_BORDER);

    nema_set_tex_color(0);

    nema_transition(g_eEffect, NEMA_TEX1, NEMA_TEX2, NEMA_BL_SRC, step, g_sFB[0].w, g_sFB[0].h);

#ifdef SMALLFB
    for (int32_t stripe = 0; stripe < SMALLFB_STRIPES; ++stripe)
    {
        nema_cl_bind(&g_sCL);
        nema_cl_rewind(&g_sCL);

        //!< Set Clipping Rectangle
        nema_set_clip(0, stripe * RESY / SMALLFB_STRIPES, RESX, RESY / SMALLFB_STRIPES);
        //!< Bind Framebuffer
        nema_bind_dst_tex(g_sLayer[g_i32CurFB].baseaddr_phys - stripe * RESY / SMALLFB_STRIPES * g_sFB[0].stride, g_sFB[0].w, g_sFB[0].h, g_sFB[0].format, g_sFB[0].stride);

        nema_cl_jump(&g_sCLDrawEntireScene);
        nema_cl_submit(&g_sCL);
        (void)nema_cl_wait(&g_sCL);

        if (g_sDispCfg[g_eDispType].eInterface == IF_DSI)
        {
            am_devices_dsi_rm67162_set_region(RESX, RESY / SMALLFB_STRIPES, PANEL_OFFSET, PANEL_OFFSET + (RESY / SMALLFB_STRIPES)*stripe);
        }
        else if (g_sDispCfg[g_eDispType].eInterface == IF_QSPI)
        {
            am_devices_nemadc_rm67162_set_region(MIPICFG_QSPI, RESX, RESY / SMALLFB_STRIPES, PANEL_OFFSET, PANEL_OFFSET + (RESY / SMALLFB_STRIPES) * stripe);
        }
        else if (g_sDispCfg[g_eDispType].eInterface == IF_SPI4)
        {
            am_devices_nemadc_rm67162_set_region(MIPICFG_SPI4, RESX, RESY / SMALLFB_STRIPES, PANEL_OFFSET, PANEL_OFFSET + (RESY / SMALLFB_STRIPES) * stripe);
        }
        else if (g_sDispCfg[g_eDispType].eInterface == IF_DSPI)
        {
            am_devices_nemadc_rm67162_set_region(MIPICFG_DSPI, RESX, RESY / SMALLFB_STRIPES, PANEL_OFFSET, PANEL_OFFSET + (RESY / SMALLFB_STRIPES) * stripe);
        }

        nemadc_set_layer(0, &g_sLayer[0]);
        if (g_sDispCfg[g_eDispType].eInterface == IF_DSI)
        {
            dsi_send_frame_single(NEMADC_OUTP_OFF);
        }
        else
        {
            nemadc_send_frame_single();
        }

    }
#else
    nema_cl_bind(&g_sCL);
    nema_cl_rewind(&g_sCL);

    //!< Set Clipping Rectangle
    nema_set_clip(0, 0, RESX, RESY);
    //!< Bind Framebuffer
    nema_bind_dst_tex(g_sLayer[g_i32CurFB].baseaddr_phys, g_sFB[0].w, g_sFB[0].h, g_sFB[0].format, g_sFB[0].stride);

    nema_cl_jump(&g_sCLDrawEntireScene);

    nema_cl_submit(&g_sCL);
    nema_cl_wait(&g_sCL);

    nemadc_set_layer(0, &g_sLayer[0]);
    if (g_sDispCfg[g_eDispType].eInterface == IF_DSI)
    {
        dsi_send_frame_single(NEMADC_OUTP_OFF);
    }
    else
    {
        nemadc_send_frame_single();
    }
#endif
}

#define ANIMATION_STEP_0_1 0.02f

static void
next_effect(void)
{
    g_eEffect = (nema_transition_t)(((int32_t)g_eEffect + 1) % NEMA_TRANS_MAX);
}

static void
loop(void)
{
#define MIN_STEP          0.f //0.18 //0.f
#define MAX_STEP          1.f //0.24 //1.f
    float step = MIN_STEP;
    float step_step = ANIMATION_STEP_0_1;
#if defined(BAREMETAL) && defined(APOLLO4_FPGA) && defined(AM_PART_APOLLO4P)
    am_hal_stimer_config(AM_HAL_STIMER_HFRC_6MHZ | AM_HAL_STIMER_CFG_RUN);
#endif
    while (1)
    {
        display(step);

        if (step <= MIN_STEP)
        {
            step = MIN_STEP;
            step_step = ANIMATION_STEP_0_1;
            next_effect();
        }
        else if (step >= MAX_STEP)
        {
            step = MAX_STEP;
            step_step = -ANIMATION_STEP_0_1;
            next_effect();
        }

        step += step_step;

        nema_calculate_fps();
    }
}

int32_t
transition_effects(void)
{
    init();

    loop();

    return 0;
}

