//*****************************************************************************
//
//! @file am_devices_mspi_rm69330.c
//!
//! @brief General Multibit SPI Display driver.
//!
//! @addtogroup mspi_rm69330r RM69330 MSPI Display Driver
//! @ingroup devices
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2023, Ambiq Micro, Inc.
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
// This is part of revision release_sdk_3_1_1-10cda4b5e0 of the AmbiqSuite Development Package.
//
//*****************************************************************************
#include "am_mcu_apollo.h"
#include "am_devices_mspi_rm69330.h"
#include "am_util_stdio.h"
#include "am_util_delay.h"

//*****************************************************************************
//
// Global variables.
//
//*****************************************************************************
#define BYTE_NUM_PER_WRITE                          65535
#define AM_DEVICES_MSPI_RM69330_TIMEOUT             100000

//
// Adjust TE frequency of display.
//
//#define ADJ_TE_FREQ
static struct
{
    uint32_t row_start;
    uint32_t row_end;
    uint32_t col_start;
    uint32_t col_end;
} gs_display_info;

static am_devices_mspi_rm69330_graphic_conf_t g_sGraphic_conf =
{
    .bus_mode       = AM_DEVICES_MSPI_RM69330_SPI_WRAM,
    .color_mode     = AM_DEVICES_MSPI_RM69330_COLOR_MODE_16BIT,
    .scan_mode      = AM_DEVICES_MSPI_RM69330_SCAN_MODE_270,
    .max_row        = AM_DEVICES_RM69330_NUM_ROWS,
    .max_col        = AM_DEVICES_RM69330_NUM_COLUMNS,
    .row_offset     = 0,
    .col_offset     = 0
};

typedef struct
{
    uint32_t                    ui32Module;
    void                        *pMspiHandle;
    am_hal_mspi_dev_config_t    mspiDevCfg;
    bool                        bOccupied;
} am_devices_mspi_rm69330_t;

#if defined(AM_PART_APOLLO4) || defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
static am_hal_mspi_config_t gGDMspiCfg =
{
    .ui32TCBSize          = 0,
    .pTCB                 = NULL,
#ifdef RM69330_CLK_ON_D4_EN
    .bClkonD4             = 1,
#else
    .bClkonD4             = 0,
#endif
};
#endif
// Display MSPI configuration
static am_hal_mspi_dev_config_t  SerialCE0DisplayMSPICfg =
{
    .eSpiMode             = AM_HAL_MSPI_SPI_MODE_0,
    .eClockFreq           = AM_HAL_MSPI_CLK_48MHZ,
    .ui8TurnAround        = 1,
    .eAddrCfg             = AM_HAL_MSPI_ADDR_3_BYTE,
    .eInstrCfg            = AM_HAL_MSPI_INSTR_1_BYTE,
    .eDeviceConfig        = AM_HAL_MSPI_FLASH_SERIAL_CE0,
    .bSendInstr           = true,
    .bSendAddr            = false,
    .bTurnaround          = false,
#if defined(AM_PART_APOLLO3P)
    .ui8WriteLatency      = 0,
    .bEnWriteLatency      = false,
    .bEmulateDDR          = false,
    .ui16DMATimeLimit     = 0,
    .eDMABoundary         = AM_HAL_MSPI_BOUNDARY_NONE,
#endif
#if defined(AM_PART_APOLLO4) || defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
  .ui8WriteLatency      = 0,
  .bEnWriteLatency      = false,
  .bEmulateDDR          = false,
  .ui16DMATimeLimit     = 70,
  .eDMABoundary         = AM_HAL_MSPI_BOUNDARY_BREAK1K,
#if defined(AM_PART_APOLLO4)
  .eDeviceNum           = AM_HAL_MSPI_DEVICE0,
#endif
#else
  .ui32TCBSize          = 0,
  .pTCB                 = NULL,
  .scramblingStartAddr  = 0,
  .scramblingEndAddr    = 0,
#endif
#if defined(AM_PART_APOLLO4) || defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
    .ui16ReadInstr         = AM_DEVICES_MSPI_RM69330_FAST_READ,
    .ui16WriteInstr        = AM_DEVICES_MSPI_RM69330_PIXEL_WRITE_ADDR4,
#else
    .ui8ReadInstr         = AM_DEVICES_MSPI_RM69330_FAST_READ,
    .ui8WriteInstr        = AM_DEVICES_MSPI_RM69330_PIXEL_WRITE_ADDR4,
#endif
};

// Display MSPI configuration
static am_hal_mspi_dev_config_t  SerialCE1DisplayMSPICfg =
{
    .eSpiMode             = AM_HAL_MSPI_SPI_MODE_0,
    .eClockFreq           = AM_HAL_MSPI_CLK_48MHZ,
    .ui8TurnAround        = 1,
    .eAddrCfg             = AM_HAL_MSPI_ADDR_3_BYTE,
    .eInstrCfg            = AM_HAL_MSPI_INSTR_1_BYTE,
    .eDeviceConfig        = AM_HAL_MSPI_FLASH_SERIAL_CE1,
    .bSendInstr           = true,
    .bSendAddr            = false,
    .bTurnaround          = false,
#if defined(AM_PART_APOLLO3P)
    .ui8WriteLatency      = 0,
    .bEnWriteLatency      = false,
    .bEmulateDDR          = false,
    .ui16DMATimeLimit     = 0,
    .eDMABoundary         = AM_HAL_MSPI_BOUNDARY_NONE,
#endif
#if defined (AM_PART_APOLLO3) || defined (AM_PART_APOLLO3P)
    .ui32TCBSize          = 0,
    .pTCB                 = 0,
    .scramblingStartAddr  = 0,
    .scramblingEndAddr    = 0,
#endif
#if defined(AM_PART_APOLLO4) || defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
    .ui16ReadInstr         = AM_DEVICES_MSPI_RM69330_FAST_READ,
    .ui16WriteInstr        = AM_DEVICES_MSPI_RM69330_PIXEL_WRITE_ADDR4,
#else
    .ui8ReadInstr         = AM_DEVICES_MSPI_RM69330_FAST_READ,
    .ui8WriteInstr        = AM_DEVICES_MSPI_RM69330_PIXEL_WRITE_ADDR4,
#endif
};

