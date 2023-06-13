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
// #include "wsf_types.h"
// #include "wsf_msg.h"
// #include "wsf_buf.h"
// #include "wsf_trace.h"
// #include "hci_api.h"
// #include "bstream.h"
// #include "dm_api.h"
// #include "att_api.h"
// #include "smp_api.h"
// #include "app_api.h"
// #include "app_db.h"
// #include "app_ui.h"
// #include "app_hw.h"
// #include "svc_ch.h"
// #include "svc_core.h"
// #include "svc_dis.h"
// #include "gatt_api.h"

// #include "hci_drv_apollo.h"
// #include "hci_drv_cooper.h"
// #include "hci_core.h"
// #include "att_api.h"
#include "ns_ble.h"
#include "svc_myservice.h"

// #include "am_devices_cooper.h"

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
    /* cccd handle                value range               security level */
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

void
myServiceHandlerInit(wsfHandlerId_t handlerId);
void
myServiceHandler(wsfEventMask_t event, wsfMsgHdr_t *pMsg);
// void
// ns_ble_init(void);

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

void
myServiceInit(void) {

    myServiceControl.bufferPool = myServiceWSFBufferPool;
    myServiceControl.bufferPoolSize = sizeof(myServiceWSFBufferPool);
    myServiceControl.bufferDescriptors = myServiceBufferDescriptors;
    myServiceControl.bufferDescriptorsSize = sizeof(myServiceBufferDescriptors);
    myServiceControl.wsfBufCount = MY_SERVICE_WSF_BUFFER_POOLS;
    myServiceControl.handler_init_cb = myServiceHandlerInit;
    myServiceControl.handler_cb = myServiceHandler;
    myServiceControl.cccSet = myServiceCccSet;
    myServiceControl.cccCount = MYSERVICE_NUM_CCC_IDX;
    // myServiceControl.ccc_cb = myServiceCccCallback;

    ns_ble_generic_init(TRUE, &g_ns_ble_control,
                        &myServiceControl); // Use defaults to init BLE stack
    // ns_ble_per_service_init(&g_ns_ble_control, &myServiceControl); // Init service-specific stuff

    // myServiceDefine(&myServiceControl); // Adds service and characteristics to WSF structs

    // ns_ble_add_service(&myServiceControl); // Add service to BLE stack

    // ns_ble_start(); // Start BLE stack
}
