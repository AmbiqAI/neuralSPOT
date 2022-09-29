//*****************************************************************************
//
//! @file am_devices_mspi_psram_w958d6nw.c
//!
//! @brief Winbond MSPI PSRAM driver.
//!
//! @addtogroup mspi_psram_w958d6nw W958D6NW MSPI PSRAM Driver
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
#include "am_devices_mspi_psram_w958d6nw.h"
#include "am_util_stdio.h"
#include "am_bsp.h"
#include "am_util.h"
#include "am_util_delay.h"

//*****************************************************************************
//
// Global variables.
//
//*****************************************************************************
//#define W958D6NW_USE_NON_DQS_MODE
#if defined(APOLLO4_FPGA)
#define W958D6NW_USE_NON_DQS_MODE
#endif
#define AM_DEVICES_W958D6NW_PSRAM_TIMEOUT              1000000
#define PSRAM_TIMING_SCAN_MIN_ACCEPTANCE_LENGTH   (8)     // there should be at least
                                                          // this amount of consecutive
                                                          // passing settings to be accepted.
am_hal_mspi_xip_config_t gWBDDRXipConfig[] =
{
  {
    .ui32APBaseAddr       = MSPI0_APERTURE_START_ADDR,
    .eAPMode              = AM_HAL_MSPI_AP_READ_WRITE,
    .eAPSize              = AM_HAL_MSPI_AP_SIZE64M,
    .scramblingStartAddr  = 0,
    .scramblingEndAddr    = 0,
  },
  {
    .ui32APBaseAddr       = MSPI1_APERTURE_START_ADDR,
    .eAPMode              = AM_HAL_MSPI_AP_READ_WRITE,
    .eAPSize              = AM_HAL_MSPI_AP_SIZE64M,
    .scramblingStartAddr  = 0,
    .scramblingEndAddr    = 0,
  },
  {
    .ui32APBaseAddr       = MSPI2_APERTURE_START_ADDR,
    .eAPMode              = AM_HAL_MSPI_AP_READ_WRITE,
    .eAPSize              = AM_HAL_MSPI_AP_SIZE64M,
    .scramblingStartAddr  = 0,
    .scramblingEndAddr    = 0,
  }
};

am_hal_mspi_dqs_t gWBDDRDqsCfg[] =
{
  {
#ifdef W958D6NW_USE_NON_DQS_MODE
    .bDQSEnable             = 0,
#else
    .bDQSEnable             = 1,
#endif
    .bDQSSyncNeg            = 0,
    .bEnableFineDelay       = 0,
    .ui8TxDQSDelay          = 0,
    .ui8RxDQSDelay          = 16,
    .ui8RxDQSDelayNeg       = 0,
    .bRxDQSDelayNegEN       = 0,
    .ui8RxDQSDelayHi        = 0,
    .ui8RxDQSDelayNegHi     = 0,
    .bRxDQSDelayHiEN        = 0,
  },
  {
#ifdef W958D6NW_USE_NON_DQS_MODE
    .bDQSEnable             = 0,
#else
    .bDQSEnable             = 1,
#endif
    .bDQSSyncNeg            = 0,
    .bEnableFineDelay       = 0,
    .ui8TxDQSDelay          = 0,
    .ui8RxDQSDelay          = 16,
    .ui8RxDQSDelayNeg       = 0,
    .bRxDQSDelayNegEN       = 0,
    .ui8RxDQSDelayHi        = 0,
    .ui8RxDQSDelayNegHi     = 0,
    .bRxDQSDelayHiEN        = 0,
  },
  {
#ifdef W958D6NW_USE_NON_DQS_MODE
    .bDQSEnable             = 0,
#else
    .bDQSEnable             = 1,
#endif
    .bDQSSyncNeg            = 0,
    .bEnableFineDelay       = 0,
    .ui8TxDQSDelay          = 0,
    .ui8RxDQSDelay          = 16,
    .ui8RxDQSDelayNeg       = 0,
    .bRxDQSDelayNegEN       = 0,
    .ui8RxDQSDelayHi        = 0,
    .ui8RxDQSDelayNegHi     = 0,
    .bRxDQSDelayHiEN        = 0,
  }
};

am_hal_mspi_xip_misc_t gWBXipMiscCfg[] =
{
  {
    .ui32CEBreak        = 10,
    .bXIPBoundary       = true,
    .bXIPOdd            = true,
    .bAppndOdd          = false,
    .bBEOn              = false,
    .eBEPolarity        = AM_HAL_MSPI_BE_LOW_ENABLE,
  },
  {
    .ui32CEBreak        = 10,
    .bXIPBoundary       = true,
    .bXIPOdd            = true,
    .bAppndOdd          = false,
    .bBEOn              = false,
    .eBEPolarity        = AM_HAL_MSPI_BE_LOW_ENABLE,
  },
  {
    .ui32CEBreak        = 10,
    .bXIPBoundary       = true,
    .bXIPOdd            = true,
    .bAppndOdd          = false,
    .bBEOn              = false,
    .eBEPolarity        = AM_HAL_MSPI_BE_LOW_ENABLE,
  }
};

am_hal_mspi_config_t gWBDDRMspiCfg =
{
  .ui32TCBSize          = 0,
  .pTCB                 = NULL,
  .bClkonD4             = 0
};

am_hal_mspi_rxcfg_t gWBMspiRxCfg =
{
    .ui8DQSturn         = 2,
    .bRxHI              = 0,
    .bTaForth           = 1,
    .bHyperIO           = 1,
    .ui8RxSmp           = 1,
    .bRBX               = 0,
    .bWBX               = 0,
    .bSCLKRxHalt        = 0,
    .bRxCapEXT          = 0,
    .ui8Sfturn          = 10,
};

am_hal_mspi_dev_config_t  WBDDROctalCE0MSPIConfig =
{
  .eAddrCfg             = AM_HAL_MSPI_ADDR_4_BYTE,
  .eInstrCfg            = AM_HAL_MSPI_INSTR_2_BYTE,
  .ui16ReadInstr        = AM_DEVICES_MSPI_PSRAM_W958D6NW_DDR_READ,
  .ui16WriteInstr       = AM_DEVICES_MSPI_PSRAM_W958D6NW_DDR_WRITE,
  .eDeviceConfig        = AM_HAL_MSPI_FLASH_OCTAL_DDR_CE0,
  .eSpiMode             = AM_HAL_MSPI_SPI_MODE_0,
  .bSendAddr            = true,
  .bSendInstr           = true,
  .bTurnaround          = true,
  .eClockFreq           = AM_HAL_MSPI_CLK_96MHZ,
#ifdef W958D6NW_USE_NON_DQS_MODE
  .ui8TurnAround        = 14,
  .ui8WriteLatency      = 14,
#else
  .ui8TurnAround        = 6,
  .ui8WriteLatency      = 6,
#endif
  .bEnWriteLatency      = true,
  .bEmulateDDR          = true,
#if defined(APOLLO4_FPGA)
  .ui16DMATimeLimit     = 20,
#else
  .ui16DMATimeLimit     = 40,
#endif
  .eDMABoundary         = AM_HAL_MSPI_BOUNDARY_BREAK1K,
};

