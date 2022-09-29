//*****************************************************************************
//
//! @file am_devices_dsi_rm67162.c
//!
//! @brief Generic Raydium OLED display driver.
//!
//! @addtogroup dsi_rm67162 RM67162 DSI Display Device Driver
//! @ingroup devices
//! @{
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

#include "am_devices_dsi_rm67162.h"
#include "am_util_delay.h"
#include "am_bsp.h"
#include "am_util.h"
#ifndef SIMULATION
#define DELAY am_util_delay_ms
#else
#define DELAY(...)
#endif

//*****************************************************************************
//
// @brief hardware reset rm67162.
//
// This function send a RESX signal to the display
//
//*****************************************************************************
void
send_reset_signal(void)
{
    //
    // send display a reset
    //
    am_hal_gpio_pinconfig(AM_BSP_GPIO_DISP_DSI_RES,  am_hal_gpio_pincfg_output);
    am_hal_gpio_state_write(AM_BSP_GPIO_DISP_DSI_RES, AM_HAL_GPIO_OUTPUT_SET);
    DELAY(5);
    am_hal_gpio_state_write(AM_BSP_GPIO_DISP_DSI_RES, AM_HAL_GPIO_OUTPUT_CLEAR);
    DELAY(20);
    am_hal_gpio_state_write(AM_BSP_GPIO_DISP_DSI_RES, AM_HAL_GPIO_OUTPUT_SET);
    DELAY(150);
}

//*****************************************************************************
//
//! @brief wait NemaDC to become idle
//!
//! Wait NemaDC to become idle.
//!
//! @return Status.
//
//*****************************************************************************
void
wait_NemaDC_dbi_idle(void)
{
    uint32_t ui32usMaxDelay = 100000;

    //
    // wait NemaDC to become idle
    //
    am_hal_delay_us_status_change(ui32usMaxDelay, (uint32_t)&DC->STATUS, DC_STATUS_dbi_busy, 0);
#if 0
#ifdef BAREMETAL
    am_hal_delay_us_status_change(ui32usMaxDelay, (uint32_t)&DC->STATUS, DC_STATUS_dbi_busy, 0);
#else
    uint32_t ui32_cur_ms = 0;

    while ( ++ui32_cur_ms < ui32usMaxDelay / 1000 )
    {
        if (am_hal_delay_us_status_change(ui32usMaxDelay, (uint32_t)&DC->STATUS, DC_STATUS_dbi_busy, 0) == AM_HAL_STATUS_SUCCESS)
        {
            break;
        }
    }
#endif
#endif
}

//*****************************************************************************
//
//! @brief DC DBI interface to DSI
//!
//! This function configures data type for DBI to DSI
//!
//! @return None.
//
//*****************************************************************************
//static void
//dc_dbi_to_dsi_ct(uint32_t ui32DataType, uint32_t ui32CmdType, unsigned int ui32Type)
//{
//    //
//    // Data CT offset
//    //
//    const uint32_t ui32Offset = (8U);
//
//    //
//    // Data/Command CT values
//    //
//    uint32_t ui32CtVal = (ui32Type |
//                         ui32DataType |
//                         (ui32CmdType << ui32Offset));
//
//    //
//    // Enable GE/CT signals
//    //
//    nemadc_reg_write(NEMADC_REG_FORMAT_CTRL, ui32CtVal);
//}

