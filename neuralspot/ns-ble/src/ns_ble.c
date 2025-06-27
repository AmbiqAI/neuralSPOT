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
    {96, 96, 0}    /*! Advertising intervals in 0.625 ms units */
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
    3100, /*! Connection idle period in ms before attempting */
    // 3000, /*! Connection idle period in ms before attempting */
    8,   /*! 7.5ms */
    18,  /*! 15ms */
    0,   /*! Connection latency */
    600, /*! Supervision timeout in 10ms units */
    5    /*! Number of update attempts before giving up */
};

/*! SMP security parameter configuration */
static smpCfg_t ns_ble_default_SmpCfg = {
    // 3000,                /*! 'Repeated attempts' timeout in msec */
    3200,                /*! 'Repeated attempts' timeout in msec */
    SMP_IO_NO_IN_NO_OUT, /*! I/O Capability */
    7,                   /*! Minimum encryption key length */
    16,                  /*! Maximum encryption key length */
    3,                   /*! Attempts to trigger 'repeated attempts' timeout */
    0,                   /*! Device authentication requirements */
};

/*! advertising data, discoverable mode */
static const uint8_t ns_ble_generic_data_disc[] = {
    /*! flags */
    2,                        /*! length */
    DM_ADV_TYPE_FLAGS,        /*! AD type */
    DM_FLAG_LE_GENERAL_DISC | /*! flags */
        DM_FLAG_LE_BREDR_NOT_SUP,

    /*! tx power */
    2,                    /*! length */
    DM_ADV_TYPE_TX_POWER, /*! AD type */
    0,                    /*! tx power */

    /*! service UUID list */
    3,                   /*! length */
    DM_ADV_TYPE_16_UUID, /*! AD type */
    UINT16_TO_BYTES(ATT_UUID_DEVICE_INFO_SERVICE),

    17,                   /*! length */
    DM_ADV_TYPE_128_UUID, /*! AD type */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

/*! scan data, discoverable mode */
static const uint8_t ns_ble_generic_scan_data_disc[] = {
    /*! device name */
    32,                     /*! length */
    DM_ADV_TYPE_LOCAL_NAME, /*! AD type */
    'n',
    's',
    '_',
    'g',
    'e',
    'n',
    'e',
    'r', // 31 bytes
    'i',
    'c',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
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
static void ns_ble_generic_DmCback(dmEvt_t *pDmEvt) {
    dmEvt_t *pMsg;
    uint16_t len;
    // ns_lp_printf("ns_ble_generic_DmCback\n");
    len = DmSizeOfEvt(pDmEvt);

    if ((pMsg = WsfMsgAlloc(len)) != NULL) {
        memcpy(pMsg, pDmEvt, len);
        // ns_lp_printf(
        // "ns_ble_generic_DmCback: pMsg->hdr.event = %d\n", (ns_ble_msg_t *)pMsg->hdr.event);
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
static void ns_ble_generic_AttCback(attEvt_t *pEvt) {
    attEvt_t *pMsg;
    if ((pMsg = WsfMsgAlloc(sizeof(attEvt_t) + pEvt->valueLen)) != NULL) {
        memcpy(pMsg, pEvt, sizeof(attEvt_t));
        pMsg->pValue = (uint8_t *)(pMsg + 1);
        memcpy(pMsg->pValue, pEvt->pValue, pEvt->valueLen);
        // ns_lp_printf(
        // "ns_ble_generic_AttCback: pMsg->pValue = %d\n", (ns_ble_msg_t *)pMsg->hdr.event);
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
static void ns_ble_generic_CccCback(attsCccEvt_t *pEvt) {
    attsCccEvt_t *pMsg;
    appDbHdl_t dbHdl;
    // ns_lp_printf("ns_ble_generic_CccCback\n");
    /* if CCC not set from initialization and there's a device record */
    if ((pEvt->handle != ATT_HANDLE_NONE) &&
        ((dbHdl = AppDbGetHdl((dmConnId_t)pEvt->hdr.param)) != APP_DB_HDL_NONE)) {
        /* store value in device database */
        // ns_lp_printf("ns_ble_generic_CccCback: pEvt->handle = %d\n", pEvt->handle);
        AppDbSetCccTblValue(dbHdl, pEvt->idx, pEvt->value);
    }

    if ((pMsg = WsfMsgAlloc(sizeof(attsCccEvt_t))) != NULL) {
        memcpy(pMsg, pEvt, sizeof(attsCccEvt_t));
        WsfMsgSend(g_ns_ble_control.handlerId, pMsg);
    } else {
        ns_lp_printf("ns_ble_generic_CccCback allocated failed\n");
    }
}

static void ns_ble_generic_conn_update(dmEvt_t *pMsg) {
    // hciLeConnUpdateCmplEvt_t *evt = (hciLeConnUpdateCmplEvt_t *)pMsg;

    // ns_lp_printf("connection update status = 0x%x\n", evt->status);

    // if (evt->status == 0) {
    //     ns_lp_printf("handle = 0x%x", evt->handle);
    //     ns_lp_printf("connInterval = 0x%x", evt->connInterval);
    //     ns_lp_printf("connLatency = 0x%x", evt->connLatency);
    //     ns_lp_printf("supTimeout = 0x%x", evt->supTimeout);
    // }
}

static void ns_ble_generic_conn_open(dmEvt_t *pMsg) {
    // hciLeConnCmplEvt_t *evt = (hciLeConnCmplEvt_t *)pMsg;

    // ns_lp_printf("connection opened\n");
    // ns_lp_printf("handle = 0x%x\n", evt->handle);
    // ns_lp_printf("role = 0x%x\n", evt->role);
    // APP_TRACE_INFO3(
    //     "addrMSB = %02x%02x%02x%02x%02x%02x\n", evt->peerAddr[0], evt->peerAddr[1],
    //     evt->peerAddr[2]);
    // APP_TRACE_INFO3(
    //     "addrLSB = %02x%02x%02x%02x%02x%02x\n", evt->peerAddr[3], evt->peerAddr[4],
    //     evt->peerAddr[5]);
    // ns_lp_printf("connInterval = 0x%x\n", evt->connInterval);
    // ns_lp_printf("connLatency = 0x%x\n", evt->connLatency);
    // ns_lp_printf("supTimeout = 0x%x\n", evt->supTimeout);
}

static void ns_ble_generic_advSetup(ns_ble_msg_t *pMsg) {
    ns_ble_service_control_t *svc = g_ns_ble_control.service_config;
    // ns_lp_printf("ns_ble_generic_advSetup\n");
    /* set advertising and scan response data for discoverable mode */
    AppAdvSetData(APP_ADV_DATA_DISCOVERABLE, svc->advDataLen, (uint8_t *)svc->advData);
    AppAdvSetData(APP_SCAN_DATA_DISCOVERABLE, svc->scanDataLen, (uint8_t *)svc->scanData);

    /* set advertising and scan response data for connectable mode */
    AppAdvSetData(APP_ADV_DATA_CONNECTABLE, svc->advDataLen, (uint8_t *)svc->advData);
    AppAdvSetData(APP_SCAN_DATA_CONNECTABLE, svc->scanDataLen, (uint8_t *)svc->scanData);

    /* start advertising; automatically set connectable/discoverable mode and bondable mode */
    AppAdvStart(APP_MODE_AUTO_INIT);
}

static void ns_ble_generic_procMsg(ns_ble_msg_t *pMsg) {
    uint8_t uiEvent = APP_UI_NONE;
    ns_ble_service_control_t *svc = g_ns_ble_control.service_config;
    // ns_lp_printf("ns_ble_generic_procMsg %d\n", pMsg->hdr.event);
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
        ns_lp_printf("ATTS_CCC_STATE_IND\n");
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
        ns_lp_printf("DM_HW_ERROR_IND\n");
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

void ns_ble_generic_handlerInit(wsfHandlerId_t handlerId, ns_ble_service_control_t *cfg) {
    // ns_lp_printf("ns_ble_generic_handlerInit\n");
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
}

void ns_ble_generic_handler(wsfEventMask_t event, wsfMsgHdr_t *pMsg) {
    // ns_lp_printf("ns_ble_generic_handler: %d\n", event);
    if (pMsg != NULL) {
        // ns_lp_printf("Amdtp got evt %d\n", pMsg->event);

        /* process ATT messages */
        if (pMsg->event >= ATT_CBACK_START && pMsg->event <= ATT_CBACK_END) {
            /* process server-related ATT messages */
            AppServerProcAttMsg(pMsg);
        }
        /* process DM messages */
        else if (pMsg->event >= DM_CBACK_START && pMsg->event <= DM_CBACK_END) {
            /* process advertising and connection-related messages */
            // ns_lp_printf("Amdtp got evt %d\n", pMsg->event);

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
    g_ns_ble_control.nextHandleId = 0;

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
#if defined(AM_PART_APOLLO3P) || defined(AM_PART_APOLLO3)
    HciVscSetRfPowerLevelEx(TX_POWER_LEVEL_MINUS_10P0_dBm);
#else
    HciVscSetRfPowerLevelEx(TX_POWER_LEVEL_MINUS_20P0_dBm);
#endif

    // if (useDefault) {
    //     *control = &g_ns_ble_control;
    // }
}

#if defined(AM_PART_APOLLO3P) || defined(AM_PART_APOLLO3)
void am_ble_isr(void) { HciDrvIntService(); }
#else
void am_cooper_irq_isr(void) {
    uint32_t ui32IntStatus;
    AM_CRITICAL_BEGIN
    am_hal_gpio_interrupt_irq_status_get(AM_COOPER_IRQn, false, &ui32IntStatus);
    am_hal_gpio_interrupt_irq_clear(AM_COOPER_IRQn, ui32IntStatus);
    AM_CRITICAL_END
    am_hal_gpio_interrupt_service(AM_COOPER_IRQn, ui32IntStatus);
}
#endif
void am_uart_isr(void) {
    uint32_t ui32Status;

    // Read and save the interrupt status, but clear out the status register.
    ui32Status = UARTn(0)->MIS;
    UARTn(0)->IEC = ui32Status;
}

//*****************************************************************************
// ns-ble Simplified BLE API
//*****************************************************************************

void ns_ble_pre_init(void) {
// Set NVICs for BLE
#if defined(AM_PART_APOLLO3P) || defined(AM_PART_APOLLO3)
    NVIC_SetPriority(BLE_IRQn, NVIC_configMAX_SYSCALL_INTERRUPT_PRIORITY);
#else
    NVIC_SetPriority(COOPER_IOM_IRQn, 4);
    NVIC_SetPriority(AM_COOPER_IRQn, 4);
#endif
}

void ns_ble_new_handler(wsfEventMask_t event, wsfMsgHdr_t *pMsg) {
    // ns_lp_printf("ns_ble_new_handler\n");
}

void ns_ble_new_handler_init(wsfHandlerId_t handlerId) {
    // ns_lp_printf("ns_ble_new_handler_init\n");
}

static void ns_ble_generic_new_handle_cnf(attEvt_t *pMsg){};

void ns_ble_send_value(ns_ble_characteristic_t *c, attEvt_t *pMsg) {
    dmConnId_t connId = 1;
    // ns_lp_printf("ns_ble_send_value");
    if (AttsCccEnabled(connId, c->indicationTimer.msg.status)) {
        int ret = AttsSetAttr(c->valueHandle, c->valueLen, c->applicationValue);
        if (ret != ATT_SUCCESS) {
            ns_lp_printf("... failed to send\n");
        }
        ns_interrupt_master_disable(); // critical region
        AttsHandleValueNtf(connId, c->valueHandle, c->valueLen, c->applicationValue);
        ns_interrupt_master_enable();
    } else {
        // ns_lp_printf("... not sent\n");
    }
}

static bool ns_ble_handle_indication_timer_expired(ns_ble_msg_t *pMsg) {
    uint8_t event = pMsg->hdr.event;
    ns_ble_service_t *service;
    // ns_lp_printf("ns_ble_handle_indication_timer_expired\n");
    for (int i = 0; i < g_ns_ble_control.numServices; i++) {
        service = g_ns_ble_control.services[i];
        for (int j = 0; j < service->numCharacteristics; j++) {
            if (service->characteristics[j]->cccIndicationHandle == event) {
                ns_ble_characteristic_t *c = service->characteristics[j];
                // Found a match, handle timer expiry
                // Call the callback to update the value of attribute
                c->notifyHandlerCb(service, c);

                // Send the value if not asynchronous
                if (c->indicationIsAsynchronous == false) {
                    ns_ble_send_value(c, (attEvt_t *)pMsg);
                }

                // Restart timer
                WsfTimerStartMs(&c->indicationTimer, c->indicationPeriod);
                return true;
            }
        }
    }
    return false;
}

static void ns_ble_process_ccc_state(attsCccEvt_t *pMsg) {
    uint8_t idx = pMsg->idx;
    ns_ble_service_t *service;
    // ns_lp_printf("ns_ble_process_ccc_state\n");
    for (int i = 0; i < g_ns_ble_control.numServices; i++) {
        service = g_ns_ble_control.services[i];
        for (int j = 0; j < service->numCharacteristics; j++) {
            ns_ble_characteristic_t *c = service->characteristics[j];
            // ns_lp_printf("webbleProcessCccState: %d %d %d\n", j, idx, c->cccIndex);
            if (service->characteristics[j]->cccIndex == idx) {
                if (pMsg->value == ATT_CLIENT_CFG_NOTIFY) {
                    // Start the timer
                    ns_lp_printf("webbleStartTimer\n");
                    WsfTimerStartMs(&c->indicationTimer, c->indicationPeriod);
                } else {
                    // Stop the timer
                    ns_lp_printf("webbleStopTimer\n");
                    WsfTimerStop(&c->indicationTimer);
                }
            }
        }
    }
}

bool ns_ble_new_proc_msg(ns_ble_msg_t *pMsg) {
    bool messageHandled = true;
    // ns_lp_printf("ns_ble_new_proc_msg\n");
    // ns_lp_printf("ns_ble_new_proc_msg: %d\n", pMsg->hdr.event);
    switch (pMsg->hdr.event) {
    case DM_CONN_OPEN_IND:
        ns_ble_generic_conn_open((dmEvt_t *)pMsg);
        DmConnSetDataLen(1, 251, 0x848);
        break;

    case DM_CONN_CLOSE_IND:
        // amdtps_conn_close((dmEvt_t *) pMsg);
        break;

    case ATTS_CCC_STATE_IND:
        ns_ble_process_ccc_state((attsCccEvt_t *)pMsg);
        break;

    case ATTS_HANDLE_VALUE_CNF:
        ns_ble_generic_new_handle_cnf((attEvt_t *)pMsg);
        break;
    default:
        // Check to see if even matches an indication handle, if so, call the
        // handler.
        messageHandled = ns_ble_handle_indication_timer_expired(pMsg);
        break;
    }

    return messageHandled;
}

int ns_ble_char2uuid(char const uuidString[16], ns_ble_uuid128_t *uuid128) {
    // Convert the string into uint array needed by WSF
    // Written by CoPilot!

    // ns_lp_printf("ns_ble_char2uuid: %s\n", uuidString);
    for (int i = 0; i < 16; i++) {
        char c1 = uuidString[i * 2];
        char c2 = uuidString[i * 2 + 1];
        if (c1 >= '0' && c1 <= '9') {
            uuid128->array[15 - i] = (c1 - '0') << 4;
        } else if (c1 >= 'a' && c1 <= 'f') {
            uuid128->array[15 - i] = (c1 - 'a' + 10) << 4;
        } else if (c1 >= 'A' && c1 <= 'F') {
            uuid128->array[15 - i] = (c1 - 'A' + 10) << 4;
        } else {
            return -1;
        }

        if (c2 >= '0' && c2 <= '9') {
            uuid128->array[15 - i] |= (c2 - '0');
        } else if (c2 >= 'a' && c2 <= 'f') {
            uuid128->array[15 - i] |= (c2 - 'a' + 10);
        } else if (c2 >= 'A' && c2 <= 'F') {
            uuid128->array[15 - i] |= (c2 - 'A' + 10);
        } else {
            return -1;
        }
    }

    return NS_STATUS_SUCCESS;
}

uint16_t ns_ble_get_next_handle_id(ns_ble_service_t *service) {
    return g_ns_ble_control.nextHandleId++;
}

uint8_t ns_ble_generic_write_cback(
    dmConnId_t connId, uint16_t handle, uint8_t operation, uint16_t offset, uint16_t len,
    uint8_t *pValue, attsAttr_t *pAttr) {
    // ns_lp_printf("ns_ble_generic_write_cback, handle %d\n", handle);
    for (int i = 0; i < g_ns_ble_control.numServices; i++) {
        ns_ble_service_t *service = g_ns_ble_control.services[i];
        for (int j = 0; j < service->numCharacteristics; j++) {
            if (service->characteristics[j]->valueHandle == handle) {
                if (service->characteristics[j]->writeHandlerCb) {
                    return service->characteristics[j]->writeHandlerCb(
                        service, service->characteristics[j], pValue);
                }
            }
        }
    }

    return ATT_ERR_HANDLE;
}

uint8_t ns_ble_generic_read_cback(
    dmConnId_t connId, uint16_t handle, uint8_t operation, uint16_t offset, attsAttr_t *pAttr) {
    // ns_lp_printf("ns_ble_generic_read_cback, handle %d\n", handle);
    for (int i = 0; i < g_ns_ble_control.numServices; i++) {
        ns_ble_service_t *service = g_ns_ble_control.services[i];
        for (int j = 0; j < service->numCharacteristics; j++) {
            if (service->characteristics[j]->valueHandle == handle) {
                if (service->characteristics[j]->readHandlerCb) {
                    return service->characteristics[j]->readHandlerCb(
                        service, service->characteristics[j], pAttr->pValue);
                }
            }
        }
    }
    ns_lp_printf("ns_ble_generic_read_cback, handle %d, not found\n", handle);
    return ATT_ERR_HANDLE;
}

// Create a Service
int ns_ble_create_service(ns_ble_service_t *service) {
    service->nextHandleId = service->baseHandle;
    service->handleId = ns_ble_get_next_handle_id(service);
    service->attributes = NULL;
    service->readCback = ns_ble_generic_read_cback;
    service->writeCback = ns_ble_generic_write_cback;

    // *** Fill in service attributes
    // Primary Service Declaration
    service->primaryAttributeLen = sizeof(service->uuid128.array); // need a pointer for some reason
    service->primaryAttribute.pUuid = attPrimSvcUuid;
    service->primaryAttribute.pValue = service->uuid128.array;
    service->primaryAttribute.pLen = &(service->primaryAttributeLen);
    service->primaryAttribute.maxLen = sizeof(service->uuid128.array);
    service->primaryAttribute.settings = 0;
    service->primaryAttribute.permissions = ATTS_PERMIT_READ;

    uint16_t incomingNumAttributes = service->numAttributes;
    service->numAttributes++;
    // Create Attribute List and add primary service declaration
    service->attributes = ns_malloc(sizeof(attsAttr_t) * service->numAttributes);
    if (service->attributes == NULL) {
        return NS_STATUS_FAILURE;
    }
    // *** Service Attributes
    // Add primary service declaration
    memcpy(&(service->attributes[0]), &(service->primaryAttribute), sizeof(attsAttr_t));
    service->nextAttributeIndex = 1;
    service->nextHandleId++;

    // Allocate memory for characteristic array
    service->characteristics =
        ns_malloc(sizeof(ns_ble_characteristic_t *) * service->numCharacteristics);
    service->nextCharacteristicIndex = 0;

    // *** Discovery and Advertisement structs
    // Copy over the generic values, then overwrite with service-specific values

    // Advertisement Data
    service->advData = ns_malloc(sizeof(ns_ble_generic_data_disc));
    if (service->advData == NULL) {
        return NS_STATUS_FAILURE;
    }
    service->advDataLen = sizeof(ns_ble_generic_data_disc);
    memcpy(service->advData, &ns_ble_generic_data_disc, sizeof(ns_ble_generic_data_disc));
    memcpy(
        service->advData + 12, &service->uuid128,
        sizeof(service->uuid128)); // uuid128 is at offset 12

    // Scan Data
    service->scanData = ns_malloc(sizeof(ns_ble_generic_scan_data_disc));
    if (service->scanData == NULL) {
        return NS_STATUS_FAILURE;
    }

    // service->scanDataLen = sizeof(ns_ble_generic_scan_data_disc);
    service->scanDataLen = service->nameLen + 2;
    memcpy(service->scanData, &ns_ble_generic_scan_data_disc, service->scanDataLen);
    memcpy(service->scanData + 2, service->name, service->nameLen); // name is at offset 2
    service->scanData[0] = service->nameLen + 1;

    // *** CCC Configuration
    // Allocate memory for CCCSet array
    // Fill in as Notify attributes are added

    // The number of CCC attributes can be calculated from numCharacteristics and numAttributes
    // There are always at least 2 attributes (declaration and value). Characteristics
    // with CCC will have a 3rd attribute (CCC).
    uint16_t numCccAttributes = incomingNumAttributes - service->numCharacteristics * 2;
    service->cccSet =
        ns_malloc(sizeof(attsCccSet_t) * (numCccAttributes + 1)); // add one for GAT_SC
    if (service->cccSet == NULL) {
        return NS_STATUS_FAILURE;
    }
    service->cccSet[0].handle = GATT_SC_CH_CCC_HDL;
    service->cccSet[0].valueRange = ATT_CLIENT_CFG_INDICATE;
    service->cccSet[0].secLevel = DM_SEC_LEVEL_NONE;
    service->nextCccIndex = 1;
    service->nextCccIndicationHandle = 0xC0; // TODO pick a better way to start this

    // *** TODO name and version attributes

    // *** Create and populate service control block
    service->control = ns_malloc(sizeof(ns_ble_service_control_t));
    if (service->control == NULL) {
        return NS_STATUS_FAILURE;
    }

    // *** Fill in service control block
    // TODO: refactor service and control block structs
    service->control->bufferPool = service->poolConfig->pool;
    service->control->bufferPoolSize = service->poolConfig->poolSize;
    service->control->bufferDescriptors = service->poolConfig->desc;
    service->control->wsfBufCount = service->poolConfig->descNum;
    service->control->advData = service->advData;
    service->control->advDataLen = service->advDataLen;
    service->control->scanData = service->scanData;
    service->control->scanDataLen = service->scanDataLen;
    service->control->cccSet = service->cccSet;
    service->control->cccCount = numCccAttributes + 1;
    service->control->handler_init_cb = &ns_ble_new_handler_init;
    service->control->handler_cb = &ns_ble_new_handler;
    service->control->procMsg_cb = &ns_ble_new_proc_msg;

    // Generic_init will fill in g_ns_ble_control with handlerIds
    // (after initializing the cordio stack.)
    ns_ble_generic_init(TRUE, &g_ns_ble_control, service->control);
    g_ns_ble_control.services[0] = service;
    g_ns_ble_control.numServices = 1;

    return NS_STATUS_SUCCESS;
}

// Create a Characteristic and related attributes
int ns_ble_create_characteristic(
    ns_ble_characteristic_t *c, const char *uuidString, void *applicationValue,
    uint16_t valueLength, uint16_t properties, ns_ble_characteristic_read_handler_t readHandlerCb,
    ns_ble_characteristic_write_handler_t writeHandlerCb,
    ns_ble_characteristic_notify_handler_t notifyHandlerCb, uint16_t periodMs, uint8_t async,
    uint16_t *attributeCount) {
    uint8_t prop = 0;
    uint16_t permissions = 0;

    c->readHandlerCb = readHandlerCb;
    c->writeHandlerCb = writeHandlerCb;
    c->notifyHandlerCb = notifyHandlerCb;

    // *** Remember mem location of attribute's value
    // (different from WSF 'value', which is a placeholder)
    c->applicationValue = applicationValue;
    c->valueLen = valueLength;

    NS_TRY(ns_ble_char2uuid(uuidString, &(c->uuid128)), "Failed to create UUID\n");

    // Parse properties
    if (properties & NS_BLE_READ) {
        permissions |= ATTS_PERMIT_READ;
        prop |= ATT_PROP_READ;
    }

    if (properties & NS_BLE_WRITE) {
        permissions |= ATTS_PERMIT_WRITE;
        prop |= ATT_PROP_WRITE;
    }

    if (properties & NS_BLE_NOTIFY) {
        prop |= ATT_PROP_NOTIFY;
    }
    c->pValue = ns_malloc(valueLength);

    // *** Create Attribute List entries (Declaration, Value, CCC)

    // Characteristic Declaration Attribute
    // Declaration Properties
    c->declarationProperties[0] = prop;
    c->declarationProperties[1] = 0; // placeholder for declaration handle
    c->declarationProperties[2] = 0; // placeholder for declaration handle
    memcpy(&(c->declarationProperties[3]), c->uuid128.array, 16);
    c->declarationLen = sizeof(c->declarationProperties);

    // Put declaration attribute together
    c->declaration.pUuid = attChUuid;
    c->declaration.pValue = c->declarationProperties;
    c->declaration.pLen = &(c->declarationLen);
    c->declaration.maxLen = sizeof(c->declarationProperties);
    c->declaration.settings = 0;
    c->declaration.permissions = ATTS_PERMIT_READ;
    *attributeCount += 1;

    // Characteristic Value Attribute
    c->value.pUuid = c->uuid128.array;
    c->value.pValue = c->pValue;
    c->value.pLen = &(c->valueLen);
    c->value.maxLen = valueLength;
    c->value.settings = ATTS_SET_UUID_128 | ATTS_SET_VARIABLE_LEN;
    if (readHandlerCb != NULL) {
        c->value.settings |= ATTS_SET_READ_CBACK;
    }
    if (writeHandlerCb != NULL) {
        c->value.settings |= ATTS_SET_WRITE_CBACK;
    }
    c->value.permissions = permissions;
    *attributeCount += 1;

    // If Notify, add CCC
    if (properties & NS_BLE_NOTIFY) {
        // Create the CCC attribute
        c->cccLen = sizeof(c->cccArray);
        c->ccc.pUuid = attCliChCfgUuid;
        c->ccc.pValue = c->cccArray;
        c->ccc.pLen = &(c->cccLen);
        c->ccc.maxLen = sizeof(c->cccArray);
        c->ccc.settings = ATTS_SET_CCC;
        c->ccc.permissions = ATTS_PERMIT_READ | ATTS_PERMIT_WRITE;
        c->indicationPeriod = periodMs;
        c->indicationIsAsynchronous = async;
        *attributeCount += 1;
    } else {
        c->ccc.pUuid = NULL;
    }

    return NS_STATUS_SUCCESS;
}

int ns_ble_add_characteristic(ns_ble_service_t *s, ns_ble_characteristic_t *c) {
    // *** Add Characteristic to Service Attributes List
    // Declaration Attribute
    memcpy(&(s->attributes[s->nextAttributeIndex++]), &(c->declaration), sizeof(attsAttr_t));

    // Value Attribute
    memcpy(&(s->attributes[s->nextAttributeIndex++]), &(c->value), sizeof(attsAttr_t));

    // CCC Attribute
    if (c->ccc.pUuid != NULL) {
        memcpy(&(s->attributes[s->nextAttributeIndex++]), &(c->ccc), sizeof(attsAttr_t));
    }

    // *** Set Characteristic Handles
    c->declarationHandle = s->nextHandleId++;
    c->valueHandle = s->nextHandleId++;
    c->declarationProperties[1] = c->valueHandle & 0xff;
    c->declarationProperties[2] = (c->valueHandle >> 8) & 0xff;

    // *** CCC table and time setup (if notify is enabled for the characteristic)
    if (c->ccc.pUuid != NULL) {
        c->cccHandle = s->nextHandleId++;
        c->cccIndicationHandle = s->nextCccIndicationHandle++;
        c->cccIndex = s->nextCccIndex;

        // Add an entry to the CCC table
        s->cccSet[s->nextCccIndex].handle = c->cccHandle;
        s->cccSet[s->nextCccIndex].valueRange = ATT_CLIENT_CFG_NOTIFY;
        s->cccSet[s->nextCccIndex].secLevel = DM_SEC_LEVEL_NONE;

        // Set up CCC timer
        c->indicationTimer.handlerId = g_ns_ble_control.handlerId;
        c->indicationTimer.msg.event = c->cccIndicationHandle;
        c->indicationTimer.msg.status = s->nextCccIndex;
        s->nextCccIndex += 1;
    }

    // Add Characteristic to Service Characteristic List
    s->characteristics[s->nextCharacteristicIndex++] = c;

    return NS_STATUS_SUCCESS;
}

int ns_ble_start_service(ns_ble_service_t *s) {
    // *** Finish creating Service structures, then kick it off
    if (s->nextCharacteristicIndex != s->numCharacteristics) {
        ns_lp_printf(
            "ns_ble_start_service: numCharacteristics mismatch, specified %d, added %d\n",
            s->numCharacteristics, s->nextCharacteristicIndex);
        return NS_STATUS_FAILURE;
    }
    // Populate Group
    s->group.pNext = NULL;
    s->group.pAttr = s->attributes;
    s->group.readCback = s->readCback;
    s->group.writeCback = s->writeCback;
    s->group.startHandle = s->baseHandle;
    s->group.endHandle = s->nextHandleId - 1;

    AttsAddGroup(&(s->group));
    GattSetSvcChangedIdx(0); // TODO something better here

    // *** Kick off the service
    DmDevReset();

    return NS_STATUS_SUCCESS;
}

int ns_ble_set_tx_power(txPowerLevel_t power) {
    // valid power level is checked in HciVscSetRfPowerLevelEx(power), so no need to check here
    if(HciVscSetRfPowerLevelEx(power)) {
        return NS_STATUS_SUCCESS;
    }
    else return NS_STATUS_FAILURE;
}