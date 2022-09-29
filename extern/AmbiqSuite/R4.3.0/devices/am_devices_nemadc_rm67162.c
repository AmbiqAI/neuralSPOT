//*****************************************************************************
//
//! @file am_devices_nemadc_rm67162.h
//!
//! @brief Generic Raydium display driver.
//!
//! @addtogroup nemaadc_rm67162 RM67162 NEMADC Driver
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

#include "am_devices_nemadc_rm67162.h"
#include "am_util_delay.h"
#include "am_bsp.h"

#ifndef SIMULATION
#define DELAY am_util_delay_ms
#else
#define DELAY(...)
#endif

//*****************************************************************************
//
//! @brief hardware reset rm67162.
//!
//!
//! This function send a RESX signal to the display
//!
//! @return None.
//
//*****************************************************************************
static void send_RESX_signal(void)
{
    //
    // send display a reset
    //

    uint32_t ui32GpioNum;
    if (g_sDispCfg[g_eDispType].eInterface == IF_DSPI)
    {
        ui32GpioNum = AM_BSP_GPIO_DISP_DSPI_RES;
    }
    else if (g_sDispCfg[g_eDispType].eInterface == IF_QSPI)
    {
        ui32GpioNum = AM_BSP_GPIO_DISP_QSPI_RES;
    }
    else
    {
        ui32GpioNum = AM_BSP_GPIO_DISP_SPI_RES;
    }
    am_hal_gpio_pinconfig(ui32GpioNum,  am_hal_gpio_pincfg_output);
    am_hal_gpio_state_write(ui32GpioNum, AM_HAL_GPIO_OUTPUT_SET);
    DELAY(5);
    am_hal_gpio_state_write(ui32GpioNum, AM_HAL_GPIO_OUTPUT_CLEAR);
    DELAY(20);
    am_hal_gpio_state_write(ui32GpioNum, AM_HAL_GPIO_OUTPUT_SET);
    DELAY(150);
}

//*****************************************************************************
//
//! @brief send QSPI command.
//!
//!
//! This function sends QSPI commands to rm67162.
//!
//! @return None.
//
//****************************************************************************
void send_MIPI_cmd(uint8_t spi_mode, uint8_t cmd, uint8_t* data, uint8_t l)
{
    if (spi_mode == QSPI_MODE)
    {   //QSPI
        uint32_t dbi_cfg = nemadc_reg_read(NEMADC_REG_DBIB_CFG);
        nemadc_MIPI_CFG_out(dbi_cfg | MIPICFG_SPI_HOLD);
        nemadc_MIPI_out(MIPI_DBIB_CMD | MIPI_MASK_QSPI | CMD1_DATA1);
        nemadc_MIPI_out(MIPI_DBIB_CMD | MIPI_MASK_QSPI | MIPI_CMD24 | (cmd << CMD_OFFSET));
        for ( int i = 0; i < l; i++ )
        {
            nemadc_MIPI_out(MIPI_DBIB_CMD | MIPI_MASK_QSPI | data[i]);
        }
        nemadc_MIPI_CFG_out(dbi_cfg);
    }
    else if (spi_mode == DSPI_MODE)
    {   //DSPI
        uint32_t dbi_cfg = nemadc_reg_read(NEMADC_REG_DBIB_CFG);
        nemadc_MIPI_CFG_out(dbi_cfg & (~MIPICFG_DSPI) & (~MIPICFG_QSPI));
        nemadc_MIPI_out(MIPI_DBIB_CMD | cmd);
        for ( int i = 0; i < l; i++ )
        {
            nemadc_MIPI_out(data[i]);
        }
        nemadc_MIPI_CFG_out(dbi_cfg);
    }
    else
    {   //SPI4
        nemadc_MIPI_out(MIPI_DBIB_CMD | cmd);
        for ( int i = 0; i < l; i++ )
        {
            nemadc_MIPI_out(data[i]);
        }
    }

}