//*****************************************************************************
//
//! @brief send DSI DCS command.
//!
//! @param ui8Cmd
//! @param pui8Data
//! @param ui8Len
//! @param bHS
//!
//! This function sends DSI commands to rm67162.
//
//****************************************************************************
void
dsi_dcs_write(uint8_t ui8Cmd, uint8_t* pui8Data, uint8_t ui8Len, bool bHS)
{
    uint8_t ui8Index;
    uint32_t ui32Mode, ui32Gpio;
    uint32_t ui32Val6 = 0;
    ui32Gpio = nemadc_reg_read(NEMADC_REG_GPIO);
    //
    // B2 supports setting DC primary clock divide ratio to 1 when sending frame,
    // but it only supports setting DC primary clock divide ratio to 2 when sending commands.
    //
    if (APOLLO4_B2)
    {
        nemadc_clkdiv(2, 1, 4, 0);
    }

    if (bHS == true) // high speed mode
    {
        nemadc_reg_write(NEMADC_REG_GPIO, ui32Gpio & (~0x1));
    }
    else // low power mode
    {
        nemadc_reg_write(NEMADC_REG_GPIO, ui32Gpio | 0x1);
    }

    nemadc_dsi_ct((uint32_t)0, //DC_dt_DCS_long_write,
                  (uint32_t)0, //DC_dt_DCS_short_write_param_n1,
                   NemaDC_dcs_datacmd);
    wait_NemaDC_dbi_idle();
    //
    // enable command/parameters packing
    //
    ui32Mode = nemadc_reg_read(NEMADC_REG_DBIB_CFG);
    nemadc_MIPI_CFG_out(ui32Mode | MIPICFG_SPI_HOLD);
    //
    // Download command & parameter to DBI i/f
    //
    nemadc_MIPI_out(NemaDC_DBI_cmd | ui8Cmd);
    for (ui8Index = 0; ui8Index < ui8Len; ui8Index++)
    {
        nemadc_MIPI_out(pui8Data[ui8Index]);
    }

    //
    // Send command-parameter packet.
    // Apollo4 ERR074 workaround
    //
    if (!APOLLO4_GT_B2)
    {
        am_util_delay_us(20);
        ui32Val6 = CLKGEN->DISPCLKCTRL_b.DISPCLKSEL;
        am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_OFF, NULL);
    }
    nemadc_MIPI_CFG_out(ui32Mode);
    if (!APOLLO4_GT_B2)
    {
        CLKGEN->DISPCLKCTRL_b.DISPCLKSEL = ui32Val6;
        am_util_delay_us(10);
    }
    wait_NemaDC_dbi_idle();
    //
    // For B2, change DC primary clock ratio back to 1 after sending commands.
    //
    if (APOLLO4_B2)
    {
        nemadc_clkdiv(1, 1, 4, 0);
    }
}

//*****************************************************************************
//
// @brief DSI read.
//
// @param cmd
// @param n_data
// @param bHS
//
// @return
//
//****************************************************************************
uint32_t
dsi_dcs_read(uint8_t cmd, uint8_t n_data, bool bHS)
{
    uint32_t dc_cmd_field;
    uint32_t ui32Mode;
    uint32_t ui32Val6 = 0;
    int32_t  mask;
    uint32_t ui32Gpio;
    ui32Gpio = nemadc_reg_read(NEMADC_REG_GPIO);
    //
    // B2 supports setting DC primary clock divide ratio to 1 when sending frame,
    // but it only supports setting DC primary clock divide ratio to 2 when sending commands.
    //
    if (APOLLO4_B2)
    {
        nemadc_clkdiv(2, 1, 4, 0);
    }

    if (bHS == true) // high speed mode
    {
        nemadc_reg_write(NEMADC_REG_GPIO, ui32Gpio & (~0x1));
    }
    else // low power mode
    {
        nemadc_reg_write(NEMADC_REG_GPIO, ui32Gpio | 0x1);
    }

    switch ( n_data )
    {
        case 1 :
            dc_cmd_field = (NemaDC_DBI_cmd | NemaDC_DBI_read);
            mask = 0xff;
            break;
        case 2 :
            dc_cmd_field = (NemaDC_DBI_cmd | NemaDC_DBI_read | NemaDC_rcmd16);
            mask = 0xffff;
            break;
        case 3 :
            dc_cmd_field = (NemaDC_DBI_cmd | NemaDC_DBI_read | NemaDC_rcmd24);
            mask = 0xffffff;
            break;
        case 4 :
            dc_cmd_field = (NemaDC_DBI_cmd | NemaDC_DBI_read | NemaDC_rcmd32);
            mask = 0xffffffff;
            break;
        default :
            dc_cmd_field = (NemaDC_DBI_cmd | NemaDC_DBI_read);
            mask = 0xff;
            break;
    }

    nemadc_dsi_ct((uint32_t)0, //DC_dt_DCS_read_param_no,
                     (uint32_t)0, //DC_dt_DCS_read_param_no,
                     NemaDC_dcs_datacmd);
    wait_NemaDC_dbi_idle();

    //
    // enable command/parameters packing
    //
    ui32Mode = nemadc_reg_read(NEMADC_REG_DBIB_CFG);
    nemadc_MIPI_CFG_out(ui32Mode | MIPICFG_EN_DVALID | MIPICFG_SPI_HOLD);
    nemadc_reg_write(NEMADC_REG_DBIB_CMD, dc_cmd_field | cmd);

    //
    // Send command-parameter packet.
    // Apollo4 ERR074 workaround
    //
    if (!APOLLO4_GT_B2)
    {
        am_util_delay_us(20);
        ui32Val6 = CLKGEN->DISPCLKCTRL_b.DISPCLKSEL;
        am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_OFF, NULL);
    }
    nemadc_MIPI_CFG_out(ui32Mode | MIPICFG_EN_DVALID);
    if (!APOLLO4_GT_B2)
    {
        CLKGEN->DISPCLKCTRL_b.DISPCLKSEL = ui32Val6;
        am_util_delay_us(10);
    }
    wait_NemaDC_dbi_idle();
    nemadc_MIPI_CFG_out(ui32Mode);
    //
    // For B2, change DC primary clock ratio back to 1 after sending commands.
    //
    if (APOLLO4_B2)
    {
        nemadc_clkdiv(1, 1, 4, 0);
    }
    //
    // return read parameters
    //
    return nemadc_reg_read(NEMADC_REG_DBIB_RDAT)&mask;
}

