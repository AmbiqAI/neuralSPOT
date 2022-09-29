//*****************************************************************************
//
//! @file graphics_power_control.c
//!
//! @brief Power ON/OFF GPU,DC,DSI(DPHY).
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
#include "nemagfx_vg_circular_bar.h"

#include "nema_core.h"
#include "nema_utils.h"
#include "nema_event.h"
#include "nema_dc.h"
#include "nema_hal.h"
#include "nema_dc_mipi.h"
#include "nema_regs.h"

#include "am_devices_dsi_rm67162.h"
#include "am_devices_nemadc_rm67162.h"

//*****************************************************************************
//
// Macro definitions
//
//*****************************************************************************


//*****************************************************************************
//
//! @brief this function used to disable D-PHY power and clocks then reduce
//! power consumption.
//!
//! @return 1- D-PHY power had disabled before,will not output entering
//!            ULPS sequence.
//!         0- D-PHY have output entering ULPS sequence(escape mode entry + LPDT command).
//
//*****************************************************************************
int32_t
dphy_power_down(void)
{
    bool status;
    am_hal_pwrctrl_periph_enabled(AM_HAL_PWRCTRL_PERIPH_DISPPHY, &status);
    if ( status )
    {
       am_hal_dsi_napping();
    }
    else
    {
        return 1;
    }
    return 0;
}
//*****************************************************************************
//
//! @brief this function used to enable D-PHY power and clocks and generate
//! exit escape sequence.
//!
//! @param ui8LanesNum      - D-PHY data lanes numbers.
//! @param ui8DBIBusWidth   - display bus interface width.
//! @param ui32FreqTrim     - frequency trim.
//!
//! @return 0.
//
//*****************************************************************************
int32_t
dphy_power_up(uint8_t ui8LanesNum, uint8_t ui8DBIBusWidth, uint32_t ui32FreqTrim)
{
    am_hal_dsi_wakeup(ui8LanesNum, ui8DBIBusWidth, ui32FreqTrim);
    return 0;
}
//*****************************************************************************
//
//! @brief this function used to disable DC power
//!
//! @note after called this function,please don't operation any DC registers
//! until enable and initial the DC.
//!
//! @return 1- DC power had disabled before
//!         0- DC power have disabled now.
//
//*****************************************************************************
int32_t
dc_power_down(void)
{
    bool status;
    am_hal_pwrctrl_periph_enabled(AM_HAL_PWRCTRL_PERIPH_DISP, &status);
    if ( status )
    {
        nemadc_MIPI_CFG_out(0);
        nemadc_set_mode(0);
        am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_DISP);
    }
    else
    {
        return 1;
    }
    return 0;
}
//*****************************************************************************
//
//! @brief this function used to enable DC power and initilize DC
//!
//! @param ui32PixelFormat   graphics pixel format.
//!
//! @return 0- DC power have initialize completely.
//!         other - DC initallize error.
//
//*****************************************************************************
int32_t
dc_power_up(uint32_t ui32PixelFormat, int i32ResX, int i32ResY )
{
    int32_t i32Ret = 1;
    bool status;
    am_hal_pwrctrl_periph_enabled(AM_HAL_PWRCTRL_PERIPH_DISP, &status);
    if ( !status )
    {
        am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_DISP);
        i32Ret = nemadc_init();
        if (i32Ret != 0)
        {
            return i32Ret;
        }

        switch (g_sDispCfg[g_eDispType].eInterface)
        {
            case IF_SPI4:

                nemadc_MIPI_CFG_out(MIPICFG_SPI4 | MIPICFG_SPI_CSX_V | MIPICFG_DBI_EN |
                                    MIPICFG_RESX | MIPICFG_1RGB565_OPT0 | MIPICFG_DIS_TE);
                nemadc_timing(i32ResX, 4, 10, 10,
                              i32ResY, 10, 50, 10);
                break;

            case IF_DSPI:
                nemadc_MIPI_CFG_out(MIPICFG_DSPI | MIPICFG_SPI4 | MIPICFG_SPI_CSX_V |
                                    MIPICFG_DBI_EN | MIPICFG_RESX |
                                    MIPICFG_2RGB565_OPT0 | MIPICFG_DIS_TE);
                nemadc_timing(i32ResX, 4, 10, 10,
                              i32ResY, 10, 50, 10);
                break;

            case IF_QSPI:
                nemadc_MIPI_CFG_out(MIPICFG_QSPI | MIPICFG_SPI4 | MIPICFG_SPI_CSX_V |
                                    MIPICFG_DBI_EN | MIPICFG_RESX |
                                    MIPICFG_4RGB565_OPT0 | MIPICFG_DIS_TE);
                nemadc_timing(i32ResX, 4, 10, 10,
                              i32ResY, 10, 50, 10);
                break;

            case IF_DSI:
            default:
                //
                // Enable dc clock
                //
                nemadc_reg_write(NEMADC_REG_CLKCTRL_CG, NemaDC_clkctrl_cg_clk_en);
                //
                // Set clock divider
                //
                if (APOLLO4_GE_B2)
                {
                    nemadc_clkdiv(1, 1, 4, 0);
                }
                else
                {
                    nemadc_clkdiv(2, 1, 4, 0);
                }
                //
                // Enable fast pixel generation slow transfer
                //
                if (APOLLO4_GE_B2)
                {
                    nemadc_reg_write(NEMADC_REG_CLKCTRL_CG,
                                    (NemaDC_clkctrl_cg_clk_swap |
                                     NemaDC_clkctrl_cg_l0_bus_clk |
                                     NemaDC_clkctrl_cg_clk_en));
                }
                else
                {
                    nemadc_reg_write(NEMADC_REG_CLKCTRL_CG,
                                    (NemaDC_clkctrl_cg_clk_swap |
                                     NemaDC_clkctrl_cg_clk_en));
                }
                nemadc_clkctrl((uint32_t)TB_LCDPANEL_MIPI_DBIB );
                //
                // Program NemaDC MIPI interface
                //
                if (APOLLO4_GE_B2)
                {
                    nemadc_MIPI_CFG_out(MIPICFG_DBI_EN          |
                                        MIPICFG_RESX            |
                                        MIPICFG_EXT_CTRL        |
                                        MIPICFG_EN_STALL        |
    #ifndef ENABLE_TE
                                        MIPICFG_DIS_TE          | // comment out this line when TE is enabled
    #endif
                                        MIPICFG_PIXCLK_OUT_EN   |
                                        ui32PixelFormat
                    );
                }
                else
                {
                    nemadc_MIPI_CFG_out(MIPICFG_DBI_EN          |
                                        MIPICFG_RESX            |
                                        MIPICFG_EXT_CTRL        |
    #ifndef ENABLE_TE
                                        MIPICFG_DIS_TE          | // comment out this line when TE is enabled
    #endif
                                        MIPICFG_PIXCLK_OUT_EN   |
                                        ui32PixelFormat
                    );
                }
                //
                // Program NemaDC to transfer a i32ResX*i32ResY region
                //
                nemadc_timing(i32ResX, 4, 10, 1,
                              i32ResY, 1, 1, 1);
                break;
        }
    }

    return i32Ret;
}

