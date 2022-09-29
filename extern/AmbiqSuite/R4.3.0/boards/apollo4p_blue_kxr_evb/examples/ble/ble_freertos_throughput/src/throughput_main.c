// ****************************************************************************
//
//  throughput_main.c
//! @file
//!
//! @brief Ambiq Micro's demonstration of throughput example.
//!
//! @{
//
// ****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2019, Ambiq Micro
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
// This is part of revision v2.2.0-6-g9329ccc58 of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include <stdbool.h>

#include <string.h>
#include "wsf_types.h"
#include "bstream.h"
#include "wsf_msg.h"
#include "wsf_trace.h"
#include "hci_api.h"
#include "dm_api.h"
#include "att_api.h"
#include "app_api.h"
#include "app_ui.h"
#include "svc_core.h"
#include "throughput_api.h"
#include "svc_throughput.h"
#include "am_util.h"

/**************************************************************************************************
  Macros
**************************************************************************************************/
/*! WSF message event starting value */
#define THROUGHPUT_MSG_START               0xA0
#define READ_REM_FEA_TIMEOUT_MS           (2000)

#if defined(PHY_TEST) && (PHY_TEST == 1)
#include "hci_core.h"
typedef struct
{
    bool_t sup2MPhy;
    bool_t supLECodedPhy;
    uint8_t phyMode;
    uint8_t priPhyChanl;
    uint8_t secPhyChanl;
}phyTest_type;

phyTest_type phyTest;
uint8_t advHandle[DM_NUM_ADV_SETS]  = {0};
#endif

/*! WSF message event enumeration */
enum
{
    THROUGHPUT_SEND_DATA_TIMER_IND = THROUGHPUT_MSG_START,  /*! data sending timer expired */
};

/**************************************************************************************************
  Data Types
**************************************************************************************************/

/*! Application message type */
typedef union
{
    wsfMsgHdr_t     hdr;
    dmEvt_t         dm;
    attsCccEvt_t    ccc;
    attEvt_t        att;
} tpMsg_t;

/**************************************************************************************************
  Configurable Parameters
**************************************************************************************************/
#if defined(PHY_TEST) && (PHY_TEST == 1)
static const appExtAdvCfg_t advTestCfg =
{
#if (DM_NUM_ADV_SETS == 1)
    {0},
    {800},
    {0},
    {FALSE},
    {0},
#elif (DM_NUM_ADV_SETS > 1)
    {0, 0},
    {800, 800},
    {0, 0},
    {FALSE, TRUE},
#endif
};
#else
/*! configurable parameters for advertising */
static const appAdvCfg_t tpAdvCfg =
{
    {    0,     0,     0},                  /*! Advertising durations in ms */
    {  800,   800,     0}                   /*! Advertising intervals in 0.625 ms units */
};
#endif
/*! configurable parameters for slave */
static const appSlaveCfg_t tpSlaveCfg =
{
    THROUGHPUT_CONN_MAX,                           /*! Maximum connections */
};

/*! configurable parameters for security */
static const appSecCfg_t tpSecCfg =
{
    0, //DM_AUTH_BOND_FLAG,                 /*! Authentication and bonding flags */
    0,                                      /*! Initiator key distribution flags */
    DM_KEY_DIST_LTK,                        /*! Responder key distribution flags */
    FALSE,                                  /*! TRUE if Out-of-band pairing data is present */
    FALSE                                   /*! TRUE to initiate security upon connection */
};

/*! configurable parameters for throughput connection parameter update */
static appUpdateCfg_t tpUpdateCfg =
{
    0,                                /*! Connection idle period in ms before attempting
                                              connection parameter update; set to zero to disable */
    (7.5 / 1.25),                         /*! 15 ms */
    (15 / 1.25),                         /*! 30 ms */
    4,                                   /*! Connection latency */
    (6000 / 10),                         /*! Supervision timeout in 10ms units */
    5                                    /*! Number of update attempts before giving up */
};

/**************************************************************************************************
  Advertising Data
**************************************************************************************************/

/*! advertising data, discoverable mode */
uint8_t tpAdvDataDisc[] =
{
    /*! flags */
    2,                                      /*! length */
    DM_ADV_TYPE_FLAGS,                      /*! AD type */
    DM_FLAG_LE_GENERAL_DISC |               /*! flags */
    DM_FLAG_LE_BREDR_NOT_SUP,

    /*! device name */
    15,                                     /*! length */
    DM_ADV_TYPE_LOCAL_NAME,                 /*! AD type */
    'a',
    'm',
    'b',
    'i',
    'q',
    '-',
    'B',
    'L',
    'E',
    '-',
    't',
    'e',
    's',
    't',

    /*! service UUID list */
    3,                                      /*! length */
    DM_ADV_TYPE_16_UUID,                    /*! AD type */
    UINT16_TO_BYTES(ATT_UUID_DEVICE_INFO_SERVICE)

};

