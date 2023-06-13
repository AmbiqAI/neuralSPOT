/**
 * @file ns_ble.c
 * @author Ambiq Team
 * @brief Generic BLE Wrapper
 * @version 0.1
 * @date 2023-06-08
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "ns_ble.h"

// *** Globals
ns_ble_control_t g_ns_ble_control;

// *** Generic Default Configurations

/*! configurable parameters for advertising */
static appAdvCfg_t ns_ble_default_AdvCfg = {
    {60000, 0, 0}, /*! Advertising durations in ms */
    {800, 800, 0}  /*! Advertising intervals in 0.625 ms units */
};

/*! configurable parameters for slave */
static appSlaveCfg_t ns_ble_default_SlaveCfg = {
    NS_BLE_CONN_MAX, /*! Maximum connections */
};

/*! configurable parameters for security */
static appSecCfg_t ns_ble_default_SecCfg = {
    DM_AUTH_BOND_FLAG | DM_AUTH_SC_FLAG,
    /*! Authentication and bonding flags */ // TODO SECURE vs non-secure
    0,                                      /*! Initiator key distribution flags */
    DM_KEY_DIST_LTK,                        /*! Responder key distribution flags */
    FALSE,                                  /*! TRUE if Out-of-band pairing data is present */
    FALSE                                   /*! TRUE to initiate security upon connection */
};

/*! configurable parameters for connection parameter update */
static appUpdateCfg_t ns_ble_default_UpdateCfg = {
    1000, /*! Connection idle period in ms before attempting */
    6,    /*! 7.5ms */
    12,   /*! 15ms */
    0,    /*! Connection latency */
    400,  /*! Supervision timeout in 10ms units */
    5     /*! Number of update attempts before giving up */
};

/*! SMP security parameter configuration */
static smpCfg_t ns_ble_default_SmpCfg = {
    3000,                /*! 'Repeated attempts' timeout in msec */
    SMP_IO_NO_IN_NO_OUT, /*! I/O Capability */
    7,                   /*! Minimum encryption key length */
    16,                  /*! Maximum encryption key length */
    3,                   /*! Attempts to trigger 'repeated attempts' timeout */
    0,                   /*! Device authentication requirements */
};

/*************************************************************************************************/
/*!
 *  \fn     ns_ble_generic_DmCback
 *
 *  \brief  Application DM callback.
 *
 *  \param  pDmEvt  DM callback event
 *
 *  \return None.
 */
/*************************************************************************************************/
static void
ns_ble_generic_DmCback(dmEvt_t *pDmEvt) {
    dmEvt_t *pMsg;
    uint16_t len;

    len = DmSizeOfEvt(pDmEvt);

    if ((pMsg = WsfMsgAlloc(len)) != NULL) {
        memcpy(pMsg, pDmEvt, len);
        // #if configUSE_AMVOS_HID
        //     WsfMsgSend(hidAppCb.handlerId, pMsg);
        // #else
        WsfMsgSend(g_ns_ble_control.handlerId, pMsg);
        // #endif
    }
}

/*************************************************************************************************/
/*!
 *  \fn     ns_ble_generic_AttCback
 *
 *  \brief  Application ATT callback.
 *
 *  \param  pEvt    ATT callback event
 *
 *  \return None.
 */
/*************************************************************************************************/
static void
ns_ble_generic_AttCback(attEvt_t *pEvt) {
    attEvt_t *pMsg;

    if ((pMsg = WsfMsgAlloc(sizeof(attEvt_t) + pEvt->valueLen)) != NULL) {
        memcpy(pMsg, pEvt, sizeof(attEvt_t));
        pMsg->pValue = (uint8_t *)(pMsg + 1);
        memcpy(pMsg->pValue, pEvt->pValue, pEvt->valueLen);

        // #if configUSE_AMVOS_HID
        //     WsfMsgSend(hidAppCb.handlerId, pMsg);
        // #else
        WsfMsgSend(g_ns_ble_control.handlerId, pMsg);
        // #endif
    }
}

/*************************************************************************************************/
/*!
 *  \fn     ns_ble_generic_CccCback
 *
 *  \brief  Application ATTS client characteristic configuration callback.
 *
 *  \param  pDmEvt  DM callback event
 *
 *  \return None.
 */
/*************************************************************************************************/
static void
ns_ble_generic_CccCback(attsCccEvt_t *pEvt) {
    attsCccEvt_t *pMsg;
    appDbHdl_t dbHdl;

    /* if CCC not set from initialization and there's a device record */
    if ((pEvt->handle != ATT_HANDLE_NONE) &&
        ((dbHdl = AppDbGetHdl((dmConnId_t)pEvt->hdr.param)) != APP_DB_HDL_NONE)) {
        /* store value in device database */
        AppDbSetCccTblValue(dbHdl, pEvt->idx, pEvt->value);
    }

    if ((pMsg = WsfMsgAlloc(sizeof(attsCccEvt_t))) != NULL) {
        memcpy(pMsg, pEvt, sizeof(attsCccEvt_t));

        // #if configUSE_AMVOS_HID
        //     WsfMsgSend(hidAppCb.handlerId, pMsg);
        // #else
        WsfMsgSend(g_ns_ble_control.handlerId, pMsg);
        // #endif
    }
}

