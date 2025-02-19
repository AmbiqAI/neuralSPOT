//*****************************************************************************
//
//! @file am_devices_mspi_psram_aps25616ba_1p2v.c
//!
//! @brief APM DDR HEX and Octal SPI PSRAM driver.
//!
//! @addtogroup mspi_psram_aps25616ba APS25616BA MSPI PSRAM 1.2V Driver
//! @ingroup devices
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2025, Ambiq Micro, Inc.
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
// This is part of revision release_sdk5p2-040c7863bb of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include <string.h>
#include "am_mcu_apollo.h"
#include "am_devices_mspi_psram_aps25616ba_1p2v.h"
#include "am_bsp.h"
#include "am_util.h"

//*****************************************************************************
//
// Global variables.
//
//*****************************************************************************
//#define USE_NON_DQS_MODE

#if defined(USE_NEW_DDR)
#define MSPI_BASE_FREQUENCY AM_HAL_MSPI_CLK_96MHZ
#else
#define MSPI_BASE_FREQUENCY AM_HAL_MSPI_CLK_48MHZ
#endif

#define APS25616BA_tHS_MIN_US        155   // with margin
#define APS25616BA_tXHS_MIN_US       155 // with margin

#define AM_DEVICES_MSPI_PSRAM_TIMEOUT             1000000
#if defined(AM_PART_APOLLO5_API)
#define PSRAM_TIMING_SCAN_MIN_ACCEPTANCE_LENGTH   (3)     // there should be at least
                                                          // this amount of consecutive
                                                          // passing settings to be accepted.
#elif defined(AM_PART_APOLLO4_API)
#define PSRAM_TIMING_SCAN_MIN_ACCEPTANCE_LENGTH   (8)     // there should be at least
                                                          // this amount of consecutive
                                                          // passing settings to be accepted.
#endif

am_hal_mspi_xip_config_t gAPMDDRXipConfig[] =
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
  },
#if AM_REG_MSPI_NUM_MODULES == 4
  {
    .ui32APBaseAddr       = MSPI3_APERTURE_START_ADDR,
    .eAPMode              = AM_HAL_MSPI_AP_READ_WRITE,
    .eAPSize              = AM_HAL_MSPI_AP_SIZE64M,
    .scramblingStartAddr  = 0,
    .scramblingEndAddr    = 0,
  }
#endif
};

am_hal_mspi_dqs_t gAPMDDRDqsCfg[] =
{
  {
#ifdef USE_NON_DQS_MODE
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
#ifdef USE_NON_DQS_MODE
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
#ifdef USE_NON_DQS_MODE
    .bDQSEnable             = 0,
#else
    .bDQSEnable             = 1,
#endif
    .bDQSSyncNeg            = 0,
    .bEnableFineDelay       = 0,
#if defined(AM_PART_APOLLO4P)
    .ui8TxDQSDelay          = 12,
#else
    .ui8TxDQSDelay          = 0,
#endif
    .ui8RxDQSDelay          = 16,
    .ui8RxDQSDelayNeg       = 0,
    .bRxDQSDelayNegEN       = 0,
    .ui8RxDQSDelayHi        = 0,
    .ui8RxDQSDelayNegHi     = 0,
    .bRxDQSDelayHiEN        = 0,
  },
#if defined(AM_PART_APOLLO5_API)
  {
#ifdef USE_NON_DQS_MODE
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
#endif
};

am_hal_mspi_xip_misc_t gAPMXipMiscCfg[] =
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
  },
#if defined(AM_PART_APOLLO5_API)
  {
    .ui32CEBreak        = 10,
    .bXIPBoundary       = true,
    .bXIPOdd            = true,
    .bAppndOdd          = false,
    .bBEOn              = false,
    .eBEPolarity        = AM_HAL_MSPI_BE_LOW_ENABLE,
  }
#endif
};

am_hal_mspi_config_t gAPMDDRMspiCfg[] =
{
  {
    .ui32TCBSize          = 0,
    .pTCB                 = NULL,
    .bClkonD4             = 0
  },
  {
    .ui32TCBSize          = 0,
    .pTCB                 = NULL,
    .bClkonD4             = 0
  },
  {
    .ui32TCBSize          = 0,
    .pTCB                 = NULL,
    .bClkonD4             = 0
  },
#if defined(AM_PART_APOLLO5_API)
  {
    .ui32TCBSize          = 0,
    .pTCB                 = NULL,
    .bClkonD4             = 0
  },
#endif
};

am_devices_mspi_psram_ddr_timing_config_t gAPMTimingCfg[] =
{
  {
    {
      .bTxNeg = 1,
      .bRxNeg = 0,
      .bRxCap = 0,
      .ui8TxDQSDelay = 0,
      .ui8RxDQSDelay = 0,
      .ui8Turnaround = 6,
    },
    .bValid = false,
  },
  {
    {
      .bTxNeg = 1,
      .bRxNeg = 0,
      .bRxCap = 0,
      .ui8TxDQSDelay = 0,
      .ui8RxDQSDelay = 0,
      .ui8Turnaround = 6,
    },
    .bValid = false,
  },
  {
    {
      .bTxNeg = 1,
      .bRxNeg = 0,
      .bRxCap = 0,
      .ui8TxDQSDelay = 0,
      .ui8RxDQSDelay = 0,
      .ui8Turnaround = 6,
    },
    .bValid = false,
  },
#if defined(AM_PART_APOLLO5_API)
  {
    {
      .bTxNeg = 1,
      .bRxNeg = 0,
      .bRxCap = 0,
      .ui8TxDQSDelay = 0,
      .ui8RxDQSDelay = 0,
      .ui8Turnaround = 6,
    },
    .bValid = false,
  },
#endif
};

am_hal_mspi_rxcfg_t gAPMMspiRxCfg =
{
    .ui8DQSturn         = 2,
    .bRxHI              = 0,
    .bTaForth           = 1,
    .bHyperIO           = 0,
    .ui8RxSmp           = 1,
    .bRBX               = 0,
    .bWBX               = 0,
    .bSCLKRxHalt        = 0,
    .bRxCapEXT          = 0,
    .ui8Sfturn          = 10,
};

am_hal_mspi_dev_config_t  APMDDROctalMSPIConfig =
{
  .eAddrCfg             = AM_HAL_MSPI_ADDR_4_BYTE,
  .eInstrCfg            = AM_HAL_MSPI_INSTR_2_BYTE,
  .ui16ReadInstr        = AM_DEVICES_MSPI_PSRAM_DDR_READ,
  .ui16WriteInstr       = AM_DEVICES_MSPI_PSRAM_DDR_WRITE,
  .eDeviceConfig        = AM_HAL_MSPI_FLASH_OCTAL_DDR_CE0,
  .eSpiMode             = AM_HAL_MSPI_SPI_MODE_0,
  .bSendAddr            = true,
  .bSendInstr           = true,
  .bTurnaround          = true,
#if defined (AM_PART_APOLLO5_API)
  .eClockFreq           = AM_HAL_MSPI_CLK_250MHZ,
#else
  .eClockFreq           = AM_HAL_MSPI_CLK_96MHZ,
#endif
#ifdef USE_NON_DQS_MODE
  .ui8TurnAround        = 12,
  .ui8WriteLatency      = 6,
#else
  .ui8TurnAround        = 6,
  .ui8WriteLatency      = 6,
#endif
  .bEnWriteLatency      = true,
  .bEmulateDDR          = true,
#if defined(AM_PART_APOLLO5_API)
#if defined(USE_NEW_DDR)
  .bNewDDR              = true,
#else
  .bNewDDR              = false,
#endif
  .eCeLatency           = AM_HAL_MSPI_CE_LATENCY_NORMAL,
#endif
  .ui16DMATimeLimit     = 40,
  .eDMABoundary         = AM_HAL_MSPI_BOUNDARY_BREAK1K,
};

typedef struct
{
    uint32_t                                ui32Module;     //MSPI instance number
    void                                    *pMspiHandle;   //MSPI instance handle
    am_hal_mspi_dev_config_t                stSetting;      //MSPI instance setting
    bool                                    bOccupied;      //Is device occupied
    am_devices_mspi_psram_aps25616ba_regs_t  sRegisters;     //Device registers

    am_hal_mspi_config_t                        *pMspiCfg;      //pointer to global storage
    am_hal_mspi_xip_config_t                    *pXipCfg;       //pointer to global storage
    am_hal_mspi_xip_misc_t                      *pXipMiscCfg;   //pointer to global storage
    am_hal_mspi_dqs_t                           *pDqsCfg;       //pointer to global storage
    am_hal_mspi_rxcfg_t                         *pRxCfg;        //pointer to global storage
    am_devices_mspi_psram_ddr_timing_config_t   *pTimingCfg;    //pointer to global storage
} am_devices_mspi_psram_t;

am_devices_mspi_psram_t gAPMDDRPsram[AM_DEVICES_MSPI_PSRAM_MAX_DEVICE_NUM];

