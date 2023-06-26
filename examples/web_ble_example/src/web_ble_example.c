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
#include "svc_webble.h"
#include "ns_peripherals_power.h"
#include "FreeRTOS.h"
#include "task.h"
#include "arm_math.h"
/***********************************/
/****** application-specific stuff */
/***********************************/

/**************************************************************************************************
  Advertising Data
**************************************************************************************************/

/*! advertising data, discoverable mode */
static uint8_t webbleAdvDataDisc[] = {
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
    ATT_UUID_WEBBLE_SERVICE};

/*! scan data, discoverable mode */
static uint8_t webbleScanDataDisc[] = {
    /*! device name */
    7,                      /*! length */
    DM_ADV_TYPE_LOCAL_NAME, /*! AD type */
    'W',
    'e',
    'b',
    'b',
    'l',
    'e',
};

/**************************************************************************************************
  Client Characteristic Configuration Descriptors
**************************************************************************************************/

/*! enumeration of client characteristic configuration descriptors */
enum {
    WEBBLE_GATT_SC_CCC_IDX,       /*! GATT service, service changed characteristic */
    WEBBLE_accelerometer_CCC_IDX, /*! Webble service, accel characteristic */
    WEBBLE_gyroscope_CCC_IDX,     /*! Webble service, gyro characteristic */
    WEBBLE_quaternion_CCC_IDX,    /*! Webble service, quaternion characteristic */
    WEBBLE_NUM_CCC_IDX
};

#define WEBBLE_MSG_START 0xC0

enum {
    WEBBLE_accelerometer_IND = WEBBLE_MSG_START,
    WEBBLE_gyroscope_IND,
    WEBBLE_quaternion_IND,
};

static const attsCccSet_t webbleCccSet[WEBBLE_NUM_CCC_IDX] = {
    {GATT_SC_CH_CCC_HDL, ATT_CLIENT_CFG_INDICATE, DM_SEC_LEVEL_NONE},
    {WEBBLE_ACCELEROMETER_CCC_HDL, ATT_CLIENT_CFG_NOTIFY, DM_SEC_LEVEL_NONE},
    {WEBBLE_GYROSCOPE_CCC_HDL, ATT_CLIENT_CFG_NOTIFY, DM_SEC_LEVEL_NONE},
    {WEBBLE_QUATERNION_CCC_HDL, ATT_CLIENT_CFG_NOTIFY, DM_SEC_LEVEL_NONE},
};

#define WEBBLE_WSF_BUFFER_POOLS 4
#define WEBBLE_WSF_BUFFER_SIZE                                                                     \
    (WEBBLE_WSF_BUFFER_POOLS * 16 + 16 * 8 + 32 * 4 + 64 * 6 + 280 * 14) / sizeof(uint32_t)

static uint32_t webbleWSFBufferPool[WEBBLE_WSF_BUFFER_SIZE];
static wsfBufPoolDesc_t webbleBufferDescriptors[WEBBLE_WSF_BUFFER_POOLS] = {
    {16, 8}, // 16 bytes, 8 buffers
    {32, 4},
    {64, 6},
    {512, 14}};

static ns_ble_service_control_t webbleControl;
static ns_ble_connection_t webbleConnectionControl;

typedef struct {
    wsfTimer_t measTimer; /*! \brief periodic measurement timer */
    uint16_t handle;
    uint16_t len;
    uint8_t *value;
} webbleCb_t;

static webbleCb_t webbleAccelCb;
static webbleCb_t webbleGyroCb;
static webbleCb_t webbleQuatCb;

// Values
float temperature = 0.0;
uint16_t humidity = 2;
float pressure = 4.0;
float accel[3] = {0.0, 0.0, 0.0};
float gyro[3] = {0.0, 0.0, 0.0};
float quat[4] = {0.0, 0.0, 0.0, 0.0};
float bsec = 6.0;
uint32_t co2 = 8;
uint16_t gas = 1;
float x = 0.0;

static void startNotifications(webbleCb_t *cb) { WsfTimerStartMs(&(cb->measTimer), 200); }

static void stopNotifications(webbleCb_t *cb) { WsfTimerStop(&(cb->measTimer)); }