// *** Public Functions
void
ns_ble_generic_handlerInit(wsfHandlerId_t handlerId, ns_ble_service_control_t *cfg) {

    /* store handler ID */
    cfg->handlerId = handlerId;

    /* Set configuration pointers */
    pAppAdvCfg = g_ns_ble_control.advCfg;
    pAppSlaveCfg = g_ns_ble_control.slaveCfg;
    pAppSecCfg = g_ns_ble_control.secCfg;
    pAppUpdateCfg = g_ns_ble_control.updateCfg;

    /* Set stack configuration pointers */
    pSmpCfg = g_ns_ble_control.smpCfg;

    // Application specific initialization
    cfg->handler_init_cb(handlerId);
    cfg->service_init_cb(handlerId, &g_ns_ble_control, cfg);

    /* Initialize application framework */
    AppSlaveInit();
    AppServerInit();

    //   /* initialize amdtp service server */
    //   amdtps_init(handlerId, (AmdtpsCfg_t *) &amdtpAmdtpsCfg, amdtpDtpRecvCback,
    //   amdtpDtpTransCback);

    // #ifdef MEASURE_THROUGHPUT
    //   measTpTimer.handlerId = handlerId;
    //   measTpTimer.msg.event = AMDTP_MEAS_TP_TIMER_IND;
    // #endif
}

void
ns_ble_generic_handler(wsfEventMask_t event, wsfMsgHdr_t *pMsg) {
    if (pMsg != NULL) {
        // APP_TRACE_INFO1("Amdtp got evt %d", pMsg->event);

        /* process ATT messages */
        if (pMsg->event >= ATT_CBACK_START && pMsg->event <= ATT_CBACK_END) {
            /* process server-related ATT messages */
            AppServerProcAttMsg(pMsg);
        }
        /* process DM messages */
        else if (pMsg->event >= DM_CBACK_START && pMsg->event <= DM_CBACK_END) {
            /* process advertising and connection-related messages */
            AppSlaveProcDmMsg((dmEvt_t *)pMsg);

            /* process security-related messages */
            AppSlaveSecProcDmMsg((dmEvt_t *)pMsg);
        }

        /* perform profile and user interface-related operations */
        g_ns_ble_control.service_config->handler_cb(event, pMsg);
        // amdtpProcMsg((amdtpMsg_t *) pMsg);
    }
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
        memset(&g_ns_ble_control, 0, sizeof(g_ns_ble_control));

        // set default config, should work most of the time
        g_ns_ble_control.api = &ns_ble_V0_0_1;
        g_ns_ble_control.advCfg = &ns_ble_default_AdvCfg;
        g_ns_ble_control.slaveCfg = &ns_ble_default_SlaveCfg;
        g_ns_ble_control.secCfg = &ns_ble_default_SecCfg;
        g_ns_ble_control.smpCfg = &ns_ble_default_SmpCfg;
        // g_ns_ble_control.connCfg = &ns_ble_connCfg_default;
        // g_ns_ble_control.appDiscCfg = &ns_ble_appDiscCfg_default;
        // g_ns_ble_control.appCfg = &ns_ble_appCfg_default;

    } else {
        // API Sanity Check

        // Copy in desired config (only config sections are valid at this point)
        if (&g_ns_ble_control != generic_cfg) {
            memcpy(&g_ns_ble_control, generic_cfg, sizeof(ns_ble_control_t));
        }
    }

    g_ns_ble_control.service_config = service_cfg;

    // Initialize the WSF OS.
    // Set up timers for the WSF scheduler.
    WsfOsInit();
    WsfTimerInit();

    // Initialize a buffer pool for WSF dynamic memory needs.
    wsfBufMemLen = WsfBufInit(service_cfg->bufferPoolSize, (uint8_t *)service_cfg->bufferPool,
                              service_cfg->wsfBufCount, service_cfg->bufferDescriptors);

    if (wsfBufMemLen > service_cfg->bufferPoolSize) {
        am_util_debug_printf("Memory pool is too small by %d\r\n",
                             wsfBufMemLen - service_cfg->bufferPoolSize);
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

    handlerId = WsfOsSetNextHandler(ns_ble_generic_handler);
    ns_ble_generic_handlerInit(handlerId, service_cfg);

    handlerId = WsfOsSetNextHandler(HciDrvHandler);
    HciDrvHandlerInit(handlerId);

    // Register Generic BLE Callbacks
    DmRegister(&ns_ble_generic_DmCback);
    DmConnRegister(DM_CLIENT_ID_APP, &ns_ble_generic_DmCback);
    AttRegister(&ns_ble_generic_AttCback);
    AttConnRegister(AppServerConnCback);
    AttsCccRegister(service_cfg->cccCount, (attsCccSet_t *)service_cfg->cccSet,
                    &ns_ble_generic_CccCback);
    SvcCoreGattCbackRegister(GattReadCback, GattWriteCback);

    // Add generic groups
    SvcCoreAddGroup();
    SvcDisAddGroup();

    // if (useDefault) {
    //     *control = &g_ns_ble_control;
    // }
}