static uint32_t
am_devices_psram_aps25616ba_default_regs_set(am_devices_mspi_psram_t *pPsram)
{
    memset(&pPsram->sRegisters, 0, sizeof(pPsram->sRegisters));

    pPsram->sRegisters.MR0_b.DS = AM_DEVICES_MSPI_PSRAM_APS_Z8_DRIVE_STRENGTH_HALF;
#if defined(USE_APS51216BA)
    pPsram->sRegisters.MR0_b.RLC = AM_DEVICES_MSPI_PSRAM_APS_Z8_RLC_6;
#else
    pPsram->sRegisters.MR0_b.RLC = AM_DEVICES_MSPI_PSRAM_APS_Z8_RLC_5;
#endif

    pPsram->sRegisters.MR4_b.WLC = AM_DEVICES_MSPI_PSRAM_APS_Z8_WLC_6;

    pPsram->sRegisters.MR8_b.BL = 1;
    pPsram->sRegisters.MR8_b.BT = 1;

    return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

static inline uint8_t am_devices_mspi_psram_aps_z8_get_wlc(am_devices_mspi_psram_aps_z8_wlc_e eWLC, uint8_t *pWLC)
{
    switch(eWLC)
    {
#if !defined(USE_APS51216BA)
      case AM_DEVICES_MSPI_PSRAM_APS_Z8_WLC_3:
          *pWLC = 3;
          break;
      case AM_DEVICES_MSPI_PSRAM_APS_Z8_WLC_4:
          *pWLC = 4;
          break;
#endif
      case AM_DEVICES_MSPI_PSRAM_APS_Z8_WLC_5:
          *pWLC = 5;
          break;
      case AM_DEVICES_MSPI_PSRAM_APS_Z8_WLC_6:
          *pWLC = 6;
          break;
      case AM_DEVICES_MSPI_PSRAM_APS_Z8_WLC_7:
          *pWLC = 7;
          break;
      case AM_DEVICES_MSPI_PSRAM_APS_Z8_WLC_8:
          *pWLC = 8;
          break;
      case AM_DEVICES_MSPI_PSRAM_APS_Z8_WLC_9:
          *pWLC = 9;
          break;
      case AM_DEVICES_MSPI_PSRAM_APS_Z8_WLC_10:
          *pWLC = 10;
          break;
      default:
          return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }
    return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

static void pfnMSPI_APMPSRAM_DDR_Callback(void *pCallbackCtxt, uint32_t status)
{
#if defined(AM_PART_APOLLO5_API)
    //
    // Flush and invalidate whole cache
    // Recommend user to manage cache coherency based on application usage
    //
    am_hal_cachectrl_dcache_invalidate(NULL, true);
#endif
    //
    // Set the DMA complete flag.
    //
    *(volatile uint32_t *)pCallbackCtxt = status;
}

//*****************************************************************************
//
//! @brief Generic Command Write function.
//!
//! @param pMspiHandle
//! @param ui16Instr
//! @param bSendAddr
//! @param ui32Addr
//! @param pData
//! @param ui32NumBytes
//!
//! @return
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
  //
  // Create the individual write transaction.
  //
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
  //
  // Execute the transction over MSPI.
  //
  return am_hal_mspi_blocking_transfer(pMspiHandle,
                                       &Transaction,
                                       AM_DEVICES_MSPI_PSRAM_TIMEOUT);
}

//*****************************************************************************
//
//! @brief Generic Command Read function.
//!
//! @param pMspiHandle
//! @param ui16Instr
//! @param bSendAddr
//! @param ui32Addr
//! @param pData
//! @param ui32NumBytes
//!
//! @return
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
  //
  // Create the individual write transaction.
  //
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
  //
  // Execute the transction over MSPI.
  //
  return am_hal_mspi_blocking_transfer(pMspiHandle,
                                       &Transaction,
                                       AM_DEVICES_MSPI_PSRAM_TIMEOUT);
}

