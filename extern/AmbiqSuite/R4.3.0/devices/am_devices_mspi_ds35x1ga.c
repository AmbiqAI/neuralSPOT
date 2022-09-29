//*****************************************************************************
//
//! @file am_devices_mspi_ds35x1ga.c
//!
//! @brief Multibit SPI ds35x1ga NAND flash driver.
//!
//! @addtogroup mspi_ds35x1ga DX35X1GA MSPI Driver
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
#include <string.h>

#include "am_mcu_apollo.h"
#include "am_devices_mspi_ds35x1ga.h"
#include "am_util_stdio.h"
#include "am_bsp.h"
#include "am_util_delay.h"


//*****************************************************************************
//
// Global variables.
//
//*****************************************************************************
#if defined(AM_PART_APOLLO4L)
#define DS35X1GA_QUAD_CLKON4_MODE_EN
#endif

#define AM_DEVICES_MSPI_DS35X1GA_TIMEOUT       1000000
#define AM_DEVICES_MSPI_DS35X1GA_ERASE_TIMEOUT 1000000

typedef struct
{
    uint32_t ui32Module;
    void *pMspiHandle;
    am_hal_mspi_dev_config_t *pCurrentSetting;
    am_hal_mspi_dev_config_t *pSerialSetting;
    bool bOccupied;
} am_devices_mspi_ds35x1ga_t;

#if defined(AM_PART_APOLLO4_API)
am_hal_mspi_config_t gMspiCfg =
{
  .ui32TCBSize          = 0,
  .pTCB                 = NULL,
#if defined(DS35X1GA_QUAD_CLKON4_MODE_EN)
  .bClkonD4             = 1
#else
  .bClkonD4             = 0
#endif
};
#endif

static am_devices_mspi_ds35x1ga_t gAmDs35x1ga[AM_DEVICES_MSPI_DS35X1GA_MAX_DEVICE_NUM];

static am_hal_mspi_dev_config_t MSPI_DS35X1GA_Serial_CE0_MSPIConfig =
{
    .eSpiMode = AM_HAL_MSPI_SPI_MODE_0,
    .eClockFreq = AM_HAL_MSPI_CLK_4MHZ,
    .ui8TurnAround = 8,
    .eAddrCfg = AM_HAL_MSPI_ADDR_2_BYTE,
    .eInstrCfg = AM_HAL_MSPI_INSTR_1_BYTE,
    .eDeviceConfig = AM_HAL_MSPI_FLASH_SERIAL_CE0,
    .bSendInstr = true,
    .bSendAddr = true,
    .bTurnaround = true,
#if defined(AM_PART_APOLLO4_API)
    .ui16ReadInstr         = AM_DEVICES_MSPI_DS35X1GA_READ_BUFFER_X1,
    .ui16WriteInstr        = AM_DEVICES_MSPI_DS35X1GA_PROGRAM_LOAD_X1,
#else
    .ui8ReadInstr = AM_DEVICES_MSPI_DS35X1GA_READ_BUFFER_X1,
    .ui8WriteInstr = AM_DEVICES_MSPI_DS35X1GA_PROGRAM_LOAD_X1,
#endif

#if defined(AM_PART_APOLLO3P)
    .ui8WriteLatency = 0,
    .bEnWriteLatency = false,
#else
    .ui8WriteLatency = 0,
    .bEnWriteLatency = false,
#endif
    .bEmulateDDR = false,
    .ui16DMATimeLimit = 0,
    .eDMABoundary = AM_HAL_MSPI_BOUNDARY_NONE,

#if defined(AM_PART_APOLLO4_API)
#if defined(AM_PART_APOLLO4)
    .eDeviceNum           = AM_HAL_MSPI_DEVICE0,
#endif
#else
    .ui32TCBSize          = 0,
    .pTCB                 = NULL,
    .scramblingStartAddr  = 0,
    .scramblingEndAddr    = 0,
#endif
};

static am_hal_mspi_dev_config_t MSPI_DS35X1GA_Serial_CE1_MSPIConfig =
{
    .eSpiMode = AM_HAL_MSPI_SPI_MODE_0,
    .eClockFreq = AM_HAL_MSPI_CLK_4MHZ,
    .ui8TurnAround = 8,
    .eAddrCfg = AM_HAL_MSPI_ADDR_2_BYTE,
    .eInstrCfg = AM_HAL_MSPI_INSTR_1_BYTE,
    .eDeviceConfig = AM_HAL_MSPI_FLASH_SERIAL_CE1,
    .bSendInstr = true,
    .bSendAddr = true,
    .bTurnaround = true,
#if defined(AM_PART_APOLLO4_API)
    .ui16ReadInstr         = AM_DEVICES_MSPI_DS35X1GA_READ_BUFFER_X1,
    .ui16WriteInstr        = AM_DEVICES_MSPI_DS35X1GA_PROGRAM_LOAD_X1,
#else
    .ui8ReadInstr = AM_DEVICES_MSPI_DS35X1GA_READ_BUFFER_X1,
    .ui8WriteInstr = AM_DEVICES_MSPI_DS35X1GA_PROGRAM_LOAD_X1,
#endif

#if defined(AM_PART_APOLLO3P)
    .ui8WriteLatency = 0,
    .bEnWriteLatency = false,
#else
    .ui8WriteLatency = 0,
    .bEnWriteLatency = false,
#endif
    .bEmulateDDR = false,
    .ui16DMATimeLimit = 0,
    .eDMABoundary = AM_HAL_MSPI_BOUNDARY_NONE,

#if defined(AM_PART_APOLLO4_API)
#if defined(AM_PART_APOLLO4)
    .eDeviceNum           = AM_HAL_MSPI_DEVICE0,
#endif
#else
    .ui32TCBSize          = 0,
    .pTCB                 = NULL,
    .scramblingStartAddr  = 0,
    .scramblingEndAddr    = 0,
#endif
};


static am_hal_mspi_dev_config_t MSPI_DS35X1GA_Quad_CE0_MSPIConfig =
{
    .eSpiMode = AM_HAL_MSPI_SPI_MODE_0,
    .eClockFreq = AM_HAL_MSPI_CLK_4MHZ,
    .ui8TurnAround = 8,
    .eAddrCfg = AM_HAL_MSPI_ADDR_2_BYTE,
    .eInstrCfg = AM_HAL_MSPI_INSTR_1_BYTE,
    .eDeviceConfig = AM_HAL_MSPI_FLASH_QUAD_CE0_1_1_4,
    .bSendInstr = true,
    .bSendAddr = true,
    .bTurnaround = true,
#if defined(AM_PART_APOLLO4_API)
    .ui16ReadInstr         = AM_DEVICES_MSPI_DS35X1GA_READ_BUFFER_X4,
    .ui16WriteInstr        = AM_DEVICES_MSPI_DS35X1GA_PROGRAM_LOAD_X4,
#else
    .ui8ReadInstr = AM_DEVICES_MSPI_DS35X1GA_READ_BUFFER_X4,
    .ui8WriteInstr = AM_DEVICES_MSPI_DS35X1GA_PROGRAM_LOAD_X4,
#endif

#if defined(AM_PART_APOLLO3P)
    .ui8WriteLatency = 6,
    .bEnWriteLatency = true,
#else
    .ui8WriteLatency = 0,
    .bEnWriteLatency = false,
#endif
    .bEmulateDDR = false,
    .ui16DMATimeLimit = 0,
    .eDMABoundary = AM_HAL_MSPI_BOUNDARY_NONE,

#if defined(AM_PART_APOLLO4_API)
#if defined(AM_PART_APOLLO4)
    .eDeviceNum           = AM_HAL_MSPI_DEVICE0,
#endif
#else
    .ui32TCBSize          = 0,
    .pTCB                 = NULL,
    .scramblingStartAddr  = 0,
    .scramblingEndAddr    = 0,
#endif
};