static am_hal_mspi_dev_config_t  QuadCE0DisplayMSPICfg =
{
    .eSpiMode             = AM_HAL_MSPI_SPI_MODE_0,
    .eClockFreq           = AM_HAL_MSPI_CLK_48MHZ,
    .ui8TurnAround        = 0,
    .eAddrCfg             = AM_HAL_MSPI_ADDR_3_BYTE,
    .eInstrCfg            = AM_HAL_MSPI_INSTR_1_BYTE,
    .eDeviceConfig        = AM_HAL_MSPI_FLASH_QUAD_CE0_1_4_4,
    .bSendInstr           = true,
    .bSendAddr            = true,
    .bTurnaround          = false,
#if defined(AM_PART_APOLLO3P)
    .ui8WriteLatency      = 0,
    .bEnWriteLatency      = false,
    .bEmulateDDR          = false,
    .ui16DMATimeLimit     = 0,
    .eDMABoundary         = AM_HAL_MSPI_BOUNDARY_NONE,
#endif
#if defined(AM_PART_APOLLO4) || defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
    .ui8WriteLatency      = 0,
    .bEnWriteLatency      = false,
    .bEmulateDDR          = false,
    .ui16DMATimeLimit     = 0,
    .eDMABoundary         = AM_HAL_MSPI_BOUNDARY_NONE,
#if defined(AM_PART_APOLLO4)
    .eDeviceNum           = AM_HAL_MSPI_DEVICE0,
#endif
#else
    .ui32TCBSize          = 0,
    .pTCB                 = NULL,
    .scramblingStartAddr  = 0,
    .scramblingEndAddr    = 0,
#endif

#if defined(AM_PART_APOLLO4) || defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
    .ui16ReadInstr         = AM_DEVICES_MSPI_RM69330_FAST_READ,
    .ui16WriteInstr        = AM_DEVICES_MSPI_RM69330_PIXEL_WRITE_ADDR4,
#else
    .ui8ReadInstr         = AM_DEVICES_MSPI_RM69330_FAST_READ,
    .ui8WriteInstr        = AM_DEVICES_MSPI_RM69330_PIXEL_WRITE_ADDR4,
#endif
};

static am_hal_mspi_dev_config_t  QuadCE1DisplayMSPICfg =
{
    .eSpiMode             = AM_HAL_MSPI_SPI_MODE_0,
    .eClockFreq           = AM_HAL_MSPI_CLK_48MHZ,
    .ui8TurnAround        = 0,
    .eAddrCfg             = AM_HAL_MSPI_ADDR_3_BYTE,
    .eInstrCfg            = AM_HAL_MSPI_INSTR_1_BYTE,
    .eDeviceConfig        = AM_HAL_MSPI_FLASH_QUAD_CE1_1_4_4,
    .bSendInstr           = true,
    .bSendAddr            = true,
    .bTurnaround          = false,
#if defined(AM_PART_APOLLO3P)
    .ui8WriteLatency      = 0,
    .bEnWriteLatency      = false,
    .bEmulateDDR          = false,
    .ui16DMATimeLimit     = 0,
    .eDMABoundary         = AM_HAL_MSPI_BOUNDARY_NONE,
#endif
#if defined (AM_PART_APOLLO3) || defined (AM_PART_APOLLO3P)
    .ui32TCBSize          = 0,
    .pTCB                 = NULL,
    .scramblingStartAddr  = 0,
    .scramblingEndAddr    = 0,
#endif
#if defined(AM_PART_APOLLO4) || defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
    .ui16ReadInstr         = AM_DEVICES_MSPI_RM69330_FAST_READ,
    .ui16WriteInstr        = AM_DEVICES_MSPI_RM69330_PIXEL_WRITE_ADDR4,
#else
    .ui8ReadInstr         = AM_DEVICES_MSPI_RM69330_FAST_READ,
    .ui8WriteInstr        = AM_DEVICES_MSPI_RM69330_PIXEL_WRITE_ADDR4,
#endif
};

am_devices_mspi_rm69330_t gAmDisplay[AM_DEVICES_MSPI_RM69330_MAX_DEVICE_NUM];

am_hal_mspi_clock_e g_MaxReadFreq = AM_HAL_MSPI_CLK_8MHZ;
//*****************************************************************************
//
//! @brief
//! @param pCallbackCtxt
//! @param status
//
//*****************************************************************************
void
pfnMSPI_RM69330_Callback(void *pCallbackCtxt, uint32_t status)
{
    //
    // Set the DMA complete flag.
    //
    *(volatile uint32_t *)pCallbackCtxt = status;
}
//*****************************************************************************
//
//! @brief
//! @param pHandle
//! @param ui32Instr
//! @param pData
//! @param ui32NumBytes
//! @return
//
//*****************************************************************************
static uint32_t
am_devices_mspi_rm69330_command_write(void *pHandle, uint32_t ui32Instr,
                                      uint8_t *pData,
                                      uint32_t ui32NumBytes)
{
    am_hal_mspi_pio_transfer_t      Transaction;
    am_devices_mspi_rm69330_t       *pDisplay = (am_devices_mspi_rm69330_t *)pHandle;
    uint32_t ui32DeviceConfig = (pDisplay->mspiDevCfg.eDeviceConfig % 2) ? AM_HAL_MSPI_FLASH_SERIAL_CE1 : AM_HAL_MSPI_FLASH_SERIAL_CE0;
    bool bNeedSwitch = false;
    uint32_t        ui32Status = AM_DEVICES_MSPI_RM69330_STATUS_SUCCESS;

    if ( pDisplay->mspiDevCfg.eDeviceConfig > AM_HAL_MSPI_FLASH_SERIAL_CE1 )
    {
        bNeedSwitch = true;
    }

    if ( (!pDisplay->bOccupied) || (ui32NumBytes > 4) )
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR; // display has not been initialized. // too many bytes
    }
    //
    // Switch to Cmd configuration.
    //
    if ( bNeedSwitch )
    {
        am_hal_mspi_control(pDisplay->pMspiHandle, AM_HAL_MSPI_REQ_DEVICE_CONFIG, &ui32DeviceConfig);
    }
    //am_hal_mspi_control(pDisplay->pMspiHandle, AM_HAL_MSPI_REQ_LITTLE_ENDIAN, NULL);
    //
    // Create the individual write transaction.
    //
    Transaction.ui32NumBytes       = ui32NumBytes;
    Transaction.bScrambling        = false;
    Transaction.eDirection         = AM_HAL_MSPI_TX;
    Transaction.bSendAddr          = true;
    Transaction.ui32DeviceAddr     = ui32Instr << 8;
    Transaction.bSendInstr         = true;
    Transaction.ui16DeviceInstr    = AM_DEVICES_MSPI_RM69330_CMD_WRITE;
    Transaction.bTurnaround        = false;

