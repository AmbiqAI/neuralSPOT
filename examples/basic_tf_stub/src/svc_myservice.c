

//*****************************************************************************
//
// ${copyright}
//
// This is part of revision ${version} of the AmbiqSuite Development Package.
//
//*****************************************************************************
#include "svc_myservice.h"
#include "att_api.h"
#include "bstream.h"
#include "svc_cfg.h"
#include "svc_ch.h"
#include "wsf_trace.h"
#include "wsf_types.h"

/**************************************************************************************************
 Static Variables
**************************************************************************************************/
/* UUIDs */
static const uint8_t svcRxUuid[] = {ATT_UUID_MYSERVICE_RX};
static const uint8_t svcTxUuid[] = {ATT_UUID_MYSERVICE_TX};
static const uint8_t svcAckUuid[] = {ATT_UUID_MYSERVICE_ACK};

/**************************************************************************************************
 Service variables
**************************************************************************************************/

/* AMDTP service declaration */
static const uint8_t myserviceSvc[] = {ATT_UUID_MYSERVICE_SERVICE};
static const uint16_t myserviceLenSvc = sizeof(myserviceSvc);

/* AMDTP RX characteristic */
static const uint8_t myserviceRxCh[] = {ATT_PROP_WRITE_NO_RSP, UINT16_TO_BYTES(MYSERVICE_RX_HDL),
                                        ATT_UUID_MYSERVICE_RX};
static const uint16_t myserviceLenRxCh = sizeof(myserviceRxCh);

/* AMDTP TX characteristic */
static const uint8_t myserviceTxCh[] = {ATT_PROP_NOTIFY, UINT16_TO_BYTES(MYSERVICE_TX_HDL),
                                        ATT_UUID_MYSERVICE_TX};
static const uint16_t myserviceLenTxCh = sizeof(myserviceTxCh);

/* AMDTP RX ack characteristic */
static const uint8_t myserviceAckCh[] = {(ATT_PROP_WRITE_NO_RSP | ATT_PROP_NOTIFY),
                                         UINT16_TO_BYTES(MYSERVICE_ACK_HDL),
                                         ATT_UUID_MYSERVICE_ACK};
static const uint16_t myserviceLenAckCh = sizeof(myserviceAckCh);

/* AMDTP RX data */
/* Note these are dummy values */
static const uint8_t myserviceRx[] = {0};
static const uint16_t myserviceLenRx = sizeof(myserviceRx);

/* AMDTP TX data */
/* Note these are dummy values */
static const uint8_t myserviceTx[] = {0};
static const uint16_t myserviceLenTx = sizeof(myserviceTx);

/* Proprietary data client characteristic configuration */
static uint8_t myserviceTxChCcc[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t myserviceLenTxChCcc = sizeof(myserviceTxChCcc);

/* AMDTP RX ack data */
/* Note these are dummy values */
static const uint8_t myserviceAck[] = {0};
static const uint16_t myserviceLenAck = sizeof(myserviceAck);

/* Proprietary data client characteristic configuration */
static uint8_t myserviceAckChCcc[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t myserviceLenAckChCcc = sizeof(myserviceAckChCcc);

/* Attribute list for AMDTP group */
static const attsAttr_t myserviceList[] = {
    {attPrimSvcUuid, (uint8_t *)myserviceSvc, (uint16_t *)&myserviceLenSvc, sizeof(myserviceSvc), 0,
     ATTS_PERMIT_READ},
    {attChUuid, (uint8_t *)myserviceRxCh, (uint16_t *)&myserviceLenRxCh, sizeof(myserviceRxCh), 0,
     ATTS_PERMIT_READ},
    {svcRxUuid, (uint8_t *)myserviceRx, (uint16_t *)&myserviceLenRx, ATT_VALUE_MAX_LEN,
     (ATTS_SET_UUID_128 | ATTS_SET_VARIABLE_LEN | ATTS_SET_WRITE_CBACK), ATTS_PERMIT_WRITE},
    {attChUuid, (uint8_t *)myserviceTxCh, (uint16_t *)&myserviceLenTxCh, sizeof(myserviceTxCh), 0,
     ATTS_PERMIT_READ},
    {
        svcTxUuid, (uint8_t *)myserviceTx, (uint16_t *)&myserviceLenTx,
        sizeof(myserviceTx), // ATT_VALUE_MAX_LEN,
        0,                   //(ATTS_SET_UUID_128 | ATTS_SET_VARIABLE_LEN),
        0,                   // ATTS_PERMIT_READ
    },
    {attCliChCfgUuid, (uint8_t *)myserviceTxChCcc, (uint16_t *)&myserviceLenTxChCcc,
     sizeof(myserviceTxChCcc), ATTS_SET_CCC, (ATTS_PERMIT_READ | ATTS_PERMIT_WRITE)},
    {attChUuid, (uint8_t *)myserviceAckCh, (uint16_t *)&myserviceLenAckCh, sizeof(myserviceAckCh),
     0, ATTS_PERMIT_READ},
    {
        svcAckUuid,
        (uint8_t *)myserviceAck,
        (uint16_t *)&myserviceLenAck,
        ATT_VALUE_MAX_LEN,
        (ATTS_SET_UUID_128 | ATTS_SET_VARIABLE_LEN | ATTS_SET_WRITE_CBACK),
        ATTS_PERMIT_WRITE,
    },
    {attCliChCfgUuid, (uint8_t *)myserviceAckChCcc, (uint16_t *)&myserviceLenAckChCcc,
     sizeof(myserviceAckChCcc), ATTS_SET_CCC, (ATTS_PERMIT_READ | ATTS_PERMIT_WRITE)}};

/* AMDTP group structure */
static attsGroup_t svcMyServiceGroup = {NULL, (attsAttr_t *)myserviceList, NULL,
                                        NULL, MYSERVICE_START_HDL,         MYSERVICE_END_HDL};

/*************************************************************************************************/
/*!
 *  \fn     SvcAmdtpsAddGroup
 *
 *  \brief  Add the services to the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void
SvcMyServiceAddGroup(void) {
    AttsAddGroup(&svcMyServiceGroup);
}

/*************************************************************************************************/
/*!
 *  \fn     SvcAmdtpRemoveGroup
 *
 *  \brief  Remove the services from the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void
SvcMyServiceRemoveGroup(void) {
    AttsRemoveGroup(MYSERVICE_START_HDL);
}

/*************************************************************************************************/
/*!
 *  \fn     SvcAmdtpsCbackRegister
 *
 *  \brief  Register callbacks for the service.
 *
 *  \param  readCback   Read callback function.
 *  \param  writeCback  Write callback function.
 *
 *  \return None.
 */
/*************************************************************************************************/
void
SvcMyServiceCbackRegister(attsReadCback_t readCback, attsWriteCback_t writeCback) {
    svcMyServiceGroup.readCback = readCback;
    svcMyServiceGroup.writeCback = writeCback;
}