//*****************************************************************************
//
// @brief send DSI generic command.
//
// This function sends DSI commands to rm67162.
//
//****************************************************************************
void
dsi_generic_write(uint8_t* pui8Data, uint8_t ui8Len, bool bHS)
{
    uint8_t ui8Index;
    uint32_t ui32Mode, ui32Gpio;
    uint32_t ui32Val6 = 0;
    ui32Gpio = nemadc_reg_read(NEMADC_REG_GPIO);
    nemadc_reg_write(NEMADC_REG_GPIO, ui32Gpio & (~0x6)); // set vc_no to 00.
    ui32Gpio = nemadc_reg_read(NEMADC_REG_GPIO);
    //
    // B2 supports setting DC primary clock divide ratio to 1 when sending frame,
    // but it only supports setting DC primary clock divide ratio to 2 when sending commands.
    //
    if (APOLLO4_B2)
    {
        nemadc_clkdiv(2, 1, 4, 0);
    }

    if (bHS == true) //high speed mode
    {
        nemadc_reg_write(NEMADC_REG_GPIO, ui32Gpio & (~0x1));
    }
    else // low power mode
    {
        nemadc_reg_write(NEMADC_REG_GPIO, ui32Gpio | 0x1);
    }

    nemadc_dsi_ct((uint32_t)0, //DC_dt_generic_long_write,
                  (uint32_t)0, //DC_dt_generic_long_write,
                   NemaDC_ge_datacmd);
    wait_NemaDC_dbi_idle();

    //
    // enable command/parameters packing
    //
    ui32Mode = nemadc_reg_read(NEMADC_REG_DBIB_CFG);

    //
    // the maximum value of ui8Len is 253(bytes) for LP mode,8(bytes) for HP mode.
    //
    if ( ui8Len < 9 )
    {
        nemadc_MIPI_CFG_out(ui32Mode | MIPICFG_SPI_HOLD);
    }
    else
    {
        nemadc_MIPI_CFG_out(ui32Mode | MIPICFG_FRC_CSX_0);
    }

    //
    // Download command & parameter to DBI i/f
    //
    if ( ui8Len == 0 )
    {
        nemadc_MIPI_out(NemaDC_DBI_cmd | 0x00);
    }
    else
    {
        for (ui8Index = 0; ui8Index < ui8Len; ui8Index++)
        {
            if ( ui8Len < 3 )
            {
                nemadc_MIPI_out(NemaDC_DBI_cmd | pui8Data[ui8Index]);
            }
            else
            {
                nemadc_MIPI_out(pui8Data[ui8Index]);
            }
        }
    }

    //
    // Send command-parameter packet.
    // Apollo4 ERR074 workaround
    //
    if (!APOLLO4_GT_B2)
    {
        am_util_delay_us(20);
        ui32Val6 = CLKGEN->DISPCLKCTRL_b.DISPCLKSEL;
        am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_OFF, NULL);
    }
    nemadc_MIPI_CFG_out(ui32Mode);
    if (!APOLLO4_GT_B2)
    {
        CLKGEN->DISPCLKCTRL_b.DISPCLKSEL = ui32Val6;
        am_util_delay_us(10);
    }
    wait_NemaDC_dbi_idle();
    //
    // For B2, change DC primary clock ratio back to 1 after sending commands.
    //
    if (APOLLO4_B2)
    {
        nemadc_clkdiv(1, 1, 4, 0);
    }
}