//*****************************************************************************
//
//! @brief Reset the external psram
//!
//! @param pPsram
//!
//! @return
//
//*****************************************************************************
static uint32_t
am_devices_mspi_psram_aps25616ba_reset(am_devices_mspi_psram_t *pPsram)
{
  uint32_t      ui32PIOBuffer = 0;
  //
  // Global Reset DDR PSRAM.
  //
  if (AM_HAL_STATUS_SUCCESS != am_device_command_write(pPsram->pMspiHandle, AM_DEVICES_MSPI_PSRAM_DDR_GLOBAL_RESET, true, 0, &ui32PIOBuffer, 2))
  {
    return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief Set PSRAM into HEX mode
//!
//! @param pPsram
//!
//! @return
//
//*****************************************************************************
static uint32_t
am_devices_mspi_psram_aps25616ba_enter_hex_mode(am_devices_mspi_psram_t *pPsram)
{
  uint32_t     ui32Status;
  uint32_t     ui32Rawdata;
  //
  // Read PSRAM MR8 register
  //
  am_util_debug_printf("Read PSRAM MR8 register\n");
  ui32Status = am_device_command_read(pPsram->pMspiHandle, AM_DEVICES_MSPI_PSRAM_DDR_READ_REGISTER, true, 8, &ui32Rawdata, 4);
  if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
  {
      am_util_debug_printf("Failed to read PSRAM MR8 register!\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  else
  {
      pPsram->sRegisters.MR8 = (uint8_t)ui32Rawdata;
      am_util_debug_printf("PSRAM Register MR8 = 0x%X\n", pPsram->sRegisters.MR8);
      am_util_debug_printf("PSRAM IO mode = 0x%X\n\n", pPsram->sRegisters.MR8_b.IOM);
  }

  pPsram->sRegisters.MR8_b.IOM = 1;
  ui32Rawdata = pPsram->sRegisters.MR8;
  ui32Status = am_device_command_write(pPsram->pMspiHandle, AM_DEVICES_MSPI_PSRAM_DDR_WRITE_REGISTER, true, 8, &ui32Rawdata, 4);
  if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
  {
      am_util_debug_printf("Failed to set PSRAM into HEX mode!\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  else
  {
      am_util_debug_printf("Set PSRAM into HEX mode\n\n");
  }

  return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief Set PSRAM into Octal mode and exit HEX mode
//!
//! @param pMspiHandle
//!
//! @return
//
//*****************************************************************************
static uint32_t
am_devices_mspi_psram_aps25616ba_exit_hex_mode(void *pMspiHandle)
{
  uint32_t     ui32Status;
  uint32_t     ui32Rawdata;
  uint8_t      ui8IOModeReg = 0;
  //
  // Read PSRAM MR8 register
  //
  am_util_debug_printf("Read PSRAM MR8 register\n");
  ui32Status = am_device_command_read(pMspiHandle, AM_DEVICES_MSPI_PSRAM_DDR_READ_REGISTER, true, 8, &ui32Rawdata, 4);
  if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
  {
      am_util_debug_printf("Failed to read PSRAM MR8 register!\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  else
  {
      ui8IOModeReg = (uint8_t)ui32Rawdata;
      am_util_debug_printf("PSRAM Register MR8 = 0x%X\n", ui8IOModeReg);
      am_util_debug_printf("PSRAM IO mode = 0x%X\n\n", (ui8IOModeReg & 0x40) >> 6 );
  }

  ui32Rawdata = ui8IOModeReg & 0xBF;

  ui32Status = am_device_command_write(pMspiHandle, AM_DEVICES_MSPI_PSRAM_DDR_WRITE_REGISTER, true, 8, &ui32Rawdata, 4);
  if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
  {
      am_util_debug_printf("Failed to configure PSRAM to exit HEX mode!\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  else
  {
      am_util_debug_printf("PSRAM is set to Octal mode\n\n");
  }

  return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief Reads the ID of the external psram and returns the value.
//!
//! @param pPsram
//!
//! This function reads the device ID register of the external psram, and returns
//! the result as an 32-bit unsigned integer value.
//!
//! @return 32-bit status
//
//*****************************************************************************
static uint32_t
am_devices_mspi_psram_aps25616ba_info(am_devices_mspi_psram_t *pPsram)
{
  uint32_t  ui32Rawdata;
  //
  // Read PSRAM Vendor ID and Device ID and return status.
  //
  am_util_debug_printf("Read PSRAM Vendor ID\n");
  if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != am_device_command_read(pPsram->pMspiHandle, AM_DEVICES_MSPI_PSRAM_DDR_READ_REGISTER, true, 1, &ui32Rawdata, 4))
  {
      am_util_debug_printf("Failed to read PSRAM Vendor ID!\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  else
  {
    pPsram->sRegisters.MR1 = (uint8_t)ui32Rawdata;
    am_util_debug_printf("PSRAM Register MR1 = 0x%X\n", pPsram->sRegisters.MR1);
    if ( pPsram->sRegisters.MR1_b.VID == 0xD )
    {
      am_util_debug_printf("PSRAM Vendor ID =  01101\n\n");
    }
    else
    {
      am_util_debug_printf("Fail to get correct PSRAM Vendor ID!\n\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }
  }

  am_util_debug_printf("Read PSRAM Device ID\n");
  if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != am_device_command_read(pPsram->pMspiHandle, AM_DEVICES_MSPI_PSRAM_DDR_READ_REGISTER, true, 2, &ui32Rawdata, 4))
  {
      am_util_debug_printf("Failed to read PSRAM Device ID!\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  else
  {
    pPsram->sRegisters.MR2 = (uint8_t)ui32Rawdata;
    pPsram->sRegisters.MR3 = (uint8_t)(ui32Rawdata>>8);
    am_util_debug_printf("PSRAM Register MR2 = 0x%X\n", pPsram->sRegisters.MR2);
    am_util_debug_printf("PSRAM Device ID =  Generation %d\n", pPsram->sRegisters.MR2_b.GENERATION == 0 ? 5 : pPsram->sRegisters.MR2_b.GENERATION + 1);
    if ( pPsram->sRegisters.MR2_b.DENSITY == 0x1 )
    {
      am_util_debug_printf("PSRAM Density =  32Mb\n\n");
    }
    else if ( pPsram->sRegisters.MR2_b.DENSITY == 0x3 )
    {
      am_util_debug_printf("PSRAM Density =  64Mb\n\n");
    }
    else if ( pPsram->sRegisters.MR2_b.DENSITY == 0x5 )
    {
      am_util_debug_printf("PSRAM Density =  128Mb\n\n");
    }
    else if ( pPsram->sRegisters.MR2_b.DENSITY == 0x7 )
    {
      am_util_debug_printf("PSRAM Density =  256Mb\n\n");
    }
    else if ( pPsram->sRegisters.MR2_b.DENSITY == 0x6 )
    {
      am_util_debug_printf("PSRAM Density =  512Mb\n\n");
    }
    am_util_debug_printf("PSRAM Register MR3 = 0x%X\n", pPsram->sRegisters.MR3);
    if ( pPsram->sRegisters.MR3_b.RBXen )
    {
      am_util_debug_printf("PSRAM RBX Supported\n\n");
    }
    else
    {
      am_util_debug_printf("PSRAM RBX not Supported\n\n");
    }
  }

  return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;

}

static uint32_t
am_devices_mspi_peripheral_init(uint32_t ui32Module,
                                am_devices_mspi_psram_config_t *pDevCfg,
                                am_devices_mspi_psram_t *pPsram)
{
  uint32_t    ui32Status;

  if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_initialize(ui32Module, &pPsram->pMspiHandle))
  {
      am_util_debug_printf("Error - Failed to initialize MSPI.\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_power_control(pPsram->pMspiHandle, AM_HAL_SYSCTRL_WAKE, false))
  {
      am_util_debug_printf("Error - Failed to power on MSPI.\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  pPsram->pMspiCfg->ui32TCBSize = pDevCfg->ui32NBTxnBufLength;
  pPsram->pMspiCfg->pTCB = pDevCfg->pNBTxnBuf;
  if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_configure(pPsram->pMspiHandle, pPsram->pMspiCfg))
  {
      am_util_debug_printf("Error - Failed to configure MSPI device.\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  pPsram->stSetting = APMDDROctalMSPIConfig;

  switch (pDevCfg->eDeviceConfig)
  {
      case AM_HAL_MSPI_FLASH_OCTAL_DDR_CE0:
      case AM_HAL_MSPI_FLASH_HEX_DDR_CE0:
        pPsram->stSetting.eDeviceConfig = AM_HAL_MSPI_FLASH_OCTAL_DDR_CE0;
        break;
      case AM_HAL_MSPI_FLASH_OCTAL_DDR_CE1:
      case AM_HAL_MSPI_FLASH_HEX_DDR_CE1:
        pPsram->stSetting.eDeviceConfig = AM_HAL_MSPI_FLASH_OCTAL_DDR_CE1;
        break;

      default:
          return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  pPsram->stSetting.eClockFreq = MSPI_BASE_FREQUENCY;
  if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != am_devices_mspi_psram_aps_z8_get_wlc(AM_DEVICES_MSPI_PSRAM_APS_Z8_WLC_6, &pPsram->stSetting.ui8WriteLatency) )
  {
      am_util_debug_printf("Invalid Write Latency Code!\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  pPsram->stSetting.ui8WriteLatency *= 2;
#if defined(USE_APS51216BA)
  pPsram->stSetting.ui8TurnAround = (AM_DEVICES_MSPI_PSRAM_APS_Z8_RLC_6 + 4) * 2;
#else
  pPsram->stSetting.ui8TurnAround = (AM_DEVICES_MSPI_PSRAM_APS_Z8_RLC_5 + 3) * 2;
#endif

  if ( !pPsram->pDqsCfg->bDQSEnable )
  {
      pPsram->stSetting.ui8TurnAround *= 2;
  }

  if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_device_configure(pPsram->pMspiHandle, &pPsram->stSetting))
  {
      am_util_debug_printf("Error - Failed to configure MSPI device.\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  pPsram->pXipCfg->scramblingStartAddr = pDevCfg->ui32ScramblingStartAddr;
  pPsram->pXipCfg->scramblingEndAddr = pDevCfg->ui32ScramblingEndAddr;
  ui32Status = am_hal_mspi_control(pPsram->pMspiHandle, AM_HAL_MSPI_REQ_XIP_CONFIG, pPsram->pXipCfg);
  if (AM_HAL_STATUS_SUCCESS != ui32Status)
  {
    return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  ui32Status = am_hal_mspi_control(pPsram->pMspiHandle, AM_HAL_MSPI_REQ_XIP_MISC_CONFIG, pPsram->pXipMiscCfg);
  if (AM_HAL_STATUS_SUCCESS != ui32Status)
  {
    return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  ui32Status = am_hal_mspi_control(pPsram->pMspiHandle, AM_HAL_MSPI_REQ_DQS, pPsram->pDqsCfg);
  if (AM_HAL_STATUS_SUCCESS != ui32Status)
  {
    return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  ui32Status = am_hal_mspi_control(pPsram->pMspiHandle, AM_HAL_MSPI_REQ_RXCFG, pPsram->pRxCfg);
  if (AM_HAL_STATUS_SUCCESS != ui32Status)
  {
    return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  //
  // Enable DDR emulation in MSPI
  //
  ui32Status = am_hal_mspi_control(pPsram->pMspiHandle, AM_HAL_MSPI_REQ_DDR_EN, NULL);
  if (AM_HAL_STATUS_SUCCESS != ui32Status)
  {
    return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_enable(pPsram->pMspiHandle))
  {
      am_util_debug_printf("Error - Failed to enable MSPI.\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  am_bsp_mspi_pins_enable(ui32Module, pPsram->stSetting.eDeviceConfig);
  am_util_delay_us(150);

  return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

static uint32_t
am_devices_mspi_psram_aps25616ba_device_init(am_devices_mspi_psram_t *pPsram, am_hal_mspi_clock_e eTargetFreq)
{
  uint32_t     ui32Status;
  uint32_t     ui32Rawdata;
  uint8_t      ui8RLCReg = 0;

  //
  // Send reset command to PSRAM
  //
  if (AM_HAL_STATUS_SUCCESS != am_devices_mspi_psram_aps25616ba_reset(pPsram))
  {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  am_util_delay_us(2);

  //
  // Read PSRAM ID
  //
  if (AM_HAL_STATUS_SUCCESS != am_devices_mspi_psram_aps25616ba_info(pPsram))
  {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  //
  // Set Read boundary crossing as needed
  //
  if ( pPsram->sRegisters.MR3_b.RBXen && pPsram->pRxCfg->bRBX )
  {
      if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != am_device_command_read(pPsram->pMspiHandle, AM_DEVICES_MSPI_PSRAM_DDR_READ_REGISTER, true, 8, &ui32Rawdata, 4))
      {
          am_util_debug_printf("Failed to read PSRAM MR8 register!\n");
          return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
      }
      else
      {
          pPsram->sRegisters.MR8 = (uint8_t)ui32Rawdata;
          am_util_debug_printf("PSRAM Register MR8 = 0x%02X\n", pPsram->sRegisters.MR8);
          am_util_debug_printf("PSRAM RBX mode = 0x%X\n", pPsram->sRegisters.MR8_b.RBX);
      }
      pPsram->sRegisters.MR8_b.RBX = 1;
      ui32Rawdata = pPsram->sRegisters.MR8;
      if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != am_device_command_write(pPsram->pMspiHandle, AM_DEVICES_MSPI_PSRAM_DDR_WRITE_REGISTER, true, 8, &ui32Rawdata, 4))
      {
          am_util_debug_printf("Failed to write PSRAM Register MR8!\n");
          return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
      }
      else
      {
          am_util_debug_printf("Set PSRAM Register MR8 into 0x%02X\n\n", pPsram->sRegisters.MR8);
      }
  }

  //
  // Read PSRAM Write Latency Code
  //
  am_util_debug_printf("Read PSRAM Write Latency Code\n");
  ui32Status = am_device_command_read(pPsram->pMspiHandle, AM_DEVICES_MSPI_PSRAM_DDR_READ_REGISTER, true, 4, &ui32Rawdata, 4);
  if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
  {
      am_util_debug_printf("Failed to read PSRAM Write Latency Code!\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  else
  {
      pPsram->sRegisters.MR4 = (uint8_t)ui32Rawdata;
      am_util_debug_printf("PSRAM Register MR4 = 0x%02X\n", pPsram->sRegisters.MR4);
      uint8_t ui8Temp;
      if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != am_devices_mspi_psram_aps_z8_get_wlc(pPsram->sRegisters.MR4_b.WLC, &ui8Temp) )
      {
          am_util_debug_printf("Invalid Write Latency Code!\n");
          return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
      }
      am_util_debug_printf("PSRAM Write Latency = %d\n\n", ui8Temp);
  }

  //
  // Read PSRAM Register MR0
  //
  am_util_debug_printf("Read PSRAM Register MR0\n");
  ui32Status = am_device_command_read(pPsram->pMspiHandle, AM_DEVICES_MSPI_PSRAM_DDR_READ_REGISTER, true, 0, &ui32Rawdata, 4);
  if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
  {
      am_util_debug_printf("Failed to read PSRAM Register MR0!\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  else
  {
      pPsram->sRegisters.MR0 = (uint8_t)ui32Rawdata;
      am_util_debug_printf("PSRAM Register MR0 = 0x%02X\n", pPsram->sRegisters.MR0);
      am_util_debug_printf("PSRAM Read Latency = %d\n", pPsram->sRegisters.MR0_b.RLC + 3 );
  }

#if defined(USE_APS51216BA)
  switch(eTargetFreq)
  {
#if defined (AM_PART_APOLLO5_API)
    case AM_HAL_MSPI_CLK_250MHZ:
      pPsram->sRegisters.MR0_b.RLC = AM_DEVICES_MSPI_PSRAM_APS_Z8_RLC_7;
      pPsram->sRegisters.MR4_b.WLC = AM_DEVICES_MSPI_PSRAM_APS_Z8_WLC_8;
      break;
    case AM_HAL_MSPI_CLK_192MHZ:
      pPsram->sRegisters.MR0_b.RLC = AM_DEVICES_MSPI_PSRAM_APS_Z8_RLC_6;
      pPsram->sRegisters.MR4_b.WLC = AM_DEVICES_MSPI_PSRAM_APS_Z8_WLC_7;
      break;
    case AM_HAL_MSPI_CLK_125MHZ:
      pPsram->sRegisters.MR0_b.RLC = AM_DEVICES_MSPI_PSRAM_APS_Z8_RLC_5;
      pPsram->sRegisters.MR4_b.WLC = AM_DEVICES_MSPI_PSRAM_APS_Z8_WLC_6;
      break;
#endif
    case AM_HAL_MSPI_CLK_96MHZ:
      pPsram->sRegisters.MR0_b.RLC = AM_DEVICES_MSPI_PSRAM_APS_Z8_RLC_5;
      pPsram->sRegisters.MR4_b.WLC = AM_DEVICES_MSPI_PSRAM_APS_Z8_WLC_6;
      break;
    default:
      pPsram->sRegisters.MR0_b.RLC = AM_DEVICES_MSPI_PSRAM_APS_Z8_RLC_4;
      pPsram->sRegisters.MR4_b.WLC = AM_DEVICES_MSPI_PSRAM_APS_Z8_WLC_5;
      break;
  }
#else
  switch(eTargetFreq)
  {
#if defined (AM_PART_APOLLO5_API)
    case AM_HAL_MSPI_CLK_250MHZ:
      pPsram->sRegisters.MR0_b.RLC = AM_DEVICES_MSPI_PSRAM_APS_Z8_RLC_6;
      pPsram->sRegisters.MR4_b.WLC = AM_DEVICES_MSPI_PSRAM_APS_Z8_WLC_7;
      break;
    case AM_HAL_MSPI_CLK_192MHZ:
      pPsram->sRegisters.MR0_b.RLC = AM_DEVICES_MSPI_PSRAM_APS_Z8_RLC_5;
      pPsram->sRegisters.MR4_b.WLC = AM_DEVICES_MSPI_PSRAM_APS_Z8_WLC_6;
      break;
    case AM_HAL_MSPI_CLK_125MHZ:
      pPsram->sRegisters.MR0_b.RLC = AM_DEVICES_MSPI_PSRAM_APS_Z8_RLC_4;
      pPsram->sRegisters.MR4_b.WLC = AM_DEVICES_MSPI_PSRAM_APS_Z8_WLC_5;
      break;
#endif
    case AM_HAL_MSPI_CLK_96MHZ:
      pPsram->sRegisters.MR0_b.RLC = AM_DEVICES_MSPI_PSRAM_APS_Z8_RLC_4;
      pPsram->sRegisters.MR4_b.WLC = AM_DEVICES_MSPI_PSRAM_APS_Z8_WLC_5;
      break;
    default:
      pPsram->sRegisters.MR0_b.RLC = AM_DEVICES_MSPI_PSRAM_APS_Z8_RLC_3;
      pPsram->sRegisters.MR4_b.WLC = AM_DEVICES_MSPI_PSRAM_APS_Z8_WLC_3;
      break;
  }
#endif //defined(USE_APS51216BA)

  if ( !pPsram->pDqsCfg->bDQSEnable )
  {
      pPsram->sRegisters.MR0_b.LT = 1;
      am_util_debug_printf("\nSet read latency into fixed type in NON-DQS mode!\n");
  }
  ui32Rawdata = pPsram->sRegisters.MR0;

  ui32Status = am_device_command_write(pPsram->pMspiHandle, AM_DEVICES_MSPI_PSRAM_DDR_WRITE_REGISTER, true, 0, &ui32Rawdata, 4);
  if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
  {
      am_util_debug_printf("\nFailed to write PSRAM Register MR0!\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  else
  {
      am_util_debug_printf("\nSet PSRAM Register MR0 into 0x%02X\n", pPsram->sRegisters.MR0);
  }

  ui32Rawdata = pPsram->sRegisters.MR4;

  ui32Status = am_device_command_write(pPsram->pMspiHandle, AM_DEVICES_MSPI_PSRAM_DDR_WRITE_REGISTER, true, 4, &ui32Rawdata, 4);
  if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
  {
      am_util_debug_printf("Failed to write PSRAM Write Latency Code!\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  else
  {
      am_util_debug_printf("Set PSRAM Register MR4 into 0x%02X\n\n", pPsram->sRegisters.MR4);
  }

  if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != am_devices_mspi_psram_aps_z8_get_wlc(pPsram->sRegisters.MR4_b.WLC, &pPsram->stSetting.ui8WriteLatency) )
  {
      am_util_debug_printf("Invalid Write Latency Code!\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  pPsram->stSetting.ui8WriteLatency *= 2;
#if defined(USE_APS51216BA)
  pPsram->stSetting.ui8TurnAround = (pPsram->sRegisters.MR0_b.RLC + 4) * 2;
#else
  pPsram->stSetting.ui8TurnAround = (pPsram->sRegisters.MR0_b.RLC + 3) * 2;
#endif
  if ( !pPsram->pDqsCfg->bDQSEnable )
  {
      pPsram->stSetting.ui8TurnAround *= 2;
  }

  if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_device_configure(pPsram->pMspiHandle, &pPsram->stSetting))
  {
      am_util_debug_printf("Error - Failed to configure MSPI device.\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  am_util_debug_printf("Read PSRAM Read Latency Code\n");
  ui32Status = am_device_command_read(pPsram->pMspiHandle, AM_DEVICES_MSPI_PSRAM_DDR_READ_REGISTER, true, 0, &ui32Rawdata, 4);
  if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
  {
      am_util_debug_printf("Failed to read PSRAM Read Latency Code!\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  else
  {
      ui8RLCReg = (uint8_t)ui32Rawdata;
      if ( ui8RLCReg == pPsram->sRegisters.MR0 )
      {
        am_util_debug_printf("PSRAM Register MR0 = 0x%02X\nSet Success\n", ui8RLCReg);
        am_util_debug_printf("PSRAM Read Latency = %d\n\n", pPsram->sRegisters.MR0_b.RLC + 3 );
      }
      else
      {
        am_util_debug_printf("PSRAM Register MR0 = 0x%02X\nSet Fail\n\n", ui8RLCReg);
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
      }
  }

  am_util_debug_printf("Read PSRAM Write Latency Code\n");
  ui32Status = am_device_command_read(pPsram->pMspiHandle, AM_DEVICES_MSPI_PSRAM_DDR_READ_REGISTER, true, 4, &ui32Rawdata, 4);
  if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
  {
      am_util_debug_printf("Failed to read PSRAM Write Latency Code!\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  else
  {
    ui8RLCReg = (uint8_t)ui32Rawdata;
    if ( ui8RLCReg == pPsram->sRegisters.MR4 )
    {
      am_util_debug_printf("PSRAM Register MR4 = 0x%02X\nSet Success\n", ui8RLCReg);
      uint8_t ui8Temp;
      if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != am_devices_mspi_psram_aps_z8_get_wlc(pPsram->sRegisters.MR4_b.WLC, &ui8Temp) )
      {
          am_util_debug_printf("Invalid Write Latency Code!\n");
          return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
      }
      am_util_debug_printf("PSRAM Write Latency = %d\n\n", ui8Temp);
    }
    else
    {
      am_util_debug_printf("PSRAM Register MR4 = 0x%02X\nSet Fail\n\n", ui8RLCReg);
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }
  }

  return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief This function takes care of splitting the transaction as needed, if the transaction crosses
//! PSRAM page boundary or because of tCEM restrictions, if hardware does not support it
//!
//! @param pPsram
//! @param bHiPrio
//! @param bWrite
//! @param pui8Buffer
//! @param ui32Address
//! @param ui32NumBytes
//! @param ui32PauseCondition
//! @param ui32StatusSetClr
//! @param pfnCallback
//! @param pCallbackCtxt
//!
//! @return
//
//*****************************************************************************
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
  //
  // Set the DMA priority
  //
  Transaction.ui8Priority = 1;

  //
  // Set the transfer direction to RX (Read)
  //
  Transaction.eDirection = bWrite ? AM_HAL_MSPI_TX: AM_HAL_MSPI_RX;

  //
  // Initialize the CQ stimulus.
  //
  Transaction.ui32PauseCondition = ui32PauseCondition;
  //
  // Initialize the post-processing
  //
  Transaction.ui32StatusSetClr = 0;
  //
  // Need to be aware of page size
  //
  while (ui32NumBytes)
  {
    uint32_t size;
    if ((ui32Address & 0x3) &&
        ((AM_DEVICES_MSPI_PSRAM_PAGE_SIZE - (ui32Address & (AM_DEVICES_MSPI_PSRAM_PAGE_SIZE - 1))) < ui32NumBytes))
    {
      //
      // Hardware does not support Page splitting if address is not word aligned
      // Need to split the transaction
      //
      size = 4 - (ui32Address & 0x3);
    }
    else
    {
      size = ui32NumBytes;
    }

    bool bLast = (size == ui32NumBytes);
    //
    // Set the transfer count in bytes.
    //
    Transaction.ui32TransferCount = size;
    //
    // Set the address to read data from.
    //
    Transaction.ui32DeviceAddress = ui32Address;
    //
    // Set the target SRAM buffer address.
    //
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
// Initialize the mspi_psram driver.
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_aps25616ba_ddr_init(uint32_t ui32Module,
                                          am_devices_mspi_psram_config_t *pDevCfg,
                                          void **ppHandle,
                                          void **ppMspiHandle)
{
    uint32_t                    ui32Status;
    am_hal_mspi_dev_config_t    *psConfig;
    am_devices_mspi_psram_t     *pPsram;

    if ((ui32Module > AM_REG_MSPI_NUM_MODULES) || (pDevCfg == NULL))
    {
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }


    //
    // Allocate a vacant device handle
    //
    if ( gAPMDDRPsram[ui32Module].bOccupied == false )
    {
        pPsram = &gAPMDDRPsram[ui32Module];
        pPsram->pMspiCfg     = &gAPMDDRMspiCfg[ui32Module];
        pPsram->pXipCfg      = &gAPMDDRXipConfig[ui32Module];
        pPsram->pXipMiscCfg  = &gAPMXipMiscCfg[ui32Module];
        pPsram->pDqsCfg      = &gAPMDDRDqsCfg[ui32Module];
        pPsram->pRxCfg       = &gAPMMspiRxCfg;
        pPsram->pTimingCfg   = &gAPMTimingCfg[ui32Module];
        psConfig = &pPsram->stSetting;
        am_devices_psram_aps25616ba_default_regs_set(pPsram);
    }
    else
    {
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    am_util_debug_printf("\nStart PSRAM Initialization\n");

    //
    // Configure the MSPI into Octal mode for PSRAM register access and Hex mode for data access.
    //

    if (AM_HAL_STATUS_SUCCESS != am_devices_mspi_peripheral_init(ui32Module, pDevCfg, pPsram))
    {
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != am_devices_mspi_psram_aps25616ba_device_init(pPsram, pDevCfg->eClockFreq))
    {
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    psConfig->eClockFreq = pDevCfg->eClockFreq;

    switch (pDevCfg->eDeviceConfig)
    {
        case AM_HAL_MSPI_FLASH_OCTAL_DDR_CE0:
        case AM_HAL_MSPI_FLASH_HEX_DDR_CE0:
        case AM_HAL_MSPI_FLASH_OCTAL_DDR_CE1:
        case AM_HAL_MSPI_FLASH_HEX_DDR_CE1:
          psConfig->eDeviceConfig = pDevCfg->eDeviceConfig;
          break;

        default:
            return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    if ((psConfig->eDeviceConfig == AM_HAL_MSPI_FLASH_HEX_DDR_CE0) || (psConfig->eDeviceConfig == AM_HAL_MSPI_FLASH_HEX_DDR_CE1))
    {
        //
        // Set PSRAM into HEX mode
        //
        if (AM_HAL_STATUS_SUCCESS != am_devices_mspi_psram_aps25616ba_enter_hex_mode(pPsram))
        {
            return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
        }
    }

    //
    // Disable MSPI defore re-configuring it
    //
    ui32Status = am_hal_mspi_disable(pPsram->pMspiHandle);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

#if defined (AM_PART_APOLLO5_API)
    if (AM_HAL_MSPI_CLK_250MHZ == psConfig->eClockFreq
     || AM_HAL_MSPI_CLK_192MHZ == psConfig->eClockFreq
     || AM_HAL_MSPI_CLK_125MHZ == psConfig->eClockFreq
     || AM_HAL_MSPI_CLK_96MHZ == psConfig->eClockFreq)
    {
      pPsram->pRxCfg->ui8RxSmp = 2;
    }
#else
    if (AM_HAL_MSPI_CLK_96MHZ == psConfig->eClockFreq)
    {
      pPsram->pRxCfg->ui8RxSmp = 2;
    }
#endif
    ui32Status = am_hal_mspi_control(pPsram->pMspiHandle, AM_HAL_MSPI_REQ_RXCFG, pPsram->pRxCfg);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    //
    // Reconfig MSPI device settings
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_device_configure(pPsram->pMspiHandle, psConfig))
    {
        am_util_debug_printf("Error - Failed to reconfig MSPI device.\n");
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    //
    // Re-Enable MSPI
    //
    ui32Status = am_hal_mspi_enable(pPsram->pMspiHandle);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    //
    // Re-config the MSPI pins.
    //
    am_bsp_mspi_pins_enable(ui32Module, psConfig->eDeviceConfig);

    //
    // Enable MSPI interrupts.
    //
    ui32Status = am_hal_mspi_interrupt_clear(pPsram->pMspiHandle, AM_HAL_MSPI_INT_CQUPD | AM_HAL_MSPI_INT_ERR );
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    ui32Status = am_hal_mspi_interrupt_enable(pPsram->pMspiHandle, AM_HAL_MSPI_INT_CQUPD | AM_HAL_MSPI_INT_ERR );
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    //
    // Return the handle.
    //
    pPsram->bOccupied = true;
    pPsram->ui32Module = ui32Module;
    *ppHandle = (void *)pPsram;
    *ppMspiHandle = pPsram->pMspiHandle;

    //
    // Return the status.
    //
    return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

//*****************************************************************************
//
// DeInitialize the mspi_psram driver.
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_aps25616ba_ddr_deinit(void *pHandle)
{
    uint32_t    ui32Status;
    am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;

    if ( pHandle == NULL )
    {
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    am_util_debug_printf("\nStart PSRAM Deinitialization\n");

    if ((pPsram->stSetting.eDeviceConfig == AM_HAL_MSPI_FLASH_HEX_DDR_CE0) || (pPsram->stSetting.eDeviceConfig == AM_HAL_MSPI_FLASH_HEX_DDR_CE1))
    {
        //
        // Set PSRAM into Octal mode. Exit Hex mode.
        //
        ui32Status = am_devices_mspi_psram_aps25616ba_exit_hex_mode(pPsram->pMspiHandle);
        if (AM_HAL_STATUS_SUCCESS != ui32Status)
        {
            return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
        }
    }

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
    //
    // Free this device handle
    //
    pPsram->bOccupied = false;

    //
    // Return the status.
    //
    return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Reads the contents of the external PSRAM into a buffer.
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_aps25616ba_ddr_read(void *pHandle,
                                          uint8_t *pui8RxBuffer,
                                          uint32_t ui32ReadAddress,
                                          uint32_t ui32NumBytes,
                                          bool bWaitForCompletion)
{
  uint32_t                      ui32Status;
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;
  if ( pHandle == NULL )
  {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  if (bWaitForCompletion)
  {
    //
    // Start the transaction.
    //
    volatile uint32_t ui32DMAStatus = 0xFFFFFFFF;
    ui32Status = psram_nonblocking_transfer(pPsram, false, false,
                                            pui8RxBuffer,
                                            ui32ReadAddress,
                                            ui32NumBytes,
                                            0,
                                            0,
                                            pfnMSPI_APMPSRAM_DDR_Callback,
                                            (void *)&ui32DMAStatus);
    //
    // Check the transaction status.
    //
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }
    //
    // Wait for DMA Complete or Timeout
    //
    for (uint32_t i = 0; i < AM_DEVICES_MSPI_PSRAM_TIMEOUT; i++)
    {
      if (ui32DMAStatus != 0xFFFFFFFF)
      {
        break;
      }
      //
      // Call the BOOTROM cycle function to delay for about 1 microsecond.
      //
      am_hal_delay_us(1);
    }
    //
    // Check the status.
    //
    if (ui32DMAStatus != AM_HAL_STATUS_SUCCESS)
    {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }
  }
  else
  {
    //
    // Check the transaction status.
    //
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
// Reads the contents of the external PSRAM into a buffer.
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_aps25616ba_ddr_read_adv(void *pHandle,
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
  if ( pHandle == NULL )
  {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  ui32Status = psram_nonblocking_transfer(pPsram, false, false,
                                          pui8RxBuffer,
                                          ui32ReadAddress,
                                          ui32NumBytes,
                                          ui32PauseCondition,
                                          ui32StatusSetClr,
                                          pfnCallback,
                                          pCallbackCtxt);
  //
  // Check the transaction status.
  //
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
// Reads the contents of the external psram into a buffer
// with high priority
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_aps25616ba_ddr_read_hiprio(void *pHandle,
                                                 uint8_t *pui8RxBuffer,
                                                 uint32_t ui32ReadAddress,
                                                 uint32_t ui32NumBytes,
                                                 am_hal_mspi_callback_t pfnCallback,
                                                 void *pCallbackCtxt)
{
  uint32_t                      ui32Status;
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;
  if ( pHandle == NULL )
  {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  ui32Status = psram_nonblocking_transfer(pPsram, true, false,
                                          pui8RxBuffer,
                                          ui32ReadAddress,
                                          ui32NumBytes,
                                          0,
                                          0,
                                          pfnCallback,
                                          pCallbackCtxt);
  //
  // Check the transaction status.
  //
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
am_devices_mspi_psram_aps25616ba_ddr_nonblocking_read(void *pHandle,
                                                      uint8_t *pui8RxBuffer,
                                                      uint32_t ui32ReadAddress,
                                                      uint32_t ui32NumBytes,
                                                      am_hal_mspi_callback_t pfnCallback,
                                                      void *pCallbackCtxt)
{
  uint32_t                      ui32Status;
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;
  if ( pHandle == NULL )
  {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  //
  // Check the transaction status.
  //
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
//  Programs the given range of psram addresses.
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_aps25616ba_ddr_write(void *pHandle,
                                           uint8_t *pui8TxBuffer,
                                           uint32_t ui32WriteAddress,
                                           uint32_t ui32NumBytes,
                                           bool bWaitForCompletion)
{
  uint32_t                      ui32Status;
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;
  if ( pHandle == NULL )
  {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  if (bWaitForCompletion)
  {
    //
    // Start the transaction.
    //
    volatile uint32_t ui32DMAStatus = 0xFFFFFFFF;
    ui32Status = psram_nonblocking_transfer(pPsram, false, true,
                                            pui8TxBuffer,
                                            ui32WriteAddress,
                                            ui32NumBytes,
                                            0,
                                            0,
                                            pfnMSPI_APMPSRAM_DDR_Callback,
                                            (void *)&ui32DMAStatus);
    //
    // Check the transaction status.
    //
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }
    //
    // Wait for DMA Complete or Timeout
    //
    for (uint32_t i = 0; i < AM_DEVICES_MSPI_PSRAM_TIMEOUT; i++)
    {
      if (ui32DMAStatus != 0xFFFFFFFF)
      {
        break;
      }
      //
      // Call the BOOTROM cycle function to delay for about 1 microsecond.
      //
      am_hal_delay_us(1);
    }
    //
    // Check the status.
    //
    if (ui32DMAStatus != AM_HAL_STATUS_SUCCESS)
    {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }
  }
  else
  {
    //
    // Check the transaction status.
    //
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
// Programs the given range of psram addresses.
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_aps25616ba_ddr_write_adv(void *pHandle,
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
  if ( pHandle == NULL )
  {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  ui32Status = psram_nonblocking_transfer(pPsram, false, true,
                                          puiTxBuffer,
                                          ui32WriteAddress,
                                          ui32NumBytes,
                                          ui32PauseCondition,
                                          ui32StatusSetClr,
                                          pfnCallback,
                                          pCallbackCtxt);
  //
  // Check the transaction status.
  //
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
// Programs the contents of the external psram into a buffer
// with high priority
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_aps25616ba_ddr_write_hiprio(void *pHandle,
                                                  uint8_t *pui8TxBuffer,
                                                  uint32_t ui32WriteAddress,
                                                  uint32_t ui32NumBytes,
                                                  am_hal_mspi_callback_t pfnCallback,
                                                  void *pCallbackCtxt)
{
  uint32_t                      ui32Status;
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;
  if ( pHandle == NULL )
  {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  //
  // Check the transaction status.
  //
  ui32Status = psram_nonblocking_transfer(pPsram, true, true,
                                          pui8TxBuffer,
                                          ui32WriteAddress,
                                          ui32NumBytes,
                                          0,
                                          0,
                                          pfnCallback,
                                          pCallbackCtxt);
  //
  // Check the transaction status.
  //
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
//! @brief
//!
//! @param pHandle
//! @param pui8TxBuffer
//! @param ui32WriteAddress
//! @param ui32NumBytes
//! @param pfnCallback
//! @param pCallbackCtxt
//!
//! @return
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_aps25616ba_ddr_nonblocking_write(void *pHandle,
                                                       uint8_t *pui8TxBuffer,
                                                       uint32_t ui32WriteAddress,
                                                       uint32_t ui32NumBytes,
                                                       am_hal_mspi_callback_t pfnCallback,
                                                       void *pCallbackCtxt)
{
  uint32_t                      ui32Status;
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;
  if ( pHandle == NULL )
  {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  //
  // Check the transaction status.
  //
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
//  Reconfigure MSPI XIP settings.
//
//*****************************************************************************
uint32_t am_devices_mspi_psram_aps25616ba_xip_config(void *pHandle, am_hal_mspi_xip_config_t *pXipconfig)
{
    am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;

    if ( pHandle == NULL )
    {
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    am_hal_mspi_xip_config_t *pXipCfg = pPsram->pXipCfg;

    *pXipCfg = *pXipconfig;

    if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_control(pPsram->pMspiHandle, AM_HAL_MSPI_REQ_XIP_CONFIG, pXipCfg))
    {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

//*****************************************************************************
//
//  Sets up the MSPI and external psram into XIP mode.
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_aps25616ba_ddr_enable_xip(void *pHandle)
{
  uint32_t ui32Status;
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;
  if ( pHandle == NULL )
  {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  //
  // Enable XIP on the MSPI.
  //
  ui32Status = am_hal_mspi_control(pPsram->pMspiHandle, AM_HAL_MSPI_REQ_XIP_EN, NULL);
  if (AM_HAL_STATUS_SUCCESS != ui32Status)
  {
    return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

//*****************************************************************************
//
//   Removes the MSPI and external psram from XIP mode.
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_aps25616ba_ddr_disable_xip(void *pHandle)
{
  uint32_t ui32Status;
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;
  if ( pHandle == NULL )
  {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  //
  // Disable XIP on the MSPI.
  //
  ui32Status = am_hal_mspi_control(pPsram->pMspiHandle, AM_HAL_MSPI_REQ_XIP_DIS, NULL);
  if (AM_HAL_STATUS_SUCCESS != ui32Status)
  {
    return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

//*****************************************************************************
//
//   Sets up the MSPI and external psram into scrambling mode.
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_aps25616ba_ddr_enable_scrambling(void *pHandle)
{
  uint32_t ui32Status;
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;
  if ( pHandle == NULL )
  {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  //
  // Enable scrambling on the MSPI.
  //
  ui32Status = am_hal_mspi_control(pPsram->pMspiHandle, AM_HAL_MSPI_REQ_SCRAMB_EN, NULL);
  if (AM_HAL_STATUS_SUCCESS != ui32Status)
  {
    return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

//*****************************************************************************
//
//   Removes the MSPI and external psram from scrambling mode.
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_aps25616ba_ddr_disable_scrambling(void *pHandle)
{
  uint32_t ui32Status;
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;
  if ( pHandle == NULL )
  {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  //
  // Disable Scrambling on the MSPI.
  //
  ui32Status = am_hal_mspi_control(pPsram->pMspiHandle, AM_HAL_MSPI_REQ_SCRAMB_DIS, NULL);
  if (AM_HAL_STATUS_SUCCESS != ui32Status)
  {
    return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Reset the external psram
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_aps25616ba_ddr_reset(void *pHandle)
{
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;
  if ( pHandle == NULL )
  {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  return am_devices_mspi_psram_aps25616ba_reset(pPsram);
}

//*****************************************************************************
//
//   Reads the ID of the external psram and returns the value. If device not
//   initialized then return 0xFFFFFFFF
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_aps25616ba_ddr_id(void *pHandle)
{
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;
  if ( pHandle == NULL )
  {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }
  uint32_t ui32DeviceID;
  if ( pPsram->pMspiHandle == NULL || !pPsram->bOccupied )
  {
    return 0xFFFFFFFF;
  }

  ui32DeviceID = pPsram->sRegisters.MR2 & 0x0000001F;
  ui32DeviceID |= (uint32_t)pPsram->sRegisters.MR1_b.VID << 8;

  return ui32DeviceID;
}

//*****************************************************************************
//
//   Reads the info of the external psram and returns the value.
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_aps25616ba_ddr_info(void *pHandle, am_devices_mspi_psram_info_t *pPsramInfo)
{
  am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;
  if ( pHandle == NULL )
  {
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  if ( pPsram->pMspiHandle == NULL || !pPsram->bOccupied )
  {
    return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
  }

  pPsramInfo->ui8VendorId = pPsram->sRegisters.MR1_b.VID;
  pPsramInfo->ui8DeviceId = pPsram->sRegisters.MR2_b.GENERATION;
  pPsramInfo->ui32BaseAddr = pPsram->pXipCfg->ui32APBaseAddr;

  switch (pPsram->sRegisters.MR2_b.DENSITY)
  {
    case 1:
      pPsramInfo->ui32DeviceSizeKb = 32 / 8 * 1024U;
      break;
    case 3:
      pPsramInfo->ui32DeviceSizeKb = 64 / 8 * 1024U;
      break;
    case 5:
      pPsramInfo->ui32DeviceSizeKb = 128 / 8 * 1024U;
      break;
    case 7:
      pPsramInfo->ui32DeviceSizeKb = 256 / 8 * 1024U;
      break;
    case 6:
      pPsramInfo->ui32DeviceSizeKb = 512 / 8 * 1024U;
      break;
  }

  return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

#if defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L) || defined(AM_PART_APOLLO5_API)
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
//*****************************************************************************
//
//! @brief
//!
//! @param pattern_index
//! @param buff
//! @param len
//!
//! @return
//
//*****************************************************************************
static int prepare_test_pattern(uint32_t pattern_index, uint8_t* buff, uint32_t len)
{
    uint32_t *pui32TxPtr = (uint32_t*)buff;
    uint8_t  *pui8TxPtr  = (uint8_t*)buff;
    //
    // length has to be multiple of 4 bytes
    //
    if ( len % 4 )
    {
        return -1;
    }

    switch ( pattern_index )
    {
        case 0:
            //
            // 0x5555AAAA
            //
            for (uint32_t i = 0; i < len / 4; i++)
            {
               pui32TxPtr[i] = (0x5555AAAA);
            }
            break;
        case 1:
            //
            // 0xFFFF0000
            //
            for (uint32_t i = 0; i < len / 4; i++)
            {
               pui32TxPtr[i] = (0xFFFF0000);
            }
            break;
        case 2:
            //
            // walking
            //
            for (uint32_t i = 0; i < len; i++)
            {
               pui8TxPtr[i] = 0x01 << (i % 8);
            }
            break;
        case 3:
            //
            // incremental from 1
            //
            for (uint32_t i = 0; i < len; i++)
            {
               pui8TxPtr[i] = ((i + 1) & 0xFF);
            }
            break;
        case 4:
            //
            // decremental from 0xff
            //
            for ( uint32_t i = 0; i < len; i++ )
            {
                //
                // decrement starting from 0xff
                //
                pui8TxPtr[i] = (0xff - i) & 0xFF;
            }
            break;
        default:
            //
            // incremental from 1
            //
            for (uint32_t i = 0; i < len; i++)
            {
               pui8TxPtr[i] = ((i + 1) & 0xFF);
            }
            break;

    }

    return 0;
}

//#define MEMORY_WORD_ACCESS
//#define MEMORY_SHORT_ACCESS
//#define MEMORY_BYTE_ACCESS
#define MEMORY_COPY_ACCESS
#if defined(MEMORY_WORD_ACCESS)
AM_SHARED_RW uint8_t ui32TxBuffer[PSRAM_CHECK_DATA_SIZE_BYTES / 4];
AM_SHARED_RW uint8_t ui32RxBuffer[PSRAM_CHECK_DATA_SIZE_BYTES / 4];
#elif defined(MEMORY_SHORT_ACCESS)
AM_SHARED_RW uint8_t ui16TxBuffer[PSRAM_CHECK_DATA_SIZE_BYTES / 2];
AM_SHARED_RW uint8_t ui16RxBuffer[PSRAM_CHECK_DATA_SIZE_BYTES / 2];
#else
AM_SHARED_RW uint8_t ui8TxBuffer[PSRAM_CHECK_DATA_SIZE_BYTES];
AM_SHARED_RW uint8_t ui8RxBuffer[PSRAM_CHECK_DATA_SIZE_BYTES];
#endif

//*****************************************************************************
//
//! @brief
//!
//! @param length
//! @param address
//!
//! @return
//
//*****************************************************************************
static bool
psram_check_by_xip(uint32_t length, uint32_t address)
{
    //
    // Try to use as less ram as possible in stack
    //
    uint32_t ui32NumberOfBytesLeft = length;
    uint32_t ui32TestBytes = 0;
    uint32_t ui32AddressOffset = 0;
    uint8_t ui8PatternCounter = 0;
    uint32_t ix;

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

#if defined(MEMORY_WORD_ACCESS)

        //
        // Write to target address with test pattern with given length
        // Use 5 patterns: 0x5555AAAA, 0xFFFF0000, Walking, incremental and decremental
        //
        prepare_test_pattern((ui8PatternCounter) % 5, (uint8_t*)ui32TxBuffer, ui32TestBytes);
        ui8PatternCounter++;
        // write to target address
        am_hal_sysctrl_bus_write_flush();
        uint32_t * pu32Ptr = (uint32_t *)(address + ui32AddressOffset);
        for (ix = 0; ix < ui32TestBytes / 4; ix++)
        {
          *pu32Ptr++ = ui32TxBuffer[ix];
        }

        //
        // Read back data
        //
        am_hal_sysctrl_bus_write_flush();
        pu32Ptr = (uint32_t *)(address + ui32AddressOffset);
        for (ix = 0; ix < ui32TestBytes / 4; ix++)
        {
           ui32RxBuffer[ix] = *pu32Ptr++ ;
        }
        //
        // Verify the result
        //
        for (ix = 0; ix < ui32TestBytes / 4; ix++)
        {
            if (ui32RxBuffer[ix] != ui32TxBuffer[ix])
            {
                //am_util_debug_printf("    Failed to verify at offset 0x%08x, expect data : 0x%08x, read data : 0x%08x !\n", ui32AddressOffset, ui8RxBuffer[ix], ui8TxBuffer[ix]);
                //
                // verify failed, return directly
                //
                return false;

            }
        }

#elif defined(MEMORY_SHORT_ACCESS)

        //
        // Write to target address with test pattern with given length
        // Use 5 patterns: 0x5555AAAA, 0xFFFF0000, Walking, incremental and decremental
        //
        prepare_test_pattern((ui8PatternCounter) % 5, (uint8_t*)ui16TxBuffer, ui32TestBytes);
        ui8PatternCounter++;
        //
        // write to target address
        //
        am_hal_sysctrl_bus_write_flush();
        uint16_t * pu16Ptr = (uint16_t *)(address + ui32AddressOffset);
        for (ix = 0; ix < ui32TestBytes / 2; ix++)
        {
          *pu16Ptr++ = ui16TxBuffer[ix];
        }

        //
        // Read back data
        //
        am_hal_sysctrl_bus_write_flush();
        pu16Ptr = (uint16_t *)(address + ui32AddressOffset);
        for (ix = 0; ix < ui32TestBytes / 2; ix++)
        {
           ui16RxBuffer[ix] = *pu16Ptr++ ;
        }
        //
        // Verify the result
        //
        for (ix = 0; ix < ui32TestBytes / 2; ix++)
        {
            if (ui16RxBuffer[ix] != ui16TxBuffer[ix])
            {
                //am_util_debug_printf("    Failed to verify at offset 0x%08x, expect data : 0x%08x, read data : 0x%08x !\n", ui32AddressOffset, ui8RxBuffer[ix], ui8TxBuffer[ix]);
                //
                // verify failed, return directly
                //
                return false;

            }
        }
#elif defined(MEMORY_BYTE_ACCESS)

        //
        // Write to target address with test pattern with given length
        // Use 5 patterns: 0x5555AAAA, 0xFFFF0000, Walking, incremental and decremental
        //

        prepare_test_pattern((ui8PatternCounter) % 5, ui8TxBuffer, ui32TestBytes);
        ui8PatternCounter++;
        //
        // write to target address
        //
        am_hal_sysctrl_bus_write_flush();
        uint8_t * pu8Ptr = (uint8_t *)(address + ui32AddressOffset);
        for (ix = 0; ix < ui32TestBytes; ix++)
        {
          *pu8Ptr++ = ui8TxBuffer[ix];
        }

        //
        // Read back data
        //
        am_hal_sysctrl_bus_write_flush();
        pu8Ptr = (uint8_t *)(address + ui32AddressOffset);
        for (ix = 0; ix < ui32TestBytes; ix++)
        {
          ui8RxBuffer[ix] = *pu8Ptr++ ;
        }

        //
        // Verify the result
        //
        for (ix = 0; ix < ui32TestBytes; ix++)
        {
            if (ui8RxBuffer[ix] != ui8TxBuffer[ix])
            {
                //am_util_debug_printf("    Failed to verify at offset 0x%08x, expect data : 0x%08x, read data : 0x%08x !\n", ui32AddressOffset, ui8RxBuffer[ix], ui8TxBuffer[ix]);
                //
                // verify failed, return directly
                //
                return false;

            }

        }
#elif defined(MEMORY_COPY_ACCESS)

        //
        // Write to target address with test pattern with given length
        // Use 5 patterns: 0x5555AAAA, 0xFFFF0000, Walking, incremental and decremental
        //

        prepare_test_pattern((ui8PatternCounter) % 5, ui8TxBuffer, ui32TestBytes);
        ui8PatternCounter++;
        //
        // write to target address
        //
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
        for (ix = 0; ix < ui32TestBytes; ix++)
        {
            if (ui8RxBuffer[ix] != ui8TxBuffer[ix])
            {
                //am_util_debug_printf("    Failed to verify at offset 0x%08x, expect data : 0x%08x, read data : 0x%08x !\n", ui32AddressOffset, ui8RxBuffer[ix], ui8TxBuffer[ix]);
                //
                // verify failed, return directly
                //
                return false;

            }

        }
#endif

        ui32AddressOffset += ui32TestBytes;
    }

    return true;
}

//*****************************************************************************
//
//! @brief Count the longest consecutive 1s in a 32bit word
//! @details Static helper function:
//!
//! @param pVal
//!
//! @return
//
//*****************************************************************************
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

//*****************************************************************************
//
//! @brief Find and return the mid point of the longest continuous 1s in a 32bit word
//! @details Static helper function:
//!
//! @param pVal
//!
//! @return
//
//*****************************************************************************
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
        //
        // window is likely on low side
        //
        pick_point = pick_point - remainder;    // minus only when pattern length is odd
    }
    else if ( (pick_point > 15) && (val & 0x40000000) )
    {
        //
        // window is likely on high side
        //
        pick_point = pick_point + 1;
    }
    else
    {
        //
        // window is in the middle, no action
        //
    }

    return pick_point;
}

//*****************************************************************************
//
//  Checks PSRAM timing and determine a delay setting.
//
//*****************************************************************************
#if defined(AM_PART_APOLLO5_API)
#define PSRAM_TIMING_SCAN_SIZE_BYTES (4*AM_DEVICES_MSPI_PSRAM_PAGE_SIZE)
#else
#define PSRAM_TIMING_SCAN_SIZE_BYTES (128*AM_DEVICES_MSPI_PSRAM_PAGE_SIZE)
#endif

#if defined(AM_PART_APOLLO4P)
#define SCAN_TXDQSDELAY
#define SCAN_RXDQSDELAY
#elif defined(AM_PART_APOLLO4L)
#define SCAN_RXDQSDELAY
#else
#define SCAN_TXDQSDELAY
#define SCAN_RXDQSDELAY
#endif

#if defined(SCAN_TXNEG)
#define SCAN_TXNEG_START 0
#define SCAN_TXNEG_END   1
#endif

#if defined(SCAN_RXNEG)
#define SCAN_RXNEG_START 0
#define SCAN_RXNEG_END   1
#endif

#if defined(SCAN_RXCAP)
#define SCAN_RXCAP_START 0
#define SCAN_RXCAP_END   1
#endif

#if defined(SCAN_TURNAROUND)
#define SCAN_TURNAROUND_START 0
#define SCAN_TURNAROUND_END   1
#endif

#if defined(SCAN_TXDQSDELAY)
#define SCAN_TXDQSDELAY_START 0
#define SCAN_TXDQSDELAY_END   31
#endif

#if defined(SCAN_RXDQSDELAY)
#define SCAN_RXDQSDELAY_START 0
#define SCAN_RXDQSDELAY_END   31
#endif

//*****************************************************************************
//
// Checks PSRAM timing and determine a delay setting.
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_aps25616ba_ddr_init_timing_check(uint32_t ui32Module,
                                                       am_devices_mspi_psram_config_t *pDevCfg,
                                                       am_devices_mspi_psram_ddr_timing_config_t *pDevDdrCfg)
{
    uint32_t ui32Status;
    am_devices_mspi_psram_t *pPsram;
    void *pHandle;
    uint32_t Txdqsdelay = 0;
    uint32_t Rxdqsdelay = 0;

    uint32_t ui32CheckAddress;
    uint32_t ui32CCOResult = 0;
    uint32_t ui32TxResult = 0;
    uint32_t ui32RxResultArray[32];

    am_devices_mspi_psram_ddr_timing_config_t *pTimingCfg;
    am_hal_mspi_timing_scan_t scanCfg;
    //
    // initialize interface
    //
    ui32Status = am_devices_mspi_psram_aps25616ba_ddr_init(ui32Module, pDevCfg, (void **)&pPsram, &pHandle);
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        am_util_debug_printf("    Failed to configure the MSPI and PSRAM Device correctly!\n");
        return ui32Status;
    }

    //
    //get value configured by init & hal
    //
    ui32Status = am_hal_mspi_control(pPsram->pMspiHandle, AM_HAL_MSPI_REQ_TIMING_SCAN_GET, &scanCfg);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    //
    // Put the MSPI into XIP mode.
    //
    ui32Status = am_devices_mspi_psram_aps25616ba_ddr_enable_xip(pPsram);
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        am_util_debug_printf("    Failed to disable XIP mode in the MSPI!\n");
        return ui32Status;
    }

    pTimingCfg = pPsram->pTimingCfg;
#if defined(FAST_TIMING_SCAN)
    if ( pTimingCfg->bValid )
    {
        //
        // apply settings
        //
        ui32Status = am_hal_mspi_control(pPsram->pMspiHandle, AM_HAL_MSPI_REQ_TIMING_SCAN, &pTimingCfg->sTimingCfg);
        if (AM_HAL_STATUS_SUCCESS != ui32Status)
        {
            return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
        }
        //
        // run data check
        //
        if ( psram_check_by_xip(PSRAM_TIMING_SCAN_SIZE_BYTES, pPsram->pXipCfg->ui32APBaseAddr) )
        {
          *pDevDdrCfg = *pTimingCfg;
          am_devices_mspi_psram_aps25616ba_ddr_deinit(pPsram);
          am_util_debug_printf("    Skipping Timing Scan!\n");
          return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
        }
    }
#endif

    am_util_debug_printf("    Start Use XIP to Timing Scan!\n");
    uint32_t ui32TxNeg = scanCfg.bTxNeg;
    uint32_t ui32RxNeg = scanCfg.bRxNeg;
    uint32_t ui32RxCap = scanCfg.bRxCap;
#if defined(SCAN_TURNAROUND)
    uint32_t ui32Turnaround = scanCfg.ui8Turnaround;
#endif
#if defined(SCAN_TXNEG)
    for ( ui32TxNeg = SCAN_TXNEG_START; ui32TxNeg <= SCAN_TXNEG_END; ui32TxNeg++ )
#endif
    {
        scanCfg.bTxNeg = (bool)ui32TxNeg;
#if defined(SCAN_RXNEG)
        for ( ui32RxNeg = SCAN_RXNEG_START; ui32RxNeg <= SCAN_RXNEG_END; ui32RxNeg++ )
#endif
        {
            scanCfg.bRxNeg = (bool)ui32RxNeg;
#if defined(SCAN_RXCAP)
            for ( ui32RxCap = SCAN_RXCAP_START; ui32RxCap <= SCAN_RXCAP_END; ui32RxCap++ )
#endif
            {
                scanCfg.bRxCap = (bool)ui32RxCap;
#if defined(SCAN_TURNAROUND)
                for (scanCfg.ui8Turnaround = ui32Turnaround + SCAN_TURNAROUND_START; scanCfg.ui8Turnaround <= ui32Turnaround + SCAN_TURNAROUND_END; scanCfg.ui8Turnaround++ )
#endif
                {
                    am_util_debug_printf("    TxNeg=%d, RxNeg=%d, RxCap=%d, Turnaround=%d\n", scanCfg.bTxNeg, scanCfg.bRxNeg, scanCfg.bRxCap, scanCfg.ui8Turnaround)
                    ui32TxResult = 0;
                    memset(ui32RxResultArray, 0, sizeof(ui32RxResultArray));
#if defined(SCAN_TXDQSDELAY)
                    for (scanCfg.ui8TxDQSDelay = SCAN_TXDQSDELAY_START; scanCfg.ui8TxDQSDelay <= SCAN_TXDQSDELAY_END; scanCfg.ui8TxDQSDelay++)
#endif
                    {
#if defined(SCAN_RXDQSDELAY)
                        for (scanCfg.ui8RxDQSDelay = SCAN_RXDQSDELAY_START; scanCfg.ui8RxDQSDelay <= SCAN_RXDQSDELAY_END; scanCfg.ui8RxDQSDelay++)
#endif
                        {
                            //
                            // apply settings
                            //
                            ui32Status = am_hal_mspi_control(pPsram->pMspiHandle, AM_HAL_MSPI_REQ_TIMING_SCAN, &scanCfg);
                            if (AM_HAL_STATUS_SUCCESS != ui32Status)
                            {
                                return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
                            }
                            uint32_t ui32Offset = (scanCfg.bTxNeg + scanCfg.bRxNeg + scanCfg.bRxCap + scanCfg.ui8Turnaround) * PSRAM_TIMING_SCAN_SIZE_BYTES + (scanCfg.ui8TxDQSDelay + scanCfg.ui8RxDQSDelay) * 2;
                            ui32CheckAddress = pPsram->pXipCfg->ui32APBaseAddr + ui32Offset;
                            //
                            // run data check
                            //
                            if ( psram_check_by_xip(PSRAM_TIMING_SCAN_SIZE_BYTES, ui32CheckAddress) )
                            {
                                //
                                // data check pass
                                //
                                ui32RxResultArray[scanCfg.ui8TxDQSDelay] |= 0x01 << scanCfg.ui8RxDQSDelay;
                            }
                            else
                            {
                                //
                                // data check failed
                                //
                            }
                        }
#if defined(SCAN_RXDQSDELAY)
                        ui32CCOResult = count_consecutive_ones(&ui32RxResultArray[scanCfg.ui8TxDQSDelay]);
                        if ( ui32CCOResult > PSRAM_TIMING_SCAN_MIN_ACCEPTANCE_LENGTH )
                        {
                            ui32TxResult |= 0x01 << scanCfg.ui8TxDQSDelay;
                        }
                        am_util_debug_printf("    TxDQSDelay: %d, RxDQSDelay Scan = 0x%08X, Window size = %d\n", scanCfg.ui8TxDQSDelay, ui32RxResultArray[scanCfg.ui8TxDQSDelay], ui32CCOResult);
#else
                        if ( ui32RxResultArray[scanCfg.ui8TxDQSDelay] != 0 )
                        {
                            ui32TxResult |= 0x01 << scanCfg.ui8TxDQSDelay;
                        }
                        am_util_debug_printf("    TxDQSDelay: %d, RxDQSDelay Scan = 0x%08X\n", scanCfg.ui8TxDQSDelay, ui32RxResultArray[scanCfg.ui8TxDQSDelay]);
#endif
                    }
                    //
                    // Check Result
                    //
                    if ( ui32TxResult == 0 )
                    {
                        //
                        // no window is found
                        //
#if defined(SCAN_TXNEG) || defined(SCAN_RXNEG) || defined(SCAN_RXCAP) || defined(SCAN_TURNAROUND)
                        continue;
#else
                        am_util_debug_printf("Timing Scan found no window!\n");
                        //
                        // Deinitialize the MSPI interface
                        //
                        am_devices_mspi_psram_aps25616ba_ddr_deinit(pPsram);
                        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
#endif
                    }
#if defined(SCAN_TXDQSDELAY)
                    //
                    // Find TXDQSDELAY Value
                    //
                    Txdqsdelay = find_mid_point(&ui32TxResult);
#else
                    Txdqsdelay = scanCfg.ui8TxDQSDelay;
#endif

#if defined(SCAN_RXDQSDELAY)
                    //
                    // Find RXDQSDELAY Value
                    //
                    Rxdqsdelay = find_mid_point(&ui32RxResultArray[Txdqsdelay]);
#else
                    Rxdqsdelay = scanCfg.ui8RxDQSDelay;
#endif

                    am_util_debug_printf("Selected timing scan setting: TxNeg=%d, RxNeg=%d, RxCap=%d, Turnaround=%d, TxDQSDelay=%d, RxDQSDelay=%d\n", scanCfg.bTxNeg, scanCfg.bRxNeg, scanCfg.bRxCap, scanCfg.ui8Turnaround, Txdqsdelay, Rxdqsdelay);

                    scanCfg.ui8TxDQSDelay = Txdqsdelay;
                    scanCfg.ui8RxDQSDelay = Rxdqsdelay;
                    pTimingCfg->sTimingCfg = scanCfg;
                    pTimingCfg->bValid = true;
                    //
                    // Set output values
                    //
                    *pDevDdrCfg = *pTimingCfg;
                    am_devices_mspi_psram_aps25616ba_ddr_deinit(pPsram);
                    return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
                }
            }
        }
    }
    am_util_debug_printf("Timing Scan found no window!\n");
    //
    // Deinitialize the MSPI interface
    //
    am_devices_mspi_psram_aps25616ba_ddr_deinit(pPsram);

    return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
}

//*****************************************************************************
//
//  Apply given DDR timing settings to target MSPI instance.
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_aps25616ba_apply_ddr_timing(void *pHandle,
                                                  am_devices_mspi_psram_ddr_timing_config_t *pDevDdrCfg)
{
    am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;
    if ( pHandle == NULL )
    {
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }
    //
    // apply timing settings
    //
    *pPsram->pTimingCfg = *pDevDdrCfg;
    am_util_stdio_printf("    Apply Timing TxNeg=%d, RxNeg=%d, RxCap=%d, Turnaround=%d, TxDQSDelay=%d, RxDQSDelay=%d\n", pDevDdrCfg->sTimingCfg.bTxNeg,
                                                                                                                         pDevDdrCfg->sTimingCfg.bRxNeg,
                                                                                                                         pDevDdrCfg->sTimingCfg.bRxCap,
                                                                                                                         pDevDdrCfg->sTimingCfg.ui8Turnaround,
                                                                                                                         pDevDdrCfg->sTimingCfg.ui8TxDQSDelay,
                                                                                                                         pDevDdrCfg->sTimingCfg.ui8RxDQSDelay);
    return am_hal_mspi_control(pPsram->pMspiHandle, AM_HAL_MSPI_REQ_TIMING_SCAN, &pPsram->pTimingCfg->sTimingCfg);

}
#endif

//*****************************************************************************
//
// Enter half sleep
//
// Send a command to Enter Half Sleep Mode. Will need to Be in OCTAL mode
// to access the register per the device driver.
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_aps25616ba_enter_halfsleep(void *pHandle)
{
    uint32_t ui32PIOBuffer = 0xF0;
    am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;

    if ( pHandle == NULL )
    {
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    am_hal_mspi_dev_config_t stSetting = pPsram->stSetting;
    //
    // Disable MSPI defore re-configuring it
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_disable(pPsram->pMspiHandle))
    {
        am_util_debug_printf("Error - Failed to Disable MSPI.\n");
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    //
    // Reconfigure MSPI to OCTAL
    //
    switch (pPsram->stSetting.eDeviceConfig)
    {
        case AM_HAL_MSPI_FLASH_OCTAL_DDR_CE0:
        case AM_HAL_MSPI_FLASH_HEX_DDR_CE0:
            stSetting.eDeviceConfig = AM_HAL_MSPI_FLASH_OCTAL_DDR_CE0;
            break;
        case AM_HAL_MSPI_FLASH_OCTAL_DDR_CE1:
        case AM_HAL_MSPI_FLASH_HEX_DDR_CE1:
            stSetting.eDeviceConfig = AM_HAL_MSPI_FLASH_OCTAL_DDR_CE1;
            break;
        default:
            return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }
    //
    // maintain 24MHz clock so that when exiting halfsleep, the dummy command CE low pulse is fixed.
    //
    stSetting.eClockFreq = MSPI_BASE_FREQUENCY;

    if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_device_configure(pPsram->pMspiHandle, &stSetting))
    {
        am_util_debug_printf("Error - Failed to configure MSPI device.\n");
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    //
    // Re-Enable MSPI
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_enable(pPsram->pMspiHandle))
    {
        am_util_debug_printf("Error - Failed to Enable MSPI!\n");
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    //
    // Re-config the MSPI pins.
    //
    am_bsp_mspi_pins_enable(pPsram->ui32Module, stSetting.eDeviceConfig);

    //
    // Send command to Enter half sleep
    //
    if (AM_HAL_STATUS_SUCCESS != am_device_command_write(pPsram->pMspiHandle, AM_DEVICES_MSPI_PSRAM_DDR_WRITE_REGISTER, true, 6, &ui32PIOBuffer, 1))
    {
        am_util_debug_printf("Failed to write PSRAM MR6 register!\n");
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    am_util_delay_us(APS25616BA_tHS_MIN_US);

    return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

//*****************************************************************************
//
// This function resets the device to bring it out of halfsleep
//
//*****************************************************************************
uint32_t
am_devices_mspi_psram_aps25616ba_exit_halfsleep(void *pHandle)
{
    am_devices_mspi_psram_t *pPsram = (am_devices_mspi_psram_t *)pHandle;

    if ( pHandle == NULL )
    {
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }
#if !defined(AM_PART_APOLLO5_API)
    uint32_t pin_num = 0;
    am_hal_gpio_pincfg_t gpio_pincfg = AM_HAL_GPIO_PINCFG_DEFAULT;
    am_bsp_mspi_ce_pincfg_get(pPsram->ui32Module, pPsram->eDeviceConfig, &pin_num, &gpio_pincfg);

    //
    // Configure CE pin to output and hold high
    //
    am_hal_gpio_output_set(pin_num);
    am_hal_gpio_pinconfig(pin_num, am_hal_gpio_pincfg_output);

    //
    // Start reset pulse on CE1
    //
    am_hal_gpio_output_clear(pin_num);

    //
    // hold reset pin for 60ns - 500ns
    //
    APS25616BA_tXPHS_delay(1);

    //
    // Set pin to high to finish reset
    //
    am_hal_gpio_output_set(pin_num);

    //
    // Reconfigure pin for CE on PSRAM
    //
    am_hal_gpio_pinconfig(pin_num, gpio_pincfg);
#else
    uint32_t ui32PIOBuffer = 0;
    //
    // Send dummy command to pull CE for tXPHS
    //
    if (AM_HAL_STATUS_SUCCESS != am_device_command_write(pPsram->pMspiHandle, 0x0000, true, 0, &ui32PIOBuffer, 2))
    {
      am_util_debug_printf("Error - Failed to send dummy command.\n");
      return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }
#endif
    //
    // Delay after setting pin high to allow for device to accept command
    //  and go into half sleep mode
    //
    am_util_delay_us(APS25616BA_tXHS_MIN_US);

    //
    // Disable MSPI defore re-configuring it
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_disable(pPsram->pMspiHandle))
    {
        am_util_debug_printf("Error - Failed to Disable MSPI.\n");
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    //
    // Reconfigure MSPI
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_device_configure(pPsram->pMspiHandle, &pPsram->stSetting))
    {
        am_util_debug_printf("Error - Failed to configure MSPI device.\n");
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    //
    // Re-Enable MSPI
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_mspi_enable(pPsram->pMspiHandle))
    {
        am_util_debug_printf("Error - Failed to Enable MSPI!\n");
        return AM_DEVICES_MSPI_PSRAM_STATUS_ERROR;
    }

    //
    // Re-config the MSPI pins.
    //
    am_bsp_mspi_pins_enable(pPsram->ui32Module, pPsram->stSetting.eDeviceConfig);

    return AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS;
}

#if (defined (__ARMCC_VERSION)) && (__ARMCC_VERSION < 6000000)
__asm void
APS25616BA_tXPHS_delay( uint32_t ui32Iterations )
{
    subs    r0, #1
    bne     APS25616BA_tXPHS_delay
    bx      lr
}
#elif (defined (__ARMCC_VERSION)) && (__ARMCC_VERSION >= 6000000)
void
APS25616BA_tXPHS_delay( uint32_t ui32Iterations )
{
  __asm
  (
    " subs  r0, #1\n"
    " bne   APS25616BA_tXPHS_delay\n"
  );
}
#elif defined(__GNUC_STDC_INLINE__)
__attribute__((naked))
void
APS25616BA_tXPHS_delay( uint32_t ui32Iterations )
{
    __asm
    (
        "   subs    r0, #1\n"
        "   bne     APS25616BA_tXPHS_delay\n"
        "   bx      lr\n"
    );
}
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma diag_suppress = Pe940   // Suppress IAR compiler warning about missing
                                // return statement on a non-void function
__stackless inline void
APS25616BA_tXPHS_delay( uint32_t ui32Iterations )
{
    __asm(" subs    r0, #1 ");
    __asm(" bne     APS25616BA_tXPHS_delay ");
}
#pragma diag_default = Pe940    // Restore IAR compiler warning
#else
#error Compiler is unknown, please contact Ambiq support team
#endif

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
