//*****************************************************************************
//
//! @file hci_drv_apollo3.h
//!
//! @brief Support functions for the Nationz BTLE radio in Apollo3.
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
#ifndef HCI_DRV_APOLLO3_H
#define HCI_DRV_APOLLO3_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// NATIONZ vendor specific events
//
//*****************************************************************************


// Tx power level in dBm.
typedef enum
{
  TX_POWER_LEVEL_MINUS_10P0_dBm = 0x4,
  TX_POWER_LEVEL_MINUS_5P0_dBm = 0x5,
  TX_POWER_LEVEL_0P0_dBm = 0x8,
  TX_POWER_LEVEL_PLUS_3P0_dBm = 0xF,
  TX_POWER_LEVEL_INVALID = 0x10,
}txPowerLevel_t;


bool_t HciVscSetRfPowerLevelEx(txPowerLevel_t txPowerlevel);
#define HciVsA3_SetRfPowerLevelEx HciVscSetRfPowerLevelEx
void   HciVscConstantTransmission(uint8_t txchannel);
#define HciVsA3_ConstantTransmission HciVscConstantTransmission
void   HciVscCarrierWaveMode(uint8_t txchannel);
#define HciVsA3_CarrierWaveMode HciVscCarrierWaveMode
bool_t HciVscSetCustom_BDAddr(uint8_t *bd_addr);
extern void HciVscUpdateBDAddress(void);

//*****************************************************************************
//
// Hci driver functions unique to Apollo3
//
//*****************************************************************************
extern void HciDrvHandler(wsfEventMask_t event, wsfMsgHdr_t *pMsg);
extern void HciDrvHandlerInit(wsfHandlerId_t handlerId);
extern void HciDrvIntService(void);

#ifdef __cplusplus
}
#endif

#endif // HCI_DRV_APOLLO3_H
