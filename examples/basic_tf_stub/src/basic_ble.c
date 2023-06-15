/**
 * @file basic_ble.c
 * @author Ambiq
 * @brief Simple Web BLE configuration example. This file defines everything needed
 * to instantiate a BLE service and handle BLE events.
 * @version 0.1
 * @date 2023-06-05
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "ns_ambiqsuite_harness.h"
#include "ns_ble.h"
#include "svc_myservice.h"

/***********************************/
/****** application-specific stuff */
/***********************************/

/**************************************************************************************************
  Client Characteristic Configuration Descriptors
**************************************************************************************************/

/*! enumeration of client characteristic configuration descriptors */
enum {
    MYSERVICE_GATT_SC_CCC_IDX, /*! GATT service, service changed characteristic */
    MYSERVICE_CCC_IDX,         /*! AMDTP service, tx characteristic */
    MYSERVICE_RX_ACK_CCC_IDX,  /*! AMDTP service, rx ack characteristic */
    MYSERVICE_NUM_CCC_IDX
};

/*! client characteristic configuration descriptors settings, indexed by above enumeration */
static const attsCccSet_t myServiceCccSet[MYSERVICE_NUM_CCC_IDX] = {
    {GATT_SC_CH_CCC_HDL, ATT_CLIENT_CFG_INDICATE, DM_SEC_LEVEL_NONE}, /* AMDTP_GATT_SC_CCC_IDX */
    {MYSERVICE_TX_CH_CCC_HDL, ATT_CLIENT_CFG_NOTIFY,
     DM_SEC_LEVEL_NONE}, /* AMDTP_AMDTPS_TX_CCC_IDX */
    {MYSERVICE_ACK_CH_CCC_HDL, ATT_CLIENT_CFG_NOTIFY,
     DM_SEC_LEVEL_NONE} /* AMDTP_AMDTPS_RX_ACK_CCC_IDX */
};

#define MY_SERVICE_WSF_BUFFER_POOLS 4
#define MY_SERVICE_WSF_BUFFER_SIZE MY_SERVICE_WSF_BUFFER_POOLS * 16 + 1024

static uint32_t myServiceWSFBufferPool[MY_SERVICE_WSF_BUFFER_SIZE];
static wsfBufPoolDesc_t myServiceBufferDescriptors[MY_SERVICE_WSF_BUFFER_POOLS] = {
    {16, 8}, // 16 bytes, 8 buffers
    {32, 4},
    {64, 6},
    {280, 14}};

static ns_ble_service_control_t myServiceControl;
static ns_ble_connection_t myConnectionControl;

void myServiceHandlerInit(wsfHandlerId_t handlerId);
void myServiceHandler(wsfEventMask_t event, wsfMsgHdr_t *pMsg);

// void
// myServiceDefine(void) {

//     // Declare and initialize a BLE configuration structure.
//     // We need to declare a service and its characteristics.

//     // Declare a service.
//     uint8_t myServiceValue[] = {ATT_UUID_MYSERVICE_SERVICE};
//     attsAttr_t myService = {.pUuid = attPrimSvcUuid,
//                             .pValue = myServiceValue,
//                             .maxLen = sizeof(myServiceValue),
//                             .settings = 0,
//                             .permissions = ATTS_PERMIT_READ};

//     // Declare characteristics of the service
//     uint8_t myCharValue[20] = {0};
//     uint16_t myCharUUID = 0x1234;

//     attsAttr_t serviceAttributeList[3];

//     attsAttr_t myChar = {.pUuid = &myCharUUID,
//                          .pValue = &myCharValue,
//                          .maxLen = sizeof(myCharValue),
//                          .settings =
//                              (ATTS_SET_UUID_128 | ATTS_SET_VARIABLE_LEN | ATTS_SET_WRITE_CBACK),
//                          .permissions = ATTS_PERMIT_WRITE};
// }

// void myServiceMsgProc(wsfMsgHdr_t *pMsg) {
//     switch (pMsg->event) {
// //     case AMDTP_TIMER_IND:
// //       amdtps_proc_msg(&pMsg->hdr);
// //       break;

