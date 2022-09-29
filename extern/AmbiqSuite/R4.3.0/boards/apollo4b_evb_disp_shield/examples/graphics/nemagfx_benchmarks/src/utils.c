//*****************************************************************************
//
//! @file utils.c
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

#include "nema_core.h"
#include "nema_regs.h"
#include "nema_dc.h"
#include "nema_dc_mipi.h"
#include "utils.h"

#include "am_util_stdio.h"
#include "Ambiq200x104.rgba.h"
#include "am_devices_dsi_rm67162.h"
#include "am_devices_nemadc_rm67162.h"
#include "am_bsp.h"

float start_clk;
float start_wall;
float stop_clk;
float stop_wall;

#ifndef GPU_FREQ_MHZ
#define GPU_FREQ_MHZ 50
#endif

TLS_VAR img_obj_t g_sFB = {{0}, (FB_RESX / 4) * 4, (FB_RESY / 4) * 4, -1, 0, NEMA_RGB565, 0};
TLS_VAR img_obj_t g_sAmbiqLogo = {{0}, 200, 104, -1, 0, NEMA_RGBA8888, 0};
static TLS_VAR nemadc_layer_t sDCLayer = {(void *)0, 0, (FB_RESX / 4) * 4, (FB_RESY / 4) * 4, -1, 0, 0, (FB_RESX / 4) * 4, (FB_RESY / 4) * 4, 0xff, NEMADC_BL_SRC, 0, NEMADC_RGB565, 0, 0, 0, 0, 0, 0, 0};

static void print_test(int i32TestNo);
static void bench_report(float perf, char *pcMesUnit, float utilization);

void print_test(int i32TestNo)
{
    const char *pcTestsStr[TEST_MAX + 1] =
    {
        "_",
        "Fill_Triangle              ",
        "Fill_Triangle_Blend        ",
        "Fill_Rectangle             ",
        "Fill_Rectangle_Blend       ",
        "Fill_Quad                  ",
        "Fill_Quad_Blend            ",
        "Draw_String                ",
        "Draw_Line                  ",
        "Draw_Line_Blend            ",
        "Draw_Rectangle            ",
        "Draw_Rectangle_Blend      ",
        "Blit                      ",
        "Blit_Colorize             ",
        "Blit_Blend                ",
        "Blit_Blend_Colorize       ",
        "Blit_90                   ",
        "Blit_180                  ",
        "Blit_270                  ",
        "Blit_Vertical_Flip        ",
        "Blit_Horizontal_Flip      ",
        "Blit_SRC_Colorkeyed       ",
        "Blit_DST_Colorkeyed       ",
        "Stretch_Blit_PS           ",
        "Stretch_Blit_Blend_PS     ",
        "Stretch_Blit_BL           ",
        "Stretch_Blit_Blend_BL     ",
        "Stretch_Blit_Rotate       ",
        "Stretch_Blit_Rotate_BL    ",
        "Textured_Triangle_PS      ",
        "Textured_Triangle_Blend_PS",
        "Textured_Triangle_BL      ",
        "Textured_Triangle_Blend_BL",
        "Textured_Quad_PS          ",
        "Textured_Quad_Blend_PS    ",
        "Textured_Quad_BL          ",
        "Textured_Quad_Blend_BL    "
    };

    if (i32TestNo <= 0)
    {
        for (i32TestNo = 1; i32TestNo <= TEST_MAX; ++i32TestNo)
        {
            am_util_stdio_printf("%d    %s\r\n", i32TestNo, pcTestsStr[i32TestNo]);
        }
    }
    else if (i32TestNo > TEST_MAX)
    {
        am_util_stdio_printf("%d is not a valid test\n", i32TestNo);
    }
    else
    {
        am_util_stdio_printf("%d: %s    ", i32TestNo, pcTestsStr[i32TestNo]);
    }
}

int parse_params(int* i32TestNo, ExecutionMode_e* eMode, int i32Argc, char* pcArgv[])
{
    return 0;
}

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