static am_hal_mspi_dev_config_t MSPI_DS35X1GA_Quad_CE1_MSPIConfig =
{
    .eSpiMode = AM_HAL_MSPI_SPI_MODE_0,
    .eClockFreq = AM_HAL_MSPI_CLK_4MHZ,
    .ui8TurnAround = 8,
    .eAddrCfg = AM_HAL_MSPI_ADDR_2_BYTE,
    .eInstrCfg = AM_HAL_MSPI_INSTR_1_BYTE,
    .eDeviceConfig = AM_HAL_MSPI_FLASH_QUAD_CE1_1_1_4,
    .bSendInstr = true,
    .bSendAddr = true,
    .bTurnaround = true,
#if defined(AM_PART_APOLLO4_API)
    .ui16ReadInstr         = AM_DEVICES_MSPI_DS35X1GA_READ_BUFFER_X4,
    .ui16WriteInstr        = AM_DEVICES_MSPI_DS35X1GA_PROGRAM_LOAD_X4,
#else
    .ui8ReadInstr = AM_DEVICES_MSPI_DS35X1GA_READ_BUFFER_X4,
    .ui8WriteInstr = AM_DEVICES_MSPI_DS35X1GA_PROGRAM_LOAD_X4,
#endif

#if defined(AM_PART_APOLLO3P)
    .ui8WriteLatency = 6,
    .bEnWriteLatency = true,
#else
    .ui8WriteLatency = 0,
    .bEnWriteLatency = false,
#endif
    .bEmulateDDR = false,
    .ui16DMATimeLimit = 0,
    .eDMABoundary = AM_HAL_MSPI_BOUNDARY_NONE,

#if defined(AM_PART_APOLLO4_API)
#if defined(AM_PART_APOLLO4)
    .eDeviceNum           = AM_HAL_MSPI_DEVICE0,
#endif
#else
    .ui32TCBSize          = 0,
    .pTCB                 = NULL,
    .scramblingStartAddr  = 0,
    .scramblingEndAddr    = 0,
#endif
};


static struct
{
    am_hal_mspi_device_e eHalDeviceEnum;
    am_hal_mspi_dev_config_t *psDevConfig;
} g_DS35X1GA_DevConfig[] =
    {
        { AM_HAL_MSPI_FLASH_SERIAL_CE0, &MSPI_DS35X1GA_Serial_CE0_MSPIConfig },
        { AM_HAL_MSPI_FLASH_SERIAL_CE1, &MSPI_DS35X1GA_Serial_CE1_MSPIConfig },
        { AM_HAL_MSPI_FLASH_QUAD_CE0_1_1_4,   &MSPI_DS35X1GA_Quad_CE0_MSPIConfig   },
        { AM_HAL_MSPI_FLASH_QUAD_CE1_1_1_4,   &MSPI_DS35X1GA_Quad_CE1_MSPIConfig   },
    };

//
// Forward declarations.
//
static uint32_t am_devices_mspi_ds35x1ga_command_write(void *pHandle,
                                                       uint8_t ui8Instr,
                                                       bool bSendAddr,
                                                       uint32_t ui32Addr,
                                                       uint32_t *pData,
                                                       uint32_t ui32NumBytes);
static uint32_t am_devices_mspi_ds35x1ga_command_read(void *pHandle,
                                                      uint8_t ui8Instr,
                                                      bool bSendAddr,
                                                      uint32_t ui32Addr,
                                                      uint32_t *pData,
                                                      uint32_t ui32NumBytes);

//*****************************************************************************
//
// DS35X1GA Support
//
//*****************************************************************************

#if defined(AM_PART_APOLLO3P)
static uint8_t
ds35x1ga_cmd_addr_len(uint8_t cmd)
{
    switch ( cmd )
    {
        //
        // zero byte length of the command
        //
        case AM_DEVICES_MSPI_DS35X1GA_WRITE_DISABLE:
        case AM_DEVICES_MSPI_DS35X1GA_WRITE_ENABLE:
        case AM_DEVICES_MSPI_DS35X1GA_RESET:
        case AM_DEVICES_MSPI_DS35X1GA_READ_ID:
        return 0;

        //
        // one byte length of the command
        //
        case AM_DEVICES_MSPI_DS35X1GA_SET_FEATURE:
        case AM_DEVICES_MSPI_DS35X1GA_GET_FEATURE:
        return 1;

        //
        // two bytes length of the command
        //
        case AM_DEVICES_MSPI_DS35X1GA_READ_BUFFER_X1:
        case AM_DEVICES_MSPI_DS35X1GA_READ_BUFFER_X2:
        case AM_DEVICES_MSPI_DS35X1GA_READ_BUFFER_X4:
        case AM_DEVICES_MSPI_DS35X1GA_PROGRAM_LOAD_X1:
        case AM_DEVICES_MSPI_DS35X1GA_PROGRAM_LOAD_X4:
        return 2;

        //
        // three bytes length of the command
        //
        case AM_DEVICES_MSPI_DS35X1GA_PROGRAM_EXECUTE:
        case AM_DEVICES_MSPI_DS35X1GA_BLOCK_ERASE:
        case AM_DEVICES_MSPI_DS35X1GA_READ_CELL_ARRAY:
        return 3;
    }
}
#endif

//*****************************************************************************
//
// Set nand flash WP and hold pin as high level in serial mode
//
//*****************************************************************************
void
am_devices_mspi_ds35x1ga_wp_hold_pinconfig(void)
{
#if defined(AM_PART_APOLLO4_API)
    am_hal_gpio_pinconfig(AM_BSP_GPIO_NAND_FLASH_WP, am_hal_gpio_pincfg_output);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_NAND_FLASH_HOLD, am_hal_gpio_pincfg_output);
    am_hal_gpio_output_set(AM_BSP_GPIO_NAND_FLASH_WP);
    am_hal_gpio_output_set(AM_BSP_GPIO_NAND_FLASH_HOLD);
#else
    am_hal_gpio_pinconfig(AM_BSP_GPIO_NAND_FLASH_WP, g_AM_HAL_GPIO_OUTPUT);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_NAND_FLASH_HOLD, g_AM_HAL_GPIO_OUTPUT);
    am_hal_gpio_state_write(AM_BSP_GPIO_NAND_FLASH_WP, AM_HAL_GPIO_OUTPUT_SET);
    am_hal_gpio_state_write(AM_BSP_GPIO_NAND_FLASH_HOLD, AM_HAL_GPIO_OUTPUT_SET);