// // #ifdef MEASURE_THROUGHPUT
// //     case AMDTP_MEAS_TP_TIMER_IND:
// //       showThroughput();
// //       break;
// // #endif

//     case ATTS_HANDLE_VALUE_CNF:
//       //APP_TRACE_INFO1("ATTS_HANDLE_VALUE_CNF, status = %d", pMsg->att.hdr.status);
//       amdtps_proc_msg(&pMsg->hdr);
//       break;

//     case ATTS_CCC_STATE_IND:
//       amdtpProcCccState(pMsg);
//       break;

//     case DM_RESET_CMPL_IND:

//     default:
//         break;
//     }
// }

static void myServiceProcCccState(ns_ble_msg_t *pMsg) {
    // uint8_t status = ATT_ERR_NO_ERROR;
    // uint16_t handle = pMsg->hdr.param;
    // uint8_t idx = 0;

    // switch (handle) {
    // case MYSERVICE_CCC_HDL:
    //     idx = MYSERVICE_CCC_IDX;
    //     break;

    // case MYSERVICE_RX_ACK_CCC_HDL:
    //     idx = MYSERVICE_RX_ACK_CCC_IDX;
    //     break;

    // default:
    //     status = ATT_ERR_ATTR_NOT_FOUND;
    //     break;
    // }

    // if (status == ATT_ERR_NO_ERROR) {
    //     status = AttsCccSet(handle, pMsg->hdr.statusFlags, myServiceCccSet[idx].value,
    //                         myServiceCccSet[idx].secLevel);
    // }

    // if (status != ATT_ERR_NO_ERROR) {
    //     AttErrorRsp(pMsg->connHandle, pMsg->hdr.event, handle, status);
    // }
}

static void myServiceHandleValueCnf(attEvt_t *pMsg) {
    // if (pMsg->hdr.status == ATT_ERR_NO_ERROR) {
    //     amdtps_proc_msg(&pMsg->hdr);
    // }
}

bool myServiceMsgProc(ns_ble_msg_t *pMsg) {
    bool messageHandled = true;

    switch (pMsg->hdr.event) {
    case DM_CONN_OPEN_IND:
        ns_ble_generic_conn_open((dmEvt_t *)pMsg);
        DmConnSetDataLen(1, 251, 0x848);
        break;

    case DM_CONN_CLOSE_IND:
        // amdtps_conn_close((dmEvt_t *) pMsg);
        break;

    case ATTS_CCC_STATE_IND:
        myServiceProcCccState(pMsg);
        break;

    case ATTS_HANDLE_VALUE_CNF:
        myServiceHandleValueCnf((attEvt_t *)pMsg);
        break;

    default:
        messageHandled = false;
        break;
    }
    return messageHandled;
}

void myServiceInit(void) {

    myServiceControl.bufferPool = myServiceWSFBufferPool;
    myServiceControl.bufferPoolSize = sizeof(myServiceWSFBufferPool);
    myServiceControl.bufferDescriptors = myServiceBufferDescriptors;
    myServiceControl.bufferDescriptorsSize = sizeof(myServiceBufferDescriptors);
    myServiceControl.wsfBufCount = MY_SERVICE_WSF_BUFFER_POOLS;
    myServiceControl.handler_init_cb = &myServiceHandlerInit;
    myServiceControl.handler_cb = &myServiceHandler;
    myServiceControl.procMsg_cb = &myServiceMsgProc;
    myServiceControl.cccSet = myServiceCccSet;
    myServiceControl.cccCount = MYSERVICE_NUM_CCC_IDX;
    // myServiceControl.ccc_cb = myServiceCccCallback;

    ns_ble_generic_init(
        TRUE, &g_ns_ble_control,
        &myServiceControl); // Use defaults to init BLE stack
    // ns_ble_per_service_init(&g_ns_ble_control, &myServiceControl); // Init service-specific stuff

    // myServiceDefine(&myServiceControl); // Adds service and characteristics to WSF structs

    // ns_ble_add_service(&myServiceControl); // Add service to BLE stack

    // ns_ble_start(); // Start BLE stack
}
