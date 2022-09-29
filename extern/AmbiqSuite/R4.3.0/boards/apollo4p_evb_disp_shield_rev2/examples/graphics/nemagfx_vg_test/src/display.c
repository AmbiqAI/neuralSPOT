//*****************************************************************************
//
//! @file display.c
//!
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

#include "nemagfx_vg_test.h"

//*****************************************************************************
//
// Set panel display region
//
//*****************************************************************************
void display_refresh_start(void)
{
    switch (g_sDispCfg[g_eDispType].eInterface)
    {
        case IF_SPI4:
        case IF_DSPI:
        case IF_QSPI:
            nemadc_send_frame_single_start();
            break;
        case IF_DSI:
            dsi_send_frame_single_start(NEMADC_OUTP_OFF);
            break;
        default:
            break; //NOP
    }
}

void display_refresh_end(void)
{
    switch (g_sDispCfg[g_eDispType].eInterface)
    {
        case IF_SPI4:
        case IF_DSPI:
        case IF_QSPI:
            nemadc_send_frame_single_end();
            break;
        case IF_DSI:
            dsi_send_frame_single_end();
            break;
        default:
            break; //NOP
    }
}

//*****************************************************************************
//
// Set up DC, display interface and display panel.
//
//*****************************************************************************
int
display_setup(uint32_t fb_w, uint32_t fb_h)
{
    int ret;
    uint32_t ui32MipiCfg;
    uint16_t ui16PanelResX = g_sDispCfg[g_eDispType].ui32PanelResX; //panel's max resolution
    uint16_t ui16PanelResY = g_sDispCfg[g_eDispType].ui32PanelResY; //panel's max resolution
    uint16_t minx, miny;

    //
    // If you want to change display configurations in example, please change them here as below.
    //
    // g_sDsiCfg.ui8NumLanes = 1;
    // g_sDsiCfg.eDbiWidth = AM_HAL_DSI_DBI_WIDTH_8;
    // g_sDsiCfg.eDsiFreq = AM_HAL_DSI_FREQ_TRIM_X12;

    if (g_sDispCfg[g_eDispType].eInterface == IF_DSI)
    {
        //
        // VDD18 control callback function
        //
        am_hal_dsi_register_external_vdd18_callback(am_bsp_external_vdd18_switch);

        //
        // Enable DSI power and configure DSI clock.
        //
        am_hal_dsi_init();
    }
    else
    {
        if (g_sDispCfg[g_eDispType].bUseDPHYPLL == true)
        {
            am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_DPHYPLL, NULL);
            am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_DCCLK_ENABLE, NULL);
            am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_PLLCLKSEL_HFRC12, NULL);
            am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_PLLCLK_ENABLE, NULL);
        }
        else
        {
            am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_HFRC96, NULL);
            am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_DCCLK_ENABLE, NULL);
        }
    }
    am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_DISP);
    if (g_sDispCfg[g_eDispType].bUseDPHYPLL == true)
    {
        am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_DISPPHY);
    }

    //Initialize Nema|dc
    ret = nemadc_init();
    if (ret != 0)
    {
        am_util_stdio_printf("DC init failed!\n");
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

    //Set the display region to center
    if ( fb_w > ui16PanelResX )
    {
        minx = 0; // set the minimum value to 0
    }
    else
    {
        minx = (ui16PanelResX - fb_w) >> 1;
        minx = (minx >> 1) << 1;
    }

    if ( fb_h > ui16PanelResY )
    {
        miny = 0; // set the minimum value to 0
    }
    else
    {
        miny = (ui16PanelResY - fb_h) >> 1;
        miny = (miny >> 1) << 1;
    }

    //Initialize the display
    switch (g_sDispCfg[g_eDispType].eInterface)
    {
        case IF_SPI4:
            am_devices_nemadc_rm67162_init(MIPICFG_SPI4, MIPICFG_1RGB565_OPT0, fb_w, fb_h, minx, miny);
            break;
        case IF_DSPI:
            am_devices_nemadc_rm67162_init(MIPICFG_DSPI | MIPICFG_SPI4, MIPICFG_2RGB888_OPT0, fb_w, fb_h, minx, miny);
            break;
        case IF_QSPI:
            am_devices_nemadc_rm67162_init(MIPICFG_QSPI | MIPICFG_SPI4, MIPICFG_4RGB565_OPT0, fb_w, fb_h, minx, miny);
            break;
        case IF_DSI:
            am_devices_dsi_rm67162_init(ui32MipiCfg, fb_w, fb_h, minx, miny);
            break;
        default:
            ; //NOP
    }
    return 0;
}
