//*****************************************************************************
//
//! @file dsi_rw_command.c
//!
//! @brief DSI example.
//!
//! This example demonstrates how to read and write RM67162 display registers with DSI.
//!
//! 1-lane DSI includes 4 signals,
//!   * Differential clock lane - positive (CLKP)
//!   * Differential clock lane - negative (CLKN)
//!   * Differential data lane 0 - positive (D0P)
//!   * Differential data lane 0 - negative (D0N).
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

#include "am_bsp.h"
#include "nema_hal.h"
#include "nema_dc.h"
#include "nema_dc_mipi.h"
#include "am_util_delay.h"
#include "am_util_stdio.h"
#include "nema_dc_regs.h"
#include "string.h"
#include "am_devices_dsi_rm67162.h"

//*****************************************************************************
//
// Macro definitions
//
//*****************************************************************************
#define FB_RESX 200
#define FB_RESY 200

//
// Update these macros when Rev B silicon is ready
//
#define NUM_OF_WIDTH_OPTIONS 1
#define NUM_OF_TRIM_OPTIONS  6
#define NUM_OF_CMD_TYPES     2
#define LENGTH_SpeedMode     2
#define LENGTH_CmdType       3
#define LENGTH_Result        6

//*****************************************************************************
//
//! @brief Print test results
//! @param uint8_t ui8DbiWidth, uint32_t ui32FreqTrim, cmd_type_enum eCmdType, bool bHS, bool bItemPassype
//! @return void.
//
//*****************************************************************************
void
result_print(uint8_t ui8DbiWidth, uint32_t ui32FreqTrim, bool bHS, \
             am_hal_dsi_cmd_type_e eCmdType, bool bWr, uint8_t ui8NumPara, uint8_t ui8NumData, bool bItemPass)
{
    char * pcStr1 = "HS";
    char * pcStr2 = "LP";
    char * pcStr3 = "DCS";
    char * pcStr4 = "Generic";
    char * pcStr5 = "long";
    char * pcStr6 = "short";
    char * pcStr7 = "passed";
    char * pcStr8 = "failed";
    char * psSpeedMode;
    char * psCmdType;
    char * psLength;
    char * psResult;
    if (bHS == true)
    {
        //! HS
        psSpeedMode = pcStr1;
    }
    else
    {
        //! LP
        psSpeedMode = pcStr2;
    }
    if (eCmdType == DCS_CMD)
    {
        psCmdType = pcStr3;
    }
    else
    {
        psCmdType = pcStr4;
    }
    if (bWr == true)
    {
        if (((eCmdType == DCS_CMD) && (ui8NumPara > 1))
            || ((eCmdType == GE_CMD) && (ui8NumPara > 2)))
        {
            psLength = pcStr5;
        }
        else
        {
            psLength = pcStr6;
        }
    }
    if (bItemPass == true)
    {
        psResult = pcStr7;
    }
    else
    {
        psResult = pcStr8;
    }
    if (bWr == true)
    {
        am_util_stdio_printf("%d-bit DBI width, Frequence trim value - 0x%02X, %s mode, %s %s write command, %d parameter(s), test is %s.\n", \
                              ui8DbiWidth, ui32FreqTrim, psSpeedMode, psCmdType, psLength, ui8NumPara, psResult);
    }
    else
    {
        am_util_stdio_printf("%d-bit DBI width, Frequence trim value - 0x%02X, %s mode, %s read command, %d parameter(s), %d byte(s) of returned data, test is %s.\n", \
                              ui8DbiWidth, ui32FreqTrim, psSpeedMode, psCmdType, ui8NumPara, ui8NumData, psResult);
    }
}