/*! scan data, discoverable mode */
static const uint8_t tpScanDataDisc[] =
{
    /*! tx power */
    2,                                      /*! length */
    DM_ADV_TYPE_TX_POWER,                   /*! AD type */
    0,                                      /*! tx power */

};

/**************************************************************************************************
  Client Characteristic Configuration Descriptors
**************************************************************************************************/

/*! enumeration of client characteristic configuration descriptors */
enum
{
    THROUGHPUT_GATT_SC_CCC_IDX,             /*! GATT service, service changed characteristic */
    THROUGHPUT_TX_CCC_IDX,                  /*! throughput service, tx characteristic */
    THROUGHPUT_CON_UPT_CCC_IDX,              /*! throughput service, set Connection parameters characteristic */
    THROUGHPUT_NUM_CCC_IDX
};

/*! client characteristic configuration descriptors settings, indexed by above enumeration */
static const attsCccSet_t tpCccSet[THROUGHPUT_NUM_CCC_IDX] =
{
    /* cccd handle          value range               security level */
    {GATT_SC_CH_CCC_HDL,    ATT_CLIENT_CFG_INDICATE,  DM_SEC_LEVEL_NONE},   /* THROUGHPUT_GATT_SC_CCC_IDX */
    {THROUGHPUT_TX_CH_CCC_HDL,  ATT_CLIENT_CFG_NOTIFY,    DM_SEC_LEVEL_NONE},    /* THROUGHPUT_TX_CCC_IDX */
    {THROUGHPUT_CON_UPT_CCC_HDL,  ATT_CLIENT_CFG_NOTIFY,    DM_SEC_LEVEL_NONE}    /* THROUGHPUT_CON_UPT_CCC_IDX */
};

/**************************************************************************************************
  Global Variables
**************************************************************************************************/
#define TX_PACKET_CNT_MAX    65535
#define TX_DATA_TIMER_MS     100
#define UPLINK_DATA_CNT      512
#define BIDIRCT_PKT_CNT_MAX  10000
#define UPLINK_FLAG          0xA5
#define DOWNLINK_FLAG        0x5A
#define MTU_REQ_FLAG         0xFE
#define BIDIRECT_FLAG        0x88
#define UPLINK_DATA          0xAA

// the transmit mode between phone and throughput example
// UPLINK means sending notification from device to phone
// DOWNLINK means writing data from phone to device
// BIDIRECT means firstly device sent notification then phone write data
typedef enum
{
    TRANSMIT_MODE_NONE,
    TRANSMIT_MODE_UPLINK,
    TRANSMIT_MODE_DOWNLINK,
    TRANSMIT_MODE_BIDIRECT
}transmit_mode_t;

uint8_t transmit_mode = TRANSMIT_MODE_NONE;

/* Control block */
static struct
{
    dmConnId_t              connId;
    wsfHandlerId_t          handlerId;
    wsfTimer_t              sendDataTimer;  // timer for sending data
}throughputCb;

uint8_t     uplinkData[UPLINK_DATA_CNT] = {0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};
uint32_t    txIdx = 0;
uint16_t    preRxIdx = 0, rxIdx = 0;
uint16_t    maxTransPacket = TX_PACKET_CNT_MAX;
uint8_t     g_mtu;

/*************************************************************************************************/
/*!
 *  \fn     throughputDmCback
 *
 *  \brief  Application DM callback.
 *
 *  \param  pDmEvt  DM callback event
 *
 *  \return None.
 */
/*************************************************************************************************/
static void throughputDmCback(dmEvt_t *pDmEvt)
{
    dmEvt_t *pMsg;

    if ((pMsg = WsfMsgAlloc(sizeof(dmEvt_t))) != NULL)
    {
        memcpy(pMsg, pDmEvt, sizeof(dmEvt_t));
        WsfMsgSend(throughputCb.handlerId, pMsg);
    }
}

/*************************************************************************************************/
/*!
 *  \fn     throughputAttCback
 *
 *  \brief  Application ATT callback.
 *
 *  \param  pEvt    ATT callback event
 *
 *  \return None.
 */
