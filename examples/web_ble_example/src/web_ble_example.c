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

static ns_ble_pool_config_t webbleWsfBuffers = {
    .pool = webbleWSFBufferPool,
    .poolSize = sizeof(webbleWSFBufferPool),
    .desc = webbleBufferDescriptors,
    .descNum = WEBBLE_WSF_BUFFER_POOLS};

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
uint8_t rgb[] = {0, 0, 0};
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
    ns_lp_printf("webbleRead_cb\n");
    ns_lp_printf("handle: %d\n", handle);
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
int ideal_main(void);

void RadioTask(void *pvParameters) {
    ns_lp_printf("RadioTask\n");
    // webbleInit();
    NS_TRY(ideal_main(), "ideal_main failed.\n");
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
    // ideal_main();
    xTaskCreate(setup_task, "Setup", 512, 0, 3, &my_xSetupTask);
    vTaskStartScheduler();
    while (1) {
    };
}

// ----------------------------------------------------------------------------

#define webbleUuid(uuid) "19b10000" uuid "537e4f6cd104768a1214"

int webbleReadHandler(ns_ble_service_t *s, struct ns_ble_characteristic *c, void *dest) {
    ns_lp_printf("webbleReadHandler\n");
    memcpy(dest, c->applicationValue, c->valueLen);
    return NS_STATUS_SUCCESS;
}

int webbleWriteHandler(ns_ble_service_t *s, struct ns_ble_characteristic *c, void *src) {
    ns_lp_printf("webbleWriteHandler\n");
    return NS_STATUS_SUCCESS;
}

int webbleNotifyHandler(ns_ble_service_t *s, struct ns_ble_characteristic *c) {
    ns_lp_printf("webbleNotifyHandler\n");
    webbleUpdateSensorValues();
    return NS_STATUS_SUCCESS;
}

ns_ble_service_t webbleService;
ns_ble_characteristic_t webbleTemperature, webbleHumidity, webblePressure;
ns_ble_characteristic_t webbleAccel, webbleGyro, webbleQuat;
ns_ble_characteristic_t webbleRgb, webbleBsec, webbleCo2, webbleGas;

void print_attribute(attsAttr_t *a, attsAttr_t *b) {
    ns_lp_printf(
        "a. settings: %d, permissions %d, maxLen %d\n", a->settings, a->permissions, a->maxLen);
    ns_lp_printf(
        "b. settings: %d, permissions %d, maxLen %d\n", b->settings, b->permissions, b->maxLen);
    if (a->settings & ATTS_SET_UUID_128) {
        ns_lp_printf("a. UUID: ");
        for (int i = 0; i < 16; i++) {
            ns_lp_printf("%02x", a->pUuid[i]);
        }
        ns_lp_printf("\n");
        ns_lp_printf("b. UUID: ");
        for (int i = 0; i < 16; i++) {
            ns_lp_printf("%02x", b->pUuid[i]);
        }
        ns_lp_printf("\n");
    }
}

int ideal_main(void) {

    char webbleName[] = "Webble";

    // Initialize BLE service with default values
    // memcpy(&webbleService, &defaultBleService, sizeof(defaultBleService));

    // Customize Service-specific fields
    NS_TRY(
        ns_ble_char2uuid(webbleUuid("0000"), &(webbleService.uuid128)), "Failed to convert UUID\n");
    memcpy(webbleService.name, webbleName, sizeof(webbleService.name));
    webbleService.nameLen = sizeof(webbleName) - 1; // exclude null terminator
    webbleService.baseHandle = 0x0800;
    webbleService.poolConfig = &webbleWsfBuffers;
    webbleService.numAttributes = 0;

    // Define characteristics to add to service
    // uint16_t attributeCount = 0; // keeps track of the number of attributes added to the service

    ns_ble_create_characteristic(
        &webbleTemperature, webbleUuid("2001"), &temperature, sizeof(temperature), NS_BLE_READ,
        &webbleReadHandler, NULL, NULL, 0, &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &webbleHumidity, webbleUuid("3001"), &humidity, sizeof(humidity), NS_BLE_READ,
        &webbleReadHandler, NULL, NULL, 0, &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &webblePressure, webbleUuid("4001"), &pressure, sizeof(pressure), NS_BLE_READ,
        &webbleReadHandler, NULL, NULL, 0, &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &webbleAccel, webbleUuid("5001"), accel, sizeof(accel), NS_BLE_READ | NS_BLE_NOTIFY, NULL,
        NULL, &webbleNotifyHandler, 200, &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &webbleGyro, webbleUuid("6001"), gyro, sizeof(gyro), NS_BLE_READ | NS_BLE_NOTIFY, NULL,
        NULL, &webbleNotifyHandler, 200, &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &webbleQuat, webbleUuid("7001"), quat, sizeof(quat), NS_BLE_READ | NS_BLE_NOTIFY, NULL,
        NULL, &webbleNotifyHandler, 200, &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &webbleRgb, webbleUuid("8001"), rgb, sizeof(rgb), NS_BLE_READ | NS_BLE_WRITE,
        &webbleReadHandler, &webbleWriteHandler, NULL, 0, &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &webbleBsec, webbleUuid("9001"), &bsec, sizeof(bsec), NS_BLE_READ, &webbleReadHandler, NULL,
        NULL, 0, &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &webbleCo2, webbleUuid("9002"), &co2, sizeof(co2), NS_BLE_READ, &webbleReadHandler, NULL,
        NULL, 0, &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &webbleGas, webbleUuid("9003"), &gas, sizeof(gas), NS_BLE_READ, &webbleReadHandler, NULL,
        NULL, 0, &(webbleService.numAttributes));

    // Create the service
    webbleService.numCharacteristics = 10;
    ns_ble_create_service(&webbleService);
    ns_ble_add_characteristic(&webbleService, &webbleTemperature);
    ns_ble_add_characteristic(&webbleService, &webbleHumidity);
    ns_ble_add_characteristic(&webbleService, &webblePressure);
    ns_ble_add_characteristic(&webbleService, &webbleAccel);
    ns_ble_add_characteristic(&webbleService, &webbleGyro);
    ns_ble_add_characteristic(&webbleService, &webbleQuat);
    ns_ble_add_characteristic(&webbleService, &webbleRgb);
    ns_ble_add_characteristic(&webbleService, &webbleBsec);
    ns_ble_add_characteristic(&webbleService, &webbleCo2);
    ns_ble_add_characteristic(&webbleService, &webbleGas);

    // for (int i = 0; i < webbleService.numAttributes; i++) {
    //     // ns_lp_printf("%d. %02x %02x \n", i, ((uint8_t*)webbleService.attributes)[i],
    //     ((uint8_t*)webbleAttributeList)[i]); ns_lp_printf("%d. \n", i);
    //     print_attribute(&((attsAttr_t*)webbleService.attributes)[i],
    //     &((attsAttr_t*)webbleAttributeList)[i]);
    // }

    ns_ble_start_service(&webbleService); // Initialize BLE, create structs, start service
    return NS_STATUS_SUCCESS;
}