//*****************************************************************************
//
// @brief DSI generic read command.
//
// @param cmd
// @param n_para
// @param n_data
// @param bHS
//
// This function reads parameters from rm67162.
//
// @return
//
//****************************************************************************
uint32_t
dsi_generic_read(uint8_t *cmd, uint8_t n_para, uint8_t n_data, bool bHS)
{
    unsigned int dc_cmd_field;
    int  mask, i;
    uint32_t ui32Mode, ui32Gpio;
    uint32_t ui32Val6 = 0;
    ui32Gpio = nemadc_reg_read(NEMADC_REG_GPIO);
    nemadc_reg_write(NEMADC_REG_GPIO, ui32Gpio & (~0x6)); // set vc_no to 00.
    ui32Gpio = nemadc_reg_read(NEMADC_REG_GPIO);
    //
    // B2 supports setting DC primary clock divide ratio to 1 when sending frame,
    // but it only supports setting DC primary clock divide ratio to 2 when sending commands.
    //
    if (APOLLO4_B2)
    {
        nemadc_clkdiv(2, 1, 4, 0);
    }

    if (bHS == true) // high speed mode
    {
        nemadc_reg_write(NEMADC_REG_GPIO, ui32Gpio & (~0x1));
    }
    else // low power mode
    {
        nemadc_reg_write(NEMADC_REG_GPIO, ui32Gpio | 0x1);
    }

    switch ( n_data )
    {
        case 1 :
            dc_cmd_field = (NemaDC_DBI_cmd | NemaDC_ext_ctrl | NemaDC_DBI_read);
            mask = 0xff;
            break;
        case 2 :
            dc_cmd_field = (NemaDC_DBI_cmd | NemaDC_ext_ctrl | NemaDC_DBI_read | NemaDC_rcmd16);
            mask = 0xffff;
            break;
        case 3 :
            dc_cmd_field = (NemaDC_DBI_cmd | NemaDC_ext_ctrl | NemaDC_DBI_read | NemaDC_rcmd24);
            mask = 0xffffff;
            break;
        case 4 :
            dc_cmd_field = (NemaDC_DBI_cmd | NemaDC_ext_ctrl | NemaDC_DBI_read | NemaDC_rcmd32);
            mask = 0xffffffff;
            break;
        default :
            dc_cmd_field = (NemaDC_DBI_cmd | NemaDC_ext_ctrl | NemaDC_DBI_read);
            mask = 0xff;
            break;
    }

    nemadc_dsi_ct((uint32_t)0, //DC_dt_DCS_read_param_no,
                     (uint32_t)0, //DC_dt_DCS_read_param_no,
                     NemaDC_ge_datacmd);

    //
    // wait NemaDC idle
    //
    wait_NemaDC_dbi_idle();

    //
    // send command
    //
    ui32Mode = nemadc_reg_read(NEMADC_REG_DBIB_CFG);
    nemadc_MIPI_CFG_out(ui32Mode | MIPICFG_EN_DVALID | MIPICFG_SPI_HOLD);

    if (n_para == 0) // Generic read, no parameter
    {
        nemadc_reg_write(NEMADC_REG_DBIB_CMD, dc_cmd_field | 0x00);
    }
    else
    {
        for (i = 0; i < n_para; i++)
        {
            nemadc_reg_write(NEMADC_REG_DBIB_CMD, dc_cmd_field | cmd[i]);
        }
    }

    //
    // Send command-parameter packet.
    // Apollo4 ERR074 workaround
    //
    if (!APOLLO4_GT_B2)
    {
        am_util_delay_us(20);
        ui32Val6 = CLKGEN->DISPCLKCTRL_b.DISPCLKSEL;
        am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_OFF, NULL);
    }
    nemadc_MIPI_CFG_out(ui32Mode | MIPICFG_EN_DVALID);
    if (!APOLLO4_GT_B2)
    {
        CLKGEN->DISPCLKCTRL_b.DISPCLKSEL = ui32Val6;
        am_util_delay_us(10);
    }
    //
    // wait NemaDC idle
    //
    wait_NemaDC_dbi_idle();
    nemadc_MIPI_CFG_out(ui32Mode);
    //
    // For B2, change DC primary clock ratio back to 1 after sending commands.
    //
    if (APOLLO4_B2)
    {
        nemadc_clkdiv(1, 1, 4, 0);
    }
    //
    // return read parameters
    //
    return nemadc_reg_read(NEMADC_REG_DBIB_RDAT)&mask;
}