/*************************************************************************************************/
static void throughputAttCback(attEvt_t *pEvt)
{
    attEvt_t *pMsg;

    if ((pMsg = WsfMsgAlloc(sizeof(attEvt_t) + pEvt->valueLen)) != NULL)
    {
        memcpy(pMsg, pEvt, sizeof(attEvt_t));
        pMsg->pValue = (uint8_t *) (pMsg + 1);
        memcpy(pMsg->pValue, pEvt->pValue, pEvt->valueLen);
        WsfMsgSend(throughputCb.handlerId, pMsg);
    }
}

/*************************************************************************************************/
/*!
 *  \fn     throughputCccCback
 *
 *  \brief  Application ATTS client characteristic configuration callback.
 *
 *  \param  pDmEvt  DM callback event
 *
 *  \return None.
 */
/*************************************************************************************************/
 static void throughputCccCback(attsCccEvt_t *pEvt)
{
    attsCccEvt_t  *pMsg;
    appDbHdl_t    dbHdl;

    /* if CCC not set from initialization and there's a device record */
    if ((pEvt->handle != ATT_HANDLE_NONE) &&
        ((dbHdl = AppDbGetHdl((dmConnId_t) pEvt->hdr.param)) != APP_DB_HDL_NONE))
    {
        /* store value in device database */
        AppDbSetCccTblValue(dbHdl, pEvt->idx, pEvt->value);
    }

    if ((pMsg = WsfMsgAlloc(sizeof(attsCccEvt_t))) != NULL)
    {
        memcpy(pMsg, pEvt, sizeof(attsCccEvt_t));
        WsfMsgSend(throughputCb.handlerId, pMsg);
    }
}

/*************************************************************************************************/
/*!
 *  \fn     throughputProcCccState
 *
 *  \brief  Process CCC state change.
 *
 *  \param  pMsg    Pointer to message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void throughputProcCccState(tpMsg_t *pMsg)
{
    APP_TRACE_INFO3("ccc state ind value=%d,handle=%d,idx=%d", pMsg->ccc.value, pMsg->ccc.handle, pMsg->ccc.idx);

    if (pMsg->ccc.idx == THROUGHPUT_TX_CCC_IDX)
    {
        if (pMsg->ccc.value == ATT_CLIENT_CFG_NOTIFY)
        {
            // notify enabled
            throughputCb.connId = (dmConnId_t) pMsg->ccc.hdr.param;
        }
    }
    else if (pMsg->ccc.idx == THROUGHPUT_CON_UPT_CCC_IDX)
    {
        if (pMsg->ccc.value == ATT_CLIENT_CFG_NOTIFY)
        {
            // notify enabled
            throughputCb.connId = (dmConnId_t) pMsg->ccc.hdr.param;
        }
    }
}

/*************************************************************************************************/
/*!
 *  \fn     throughputSetup
 *
 *  \brief  Set up advertising and other procedures that need to be performed after
 *          device reset.
 *
 *  \param  pMsg    Pointer to message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void throughputSetup(tpMsg_t *pMsg)
{
#if defined(PHY_TEST) && (PHY_TEST == 1)
    AppExtAdvSetData(advHandle[0], APP_ADV_DATA_DISCOVERABLE, sizeof(tpAdvDataDisc), (uint8_t *) tpAdvDataDisc, HCI_EXT_ADV_DATA_LEN);
    AppExtAdvSetData(advHandle[0], APP_SCAN_DATA_DISCOVERABLE, sizeof(tpScanDataDisc), (uint8_t *) tpScanDataDisc, HCI_EXT_ADV_DATA_LEN);
    DmAdvSetPhyParam(advHandle[0], HCI_ADV_PHY_LE_1M, 0, HCI_ADV_PHY_LE_2M);

    AppExtAdvStart(1, &advHandle[0], APP_MODE_AUTO_INIT);

#else
    /* set advertising and scan response data for discoverable mode */
    AppAdvSetData(APP_ADV_DATA_DISCOVERABLE, sizeof(tpAdvDataDisc), (uint8_t *) tpAdvDataDisc);
    AppAdvSetData(APP_SCAN_DATA_DISCOVERABLE, sizeof(tpScanDataDisc), (uint8_t *) tpScanDataDisc);

    /* set advertising and scan response data for connectable mode */
    AppAdvSetData(APP_ADV_DATA_CONNECTABLE, sizeof(tpAdvDataDisc), (uint8_t *) tpAdvDataDisc);
    AppAdvSetData(APP_SCAN_DATA_CONNECTABLE, sizeof(tpScanDataDisc), (uint8_t *) tpScanDataDisc);

    /* start advertising; automatically set connectable/discoverable mode and bondable mode */
    AppAdvStart(APP_MODE_AUTO_INIT);