#endif
}

static uint32_t
am_devices_mspi_ds35x1ga_read_status(void *pHandle, uint8_t ui8Addr, uint8_t *pu8iData)
{
    uint8_t status = AM_DEVICES_DS35X1GA_OIP;

    for (uint32_t i = 0; i < AM_DEVICES_MSPI_DS35X1GA_TIMEOUT; i++)
    {
        am_devices_mspi_ds35x1ga_command_read(pHandle,
            AM_DEVICES_MSPI_DS35X1GA_GET_FEATURE, true, (uint32_t)ui8Addr, (uint32_t *)(&status), 1);
        if ((status & AM_DEVICES_DS35X1GA_OIP) != AM_DEVICES_DS35X1GA_OIP)
        {
            *pu8iData = status;
            return AM_DEVICES_MSPI_DS35X1GA_STATUS_SUCCESS;
        }
        am_util_delay_us(10);
    }
    *pu8iData = status;
    return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
}

static uint32_t
am_devices_mspi_ds35x1ga_write_status(void *pHandle, uint8_t ui8Data, uint8_t ui8Addr, uint8_t *pui8Data)
{
    uint8_t status = 0;

    if (AM_HAL_STATUS_SUCCESS != am_devices_mspi_ds35x1ga_command_write(pHandle,
        AM_DEVICES_MSPI_DS35X1GA_SET_FEATURE, true, (uint32_t)ui8Addr, (uint32_t *)(&ui8Data), 1))
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }
    if (AM_HAL_STATUS_SUCCESS != am_devices_mspi_ds35x1ga_command_read(pHandle,
        AM_DEVICES_MSPI_DS35X1GA_GET_FEATURE, true, (uint32_t)ui8Addr, (uint32_t *)(&status), 1))
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }
    *pui8Data = status;

    return AM_DEVICES_MSPI_DS35X1GA_STATUS_SUCCESS;
}

static uint32_t
am_devices_mspi_ds35x1ga_reset(void *pHandle)
{
    uint8_t ui8PIOBuffer = 0x0;

    if (AM_HAL_STATUS_SUCCESS != am_devices_mspi_ds35x1ga_command_write(pHandle, AM_DEVICES_MSPI_DS35X1GA_RESET, false, 0, NULL, 0))
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    if (AM_DEVICES_MSPI_DS35X1GA_STATUS_SUCCESS != am_devices_mspi_ds35x1ga_read_status(pHandle, AM_DEVICES_MSPI_DS35X1GA_FEATURE_STATUS, &ui8PIOBuffer))
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    return AM_DEVICES_MSPI_DS35X1GA_STATUS_SUCCESS;
}
//
// Device specific initialization function.
//
static uint32_t
am_device_init_flash(void *pHandle)
{
    uint8_t ui8FeatureData = 0x0;
    uint8_t ui8FeatureStatus = 0x0;


    //
    // The block lock feature provides the ability to protect the entire device, or ranges of blocks,
    // from the PROGRAM and ERASE operations. After power-up, the device is in the "locked"
    // state, i.e., bits 1, 2, 3, 4, and 5 of the block lock register are set to 1.
    // To unlock all the blocks, or a range of blocks, the SET FEATURES command must be issued with
    // the A0h feature address.
    //
    if (AM_DEVICES_MSPI_DS35X1GA_STATUS_SUCCESS != am_devices_mspi_ds35x1ga_write_status(pHandle,
        ui8FeatureData, AM_DEVICES_MSPI_DS35X1GA_FEATURE_A0, &ui8FeatureStatus))
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    //
    // the SET FEATURES command must be issued to enable the Nand flash ECC and QE (Quad 1-1-4 mode for high speed
    // reading and writing) setting with sending B0 feature address.
    //
    ui8FeatureData = AM_DEVICES_DS35X1GA_OTP_QC_EN | AM_DEVICES_DS35X1GA_OTP_ECC_EN;
    if (AM_DEVICES_MSPI_DS35X1GA_STATUS_SUCCESS != am_devices_mspi_ds35x1ga_write_status(pHandle,
        ui8FeatureData, AM_DEVICES_MSPI_DS35X1GA_FEATURE_B0, &ui8FeatureStatus))
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    return AM_DEVICES_MSPI_DS35X1GA_STATUS_SUCCESS;
}

//
// Device specific de-initialization function.
//
static uint32_t
am_device_deinit_flash(void *pHandle)
{
    // To be add if needed
    return AM_DEVICES_MSPI_DS35X1GA_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Generic Command Write function.
//
//*****************************************************************************

#if defined(AM_PART_APOLLO3P)
static uint8_t
am_devices_mspi_ds35x1ga_asize_reconfig(void * pHandle, uint8_t ui8NewLen)
{
    am_devices_mspi_ds35x1ga_t *pFlash = (am_devices_mspi_ds35x1ga_t *)pHandle;

    am_hal_mspi_control(pFlash->pMspiHandle, AM_HAL_MSPI_REQ_ASIZE_SET, &ui8NewLen);

    return ui8NewLen;
}
#endif

static uint32_t
am_devices_mspi_device_reconfigure(void * pMspiHandle, am_hal_mspi_dev_config_t *pConfig)
{
    // Disable MSPI defore re-configuring it
    uint32_t ui32Status = am_hal_mspi_disable(pMspiHandle);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Error - Failed to disble mspi.\n");
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }
    //
    // Re-Configure the MSPI for the requested operation mode.
    //
    ui32Status = am_hal_mspi_device_configure(pMspiHandle, pConfig);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Error - Failed to configure mspi.\n");
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }
    // Re-Enable MSPI
    ui32Status = am_hal_mspi_enable(pMspiHandle);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Error - Failed to configure mspi.\n");
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    uint32_t ui32Index = 0;
    for (ui32Index = 0; ui32Index < AM_DEVICES_MSPI_DS35X1GA_MAX_DEVICE_NUM; ui32Index++)
    {
        if (gAmDs35x1ga[ui32Index].bOccupied == true)
        {
            break;
        }
    }

    if (ui32Index == AM_DEVICES_MSPI_DS35X1GA_MAX_DEVICE_NUM)
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    if ( (pConfig->eDeviceConfig == AM_HAL_MSPI_FLASH_SERIAL_CE0) || (pConfig->eDeviceConfig == AM_HAL_MSPI_FLASH_SERIAL_CE1) )
    {
        am_devices_mspi_ds35x1ga_wp_hold_pinconfig();
    }
    else
    {
#if defined(DS35X1GA_QUAD_CLKON4_MODE_EN)
        am_bsp_mspi_clkond4_pins_enable(gAmDs35x1ga[ui32Index].ui32Module, pConfig->eDeviceConfig);
#else
        am_bsp_mspi_pins_enable(gAmDs35x1ga[ui32Index].ui32Module, pConfig->eDeviceConfig);
#endif
    }

    return AM_DEVICES_MSPI_DS35X1GA_STATUS_SUCCESS;
}


