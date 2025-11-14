/**
 * @file audio_webble.h
 * @author Ambiq
 * @brief Stream encoded audio to Web BLE dashboard
 * @version 0.1
 * @date 2023-10-25
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
    (WEBBLE_WSF_BUFFER_POOLS * 16 + 16 * 8 + 32 * 4 + 64 * 6 + 600 * 14) / sizeof(uint32_t)

static uint32_t webbleWSFBufferPool[WEBBLE_WSF_BUFFER_SIZE];
static wsfBufPoolDesc_t webbleBufferDescriptors[WEBBLE_WSF_BUFFER_POOLS] = {
    {16, 8}, // 16 bytes, 8 buffers
    {32, 4},
    {64, 6},
    {600, 14}};

static ns_ble_pool_config_t webbleWsfBuffers = {
    .pool = webbleWSFBufferPool,
    .poolSize = sizeof(webbleWSFBufferPool),
    .desc = webbleBufferDescriptors,
    .descNum = WEBBLE_WSF_BUFFER_POOLS};

// An arbitrary base UUID
#define webbleUuid(uuid) "19690001" uuid "1234abcd5678aabb1011"

// BLE Structs, populated by webble_service_init()
ns_ble_service_t webbleService;          // Webble Service
ns_ble_characteristic_t webbleOpusAudio; // Opus-encoded Audio Characteristic
// ns_ble_characteristic_t bleIdLatency;           // How long NNID takes to run, per 10ms frame
// ns_ble_characteristic_t bleOpusLatency;         // How long Opus takes to run, per 10ms frame
ns_ble_characteristic_t webbleVad;        // State of VAD
ns_ble_characteristic_t webbleMessage;    // Message to dashboard
ns_ble_characteristic_t dashEnrollButton; // Indicates the web dashboard Enroll button was pressed
ns_ble_characteristic_t dashIdButton;     // Indicates the web dashboard Identify button was pressed
ns_ble_characteristic_t webbleNumUtterances; // Read by the dashboard to track the number of
                                             // utterances recognized

int webbleNotifyHandler(ns_ble_service_t *s, struct ns_ble_characteristic *c) {
    // ns_lp_printf(" webbleNotifyHandler\n"); // really just a nop
    return NS_STATUS_SUCCESS;
}

int webbleReadHandler(ns_ble_service_t *s, struct ns_ble_characteristic *c, void *dest) {
    // ns_lp_printf("webbleReadHandler\n");
    memcpy(dest, c->applicationValue, c->valueLen);
    return NS_STATUS_SUCCESS;
}

int webbleWriteHandler(ns_ble_service_t *s, struct ns_ble_characteristic *c, void *src) {
    ns_lp_printf("webbleWriteHandler value %x\n", *(uint8_t *)src);
    memcpy(c->applicationValue, src, c->valueLen);
    return NS_STATUS_SUCCESS;
}

int audioWebbleServiceInit(void) {
    char webbleName[] = "WebbleAudio";

    NS_TRY(
        ns_ble_char2uuid(webbleUuid("0000"), &(webbleService.uuid128)), "Failed to convert UUID\n");

    memcpy(webbleService.name, webbleName, sizeof(webbleName));
    webbleService.nameLen = sizeof(webbleName) - 1; // exclude null terminator
    webbleService.baseHandle = 0x0800;
    webbleService.poolConfig = &webbleWsfBuffers;
    webbleService.numAttributes = 0;

    // We create a characteristic which is basically a chunk of Opus-encoded audio
    ns_ble_create_characteristic(
        &webbleOpusAudio, webbleUuid("5001"), encodedDataBuffer, sizeof(encodedDataBuffer),
        NS_BLE_READ | NS_BLE_NOTIFY, NULL, NULL, &webbleNotifyHandler, 65000, true,
        &(webbleService.numAttributes));

    // ns_ble_create_characteristic(
    //     &bleIdLatency, webbleUuid("5002"), &idLatency, sizeof(seLatency),
    //     NS_BLE_READ | NS_BLE_NOTIFY, NULL, NULL, &webbleNotifyHandler, 65000, true,
    //     &(webbleService.numAttributes));

    // ns_ble_create_characteristic(
    //     &bleOpusLatency, webbleUuid("5003"), &opusLatency, sizeof(opusLatency),
    //     NS_BLE_READ | NS_BLE_NOTIFY, NULL, NULL, &webbleNotifyHandler, 65000, true,
    //     &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &webbleVad, webbleUuid("5004"), &vad, sizeof(vad), NS_BLE_READ | NS_BLE_NOTIFY, NULL, NULL,
        &webbleNotifyHandler, 65000, true, &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &webbleMessage, webbleUuid("5005"), &msgBuf, sizeof(msgBuf), NS_BLE_READ | NS_BLE_NOTIFY,
        NULL, NULL, &webbleNotifyHandler, 65000, true, &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &dashEnrollButton, webbleUuid("5006"), &enrollBtn, sizeof(enrollBtn),
        NS_BLE_READ | NS_BLE_WRITE, &webbleReadHandler, &webbleWriteHandler, NULL, 0, false,
        &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &dashIdButton, webbleUuid("5007"), &idBtn, sizeof(idBtn), NS_BLE_READ | NS_BLE_WRITE,
        &webbleReadHandler, &webbleWriteHandler, NULL, 0, false, &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &webbleNumUtterances, webbleUuid("5008"), &numUtterances, sizeof(numUtterances),
        NS_BLE_READ | NS_BLE_NOTIFY, NULL, NULL, &webbleNotifyHandler, 65000, true,
        &(webbleService.numAttributes));

    // Create the service
    webbleService.numCharacteristics = 6; // needed to allocate memory for characteristics
    ns_ble_create_service(&webbleService);
    ns_ble_add_characteristic(&webbleService, &webbleOpusAudio);
    ns_ble_add_characteristic(&webbleService, &webbleVad);
    ns_ble_add_characteristic(&webbleService, &webbleMessage);
    ns_ble_add_characteristic(&webbleService, &dashEnrollButton);
    ns_ble_add_characteristic(&webbleService, &dashIdButton);
    ns_ble_add_characteristic(&webbleService, &webbleNumUtterances);
    ns_ble_start_service(&webbleService); // Initialize BLE, create structs, start service
    return NS_STATUS_SUCCESS;
}

void RadioTask(void *pvParameters) {
    NS_TRY(audioWebbleServiceInit(), "BLE Service Init failed.\n");

    ns_delay_us(1000000);
    while (1) {
        wsfOsDispatcher();
    }
}
