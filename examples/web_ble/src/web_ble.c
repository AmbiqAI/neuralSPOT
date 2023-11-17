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
#include "ns_peripherals_power.h"
#include "FreeRTOS.h"
#include "task.h"
#include "arm_math.h"

// WSF buffer pools are a bit of black magic. More
// development needed.
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

// Values that will be sent/recieved over BLE
// Each of these will be associated with a Webble Characteristic
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

// WSF/Cordio is built on top of FreeRTOS. We need to create a task
TaskHandle_t radio_task_handle;
TaskHandle_t my_xSetupTask;

/**
 * @brief Dummy up some interesting sensor values
 *
 */
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

// ------- Webble Service-specific code

#define webbleUuid(uuid) "19b10000" uuid "537e4f6cd104768a1214"

// BLE Structs, populated by webble_service_init()
ns_ble_service_t webbleService; // Webble Service

// Webble Service Characteristics
ns_ble_characteristic_t webbleTemperature, webbleHumidity, webblePressure;
ns_ble_characteristic_t webbleAccel, webbleGyro, webbleQuat;
ns_ble_characteristic_t webbleRgb, webbleBsec, webbleCo2, webbleGas;

// Hooks in case special handling is needed for a service
// (not needed in this case)
void webbleHandler(wsfEventMask_t event, wsfMsgHdr_t *pMsg) {
    // ns_lp_printf("webbleHandler\n");
}

void webbleHandlerInit(wsfHandlerId_t handlerId) {
    // ns_lp_printf("webbleHandlerInit\n");
}

/**
 * @brief Handle a read request from the client. Invoked by the BLE
 * stack when a client requests a read of a characteristic value.
 *
 * @param s - service handle
 * @param c - characteristic being read
 * @param dest - a buffer to copy the characteristic value into
 * @return int
 */
int webbleReadHandler(ns_ble_service_t *s, struct ns_ble_characteristic *c, void *dest) {
    // ns_lp_printf("webbleReadHandler\n");
    memcpy(dest, c->applicationValue, c->valueLen);
    return NS_STATUS_SUCCESS;
}

/**
 * @brief Handle a write request from the client. Invoked by the BLE
 * stack when a client requests a write of a characteristic value.
 *
 * @param s - service handle
 * @param c - handle of the characteristic being written
 * @param src - a buffer containing the value to be written
 * @return int
 */
int webbleWriteHandler(ns_ble_service_t *s, struct ns_ble_characteristic *c, void *src) {
    // ns_lp_printf("webbleWriteHandler value %x\n", *(uint8_t *)src);
    memcpy(c->applicationValue, src, c->valueLen);
    return NS_STATUS_SUCCESS;
}

/**
 * @brief Handle a notification from the client. Invoked every time a notification timer expires.
 * Meant to be used to update the characteristic values if that is the desired app behavior.
 * @param s - service handle
 * @param c - handle of the characteristic being notified
 * @return int
 */
int webbleNotifyHandler(ns_ble_service_t *s, struct ns_ble_characteristic *c) {
    // ns_lp_printf("webbleNotifyHandler\n");
    webbleUpdateSensorValues();
    return NS_STATUS_SUCCESS;
}

// Debug helper
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

// Must be invoked from the main RadioTask
int webble_service_init(void) {

    char webbleName[] = "Webble";
    // Creating a BLE service involves the following steps
    // 1. Initialize the service with a UUID, handle space, and buffer pool
    // 2. Define the characteristics to be added to the service
    // 3. Add those characteristics to the service
    // 4. Kick the whole thing off.
    //
    // Everything else (attribute definition, discovery, etc.) is handled by ns-ble

    // Initialize BLE service with default values
    // memcpy(&webbleService, &defaultBleService, sizeof(defaultBleService));

    // Customize Service-specific fields
    NS_TRY(
        ns_ble_char2uuid(webbleUuid("0000"), &(webbleService.uuid128)), "Failed to convert UUID\n");
    memcpy(webbleService.name, webbleName, sizeof(webbleName));
    webbleService.nameLen = sizeof(webbleName) - 1; // exclude null terminator
    webbleService.baseHandle = 0x0800;
    webbleService.poolConfig = &webbleWsfBuffers;
    webbleService.numAttributes = 0;

    // Define characteristics to add to service.

    ns_ble_create_characteristic(
        &webbleTemperature, webbleUuid("2001"), &temperature, sizeof(temperature), NS_BLE_READ,
        &webbleReadHandler, NULL, NULL, 0, false, &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &webbleHumidity, webbleUuid("3001"), &humidity, sizeof(humidity), NS_BLE_READ,
        &webbleReadHandler, NULL, NULL, 0, false, &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &webblePressure, webbleUuid("4001"), &pressure, sizeof(pressure), NS_BLE_READ,
        &webbleReadHandler, NULL, NULL, 0, false, &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &webbleAccel, webbleUuid("5001"), accel, sizeof(accel), NS_BLE_READ | NS_BLE_NOTIFY, NULL,
        NULL, &webbleNotifyHandler, 200, false, &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &webbleGyro, webbleUuid("6001"), gyro, sizeof(gyro), NS_BLE_READ | NS_BLE_NOTIFY, NULL,
        NULL, &webbleNotifyHandler, 200, false, &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &webbleQuat, webbleUuid("7001"), quat, sizeof(quat), NS_BLE_READ | NS_BLE_NOTIFY, NULL,
        NULL, &webbleNotifyHandler, 200, false, &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &webbleRgb, webbleUuid("8001"), rgb, sizeof(rgb), NS_BLE_READ | NS_BLE_WRITE,
        &webbleReadHandler, &webbleWriteHandler, NULL, 0, false, &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &webbleBsec, webbleUuid("9001"), &bsec, sizeof(bsec), NS_BLE_READ, &webbleReadHandler, NULL,
        NULL, 0, false, &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &webbleCo2, webbleUuid("9002"), &co2, sizeof(co2), NS_BLE_READ, &webbleReadHandler, NULL,
        NULL, 0, false, &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &webbleGas, webbleUuid("9003"), &gas, sizeof(gas), NS_BLE_READ, &webbleReadHandler, NULL,
        NULL, 0, false, &(webbleService.numAttributes));

    // Create the service
    webbleService.numCharacteristics = 10; // needed to allocate memory for characteristics
    ns_ble_create_service(&webbleService);

    // Add characteristics defined above to the service
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

    ns_ble_start_service(&webbleService); // Initialize BLE, create structs, start service
    return NS_STATUS_SUCCESS;
}
void RadioTask(void *pvParameters) {
    NS_TRY(webble_service_init(), "ideal_main failed.\n");
    while (1) {
        wsfOsDispatcher();
    }
}

void setup_task(void *pvParameters) {
    ns_lp_printf("setup_task\n");
    ns_ble_pre_init(); // Set NVIC priorities
    xTaskCreate(RadioTask, "RadioTask", 512, 0, 3, &radio_task_handle);
    vTaskSuspend(NULL);
    while (1)
        ;
}

int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\b");
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed\n");
    NS_TRY(ns_set_performance_mode(NS_MINIMUM_PERF), "Set CPU Perf mode failed.");
    // am_bsp_low_power_init();

    ns_itm_printf_enable();
    ns_interrupt_master_enable();
    xTaskCreate(setup_task, "Setup", 512, 0, 3, &my_xSetupTask);
    vTaskStartScheduler();
    while (1) {
    };
}
