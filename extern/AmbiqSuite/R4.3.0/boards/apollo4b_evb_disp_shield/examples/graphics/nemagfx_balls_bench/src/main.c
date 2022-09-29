//*****************************************************************************
//
//! @file main.c
//!
//! @brief NemaGFX example.

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

#include "am_bsp.h"
#include "nema_core.h"
#include "nema_utils.h"
#include "am_hal_global.h"
#include "stdlib.h"
#include "am_util_stdio.h"

#ifndef DONT_USE_NEMADC
#include "nema_dc.h"
#include "nema_dc_mipi.h"
#endif

#include "am_devices_dsi_rm67162.h"
#include "am_devices_nemadc_rm67162.h"

#ifndef BAREMETAL
    #include "FreeRTOS.h"
    #include "task.h"
#endif

#define RESX (g_sDispCfg[g_eDispType].ui32PanelResX / 64 * 64)
#define RESY (g_sDispCfg[g_eDispType].ui32PanelResY / 64 * 64)

#define FRAME_BUFFERS  2
static img_obj_t g_sFBs[FRAME_BUFFERS];
static nemadc_layer_t sLayer[FRAME_BUFFERS];

float start_time;
float end_time;

static void
fb_reload_rgb565_2(void)
{
    for (int i = 0; i < FRAME_BUFFERS; ++i)
    {
        g_sFBs[i].w = RESX;
        g_sFBs[i].h = RESY;
        g_sFBs[i].format = NEMA_RGB565;
        g_sFBs[i].stride = RESX * 2;
        g_sFBs[i].bo = nema_buffer_create(g_sFBs[i].stride * g_sFBs[i].h);
        (void)nema_buffer_map(&g_sFBs[i].bo);

        sLayer[i].startx = sLayer[i].starty = 0;
        sLayer[i].sizex = sLayer[i].resx = g_sFBs[i].w;
        sLayer[i].sizey = sLayer[i].resy = g_sFBs[i].h;
        sLayer[i].stride = g_sFBs[i].stride;
        sLayer[i].format = (uint32_t)(NEMADC_RGB565 | (uint32_t)NEMADC_MODULATE_A);

        sLayer[i].blendmode = NEMADC_BL_SIMPLE;
        sLayer[i].alpha     = 0x80;
        sLayer[i].flipx_en      = 0;
        sLayer[i].flipy_en      = 0;
        sLayer[i].baseaddr_phys = g_sFBs[i].bo.base_phys;
        sLayer[i].baseaddr_virt = g_sFBs[i].bo.base_virt;
    }
}

static int i32CurFB = 0;
static int i32LastFB = -1;

static void
swap_fb(void)
{
    i32LastFB = i32CurFB;
    i32CurFB = (i32CurFB + 1) % FRAME_BUFFERS;
}

#define MAX_CIRCLE_NUM               (15)

typedef struct
{
    int i32LastX, i32LastY;
    int i32CenterX, i32CenterY;
    int i32Radius;
    int speed_x, speed_y;
    unsigned char red, green, blue, aplha;
    unsigned char isFilled;
    char direction;
}
Circle_t;

Circle_t g_sCircle[MAX_CIRCLE_NUM];

void
update_circle(void)
{
    uint16_t i;
    int16_t i16CurrentY;
    int16_t i16CurrentX;

    for (i = 0; i < MAX_CIRCLE_NUM; i++)
    {
        i16CurrentY = g_sCircle[i].i32CenterY + g_sCircle[i].speed_y;
        if (i16CurrentY > RESY - g_sCircle[i].i32Radius || i16CurrentY < g_sCircle[i].i32Radius)
        {
            if (g_sCircle[i].speed_y > 0)
            {
                g_sCircle[i].i32CenterY = RESY - g_sCircle[i].i32Radius;
                g_sCircle[i].speed_y = -g_sCircle[i].speed_y;
            }
            else
            {
                g_sCircle[i].i32CenterY = g_sCircle[i].i32Radius;
                g_sCircle[i].speed_y = abs(g_sCircle[i].speed_y);
            }
        }
        else
        {
            g_sCircle[i].i32CenterY = i16CurrentY;
        }
    }

    for (i = 0; i < MAX_CIRCLE_NUM; i++)
    {
        i16CurrentX = g_sCircle[i].i32CenterX + g_sCircle[i].speed_x;
        if (i16CurrentX > RESX - g_sCircle[i].i32Radius || i16CurrentX < g_sCircle[i].i32Radius)
        {
            if (g_sCircle[i].speed_x > 0)
            {
                g_sCircle[i].i32CenterX = RESX - g_sCircle[i].i32Radius;
                g_sCircle[i].speed_x = -g_sCircle[i].speed_x;
            }
            else
            {
                g_sCircle[i].i32CenterX = g_sCircle[i].i32Radius;
                g_sCircle[i].speed_x = abs(g_sCircle[i].speed_x);
            }
        }
        else
        {
            g_sCircle[i].i32CenterX = i16CurrentX;
        }
    }

    for (i = 0; i < MAX_CIRCLE_NUM; i++)
    {
        nema_fill_circle(g_sCircle[i].i32CenterX, g_sCircle[i].i32CenterY, g_sCircle[i].i32Radius,
                         nema_rgba(g_sCircle[i].red, g_sCircle[i].green, g_sCircle[i].blue, g_sCircle[i].aplha));
    }
}