static void webbleProcCccState(ns_ble_msg_t *pMsg) {
    // ns_lp_printf("webbleProcCccState\n");
    attsCccEvt_t *pEvt = (attsCccEvt_t *)pMsg;

    // APP_TRACE_INFO1("[%s]", __func__);
    // APP_TRACE_INFO1("handle           = 0x%X", pEvt->handle);
    // APP_TRACE_INFO1("value            = 0x%X", pEvt->value);
    // APP_TRACE_INFO1("idx              = 0x%X", pEvt->idx);

    switch (pEvt->idx) {
    case WEBBLE_GATT_SC_CCC_IDX:
        break;
    case WEBBLE_accelerometer_CCC_IDX:
        if (pEvt->value == ATT_CLIENT_CFG_NOTIFY) {
            // enable notifications
            startNotifications(&webbleAccelCb);
        } else {
            // disable notifications
            stopNotifications(&webbleAccelCb);
        }
        break;
    case WEBBLE_gyroscope_CCC_IDX:
        if (pEvt->value == ATT_CLIENT_CFG_NOTIFY) {
            // enable notifications
            startNotifications(&webbleGyroCb);
            // webbleControl.gyroscopeNotifications = true;
        } else {
            // disable notifications
            stopNotifications(&webbleGyroCb);
            // webbleControl.gyroscopeNotifications = false;
        }
        break;
    case WEBBLE_quaternion_CCC_IDX:
        if (pEvt->value == ATT_CLIENT_CFG_NOTIFY) {
            // enable notifications
            startNotifications(&webbleQuatCb);
            // webbleControl.quaternionNotifications = true;
        } else {
            // disable notifications
            stopNotifications(&webbleQuatCb);
            // webbleControl.quaternionNotifications = false;
        }
        break;
    default:
        ns_lp_printf("webbleProcCccState: unknown idx %d\n", pEvt->idx);
        break;
    }
}

int cordioisstupid = 0;

static void webbleSendValue(webbleCb_t *cb, attEvt_t *pMsg) {
    dmConnId_t connId = 1;
    // ns_lp_printf("webbleSendValue");
    if (AttsCccEnabled(connId, cb->measTimer.msg.status)) {
        // if ((cb->handle == 2059) && (gyroisstupid <= 10)) {
        if ((cordioisstupid <= 20)) {
            // ns_lp_printf("... handle %d len %d value %d\n", cb->handle, cb->len, cb->value);
            //     ns_lp_printf("... pmsg len %d  val %f\n", pMsg->valueLen, pMsg->pValue[0]);
            cordioisstupid++;
        }
        AttsSetAttr(cb->handle, cb->len, cb->value);
        AttsHandleValueNtf(connId, cb->handle, cb->len, cb->value);
    } else {
        ns_lp_printf("... not sent\n");
    }
}

static void webbleUpdateSensorValues(void) {
    // ns_lp_printf("webbleUpdateSensorValues\n");
    x += 0.1;
    if (x > 2 * PI) {
        x = 0.0;
    }
    accel[0] = arm_sin_f32(x);
    accel[1] = arm_cos_f32(x);
    accel[2] = arm_sin_f32(1 - x);
    gyro[0] = arm_sin_f32(-x);
    gyro[1] = arm_cos_f32(-x);
    gyro[2] = arm_sin_f32(1 - x);
    quat[0] = arm_sin_f32(x);
    quat[1] = arm_cos_f32(x);
    quat[2] = arm_sin_f32(-x);
    quat[3] = arm_sin_f32(-x);
    temperature = arm_sin_f32(x / 2);
    pressure = arm_cos_f32(x / 2);
    bsec = arm_sin_f32(x / 4);
}

static void webbleTimerExpired(webbleCb_t *cb, attEvt_t *pMsg) {
    ns_lp_printf("webbleTimerExpired\n");
    cordioisstupid = 0;
    webbleUpdateSensorValues();
    webbleSendValue(cb, pMsg);
    startNotifications(cb); // restart timer
}