//*****************************************************************************
//
// @brief Initialize the rm67162 dsi driver.
//
// @param  ui32PixelFormat     - Pixel color format
// @param  ui16ResX, ui16ResY  - Resolution of frame
// @param  ui16MinX, ui16MinY  - Start point of panel region to be updated
//
// @note This function should be called before any other am_devices_rm67162
// functions. It is used to set tell the other functions how to communicate
// with the OLED display hardware.
//
// @return Status.
//
//*****************************************************************************
uint32_t
am_devices_dsi_rm67162_init(uint32_t ui32PixelFormat, uint16_t ui16ResX, uint16_t ui16ResY, uint16_t ui16MinX, uint16_t ui16MinY)
{
    uint8_t ui8CmdBuf[4];
    uint16_t ui16MaxX, ui16MaxY;
    const int MIPI_set_cmd_page = 0xFE;

    if (g_eDispType == RM69330_DSI)
    {
        #ifdef AM_BSP_DISPLAY_OFFSET
            ui16MinX += AM_BSP_DISPLAY_OFFSET; // To ajust Apollo4b_bga_evb_disp_shld offset
        #endif
    }

    ui16MaxX = ui16MinX + ui16ResX - 1;
    ui16MaxY = ui16MinY + ui16ResY - 1;
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
    nemadc_clkctrl((uint32_t)TB_LCDPANEL_MIPI_DBIB );

    //
    // hardware reset
    //
    send_reset_signal();

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
    DELAY(10);

    //
    // Enable/disable tearing
    //
#ifndef ENABLE_TE
    dsi_dcs_write(MIPI_set_tear_off, NULL, 0, true);
#else
    ui8CmdBuf[0] = 0x02; // TE output active at refresh frame
    dsi_dcs_write(MIPI_set_tear_on, ui8CmdBuf, 1, true);
    DELAY(10);

    //
    // Set the time when TE is sent out
    //
    uint16_t ui16ScanLine = g_sDispCfg[g_eDispType].ui32PanelResY - 10;
    ui8CmdBuf[0] = ui16ScanLine >> 8;
    ui8CmdBuf[1] = ui16ScanLine & 0x00FF;
    dsi_dcs_write(MIPI_set_tear_scanline, ui8CmdBuf, 2, true);
#endif
    DELAY(10);

    //
    // Sending brightness command.
    //
    const int MIPI_set_display_brightness = 0x51;
    ui8CmdBuf[0] = MIPI_set_display_brightness;
    ui8CmdBuf[1] = 0xff;
    dsi_generic_write(ui8CmdBuf, 2, true);
    DELAY(10);

    //
    // Set MIPI Panel Pixel Format
    //
    ui8CmdBuf[0] = (uint8_t)(ui32PixelFormat & 0x3f);
    dsi_dcs_write(MIPI_set_pixel_format, ui8CmdBuf, 1, true);
    DELAY(10);

#if defined(ADJ_TE_FREQ) && defined(ENABLE_TE)
    //
    // Adjust TE frequency of display.
    //
    const int MIPI_set_lfrctr4 = 0x28;
    ui8CmdBuf[0] = MIPI_set_cmd_page;
    ui8CmdBuf[1] = 0x01; // MCS
    dsi_generic_write(ui8CmdBuf, 2, true);
    DELAY(10);
    ui8CmdBuf[0] = MIPI_set_lfrctr4;
    ui8CmdBuf[1] = 0x65; // 0x65 - ~45fps, 0x70 - ~50fps
    dsi_generic_write(ui8CmdBuf, 2, true);
    DELAY(10);
    ui8CmdBuf[0] = MIPI_set_cmd_page;
    ui8CmdBuf[1] = 0x00; // UCS
    dsi_generic_write(ui8CmdBuf, 2, true);
    DELAY(10);
#endif

    //
    // When using RM69330 DSI, set N565 reg to 1 to solve color issue.
    //
    if (g_eDispType == RM69330_DSI)
    {
        const int MIPI_set_hsifopctr = 0x0A;
        ui8CmdBuf[0] = MIPI_set_cmd_page;
        ui8CmdBuf[1] = 0x01; // MCS
        dsi_generic_write(ui8CmdBuf, 2, true);
        DELAY(10);
        ui8CmdBuf[0] = MIPI_set_hsifopctr;
        ui8CmdBuf[1] = 0xF8; // set N565 to 1
        dsi_generic_write(ui8CmdBuf, 2, true);
        DELAY(10);
        ui8CmdBuf[0] = MIPI_set_cmd_page;
        ui8CmdBuf[1] = 0x00; // UCS
        dsi_generic_write(ui8CmdBuf, 2, true);
        DELAY(10);
    }
#if DSI_DEBUG
    nemadc_dsi_ct(NemaDC_dt_DCS_read_param_no,
                  NemaDC_dt_DCS_read_param_no,
                  NemaDC_dcs_datacmd);
    nemadc_MIPI_out(NemaDC_DBI_cmd | NemaDC_ext_ctrl | NemaDC_DBI_read | MIPI_get_pixel_format);
    am_util_delay_us(100);
    DSI->DEVICEREADY_b.DISPLAYBUSPOSSESSEN = 1;
    am_util_delay_us(1);
    DSI->DEVICEREADY_b.DISPLAYBUSPOSSESSEN = 0;
    am_util_delay_us(10);
    uint32_t ui32IntrStatus = DSI->INTRSTAT;
    DSI->DEVICEREADY_b.DISPLAYBUSPOSSESSEN = 0;
    DSI->INTRSTAT = 0x03FFFFFF;
    dsi_dcs_write(MIPI_soft_reset, NULL, 0, true);
    if ((ui32IntrStatus == 0x11100000) || (ui32IntrStatus == 0x10100000))
    {
        am_util_stdio_printf("\nACK from display is right, ACK is 0x%8X, please also check the image displayed on panel.\n", ui32IntrStatus);
    }
    else
    {
        am_util_stdio_printf("\nACK from display is wrong. ACK is 0x%8X.\n", ui32IntrStatus);
        return 1;
    }
#endif

    if (g_sDispCfg[g_eDispType].bFlip == true)
    {
        ui8CmdBuf[0] = 0x02;
    }
    else
    {
        ui8CmdBuf[0] = 0x00;
    }
    dsi_dcs_write(MIPI_set_address_mode, ui8CmdBuf, 1, true);
    DELAY(10);

    //
    // Enable MIPI Panel
    //
    dsi_dcs_write(MIPI_exit_sleep_mode, NULL, 0, true);
    DELAY(130);

    dsi_dcs_write(MIPI_set_display_on, NULL, 0, true);
    DELAY(200);

    //
    // Set MIPI Panel region to be updated
    //
    ui8CmdBuf[0] = (uint8_t)(ui16MinX >> 8U);
    ui8CmdBuf[1] = (uint8_t)(ui16MinX & 0xFFU);
    ui8CmdBuf[2] = (uint8_t)(ui16MaxX >> 8U);
    ui8CmdBuf[3] = (uint8_t)(ui16MaxX & 0xFFU);
    dsi_dcs_write(MIPI_set_column_address, ui8CmdBuf, 4, true);
    DELAY(200);
    ui8CmdBuf[0] = (uint8_t)(ui16MinY >> 8U);
    ui8CmdBuf[1] = (uint8_t)(ui16MinY & 0xFFU);
    ui8CmdBuf[2] = (uint8_t)(ui16MaxY >> 8U);
    ui8CmdBuf[3] = (uint8_t)(ui16MaxY & 0xFFU);
    dsi_dcs_write(MIPI_set_page_address, ui8CmdBuf, 4, true);
    DELAY(200);

    //
    // Program NemaDC to transfer a resx*resy region
    //
    nemadc_timing(ui16ResX, 4, 10, 1,
                  ui16ResY, 1, 1, 1);

    return 0;
}