//*****************************************************************************
//
//! @brief this function used to disable GPU power
//!
//! @note after called this function,please don't operation any GPU registers
//! until enable and initial the GPU.
//!
//! @return 1- GPU power had disabled before
//!         0- GPU power have disabled now.
//
//*****************************************************************************
int32_t
gpu_power_down(void)
{
    bool status;
    am_hal_pwrctrl_periph_enabled(AM_HAL_PWRCTRL_PERIPH_GFX, &status);
    if ( status )
    {
        //
        // Disable clock
        //
        nema_reg_write(NEMA_CGCTRL, 0);
        //
        // Reset GPU status
        //
        nema_reg_write(NEMA_STATUS, 0xFFFFFFFF);
        am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_GFX);
    }
    else
    {
        return 1;
    }
    return 0;
}
//*****************************************************************************
//
//! @brief this function used to enable GPU power and initialize nemaGFX
//!
//! @return 0- GPU power have initialize completely.
//!         other- GPU power initialize error.
//
//*****************************************************************************
int32_t
gpu_power_up(void)
{
    int32_t i32Ret = 1;
    bool status;
    am_hal_pwrctrl_periph_enabled(AM_HAL_PWRCTRL_PERIPH_GFX, &status);
    if ( !status )
    {
        am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_GFX);
        //
        // Initialize NemaGFX
        //
        i32Ret = nema_init();
    }
    return i32Ret;
}

int32_t
display_power_down(void)
{
    if (g_sDispCfg[g_eDispType].eInterface == IF_DSI)
    {
        dphy_power_down();
    }
    else
    {
        if (g_sDispCfg[g_eDispType].bUseDPHYPLL == true)
        {
            am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_DPHYPLL, NULL);
            am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_OFF, NULL);
            am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_PLLCLKSEL_OFF, NULL);
            am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_PLLCLK_DISABLE, NULL);
        }
        else
        {
            am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_OFF, NULL);
            am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_DCCLK_DISABLE, NULL);
        }
    }
    dc_power_down();
    return 0;
}

int32_t
display_power_up(uint32_t ui32PixelFormat, int i32ResX, int i32ResY )
{
    dc_power_up(ui32PixelFormat, i32ResX, i32ResY);
    if ((g_sDispCfg[g_eDispType].eInterface == IF_DSI) || (g_sDispCfg[g_eDispType].bUseDPHYPLL == true))
    {
        dphy_power_up(g_sDsiCfg.ui8NumLanes, g_sDsiCfg.eDbiWidth, g_sDsiCfg.eDsiFreq);
    }
    else
    {
        am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_HFRC96, NULL);
        am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_DCCLK_ENABLE, NULL);
    }
    return 0;
}