static inline uint32_t am_devices_mspi_ds35x1ga_enter_command_mode(void *pHandle)
{
    am_devices_mspi_ds35x1ga_t *pFlash = (am_devices_mspi_ds35x1ga_t *)pHandle;
    if (pFlash->pCurrentSetting != pFlash->pSerialSetting)
    {
        return am_devices_mspi_device_reconfigure(pFlash->pMspiHandle, pFlash->pSerialSetting);
    }
    return AM_DEVICES_MSPI_DS35X1GA_STATUS_SUCCESS;
}

static inline uint32_t am_devices_mspi_ds35x1ga_exit_command_mode(void *pHandle)
{
    am_devices_mspi_ds35x1ga_t *pFlash = (am_devices_mspi_ds35x1ga_t *)pHandle;
    if (pFlash->pCurrentSetting != pFlash->pSerialSetting)
    {
        return am_devices_mspi_device_reconfigure(pFlash->pMspiHandle, pFlash->pCurrentSetting);
    }
    return AM_DEVICES_MSPI_DS35X1GA_STATUS_SUCCESS;
}

static uint32_t
am_devices_mspi_ds35x1ga_command_write(void *pHandle, uint8_t ui8Instr, bool bSendAddr,
                                       uint32_t ui32Addr, uint32_t *pData,
                                       uint32_t ui32NumBytes)
{
    uint32_t ui32Status;
    am_devices_mspi_ds35x1ga_t *pFlash = (am_devices_mspi_ds35x1ga_t *)pHandle;
    am_hal_mspi_pio_transfer_t stMSPIFlashPIOTransaction = {0};

    // Create the individual write transaction.
    stMSPIFlashPIOTransaction.ui32NumBytes       = ui32NumBytes;
    stMSPIFlashPIOTransaction.eDirection         = AM_HAL_MSPI_TX;
    stMSPIFlashPIOTransaction.bSendAddr          = bSendAddr;
    stMSPIFlashPIOTransaction.ui32DeviceAddr     = ui32Addr;
    stMSPIFlashPIOTransaction.bSendInstr         = true;
    stMSPIFlashPIOTransaction.ui16DeviceInstr    = ui8Instr;
    stMSPIFlashPIOTransaction.bTurnaround        = false;
    stMSPIFlashPIOTransaction.pui32Buffer        = pData;

#if defined(AM_PART_APOLLO3P)
    stMSPIFlashPIOTransaction.bQuadCmd        = false;
#endif

#if defined(AM_PART_APOLLO4_API)
#if defined(AM_PART_APOLLO4)
    stMSPIFlashPIOTransaction.eDeviceNum         = AM_HAL_MSPI_DEVICE0;
#endif
    stMSPIFlashPIOTransaction.bDCX               = false;
    stMSPIFlashPIOTransaction.bEnWRLatency       = false;
    stMSPIFlashPIOTransaction.bContinue          = false;   // MSPI CONT is deprecated for Apollo4
#endif

#if defined(AM_PART_APOLLO4_API)
    if ( (ui8Instr == AM_DEVICES_MSPI_DS35X1GA_PROGRAM_EXECUTE) || (ui8Instr == AM_DEVICES_MSPI_DS35X1GA_READ_CELL_ARRAY) || (ui8Instr == AM_DEVICES_MSPI_DS35X1GA_BLOCK_ERASE) || (ui8Instr == AM_DEVICES_MSPI_DS35X1GA_SET_FEATURE) )
    {
        // Write status/control register command byte address
        am_hal_mspi_instr_addr_t sInstAddrCfg;

        if (ui8Instr == AM_DEVICES_MSPI_DS35X1GA_SET_FEATURE)
        {
            sInstAddrCfg.eAddrCfg = AM_HAL_MSPI_ADDR_1_BYTE;
        }
        else
        {
            sInstAddrCfg.eAddrCfg = AM_HAL_MSPI_ADDR_3_BYTE;
        }

        sInstAddrCfg.eInstrCfg = pFlash->pSerialSetting->eInstrCfg;   // keep instruction setting the same
        am_hal_mspi_control(pFlash->pMspiHandle, AM_HAL_MSPI_REQ_SET_INSTR_ADDR_LEN, &sInstAddrCfg);
    }
#else
    uint8_t nlen = 0, olen = 0;
    if (bSendAddr)
    {
        nlen = ds35x1ga_cmd_addr_len(ui8Instr);
        olen = am_devices_mspi_ds35x1ga_asize_reconfig(pFlash, nlen);
    }
#endif

#ifdef DEBUG_PRINTF
    am_util_stdio_printf("cmd write instr 0x%x, addr len: %d\n", ui8Instr, nlen);
#endif

    // Execute the transction over MSPI.
    ui32Status = am_hal_mspi_blocking_transfer(pFlash->pMspiHandle, &stMSPIFlashPIOTransaction,
                                               AM_DEVICES_MSPI_DS35X1GA_TIMEOUT);

#if defined(AM_PART_APOLLO4_API)
    // restore the address length setting
    if ( (ui8Instr == AM_DEVICES_MSPI_DS35X1GA_PROGRAM_EXECUTE) || (ui8Instr == AM_DEVICES_MSPI_DS35X1GA_READ_CELL_ARRAY) || (ui8Instr == AM_DEVICES_MSPI_DS35X1GA_BLOCK_ERASE) || (ui8Instr == AM_DEVICES_MSPI_DS35X1GA_SET_FEATURE) )
    {
        am_hal_mspi_instr_addr_t sInstAddrCfg;
        sInstAddrCfg.eAddrCfg = pFlash->pSerialSetting->eAddrCfg;
        sInstAddrCfg.eInstrCfg = pFlash->pSerialSetting->eInstrCfg;
        am_hal_mspi_control(pFlash->pMspiHandle, AM_HAL_MSPI_REQ_SET_INSTR_ADDR_LEN, &sInstAddrCfg);
    }
#else
    if (bSendAddr && nlen != olen)
    {
        am_devices_mspi_ds35x1ga_asize_reconfig(pFlash, olen);
    }
#endif

    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
      return ui32Status;
    }

    return ui32Status;
}