//*****************************************************************************
//
// @brief Set the rm67162 updated region.
//
// @param  ui16ResX, ui16ResY    - resolution of frame
// @param  ui16MinX, ui16MinY    - start point of panel region to be updated
//
// This is used to set rm67162 updated region.
//
// @return Status.
//
//*****************************************************************************
uint32_t
am_devices_dsi_rm67162_set_region(uint16_t ui16ResX, uint16_t ui16ResY, uint16_t ui16MinX, uint16_t ui16MinY)
{
    uint8_t ui8CmdBuf[4];
    uint16_t ui16MaxX, ui16MaxY;

    ui16MaxX = ui16MinX + ui16ResX - 1;
    ui16MaxY = ui16MinY + ui16ResY - 1;

    //
    // Set MIPI Panel region to be updated
    //
    ui8CmdBuf[0] = (uint8_t)(ui16MinX >> 8U);
    ui8CmdBuf[1] = (uint8_t)(ui16MinX & 0xFFU);
    ui8CmdBuf[2] = (uint8_t)(ui16MaxX >> 8U);
    ui8CmdBuf[3] = (uint8_t)(ui16MaxX & 0xFFU);
    dsi_dcs_write(MIPI_set_column_address, ui8CmdBuf, 4, true);

    ui8CmdBuf[0] = (uint8_t)(ui16MinY >> 8U);
    ui8CmdBuf[1] = (uint8_t)(ui16MinY & 0xFFU);
    ui8CmdBuf[2] = (uint8_t)(ui16MaxY >> 8U);
    ui8CmdBuf[3] = (uint8_t)(ui16MaxY & 0xFFU);
    dsi_dcs_write(MIPI_set_page_address, ui8CmdBuf, 4, true);

    return 0;
}

