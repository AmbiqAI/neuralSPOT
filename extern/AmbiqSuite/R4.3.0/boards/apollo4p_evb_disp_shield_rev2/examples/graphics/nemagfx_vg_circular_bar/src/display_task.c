//*****************************************************************************
//
//! @file display_task.c
//!
//! @brief Task to handle DISPLAY operations.
//!
//! AM_DEBUG_PRINTF
//! If enabled, debug messages will be sent over ITM.
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

//*****************************************************************************
//
// Global includes for this project.
//
//*****************************************************************************
#include "nemagfx_vg_circular_bar.h"

#include "nema_dc.h"
#include "nema_core.h"
#include "nema_math.h"
#include "nema_utils.h"
#include "nema_event.h"
#include "nema_dc_mipi.h"

#include "am_devices_dsi_rm67162.h"
#include "am_devices_nemadc_rm67162.h"

#include "graphics_power_control.h"
//*****************************************************************************
//
// Macro definitions
//
//*****************************************************************************
#define DC_PIXEL_FORMAT FMT_RGB888

//Offset of the panel
#define PANEL_OFFSET_X (((g_sDispCfg[g_eDispType].ui32PanelResX/4*4)-FB_RESX)/2)
#define PANEL_OFFSET_Y (((g_sDispCfg[g_eDispType].ui32PanelResY/4*4)-FB_RESY)/2)

//*****************************************************************************
//
// DISPLAY task handle.
//
//*****************************************************************************
TaskHandle_t DisplayTaskHandle;
uint32_t ui32MipiCfg = MIPICFG_8RGB888_OPT0; // default config

//layer
nemadc_layer_t g_sLayerOne =
{
    .startx        = 0,
    .sizex         = FB_RESX,
    .resx          = FB_RESX,
    .starty        = 0,
    .sizey         = FB_RESY,
    .resy          = FB_RESY,
    .stride        = -1,
    .format        = NEMADC_RGB565,
    .blendmode     = NEMADC_BL_SRC,
    .buscfg        = 0,
    .alpha         = 0xff,
    .flipx_en      = 0,
    .flipy_en      = 0,
};

//*****************************************************************************
//
// Global Variables
//
//*****************************************************************************

//*****************************************************************************
//
// Task function.
//
//*****************************************************************************
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
display_setup(void)
{
    int ret;
    uint16_t ui16PanelResX = g_sDispCfg[g_eDispType].ui32PanelResX; //panel's max resolution
    uint16_t ui16PanelResY = g_sDispCfg[g_eDispType].ui32PanelResY; //panel's max resolution
    uint16_t minx, miny;

    //
    // If you want to change display configurations in example, please change them here as below.
    //
#ifdef APOLLO4_FPGA
     g_eDispType = RM67162_SPI4;
#endif
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
        pixel_format_t eFormat = DC_PIXEL_FORMAT;
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
    if ( FB_RESX > ui16PanelResX )
    {
        minx = 0; // set the minimum value to 0
    }
    else
    {
        minx = (ui16PanelResX - FB_RESX) >> 1;
        minx = (minx >> 1) << 1;
    }

    if ( FB_RESY > ui16PanelResY )
    {
        miny = 0; // set the minimum value to 0
    }
    else
    {
        miny = (ui16PanelResY - FB_RESY) >> 1;
        miny = (miny >> 1) << 1;
    }

    //Initialize the display
    switch (g_sDispCfg[g_eDispType].eInterface)
    {
        case IF_SPI4:
            am_devices_nemadc_rm67162_init(MIPICFG_SPI4, MIPICFG_1RGB565_OPT0, FB_RESX, FB_RESY, minx, miny);
            break;
        case IF_DSPI:
            am_devices_nemadc_rm67162_init(MIPICFG_DSPI | MIPICFG_SPI4, MIPICFG_2RGB888_OPT0, FB_RESX, FB_RESY, minx, miny);
            break;
        case IF_QSPI:
            am_devices_nemadc_rm67162_init(MIPICFG_QSPI | MIPICFG_SPI4, MIPICFG_4RGB565_OPT0, FB_RESX, FB_RESY, minx, miny);
            break;
        case IF_DSI:
            am_devices_dsi_rm67162_init(ui32MipiCfg, FB_RESX, FB_RESY, minx, miny);
            break;
        default:
            ; //NOP
    }
    return 0;
}

//*****************************************************************************
//
// Short Description.
//
//*****************************************************************************

void
DisplayTask(void *pvParameters)
{
    int ret;

    am_util_stdio_printf("Display task start!\n");

    //Init display system, including DC, DSI(or SPI), panel
    ret = display_setup();
    if (ret != 0)
    {
        am_util_stdio_printf("display init failed!\n");

        //suspend and delete this task.
        vTaskDelete(NULL);
    }

    while(1)
    {
        //Wait start.
        xSemaphoreTake( g_semDCStart, portMAX_DELAY);

#ifdef USE_DEBUG_PIN_IN_GUI_TASK
        am_hal_gpio_output_clear(DEBUG_PIN_4);
        am_hal_gpio_output_clear(DEBUG_PIN_5);
#endif

        //This API will directly return if display system is still power on.
        display_power_up(ui32MipiCfg, FB_RESX, FB_RESY);

        //Set the data location to be send by DC.
        g_sLayerOne.baseaddr_phys = g_pFrameBufferDC->bo.base_phys;
        g_sLayerOne.baseaddr_virt = g_pFrameBufferDC->bo.base_virt;
        nemadc_set_layer(0, &g_sLayerOne);

        //Start DC
        display_refresh_start();

#ifdef USE_DEBUG_PIN_IN_GUI_TASK
        am_hal_gpio_output_set(DEBUG_PIN_2);
#endif

        //Wait DC complete interrupt.
        nemadc_wait_vsync();
        //Do follow-up operations required by hardware.
        display_refresh_end();

#ifdef USE_DEBUG_PIN_IN_GUI_TASK
        am_hal_gpio_output_clear(DEBUG_PIN_2);
#endif

        xSemaphoreGive(g_semDCEnd);

        //Power Down
        display_power_down();

#ifdef USE_DEBUG_PIN_IN_GUI_TASK
        am_hal_gpio_output_set(DEBUG_PIN_4);
#endif

    }

}
