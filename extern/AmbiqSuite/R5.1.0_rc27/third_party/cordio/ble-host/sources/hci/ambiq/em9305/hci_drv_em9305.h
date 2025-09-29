//*****************************************************************************
//
//! @file hci_drv_em9305.h
//!
//! @brief Support functions for the EM9305 radio.
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
// This is part of revision release_sdk5p1p0-acc60980d8 of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef HCI_DRV_EM9305_H
#define HCI_DRV_EM9305_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "am_devices_em9305.h"


 ///Transmitter test Packet Payload Type
 enum
 {
    ///Pseudo-random 9 TX test payload type
    PAYL_PSEUDO_RAND_9            = 0x00,
    ///11110000 TX test payload type
    PAYL_11110000,
    ///10101010 TX test payload type
    PAYL_10101010,
    ///Pseudo-random 15 TX test payload type
    PAYL_PSEUDO_RAND_15,
    ///All 1s TX test payload type
    PAYL_ALL_1,
    ///All 0s TX test payload type
    PAYL_ALL_0,
    ///00001111 TX test payload type
    PAYL_00001111,
    ///01010101 TX test payload type
    PAYL_01010101,
};

 /*! \brief LE vendor transmitter test command parameters */
 typedef struct
 {
     // TX test mode (PM = Packet Mode, CM = Continuously modulated signal):
     // 0x00 = PM (sending packets as in BLE DTM mode). Not a full implementation of
     // DTM. Please use the standard HCI command for DTM if doing more than just
     // testing the radio can send packetized data.
     // 0x01 = CM - PRBS9 sequence
     // 0x02 = CM - PRBS15 sequence
     // 0x03 = CM - '0000000000000000' sequence
     // 0x04 = CM - '1111111111111111' sequence
     // 0x05 = CM - '0101010101010101' sequence
     // 0x06 = CM - '0011001100110011' sequence
     // 0x07 = CM - '0000111100001111' sequence
     // 0x08 = CM - '0000000011111111' sequence
     // All other values Reserved for future use.
     uint8_t tx_test_mode;
     // TX channel
     // N = (F-2402) / 2
     // Range: 0x00 to 0x27
     // Frequency Range: 2402 MHz to 2480 MHz
     uint8_t tx_channel;
     // Length in bytes of payload data in each packet
     uint8_t test_data_len;
     // Packet payload type:
     // 0x00 = PRBS9 sequence
     // 0x01 = Repeated '11110000' sequence
     // 0x02 = Repeated '10101010' sequence
     // 0x03 = PRBS15
     // 0x04 = Repeated '11111111' sequence
     // 0x05 = Repeated '00000000' sequence
     // 0x06 = Repeated '00001111' sequence
     // 0x07 = Repeated '01010101' sequence
     // All other values Reserved for future use
     uint8_t pkt_payl;
     //PHY mode:
     // 0x01 = Transmitter set to use the LE 1M PHY
     // 0x02 = Transmitter set to use the LE 2M PHY
     // 0x03 = Transmitter set to use the LE Coded PHY with S=8 data coding
     // 0x04 = Transmitter set to use the LE Coded PHY with S=2 data coding
     // All other values Reserved for future use.
     uint8_t phy;
     /// Transmit Power Level(@enum txPowerLevel_t)
     uint8_t tx_power_level;
}__attribute__ ((__packed__))hciVscStartTransTestCmd_t;

/*! \brief LE vendor transmitter test event parameters */
typedef struct{
    // Standard Bluetooth Error Code.
    uint8_t status;
    // Maximum transmit power level available.
    uint8_t tx_power_max;
    // Actual transmit power level used.
    uint8_t tx_power_actual;
}__attribute__ ((__packed__))hciVscStartTransTestEvt_t;

#define LE_FEATS_LEN                   0x08

#define LL_FEATURES_BYTE0  ( HCI_LE_SUP_FEAT_ENCRYPTION  \
                                 | HCI_LE_SUP_FEAT_CONN_PARAM_REQ_PROC \
                                 | HCI_LE_SUP_FEAT_EXT_REJECT_IND \
                                 | HCI_LE_SUP_FEAT_SLV_INIT_FEAT_EXCH \
                                 | HCI_LE_SUP_FEAT_LE_PING \
                                 | HCI_LE_SUP_FEAT_DATA_LEN_EXT \
                                 | HCI_LE_SUP_FEAT_PRIVACY \
                                 | HCI_LE_SUP_FEAT_EXT_SCAN_FILT_POLICY )

#define LL_FEATURES_BYTE1  ( HCI_LE_SUP_FEAT_LE_2M_PHY \
                             | HCI_LE_SUP_FEAT_STABLE_MOD_IDX_RECEIVER \
                             | HCI_LE_SUP_FEAT_LE_CODED_PHY \
                             | HCI_LE_SUP_FEAT_LE_EXT_ADV \
                             | HCI_LE_SUP_FEAT_LE_PER_ADV \
                             | HCI_LE_SUP_FEAT_CH_SEL_2)

#define LL_FEATURES_BYTE2  ( HCI_LE_SUP_FEAT_MIN_NUN_USED_CHAN )

