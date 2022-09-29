//*****************************************************************************
//
//  main.c
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
#include "am_util.h"

#include "nema_core.h"
#include "nema_event.h"
#include "nema_utils.h"

#include "nema_dc_hal.h"
#include "nema_dc.h"

#include "ng_globals.h"
#include "ng_display.h"
#include "ng_event.h"
#include "ng_callbacks.h"
#include "ng_gestures.h"
#include "ng_timer.h"
#include "ng_screen_trans.h"

#include "images.h"
#include "fonts.h"
#include "framebuffers.h"
#include "screen_groups.h"

#include "am_devices_dsi_rm67162.h"
#include "am_devices_nemadc_rm67162.h"

#if (defined(SCENARIO_A) || defined(SCENARIO_B))
    #define RESX        390
    #define RESY        390
#elif defined(SCENARIO_C)
    #define RESX        456
    #define RESY        456
#endif

static int ng_init()
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
        minx = 0;   // set the minimum value to 0
    }
    else
    {
        minx = (panel_resx - RESX) >> 1;
        minx = (minx >> 1) << 1;
    }

    if ( RESY > panel_resy )
    {
        miny = 0;   // set the minimum value to 0
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

    load_framebuffers();
    load_image_assets();
    load_font_assets();
    init_screen_groups();
    event_list_init();
    nema_event_init(1, 0, 0, NG_RESX, NG_RESY);
    ng_event_init();

    //Check if globals (project generated) have been forwarded to the API
    ret = ng_globals_sanity_check();
    if (ret)
    {
        return ret;
    }

    //Format        | Pixclock | RESX | FP | SYNC | BP | RESY | FP | SYNC | BP
    //800x600, 60Hz | 40.000   | 800  | 40 | 128  | 88 | 600  | 1  | 4    | 23
    // nemadc_timing(800, 40, 128, 88, 600, 1, 4, 23);
    // nemadc_set_layer(0, &NG_DC_LAYER[0]);

    //Initialize display module
    ng_display_init();

    //Trigger events
    for (int i = 0; i < NG_EVENT_LIST_SIZE; ++i)
    {
        ng_event_base_t *event = NG_EVENT_LIST[i];

        if ( event->trigger == EV_TRIGGER_APP_START )
        {
            event->start(event);
        }
    }
    return 0;
}

int guage()
{
#if defined(BAREMETAL) && defined(APOLLO4_FPGA) && defined(AM_PART_APOLLO4P)
    am_hal_stimer_config(AM_HAL_STIMER_HFRC_6MHZ | AM_HAL_STIMER_CFG_RUN);
#endif
    // Init NemaGFX, NemaDC, GUI environment (GItems, assets, events)
    int ret = ng_init();
    if (ret)
    {
        return ret;
    }

    //Enter application's main loop
    ng_main_loop(0);

    nema_cl_destroy(&cl);
    nema_cl_destroy(&cl_screen);

    return 0;
}