#endif
}

/*************************************************************************************************/
/*!
 *  \fn     throughputBtnCback
 *
 *  \brief  Button press callback.
 *
 *  \param  btn    Button press.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void throughputBtnCback(uint8_t btn)
{
    dmConnId_t      connId = AppConnIsOpen();

    /* button actions when connected */
    if (connId != DM_CONN_ID_NONE)
    {
        switch (btn)
        {
            case APP_UI_BTN_1_SHORT:

#if defined(PHY_TEST) && (PHY_TEST == 1)
                /* Toggle PHY Test Mode */
                if ((phyTest.phyMode == HCI_PHY_LE_1M_BIT) && (phyTest.sup2MPhy))
                {
                  APP_TRACE_INFO0("Request 2M TX/RX PHY");
                  DmSetPhy(connId, HCI_ALL_PHY_ALL_PREFERENCES, HCI_PHY_LE_2M_BIT, HCI_PHY_LE_2M_BIT, HCI_PHY_OPTIONS_NONE);
                }
                else if ((phyTest.phyMode == HCI_PHY_LE_2M_BIT) && (phyTest.supLECodedPhy))
                {
                  APP_TRACE_INFO0("Request LE Coded TX/RX PHY");
                  DmSetPhy(connId, HCI_ALL_PHY_ALL_PREFERENCES, HCI_PHY_LE_CODED_BIT, HCI_PHY_LE_CODED_BIT, HCI_PHY_OPTIONS_NONE);
                }
                else
                {
                  APP_TRACE_INFO0("Request 1M TX/RX PHY");
                  DmSetPhy(connId, HCI_ALL_PHY_ALL_PREFERENCES, HCI_PHY_LE_1M_BIT, HCI_PHY_LE_1M_BIT, HCI_PHY_OPTIONS_NONE);
                }
#endif
            break;

            case APP_UI_BTN_1_MED:
#if defined(PHY_TEST) && (PHY_TEST == 1)
                /* switch primary advertising PHY */
                if ( phyTest.supLECodedPhy )
                {
                    phyTest.priPhyChanl = (phyTest.priPhyChanl == HCI_ADV_PHY_LE_1M) ? HCI_ADV_PHY_LE_CODED : (HCI_ADV_PHY_LE_1M);
                    APP_TRACE_INFO1("Set Primary Adv PHY to %s", (phyTest.priPhyChanl == HCI_ADV_PHY_LE_1M) ? "1M PHY" : "LE Coded PHY");
                    DmAdvSetPhyParam(advHandle[0], phyTest.priPhyChanl, 0, phyTest.secPhyChanl);
                }
                else
                {
                    APP_TRACE_INFO0("Peer device NOT support LE Coded PHY");
                }
#endif
            break;

            case APP_UI_BTN_1_LONG:
#if defined(PHY_TEST) && (PHY_TEST == 1)
                /* switch secondary advertising PHY */
                if ((phyTest.secPhyChanl == HCI_PHY_LE_1M_BIT) && (phyTest.sup2MPhy))
                {
                    APP_TRACE_INFO0("Set Second Adv Phy to 2M");
                    phyTest.secPhyChanl = HCI_PHY_LE_2M_BIT;
                }
                else if ((phyTest.secPhyChanl == HCI_PHY_LE_2M_BIT) && (phyTest.supLECodedPhy))
                {
                    APP_TRACE_INFO0("Set Second Adv Phy to LE Coded Phy");
                    phyTest.secPhyChanl = HCI_PHY_LE_CODED_BIT;
                }
                else
                {
                    APP_TRACE_INFO0("Set Second Adv Phy to 1M");
                    phyTest.secPhyChanl = HCI_PHY_LE_1M_BIT;
                }

                DmAdvSetPhyParam(advHandle[0], phyTest.priPhyChanl, 0, phyTest.secPhyChanl);
#endif
            break;

            case APP_UI_BTN_2_SHORT:
            break;

            default:
            break;
        }
    }
}