#if defined(AM_PART_APOLLO3P)
    Transaction.bDCX                    = false;
    Transaction.bEnWRLatency            = false;
    Transaction.bContinue               = false;    // MSPI CONT is deprecated for Apollo3
#endif
#if !defined(AM_PART_APOLLO4) && !defined(AM_PART_APOLLO4B) && !defined(AM_PART_APOLLO4P) && !defined(AM_PART_APOLLO4L)
    Transaction.bQuadCmd                = false;
#else
    Transaction.bDCX                    = false;
    Transaction.bEnWRLatency            = false;
    Transaction.bContinue               = false;    // MSPI CONT is deprecated for Apollo4
#if defined(AM_PART_APOLLO4)
    Transaction.eDeviceNum              = AM_HAL_MSPI_DEVICE0;
#endif
#endif

    Transaction.pui32Buffer        = (uint32_t*)pData;
    //
    // Execute the transaction over MSPI.
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_blocking_transfer(pDisplay->pMspiHandle, &Transaction,
                                         AM_DEVICES_MSPI_RM69330_TIMEOUT))
    {
        am_util_stdio_printf("Error - Failed to send command.\n");
        ui32Status = AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }
    //am_hal_mspi_control(pDisplay->pMspiHandle, AM_HAL_MSPI_REQ_BIG_ENDIAN, NULL);
    //
    // Switch to Device configuration.
    //
    if ( bNeedSwitch )
    {
        //
        // Re-Configure the MSPI for the requested operation mode.
        //
#if !defined(AM_PART_APOLLO4) && !defined(AM_PART_APOLLO4B) && !defined(AM_PART_APOLLO4P) && !defined(AM_PART_APOLLO4L)
        am_hal_mspi_control(pDisplay->pMspiHandle, AM_HAL_MSPI_REQ_DEVICE_CONFIG, &pDisplay->mspiDevCfg.eDeviceConfig);
#else
        //
        // Re-Configure the MSPI for the requested operation mode.
        //
        ui32Status = am_hal_mspi_device_configure(pDisplay->pMspiHandle, &pDisplay->mspiDevCfg);
        if (AM_HAL_STATUS_SUCCESS != ui32Status)
        {
            return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
        }

        //
        // Re-Enable MSPI
        //
        ui32Status = am_hal_mspi_enable(pDisplay->pMspiHandle);
        if (AM_HAL_STATUS_SUCCESS != ui32Status)
        {
            return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
        }

        //
        // Configure the MSPI pins.
        //
#ifdef RM69330_CLK_ON_D4_EN
        am_bsp_mspi_clkond4_pins_enable(pDisplay->ui32Module, pDisplay->mspiDevCfg.eDeviceConfig);
#else
        am_bsp_mspi_pins_enable(pDisplay->ui32Module, pDisplay->mspiDevCfg.eDeviceConfig);
#endif
#endif
    }

    return ui32Status;
}

//*****************************************************************************
//
//! @brief
//! @param pHandle
//! @param ui32Instr
//! @param pData
//! @param ui32NumBytes
//! @return
//
//*****************************************************************************
static uint32_t
am_devices_mspi_rm69330_command_read(void *pHandle, uint32_t ui32Instr,
                                     uint32_t *pData,
                                     uint32_t ui32NumBytes)
{
    am_hal_mspi_pio_transfer_t      Transaction;
    am_devices_mspi_rm69330_t       *pDisplay = (am_devices_mspi_rm69330_t *)pHandle;
    // Note: the read operation can work in both 4 and 3 wires mode, it depends on the hardware connection,
    // here is the example for 4 wires connection, for people who use 3 wires connection, the micros below need
    // to be changed to AM_HAL_MSPI_FLASH_SERIAL_CE1_3WIRE and AM_HAL_MSPI_FLASH_SERIAL_CE0_3WIRE, and the
    // bNeedSwitch condition need to be modified accordingly.
    uint32_t ui32DeviceConfig = (pDisplay->mspiDevCfg.eDeviceConfig % 2) ? AM_HAL_MSPI_FLASH_SERIAL_CE1 : AM_HAL_MSPI_FLASH_SERIAL_CE0;
    bool bNeedSwitch = false;
    uint32_t        ui32Status = AM_DEVICES_MSPI_RM69330_STATUS_SUCCESS;

    if ( pDisplay->mspiDevCfg.eDeviceConfig > AM_HAL_MSPI_FLASH_SERIAL_CE1 )
    {
        bNeedSwitch = true;
    }

    if (!pDisplay->bOccupied)
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR; // display has not been initialized.
    }
    //am_hal_mspi_control(pDisplay->pMspiHandle, AM_HAL_MSPI_REQ_LITTLE_ENDIAN, NULL);
    //
    // Switch to Cmd configuration.
    //
    if ( bNeedSwitch )
    {
        am_hal_mspi_control(pDisplay->pMspiHandle, AM_HAL_MSPI_REQ_DEVICE_CONFIG, &ui32DeviceConfig);
    }
    //
    // Only support up to 10Mhz read
    //
    am_hal_mspi_control(pDisplay->pMspiHandle, AM_HAL_MSPI_REQ_CLOCK_CONFIG, &g_MaxReadFreq);
    //
    // Create the individual write transaction.
    //
    Transaction.ui32NumBytes       = ui32NumBytes;
    Transaction.bScrambling        = false;
    Transaction.eDirection         = AM_HAL_MSPI_RX;
    Transaction.bSendAddr          = true;
    Transaction.ui32DeviceAddr     = ui32Instr << 8;
    Transaction.bSendInstr         = true;
    Transaction.ui16DeviceInstr    = AM_DEVICES_MSPI_RM69330_CMD_READ;
    Transaction.bTurnaround        = false;
