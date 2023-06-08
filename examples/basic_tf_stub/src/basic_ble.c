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
#include "att_api.h"
#include "wsf_trace.h"
#include "wsf_types.h"

/******** Generic Stuff *******/
typedef struct {
    const ns_core_api_t *api; ///< API prefix

    // State
    uint16_t hdlList[APP_DB_HDL_LIST_LEN]; /*! Cached handle list */
    wsfHandlerId_t handlerId;              /*! WSF hander ID */
    bool_t scanning;                       /*! TRUE if scanning */
    bool_t autoConnect;                    /*! TRUE if auto-connecting */
    uint8_t discState;                     /*! Service discovery state */
    uint8_t hdlListLen;                    /*! Cached handle list length */

    // Config
    // WSF Config Structs
    appMasterCfg_t *masterCfg; /*! Master configuration */
    appSecCfg_t *secfg;        /*! Security configuration */
    smpCfg_t *smpCfg;          /*! SMP configuration */
    hciConnSpec_t *connCfg;    /*! Connection parameters */
    appDiscCfg_t *appDiscCfg;  /*! Discovery configuration */
    appCfg_t *appCfg;          /*! Application configuration */

} ns_ble_control_t;

typedef void (*ns_ble_service_specific_handler_cb)(wsfEventMask_t event, wsfMsgHdr_t *pMsg);
typedef void (*ns_ble_service_specific_handler_init_cb)(wsfHandlerId_t handlerId);

ns_ble_control_t g_ns_ble_control;

void
ns_ble_generic_handlerInit(wsfHandlerId_t handlerId, ns_ble_service_control_t *cfg) {

    /* store handler ID */
    cfg->handlerId = handlerId;

    /* Set configuration pointers */
    pAppAdvCfg = (appAdvCfg_t *)&ns_ble_default_AdvCfg;
    pAppSlaveCfg = (appSlaveCfg_t *)&ns_ble_default_SlaveCfg;
    pAppSecCfg = (appSecCfg_t *)&ns_ble_default_SecCfg;
    pAppUpdateCfg = (appUpdateCfg_t *)&ns_ble_default_UpdateCfg;

    /* Initialize application framework */
    AppSlaveInit();
    AppServerInit();

    /* Set stack configuration pointers */
    pSmpCfg = (smpCfg_t *)&ns_ble_default_SmpCfg;

    //   /* initialize amdtp service server */
    //   amdtps_init(handlerId, (AmdtpsCfg_t *) &amdtpAmdtpsCfg, amdtpDtpRecvCback,
    //   amdtpDtpTransCback);

    // #ifdef MEASURE_THROUGHPUT
    //   measTpTimer.handlerId = handlerId;
    //   measTpTimer.msg.event = AMDTP_MEAS_TP_TIMER_IND;
    // #endif
}

void
ns_ble_generic_init(bool useDefault, ns_ble_control_t *generic_cfg,
                    ns_ble_service_control_t *service_cfg) {
    wsfHandlerId_t handlerId;
    uint16_t wsfBufMemLen;

    // Set NVICs for BLE
    NVIC_SetPriority(COOPER_IOM_IRQn, 4);
    NVIC_SetPriority(AM_COOPER_IRQn, 4);

    // Boot the radio.
    HciDrvRadioBoot(1);

    // Initialize the control block.
    if (useDefault) {
        memset(&ns_ble_control, 0, sizeof(ns_ble_control));

        // set default config, should work most of the time
        g_ns_ble_control.api = &ns_ble_V0_0_1;
        g_ns_ble_control.masterCfg = &ns_ble_masterCfg_default;
        g_ns_ble_control.secfg = &ns_ble_secfg_default;
        g_ns_ble_control.smpCfg = &ns_ble_smpCfg_default;
        g_ns_ble_control.connCfg = &ns_ble_connCfg_default;
        g_ns_ble_control.appDiscCfg = &ns_ble_appDiscCfg_default;
        g_ns_ble_control.appCfg = &ns_ble_appCfg_default;

    } else {
        // API Sanity Check

        // Copy in desired config (only config sections are valid at this point)
        if (g_ns_ble_control != generic_cb) {
            memcpy(&g_ns_ble_control, generic_cfg, sizeof(ns_ble_control_t));
        }
    }

    // Initialize the WSF OS.
    // Set up timers for the WSF scheduler.
    WsfOsInit();
    WsfTimerInit();

    // Initialize a buffer pool for WSF dynamic memory needs.
    wsfBufMemLen = WsfBufInit(sizeof(g_pui32BufMem), (uint8_t *)g_pui32BufMem, WSF_BUF_POOLS,
                              g_psPoolDescriptors);

    if (wsfBufMemLen > sizeof(g_pui32BufMem)) {
        am_util_debug_printf("Memory pool is too small by %d\r\n",
                             wsfBufMemLen - sizeof(g_pui32BufMem));
    }

    // Initialize the WSF security service.
    SecInit();
    SecAesInit();
    SecCmacInit();
    SecEccInit();

    // Set up callback functions for the various layers of the ExactLE stack.
    handlerId = WsfOsSetNextHandler(HciHandler);
    HciHandlerInit(handlerId);

    handlerId = WsfOsSetNextHandler(DmHandler);
    DmDevVsInit(0);
    DmAdvInit();
    DmPhyInit();
    DmConnInit();
    DmConnSlaveInit();
    DmSecInit();
    DmSecLescInit();
    DmPrivInit();
    DmHandlerInit(handlerId);

    handlerId = WsfOsSetNextHandler(L2cSlaveHandler);
    L2cSlaveHandlerInit(handlerId);
    L2cInit();
    L2cSlaveInit();

    handlerId = WsfOsSetNextHandler(AttHandler);
    AttHandlerInit(handlerId);
    AttsInit();
    AttsIndInit();
    AttcInit();

    handlerId = WsfOsSetNextHandler(SmpHandler);
    SmpHandlerInit(handlerId);
    SmprInit();
    SmprScInit();
    HciSetMaxRxAclLen(251);

    handlerId = WsfOsSetNextHandler(AppHandler);
    AppHandlerInit(handlerId);

    handlerId = WsfOsSetNextHandler(service_cfg->handler_cb);
    service_cfg->handler_init_cb(handlerId);

    handlerId = WsfOsSetNextHandler(HciDrvHandler);
    HciDrvHandlerInit(handlerId);

    // Register Generic BLE Callbacks
    DmRegister(ns_ble_generic_DmCback);
    DmConnRegister(DM_CLIENT_ID_APP, ns_ble_generic_DmCback);
    AttRegister(ns_ble_generic_DmCback);
    AttConnRegister(AppServerConnCback);
    AttsCccRegister(NS_BLE_NUM_CCC_IDX, (attsCccSet_t *)ns_ble_generic_DmCback,
                    ns_ble_generic_DmCback);
    SvcCoreGattCbackRegister(GattReadCback, GattWriteCback);

    // Add generic groups
    SvcCoreAddGroup();
    SvcDisAddGroup();

    // if (useDefault) {
    //     *control = &g_ns_ble_control;
    // }
}