//*****************************************************************************
//
//! @brief Initialize the rm67162 driver.
//!
//! @param  mode          - interface mode
//! @param  pixel_format  - pixel color format
//! @param  resx, resy    - resolution of frame
//! @param  minx, miny    - start point of panel region to be updated
//!
//! This function should be called before any other am_devices_rm67162
//! functions. It is used to set tell the other functions how to communicate
//! with the OLED display hardware.
//!
//! @return Status.
//
//*****************************************************************************
uint32_t
am_devices_nemadc_rm67162_init(uint32_t mode, uint32_t pixel_format, uint16_t resx, uint16_t resy, uint16_t minx, uint16_t miny)
{
    uint8_t spi_mode;
    uint8_t cmd_buf[4];
    uint16_t maxx, maxy;

    if (g_eDispType == RM69330_QSPI)
    {
        #ifdef AM_BSP_DISPLAY_OFFSET
            minx += AM_BSP_DISPLAY_OFFSET; // To ajust Apollo4b_bga_evb_disp_shld offset
        #endif
    }

    maxx = minx + resx - 1;
    maxy = miny + resy - 1;

    if ( 0!=(mode&MIPICFG_QSPI) )
    {
        spi_mode = QSPI_MODE;
        am_bsp_disp_qspi_pins_enable();
    }
    else if ( 0!=(mode&MIPICFG_DSPI) )
    {
        spi_mode = DSPI_MODE;
        am_bsp_disp_dspi_pins_enable();
    }
    else
    {
        spi_mode = SPI4_MODE;
        am_bsp_disp_spi_pins_enable();
    }

    // hardware reset
    send_RESX_signal();

    // Program NemaDC MIPI interface
    nemadc_MIPI_CFG_out(mode | MIPICFG_SPI_CSX_V | MIPICFG_DBI_EN | MIPICFG_RESX | pixel_format | MIPICFG_DIS_TE);
    DELAY(10);

    // Enable/disable tearing
    send_MIPI_cmd(spi_mode, MIPI_set_tear_off, NULL, 0);
    DELAY(10);

    const int MIPI_set_dspi_mode = 0xc4;
    if (spi_mode == DSPI_MODE)
    {
        cmd_buf[0] = 0xA1; //enable DSPI 1P1T 2-Wire
    }
    else
    {
        cmd_buf[0] = 0x80;
    }
    send_MIPI_cmd(spi_mode, MIPI_set_dspi_mode, cmd_buf, 1);
    DELAY(10);

    // Set MIPI Panel Pixel Format
    cmd_buf[0] = (uint8_t)(pixel_format & 0x3f);
    send_MIPI_cmd(spi_mode, MIPI_set_pixel_format, cmd_buf, 1);
    DELAY(10);

    if (g_sDispCfg[g_eDispType].bFlip == true)
    {
        cmd_buf[0] = 0x02;
    }
    else
    {
        cmd_buf[0] = 0x00;
    }
    send_MIPI_cmd(spi_mode, MIPI_set_address_mode, cmd_buf, 1);
    DELAY(10);

    const int MIPI_set_wr_display_ctrl = 0x53;
    cmd_buf[0] = 0x20;
    send_MIPI_cmd(spi_mode, MIPI_set_wr_display_ctrl, cmd_buf, 1);
    DELAY(10);

    const int MIPI_set_display_brightness = 0x51;
    cmd_buf[0] = 0xff;      // write display brightness
    send_MIPI_cmd(spi_mode, MIPI_set_display_brightness, cmd_buf, 1);
    DELAY(10);

    cmd_buf[0] = 0x0;
    cmd_buf[1] = 0x28;
    send_MIPI_cmd(spi_mode, MIPI_set_tear_scanline, cmd_buf, 2);
    DELAY(10);

    // Enable MIPI Panel
    send_MIPI_cmd(spi_mode, MIPI_exit_sleep_mode, NULL, 0);
    DELAY(130);

    send_MIPI_cmd(spi_mode, MIPI_set_display_on, NULL, 0);
    DELAY(200);

    // Set MIPI Panel region to be updated
    cmd_buf[0] = (uint8_t)(minx >> 8U);
    cmd_buf[1] = (uint8_t)(minx  & 0xFFU);
    cmd_buf[2] = (uint8_t)(maxx >> 8U);
    cmd_buf[3] = (uint8_t)(maxx  & 0xFFU);
    send_MIPI_cmd(spi_mode, MIPI_set_column_address, cmd_buf, 4);

    cmd_buf[0] = (uint8_t)(miny >> 8U);
    cmd_buf[1] = (uint8_t)(miny  & 0xFFU);
    cmd_buf[2] = (uint8_t)(maxy >> 8U);
    cmd_buf[3] = (uint8_t)(maxy  & 0xFFU);
    send_MIPI_cmd(spi_mode, MIPI_set_page_address, cmd_buf, 4);
    // Program NemaDC to transfer a resx*resy region
    nemadc_timing(resx, 4, 10, 10,
                  resy, 10, 50, 10);

    return 0;
}

