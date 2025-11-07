//*****************************************************************************
//
//! @file hci_drv_510L_radio.h
//!
//! @brief Support functions for the Apollo510L radio.
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
// This is part of revision release_sdk5_2_a_1_1-c2486c8ef of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef HCI_DRV_510L_RADIO_H
#define HCI_DRV_510L_RADIO_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "am_devices_510L_radio.h"


#define LL_FEATURES_BYTE0  ( HCI_LE_SUP_FEAT_ENCRYPTION  \
                            | HCI_LE_SUP_FEAT_CONN_PARAM_REQ_PROC \
                            | HCI_LE_SUP_FEAT_EXT_REJECT_IND \
                            | HCI_LE_SUP_FEAT_SLV_INIT_FEAT_EXCH \
                            | HCI_LE_SUP_FEAT_LE_PING \
                            | HCI_LE_SUP_FEAT_DATA_LEN_EXT \
                            | HCI_LE_SUP_FEAT_PRIVACY \
                            | HCI_LE_SUP_FEAT_EXT_SCAN_FILT_POLICY )

#define LL_FEATURES_BYTE1  ( HCI_LE_SUP_FEAT_LE_2M_PHY \
                            |HCI_LE_SUP_FEAT_LE_CODED_PHY\
                            | HCI_LE_SUP_FEAT_LE_EXT_ADV \
                            | HCI_LE_SUP_FEAT_LE_PER_ADV \
                            | HCI_LE_SUP_FEAT_CH_SEL_2 )

#define LL_FEATURES_BYTE2  ( HCI_LE_SUP_FEAT_MIN_NUN_USED_CHAN )

#define LL_FEATURES_BYTE3  (HCI_LE_SUP_FEAT_PAST_SENDER \
                            | HCI_LE_SUP_FEAT_PAST_RECIPIENT \
                            | HCI_LE_SUP_FEAT_SCA_UPDATE \
                            |HCI_LE_SUP_FEAT_REMOTE_PUB_KEY_VALIDATION\
                            | HCI_LE_SUP_FEAT_CIS_MASTER \
                            | HCI_LE_SUP_FEAT_CIS_SLAVE \
                            | HCI_LE_SUP_FEAT_ISO_BROADCASTER \
                            | HCI_LE_SUP_FEAT_ISO_SYNC_RECEIVER )

#define LL_FEATURES_BYTE4  (HCI_LE_SUP_FEAT_POWER_CONTROL_REQUEST \
                            | HCI_LE_SUP_FEAT_POWER_CHANGE_IND \
                            | HCI_LE_SUP_FEAT_PATH_LOSS_MONITOR \
                            | HCI_LE_SUP_FEAT_PRR_ADV_ADI \
                            | HCI_LE_SUP_FEAT_SUBRATING \
                            | HCI_LE_SUP_FEAT_CHAN_CLASSIFICATION )

#define LL_FEATURES_BYTE5  (HCI_LE_SUP_FEAT_ADV_CODING_SELECTION \
                            | HCI_LE_SUP_FEAT_PER_ADV_WITH_RESP_ADVERTISER \
                            | HCI_LE_SUP_FEAT_PER_ADV_WITH_RESP_SCANNER )

#define LL_FEATURES_BYTE6 (0ULL)

#define LL_FEATURES_BYTE7 (0ULL)

//*****************************************************************************
//
// AMBIQ vendor specific events
//
//*****************************************************************************
// Tx power level in dBm.
typedef enum
{
    TX_POWER_LEVEL_MINUS_20P0_dBm = -20,
    TX_POWER_LEVEL_MINUS_18P0_dBm = -18,
    TX_POWER_LEVEL_MINUS_16P0_dBm = -16,
    TX_POWER_LEVEL_MINUS_14P0_dBm = -14,
    TX_POWER_LEVEL_MINUS_12P0_dBm = -12,
    TX_POWER_LEVEL_MINUS_10P0_dBm = -10,
    TX_POWER_LEVEL_MINUS_8P0_dBm = -8,
    TX_POWER_LEVEL_MINUS_6P0_dBm = -6,
    TX_POWER_LEVEL_MINUS_4P0_dBm = -4,
    TX_POWER_LEVEL_MINUS_2P0_dBm = -2,
    TX_POWER_LEVEL_0P0_dBm = 0,
    TX_POWER_LEVEL_PLUS_1P0_dBm = 1,
    TX_POWER_LEVEL_PLUS_2P0_dBm = 2,
    TX_POWER_LEVEL_PLUS_3P0_dBm = 3,
    TX_POWER_LEVEL_PLUS_4P0_dBm = 4,
    TX_POWER_LEVEL_PLUS_5P0_dBm = 5,
    TX_POWER_LEVEL_PLUS_6P0_dBm = 6,
    TX_POWER_LEVEL_PLUS_7P0_dBm = 7,
    TX_POWER_LEVEL_PLUS_8P0_dBm = 8,
    TX_POWER_LEVEL_PLUS_9P0_dBm = 9,
    TX_POWER_LEVEL_PLUS_10P0_dBm = 10,
    TX_POWER_LEVEL_PLUS_11P0_dBm = 11,
    TX_POWER_LEVEL_PLUS_12P0_dBm = 12,
    TX_POWER_LEVEL_PLUS_13P0_dBm = 13,
    TX_POWER_LEVEL_INVALID,
}txPowerLevel_t;

// Set the default BLE TX Output power to +0dBm.
#define TX_POWER_LEVEL_DEFAULT TX_POWER_LEVEL_0P0_dBm

//*****************************************************************************
//
// Hci driver functions
//
//*****************************************************************************
extern void HciDrvHandler(wsfEventMask_t event, wsfMsgHdr_t *pMsg);
extern void HciDrvHandlerInit(wsfHandlerId_t handlerId);
extern void HciVscUpdateNvdsParam(void);
extern void HciVscUpdateBDAddress(void);
extern void HciVscUpdateLinklayerFeature(void);
extern void HciVscUpdateTxpwrLevel(txPowerLevel_t txPowerlevel);

#ifdef __cplusplus
}
#endif

#endif // HCI_DRV_510L_RADIO_H