//*****************************************************************************
//
// Generic Command Read function.
//
//*****************************************************************************
static uint32_t
am_devices_mspi_ds35x1ga_command_read(void *pHandle, uint8_t ui8Instr, bool bSendAddr,
                                      uint32_t ui32Addr, uint32_t *pData,
                                      uint32_t ui32NumBytes)
{
    uint32_t ui32Status;
    am_devices_mspi_ds35x1ga_t *pFlash = (am_devices_mspi_ds35x1ga_t *)pHandle;
    am_hal_mspi_pio_transfer_t stMSPIFlashPIOTransaction = {0};

    // Create the individual write transaction.
    stMSPIFlashPIOTransaction.eDirection         = AM_HAL_MSPI_RX;
    stMSPIFlashPIOTransaction.bSendAddr          = bSendAddr;
    stMSPIFlashPIOTransaction.ui32DeviceAddr     = ui32Addr;
    stMSPIFlashPIOTransaction.bSendInstr         = true;
    stMSPIFlashPIOTransaction.ui16DeviceInstr    = ui8Instr;

#if defined(AM_PART_APOLLO3P)
    stMSPIFlashPIOTransaction.bQuadCmd        = false;
    stMSPIFlashPIOTransaction.bTurnaround     = false;
#endif

#if defined(AM_PART_APOLLO4_API)
    if ( ui8Instr == AM_DEVICES_MSPI_DS35X1GA_GET_FEATURE )
    {
        // Read status/control register command uses 1 byte address
        am_hal_mspi_instr_addr_t sInstAddrCfg;


        sInstAddrCfg.eAddrCfg = AM_HAL_MSPI_ADDR_1_BYTE;
        sInstAddrCfg.eInstrCfg = pFlash->pSerialSetting->eInstrCfg;   // keep instruction setting the same
        am_hal_mspi_control(pFlash->pMspiHandle, AM_HAL_MSPI_REQ_SET_INSTR_ADDR_LEN, &sInstAddrCfg);
    }
#else
    uint8_t nlen = 0, olen = 0;
    if (bSendAddr)
    {
        nlen = ds35x1ga_cmd_addr_len(ui8Instr);
        olen = am_devices_mspi_ds35x1ga_asize_reconfig(pFlash, nlen);
    }
#endif

    stMSPIFlashPIOTransaction.ui32NumBytes       = ui32NumBytes;
    stMSPIFlashPIOTransaction.pui32Buffer        = pData;

#if defined(AM_PART_APOLLO4_API)
#if defined(AM_PART_APOLLO4)
    stMSPIFlashPIOTransaction.eDeviceNum         = AM_HAL_MSPI_DEVICE0;
#endif
    stMSPIFlashPIOTransaction.bDCX               = false;
    stMSPIFlashPIOTransaction.bEnWRLatency       = false;
    stMSPIFlashPIOTransaction.bContinue          = false;   // MSPI CONT is deprecated for Apollo4
#endif

#ifdef DEBUG_PRINTF
    am_util_stdio_printf("cmd read instr 0x%x, addr len, : %d bSendAddr : %d\n,", ui8Instr, nlen, bSendAddr);
#endif

    // Execute the transction over MSPI.
    ui32Status = am_hal_mspi_blocking_transfer(pFlash->pMspiHandle, &stMSPIFlashPIOTransaction,
                                               AM_DEVICES_MSPI_DS35X1GA_TIMEOUT);

#if defined(AM_PART_APOLLO4_API)
    // restore the address length setting
    if ( ui8Instr == AM_DEVICES_MSPI_DS35X1GA_GET_FEATURE )
    {
        am_hal_mspi_instr_addr_t sInstAddrCfg;
        sInstAddrCfg.eAddrCfg = pFlash->pSerialSetting->eAddrCfg;
        sInstAddrCfg.eInstrCfg = pFlash->pSerialSetting->eInstrCfg;
        am_hal_mspi_control(pFlash->pMspiHandle, AM_HAL_MSPI_REQ_SET_INSTR_ADDR_LEN, &sInstAddrCfg);
    }
#else
    if ( bSendAddr && nlen != olen )
    {
        am_devices_mspi_ds35x1ga_asize_reconfig(pFlash, olen);
    }
#endif

    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
      return ui32Status;
    }

    return ui32Status;
}

static void
pfnMSPI_DS35X1GA_Callback(void *pCallbackCtxt, uint32_t status)
{
    // Set the DMA complete flag.
    *(volatile bool *)pCallbackCtxt = true;
}


static uint32_t
am_devices_mspi_ds35x1ga_tx_enter_ad4_mode(void *pHandle)
{
    uint32_t ui32Status;
    am_devices_mspi_ds35x1ga_t *pFlash = (am_devices_mspi_ds35x1ga_t *)pHandle;

    if ( pFlash->pCurrentSetting == pFlash->pSerialSetting )
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_SUCCESS;
    }

    //
    // Set writelatency after command mode in write mode in nand flash.
    //
    ui32Status = am_hal_mspi_control(pFlash->pMspiHandle, AM_HAL_MSPI_REQ_NAND_FLASH_SET_WLAT, pFlash->pCurrentSetting);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
      return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    return AM_DEVICES_MSPI_DS35X1GA_STATUS_SUCCESS;
}


uint32_t
am_devices_mspi_ds35x1ga_init(uint32_t ui32Module, const am_devices_mspi_ds35x1ga_config_t *psMSPISettings, void **ppHandle, void **ppMspiHandle)
{
    uint32_t ui32Status;
    am_hal_mspi_dev_config_t *psConfig = g_DS35X1GA_DevConfig[0].psDevConfig;
    void *pMspiHandle;
    uint32_t ui32Index = 0;

    if ((ui32Module >= AM_REG_MSPI_NUM_MODULES) || (psMSPISettings == NULL))
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    // Allocate a vacant device handle
    for (ui32Index = 0; ui32Index < AM_DEVICES_MSPI_DS35X1GA_MAX_DEVICE_NUM; ui32Index++)
    {
        if (gAmDs35x1ga[ui32Index].bOccupied == false)
        {
            break;
        }
    }
    if (ui32Index == AM_DEVICES_MSPI_DS35X1GA_MAX_DEVICE_NUM)
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    for (uint32_t i = 0; i < (sizeof(g_DS35X1GA_DevConfig) / sizeof(g_DS35X1GA_DevConfig[0])); i++)
    {
        if (psMSPISettings->eDeviceConfig == g_DS35X1GA_DevConfig[i].eHalDeviceEnum)
        {
            psConfig = g_DS35X1GA_DevConfig[i].psDevConfig;
            psConfig->eClockFreq = psMSPISettings->eClockFreq;
#if !defined(AM_PART_APOLLO4_API)
            psConfig->pTCB = psMSPISettings->pNBTxnBuf;
            psConfig->ui32TCBSize = psMSPISettings->ui32NBTxnBufLength;
            psConfig->scramblingStartAddr = psMSPISettings->ui32ScramblingStartAddr;
            psConfig->scramblingEndAddr = psMSPISettings->ui32ScramblingEndAddr;
#endif
            break;
        }
    }

    //
    // Enable fault detection.
    //
#if defined(AM_PART_APOLLO4_API)
    am_hal_fault_capture_enable();
#else
#if AM_APOLLO3_MCUCTRL
    am_hal_mcuctrl_control(AM_HAL_MCUCTRL_CONTROL_FAULT_CAPTURE_ENABLE, 0);
#else // AM_APOLLO3_MCUCTRL
    am_hal_mcuctrl_fault_capture_enable();
#endif // AM_APOLLO3_MCUCTRL
#endif // !AM_PART_APOLLO4

    //
    // Configure the MSPI for Serial or Quad-Paired Serial operation during initialization.
    //
    switch (psConfig->eDeviceConfig)
    {
        case AM_HAL_MSPI_FLASH_SERIAL_CE0:
        case AM_HAL_MSPI_FLASH_QUAD_CE0_1_1_4:
            gAmDs35x1ga[ui32Index].pSerialSetting = &MSPI_DS35X1GA_Serial_CE0_MSPIConfig;
            if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_initialize(ui32Module, &pMspiHandle))
            {
                am_util_stdio_printf("Error - Failed to initialize MSPI.\n");
                return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
            }

            if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_power_control(pMspiHandle, AM_HAL_SYSCTRL_WAKE, false))
            {
                am_util_stdio_printf("Error - Failed to power on MSPI.\n");
                return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
            }