static void throughputSendData(uint8_t connId)
{
    if ( ( transmit_mode == TRANSMIT_MODE_BIDIRECT )
         || ( transmit_mode == TRANSMIT_MODE_UPLINK ) )
    {
        if ( txIdx <= maxTransPacket )
        {
            uplinkData[0] = txIdx & 0xff;
            uplinkData[1] = (txIdx >> 8) & 0xff;
            //Check if the CCC is enabled
            if ( AttsCccEnabled(throughputCb.connId, THROUGHPUT_TX_CCC_IDX) )
            {
                AttsHandleValueNtf(connId, THROUGHPUT_TX_HDL, g_mtu-3, uplinkData);
            }

            if ( ((txIdx & 0x3ff) == 0) && (txIdx>>10 != 0) )
            {
                APP_TRACE_INFO1("Device tx %d packages", txIdx);
            }

            txIdx++;
        }
        else if ( transmit_mode == TRANSMIT_MODE_UPLINK )
        {
            txIdx = 1;
            WsfTimerStartMs(&throughputCb.sendDataTimer, TX_DATA_TIMER_MS);
        }
    }
}

void sendDataTimerExpired(uint8_t connId)
{
    throughputSendData(connId);
}

/*************************************************************************************************/
/*!
 *  \fn     throughputProcMsg
 *
 *  \brief  Process messages from the event handler.
 *
 *  \param  pMsg    Pointer to message.
 *
 *  \return None.
 */
/*************************************************************************************************/

typedef struct
{
    uint16_t interval;
    uint16_t latency;
    uint16_t sup_timeeout;
}conPara_t;
static conPara_t connectionParameters = {0};