//*****************************************************************************
//
//! @brief Set the rm67162 updated region.
//!
//! @param  mode          - interface mode
//! @param  resx, resy    - resolution of frame
//! @param  minx, miny    - start point of panel region to be updated
//!
//! It is used to set rm67162 updated region.
//!
//! @return Status.
//
//*****************************************************************************
uint32_t
am_devices_nemadc_rm67162_set_region(uint32_t mode, uint16_t resx, uint16_t resy, uint16_t minx, uint16_t miny)
{
    uint8_t spi_mode;
    uint8_t cmd_buf[4];
    uint16_t maxx, maxy;

    maxx = minx + resx - 1;
    maxy = miny + resy - 1;

    if ( 0!=(mode&MIPICFG_QSPI) )
    {
        spi_mode = QSPI_MODE;
    }
    else if ( 0!=(mode&MIPICFG_DSPI) )
    {
        spi_mode = DSPI_MODE;
    }
    else
    {
        spi_mode = SPI4_MODE;
    }

    // Set MIPI Panel region to be updated
    cmd_buf[0] = (uint8_t)(minx >> 8U);
    cmd_buf[1] = (uint8_t)(minx  & 0xFFU);
    cmd_buf[2] = (uint8_t)(maxx >> 8U);
    cmd_buf[3] = (uint8_t)(maxx  & 0xFFU);
    send_MIPI_cmd(spi_mode, MIPI_set_column_address, cmd_buf, 4);

    cmd_buf[0] = (uint8_t)(miny >> 8U);
    cmd_buf[1] = (uint8_t)(miny  & 0xFFU);
    cmd_buf[2] = (uint8_t)(maxy >> 8U);
    cmd_buf[3] = (uint8_t)(maxy  & 0xFFU);
    send_MIPI_cmd(spi_mode, MIPI_set_page_address, cmd_buf, 4);

    return 0;
}

//*****************************************************************************
//
//! @brief send single frame.
//!
//!
//! This function sends QSPI/SPI frame to rm67162.
//!
//! @return None.
//
//****************************************************************************
void nemadc_send_frame_single(void)
{
    uint32_t dbi_cfg;
    nemadc_reg_write(NEMADC_REG_CLKCTRL_CG, 0xAA000000U); // disable clock gating
    switch (g_sDispCfg[g_eDispType].eInterface)
    {
        case IF_QSPI:
            dbi_cfg = nemadc_reg_read( NEMADC_REG_DBIB_CFG);
            nemadc_MIPI_CFG_out( dbi_cfg | MIPICFG_SPI_HOLD);
            nemadc_MIPI_out    ( MIPI_DBIB_CMD | MIPI_MASK_QSPI | CMD1_DATA4);
            nemadc_MIPI_out    ( MIPI_DBIB_CMD | MIPI_MASK_QSPI | MIPI_CMD24 |
                               (MIPI_write_memory_start << CMD_OFFSET));
            nemadc_reg_write(NEMADC_REG_INTERRUPT, 1 << 4); //!< Enable frame end interrupt
            nemadc_set_mode(NEMADC_ONE_FRAME);
            // Wait for transfer to be completed
            nemadc_wait_vsync();
            nemadc_MIPI_CFG_out(dbi_cfg);
            break;
        case IF_DSPI:
            dbi_cfg = nemadc_reg_read( NEMADC_REG_DBIB_CFG);
            nemadc_MIPI_CFG_out( dbi_cfg & (~MIPICFG_DSPI) & (~MIPICFG_QSPI));
            // Start MIPI Panel Memory Write
            nemadc_MIPI_out(MIPI_DBIB_CMD | MIPI_write_memory_start);
            nemadc_MIPI_CFG_out(((dbi_cfg & (~MIPICFG_SPI4)) | MIPICFG_SPI3)
                               | MIPICFG_SPIDC_DQSPI | MIPICFG_SPI_HOLD);
            nemadc_reg_write(NEMADC_REG_INTERRUPT, 1 << 4); //!< Enable frame end interrupt
            // Send One Frame
            nemadc_set_mode(NEMADC_ONE_FRAME);
            // Wait for transfer to be completed
            nemadc_wait_vsync();
            nemadc_MIPI_CFG_out(dbi_cfg);
            break;
        case IF_SPI4:
            // Start MIPI Panel Memory Write
            nemadc_MIPI_out(MIPI_DBIB_CMD | MIPI_write_memory_start);
            nemadc_reg_write(NEMADC_REG_INTERRUPT, 1 << 4); //!< Enable frame end interrupt
            // Send One Frame
            nemadc_set_mode(NEMADC_ONE_FRAME);
            // Wait for transfer to be completed
            nemadc_wait_vsync();
            break;
        default:
            ; // NOP
    }
    nemadc_reg_write(NEMADC_REG_CLKCTRL_CG, 0); // enable clock gating
}