//*****************************************************************************
//
// @brief send single frame.
//
// @param ui32Mode
//
// This function sends frame to rm67162.
//
//****************************************************************************
void
dsi_send_frame_single(uint32_t ui32Mode)
{
    dsi_send_frame_single_start(ui32Mode);
    nemadc_wait_vsync();
    dsi_send_frame_single_end();
}

//*****************************************************************************
//
// @brief start sending single frame.
//
// @param ui32Mode
//
// This function sends frame to rm67162.
//
//****************************************************************************
void
dsi_send_frame_single_start(uint32_t ui32Mode)
{
    uint32_t ui32Gpio;
    ui32Gpio = nemadc_reg_read(NEMADC_REG_GPIO);
    nemadc_reg_write(NEMADC_REG_GPIO, ui32Gpio & (~0x1)); // HS
    nemadc_reg_write(NEMADC_REG_CLKCTRL_CG, 0xFFFFFFF5U); // disable clock gating

    wait_NemaDC_dbi_idle();

    //
    // Set data/commands command type
    //
    nemadc_dsi_ct(NemaDC_dt_DCS_long_write,
                     NemaDC_dt_DCS_long_write,
                     NemaDC_dcs_datacmd);

    //
    // Set scan-line (DCS) command
    //
    nemadc_MIPI_out(MIPI_DBIB_CMD | MIPI_write_memory_continue | NemaDC_sline_cmd);

    uint32_t ui32DbiCfg = nemadc_reg_read(NEMADC_REG_DBIB_CFG);
    nemadc_MIPI_CFG_out(ui32DbiCfg | MIPICFG_SPI_HOLD);

    //
    // Send DCS write_memory_start command
    //
    nemadc_MIPI_out(MIPI_DBIB_CMD | MIPI_write_memory_start);

    wait_NemaDC_dbi_idle();
#ifdef ENABLE_TE
    //
    // Schedule an interrupt on next TE
    //
    nemadc_reg_write(NEMADC_REG_INTERRUPT, 1 << 3);
    //
    // Wait TE interrupt
    //
    nemadc_wait_te();
#endif
    nemadc_reg_write(NEMADC_REG_INTERRUPT, 1 << 4); //!< Enable frame end interrupt
    nemadc_set_mode(NEMADC_ONE_FRAME | ui32Mode);
}