static void webbleHandleValueCnf(attEvt_t *pMsg) {
    // ns_lp_printf("webbleHandleValueCnf\n");
    // Send value depending on which characteristic is being confirmed
    switch (pMsg->handle) {
    case WEBBLE_ACCELEROMETER_HDL:
        // webbleSendValue(&webbleAccelCb, pMsg);
        break;
    case WEBBLE_GYROSCOPE_HDL:
        // ns_lp_printf("webbleHandleValueCnf: GYROSCOPE\n");
        // webbleSendValue(&webbleGyroCb, pMsg);
        break;
    case WEBBLE_QUATERNION_HDL:
        // ns_lp_printf("webbleHandleValueCnf: QUATERNION\n");
        // webbleSendValue(&webbleQuatCb, pMsg);
        break;
    }
}

bool webbleMsgProc(ns_ble_msg_t *pMsg) {
    bool messageHandled = true;
    // ns_lp_printf("webbleMsgProc\n");

    switch (pMsg->hdr.event) {
    case DM_CONN_OPEN_IND:
        ns_ble_generic_conn_open((dmEvt_t *)pMsg);
        DmConnSetDataLen(1, 251, 0x848);
        break;

    case DM_CONN_CLOSE_IND:
        // amdtps_conn_close((dmEvt_t *) pMsg);
        break;

    case ATTS_CCC_STATE_IND:
        webbleProcCccState(pMsg);
        break;

    case ATTS_HANDLE_VALUE_CNF:
        webbleHandleValueCnf((attEvt_t *)pMsg);
        break;

    case WEBBLE_accelerometer_IND:
        webbleTimerExpired(&webbleAccelCb, (attEvt_t *)pMsg);
        break;
    case WEBBLE_gyroscope_IND:
        webbleTimerExpired(&webbleGyroCb, (attEvt_t *)pMsg);
        break;
    case WEBBLE_quaternion_IND:
        webbleTimerExpired(&webbleQuatCb, (attEvt_t *)pMsg);
        break;
    default:
        messageHandled = false;
        break;
    }
    return messageHandled;
}

uint8_t webbleWrite_cb(
    dmConnId_t connId, uint16_t handle, uint8_t operation, uint16_t offset, uint16_t len,
    uint8_t *pValue, attsAttr_t *pAttr) {
    ns_lp_printf("webbleWrite_cb\n");

    // ns_lp_printf("handle: %d\n", handle);
    // *(pAttr->pValue) = handle;

    return ATT_SUCCESS;
}

uint8_t webbleRead_cb(
    dmConnId_t connId, uint16_t handle, uint8_t operation, uint16_t offset, attsAttr_t *pAttr) {
    // ns_lp_printf("webbleRead_cb\n");
    // ns_lp_printf("handle: %d\n", handle);
    switch (handle) {
    case WEBBLE_ACCELEROMETER_HDL:
        memcpy(pAttr->pValue, accel, sizeof(accel));
        break;
    case WEBBLE_GYROSCOPE_HDL:
        memcpy(pAttr->pValue, gyro, sizeof(gyro));
        break;
    case WEBBLE_QUATERNION_HDL:
        memcpy(pAttr->pValue, quat, sizeof(quat));
        break;
    case WEBBLE_TEMPERATURE_HDL:
        memcpy(pAttr->pValue, &temperature, sizeof(temperature));
        break;
    case WEBBLE_HUMIDITY_HDL:
        memcpy(pAttr->pValue, &humidity, sizeof(humidity));
        break;
    case WEBBLE_PRESSURE_HDL:
        memcpy(pAttr->pValue, &pressure, sizeof(pressure));
        break;
    case WEBBLE_BSEC_HDL:
        memcpy(pAttr->pValue, &bsec, sizeof(bsec));
        break;
    case WEBBLE_CO2_HDL:
        memcpy(pAttr->pValue, &co2, sizeof(co2));
        break;
    case WEBBLE_GAS_HDL:
        ns_lp_printf("WEBBLE_GAS_HDL\n");
        memcpy(pAttr->pValue, &gas, sizeof(gas));
        break;
    }
    return ATT_SUCCESS;
}

void webbleHandler(wsfEventMask_t event, wsfMsgHdr_t *pMsg) {
    // ns_lp_printf("webbleHandler\n");
}