static void throughputProcMsg(tpMsg_t *pMsg)
{
    uint8_t uiEvent = APP_UI_NONE;

    switch ( pMsg->hdr.event )
    {
        case THROUGHPUT_SEND_DATA_TIMER_IND:
            sendDataTimerExpired(throughputCb.connId);
        break;

        case ATTS_HANDLE_VALUE_CNF:
        // APP_TRACE_INFO1("NTF_CNF Handle = %d", ((attEvt_t *)pMsg)->handle);
        if (pMsg->hdr.status == ATT_SUCCESS)
        {
            if ( ((attEvt_t *)pMsg)->handle == THROUGHPUT_TX_HDL )
            {
                /*send next package afer last package has been sent done*/
                throughputSendData((dmConnId_t) pMsg->hdr.param);
            }
        }
        else
        {
            // APP_TRACE_INFO1("ATTS_HANDLE_VALUE_CNF status %d", pMsg->hdr.status);
        }
        break;

        case ATTS_CCC_STATE_IND:
            throughputProcCccState(pMsg);
        break;

        case ATT_MTU_UPDATE_IND:
            APP_TRACE_INFO1("Negotiated MTU = %d", ((attEvt_t *)pMsg)->mtu);
            g_mtu = ((attEvt_t *)pMsg)->mtu;

        break;

        case DM_RESET_CMPL_IND:
            AttsCalculateDbHash();
            DmSecGenerateEccKeyReq();

            uiEvent = APP_UI_RESET_CMPL;
        break;

        case DM_ADV_SET_START_IND:
          uiEvent = APP_UI_ADV_SET_START_IND;
          break;

        case DM_ADV_START_IND:
            uiEvent = APP_UI_ADV_START;
        break;

        case DM_ADV_STOP_IND:
            uiEvent = APP_UI_ADV_STOP;
        break;

        case DM_CONN_OPEN_IND:
        {
            APP_TRACE_INFO1("Connection open, conn_id: %d", pMsg->hdr.param);
            txIdx = 0;
            rxIdx = 0;
            preRxIdx = 0;
            transmit_mode = TRANSMIT_MODE_NONE;

            g_mtu = AttGetMtu(pMsg->hdr.param);

            uiEvent = APP_UI_CONN_OPEN;

            hciLeConnCmplEvt_t *evt = (hciLeConnCmplEvt_t*) pMsg;
            /*Save the connection control block id*/
            throughputCb.connId = pMsg->hdr.param;
            connectionParameters.interval = evt->connInterval;
            connectionParameters.latency = evt->connLatency;
            connectionParameters.sup_timeeout = evt->supTimeout;
#if defined(PHY_TEST) && (PHY_TEST == 1)
            phyTest.phyMode = HCI_PHY_LE_1M_BIT;
            phyTest.priPhyChanl = HCI_PHY_LE_1M_BIT;
            phyTest.secPhyChanl = HCI_PHY_LE_1M_BIT;
            phyTest.sup2MPhy = false;
            phyTest.supLECodedPhy = false;
#endif
        }
        break;

        case DM_REMOTE_FEATURES_IND:
        {

#if defined(PHY_TEST) && (PHY_TEST == 1)
            {
                APP_TRACE_INFO2("remote feature, 0x%x 0x%x",
                                pEvent->leReadRemoteFeatCmpl.features[0],
                                pEvent->leReadRemoteFeatCmpl.features[1]);
                if ( (hciCoreCb.leSupFeat & HCI_LE_SUP_FEAT_LE_2M_PHY )
                && ((pEvent->leReadRemoteFeatCmpl.features[1]&(HCI_LE_SUP_FEAT_LE_2M_PHY>>8))>0))
                {
                    phyTest.sup2MPhy = TRUE;
                    APP_TRACE_INFO0("both Support LE 2M");
                    DmSetPhy(pMsg->dm.connOpen.handle, HCI_ALL_PHY_ALL_PREFERENCES, HCI_PHY_LE_2M_BIT, HCI_PHY_LE_2M_BIT, HCI_PHY_OPTIONS_NONE);
                }
            }
#endif
        }
        break;

        case DM_CONN_CLOSE_IND:
        {
            hciDisconnectCmplEvt_t *evt = (hciDisconnectCmplEvt_t*) pMsg;

            throughputCb.connId = DM_CONN_ID_NONE;

            APP_TRACE_INFO1(">>> Connection closed reason 0x%x <<<", evt->reason);

#if defined(PHY_TEST) && (PHY_TEST == 1)
            AppExtAdvStart(1, &advHandle[0], APP_MODE_AUTO_INIT);
#endif
            uiEvent = APP_UI_CONN_CLOSE;

        }
        break;

        case DM_PHY_UPDATE_IND:
          APP_TRACE_INFO3("DM_PHY_UPDATE_IND status: %d, RX: %d, TX: %d", pMsg->dm.phyUpdate.status, pMsg->dm.phyUpdate.rxPhy, pMsg->dm.phyUpdate.txPhy);
#if defined(PHY_TEST) && (PHY_TEST == 1)
          phyTest.phyMode = pMsg->dm.phyUpdate.rxPhy;
#endif
          break;

        case DM_CONN_UPDATE_IND:
        {
            hciLeConnUpdateCmplEvt_t *evt = (hciLeConnUpdateCmplEvt_t*) pMsg;

            (void)evt;

            APP_TRACE_INFO1("connection update status = 0x%x", evt->status);

            if (evt->status == 0)
            {
                APP_TRACE_INFO1("handle = 0x%x", evt->handle);
                APP_TRACE_INFO1("connInterval = 0x%x", evt->connInterval);
                APP_TRACE_INFO1("connLatency = 0x%x", evt->connLatency);
                APP_TRACE_INFO1("supTimeout = 0x%x", evt->supTimeout);
                connectionParameters.interval = evt->connInterval;
                connectionParameters.latency = evt->connLatency;
                connectionParameters.sup_timeeout = evt->supTimeout;
                //Check if the CCC is enabled
                if ( AttsCccEnabled(throughputCb.connId, THROUGHPUT_CON_UPT_CCC_IDX) )
                {
                    AttsHandleValueNtf(throughputCb.connId, THROUGHPUT_CON_UPT_HDL, sizeof(connectionParameters), (uint8_t *)(&connectionParameters));
                }
            }

        }
        break;

        case DM_SEC_PAIR_CMPL_IND:
            uiEvent = APP_UI_SEC_PAIR_CMPL;
        break;

        case DM_SEC_PAIR_FAIL_IND:
            uiEvent = APP_UI_SEC_PAIR_FAIL;
        break;

        case DM_SEC_ENCRYPT_IND:
            uiEvent = APP_UI_SEC_ENCRYPT;
        break;

        case DM_SEC_ENCRYPT_FAIL_IND:
            uiEvent = APP_UI_SEC_ENCRYPT_FAIL;
        break;

        case DM_SEC_AUTH_REQ_IND:
            AppHandlePasskey(&pMsg->dm.authReq);
        break;

        case DM_SEC_ECC_KEY_IND:
            DmSecSetEccKey(&pMsg->dm.eccMsg.data.key);
            throughputSetup(pMsg);
        default:
        break;
    }

    if (uiEvent != APP_UI_NONE)
    {
        AppUiAction(uiEvent);
    }
}