#if defined (AM_PART_APOLLO3) || defined (AM_PART_APOLLO3P)
    Transaction.bQuadCmd           = false;
#endif
#if defined(AM_PART_APOLLO3P)
    Transaction.bDCX                    = false;
    Transaction.bEnWRLatency            = false;
    Transaction.bContinue               = false;    // MSPI CONT is deprecated for Apollo3
#endif
#if !defined(AM_PART_APOLLO4) && !defined(AM_PART_APOLLO4B) && !defined(AM_PART_APOLLO4P) && !defined(AM_PART_APOLLO4L)
    Transaction.bQuadCmd                = false;
#else
    Transaction.bDCX                    = false;
    Transaction.bEnWRLatency            = false;
    Transaction.bContinue               = false;    // MSPI CONT is deprecated for Apollo4
#if defined(AM_PART_APOLLO4)
    Transaction.eDeviceNum              = AM_HAL_MSPI_DEVICE0;
#endif
#endif
    Transaction.pui32Buffer           = pData;
    //
    // Execute the transaction over MSPI.
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_blocking_transfer(pDisplay->pMspiHandle, &Transaction,
                                       AM_DEVICES_MSPI_RM69330_TIMEOUT))
    {
        am_util_stdio_printf("Error - Failed to send command.\n");
        ui32Status = AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }
    //am_hal_mspi_control(pDisplay->pMspiHandle, AM_HAL_MSPI_REQ_BIG_ENDIAN, NULL);
    //
    // Switch to Device configuration.
    //
#if !defined(AM_PART_APOLLO4) && !defined(AM_PART_APOLLO4B) && !defined(AM_PART_APOLLO4P) && !defined(AM_PART_APOLLO4L)
    am_hal_mspi_control(pDisplay->pMspiHandle, AM_HAL_MSPI_REQ_CLOCK_CONFIG, &pDisplay->mspiDevCfg.eClockFreq);
#endif
    if ( bNeedSwitch )
    {
        //
        // Re-Configure the MSPI for the requested operation mode.
        //
#if !defined(AM_PART_APOLLO4) && !defined(AM_PART_APOLLO4B) && !defined(AM_PART_APOLLO4P) && !defined(AM_PART_APOLLO4L)
        am_hal_mspi_control(pDisplay->pMspiHandle, AM_HAL_MSPI_REQ_DEVICE_CONFIG, &pDisplay->mspiDevCfg.eDeviceConfig);
#else
        //
        // Re-Configure the MSPI for the requested operation mode.
        //
        ui32Status = am_hal_mspi_device_configure(pDisplay->pMspiHandle, &pDisplay->mspiDevCfg);
        if (AM_HAL_STATUS_SUCCESS != ui32Status)
        {
            return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
        }

        //
        // Re-Enable MSPI
        //
        ui32Status = am_hal_mspi_enable(pDisplay->pMspiHandle);
        if (AM_HAL_STATUS_SUCCESS != ui32Status)
        {
            return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
        }

        //
        // Configure the MSPI pins.
        //
#ifdef RM69330_CLK_ON_D4_EN
        am_bsp_mspi_clkond4_pins_enable(pDisplay->ui32Module, pDisplay->mspiDevCfg.eDeviceConfig);
#else
        am_bsp_mspi_pins_enable(pDisplay->ui32Module, pDisplay->mspiDevCfg.eDeviceConfig);
#endif
#endif
    }

    return ui32Status;
}

//*****************************************************************************
//
//! @brief reset external display
//!
//! @param None
//!
//! This function reset the display device.
//!
//! @return None
//
//*****************************************************************************
static void
am_devices_mspi_rm69330_reset(void)
{
#if defined (AM_PART_APOLLO3) || defined (AM_PART_APOLLO3P)
    am_hal_gpio_output_clear(AM_BSP_GPIO_DSPL_RESET);  //AMOLED_RESET
    am_util_delay_ms(20);
    am_hal_gpio_output_set(AM_BSP_GPIO_DSPL_RESET);
#elif defined (AM_PART_APOLLO4) || defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P)
    am_hal_gpio_output_clear(AM_BSP_GPIO_DISP_QSPI_RES);  //AMOLED_RESET
    am_util_delay_ms(20);
    am_hal_gpio_output_set(AM_BSP_GPIO_DISP_QSPI_RES);
#elif defined(AM_PART_APOLLO4L)
    am_hal_gpio_output_clear(AM_BSP_GPIO_DISP_DEVICE_RST);  //AMOLED_RESET
    am_util_delay_ms(20);
    am_hal_gpio_output_set(AM_BSP_GPIO_DISP_DEVICE_RST);
#endif

    am_util_delay_ms(150);  //Delay 150ms
}
//*****************************************************************************
//
//! @brief
//! @param pHandle
//! @param psGraphic_conf
//! @return
//
//*****************************************************************************
static uint32_t
am_devices_set_row_col(void *pHandle, am_devices_mspi_rm69330_graphic_conf_t *psGraphic_conf)
{
    uint8_t data[4] = {0};

    gs_display_info.row_start = psGraphic_conf->row_offset;
    gs_display_info.row_end = psGraphic_conf->max_row + psGraphic_conf->row_offset - 1;
    gs_display_info.col_start = psGraphic_conf->col_offset;
    gs_display_info.col_end = psGraphic_conf->max_col + psGraphic_conf->col_offset - 1;
    //
    // set column start address
    //
    data[0] = (gs_display_info.col_start / 256);
    data[1] = (gs_display_info.col_start % 256);
    data[2] = (gs_display_info.col_end / 256);
    data[3] = (gs_display_info.col_end % 256);
    if (am_devices_mspi_rm69330_command_write(pHandle, AM_DEVICES_MSPI_RM69330_SET_COLUMN, data, 4))//Column
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }
    //
    // set row start address
    //
    data[0] = (gs_display_info.row_start / 256);
    data[1] = (gs_display_info.row_start % 256);
    data[2] = (gs_display_info.row_end / 256);
    data[3] = (gs_display_info.row_end % 256);
    if (am_devices_mspi_rm69330_command_write(pHandle, AM_DEVICES_MSPI_RM69330_SET_ROW, data, 4))//row
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

    return AM_DEVICES_MSPI_RM69330_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief
//! @param pHandle
//! @param psGraphic_conf
//! @return
//
//*****************************************************************************
static uint32_t
am_devices_lcm_init(void *pHandle, am_devices_mspi_rm69330_graphic_conf_t *psGraphic_conf)
{
    uint8_t cmd_buf[10];
    am_devices_mspi_rm69330_reset();

    cmd_buf[0] = 0x00;      //switch to User Commands Sets(UCS = CMD1)
    if ( am_devices_mspi_rm69330_command_write(pHandle, AM_DEVICES_MSPI_RM69330_CMD_MODE, cmd_buf, 1) )  // set page CMD1
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

    cmd_buf[0] = psGraphic_conf->bus_mode;
    if ( am_devices_mspi_rm69330_command_write(pHandle, AM_DEVICES_MSPI_RM69330_SET_DSPI_MODE, cmd_buf, 1) )  // spi ram enable default is mipi,Aaron modified
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

    if ( am_devices_mspi_rm69330_command_write(pHandle, AM_DEVICES_MSPI_RM69330_IDLE_MODE_OFF, NULL, 0) )  // idle mode off
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

    am_util_delay_ms(120);

    if ( am_devices_mspi_rm69330_command_write(pHandle, AM_DEVICES_MSPI_RM69330_DISPLAY_OFF, NULL, 0) )  // display off
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

    cmd_buf[0] = psGraphic_conf->scan_mode;
    if ( am_devices_mspi_rm69330_command_write(pHandle, AM_DEVICES_MSPI_RM69330_SCAN_DIRECTION, cmd_buf, 1) )  // scan direction to 0
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

    cmd_buf[0] = psGraphic_conf->color_mode;
    if ( am_devices_mspi_rm69330_command_write(pHandle, AM_DEVICES_MSPI_RM69330_PIXEL_FORMAT, cmd_buf, 1) )
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

    cmd_buf[0] = 0x00;      // TE on , only V-blanking
    if ( am_devices_mspi_rm69330_command_write(pHandle, AM_DEVICES_MSPI_RM69330_TE_LINE_ON, cmd_buf, 1) )
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

    cmd_buf[0] = 0xff;      // write display brightness
    if ( am_devices_mspi_rm69330_command_write(pHandle, AM_DEVICES_MSPI_RM69330_WRITE_DISPLAY_BRIGHTNESS, cmd_buf, 1) )
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

    am_util_delay_ms(10);

    if ( am_devices_mspi_rm69330_command_write(pHandle, AM_DEVICES_MSPI_RM69330_SLEEP_OUT, NULL, 0) )  // sleep out
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

    am_util_delay_ms(10);

    am_devices_set_row_col(pHandle, psGraphic_conf);

    am_util_delay_ms(200);

    if ( am_devices_mspi_rm69330_command_write(pHandle, AM_DEVICES_MSPI_RM69330_NORMAL_MODE_ON, NULL, 0) )  // normal display on
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

//    if ( am_devices_mspi_rm69330_command_write(pHandle, AM_DEVICES_MSPI_RM69330_IDLE_MODE_OFF, NULL, 0) )  // idle mode off
//    {
//        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
//    }
//
//    if ( am_devices_mspi_rm69330_command_write(pHandle, AM_DEVICES_MSPI_RM69330_SLEEP_OUT, NULL, 0) )  // sleep out
//    {
//        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
//    }

    am_util_delay_ms(10);

    return AM_DEVICES_MSPI_RM69330_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Initialize the MSPI_RM69330 driver.
//
//*****************************************************************************
uint32_t
am_devices_mspi_rm69330_init(uint32_t ui32Module,
                             am_devices_mspi_rm69330_config_t *pDevCfg,
                             void **ppHandle,
                             void **ppMspiHandle)
{
    uint32_t        ui32Status;
    uint32_t        ui32DeviceID;
    uint32_t        ui32Index;
    void            *pMspiHandle;

    if ((ui32Module > AM_REG_MSPI_NUM_MODULES) || (pDevCfg == NULL))
    {
      return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

    //
    // Enable fault detection.
    //
#if AM_APOLLO3_MCUCTRL
#else // AM_APOLLO3_MCUCTRL
    am_hal_mcuctrl_fault_capture_enable();
#endif // AM_APOLLO3_MCUCTRL

    // Allocate a vacant device handle
    for ( ui32Index = 0; ui32Index < AM_DEVICES_MSPI_RM69330_MAX_DEVICE_NUM; ui32Index++ )
    {
        if ( gAmDisplay[ui32Index].bOccupied == false )
        {
            break;
        }
    }
    if ( ui32Index == AM_DEVICES_MSPI_RM69330_MAX_DEVICE_NUM )
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

    //
    // Enable fault detection.
    //
    // am_hal_fault_capture_enable();

    switch (pDevCfg->eDeviceConfig)
    {
        case AM_HAL_MSPI_FLASH_SERIAL_CE0:
            gAmDisplay[ui32Index].mspiDevCfg = SerialCE0DisplayMSPICfg;
            break;
        case AM_HAL_MSPI_FLASH_QUAD_CE0_1_1_4:
        case AM_HAL_MSPI_FLASH_QUAD_CE0_1_4_4:
            gAmDisplay[ui32Index].mspiDevCfg = QuadCE0DisplayMSPICfg;
            break;
        case AM_HAL_MSPI_FLASH_SERIAL_CE1:
            gAmDisplay[ui32Index].mspiDevCfg = SerialCE1DisplayMSPICfg;
            break;
        case AM_HAL_MSPI_FLASH_QUAD_CE1_1_1_4:
        case AM_HAL_MSPI_FLASH_QUAD_CE1_1_4_4:
            gAmDisplay[ui32Index].mspiDevCfg = QuadCE1DisplayMSPICfg;
            break;
        default:
            return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }
    gAmDisplay[ui32Index].mspiDevCfg.eClockFreq = pDevCfg->eClockFreq;
#if defined (AM_PART_APOLLO3) || defined (AM_PART_APOLLO3P)
    gAmDisplay[ui32Index].mspiDevCfg.ui32TCBSize = pDevCfg->ui32NBTxnBufLength;
    gAmDisplay[ui32Index].mspiDevCfg.pTCB = pDevCfg->pNBTxnBuf;
#endif
    //
    // Configure the MSPI for Serial operation during initialization.
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_initialize(ui32Module, &pMspiHandle))
    {
        am_util_stdio_printf("Error - Failed to initialize MSPI.\n");
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

    if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_power_control(pMspiHandle, AM_HAL_SYSCTRL_WAKE, false))
    {
        am_util_stdio_printf("Error - Failed to power on MSPI.\n");
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }
#if defined(AM_PART_APOLLO4) || defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
    {
        am_hal_mspi_config_t    mspiCfg = gGDMspiCfg;
        mspiCfg.ui32TCBSize = pDevCfg->ui32NBTxnBufLength;
        mspiCfg.pTCB = pDevCfg->pNBTxnBuf;
        if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_configure(pMspiHandle, &mspiCfg))
        {
            am_util_stdio_printf("Error - Failed to configure MSPI device.\n");
            return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
        }
    }
#endif
    if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_device_configure(pMspiHandle, &gAmDisplay[ui32Index].mspiDevCfg))
    {
        am_util_stdio_printf("Error - Failed to configure MSPI.\n");
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }
    if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_enable(pMspiHandle))
    {
        am_util_stdio_printf("Error - Failed to enable MSPI.\n");
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

#ifdef RM69330_CLK_ON_D4_EN
    am_bsp_mspi_clkond4_pins_enable(ui32Module, pDevCfg->eDeviceConfig);
#else
    am_bsp_mspi_pins_enable(ui32Module, pDevCfg->eDeviceConfig);
#endif

    gAmDisplay[ui32Index].pMspiHandle = pMspiHandle;
    gAmDisplay[ui32Index].ui32Module = ui32Module;
    gAmDisplay[ui32Index].bOccupied = true;

    am_devices_lcm_init((void*)&gAmDisplay[ui32Index], &g_sGraphic_conf);

#if defined(ADJ_TE_FREQ)
    //
    // Adjust TE frequency of display.
    //
    const int MIPI_set_cmd_page = 0xFE;
    uint8_t ui8CmdBuf[2];

    ui8CmdBuf[0] = MIPI_set_cmd_page;
    ui8CmdBuf[1] = 0x01; // MCS

    ui32Status = am_devices_mspi_rm69330_command_write((void*)&gAmDisplay[ui32Index], ui8CmdBuf[0], &ui8CmdBuf[1], 1);
    if (ui32Status)
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

    am_util_delay_ms(10);
    ui8CmdBuf[0] = 0x29;
    ui8CmdBuf[1] = 0x41;  //30fps
    ui32Status = am_devices_mspi_rm69330_command_write((void*)&gAmDisplay[ui32Index], ui8CmdBuf[0], &ui8CmdBuf[1], 1);
    if (ui32Status)
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

    am_util_delay_ms(10);
    ui8CmdBuf[0] = MIPI_set_cmd_page;
    ui8CmdBuf[1] = 0x00; // UCS
    ui32Status = am_devices_mspi_rm69330_command_write((void*)&gAmDisplay[ui32Index], ui8CmdBuf[0], &ui8CmdBuf[1], 1);
    if (ui32Status)
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }
    am_util_delay_ms(10);
