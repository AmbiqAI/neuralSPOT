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

const ns_core_api_t ns_ble_V0_0_1 = {.apiId = NS_BLE_API_ID, .version = NS_BLE_V0_0_1};

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
    3000, /*! Connection idle period in ms before attempting */
    8,    /*! 7.5ms */
    18,   /*! 15ms */
    0,    /*! Connection latency */
    600,  /*! Supervision timeout in 10ms units */
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
void ns_ble_generic_DmCback(dmEvt_t *pDmEvt) {
    dmEvt_t *pMsg;
    uint16_t len;
    ns_lp_printf("ns_ble_generic_DmCback\n");
    len = DmSizeOfEvt(pDmEvt);

    if ((pMsg = WsfMsgAlloc(len)) != NULL) {
        memcpy(pMsg, pDmEvt, len);
        ns_lp_printf(
            "ns_ble_generic_DmCback: pMsg->hdr.event = %d\n", (ns_ble_msg_t *)pMsg->hdr.event);
        WsfMsgSend(g_ns_ble_control.handlerId, pMsg);
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
void ns_ble_generic_AttCback(attEvt_t *pEvt) {
    attEvt_t *pMsg;
    ns_lp_printf("ns_ble_generic_AttCback\n");
    ATT_TRACE_INFO0("ATT_TRACE Works\n");
    if ((pMsg = WsfMsgAlloc(sizeof(attEvt_t) + pEvt->valueLen)) != NULL) {
        memcpy(pMsg, pEvt, sizeof(attEvt_t));
        pMsg->pValue = (uint8_t *)(pMsg + 1);
        memcpy(pMsg->pValue, pEvt->pValue, pEvt->valueLen);
        ns_lp_printf(
            "ns_ble_generic_AttCback: pMsg->pValue = %d\n", (ns_ble_msg_t *)pMsg->hdr.event);
        WsfMsgSend(g_ns_ble_control.handlerId, pMsg);
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
void ns_ble_generic_CccCback(attsCccEvt_t *pEvt) {
    attsCccEvt_t *pMsg;
    appDbHdl_t dbHdl;
    ns_lp_printf("ns_ble_generic_CccCback\n");
    /* if CCC not set from initialization and there's a device record */
    if ((pEvt->handle != ATT_HANDLE_NONE) &&
        ((dbHdl = AppDbGetHdl((dmConnId_t)pEvt->hdr.param)) != APP_DB_HDL_NONE)) {
        /* store value in device database */
        ns_lp_printf("ns_ble_generic_CccCback: pEvt->handle = %d\n", pEvt->handle);
        AppDbSetCccTblValue(dbHdl, pEvt->idx, pEvt->value);
    }

    if ((pMsg = WsfMsgAlloc(sizeof(attsCccEvt_t))) != NULL) {
        memcpy(pMsg, pEvt, sizeof(attsCccEvt_t));
        ns_lp_printf("ns_ble_generic_CccCback allocated ok\n");
        WsfMsgSend(g_ns_ble_control.handlerId, pMsg);
    }
}

//*****************************************************************************
//
// Connection Update event
//
//*****************************************************************************
void ns_ble_generic_conn_update(dmEvt_t *pMsg) {
    hciLeConnUpdateCmplEvt_t *evt = (hciLeConnUpdateCmplEvt_t *)pMsg;

    ns_lp_printf("connection update status = 0x%x", evt->status);

    if (evt->status == 0) {
        ns_lp_printf("handle = 0x%x", evt->handle);
        ns_lp_printf("connInterval = 0x%x", evt->connInterval);
        ns_lp_printf("connLatency = 0x%x", evt->connLatency);
        ns_lp_printf("supTimeout = 0x%x", evt->supTimeout);
    }
}

//*****************************************************************************
//
// Connection Open event
//
//*****************************************************************************
void ns_ble_generic_conn_open(dmEvt_t *pMsg) {
    hciLeConnCmplEvt_t *evt = (hciLeConnCmplEvt_t *)pMsg;

    ns_lp_printf("connection opened\n");
    ns_lp_printf("handle = 0x%x\n", evt->handle);
    ns_lp_printf("role = 0x%x\n", evt->role);
    APP_TRACE_INFO3(
        "addrMSB = %02x%02x%02x%02x%02x%02x\n", evt->peerAddr[0], evt->peerAddr[1],
        evt->peerAddr[2]);
    APP_TRACE_INFO3(
        "addrLSB = %02x%02x%02x%02x%02x%02x\n", evt->peerAddr[3], evt->peerAddr[4],
        evt->peerAddr[5]);
    ns_lp_printf("connInterval = 0x%x\n", evt->connInterval);
    ns_lp_printf("connLatency = 0x%x\n", evt->connLatency);
    ns_lp_printf("supTimeout = 0x%x\n", evt->supTimeout);
}

/*************************************************************************************************/
/*!
 *  \fn     amdtpSetup
 *
 *  \brief  Set up advertising and other procedures that need to be performed after
 *          device reset.
 *
 *  \param  pMsg    Pointer to message.
 *
 *  \return None.
 */
/*************************************************************************************************/
void ns_ble_generic_advSetup(ns_ble_msg_t *pMsg) {
    ns_ble_service_control_t *svc = g_ns_ble_control.service_config;
    ns_lp_printf("ns_ble_generic_advSetup\n");
    /* set advertising and scan response data for discoverable mode */
    AppAdvSetData(APP_ADV_DATA_DISCOVERABLE, svc->advDataLen, (uint8_t *)svc->advData);
    AppAdvSetData(APP_SCAN_DATA_DISCOVERABLE, svc->scanDataLen, (uint8_t *)svc->scanData);

    /* set advertising and scan response data for connectable mode */
    AppAdvSetData(APP_ADV_DATA_CONNECTABLE, svc->advDataLen, (uint8_t *)svc->advData);
    AppAdvSetData(APP_SCAN_DATA_CONNECTABLE, svc->scanDataLen, (uint8_t *)svc->scanData);

    /* start advertising; automatically set connectable/discoverable mode and bondable mode */
    AppAdvStart(APP_MODE_AUTO_INIT);
}

/*************************************************************************************************/
/*!
 *  \fn     amdtpProcMsg
 *
 *  \brief  Process messages from the event handler.
 *
 *  \param  pMsg    Pointer to message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void ns_ble_generic_procMsg(ns_ble_msg_t *pMsg) {
    uint8_t uiEvent = APP_UI_NONE;
    ns_ble_service_control_t *svc = g_ns_ble_control.service_config;
    ns_lp_printf("ns_ble_generic_procMsg %d\n", pMsg->hdr.event);
    // Pass it to the service message handler first, if it returns true, then it means
    // the message is handled by the service
    if (svc->procMsg_cb != NULL && svc->procMsg_cb(pMsg)) {
        return;
    }

    switch (pMsg->hdr.event) {
    case ATTS_HANDLE_VALUE_CNF:
        ns_lp_printf("ATTS_HANDLE_VALUE_CNF, status = %d", pMsg->att.hdr.status);
        break;

    case ATTS_CCC_STATE_IND:
        break;

    case ATT_MTU_UPDATE_IND:
        ns_lp_printf("Negotiated MTU %d", ((attEvt_t *)pMsg)->mtu);
        break;

    case DM_CONN_DATA_LEN_CHANGE_IND:
        APP_TRACE_INFO2(
            "DM_CONN_DATA_LEN_CHANGE_IND, Tx=%d, Rx=%d",
            ((hciLeDataLenChangeEvt_t *)pMsg)->maxTxOctets,
            ((hciLeDataLenChangeEvt_t *)pMsg)->maxRxOctets);
        break;

    case DM_RESET_CMPL_IND:
        ns_lp_printf("DM_RESET_CMPL_IND\n");
        AttsCalculateDbHash();
        DmSecGenerateEccKeyReq();
        ns_ble_generic_advSetup(pMsg);
        uiEvent = APP_UI_RESET_CMPL;
        break;

    case DM_ADV_START_IND:
        uiEvent = APP_UI_ADV_START;
        break;

    case DM_ADV_STOP_IND:
        uiEvent = APP_UI_ADV_STOP;
        break;

    case DM_CONN_OPEN_IND:
        ns_ble_generic_conn_open((dmEvt_t *)pMsg);
        uiEvent = APP_UI_CONN_OPEN;
        break;

    case DM_CONN_CLOSE_IND:
        ns_lp_printf("DM_CONN_CLOSE_IND, reason = 0x%x", pMsg->dm.connClose.reason);
        uiEvent = APP_UI_CONN_CLOSE;
        break;

    case DM_CONN_UPDATE_IND:
        ns_ble_generic_conn_update((dmEvt_t *)pMsg);
        break;

    case DM_PHY_UPDATE_IND:
        APP_TRACE_INFO3(
            "DM_PHY_UPDATE_IND status: %d, RX: %d, TX: %d", pMsg->dm.phyUpdate.status,
            pMsg->dm.phyUpdate.rxPhy, pMsg->dm.phyUpdate.txPhy);
        break;

    case DM_SEC_PAIR_CMPL_IND:
        if (g_ns_ble_control.secureConnections) {
            DmSecGenerateEccKeyReq();
        }
        uiEvent = APP_UI_SEC_PAIR_CMPL;
        break;

    case DM_SEC_PAIR_FAIL_IND:
        if (g_ns_ble_control.secureConnections) {
            DmSecGenerateEccKeyReq();
        }
        ns_lp_printf("DM_SEC_PAIR_FAIL_IND, status = 0x%x", pMsg->dm.pairCmpl.hdr.status);
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
        break;

    case DM_SEC_COMPARE_IND:
        AppHandleNumericComparison(&pMsg->dm.cnfInd);
        break;

    case DM_HW_ERROR_IND:
        uiEvent = APP_UI_HW_ERROR;
        break;

    case DM_VENDOR_SPEC_CMD_CMPL_IND:
        break;

    default:
        break;
    }

    if (uiEvent != APP_UI_NONE) {
        AppUiAction(uiEvent);
    }
}

// *** Public Functions
void ns_ble_generic_handlerInit(wsfHandlerId_t handlerId, ns_ble_service_control_t *cfg) {
    ns_lp_printf("ns_ble_generic_handlerInit\n");
    /* store handler ID */
    cfg->handlerId = handlerId;
    g_ns_ble_control.handlerId = handlerId;

    /* Set configuration pointers */
    pAppAdvCfg = g_ns_ble_control.advCfg;
    pAppSlaveCfg = g_ns_ble_control.slaveCfg;
    pAppSecCfg = g_ns_ble_control.secCfg;
    pAppUpdateCfg = g_ns_ble_control.updateCfg;

    /* Set stack configuration pointers */
    pSmpCfg = g_ns_ble_control.smpCfg;

    // Application specific initialization
    if (cfg->handler_init_cb != NULL)
        cfg->handler_init_cb(handlerId);
    // if (cfg->service_init_cb != NULL)
    //     cfg->service_init_cb(handlerId, &g_ns_ble_control, cfg);

    /* Initialize application framework */
    AppSlaveInit();
    // AppServerInit();

    //   /* initialize amdtp service server */
    //   amdtps_init(handlerId, (AmdtpsCfg_t *) &amdtpAmdtpsCfg, amdtpDtpRecvCback,
    //   amdtpDtpTransCback);

    // #ifdef MEASURE_THROUGHPUT
    //   measTpTimer.handlerId = handlerId;
    //   measTpTimer.msg.event = AMDTP_MEAS_TP_TIMER_IND;
    // #endif
}

void ns_ble_generic_handler(wsfEventMask_t event, wsfMsgHdr_t *pMsg) {
    ns_lp_printf("ns_ble_generic_handler: %d\n", event);
    if (pMsg != NULL) {
        // ns_lp_printf("Amdtp got evt %d", pMsg->event);

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
        if (g_ns_ble_control.service_config->handler_cb != NULL)
            g_ns_ble_control.service_config->handler_cb(event, pMsg);

        ns_ble_generic_procMsg((ns_ble_msg_t *)pMsg);

        // amdtpProcMsg((amdtpMsg_t *) pMsg);
    }
}

void ns_ble_pre_init(void) {
    // Set NVICs for BLE
    NVIC_SetPriority(COOPER_IOM_IRQn, 4);
    NVIC_SetPriority(AM_COOPER_IRQn, 4);
}

void ns_ble_generic_init(
    bool useDefault, ns_ble_control_t *generic_cfg, ns_ble_service_control_t *service_cfg) {
    wsfHandlerId_t handlerId;
    uint16_t wsfBufMemLen;

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
        g_ns_ble_control.updateCfg = &ns_ble_default_UpdateCfg;
        // g_ns_ble_control.connCfg = &ns_ble_connCfg_default;
        // g_ns_ble_control.appDiscCfg = &ns_ble_appDiscCfg_default;
        // g_ns_ble_control.appCfg = &ns_ble_appCfg_default;
        g_ns_ble_control.secureConnections = true;
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
    wsfBufMemLen = WsfBufInit(
        service_cfg->bufferPoolSize, (uint8_t *)service_cfg->bufferPool, service_cfg->wsfBufCount,
        service_cfg->bufferDescriptors);

    if (wsfBufMemLen > service_cfg->bufferPoolSize) {
        ns_lp_printf(
            "Memory pool is too small by %d\r\n", wsfBufMemLen - service_cfg->bufferPoolSize);
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
    AttsCccRegister(service_cfg->cccCount, service_cfg->cccSet, &ns_ble_generic_CccCback);

    SvcCoreGattCbackRegister(GattReadCback, GattWriteCback);

    // Add generic groups
    SvcCoreAddGroup();
    SvcDisAddGroup();

    // if (useDefault) {
    //     *control = &g_ns_ble_control;
    // }
}

// void
// ns_ble_radio_task(void *pvParameters) {
//     ns_ble_generic_init(true, NULL, NULL);
// }

//*****************************************************************************
//
// GPIO interrupt handler.
//
//*****************************************************************************
void am_cooper_irq_isr(void) {
    uint32_t ui32IntStatus;
    AM_CRITICAL_BEGIN
    am_hal_gpio_interrupt_irq_status_get(AM_COOPER_IRQn, false, &ui32IntStatus);
    am_hal_gpio_interrupt_irq_clear(AM_COOPER_IRQn, ui32IntStatus);
    AM_CRITICAL_END
    am_hal_gpio_interrupt_service(AM_COOPER_IRQn, ui32IntStatus);
}

//*****************************************************************************
//
// UART interrupt handler. TODO
//
//*****************************************************************************
void am_uart_isr(void) {
    uint32_t ui32Status;

    //
    // Read and save the interrupt status, but clear out the status register.
    //
    ui32Status = UARTn(0)->MIS;
    UARTn(0)->IEC = ui32Status;
}