am_hal_mspi_dev_config_t  WBDDROctalCE1MSPIConfig =
{
  .eAddrCfg             = AM_HAL_MSPI_ADDR_4_BYTE,
  .eInstrCfg            = AM_HAL_MSPI_INSTR_2_BYTE,
  .ui16ReadInstr        = AM_DEVICES_MSPI_PSRAM_W958D6NW_DDR_READ,
  .ui16WriteInstr       = AM_DEVICES_MSPI_PSRAM_W958D6NW_DDR_WRITE,
  .eDeviceConfig        = AM_HAL_MSPI_FLASH_OCTAL_DDR_CE1,
  .eSpiMode             = AM_HAL_MSPI_SPI_MODE_0,
  .bSendAddr            = true,
  .bSendInstr           = true,
  .bTurnaround          = true,
  .eClockFreq           = AM_HAL_MSPI_CLK_96MHZ,
#ifdef W958D6NW_USE_NON_DQS_MODE
  .ui8TurnAround        = 14,
  .ui8WriteLatency      = 14,
#else
  .ui8TurnAround        = 6,
  .ui8WriteLatency      = 6,
#endif
  .bEnWriteLatency      = true,
  .bEmulateDDR          = true,
#if defined(APOLLO4_FPGA)
  .ui16DMATimeLimit     = 20,
#else
  .ui16DMATimeLimit     = 40,
#endif
  .eDMABoundary         = AM_HAL_MSPI_BOUNDARY_BREAK1K,
};

am_hal_mspi_dev_config_t  WBDDRHEXCE0MSPIConfig =
{
  .eAddrCfg             = AM_HAL_MSPI_ADDR_4_BYTE,
  .eInstrCfg            = AM_HAL_MSPI_INSTR_2_BYTE,
  .ui16ReadInstr        = AM_DEVICES_MSPI_PSRAM_W958D6NW_DDR_READ,
  .ui16WriteInstr       = AM_DEVICES_MSPI_PSRAM_W958D6NW_DDR_WRITE,
  .eDeviceConfig        = AM_HAL_MSPI_FLASH_HEX_DDR_CE0,
  .eSpiMode             = AM_HAL_MSPI_SPI_MODE_0,
  .bSendAddr            = true,
  .bSendInstr           = true,
  .bTurnaround          = true,
  .eClockFreq           = AM_HAL_MSPI_CLK_96MHZ,
#ifdef W958D6NW_USE_NON_DQS_MODE
  .ui8TurnAround        = 14,
  .ui8WriteLatency      = 14,
#else
  .ui8TurnAround        = 6,
  .ui8WriteLatency      = 6,
#endif
  .bEnWriteLatency      = true,
  .bEmulateDDR          = true,
#if defined(APOLLO4_FPGA)
  .ui16DMATimeLimit     = 20,
#else
  .ui16DMATimeLimit     = 40,
#endif
  .eDMABoundary         = AM_HAL_MSPI_BOUNDARY_BREAK1K,
};

am_hal_mspi_dev_config_t  WBDDRHEXCE1MSPIConfig =
{
  .eAddrCfg             = AM_HAL_MSPI_ADDR_4_BYTE,
  .eInstrCfg            = AM_HAL_MSPI_INSTR_2_BYTE,
  .ui16ReadInstr        = AM_DEVICES_MSPI_PSRAM_W958D6NW_DDR_READ,
  .ui16WriteInstr       = AM_DEVICES_MSPI_PSRAM_W958D6NW_DDR_WRITE,
  .eDeviceConfig        = AM_HAL_MSPI_FLASH_HEX_DDR_CE1,
  .eSpiMode             = AM_HAL_MSPI_SPI_MODE_0,
  .bSendAddr            = true,
  .bSendInstr           = true,
  .bTurnaround          = true,
  .eClockFreq           = AM_HAL_MSPI_CLK_96MHZ,
#ifdef W958D6NW_USE_NON_DQS_MODE
  .ui8TurnAround        = 14,
  .ui8WriteLatency      = 14,
#else
  .ui8TurnAround        = 6,
  .ui8WriteLatency      = 6,
#endif
  .bEnWriteLatency      = true,
  .bEmulateDDR          = true,
#if defined(APOLLO4_FPGA)
  .ui16DMATimeLimit     = 20,
#else
  .ui16DMATimeLimit     = 40,
#endif
  .eDMABoundary         = AM_HAL_MSPI_BOUNDARY_BREAK1K,
};


typedef struct
{
  uint32_t                    ui32Module;
  void                        *pMspiHandle;
  am_hal_mspi_device_e        eDeviceConfig;
  bool                        bOccupied;
} am_devices_mspi_psram_t;

am_devices_mspi_psram_t gWBDDRPsram[AM_DEVICES_MSPI_PSRAM_MAX_DEVICE_NUM];

void pfnMSPI_WBPSRAM_DDR_Callback(void *pCallbackCtxt, uint32_t status)
{
  // Set the DMA complete flag.
  *(volatile bool *)pCallbackCtxt = true;
}

//*****************************************************************************
//
// Generic Command Write function.
//
//*****************************************************************************
static uint32_t
am_device_command_write(void *pMspiHandle,
                        uint16_t ui16Instr,
                        bool bSendAddr,
                        uint32_t ui32Addr,
                        uint32_t *pData,
                        uint32_t ui32NumBytes)
{
  am_hal_mspi_pio_transfer_t  Transaction;

  // Create the individual write transaction.
  Transaction.ui32NumBytes            = ui32NumBytes;
  Transaction.bScrambling             = false;
  Transaction.eDirection              = AM_HAL_MSPI_TX;
  Transaction.bSendAddr               = bSendAddr;
  Transaction.ui32DeviceAddr          = ui32Addr;
  Transaction.bSendInstr              = true;
  Transaction.ui16DeviceInstr         = ui16Instr;
  Transaction.bTurnaround             = false;
  Transaction.bDCX                    = false;
  Transaction.bEnWRLatency            = false;
  Transaction.bContinue               = false;  // MSPI CONT is deprecated for Apollo4
  Transaction.pui32Buffer             = pData;

  // Execute the transction over MSPI.
  return am_hal_mspi_blocking_transfer(pMspiHandle,
                                       &Transaction,
                                       AM_DEVICES_W958D6NW_PSRAM_TIMEOUT );
}

//*****************************************************************************
//
// Generic Command Read function.
//
//*****************************************************************************
static uint32_t
am_device_command_read(void *pMspiHandle,
                       uint16_t ui16Instr,
                       bool bSendAddr,
                       uint32_t ui32Addr,
                       uint32_t *pData,
                       uint32_t ui32NumBytes)
{
  am_hal_mspi_pio_transfer_t  Transaction;

  // Create the individual write transaction.
  Transaction.ui32NumBytes            = ui32NumBytes;
  Transaction.bScrambling             = false;
  Transaction.eDirection              = AM_HAL_MSPI_RX;
  Transaction.bSendAddr               = bSendAddr;
  Transaction.ui32DeviceAddr          = ui32Addr;
  Transaction.bSendInstr              = true;
  Transaction.ui16DeviceInstr         = ui16Instr;
  Transaction.bTurnaround             = true;
  Transaction.bDCX                    = false;
  Transaction.bEnWRLatency            = true;
  Transaction.bContinue               = false;  // MSPI CONT is deprecated for Apollo4
  Transaction.pui32Buffer             = pData;

  // Execute the transction over MSPI.
  return am_hal_mspi_blocking_transfer(pMspiHandle,
                                       &Transaction,
                                       AM_DEVICES_W958D6NW_PSRAM_TIMEOUT );
}

//*****************************************************************************
//
// Reset the external psram
//
//*****************************************************************************
void
am_devices_mspi_psram_w958d6nw_reset(void)
{
  //
  // Pull down PSRAM reset pin low for hardware reset
  //
  am_hal_gpio_pinconfig(AM_BSP_GPIO_PSRAM_RESET, am_hal_gpio_pincfg_output);
  am_hal_gpio_output_set(AM_BSP_GPIO_PSRAM_RESET);
  am_util_delay_us(200);
  am_hal_gpio_output_clear(AM_BSP_GPIO_PSRAM_RESET);
  am_util_delay_us(100);
  am_hal_gpio_output_set(AM_BSP_GPIO_PSRAM_RESET);

}