#if defined(AM_PART_APOLLO4_API)
            am_hal_mspi_config_t    mspiCfg = gMspiCfg;
            mspiCfg.ui32TCBSize = psMSPISettings->ui32NBTxnBufLength;
            mspiCfg.pTCB = psMSPISettings->pNBTxnBuf;
            if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_configure(pMspiHandle, &mspiCfg))
            {
                am_util_stdio_printf("Error - Failed to configure MSPI device.\n");
                return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
            }
#endif

            if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_device_configure(pMspiHandle, &MSPI_DS35X1GA_Serial_CE0_MSPIConfig))
            {
                am_util_stdio_printf("Error - Failed to configure MSPI.\n");
                return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
            }

            if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_enable(pMspiHandle))
            {
                am_util_stdio_printf("Error - Failed to enable MSPI.\n");
                return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
            }

#if defined(DS35X1GA_QUAD_CLKON4_MODE_EN)
            am_bsp_mspi_clkond4_pins_enable(ui32Module, MSPI_DS35X1GA_Serial_CE0_MSPIConfig.eDeviceConfig);
#else
            am_bsp_mspi_pins_enable(ui32Module, MSPI_DS35X1GA_Serial_CE0_MSPIConfig.eDeviceConfig);
#endif
            am_devices_mspi_ds35x1ga_wp_hold_pinconfig();
            break;

        case AM_HAL_MSPI_FLASH_SERIAL_CE1:
        case AM_HAL_MSPI_FLASH_QUAD_CE1_1_1_4:
            gAmDs35x1ga[ui32Index].pSerialSetting = &MSPI_DS35X1GA_Serial_CE1_MSPIConfig;
            if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_initialize(ui32Module, &pMspiHandle))
            {
                am_util_stdio_printf("Error - Failed to initialize MSPI.\n");
                return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
            }

            if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_power_control(pMspiHandle, AM_HAL_SYSCTRL_WAKE, false))
            {
                am_util_stdio_printf("Error - Failed to power on MSPI.\n");
                return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
            }

#if defined(AM_PART_APOLLO4_API)
            mspiCfg = gMspiCfg;
            mspiCfg.ui32TCBSize = psMSPISettings->ui32NBTxnBufLength;
            mspiCfg.pTCB = psMSPISettings->pNBTxnBuf;
            if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_configure(pMspiHandle, &mspiCfg))
            {
                am_util_stdio_printf("Error - Failed to configure MSPI device.\n");
                return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
            }
#endif

            if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_device_configure(pMspiHandle, &MSPI_DS35X1GA_Serial_CE1_MSPIConfig))
            {
                am_util_stdio_printf("Error - Failed to configure MSPI.\n");
                return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
            }

            if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_enable(pMspiHandle))
            {
                am_util_stdio_printf("Error - Failed to enable MSPI.\n");
                return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
            }

#if defined(DS35X1GA_QUAD_CLKON4_MODE_EN)
            am_bsp_mspi_clkond4_pins_enable(ui32Module, MSPI_DS35X1GA_Serial_CE1_MSPIConfig.eDeviceConfig);
#else
            am_bsp_mspi_pins_enable(ui32Module, MSPI_DS35X1GA_Serial_CE1_MSPIConfig.eDeviceConfig);
#endif
            am_devices_mspi_ds35x1ga_wp_hold_pinconfig();
            break;

        default:
            return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }


    gAmDs35x1ga[ui32Index].pCurrentSetting = psConfig;
    gAmDs35x1ga[ui32Index].pMspiHandle = pMspiHandle;
    gAmDs35x1ga[ui32Index].ui32Module = ui32Module;

    if (AM_HAL_STATUS_SUCCESS != am_devices_mspi_ds35x1ga_reset((void *)&gAmDs35x1ga[ui32Index]))
    {
        am_util_stdio_printf("Error - Failed to reset ds34x1ga.\n");
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    //
    // Device specific MSPI Flash initialization.
    //
    ui32Status = am_device_init_flash((void *)&gAmDs35x1ga[ui32Index]);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Error - Failed to initialize nand flash ds34x1ga.\n");
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    // Disable MSPI before re-configuring it
    ui32Status = am_hal_mspi_disable(pMspiHandle);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Error - Failed to disble mspi.\n");
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    //
    // Re-Configure the MSPI for the requested operation mode.
    //
    ui32Status = am_hal_mspi_device_configure(pMspiHandle, psConfig);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Error - Failed to configure mspi.\n");
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }
    // Re-Enable MSPI
    ui32Status = am_hal_mspi_enable(pMspiHandle);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Error - Failed to configure mspi.\n");
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    //
    // Configure the MSPI pins.
    //
#if defined(DS35X1GA_QUAD_CLKON4_MODE_EN)
    am_bsp_mspi_clkond4_pins_enable(ui32Module, psConfig->eDeviceConfig);
#else
    am_bsp_mspi_pins_enable(ui32Module, psConfig->eDeviceConfig);
#endif

    //
    // Enable MSPI interrupts.
    //

    ui32Status = am_hal_mspi_interrupt_clear(pMspiHandle, AM_HAL_MSPI_INT_CQUPD | AM_HAL_MSPI_INT_ERR );
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    ui32Status = am_hal_mspi_interrupt_enable(pMspiHandle, AM_HAL_MSPI_INT_CQUPD | AM_HAL_MSPI_INT_ERR );
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    //
    // Return the handle.
    //
    gAmDs35x1ga[ui32Index].bOccupied = true;
    *ppMspiHandle = pMspiHandle;
    *ppHandle = (void *)&gAmDs35x1ga[ui32Index];

    //
    // Return the status.
    //
    return AM_DEVICES_MSPI_DS35X1GA_STATUS_SUCCESS;
}

uint32_t
am_devices_mspi_ds35x1ga_deinit(void *pHandle)
{
    uint32_t ui32Status;
    am_devices_mspi_ds35x1ga_t *pFlash = (am_devices_mspi_ds35x1ga_t *)pHandle;

    //
    // Device specific MSPI Flash de-initialization.
    //
    ui32Status = am_device_deinit_flash(pHandle);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    if (AM_HAL_STATUS_SUCCESS != am_devices_mspi_ds35x1ga_reset(pHandle))
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    //
    // Disable and clear the interrupts to start with.
    //
    ui32Status = am_hal_mspi_interrupt_disable(pFlash->pMspiHandle, 0xFFFFFFFF);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }
    ui32Status = am_hal_mspi_interrupt_clear(pFlash->pMspiHandle, 0xFFFFFFFF);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    //
    // Disable the MSPI instance.
    //
    ui32Status = am_hal_mspi_disable(pFlash->pMspiHandle);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_power_control(pFlash->pMspiHandle, AM_HAL_SYSCTRL_DEEPSLEEP, false))
    {
        am_util_stdio_printf("Error - Failed to power on MSPI.\n");
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    //
    // Deinitialize the MSPI instance.
    //
    ui32Status = am_hal_mspi_deinitialize(pFlash->pMspiHandle);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    // Free this device handle
    pFlash->bOccupied = false;

    //
    // Clear the Flash Caching.
    //
#if !defined(AM_PART_APOLLO4_API)
#if AM_CMSIS_REGS
    CACHECTRL->CACHECFG = 0;
#else // AM_CMSIS_REGS
    AM_REG(CACHECTRL, CACHECFG) = 0;
#endif // AM_CMSIS_REGS
#endif // !AM_PART_APOLLO4

    //
    // Return the status.
    //
    return AM_DEVICES_MSPI_DS35X1GA_STATUS_SUCCESS;
}