void nemadc_send_frame_single_start(void)
{
    uint32_t dbi_cfg;
    nemadc_reg_write(NEMADC_REG_CLKCTRL_CG, 0xAA000000U); // disable clock gating
    switch (g_sDispCfg[g_eDispType].eInterface)
    {
        case IF_QSPI:
            dbi_cfg = nemadc_reg_read( NEMADC_REG_DBIB_CFG);
            nemadc_MIPI_CFG_out( dbi_cfg | MIPICFG_SPI_HOLD);
            nemadc_MIPI_out    ( MIPI_DBIB_CMD | MIPI_MASK_QSPI | CMD1_DATA4);
            nemadc_MIPI_out    ( MIPI_DBIB_CMD | MIPI_MASK_QSPI | MIPI_CMD24 |
                               (MIPI_write_memory_start << CMD_OFFSET));
            nemadc_reg_write(NEMADC_REG_INTERRUPT, 1 << 4); //!< Enable frame end interrupt
            nemadc_set_mode(NEMADC_ONE_FRAME);
            break;
        case IF_DSPI:
            dbi_cfg = nemadc_reg_read( NEMADC_REG_DBIB_CFG);
            nemadc_MIPI_CFG_out( dbi_cfg & (~MIPICFG_DSPI) & (~MIPICFG_QSPI));
            // Start MIPI Panel Memory Write
            nemadc_MIPI_out(MIPI_DBIB_CMD | MIPI_write_memory_start);
            nemadc_MIPI_CFG_out(((dbi_cfg & (~MIPICFG_SPI4)) | MIPICFG_SPI3)
                               | MIPICFG_SPIDC_DQSPI | MIPICFG_SPI_HOLD);
            nemadc_reg_write(NEMADC_REG_INTERRUPT, 1 << 4); //!< Enable frame end interrupt
            // Send One Frame
            nemadc_set_mode(NEMADC_ONE_FRAME);
            break;
        case IF_SPI4:
            // Start MIPI Panel Memory Write
            nemadc_MIPI_out(MIPI_DBIB_CMD | MIPI_write_memory_start);
            nemadc_reg_write(NEMADC_REG_INTERRUPT, 1 << 4); //!< Enable frame end interrupt
            // Send One Frame
            nemadc_set_mode(NEMADC_ONE_FRAME);
            break;
        default:
            ; // NOP
    }
}

void nemadc_send_frame_single_end(void)
{
    uint32_t dbi_cfg;

    switch (g_sDispCfg[g_eDispType].eInterface)
    {
        case IF_QSPI:
            dbi_cfg = nemadc_reg_read( NEMADC_REG_DBIB_CFG);
            nemadc_MIPI_CFG_out(dbi_cfg & (~MIPICFG_SPI_HOLD));
            break;
        case IF_DSPI:
            dbi_cfg = nemadc_reg_read( NEMADC_REG_DBIB_CFG);
            nemadc_MIPI_CFG_out((dbi_cfg | MIPICFG_SPI4) & (~MIPICFG_SPI3)
                               & (~MIPICFG_SPIDC_DQSPI) & (~MIPICFG_SPI_HOLD));
            break;
        default:
            break;
    }

    nemadc_reg_write(NEMADC_REG_CLKCTRL_CG, 0); // enable clock gating
}

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