uint8_t throughputReadCb(dmConnId_t connId, uint16_t handle, uint8_t operation,
                         uint16_t offset, attsAttr_t *pAttr)
{
    APP_TRACE_INFO1("throughputReadCb hande: %d\r\n", handle);
    if ( handle == THROUGHPUT_CON_UPT_HDL )
    {
        memcpy(pAttr->pValue, &connectionParameters, sizeof(connectionParameters));
    }
    return ATT_SUCCESS;
}

uint8_t throughputWriteCb(dmConnId_t connId, uint16_t handle, uint8_t operation,
                          uint16_t offset, uint16_t len, uint8_t *pValue,
                          attsAttr_t *pAttr)
{
    // APP_TRACE_INFO2("throughputWriteCb connId: %d; hande: %d\r\n", connId, handle);
    if ( handle == THROUGHPUT_CON_UPT_HDL )
    {

        hciConnSpec_t connSpec;
        // The interval parameter is already unit in 1.25ms
        BYTES_TO_UINT16(connSpec.connIntervalMin, pValue);
        connSpec.connIntervalMax = connSpec.connIntervalMin;
        BYTES_TO_UINT16(connSpec.connLatency, (pValue + 2));
        BYTES_TO_UINT16(connSpec.supTimeout, (pValue + 4)); // The supervision parameter is already unit in 10ms
        connSpec.minCeLen = 0;
        connSpec.maxCeLen = 0xffff;
        DmConnUpdate(connId, &connSpec);
        APP_TRACE_INFO3("THROUGHPUT_CON_UPT_HDL interval: %dms; latency: %d; supTimeout: %dms\r\n",
        (connSpec.connIntervalMin), connSpec.connLatency, (connSpec.supTimeout));
        return ATT_SUCCESS;
    }

    if ( (handle == THROUGHPUT_RX_HDL) && (len >= 1) )
    {
        rxIdx = (pValue[1] << 8) | pValue[0];

        // check if the Packet is an command packet to start uplink/downlink/bidrection test.
        if ( rxIdx == 1 )
        {
            // Downlink means phone writing data to device using GATT write command
            if ( pValue[3] == DOWNLINK_FLAG )
            {
                //starting trans of downlink
                txIdx = 0;
                preRxIdx = 1;
                maxTransPacket = 0;
                transmit_mode = TRANSMIT_MODE_DOWNLINK;

                APP_TRACE_INFO0("Got downlink trans command  and starting downlink trans\r\n");
                return ATT_SUCCESS;
            }
            else if ( pValue[3] == MTU_REQ_FLAG )
            {
                //send MTU to APP

                txIdx = 1;
                preRxIdx = 0;

                memset(uplinkData, MTU_REQ_FLAG, UPLINK_DATA_CNT);
                uplinkData[6] = g_mtu;
                uplinkData[7] = g_mtu;
                uplinkData[8] = g_mtu;
                uplinkData[9] = BIDIRCT_PKT_CNT_MAX&0xFF;
                uplinkData[10] = (BIDIRCT_PKT_CNT_MAX>>8)&0xFF;

                maxTransPacket = 1;

                APP_TRACE_INFO0("Got MTU cmd from APP, send Local MTU to APP");
                //Check if the CCC is enabled
                if ( AttsCccEnabled(throughputCb.connId, THROUGHPUT_TX_CCC_IDX) )
                {
                    AttsHandleValueNtf(connId, THROUGHPUT_TX_HDL, g_mtu-3, uplinkData);
                }
            }
            // Bidirection is device send 600 packets notification then phone send 600 packets data, alternatively
            else if ( (pValue[3] == BIDIRECT_FLAG) )
            {
                //starting trans of bidrection

                txIdx = 1;
                preRxIdx = 0;
                maxTransPacket = BIDIRCT_PKT_CNT_MAX;
                transmit_mode = TRANSMIT_MODE_BIDIRECT;

                memset(uplinkData, UPLINK_DATA, UPLINK_DATA_CNT);

                WsfTimerStartMs(&throughputCb.sendDataTimer, TX_DATA_TIMER_MS);
                APP_TRACE_INFO0("Got bidirection trans cmd and start bidrection trans\r\n");
            }
            // Uplink means device sends notification data to phone
            else if ( (pValue[3] == UPLINK_FLAG) )
            {
                //starting  trans of uplink

                txIdx = 1;
                preRxIdx = 0;
                maxTransPacket = TX_PACKET_CNT_MAX;
                transmit_mode = TRANSMIT_MODE_UPLINK;

                memset(uplinkData, UPLINK_DATA, UPLINK_DATA_CNT);

                WsfTimerStartMs(&throughputCb.sendDataTimer, TX_DATA_TIMER_MS);
                APP_TRACE_INFO0("Got uplink trans cmd and start Uplink trans\r\n");
            }
        }
        if (rxIdx > preRxIdx)
        {
            if (rxIdx - preRxIdx == 1)
            {
                // Device stops sending packages while device received 65535 packages.
                if (rxIdx >= TX_PACKET_CNT_MAX)
                {
                    preRxIdx = 0;
                    rxIdx = 0;
                    APP_TRACE_INFO0("Device received 65535 packages done");
                }
            }
            else
            {
                APP_TRACE_INFO2("Device received error package index, last index is %d, now is %d!", preRxIdx, rxIdx);
            }
        }
        // Not output trace for case preRxIdx==65535, rxIdx==0
        else if ( (rxIdx < preRxIdx) && (rxIdx > 0) )
        {
            APP_TRACE_INFO2("Device received error package index, last index is %d, now is %d!!!", preRxIdx, rxIdx);
        }

        /*update previous package index*/
        preRxIdx = rxIdx;
    }
    else
    {
        APP_TRACE_INFO1("Device received error len, len is %d bytes,it should be more than MTU!!!\r\n", len);
    }

    return ATT_SUCCESS;
}