#define LL_FEATURES_BYTE3  (HCI_LE_SUP_FEAT_PAST_SENDER \
                             | HCI_LE_SUP_FEAT_PAST_RECIPIENT \
                             | HCI_LE_SUP_FEAT_SCA_UPDATE \
                             | HCI_LE_SUP_FEAT_REMOTE_PUB_KEY_VALIDATION)

#if (BT_53)
#define LL_FEATURES_BYTE4  (HCI_LE_SUP_FEAT_PATH_LOSS_MONITOR \
                             | HCI_LE_SUP_FEAT_PRR_ADV_ADI\
                             | HCI_LE_SUP_FEAT_SUBRATING \
                             | HCI_LE_SUP_FEAT_SUBRATING_HOST_SUPPORT \
                             | HCI_LE_SUP_FEAT_CHAN_CLASSIFICATION)
#else
#define LL_FEATURES_BYTE4  (HCI_LE_SUP_FEAT_PATH_LOSS_MONITOR \
                             | HCI_LE_SUP_FEAT_PRR_ADV_ADI\
                             | HCI_LE_SUP_FEAT_SUBRATING \
                             | HCI_LE_SUP_FEAT_CHAN_CLASSIFICATION)
#endif // BT_53

#if (BT_54)
#define LL_FEATURES_BYTE5 ( HCI_LE_SUP_FEAT_ADV_CODING_SELECTION \
                             | HCI_LE_SUP_FEAT_ADV_CODING_SELECTION_HOST_SUPPORT \
                             | HCI_LE_SUP_FEAT_PER_ADV_WITH_RESP_ADVERTISER \
                             | HCI_LE_SUP_FEAT_PER_ADV_WITH_RESP_SCANNER)
#else
#define LL_FEATURES_BYTE5 ( HCI_LE_SUP_FEAT_ADV_CODING_SELECTION \
                             | HCI_LE_SUP_FEAT_PER_ADV_WITH_RESP_ADVERTISER \
                             | HCI_LE_SUP_FEAT_PER_ADV_WITH_RESP_SCANNER)
#endif // BT_54

#define TX_POWER_LEVEL_DEFAULT TX_POWER_LEVEL_0P0_dBm

#define TX_POWER_LEVEL_MIN_DBM  (-28)
// Tx power level in dBm
// Range: -28 to +6
typedef enum
{
  TX_POWER_LEVEL_MINUS_20P0_dBm = 0xEC,
  TX_POWER_LEVEL_MINUS_15P0_dBm = 0xF1,
  TX_POWER_LEVEL_MINUS_10P0_dBm = 0xF6,
  TX_POWER_LEVEL_MINUS_5P0_dBm  = 0xFB,
  TX_POWER_LEVEL_0P0_dBm        = 0x0,
  TX_POWER_LEVEL_PLUS_3P0_dBm   = 0x3,
  TX_POWER_LEVEL_PLUS_4P0_dBm   = 0x4,
  TX_POWER_LEVEL_PLUS_6P0_dBm   = 0x6,
  TX_POWER_LEVEL_MAXIMUM        = TX_POWER_LEVEL_PLUS_6P0_dBm,
}txPowerLevel_t;

#define MIN_SWITCHING_PATTERN_LEN  (0x02)
#define TEST_LEN_DEFAULT        (0x25)

// For FCC continous wave testing
// EM9305 does not support a center of band CW mode.
// Instead, it can use CM all 1’s or all 0’s sequence (TX test mode 0x03 or 0x04),
// which will result in a continuous wave with an offset.
#define TEST_MODE_CONTINUOUS_WAVE        0x03
// For FCC continuous modulation testing
#define TEST_MODE_CONTINUOUS_MODULATE    0x7
// Sending packets as in BLE DTM mode
#define TEST_MODE_PACKET_MODE    0

#define TEST_MODE_PKT_PAYLOAD_TYPE_DEFAULT   0x0

#define CHL_2402_INDEX    0        // low frequency
#define CHL_2440_INDEX    19       // medium frequency
#define CHL_2480_INDEX    39       // high frequency

//*****************************************************************************
//
// Hci driver functions unique to EM9305
//
//*****************************************************************************
bool_t HciVscSetRfPowerLevelEx(int8_t txPowerlevel);
void   HciVscConstantTransmission(uint8_t txchannel);
void   HciVscCarrierWaveMode(uint8_t txchannel);
bool_t HciVscSetCustom_BDAddr(uint8_t *bd_addr);
extern void HciVscUpdateBDAddress(void);
extern void HciVscUpdateLinklayerFeature(void);
extern void HciVscDisablePowerCtrl(void);
extern void HciDrvHandler(wsfEventMask_t event, wsfMsgHdr_t *pMsg);
extern void HciDrvHandlerInit(wsfHandlerId_t handlerId);
extern void HciDrvIntService(void);
extern void HciVscTransmitTestStart(hciVscStartTransTestCmd_t * transTestParam);
extern void HciVscTransmitTestEnd(void);
extern void HciDrvBleSleepSet(bool enable);
#ifdef __cplusplus
}
#endif

#endif // HCI_DRV_EM9305_H