/*! connection control block */
typedef struct {
    appDbHdl_t dbHdl; /*! Device database record handle type */
    uint8_t addrType; /*! Type of address of device to connect to */
    bdAddr_t addr;    /*! Address of device to connect to */
    bool_t doConnect; /*! TRUE to issue connect on scan complete */
} ns_ble_connection_t;

typedef struct {
    // State
    wsfHandlerId_t handlerId;        /*! WSF handler ID */
    bool_t scanning;                 /*! TRUE if scanning */
    bool_t autoConnect;              /*! TRUE if auto-connecting */
    uint8_t discState;               /*! Service discovery state */
    uint8_t hdlListLen;              /*! Cached handle list length */
    ns_ble_connection_t *connection; /*! Connection control block */

    // Service Config
    // WSF Service Metadata
    attsGroup_t *group; /*! Attribute group */
    attsAttr_t *list;   /*! Attribute list */

    // Service callbacks
    attsReadCback_t read_cb;                       /*! Read callback */
    ns_ble_service_specific_handler_cb handler_cb; /*! Called after generic event handling */
    ns_ble_service_specific_handler_init_cb handler_init_cb; /*! Service-specific init */

    // Internals
    // WSF Buffers
    uint32_t bufferBools[];
    wsfBufPoolDesc_t bufferDescriptors[];
} ns_ble_service_control_t;

/***********************************/
/****** application-specific stuff */
/***********************************/

#define MY_SERVICE_UUID 0x1234
#define MY_SERVICE_WSF_BUFFER_POOLS 4
#define MY_SERVICE_WSF_BUFFER_SIZE MY_SERVICE_WSF_BUFFER_POOLS * 16 + 1024 // TBD

static uint32_t myServiceWSFBufferPool[MY_SERVICE_WSF_BUFFER_SIZE] static wsfBufPoolDesc_t
    myServiceBufferDescriptors[MY_SERVICE_WSF_BUFFER_POOLS] = {{16, 8}, // 16 bytes, 8 buffers
                                                               {32, 4},
                                                               {64, 6},
                                                               {280, 14}};

static ns_ble_service_control_t myServiceControl;
static ns_ble_connection_t myConnectionControl;

void
myServiceHandlerInit(void);
void
myServiceHandler(attEvt_t *pEvt);
void
ns_ble_init(void);

void
myServiceDefine(void) {

    // Declare and initialize a BLE configuration structure.
    // We need to declare a service and its characteristics.

    // Declare a service.
    uint8_t myService[] = {ATT_UUID_AMVOS_SERVICE};
    attsAttr_t myService = {.pUuid = attPrimSvcUuid,
                            .pValue = myService,
                            .maxLen = sizeof(myService),
                            .settings = 0,
                            .permissions = ATTS_PERMIT_READ};

    // Declare characteristics of the service
    uint8_t myCharValue[20] = {0};
    uint16_t myCharUUID = 0x1234;

    attsAttr_t serviceAttributeList[3];

    attsAttr_t myChar = {.pUuid = &myCharUUID,
                         .pValue = &myCharValue,
                         .maxLen = sizeof(myCharValue),
                         .settings =
                             (ATTS_SET_UUID_128 | ATTS_SET_VARIABLE_LEN | ATTS_SET_WRITE_CBACK),
                         .permissions = ATTS_PERMIT_WRITE};
}

void
myServiceInit(void) {

    myServiceControl.bufferPools = myServiceWSFBufferPool;
    myServiceControl.bufferDescriptors = myServiceBufferDescriptors;
    myServiceControl.ns_ble_service_specific_handler_init_cb = myServiceHandlerInit;
    myServiceControl.ns_ble_service_specific_handler_cb = myServiceHandler;

    ns_ble_generic_init(TRUE, &g_ns_ble_control,
                        &myServiceControl);                        // Use defaults to init BLE stack
    ns_ble_per_service_init(&g_ns_ble_control, &myServiceControl); // Init service-specific stuff

    myServiceDefine(&myServiceControl); // Adds service and characteristics to WSF structs

    ns_ble_add_service(&myServiceControl); // Add service to BLE stack

    ns_ble_start(); // Start BLE stack
}