static void clear_background(uint32_t col)
{
    // Create CmdList
    static TLS_VAR nema_cmdlist_t cl;
    cl = nema_cl_create();
    // Bind CmdList
    nema_cl_bind(&cl);
    // Bind Framebuffer
    nema_bind_dst_tex(g_sFB.bo.base_phys, g_sFB.w, g_sFB.h, (nema_tex_format_t)(g_sFB.format), g_sFB.stride);
    nema_set_clip(0, 0, FB_RESX, FB_RESY);
    // Clear color
    nema_clear(col);

    nema_cl_unbind();
    nema_cl_submit(&cl);
    nema_cl_wait(&cl);

    nema_cl_destroy(&cl);
}

#define ALLOC_TSILOGO_MEM

void suite_init()
{
#ifdef ALLOC_TSILOGO_MEM
    g_sAmbiqLogo.bo = nema_buffer_create_pool(NEMA_MEM_POOL_ASSETS, g_sAmbiqLogo.w * g_sAmbiqLogo.h * 4);

    _memcpy(g_sAmbiqLogo.bo.base_virt, ui8Ambiq200x104, g_sAmbiqLogo.w * g_sAmbiqLogo.h * 4);
    nema_buffer_flush(&g_sAmbiqLogo.bo);
#else
    g_sAmbiqLogo.bo.base_virt = (void *)ui8Ambiq200x104;
    g_sAmbiqLogo.bo.base_phys = (uintptr_t)ui8Ambiq200x104;
    g_sAmbiqLogo.bo.size      = g_sAmbiqLogo.w*g_sAmbiqLogo.h*4;
#endif
    //
    // Load framebuffer
    //
    g_sFB.bo = nema_buffer_create_pool(NEMA_MEM_POOL_FB, g_sFB.w * g_sFB.h * 4);
    nema_buffer_map(&g_sFB.bo);

    sDCLayer.baseaddr_phys = g_sFB.bo.base_phys;
    sDCLayer.baseaddr_virt = g_sFB.bo.base_virt;
#if defined(BAREMETAL) && defined(APOLLO4_FPGA) && defined(AM_PART_APOLLO4P)
    am_hal_stimer_config(AM_HAL_STIMER_HFRC_6MHZ | AM_HAL_STIMER_CFG_RUN);
#endif
}

void
suite_terminate()
{
    nema_buffer_destroy(&g_sFB.bo);

#ifdef ALLOC_TSILOGO_MEM
    nema_buffer_destroy(&g_sAmbiqLogo.bo);
#endif
}

void
bench_start(int i32TestNo)
{
    clear_background(0x0);

    print_test(i32TestNo);

    start_clk = nema_get_time();
}

float perfs[64];
void
bench_report(float perf, char *pcMesUnit, float utilization)
{
    int perf_i = (int)perf;
    perf = perf - perf_i;
    int perf_f = (int)(perf * 100.f);
    am_util_stdio_printf("%d.%d %s\r\n", perf_i, perf_f, pcMesUnit);
}

void
bench_stop(int i32TestNo, int i32PixCount)
{
    stop_clk = nema_get_time();
    nema_buffer_flush(&g_sFB.bo);
    nemadc_set_layer(0, &sDCLayer);

    if (g_sDispCfg[g_eDispType].eInterface == IF_DSI)
    {
        dsi_send_frame_single(NEMADC_OUTP_OFF);
    }
    else
    {
        nemadc_send_frame_single();
    }

    float total_cpu_s  = stop_wall - start_wall;

    if (i32TestNo == 7)
    {
        //
        // test 7 - draw_strings - is measured in KChars/sec
        //
        bench_report(i32PixCount / 1000.f / total_cpu_s, " KChars/sec", 0.f);
    }
    else if (i32TestNo == 37 || i32TestNo == 38 )
    {
        bench_report(i32PixCount / 1000.f / total_cpu_s, " KCLs/sec", 0.f);
    }
    else
    {
        bench_report(i32PixCount / 1000.f / 1000.f / total_cpu_s, "MPixels/sec", 0.f);
    }

    // sleep(1);
}