//*****************************************************************************
//
//! @brief Test DCS/Generic read/write commands
//! @param am_hal_dsi_cmd_type_e eCmdType, bool bHS
//! @return bPass.
//
//*****************************************************************************
bool
test_dsi_cmd(am_hal_dsi_cmd_type_e eCmdType, bool bHS)
{
    bool bPass = true;
    bool bItemPass, bWr;
    uint8_t ui8LanesNum = g_sDsiCfg.ui8NumLanes;
    uint8_t ui8DbiWidth = g_sDsiCfg.eDbiWidth;
    uint32_t ui32FreqTrim = g_sDsiCfg.eDsiFreq;
    uint8_t ui8CmdBuf[4];
    uint8_t ui8NumWrPara, ui8NumRdPara, ui8NumRdData;
    uint32_t ui32DsiRdVal = 0;
    //
    // Initialize DSI
    //
    if (am_hal_dsi_para_config(ui8LanesNum, ui8DbiWidth, ui32FreqTrim) != 0)
    {
        bPass = false;
        return bPass;
    }

    am_bsp_disp_dsi_pins_enable();

    //
    // Enable dc clock
    //
    nemadc_reg_write(NEMADC_REG_CLKCTRL_CG, NemaDC_clkctrl_cg_clk_en);
    //
    // Set clock divider. B2 and later versions of Apollo4 support setting DC primary clock divide ratio to 1.
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
    nemadc_clkctrl((uint32_t)TB_LCDPANEL_MIPI_DBIB);
    if (APOLLO4_GE_B2)
    {
        nemadc_MIPI_CFG_out(MIPICFG_DBI_EN          |
                            MIPICFG_RESX            |
                            MIPICFG_EXT_CTRL        |
                            MIPICFG_DIS_TE          |
                            MIPICFG_EN_STALL        |
                            MIPICFG_PIXCLK_OUT_EN);
    }
    else
    {
        nemadc_MIPI_CFG_out(MIPICFG_DBI_EN          |
                            MIPICFG_RESX            |
                            MIPICFG_EXT_CTRL        |
                            MIPICFG_DIS_TE          |
                            MIPICFG_PIXCLK_OUT_EN);

    }
    //
    // hardware reset
    //
    send_reset_signal();

    if (eCmdType == DCS_CMD)
    {
        //! DCS read with no parameter
        ui8NumRdPara = 0;
        //
        // DCS read with no parameter and 1 byte of returned data.
        //
        ui8NumRdData = 1;
        ui32DsiRdVal = dsi_dcs_read(MIPI_get_pixel_format, ui8NumRdData, bHS);
        if (ui32DsiRdVal == 0x77)
        {
            bItemPass = true;
        }
        else
        {
            bItemPass = false;
            bPass = false;
        }
        bWr = false;
        result_print(ui8DbiWidth, ui32FreqTrim, bHS, eCmdType, bWr, ui8NumRdPara, ui8NumRdData, bItemPass);
        //
        // DCS short write with no parameter, DCS read with no parameter and 1 byte of returned data.
        //
        am_util_delay_ms(10);
        ui8NumWrPara = 0;
        dsi_dcs_write(MIPI_set_display_on, NULL, ui8NumWrPara, bHS);
        ui8NumRdData = 1;
        ui32DsiRdVal = dsi_dcs_read(MIPI_get_power_mode, ui8NumRdData, bHS);
        //
        // check display on register
        //
        if ((ui32DsiRdVal & 0x00000004 ) != 0)
        {
            bItemPass = true;
        }
        else
        {
            bItemPass = false;
            bPass = false;
        }
        bWr = true;
        result_print(ui8DbiWidth, ui32FreqTrim, bHS, eCmdType, bWr, ui8NumWrPara, 0, bItemPass);
        //
        // DCS short write with 1 parameter, DCS read with no parameter and 1 byte of returned data.
        //
        am_util_delay_ms(10);
        ui8CmdBuf[0] = 0x05;
        ui8NumWrPara = 1;
        dsi_dcs_write(MIPI_set_pixel_format, ui8CmdBuf, ui8NumWrPara, bHS);
        ui8NumRdData = 1;
        ui32DsiRdVal = dsi_dcs_read(MIPI_get_pixel_format, ui8NumRdData, bHS);
        if (ui32DsiRdVal == ui8CmdBuf[0])
        {
            bItemPass = true;
        }
        else
        {
            bItemPass = false;
            bPass = false;
        }
        bWr = true;
        result_print(ui8DbiWidth, ui32FreqTrim, bHS, eCmdType, bWr, ui8NumWrPara, 0, bItemPass);
        bWr = false;
        result_print(ui8DbiWidth, ui32FreqTrim, bHS, eCmdType, bWr, ui8NumRdPara, ui8NumRdData, bItemPass);
        //
        // DCS long write with 2 parameters, DCS read with no parameter and 2 bytes of returned data.
        //
        DSI->MAXRETPACSZE_b.COUNTVAL = 2;
        DSI->MAXRETPACSZE_b.HSLP = 1;
        am_util_delay_ms(10);
        ui8CmdBuf[0] = 0x08;
        ui8CmdBuf[1] = 0x0A;
        ui8NumWrPara = 2;
        dsi_dcs_write(MIPI_set_tear_scanline, ui8CmdBuf, ui8NumWrPara, bHS);
        ui8NumRdData = 2;
        ui32DsiRdVal = dsi_dcs_read(MIPI_get_scanline, ui8NumRdData, bHS);
        if ((((ui32DsiRdVal & 0x0000FF00) >> 8) == ui8CmdBuf[0]) && ((ui32DsiRdVal & 0x000000FF) == ui8CmdBuf[1]))
        {
            bItemPass = true;
        }
        else
        {
            bItemPass = false;
            bPass = false;
        }
        bWr = true;
        result_print(ui8DbiWidth, ui32FreqTrim, bHS, eCmdType, bWr, ui8NumWrPara, 0, bItemPass);
        bWr = false;
        result_print(ui8DbiWidth, ui32FreqTrim, bHS, eCmdType, bWr, ui8NumRdPara, ui8NumRdData, bItemPass);
    }
    else
    {
        //
        // Generic read with 1 parameter and 1 byte of returned data.
        //
        ui8CmdBuf[0] = 0x54;
        ui8NumRdPara = 1;
        ui8NumRdData = 1;
        //
        // Read default value
        //
        ui32DsiRdVal = dsi_generic_read(ui8CmdBuf, ui8NumRdPara, ui8NumRdData, bHS);
        if (ui32DsiRdVal == 0x28)
        {
            bItemPass = true;
        }
        else
        {
            bItemPass = false;
            bPass = false;
        }
        bWr = false;
        result_print(ui8DbiWidth, ui32FreqTrim, bHS, eCmdType, bWr, ui8NumRdPara, ui8NumRdData, bItemPass);
        //
        // Generic short write with 1 paremeter, generic read with 1 parameter and 1 byte of returned data.
        //
        am_util_delay_ms(10);
        ui8CmdBuf[0] = MIPI_set_display_on;
        ui8NumWrPara = 1;
        dsi_generic_write(ui8CmdBuf, ui8NumWrPara, bHS);
        //
        // get display's power mode
        //
        ui8CmdBuf[0] = MIPI_get_power_mode;
        ui8NumRdPara = 1;
        ui8NumRdData = 1;
        ui32DsiRdVal = dsi_generic_read(ui8CmdBuf, ui8NumRdPara, ui8NumRdPara, bHS);
        //
        // check display on register
        //
        if ((ui32DsiRdVal & 0x00000004) != 0)
        {
            bItemPass = true;
        }
        else
        {
            bItemPass = false;
            bPass = false;
        }
        bWr = true;
        result_print(ui8DbiWidth, ui32FreqTrim, bHS, eCmdType, bWr, ui8NumWrPara, 0, bItemPass);
        //
        // Generic short write with 2 paremeters, generic read with 1 parameter and 1 byte of returned data.
        //
        am_util_delay_ms(10);
        ui8CmdBuf[0] = 0x58;
        ui8CmdBuf[1] = 0x06;
        ui8NumWrPara = 2;
        dsi_generic_write(ui8CmdBuf, ui8NumWrPara, bHS);
        ui8CmdBuf[0] = 0x59;
        ui8NumRdPara = 1;
        ui8NumRdData = 1;
        ui32DsiRdVal = dsi_generic_read(ui8CmdBuf, ui8NumRdPara, ui8NumRdPara, bHS);
        if (ui32DsiRdVal == ui8CmdBuf[1])
        {
            bItemPass = true;
        }
        else
        {
            bItemPass = false;
            bPass = false;
        }
        bWr = true;
        result_print(ui8DbiWidth, ui32FreqTrim, bHS, eCmdType, bWr, ui8NumWrPara, 0, bItemPass);
        //
        // Generic long write with 3 parameters, generic read with no parameter and 2 bytes of returned data.
        //
        am_util_delay_ms(10);
        DSI->MAXRETPACSZE_b.COUNTVAL = 2;
        DSI->MAXRETPACSZE_b.HSLP = 1;
        //
        //
        ui8CmdBuf[0] = MIPI_set_tear_scanline;
        ui8CmdBuf[1] = 0x08;
        ui8CmdBuf[2] = 0x0A;
        ui8NumWrPara = 3;
        dsi_generic_write(ui8CmdBuf, ui8NumWrPara, bHS);
        //
        //
        //
        ui8CmdBuf[0] = MIPI_get_scanline;
        ui8NumRdPara = 1;
        ui8NumRdData = 2;
        ui32DsiRdVal = dsi_generic_read(ui8CmdBuf, ui8NumRdPara, ui8NumRdData, bHS);
        if ((((ui32DsiRdVal & 0x0000FF00 ) >> 8) == ui8CmdBuf[1]) /*&& ((ui32DsiRdVal & 0x000000FF ) == ui8CmdBuf[2])*/)
        {
            bItemPass = true;
        }
        else
        {
            bItemPass = false;
            bPass = false;
        }
        bWr = true;
        result_print(ui8DbiWidth, ui32FreqTrim, bHS, eCmdType, bWr, ui8NumWrPara, 0, bItemPass);
        bWr = false;
        result_print(ui8DbiWidth, ui32FreqTrim, bHS, eCmdType, bWr, ui8NumRdPara, ui8NumRdData, bItemPass);
//
// Do not test these 2 cases right now.
//
#if 0
        //
        // Generic short write with 0 paremeters, generic read with 0 parameter.
        //
        ui8NumWrPara = 0;
        dsi_generic_write(ui8CmdBuf, ui8NumWrPara, bHS);
        ui8NumRdPara = 0;
        ui8NumRdData = 1;
        ui32DsiRdVal = dsi_generic_read(NULL, ui8NumRdPara, ui8NumRdPara, bHS);
        //
        // Generic read with 2 parameters is not supported by Rev A and Rev B0.
        //
        DSI->MAXRETPACSZE_b.COUNTVAL = 1;
        DSI->MAXRETPACSZE_b.HSLP = 1;
        ui8CmdBuf[0] = MIPI_get_scanline;
        //! 1st byte
        ui8CmdBuf[1] = 0x0;
        ui8NumRdPara = 2;
        ui8NumRdData = 1;
        ui32DsiRdVal = dsi_generic_read(ui8CmdBuf, ui8NumRdPara, ui8NumRdPara, bHS);
#endif
    }
    return bPass;
}