//*****************************************************************************
//

//*****************************************************************************
//
//! @brief Reads the ID of the external psram and returns the value.
//!
//! @param pDeviceID - Pointer to the return buffer for the Device ID.
//!
//! This function reads the device ID register of the external psram, and returns
//! the result as an 32-bit unsigned integer value.
//!
//! @return 32-bit status
//
//*****************************************************************************
static uint32_t
am_devices_mspi_psram_w958d6nw_id(void *pMspiHandle)
{
  uint32_t     ui32Status;
  uint32_t     ui32Rawdata;
  uint8_t      ui8VendorIDReg = 0;
  uint8_t      ui8DeviceIDReg = 0;
  uint16_t     ui16PsramReg = 0;

  //
  // Read PSRAM Register ID0
  //
  am_util_debug_printf("Read PSRAM Register ID0\n");
  ui32Status = am_device_command_read(pMspiHandle, AM_DEVICES_MSPI_PSRAM_W958D6NW_DDR_READ_REGISTER, true, PSRAM_W958D6NW_REG_ID0_ADDR, &ui32Rawdata, 2);
  if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
  {
      am_util_debug_printf("Failed to read PSRAM Register ID0!\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  else
  {
      ui16PsramReg = (uint16_t)ui32Rawdata;
      am_util_debug_printf("PSRAM Register ID0 = 0x%04X\n", ui16PsramReg);
      ui8VendorIDReg = ui16PsramReg & 0xF;
      if ( ui8VendorIDReg  == 0x6 )
      {
        am_util_debug_printf("PSRAM Vendor ID =  0110(Winbond)\n\n");
      }
      else
      {
        am_util_debug_printf("Fail to get correct PSRAM Vendor ID!\n\n");
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
      }
  }

  //
  // Read PSRAM Register ID1
  //
  am_util_debug_printf("Read PSRAM Register ID1\n");
  ui32Status = am_device_command_read(pMspiHandle, AM_DEVICES_MSPI_PSRAM_W958D6NW_DDR_READ_REGISTER, true, PSRAM_W958D6NW_REG_ID1_ADDR, &ui32Rawdata, 2);
  if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
  {
      am_util_debug_printf("Failed to read PSRAM Register ID1!\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  else
  {
      ui16PsramReg = (uint16_t)ui32Rawdata;
      am_util_debug_printf("PSRAM Register ID1 = 0x%04X\n", ui16PsramReg);
      ui8DeviceIDReg = ui16PsramReg & 0xF;
      if ( ui8DeviceIDReg  == 0x9 )
      {
        am_util_debug_printf("PSRAM Device Type =  1001(HyperIO)\n\n");
      }
      else
      {
        am_util_debug_printf("Fail to get correct PSRAM Device Type!\n\n");
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
      }
  }

  //
  // Read PSRAM Register CFG0
  //
  am_util_debug_printf("Read PSRAM Register CFG0\n");
  ui32Status = am_device_command_read(pMspiHandle, AM_DEVICES_MSPI_PSRAM_W958D6NW_DDR_READ_REGISTER, true, PSRAM_W958D6NW_REG_CFG0_ADDR, &ui32Rawdata, 2);
  if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
  {
      am_util_debug_printf("Failed to read PSRAM Register CFG0!\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  else
  {
      ui16PsramReg = (uint16_t)ui32Rawdata;
      am_util_debug_printf("PSRAM Register CFG0 = 0x%04X\n\n", ui16PsramReg);
  }

  //
  //Change PSRAM register default setting
  //
#ifdef W958D6NW_USE_NON_DQS_MODE
  ui32Rawdata = ( ui32Rawdata & 0xFF00 ) | 0x2C; // Fixed Latency(7 clock), burst length=128
  am_util_debug_printf("Set PSRAM Register CFG0 for NON-DQS mode\n");
#else
  ui32Rawdata = ( ui32Rawdata & 0xFF00 ) | 0xE4; // Variable Latency(3 clock), burst length=128
  am_util_debug_printf("Set PSRAM Register CFG0 for DQS mode\n");
#endif

  //
  // Write PSRAM Register CFG0
  //
  ui32Status = am_device_command_write(pMspiHandle, AM_DEVICES_MSPI_PSRAM_W958D6NW_DDR_WRITE_REGISTER, true, PSRAM_W958D6NW_REG_CFG0_ADDR, &ui32Rawdata, 2);
  if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
  {
      am_util_debug_printf("Failed to write PSRAM Register CFG0!\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  else
  {
     am_util_debug_printf("Set PSRAM Register CFG0 into 0x%04X\n", ui32Rawdata);
  }


  //
  // Read PSRAM Register CFG0 again
  //
  am_util_debug_printf("Read PSRAM Register CFG0\n");
  ui32Status = am_device_command_read(pMspiHandle, AM_DEVICES_MSPI_PSRAM_W958D6NW_DDR_READ_REGISTER, true, PSRAM_W958D6NW_REG_CFG0_ADDR, &ui32Rawdata, 2);
  if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
  {
      am_util_debug_printf("Failed to read PSRAM Register CFG0!\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  else
  {
      ui16PsramReg = (uint16_t)ui32Rawdata;
      am_util_debug_printf("PSRAM Register CFG0 = 0x%04X\n\n", ui16PsramReg);
  }

  //
  // Read PSRAM Register CFG1
  //
  am_util_debug_printf("Read PSRAM Register CFG1\n");
  ui32Status = am_device_command_read(pMspiHandle, AM_DEVICES_MSPI_PSRAM_W958D6NW_DDR_READ_REGISTER, true, PSRAM_W958D6NW_REG_CFG1_ADDR, &ui32Rawdata, 2);
  if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
  {
      am_util_debug_printf("Failed to read PSRAM Register CFG1!\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  else
  {
      ui16PsramReg = (uint16_t)ui32Rawdata;
      am_util_debug_printf("PSRAM Register CFG1 = 0x%04X\n\n", ui16PsramReg);
  }

  return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;

}

// This function takes care of splitting the transaction as needed, if the transaction crosses
// PSRAM page boundary or because of tCEM restrictions, if hardware does not support it
static uint32_t
psram_nonblocking_transfer(am_devices_mspi_psram_t *pPsram,
                           bool bHiPrio,
                           bool bWrite,
                           uint8_t *pui8Buffer,
                           uint32_t ui32Address,
                           uint32_t ui32NumBytes,
                           uint32_t ui32PauseCondition,
                           uint32_t ui32StatusSetClr,
                           am_hal_mspi_callback_t pfnCallback,
                           void *pCallbackCtxt)
{
  uint32_t ui32Status = AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
  am_hal_mspi_dma_transfer_t    Transaction;

  // Set the DMA priority
  Transaction.ui8Priority = 1;


  // Set the transfer direction to RX (Read)
  Transaction.eDirection = bWrite ? AM_HAL_MSPI_TX: AM_HAL_MSPI_RX;


  // Initialize the CQ stimulus.
  Transaction.ui32PauseCondition = ui32PauseCondition;
  // Initialize the post-processing
  Transaction.ui32StatusSetClr = 0;

  // Need to be aware of page size
  while (ui32NumBytes)
  {
    uint32_t size;
    if ((ui32Address & 0x3) &&
        ((AM_DEVICES_MSPI_PSRAM_PAGE_SIZE - (ui32Address & (AM_DEVICES_MSPI_PSRAM_PAGE_SIZE - 1))) < ui32NumBytes))
    {
      // Hardware does not support Page splitting if address is not word aligned
      // Need to split the transaction
      size = 4 - (ui32Address & 0x3);
    }
    else
    {
      size = ui32NumBytes;
    }

    bool bLast = (size == ui32NumBytes);
    // Set the transfer count in bytes.
    Transaction.ui32TransferCount = size;

    // Set the address to read data from.
    Transaction.ui32DeviceAddress = ui32Address;

    // Set the target SRAM buffer address.
    Transaction.ui32SRAMAddress = (uint32_t)pui8Buffer;

    if (bLast)
    {
      Transaction.ui32StatusSetClr = ui32StatusSetClr;
    }

    if (bHiPrio)
    {
      ui32Status = am_hal_mspi_highprio_transfer(pPsram->pMspiHandle, &Transaction, AM_HAL_MSPI_TRANS_DMA,
                                                 bLast ? pfnCallback : NULL,
                                                 bLast ? pCallbackCtxt : NULL);
    }
    else
    {
      ui32Status = am_hal_mspi_nonblocking_transfer(pPsram->pMspiHandle, &Transaction, AM_HAL_MSPI_TRANS_DMA,
                                                    bLast ? pfnCallback : NULL,
                                                    bLast ? pCallbackCtxt : NULL);
    }
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
      break;
    }
    ui32Address += size;
    ui32NumBytes -= size;
    pui8Buffer += size;

    Transaction.ui32PauseCondition = 0;
  }
  return ui32Status;
}


//*****************************************************************************
//
//! @brief Initialize the mspi_psram driver.
//!
//! @param psMSPISettings - MSPI device structure describing the target spi psram.
//! @param pHandle - MSPI handler which needs to be return
//!
//! This function should be called before any other am_devices_mspi_psram
//! functions. It is used to set tell the other functions how to communicate
//! with the external psram hardware.
//!
//! @return status.
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_w958d6nw_ddr_init(uint32_t ui32Module, am_devices_mspi_psram_config_t *pDevCfg, void **ppHandle, void **ppMspiHandle)
{
    uint32_t                    ui32Status;
    am_hal_mspi_dev_config_t    mspiMemDevCfg, mspiRegDevCfg;
    void                        *pMspiHandle;
    uint32_t                    ui32Index = 0;

    if ((ui32Module > AM_REG_MSPI_NUM_MODULES) || (pDevCfg == NULL))
    {
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    //
    // Enable fault detection.
    //
    am_hal_fault_capture_enable();

    // Allocate a vacant device handle
    for ( ui32Index = 0; ui32Index < AM_DEVICES_MSPI_PSRAM_MAX_DEVICE_NUM; ui32Index++ )
    {
        if ( gWBDDRPsram[ui32Index].bOccupied == false )
        {
            break;
        }
    }
    if ( ui32Index == AM_DEVICES_MSPI_PSRAM_MAX_DEVICE_NUM)
    {
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    am_util_debug_printf("\nStart PSRAM Initialization\n");

    //
    // Configure the MSPI into Octal mode for PSRAM register access and Hex mode for data access.
    //
    switch (pDevCfg->eDeviceConfig)
    {
        case AM_HAL_MSPI_FLASH_OCTAL_DDR_CE0:
            mspiMemDevCfg = WBDDROctalCE0MSPIConfig;
            mspiRegDevCfg = WBDDROctalCE0MSPIConfig;
            break;
        case AM_HAL_MSPI_FLASH_OCTAL_DDR_CE1:
            mspiMemDevCfg = WBDDROctalCE1MSPIConfig;
            mspiRegDevCfg = WBDDROctalCE1MSPIConfig;
            break;
        case AM_HAL_MSPI_FLASH_HEX_DDR_CE0:
            mspiMemDevCfg = WBDDRHEXCE0MSPIConfig;
            mspiRegDevCfg = WBDDROctalCE0MSPIConfig;
            break;
        case AM_HAL_MSPI_FLASH_HEX_DDR_CE1:
            mspiMemDevCfg = WBDDRHEXCE1MSPIConfig;
            mspiRegDevCfg = WBDDROctalCE1MSPIConfig;
            break;
        default:
            return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }
    mspiMemDevCfg.eClockFreq = pDevCfg->eClockFreq;
    mspiRegDevCfg.eClockFreq = pDevCfg->eClockFreq;

    // First configure in HEX mode and reset
    if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_initialize(ui32Module, &pMspiHandle))
    {
        am_util_debug_printf("Error - Failed to initialize MSPI.\n");
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_power_control(pMspiHandle, AM_HAL_SYSCTRL_WAKE, false))
    {
        am_util_debug_printf("Error - Failed to power on MSPI.\n");
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    am_hal_mspi_config_t    mspiCfg = gWBDDRMspiCfg;
    mspiCfg.ui32TCBSize = pDevCfg->ui32NBTxnBufLength;
    mspiCfg.pTCB = pDevCfg->pNBTxnBuf;
    if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_configure(pMspiHandle, &mspiCfg))
    {
        am_util_debug_printf("Error - Failed to configure MSPI device.\n");
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_device_configure(pMspiHandle, &mspiRegDevCfg))
    {
        am_util_debug_printf("Error - Failed to configure MSPI device.\n");
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    am_hal_mspi_xip_config_t    xipCfg = gWBDDRXipConfig[ui32Module];

    xipCfg.scramblingStartAddr = pDevCfg->ui32ScramblingStartAddr;
    xipCfg.scramblingEndAddr = pDevCfg->ui32ScramblingEndAddr;
    ui32Status = am_hal_mspi_control(pMspiHandle, AM_HAL_MSPI_REQ_XIP_CONFIG, &xipCfg);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    am_hal_mspi_xip_misc_t    xipMiscCfg = gWBXipMiscCfg[ui32Module];
    ui32Status = am_hal_mspi_control(pMspiHandle, AM_HAL_MSPI_REQ_XIP_MISC_CONFIG, &xipMiscCfg);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    am_hal_mspi_dqs_t dqsCfg = gWBDDRDqsCfg[ui32Module];
    ui32Status = am_hal_mspi_control(pMspiHandle, AM_HAL_MSPI_REQ_DQS, &dqsCfg);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    am_hal_mspi_rxcfg_t RxCfg = gWBMspiRxCfg;
    if (AM_HAL_MSPI_CLK_96MHZ == pDevCfg->eClockFreq)
    {
      RxCfg.ui8RxSmp = 2;
    }
    ui32Status = am_hal_mspi_control(pMspiHandle, AM_HAL_MSPI_REQ_RXCFG, &RxCfg);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    //
    // Enable DDR emulation in MSPI
    //
    ui32Status = am_hal_mspi_control(pMspiHandle, AM_HAL_MSPI_REQ_DDR_EN, NULL);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_enable(pMspiHandle))
    {
        am_util_debug_printf("Error - Failed to enable MSPI.\n");
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }
    am_bsp_mspi_pins_enable(ui32Module, mspiRegDevCfg.eDeviceConfig);

    am_util_delay_us(150);

    //
    // Reset PSRAM
    //
    am_devices_mspi_psram_w958d6nw_reset();

    //
    // Switch to big endian for accessing PSRAM registers
    //
    ui32Status = am_hal_mspi_control(pMspiHandle, AM_HAL_MSPI_REQ_BIG_ENDIAN, NULL);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    //
    // Read PSRAM ID
    //
    if (AM_HAL_STATUS_SUCCESS != am_devices_mspi_psram_w958d6nw_id(pMspiHandle))
    {
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    //
    // Switch back to little endian for accessing PSRAM memory
    //
    ui32Status = am_hal_mspi_control(pMspiHandle, AM_HAL_MSPI_REQ_LITTLE_ENDIAN, NULL);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    //
    // Disable MSPI defore re-configuring it
    //
    ui32Status = am_hal_mspi_disable(pMspiHandle);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    //
    // Reconfig MSPI device settings
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_device_configure(pMspiHandle, &mspiMemDevCfg))
    {
        am_util_debug_printf("Error - Failed to reconfig MSPI device.\n");
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    //
    // Re-Enable MSPI
    //
    ui32Status = am_hal_mspi_enable(pMspiHandle);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    //
    // Re-config the MSPI pins.
    //
    am_bsp_mspi_pins_enable(ui32Module, mspiMemDevCfg.eDeviceConfig);

    //
    // Enable MSPI interrupts.
    //
    ui32Status = am_hal_mspi_interrupt_clear(pMspiHandle, AM_HAL_MSPI_INT_CQUPD | AM_HAL_MSPI_INT_ERR );
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    ui32Status = am_hal_mspi_interrupt_enable(pMspiHandle, AM_HAL_MSPI_INT_CQUPD | AM_HAL_MSPI_INT_ERR );
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    //
    // Return the handle.
    //
    gWBDDRPsram[ui32Index].bOccupied = true;
    *ppHandle = (void *)&gWBDDRPsram[ui32Index];
    *ppMspiHandle = gWBDDRPsram[ui32Index].pMspiHandle = pMspiHandle;
    gWBDDRPsram[ui32Index].ui32Module = ui32Module;
    gWBDDRPsram[ui32Index].eDeviceConfig = mspiMemDevCfg.eDeviceConfig;

    //
    // Return the status.
    //
    return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief DeInitialize the mspi_psram driver.
//!
//! @param psMSPISettings - MSPI device structure describing the target spi psram.
//! @param pHandle - MSPI handler.
//!
//! @return status.
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_w958d6nw_ddr_deinit(void *pHandle)
{
    uint32_t    ui32Status;
    am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;

    //
    // Disable and clear the interrupts to start with.
    //
    ui32Status = am_hal_mspi_interrupt_disable(pPsram->pMspiHandle, 0xFFFFFFFF);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }
    ui32Status = am_hal_mspi_interrupt_clear(pPsram->pMspiHandle, 0xFFFFFFFF);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    //
    // Disable MSPI instance.
    //
    ui32Status = am_hal_mspi_disable(pPsram->pMspiHandle);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }
    //
    // Disable power to the MSPI instance.
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_power_control(pPsram->pMspiHandle, AM_HAL_SYSCTRL_DEEPSLEEP, false))
    {
        am_util_debug_printf("Error - Failed to power on MSPI.\n");
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }
    //
    // Deinitialize the MPSI instance.
    //
    ui32Status = am_hal_mspi_deinitialize(pPsram->pMspiHandle);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    // Free this device handle
    pPsram->bOccupied = false;

    //
    // Return the status.
    //
    return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief Reads the contents of the external PSRAM into a buffer.
//!
//! @param ui32Module - MSPI instance
//! @param pui8RxBuffer - Buffer to store the received data from the PSRAM
//! @param ui32ReadAddress - Address of desired data in external PSRAM
//! @param ui32NumBytes - Number of bytes to read from external PSRAM
//! @param bWaitForCompletion - Wait for transaction completion before exiting
//!
//! This function reads the external PSRAM at the provided address and stores
//! the received data into the provided buffer location. This function will
//! only store ui32NumBytes worth of data.  If the bWaitForCompletion is true,
//! then the function will poll for DMA completion indication flag before
//! returning.
//
//! @return 32-bit status
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_w958d6nw_ddr_read(void *pHandle,
                                        uint8_t *pui8RxBuffer,
                                        uint32_t ui32ReadAddress,
                                        uint32_t ui32NumBytes,
                                        bool bWaitForCompletion)
{
  uint32_t                      ui32Status;
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;

  if (bWaitForCompletion)
  {
    // Start the transaction.
    volatile bool bDMAComplete = false;
    ui32Status = psram_nonblocking_transfer(pPsram, false, false,
                                            pui8RxBuffer,
                                            ui32ReadAddress,
                                            ui32NumBytes,
                                            0,
                                            0,
                                            pfnMSPI_WBPSRAM_DDR_Callback,
                                            (void *)&bDMAComplete);

    // Check the transaction status.
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    // Wait for DMA Complete or Timeout
    for (uint32_t i = 0; i < AM_DEVICES_W958D6NW_PSRAM_TIMEOUT ; i++)
    {
      if (bDMAComplete)
      {
        break;
      }
      //
      // Call the BOOTROM cycle function to delay for about 1 microsecond.
      //
      am_hal_delay_us(1);
    }

    // Check the status.
    if (!bDMAComplete)
    {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }
  }
  else
  {
    // Check the transaction status.
    ui32Status = psram_nonblocking_transfer(pPsram, false, false,
                                            pui8RxBuffer,
                                            ui32ReadAddress,
                                            ui32NumBytes,
                                            0,
                                            0,
                                            NULL,
                                            NULL);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }
  }
  //
  // Return the status.
  //
  return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief Reads the contents of the external PSRAM into a buffer.
//!
//! @param pHandle - MSPI instance
//! @param pui8RxBuffer - Buffer to store the received data from the PSRAM
//! @param ui32ReadAddress - Address of desired data in external PSRAM
//! @param ui32NumBytes - Number of bytes to read from external PSRAM
//! @param ui32PauseCondition - Pause condition before transaction is executed
//! @param ui32StatusSetClr - Post-transaction CQ condition
//! @param pfnCallback - Post-transaction callback function
//! @param pCallbackCtxt - Post-transaction callback context
//!
//! This function reads the external PSRAM at the provided address and stores
//! the received data into the provided buffer location. This function will
//! only store ui32NumBytes worth of data.  The Command Queue pre and post
//! transaction conditions and a callback function and context are also
//! provided.
//
//! @return 32-bit status
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_w958d6nw_ddr_read_adv(void *pHandle,
                                            uint8_t *pui8RxBuffer,
                                            uint32_t ui32ReadAddress,
                                            uint32_t ui32NumBytes,
                                            uint32_t ui32PauseCondition,
                                            uint32_t ui32StatusSetClr,
                                            am_hal_mspi_callback_t pfnCallback,
                                            void *pCallbackCtxt)
{
  uint32_t                      ui32Status;
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;

  ui32Status = psram_nonblocking_transfer(pPsram, false, false,
                                          pui8RxBuffer,
                                          ui32ReadAddress,
                                          ui32NumBytes,
                                          ui32PauseCondition,
                                          ui32StatusSetClr,
                                          pfnCallback,
                                          pCallbackCtxt);

  // Check the transaction status.
  if (AM_HAL_STATUS_SUCCESS != ui32Status)
  {
    return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  //
  // Return the status.
  //
  return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief Reads the contents of the external psram into a buffer.
//!
//! @param pui8RxBuffer - Buffer to store the received data from the psram
//! @param ui32ReadAddress - Address of desired data in external psram
//! @param ui32NumBytes - Number of bytes to read from external psram
//!
//! This function reads the external psram at the provided address and stores
//! the received data into the provided buffer location. This function will
//! only store ui32NumBytes worth of data.
//
//! @return 32-bit status
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_w958d6nw_ddr_read_hiprio(void *pHandle,
                                               uint8_t *pui8RxBuffer,
                                               uint32_t ui32ReadAddress,
                                               uint32_t ui32NumBytes,
                                               am_hal_mspi_callback_t pfnCallback,
                                               void *pCallbackCtxt)
{
  uint32_t                      ui32Status;
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;

  ui32Status = psram_nonblocking_transfer(pPsram, true, false,
                                          pui8RxBuffer,
                                          ui32ReadAddress,
                                          ui32NumBytes,
                                          0,
                                          0,
                                          pfnCallback,
                                          pCallbackCtxt);

  // Check the transaction status.
  if (AM_HAL_STATUS_SUCCESS != ui32Status)
  {
    return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  //
  // Return the status.
  //
  return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

uint32_t
am_devices_mspi_psram_w958d6nw_ddr_nonblocking_read(void *pHandle,
                                                    uint8_t *pui8RxBuffer,
                                                    uint32_t ui32ReadAddress,
                                                    uint32_t ui32NumBytes,
                                                    am_hal_mspi_callback_t pfnCallback,
                                                    void *pCallbackCtxt)
{
  uint32_t                      ui32Status;
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;

  // Check the transaction status.
  ui32Status = psram_nonblocking_transfer(pPsram, false, false,
                                          pui8RxBuffer,
                                          ui32ReadAddress,
                                          ui32NumBytes,
                                          0,
                                          0,
                                          pfnCallback,
                                          pCallbackCtxt);
  if (AM_HAL_STATUS_SUCCESS != ui32Status)
  {
    return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  //
  // Return the status.
  //
  return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}


//*****************************************************************************
//
//! @brief Programs the given range of psram addresses.
//!
//! @param ui32DeviceNumber - Device number of the external psram
//! @param pui8TxBuffer - Buffer to write the external psram data from
//! @param ui32WriteAddress - Address to write to in the external psram
//! @param ui32NumBytes - Number of bytes to write to the external psram
//!
//! This function uses the data in the provided pui8TxBuffer and copies it to
//! the external psram at the address given by ui32WriteAddress. It will copy
//! exactly ui32NumBytes of data from the original pui8TxBuffer pointer. The
//! user is responsible for ensuring that they do not overflow the target psram
//! memory or underflow the pui8TxBuffer array
//
//! @return 32-bit status
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_w958d6nw_ddr_write(void *pHandle,
                                         uint8_t *pui8TxBuffer,
                                         uint32_t ui32WriteAddress,
                                         uint32_t ui32NumBytes,
                                         bool bWaitForCompletion)
{
  uint32_t                      ui32Status;
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;

  if (bWaitForCompletion)
  {
    // Start the transaction.
    volatile bool bDMAComplete = false;
    ui32Status = psram_nonblocking_transfer(pPsram, false, true,
                                            pui8TxBuffer,
                                            ui32WriteAddress,
                                            ui32NumBytes,
                                            0,
                                            0,
                                            pfnMSPI_WBPSRAM_DDR_Callback,
                                            (void *)&bDMAComplete);

    // Check the transaction status.
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    // Wait for DMA Complete or Timeout
    for (uint32_t i = 0; i < AM_DEVICES_W958D6NW_PSRAM_TIMEOUT ; i++)
    {
      if (bDMAComplete)
      {
        break;
      }
      //
      // Call the BOOTROM cycle function to delay for about 1 microsecond.
      //
      am_hal_delay_us(1);
    }

    // Check the status.
    if (!bDMAComplete)
    {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }
  }
  else
  {
    // Check the transaction status.
    ui32Status = psram_nonblocking_transfer(pPsram, false, true,
                                            pui8TxBuffer,
                                            ui32WriteAddress,
                                            ui32NumBytes,
                                            0,
                                            0,
                                            NULL,
                                            NULL);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }
  }

  //
  // Return the status.
  //
  return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief Programs the given range of psram addresses.
//!
//! @param ui32DeviceNumber - Device number of the external psram
//! @param pui8TxBuffer - Buffer to write the external psram data from
//! @param ui32WriteAddress - Address to write to in the external psram
//! @param ui32NumBytes - Number of bytes to write to the external psram
//!
//! This function uses the data in the provided pui8TxBuffer and copies it to
//! the external psram at the address given by ui32WriteAddress. It will copy
//! exactly ui32NumBytes of data from the original pui8TxBuffer pointer. The
//! user is responsible for ensuring that they do not overflow the target psram
//! memory or underflow the pui8TxBuffer array
//
//! @return 32-bit status
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_w958d6nw_ddr_write_adv(void *pHandle,
                                             uint8_t *puiTxBuffer,
                                             uint32_t ui32WriteAddress,
                                             uint32_t ui32NumBytes,
                                             uint32_t ui32PauseCondition,
                                             uint32_t ui32StatusSetClr,
                                             am_hal_mspi_callback_t pfnCallback,
                                             void *pCallbackCtxt)
{
  uint32_t                      ui32Status;
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;

  ui32Status = psram_nonblocking_transfer(pPsram, false, true,
                                          puiTxBuffer,
                                          ui32WriteAddress,
                                          ui32NumBytes,
                                          ui32PauseCondition,
                                          ui32StatusSetClr,
                                          pfnCallback,
                                          pCallbackCtxt);

  // Check the transaction status.
  if (AM_HAL_STATUS_SUCCESS != ui32Status)
  {
    return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  //
  // Return the status.
  //
  return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief Programs the given range of psram addresses.
//!
//! @param ui32DeviceNumber - Device number of the external psram
//! @param pui8TxBuffer - Buffer to write the external psram data from
//! @param ui32WriteAddress - Address to write to in the external psram
//! @param ui32NumBytes - Number of bytes to write to the external psram
//!
//! This function uses the data in the provided pui8TxBuffer and copies it to
//! the external psram at the address given by ui32WriteAddress. It will copy
//! exactly ui32NumBytes of data from the original pui8TxBuffer pointer. The
//! user is responsible for ensuring that they do not overflow the target psram
//! memory or underflow the pui8TxBuffer array
//
//! @return 32-bit status
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_w958d6nw_ddr_write_hiprio(void *pHandle,
                                                uint8_t *pui8TxBuffer,
                                                uint32_t ui32WriteAddress,
                                                uint32_t ui32NumBytes,
                                                am_hal_mspi_callback_t pfnCallback,
                                                void *pCallbackCtxt)
{
  uint32_t                      ui32Status;
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;

  // Check the transaction status.
  ui32Status = psram_nonblocking_transfer(pPsram, true, true,
                                          pui8TxBuffer,
                                          ui32WriteAddress,
                                          ui32NumBytes,
                                          0,
                                          0,
                                          pfnCallback,
                                          pCallbackCtxt);

  // Check the transaction status.
  if (AM_HAL_STATUS_SUCCESS != ui32Status)
  {
    return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  //
  // Return the status.
  //
  return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}
uint32_t
am_devices_mspi_psram_w958d6nw_ddr_nonblocking_write(void *pHandle,
                                                     uint8_t *pui8TxBuffer,
                                                     uint32_t ui32WriteAddress,
                                                     uint32_t ui32NumBytes,
                                                     am_hal_mspi_callback_t pfnCallback,
                                                     void *pCallbackCtxt)
{
  uint32_t                      ui32Status;
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;

  // Check the transaction status.
  ui32Status = psram_nonblocking_transfer(pPsram, false, true,
                                          pui8TxBuffer,
                                          ui32WriteAddress,
                                          ui32NumBytes,
                                          0,
                                          0,
                                          pfnCallback,
                                          pCallbackCtxt);
  if (AM_HAL_STATUS_SUCCESS != ui32Status)
  {
    return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  //
  // Return the status.
  //
  return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}


//*****************************************************************************
//
//! @brief Sets up the MSPI and external psram into XIP mode.
//!
//! This function sets the external psram device and the MSPI into XIP mode.
//
//! @return 32-bit status
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_w958d6nw_ddr_enable_xip(void *pHandle)
{
  uint32_t ui32Status;
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;

  //
  // Set Aperture XIP range
  //
  ui32Status = am_hal_mspi_control(pPsram->pMspiHandle, AM_HAL_MSPI_REQ_XIP_CONFIG, &gWBDDRXipConfig[pPsram->ui32Module]);
  if (AM_HAL_STATUS_SUCCESS != ui32Status)
  {
    return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  //
  // Enable XIP on the MSPI.
  //
  ui32Status = am_hal_mspi_control(pPsram->pMspiHandle, AM_HAL_MSPI_REQ_XIP_EN, &gWBDDRXipConfig[pPsram->ui32Module]);
  if (AM_HAL_STATUS_SUCCESS != ui32Status)
  {
    return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief Removes the MSPI and external psram from XIP mode.
//!
//! This function removes the external device and the MSPI from XIP mode.
//
//! @return 32-bit status
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_w958d6nw_ddr_disable_xip(void *pHandle)
{
  uint32_t ui32Status;
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;

  //
  // Disable XIP on the MSPI.
  //
  ui32Status = am_hal_mspi_control(pPsram->pMspiHandle, AM_HAL_MSPI_REQ_XIP_DIS, &gWBDDRXipConfig[pPsram->ui32Module]);
  if (AM_HAL_STATUS_SUCCESS != ui32Status)
  {
    return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief Sets up the MSPI and external psram into scrambling mode.
//!
//! This function sets the external psram device and the MSPI into scrambling mode.
//
//! @return 32-bit status
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_w958d6nw_ddr_enable_scrambling(void *pHandle)
{
  uint32_t ui32Status;
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;

  //
  // Enable scrambling on the MSPI.
  //
  ui32Status = am_hal_mspi_control(pPsram->pMspiHandle, AM_HAL_MSPI_REQ_SCRAMB_EN, &gWBDDRXipConfig[pPsram->ui32Module]);
  if (AM_HAL_STATUS_SUCCESS != ui32Status)
  {
    return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief Removes the MSPI and external psram from scrambling mode.
//!
//! This function removes the external device and the MSPI from scrambling mode.
//
//! @return 32-bit status
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_w958d6nw_ddr_disable_scrambling(void *pHandle)
{
  uint32_t ui32Status;
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;

  //
  // Disable Scrambling on the MSPI.
  //
  ui32Status = am_hal_mspi_control(pPsram->pMspiHandle, AM_HAL_MSPI_REQ_SCRAMB_DIS, &gWBDDRXipConfig[pPsram->ui32Module]);
  if (AM_HAL_STATUS_SUCCESS != ui32Status)
  {
    return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief Reads the ID of the external psram and returns the value.
//!
//! @param pDeviceID - Pointer to the return buffer for the Device ID.
//!
//! This function reads the device ID register of the external psram, and returns
//! the result as an 32-bit unsigned integer value.
//!
//! @return 32-bit status
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_w958d6nw_ddr_id(void *pHandle)
{
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;

  return am_devices_mspi_psram_w958d6nw_id(pPsram);
}

#if defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
//*****************************************************************************
//
//! @brief write and read back check.
//!
//! @param psMSPISettings - MSPI device structure describing the target spi psram.
//! @param pHandle - MSPI handler which needs to be return
//!
//! This function should be called before any other am_devices_mspi_psram
//! functions. It is used to set tell the other functions how to communicate
//! with the external psram hardware.
//!
//! @return status.
//
//*****************************************************************************
#define PSRAM_CHECK_DATA_SIZE_BYTES  256
static int prepare_test_pattern(uint32_t pattern_index, uint8_t* buff, uint32_t len)
{
    uint32_t *pui32TxPtr = (uint32_t*)buff;
    uint8_t  *pui8TxPtr  = (uint8_t*)buff;

    // length has to be multiple of 4 bytes
    if ( len % 4 )
    {
        return -1;
    }

    switch ( pattern_index )
    {
        case 0:
            // 0x5555AAAA
            for (uint32_t i = 0; i < len / 4; i++)
            {
               pui32TxPtr[i] = (0x5555AAAA);
            }
            break;
        case 1:
            // 0xFFFF0000
            for (uint32_t i = 0; i < len / 4; i++)
            {
               pui32TxPtr[i] = (0xFFFF0000);
            }
            break;
        case 2:
            // walking
            for (uint32_t i = 0; i < len; i++)
            {
               pui8TxPtr[i] = 0x01 << (i % 8);
            }
            break;
        case 3:
            // incremental from 1
            for (uint32_t i = 0; i < len; i++)
            {
               pui8TxPtr[i] = ((i + 1) & 0xFF);
            }
            break;
        case 4:
            // decremental from 0xff
            for ( uint32_t i = 0; i < len; i++ )
            {
                // decrement starting from 0xff
                pui8TxPtr[i] = (0xff - i) & 0xFF;
            }
            break;
        default:
            // incremental from 1
            for (uint32_t i = 0; i < len; i++)
            {
               pui8TxPtr[i] = ((i + 1) & 0xFF);
            }
            break;

    }

    return 0;
}

bool
psram_check(uint32_t length, uint32_t address)
{
    // Try to use as less ram as possible in stack
    uint32_t ui32NumberOfBytesLeft = length;
    uint32_t ui32TestBytes = 0;
    uint32_t ui32AddressOffset = 0;
    uint8_t ui8PatternCounter = 0;
    uint8_t ui8TxBuffer[PSRAM_CHECK_DATA_SIZE_BYTES];
    uint8_t ui8RxBuffer[PSRAM_CHECK_DATA_SIZE_BYTES];

    while ( ui32NumberOfBytesLeft )
    {
        if ( ui32NumberOfBytesLeft > PSRAM_CHECK_DATA_SIZE_BYTES )
        {
            ui32TestBytes = PSRAM_CHECK_DATA_SIZE_BYTES;
            ui32NumberOfBytesLeft -= PSRAM_CHECK_DATA_SIZE_BYTES;
        }
        else
        {
            ui32TestBytes = ui32NumberOfBytesLeft;
            ui32NumberOfBytesLeft = 0;
        }

        //
        // Write to target address with test pattern with given length
        // Use 5 patterns: 0x5555AAAA, 0xFFFF0000, Walking, incremental and decremental
        //

        prepare_test_pattern((ui8PatternCounter) % 5, ui8TxBuffer, ui32TestBytes);
        ui8PatternCounter++;

        // write to target address
        am_hal_sysctrl_bus_write_flush();
        uint8_t * xipPointer = (uint8_t *)(address + ui32AddressOffset);
        memcpy(xipPointer, (uint8_t*)ui8TxBuffer, ui32TestBytes);

        //
        // Read back data
        //
        am_hal_sysctrl_bus_write_flush();
        xipPointer = (uint8_t *)(address + ui32AddressOffset);
        memcpy((uint8_t*)ui8RxBuffer, xipPointer, ui32TestBytes);

        //
        // Verify the result
        //
        if ( memcmp(ui8RxBuffer, ui8TxBuffer, ui32TestBytes) )
        {
            //am_util_debug_printf("    Failed to verify at offset 0x%08x!\n", ui32AddressOffset);
            // verify failed, return directly
            return true;
        }

        ui32AddressOffset += ui32TestBytes;
    }

    return false;
}

//
// Static helper function:
//  Count the longest consecutive 1s in a 32bit word
//
static uint32_t
count_consecutive_ones(uint32_t* pVal)
{
    uint32_t count = 0;
    uint32_t data = *pVal;

    while ( data )
    {
        data = (data & (data << 1));
        count++;
    }
    return count;
}

//
// Static helper function:
//  Find and return the mid point of the longest continuous 1s in a 32bit word
//
static uint32_t
find_mid_point(uint32_t* pVal)
{
    uint32_t pattern_len = 0;
    uint32_t max_len = 0;
    uint32_t pick_point = 0;
    bool pattern_start = false;
    uint32_t val = *pVal;
    uint8_t remainder = 0;
    bool pick_point_flag = false;

    for ( uint32_t i = 0; i < 32; i++ )
    {
        if ( val & (0x01 << i) )
        {
            pattern_start = true;
            pattern_len++;
        }
        else
        {
            if ( pattern_start == true )
            {
                pattern_start = false;
                pick_point_flag = true;
            }
        }
        if ( (i == 31) && ( pattern_start == true ) )
        {
            pick_point_flag = true;
        }

        if (pick_point_flag == true)
        {
            if ( pattern_len > max_len )
            {
                max_len = pattern_len;
                pick_point = i - 1 - pattern_len / 2;
                remainder = pattern_len % 2;
            }
            pattern_len = 0;
            pick_point_flag = false;
        }
    }

    //
    // check the passing window side
    //

    if ( (pick_point < 16) && (val & 0x00000002) )
    {
        // window is likely on low side
        pick_point = pick_point - remainder;    // minus only when pattern length is odd
    }
    else if ( (pick_point > 15) && (val & 0x40000000) )
    {
        // window is likely on high side
        pick_point = pick_point + 1;
    }
    else
    {
        // window is in the middle, no action
    }

    return pick_point;
}

//*****************************************************************************
//
//! @brief Checks PSRAM timing and determine a delay setting.
//!
//! @param pDeviceID - Pointer to the return buffer for the Device ID.
//!
//! This function scans through the delay settings of MSPI DDR mode and selects
//! the best parameter to use by tuning TURNAROUND/RXNEG/RXDQSDELAY0 values.
//! This function is only valid in DDR mode and ENABLEDQS0 = 0.
//!
//! @return 32-bit status, scan result in structure type
//
//*****************************************************************************
#define PSRAM_TIMING_SCAN_SIZE_BYTES (128*1024)
static const uint32_t ui32MspiXipBaseAddress[3] =
{
    0x14000000, // mspi0
    0x18000000, // mspi1
    0x1C000000, // mspi2
};

uint32_t
am_devices_mspi_psram_hex_ddr_init_timing_check(uint32_t module,
                                                am_devices_mspi_psram_config_t *pDevCfg,
                                                am_devices_mspi_psram_ddr_timing_config_t *pDevDdrCfg)
{
    uint32_t ui32Status;
    void *pDevHandle;
    void *pHandle;
    uint32_t ui32ResultArray;

    am_hal_mspi_dqs_t scanCfg = gWBDDRDqsCfg[module];
    //
    // initialize interface
    //
    am_hal_mspi_dev_config_t    *psMSPISettings;
    switch (pDevCfg->eDeviceConfig)
    {
        case AM_HAL_MSPI_FLASH_HEX_DDR_CE0:
            psMSPISettings = &WBDDRHEXCE0MSPIConfig;
            break;
        case AM_HAL_MSPI_FLASH_HEX_DDR_CE1:
            psMSPISettings = &WBDDRHEXCE1MSPIConfig;
            break;
        default:
            return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    uint16_t timeLimit = psMSPISettings->ui16DMATimeLimit;                  // save original setting here
    am_hal_mspi_dma_boundary_e dmaBound0 = psMSPISettings->eDMABoundary;    // save original setting here
    psMSPISettings->ui16DMATimeLimit    = 0;
    psMSPISettings->eDMABoundary        = AM_HAL_MSPI_BOUNDARY_NONE;
    ui32Status = am_devices_mspi_psram_w958d6nw_ddr_init(module, pDevCfg, &pDevHandle, &pHandle);
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        am_util_debug_printf("    Failed to configure the MSPI and PSRAM Device correctly!\n");
        return ui32Status;
    }
    psMSPISettings->ui16DMATimeLimit = timeLimit;   // restore original setting here
    psMSPISettings->eDMABoundary = dmaBound0;       // restore original setting here

    //
    // Put the MSPI into XIP mode.
    //
    ui32Status = am_devices_mspi_psram_w958d6nw_ddr_enable_xip(pDevHandle);
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        am_util_debug_printf("    Failed to disable XIP mode in the MSPI!\n");
        return ui32Status;
    }

    //
    // Start scan loop
    //
    for ( uint8_t RxDqs_Index = 0; RxDqs_Index <= 31; RxDqs_Index++ )
    {
        // set RXDQSDELAY0 value
        scanCfg.ui8RxDQSDelay   = RxDqs_Index;
        // apply settings
        ui32Status = am_hal_mspi_control(pHandle, AM_HAL_MSPI_REQ_DQS, &scanCfg);
        if (AM_HAL_STATUS_SUCCESS != ui32Status)
        {
            return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
        }

        // run data check
        if ( false == psram_check(PSRAM_TIMING_SCAN_SIZE_BYTES, ui32MspiXipBaseAddress[module] + RxDqs_Index) )
        {
            // data check pass
            ui32ResultArray |= 0x01 << RxDqs_Index;
        }
        else
        {
            // data check failed
        }
    }

    //
    // Check result
    //
    uint32_t ui32Result = 0;
    ui32Result = count_consecutive_ones(&ui32ResultArray);

    //
    // print result for test
    //
    am_util_debug_printf("    Setting  = 0x%08X\n",  ui32ResultArray);

#if defined(AM_DEBUG_PRINTF)
    am_util_debug_printf("    Max length = %d \n", ui32Result);
#endif
    am_util_stdio_printf("    Timing Scan found a window %d fine steps wide.\n", ui32Result);

    //
    // Check consecutive passing settings
    //
    if ( ui32Result < PSRAM_TIMING_SCAN_MIN_ACCEPTANCE_LENGTH )
    {
        // too short is the passing settings
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    //
    // Find RXDQSDELAY Value
    //
    uint32_t dqsdelay = find_mid_point(&ui32ResultArray);

#if defined(AM_DEBUG_PRINTF)
    am_util_stdio_printf("Timing Scan set the fine delay to %d steps.\n", dqsdelay);
#endif


    //
    // Deinitialize the MSPI interface
    //
    am_devices_mspi_psram_w958d6nw_ddr_deinit(pDevHandle);

    //
    // Set output values
    //
    pDevDdrCfg->ui32Rxdqsdelay = dqsdelay;
    pDevDdrCfg->ui32Rxneg = 0;
#ifdef W958D6NW_USE_NON_DQS_MODE
    pDevDdrCfg->ui32Turnaround = 14;
#else
    pDevDdrCfg->ui32Turnaround = 6;
#endif

    return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief Apply given DDR timing settings to target MSPI instance.
//!
//! @param pHandle - Handle to the PSRAM.
//! @param pDevDdrCfg - Pointer to the ddr timing config structure
//!
//! This function applies the ddr timing settings to the selected mspi instance.
//! This function must be called after MSPI instance is initialized into
//! ENABLEFINEDELAY0 = 1 mode.
//!
//! @return 32-bit status
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_apply_hex_ddr_timing(void *pHandle,
                                           am_devices_mspi_psram_ddr_timing_config_t *pDevDdrCfg)
{
    am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;
    am_hal_mspi_dqs_t applyCfg = gWBDDRDqsCfg[pPsram->ui32Module];
    // apply timing settings
    applyCfg.ui8RxDQSDelay      = pDevDdrCfg->ui32Rxdqsdelay;

#if defined(AM_DEBUG_PRINTF)
    am_util_stdio_printf("    Timing Scan set the RxDQSDelay = %d .\n", applyCfg.ui8RxDQSDelay);
#endif

    return am_hal_mspi_control(pPsram->pMspiHandle, AM_HAL_MSPI_REQ_DQS, &applyCfg);

}
#endif

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