void
test_blit_balls(void)
{
    nema_cmdlist_t sCLCircles;
    uint16_t i;
    uint32_t ui32PrintTimer = 0;
    uint8_t ui8FlagDC = 0;

    srand(1);

    for (i = 0; i < MAX_CIRCLE_NUM; i++)
    {
        g_sCircle[i].i32Radius = rand() % 100;
        g_sCircle[i].i32CenterX = rand() % (RESX - g_sCircle[i].i32Radius * 2) + g_sCircle[i].i32Radius;
        g_sCircle[i].i32CenterY = rand() % (RESY - g_sCircle[i].i32Radius * 2) + g_sCircle[i].i32Radius;
//
//        am_util_stdio_printf("g_sCircle[%d].i32Radius = %d, g_sCircle[%d].i32CenterX = %d, g_sCircle[%d].i32CenterY = %d\n",
//                              i,g_sCircle[i].i32Radius, i, g_sCircle[i].i32CenterX, i, g_sCircle[i].i32CenterY);

        g_sCircle[i].red = rand() % 256;
        g_sCircle[i].green = rand() % 256;
        g_sCircle[i].blue = rand() % 256;
        g_sCircle[i].aplha = rand() % 256;

        g_sCircle[i].speed_x = rand() % 5 + 1;
        g_sCircle[i].speed_y = rand() % 5 + 1;
    }

    sCLCircles = nema_cl_create();
    // cl_clear = nema_cl_create();

    while (1)
    {
        ui32PrintTimer++;

        nema_cl_bind(&sCLCircles);
        nema_bind_dst_tex(g_sFBs[i32CurFB].bo.base_phys, g_sFBs[i32CurFB].w, g_sFBs[i32CurFB].h, g_sFBs[i32CurFB].format, g_sFBs[i32CurFB].stride);
        nema_set_clip(0, 0, RESX, RESY);
        nema_clear(nema_rgba(0x00, 0x00, 0x00, 0xff));
        nema_set_blend_fill(NEMA_BL_SIMPLE);

        start_time = nema_get_time();
        update_circle();
        end_time = nema_get_time();
        if (ui32PrintTimer % 100 == 0)
        {
            am_util_stdio_printf("update_circle used %f s\n", end_time - start_time);
        }

        if (i32LastFB >= 0)
        {
            if (ui8FlagDC == 1)
            {
                nemadc_wait_vsync();
                if (g_sDispCfg[g_eDispType].eInterface == IF_DSI)
                {
                    dsi_send_frame_single_end();
                }
                else
                {
                    nemadc_send_frame_single_end();
                }
            }
        }

        start_time = nema_get_time();
        nema_cl_submit(&sCLCircles);
        end_time = nema_get_time();
        if (ui32PrintTimer % 100 == 0)
        {
            am_util_stdio_printf("nema_cl_submit used %f s\n", end_time - start_time);
        }

        if (i32LastFB >= 0)
        {

            nemadc_set_layer(0, &sLayer[i32LastFB]);

            if (g_sDispCfg[g_eDispType].eInterface == IF_DSI)
            {
                dsi_send_frame_single_start(NEMADC_OUTP_OFF);
            }
            else
            {
                nemadc_send_frame_single_start();
            }

            ui8FlagDC = 1;

            nema_calculate_fps();
        }

        start_time = nema_get_time();
        // if (i32LastFB >= 0)
            nema_cl_wait(&sCLCircles);
        end_time = nema_get_time();

        if (ui32PrintTimer % 100 == 0)
        {
            am_util_stdio_printf("nema_cl_wait used %f s\n", end_time - start_time);
        }

        nema_cl_rewind(&sCLCircles);

        swap_fb();

#ifndef BAREMETAL
        taskYIELD();
//        vTaskDelay(1);
#endif
    }
}

int
am_balls_bench()
{
    int i32Ret;
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
    //
    //Set the display region to center
    //
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
    //
    // Initialize the display
    //
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

//    nema_event_init(1, 0, 0, RESX, RESY);

    fb_reload_rgb565_2();
    test_blit_balls();

    return 0;
}