void webbleHandlerInit(wsfHandlerId_t handlerId) {
    // ns_lp_printf("webbleHandlerInit\n");
}

void webbleInit(void) {
    webbleControl.bufferPool = webbleWSFBufferPool;
    webbleControl.bufferPoolSize = sizeof(webbleWSFBufferPool);
    webbleControl.bufferDescriptors = webbleBufferDescriptors;
    webbleControl.bufferDescriptorsSize = sizeof(webbleBufferDescriptors);
    webbleControl.wsfBufCount = WEBBLE_WSF_BUFFER_POOLS;
    webbleControl.advData = webbleAdvDataDisc;
    webbleControl.advDataLen = sizeof(webbleAdvDataDisc);
    webbleControl.scanData = webbleScanDataDisc;
    webbleControl.scanDataLen = sizeof(webbleScanDataDisc);

    webbleControl.handler_init_cb = &webbleHandlerInit;
    webbleControl.handler_cb = &webbleHandler;
    webbleControl.procMsg_cb = &webbleMsgProc;
    webbleControl.cccSet = webbleCccSet;
    webbleControl.cccCount = WEBBLE_NUM_CCC_IDX;
    // webbleControl.ccc_cb = webbleCccCallback;

    ns_ble_generic_init(TRUE, &g_ns_ble_control,
                        &webbleControl); // Use defaults to init BLE stack

    SvcWebbleCbackRegister(webbleRead_cb, webbleWrite_cb);
    SvcWebbleAddGroup();
    GattSetSvcChangedIdx(WEBBLE_GATT_SC_CCC_IDX);

    DmDevReset();

    // Initialize local control blocks
    webbleAccelCb.measTimer.handlerId = webbleControl.handlerId;
    webbleAccelCb.measTimer.msg.event = WEBBLE_accelerometer_IND;
    webbleAccelCb.measTimer.msg.status = WEBBLE_accelerometer_CCC_IDX;
    webbleAccelCb.handle = WEBBLE_ACCELEROMETER_HDL;
    webbleAccelCb.value = (uint8_t *)accel;
    webbleAccelCb.len = sizeof(accel);

    webbleGyroCb.measTimer.handlerId = webbleControl.handlerId;
    webbleGyroCb.measTimer.msg.event = WEBBLE_gyroscope_IND;
    webbleGyroCb.measTimer.msg.status = WEBBLE_gyroscope_CCC_IDX;
    webbleGyroCb.handle = WEBBLE_GYROSCOPE_HDL;
    webbleGyroCb.value = (uint8_t *)gyro;
    webbleGyroCb.len = sizeof(gyro);

    webbleQuatCb.measTimer.handlerId = webbleControl.handlerId;
    webbleQuatCb.measTimer.msg.event = WEBBLE_quaternion_IND;
    webbleQuatCb.measTimer.msg.status = WEBBLE_quaternion_CCC_IDX;
    webbleQuatCb.handle = WEBBLE_QUATERNION_HDL;
    webbleQuatCb.value = (uint8_t *)quat;
    webbleQuatCb.len = sizeof(quat);
}

TaskHandle_t radio_task_handle;
TaskHandle_t my_xSetupTask;

void RadioTask(void *pvParameters) {
    ns_lp_printf("RadioTask\n");
    webbleInit();
    ns_lp_printf("RadioTask: webbleInit done\n");
    while (1) {
        wsfOsDispatcher();
    }
}

void setup_task(void *pvParameters) {
    ns_lp_printf("setup_task\n");
    // RadioTaskSetup();
    ns_ble_pre_init(); // Set NVIC priorities
    xTaskCreate(RadioTask, "RadioTask", 512, 0, 3, &radio_task_handle);
    vTaskSuspend(NULL);
    while (1)
        ;
}

int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\b");
    // NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed\n");
    // NS_TRY(ns_set_performance_mode(NS_MINIMUM_PERF), "Set CPU Perf mode failed.");
    am_bsp_low_power_init();

    ns_itm_printf_enable();
    ns_interrupt_master_enable();
    ns_lp_printf("Hello World\n");
    xTaskCreate(setup_task, "Setup", 512, 0, 3, &my_xSetupTask);
    vTaskStartScheduler();
    while (1) {
    };
}