//*****************************************************************************
//
//! @brief Test DSI interface.
//!
//! @return bTestPass.
//
//*****************************************************************************
bool
am_dsi_rw_command(void)
{
    bool bTestPass = true;
    bool bRet = true;
    //
    // Assign a fixed value to display type.
    //
    g_eDispType = RM67162_DSI;

    //
    // Initialize NemaDC
    //
    if (nemadc_init() != 0)
    {
        am_util_stdio_printf("DC init failed!\n");
    }

    bRet = test_dsi_cmd(DCS_CMD, HS_MODE);
    if (bRet == false)
    {
        bTestPass = false;
    }
    am_util_delay_ms(50);
    bRet = test_dsi_cmd(DCS_CMD, LP_MODE);
    if (bRet == false)
    {
        bTestPass = false;
    }
    am_util_delay_ms(50);
    bRet = test_dsi_cmd(GE_CMD, HS_MODE);
    if (bRet == false)
    {
        bTestPass = false;
    }
    am_util_delay_ms(50);
    bRet = test_dsi_cmd(GE_CMD, LP_MODE);
    if (bRet == false)
    {
        bTestPass = false;
    }
    am_util_delay_ms(50);
    if (bTestPass == true)
    {
        am_util_stdio_printf("\nAll tests passed!\n");
    }
    else
    {
        am_util_stdio_printf("\nAt least 1 test failed!\n");
    }

    return bTestPass;

} // am_dsi_rw_command()