uint32_t
am_devices_mspi_ds35x1ga_id(void *pHandle, uint32_t *pui32DeviceID)
{
    uint32_t ui32Status;

    am_devices_mspi_ds35x1ga_enter_command_mode(pHandle);

    //
    // Send the command sequence to read the Device ID and return status.
    //
    uint8_t ui8Response[3];
    ui32Status = am_devices_mspi_ds35x1ga_command_read(pHandle, AM_DEVICES_MSPI_DS35X1GA_READ_ID, false, 0, (uint32_t *)&ui8Response[0], 3);
    *pui32DeviceID = (ui8Response[1] << 8) | ui8Response[2];
    if (((*pui32DeviceID & AM_DEVICES_MSPI_DS35X1GA_ID_MASK) == AM_DEVICES_MSPI_DS35X1GA_ID) &&
        (AM_HAL_STATUS_SUCCESS == ui32Status))
    {
        am_devices_mspi_ds35x1ga_exit_command_mode(pHandle);
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_SUCCESS;
    }
    else
    {
        am_devices_mspi_ds35x1ga_exit_command_mode(pHandle);
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }
}


uint32_t am_devices_mspi_ds35x1ga_read(void *pHandle, uint32_t ui32PageNum,
                                       uint8_t *pui8DataBuffer,
                                       uint32_t ui32DataLen,
                                       uint8_t *pui8OobBuffer,
                                       uint32_t ui32OobLen,
                                       uint8_t *pui32EccResult)
{
    am_hal_mspi_dma_transfer_t Transaction;
    uint32_t ui32Status;
    uint8_t PageBuffer[AM_DEVICES_MSPI_DS35X1GA_PAGE_FULL_SIZE];
    am_devices_mspi_ds35x1ga_t *pFlash = (am_devices_mspi_ds35x1ga_t *)pHandle;


    if (ui32PageNum >= AM_DEVICES_MSPI_DS35X1GA_MAX_PAGES)
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    am_devices_mspi_ds35x1ga_enter_command_mode(pHandle);

    ui32Status = am_devices_mspi_ds35x1ga_command_write(pHandle, AM_DEVICES_MSPI_DS35X1GA_READ_CELL_ARRAY, true, ui32PageNum, NULL, 0);

    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    if (AM_DEVICES_MSPI_DS35X1GA_STATUS_SUCCESS != am_devices_mspi_ds35x1ga_read_status(pHandle, AM_DEVICES_MSPI_DS35X1GA_FEATURE_STATUS, (uint8_t *)&ui32Status))
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    switch (ui32Status & AM_DEVICES_DS35X1GA_ECCS)
    {
        case AM_DEVICES_DS35X1GA_ECCS_NO_BIT_FLIPS:
            *pui32EccResult = AM_DEVICES_MSPI_DS35X1GA_ECC_STATUS_NO_BIT_FLIPS;
            break;
        case AM_DEVICES_DS35X1GA_ECCS_BIT_FLIPS_CORRECTED:
        case AM_DEVICES_DS35X1GA_ECCS_BIT_FLIPS_CORRECTED_THR:
            *pui32EccResult = AM_DEVICES_MSPI_DS35X1GA_ECC_STATUS_BIT_FLIPS_CORRECTED;
            break;
        case AM_DEVICES_DS35X1GA_ECCS_BIT_FLIPS_NOT_CORRECTED:
            *pui32EccResult = AM_DEVICES_MSPI_DS35X1GA_ECC_STATUS_BIT_FLIPS_NOT_CORRECTED;
            break;
        default:
            *pui32EccResult = 0xFF; // invalid ECC status
            break;
    }

    am_devices_mspi_ds35x1ga_exit_command_mode(pHandle);

    // Set the DMA priority
    Transaction.ui8Priority = 1;
    // Set the transfer direction to RX (Read)
    Transaction.eDirection = AM_HAL_MSPI_RX;
    // Set the transfer count in bytes.
    Transaction.ui32TransferCount = AM_DEVICES_MSPI_DS35X1GA_PAGE_FULL_SIZE;
    // Read the whole page.
    Transaction.ui32DeviceAddress = 0x0;
    // Set the target SRAM buffer address.
    Transaction.ui32SRAMAddress = (uint32_t)PageBuffer;
    // Clear the CQ stimulus.
    Transaction.ui32PauseCondition = 0;
    // Clear the post-processing
    Transaction.ui32StatusSetClr = 0;

 #if defined(AM_PART_APOLLO4)
    Transaction.eDeviceNum         = AM_HAL_MSPI_DEVICE0;
#endif

    // Start the transaction.
    volatile bool bDMAComplete = false;
    ui32Status = am_hal_mspi_nonblocking_transfer(pFlash->pMspiHandle, &Transaction, AM_HAL_MSPI_TRANS_DMA, pfnMSPI_DS35X1GA_Callback, (void *)&bDMAComplete);

    // Check the transaction status.
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    // Wait for DMA Complete or Timeout
    for (uint32_t i = 0; i < AM_DEVICES_MSPI_DS35X1GA_TIMEOUT; i++)
    {
        if (bDMAComplete)
        {
            break;
        }
        // Call the BOOTROM cycle function to delay for about 1 microsecond.
        //
#if defined(AM_PART_APOLLO4_API)
        am_util_delay_us(1);
#else
        am_hal_flash_delay(FLASH_CYCLES_US(1));
#endif
    }

    // Check the status.
    if (!bDMAComplete)
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    memcpy(pui8DataBuffer, PageBuffer, ui32DataLen);
    memcpy(pui8OobBuffer, (&PageBuffer[AM_DEVICES_MSPI_DS35X1GA_PAGE_DATA_SIZE]), ui32OobLen);

    // Return the status.
    return AM_DEVICES_MSPI_DS35X1GA_STATUS_SUCCESS;
}