/*************************************************************************************************/
/*!
 *  \fn     ThroughputHandlerInit
 *
 *  \brief  Application handler init function called during system initialization.
 *
 *  \param  handlerID  WSF handler ID.
 *
 *  \return None.
 */
/*************************************************************************************************/
void ThroughputHandlerInit(wsfHandlerId_t handlerId)
{
    APP_TRACE_INFO0("ThroughputHandlerInit");

    throughputCb.handlerId = handlerId;
    throughputCb.connId = DM_CONN_ID_NONE;

    /* Set configuration pointers */
#if defined(PHY_TEST) && (PHY_TEST == 1)
    pAppExtAdvCfg = (appExtAdvCfg_t*)&advTestCfg;
#else
    pAppAdvCfg = (appAdvCfg_t *) &tpAdvCfg;
#endif

    throughputCb.sendDataTimer.handlerId = handlerId;
    throughputCb.sendDataTimer.msg.event = THROUGHPUT_SEND_DATA_TIMER_IND;
    pAppSlaveCfg = (appSlaveCfg_t *) &tpSlaveCfg;
    pAppSecCfg = (appSecCfg_t *) &tpSecCfg;
    pAppUpdateCfg = (appUpdateCfg_t *) &tpUpdateCfg;

    /* Initialize application framework */
    AppSlaveInit();
}

/*************************************************************************************************/
/*!
 *  \fn     ThroughputHandler
 *
 *  \brief  WSF event handler for application.
 *
 *  \param  event   WSF event mask.
 *  \param  pMsg    WSF message.
 *
 *  \return None.
 */
/*************************************************************************************************/
void ThroughputHandler(wsfEventMask_t event, wsfMsgHdr_t *pMsg)
{
    if (pMsg != NULL)
    {
        if (pMsg->event >= DM_CBACK_START && pMsg->event <= DM_CBACK_END)
        {
            /* process advertising and connection-related messages */
            AppSlaveProcDmMsg((dmEvt_t *) pMsg);

            /* process security-related messages */
            AppSlaveSecProcDmMsg((dmEvt_t *) pMsg);
        }

        /* perform profile and user interface-related operations */
        throughputProcMsg((tpMsg_t *) pMsg);
    }
}

/*************************************************************************************************/
/*!
 *  \fn     ThroughputStart
 *
 *  \brief  Start the application.
 *
 *  \return None.
 */
/*************************************************************************************************/
void ThroughputStart(void)
{
    /* Register for stack callbacks */
    DmRegister(throughputDmCback);
    DmConnRegister(DM_CLIENT_ID_APP, throughputDmCback);
    AttRegister(throughputAttCback);
    AttConnRegister(AppServerConnCback);
    AttsCccRegister(THROUGHPUT_NUM_CCC_IDX, (attsCccSet_t *) tpCccSet, throughputCccCback);

    /* Register for app framework callbacks */
    AppUiBtnRegister(throughputBtnCback);

    /* Initialize attribute server database */
    SvcCoreAddGroup();
    SvcThroughputCbackRegister(throughputReadCb, throughputWriteCb);
    SvcThroughputAddGroup();

    /* Reset the device */
    DmDevReset();
}
