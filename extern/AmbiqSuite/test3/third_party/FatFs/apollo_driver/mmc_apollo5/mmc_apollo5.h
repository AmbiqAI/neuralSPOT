/*----------------------------------------------------------------------------/
/  FatFs - Generic FAT Filesystem Module  R0.14                               /
/-----------------------------------------------------------------------------/
/
/ Copyright (C) 2020, ChaN, all right reserved.
/
/ FatFs module is an open source software. Redistribution and use of FatFs in
/ source and binary forms, with or without modification, are permitted provided
/ that the following condition is met:
/
/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/
/----------------------------------------------------------------------------*/

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
// This is part of revision release_sdk5_2_a_1_1-c2486c8ef of the AmbiqSuite Development Package.
//
//*****************************************************************************

#ifndef MMC_APOLLO5_H
#define MMC_APOLLO5_H

#include "ff.h"
#include "diskio.h"
#include "am_mcu_apollo.h"

// #define EMMC_DEEP_SLEEP_TEST

#define ENABLE_SDIO_POWER_SAVE
#define DELAY_MAX_COUNT             0x1000
#define DCACHE_SIZE 64*1024

typedef struct
{
  //host config
  am_hal_host_inst_index_e    eHost;
  am_hal_host_xfer_mode_e     eXferMode;
  uint32_t                    ui32Clock;
  am_hal_host_bus_width_e     eBusWidth;
  am_hal_host_bus_voltage_e   eBusVoltage;
  am_hal_host_uhs_mode_e      eUHSMode;
  bool                        bAsync;

  //device config
  am_hal_card_type_e              eCardType;
  am_hal_card_pwr_ctrl_policy_e   eCardPwrCtrlPolicy;
  am_hal_card_pwr_ctrl_func       pCardPwrCtrlFunc;
  am_hal_host_event_cb_t          pfunCallback;

  uint32_t ui32SectorCount;

} am_device_emmc_config_t;

typedef struct
{
    am_hal_card_host_t              *pSdhcHost;             // SDIO host handle.
    am_hal_card_t                   *pDevHandle;            // SDIO device handle.

    volatile DSTATUS                dStat;
    volatile bool                   bAsyncWriteIsDone;
    volatile bool                   bAsyncReadIsDone;
} am_device_emmc_hw_t;


extern am_device_emmc_config_t      g_sEMMC_config;
extern am_device_emmc_hw_t    g_sEMMC_hw;

/*---------------------------------------*/
/* Prototypes for disk control functions */

//*****************************************************************************
//
//! @brief MMC disk initialization function
//!
//! @param None
//!
//! This function is called to initialize a drive.
//!
//! @return DSTATUS - current drive status.
//
//*****************************************************************************
DSTATUS mmc_disk_initialize (void);

//*****************************************************************************
//
//! @brief MMC disk status function
//!
//! @param None
//!
//! This function is called to inquire the current drive status.
//!
//! @return DSTATUS - current drive status.
//
//*****************************************************************************
DSTATUS mmc_disk_status (void);

//*****************************************************************************
//
//! @brief MMC disk read function
//!
//! @param buff - Pointer to the first item of the byte array to store read data.
//! @param sector - Start sector number in 32-bit or 64-bit LBA.
//! @param count - Number of sectors to read.
//!
//! This function is called to read data from the sector(s).
//!
//! @return DRESULT - operation result.
//
//*****************************************************************************
DRESULT mmc_disk_read (BYTE* buff, LBA_t sector, UINT count);

//*****************************************************************************
//
//! @brief MMC disk write function
//!
//! @param buff - Pointer to the first item of the byte array to be written.
//! @param sector - Start sector number in 32-bit or 64-bit LBA.
//! @param count - Number of sectors to write.
//!
//! This function is called to write data to the sector(s).
//!
//! @return DRESULT - operation result.
//
//*****************************************************************************
DRESULT mmc_disk_write (const BYTE* buff, LBA_t sector, UINT count);

//*****************************************************************************
//
//! @brief MMC disk control function
//!
//! @param cmd - Command Code.
//! @param buff - Pointer to the parameter depends on the command code.
//!
//! This function is called to control device specific features and miscellaneous
//! functions other than generic read/write..
//!
//! @return DRESULT  - operation result.
//
//*****************************************************************************
DRESULT mmc_disk_ioctl (BYTE cmd, void* buff);

#endif