uint32_t am_devices_mspi_ds35x1ga_write(void *pHandle, uint32_t ui32PageNum,
                                        uint8_t *pui8DataBuffer,
                                        uint32_t ui32DataLen,
                                        uint8_t *pui8OobBuffer,
                                        uint32_t ui32OobLen)
{
    am_hal_mspi_dma_transfer_t Transaction;
    uint32_t ui32Status;
    uint8_t PageBuffer[AM_DEVICES_MSPI_DS35X1GA_PAGE_FULL_SIZE];
    am_devices_mspi_ds35x1ga_t *pFlash = (am_devices_mspi_ds35x1ga_t *)pHandle;

    if (ui32PageNum >= AM_DEVICES_MSPI_DS35X1GA_MAX_PAGES)
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    am_devices_mspi_ds35x1ga_enter_command_mode(pHandle);

    //
    // Send the command sequence to enable writing.
    //
    ui32Status = am_devices_mspi_ds35x1ga_command_write(pHandle, AM_DEVICES_MSPI_DS35X1GA_WRITE_ENABLE, false, 0, NULL, 0);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    // Prepare the source data buffer.
    memcpy(PageBuffer, pui8DataBuffer, ui32DataLen);
    memcpy(&PageBuffer[AM_DEVICES_MSPI_DS35X1GA_PAGE_DATA_SIZE], pui8OobBuffer, ui32OobLen);

    am_devices_mspi_ds35x1ga_exit_command_mode(pHandle);

    ui32Status = am_devices_mspi_ds35x1ga_tx_enter_ad4_mode(pHandle);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    // Set the DMA priority
    Transaction.ui8Priority = 1;
    // Set the transfer direction to TX (Write)
    Transaction.eDirection = AM_HAL_MSPI_TX;
    // Set the transfer count in bytes.
    Transaction.ui32TransferCount = AM_DEVICES_MSPI_DS35X1GA_PAGE_FULL_SIZE;
    // Program the whole page.

    Transaction.ui32DeviceAddress = 0x0;

    // Set the source SRAM buffer address.
    Transaction.ui32SRAMAddress = (uint32_t)PageBuffer;
    ;
    // Clear the CQ stimulus.
    Transaction.ui32PauseCondition = 0;
    // Clear the post-processing
    Transaction.ui32StatusSetClr = 0;

    // Start the transaction.
    volatile bool bDMAComplete = false;
    ui32Status = am_hal_mspi_nonblocking_transfer(pFlash->pMspiHandle, &Transaction, AM_HAL_MSPI_TRANS_DMA, pfnMSPI_DS35X1GA_Callback, (void *)&bDMAComplete);

    // Check the transaction status.
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    // Wait for DMA Complete or Timeout
    for (uint32_t i = 0; i < AM_DEVICES_MSPI_DS35X1GA_TIMEOUT; i++)
    {
        if (bDMAComplete)
        {
            // program execute here.
            am_devices_mspi_ds35x1ga_enter_command_mode(pHandle);

            ui32Status = am_devices_mspi_ds35x1ga_command_write(pHandle, AM_DEVICES_MSPI_DS35X1GA_PROGRAM_EXECUTE, true, ui32PageNum, NULL, 0);

            if (AM_HAL_STATUS_SUCCESS != ui32Status)
            {
                am_devices_mspi_ds35x1ga_exit_command_mode(pHandle);
                return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
            }

            if (AM_DEVICES_MSPI_DS35X1GA_STATUS_SUCCESS == am_devices_mspi_ds35x1ga_read_status(pHandle, AM_DEVICES_MSPI_DS35X1GA_FEATURE_STATUS, (uint8_t *)&ui32Status))
            {
                if (((ui32Status & AM_DEVICES_DS35X1GA_PRG_F) != AM_DEVICES_DS35X1GA_PRG_F) &&
                    ((ui32Status & AM_DEVICES_DS35X1GA_OIP) != AM_DEVICES_DS35X1GA_OIP))
                {
                    //
                    // Send the command sequence to disable writing.
                    //
                    ui32Status = am_devices_mspi_ds35x1ga_command_write(pHandle, AM_DEVICES_MSPI_DS35X1GA_WRITE_DISABLE, false, 0, NULL, 0);
                    if (AM_HAL_STATUS_SUCCESS != ui32Status)
                    {
                        am_devices_mspi_ds35x1ga_exit_command_mode(pHandle);
                        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
                    }
                    break;
                }
            }
            am_devices_mspi_ds35x1ga_exit_command_mode(pHandle);
        }
        //
        // Call the BOOTROM cycle function to delay for about 1 microsecond.
        //
#if defined(AM_PART_APOLLO4_API)
        am_util_delay_us(1);
#else
        am_hal_flash_delay(FLASH_CYCLES_US(1));
#endif
    }

    // Check the status.
    if (!bDMAComplete)
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    //
    // Return the status.
    //
    return AM_DEVICES_MSPI_DS35X1GA_STATUS_SUCCESS;
}


uint32_t
am_devices_mspi_ds35x1ga_block_erase(void *pHandle, uint32_t ui32BlockNum)
{
    uint32_t ui32Status;


    if (ui32BlockNum >= AM_DEVICES_MSPI_DS35X1GA_MAX_BLOCKS)
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    am_devices_mspi_ds35x1ga_enter_command_mode(pHandle);

    //
    // Send the command sequence to enable writing.
    //
    ui32Status = am_devices_mspi_ds35x1ga_command_write(pHandle, AM_DEVICES_MSPI_DS35X1GA_WRITE_ENABLE, false, 0, NULL, 0);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    //
    // Send the command to remove protection from the sector.
    //
    ui32Status = am_devices_mspi_ds35x1ga_command_write(pHandle, AM_DEVICES_MSPI_DS35X1GA_BLOCK_ERASE, true, (ui32BlockNum << 6), NULL, 0);

    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
    }

    for (uint32_t i = 0; i < AM_DEVICES_MSPI_DS35X1GA_TIMEOUT; i++)
    {
        if (AM_DEVICES_MSPI_DS35X1GA_STATUS_SUCCESS == am_devices_mspi_ds35x1ga_read_status(pHandle, AM_DEVICES_MSPI_DS35X1GA_FEATURE_STATUS, (uint8_t *)&ui32Status))
        {
            // check for ERS_F, OIP bit
            if (((ui32Status & AM_DEVICES_DS35X1GA_ERS_F) != AM_DEVICES_DS35X1GA_ERS_F) &&
                ((ui32Status & AM_DEVICES_DS35X1GA_OIP) != AM_DEVICES_DS35X1GA_OIP))
            {
                //
                // Send the command sequence to disable writing.
                //
                ui32Status = am_devices_mspi_ds35x1ga_command_write(pHandle, AM_DEVICES_MSPI_DS35X1GA_WRITE_DISABLE, false, 0, NULL, 0);
                if (AM_HAL_STATUS_SUCCESS != ui32Status)
                {
                    am_devices_mspi_ds35x1ga_exit_command_mode(pHandle);
                    return AM_DEVICES_MSPI_DS35X1GA_STATUS_ERROR;
                }
                break;
            }
        }
        //
        // Call the BOOTROM cycle function to delay for about 1 microsecond.
        //
#if defined(AM_PART_APOLLO4_API)
        am_util_delay_us(1);
#else
        am_hal_flash_delay(FLASH_CYCLES_US(1));
#endif
    }

    am_devices_mspi_ds35x1ga_exit_command_mode(pHandle);

    //
    // Return the status.
    //
    return AM_DEVICES_MSPI_DS35X1GA_STATUS_SUCCESS;
}

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