#endif
    //
    // Read the Device ID.
    //
    ui32Status = am_devices_mspi_rm69330_read_id((void*)&gAmDisplay[ui32Index], &ui32DeviceID);
    am_util_stdio_printf("RM69330 Device ID = %6X\n", (ui32DeviceID & 0x00FFFFFF));
//    if (AM_HAL_STATUS_SUCCESS != ui32Status)
//    {
//        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
//    }
    //
    // Enable MSPI interrupts.
    //
    ui32Status = am_hal_mspi_interrupt_clear(pMspiHandle, AM_HAL_MSPI_INT_CQUPD | AM_HAL_MSPI_INT_ERR );
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

    ui32Status = am_hal_mspi_interrupt_enable(pMspiHandle, AM_HAL_MSPI_INT_CQUPD | AM_HAL_MSPI_INT_ERR );
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

    //
    // Return the handle.
    //
    *ppHandle = (void *)&gAmDisplay[ui32Index];
    *ppMspiHandle = pMspiHandle;

    //
    // Return the status.
    //
    return AM_DEVICES_MSPI_RM69330_STATUS_SUCCESS;
}

//*****************************************************************************
//
// De-Initialize the mspi_rm69330 driver.
//
//*****************************************************************************
uint32_t
am_devices_mspi_rm69330_term(void *pHandle)
{
    am_devices_mspi_rm69330_t   *pDisplay = (am_devices_mspi_rm69330_t *)pHandle;
    uint32_t                    ui32Status;

    if ( pDisplay->ui32Module > AM_REG_IOM_NUM_MODULES )
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

    //
    // Disable and clear the interrupts to start with.
    //
    ui32Status = am_hal_mspi_interrupt_disable(pDisplay->pMspiHandle, 0xFFFFFFFF);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }
    ui32Status = am_hal_mspi_interrupt_clear(pDisplay->pMspiHandle, 0xFFFFFFFF);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

    //
    // Disable the MSPI.
    //
    am_hal_mspi_disable(pDisplay->pMspiHandle);

    //
    // Disable power to and uninitialize the MSPI instance.
    //
    am_hal_mspi_power_control(pDisplay->pMspiHandle, AM_HAL_SYSCTRL_DEEPSLEEP, false);

    am_hal_mspi_deinitialize(pDisplay->pMspiHandle);

    // Free this device handle
    pDisplay->bOccupied = false;

    //
    // Return the status.
    //
    return AM_DEVICES_MSPI_RM69330_STATUS_SUCCESS;
}