//*****************************************************************************
//
// @brief start sending single frame.
//
// This function sends frame to rm67162.
//
// @return None.
//
//****************************************************************************
void
dsi_send_frame_single_start_none_block_te(void)
{
    uint32_t ui32Gpio;
    ui32Gpio = nemadc_reg_read(NEMADC_REG_GPIO);
    nemadc_reg_write(NEMADC_REG_GPIO, ui32Gpio & (~0x1)); // HS
    nemadc_reg_write(NEMADC_REG_CLKCTRL_CG, 0xFFFFFFF5U); // disable clock gating

    wait_NemaDC_dbi_idle();

    //
    // Set data/commands command type
    //
    nemadc_dsi_ct(NemaDC_dt_DCS_long_write,
                     NemaDC_dt_DCS_long_write,
                     NemaDC_dcs_datacmd);

    //
    // Set scan-line (DCS) command
    //
    nemadc_MIPI_out(MIPI_DBIB_CMD | MIPI_write_memory_continue /* | NemaDC_ext_ctrl */ | NemaDC_sline_cmd);

    uint32_t ui32DbiCfg = nemadc_reg_read(NEMADC_REG_DBIB_CFG);
    nemadc_MIPI_CFG_out(ui32DbiCfg | MIPICFG_SPI_HOLD);

    //
    // Send DCS write_memory_start command
    //
    nemadc_MIPI_out(MIPI_DBIB_CMD | MIPI_write_memory_start /*| NemaDC_ext_ctrl*/);

    wait_NemaDC_dbi_idle();
    // Schedule an interrupt on next TE
    //
    nemadc_reg_write(NEMADC_REG_INTERRUPT, 1 << 3);
}

//*****************************************************************************
//
// @brief Function to dsi_send_frame_single_start_none_block_vsync
//
// This function send start flush the screen without block
//
// @param ui32Mode - NEMADC mode select
//
//*****************************************************************************
void
dsi_send_frame_single_start_none_block_vsync(uint32_t ui32Mode)
{
    nemadc_reg_write(NEMADC_REG_INTERRUPT, 1 << 4); //!< Enable frame end interrupt
    nemadc_set_mode(NEMADC_ONE_FRAME | ui32Mode);
}

//*****************************************************************************
//
// @brief Configurations after completing single frame.
//
// This function should be called after completing single frame.
//
//****************************************************************************
void
dsi_send_frame_single_end(void)
{
    uint32_t ui32Gpio;
    uint32_t ui32Val6 = 0;
    ui32Gpio = nemadc_reg_read(NEMADC_REG_GPIO);
    uint32_t ui32DbiCfg = nemadc_reg_read(NEMADC_REG_DBIB_CFG);

    //
    // Apollo4 ERR074 workaround
    //
    if (!APOLLO4_GT_B2)
    {
        am_util_delay_us(20);
        ui32Val6 = CLKGEN->DISPCLKCTRL_b.DISPCLKSEL;
        am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_OFF, NULL);
    }
    nemadc_MIPI_CFG_out(ui32DbiCfg & (~MIPICFG_SPI_HOLD));
    if (!APOLLO4_GT_B2)
    {
        CLKGEN->DISPCLKCTRL_b.DISPCLKSEL = ui32Val6;
        am_util_delay_us(10);
    }
    nemadc_reg_write(NEMADC_REG_CLKCTRL_CG, 0x5); // enable clock gating
    nemadc_reg_write(NEMADC_REG_GPIO, ui32Gpio | 0x1); // LP
}

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