//*****************************************************************************
//
//
//*****************************************************************************
uint32_t
am_devices_mspi_rm69330_display_off(void *pHandle)
{
    if ( am_devices_mspi_rm69330_command_write(pHandle, AM_DEVICES_MSPI_RM69330_DISPLAY_OFF, NULL, 0) )
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

    if ( am_devices_mspi_rm69330_command_write(pHandle, AM_DEVICES_MSPI_RM69330_SLEEP_IN, NULL, 0) )
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

    return AM_DEVICES_MSPI_RM69330_STATUS_SUCCESS;
}

//*****************************************************************************
//
//
//*****************************************************************************
uint32_t
am_devices_mspi_rm69330_display_on(void *pHandle)
{
    if ( am_devices_mspi_rm69330_command_write(pHandle, AM_DEVICES_MSPI_RM69330_DISPLAY_OFF, NULL, 0) )
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

    if (am_devices_mspi_rm69330_command_write(pHandle, AM_DEVICES_MSPI_RM69330_DISPLAY_ON, NULL, 0))
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

    return AM_DEVICES_MSPI_RM69330_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Programs the given range of display addresses.
//
//*****************************************************************************
uint32_t
am_devices_mspi_rm69330_nonblocking_write(void *pHandle, const uint8_t *pui8TxBuffer,
                                          uint32_t ui32NumBytes,
                                          bool bWaitForCompletion)
{
    am_hal_mspi_dma_transfer_t    Transaction;
    am_devices_mspi_rm69330_t *pDisplay = (am_devices_mspi_rm69330_t *)pHandle;
    uint32_t                      ui32Status;
    uint32_t                      ui32BytesLeft = ui32NumBytes;
    volatile bool bDMAComplete = false;

    while (ui32BytesLeft)
    {
        if ( ui32BytesLeft == ui32NumBytes )
        {
            Transaction.ui32DeviceAddress = AM_DEVICES_MSPI_RM69330_MEM_WRITE << 8;
        }
        else
        {
            Transaction.ui32DeviceAddress = AM_DEVICES_MSPI_RM69330_MEM_WRITE_CONTINUE << 8;
        }
        // Set the DMA priority
        Transaction.ui8Priority = 1;

        // Set the transfer direction to TX (Write)
        Transaction.eDirection = AM_HAL_MSPI_TX;

        Transaction.ui32TransferCount = (ui32BytesLeft > BYTE_NUM_PER_WRITE) ? BYTE_NUM_PER_WRITE : ui32BytesLeft;

        // Set the source SRAM buffer address.
        Transaction.ui32SRAMAddress = (uint32_t)pui8TxBuffer;

        // Clear the CQ stimulus.
        Transaction.ui32PauseCondition = 0;
        // Clear the post-processing
        Transaction.ui32StatusSetClr = 0;
#if defined(AM_PART_APOLLO4)
        Transaction.eDeviceNum         = AM_HAL_MSPI_DEVICE0;
#endif

       // Start the transaction.
        volatile uint32_t ui32DMAStatus = 0xFFFFFFFF;
        ui32Status = am_hal_mspi_nonblocking_transfer(pDisplay->pMspiHandle, &Transaction, AM_HAL_MSPI_TRANS_DMA,
                                                      pfnMSPI_RM69330_Callback, (void *)&ui32DMAStatus);

        // Check the transaction status.
        if (AM_HAL_STATUS_SUCCESS != ui32Status)
        {
            return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
        }



        if (bWaitForCompletion)
        {
          //
          // Wait for DMA Complete or Timeout
          //
          for (uint32_t i = 0; i < AM_DEVICES_MSPI_RM69330_TIMEOUT; i++)
          {
              if (AM_HAL_STATUS_SUCCESS == ui32DMAStatus)
              {
                  break;
              }
              //
              // Call the BOOTROM cycle function to delay for about 1 microsecond.
              //
              am_util_delay_us(1);
          }
          // Check the status.
          if (AM_HAL_STATUS_SUCCESS != ui32DMAStatus)
          {
              return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
          }
        }
        ui32BytesLeft -= Transaction.ui32TransferCount;
        pui8TxBuffer += Transaction.ui32TransferCount;
    }

    //
    // Return the status.
    //
    return AM_DEVICES_MSPI_RM69330_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Programs the given range of display addresses.
//
//*****************************************************************************
uint32_t
am_devices_mspi_rm69330_nonblocking_write_adv(void *pHandle,
                                              uint8_t *pui8TxBuffer,
                                              uint32_t ui32NumBytes,
                                              uint32_t ui32PauseCondition,
                                              uint32_t ui32StatusSetClr,
                                              am_hal_mspi_callback_t pfnCallback,
                                              void *pCallbackCtxt)
{
  am_hal_mspi_dma_transfer_t Transaction;
  am_devices_mspi_rm69330_t *pDisplay = (am_devices_mspi_rm69330_t *)pHandle;
  uint32_t      ui32BytesLeft = ui32NumBytes;

  //
  // Create the transaction.
  //
  Transaction.ui8Priority               = 1;
  Transaction.eDirection                = AM_HAL_MSPI_TX;
  Transaction.ui32TransferCount         = (ui32BytesLeft <= AM_HAL_MSPI_MAX_TRANS_SIZE) ? ui32BytesLeft : AM_HAL_MSPI_MAX_TRANS_SIZE;
  Transaction.ui32DeviceAddress         = AM_DEVICES_MSPI_RM69330_MEM_WRITE << 8;
  Transaction.ui32SRAMAddress           = (uint32_t)pui8TxBuffer;
  Transaction.ui32PauseCondition        = ui32PauseCondition;
  Transaction.ui32StatusSetClr          = ui32StatusSetClr;

  //
  // Execute the transction over MSPI.
  //
  if (am_hal_mspi_nonblocking_transfer(pDisplay->pMspiHandle,
                                       &Transaction,
                                       AM_HAL_MSPI_TRANS_DMA,
                                       (ui32BytesLeft <= AM_HAL_MSPI_MAX_TRANS_SIZE) ? pfnCallback : NULL,
                                       (ui32BytesLeft <= AM_HAL_MSPI_MAX_TRANS_SIZE) ? pCallbackCtxt : NULL))
  {
    return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
  }

  Transaction.ui32DeviceAddress         = AM_DEVICES_MSPI_RM69330_MEM_WRITE_CONTINUE << 8;
  for (int32_t block = 0; block < (ui32NumBytes / AM_HAL_MSPI_MAX_TRANS_SIZE); block++)
  {

    Transaction.ui32SRAMAddress += Transaction.ui32TransferCount;
    ui32BytesLeft -=  Transaction.ui32TransferCount;
    Transaction.ui32TransferCount         = (ui32BytesLeft <= AM_HAL_MSPI_MAX_TRANS_SIZE) ? ui32BytesLeft : AM_HAL_MSPI_MAX_TRANS_SIZE;
    if (am_hal_mspi_nonblocking_transfer(pDisplay->pMspiHandle,
                                         &Transaction,
                                         AM_HAL_MSPI_TRANS_DMA,
                                         (ui32BytesLeft <= AM_HAL_MSPI_MAX_TRANS_SIZE) ? pfnCallback : NULL,
                                         (ui32BytesLeft <= AM_HAL_MSPI_MAX_TRANS_SIZE) ? pCallbackCtxt : NULL))
    {
      return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

  }

  return AM_DEVICES_MSPI_RM69330_STATUS_SUCCESS;
}

//*****************************************************************************
//
//
//*****************************************************************************
uint32_t
am_devices_mspi_rm69330_row_col_reset(void *pHandle)
{
    uint32_t ui32Status = am_devices_set_row_col(pHandle, &g_sGraphic_conf);
    if (AM_DEVICES_MSPI_RM69330_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }
    return AM_DEVICES_MSPI_RM69330_STATUS_SUCCESS;
}
//*****************************************************************************
//
//
//*****************************************************************************
uint32_t
am_devices_mspi_rm69330_read_id(void *pHandle, uint32_t *pdata)
{
    if (am_devices_mspi_rm69330_command_read(pHandle, AM_DEVICES_MSPI_RM69330_READ_ID, pdata, 3))
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }
    return AM_DEVICES_MSPI_RM69330_STATUS_SUCCESS;
}

//*****************************************************************************
//
//
//*****************************************************************************
uint32_t
am_devices_mspi_rm69330_set_transfer_window(void *pHandle, uint16_t col_start, uint16_t col_end,
                                            uint16_t row_start, uint16_t row_end)
{
    uint8_t cmd_buf[4];
    cmd_buf[0] = (col_start / 256);
    cmd_buf[1] = (col_start % 256);
    cmd_buf[2] = (col_end / 256);
    cmd_buf[3] = (col_end % 256);
    if ( am_devices_mspi_rm69330_command_write(pHandle, AM_DEVICES_MSPI_RM69330_SET_COLUMN, cmd_buf, 4) )
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }

    am_util_delay_us(10);

    cmd_buf[0] = (row_start / 256);
    cmd_buf[1] = (row_start % 256);
    cmd_buf[2] = (row_end / 256);
    cmd_buf[3] = (row_end % 256);
    if ( am_devices_mspi_rm69330_command_write(pHandle, AM_DEVICES_MSPI_RM69330_SET_ROW, cmd_buf, 4) )
    {
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }
    return AM_DEVICES_MSPI_RM69330_STATUS_SUCCESS;
}

//*****************************************************************************
//
//
//*****************************************************************************
void
am_devices_rm69330_set_parameters(uint16_t col_start, uint16_t col_size,
                                  uint16_t row_start, uint16_t row_size, uint8_t color_format)
{
    g_sGraphic_conf.color_mode = color_format;
    g_sGraphic_conf.max_row    = row_size;
    g_sGraphic_conf.max_col    = col_size;
    g_sGraphic_conf.row_offset = row_start;
    g_sGraphic_conf.col_offset = col_start;
}

//*****************************************************************************
//
//
//*****************************************************************************
uint32_t
am_devices_rm69330_get_parameters(void *pHandle)
{
    am_devices_mspi_rm69330_graphic_conf_t *config = (am_devices_mspi_rm69330_graphic_conf_t *)pHandle;
    config->max_row = g_sGraphic_conf.max_row;
    config->max_col = g_sGraphic_conf.max_col;
    config->row_offset = g_sGraphic_conf.row_offset;
    config->col_offset = g_sGraphic_conf.col_offset;
    return AM_DEVICES_MSPI_RM69330_STATUS_SUCCESS;
}
//*****************************************************************************
//
//
//
//*****************************************************************************
uint32_t
am_devices_mspi_rm69330_read_format(void *pHandle, uint32_t *pdata)
{
    if (am_devices_mspi_rm69330_command_read(pHandle, AM_DEVICES_MSPI_RM69330_READ_PIXEL_FORMAT, pdata, 1))
    {
        *pdata = (uint32_t)(g_sGraphic_conf.color_mode);
        return AM_DEVICES_MSPI_RM69330_STATUS_ERROR;
    }
    return AM_DEVICES_MSPI_RM69330_STATUS_SUCCESS;
}

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

