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
    (WEBBLE_WSF_BUFFER_POOLS * 16 + 16 * 8 + 32 * 4 + 64 * 6 + 280 * 14) / sizeof(uint32_t)

static uint32_t webbleWSFBufferPool[WEBBLE_WSF_BUFFER_SIZE];
static wsfBufPoolDesc_t webbleBufferDescriptors[WEBBLE_WSF_BUFFER_POOLS] = {
    {16, 8}, // 16 bytes, 8 buffers
    {32, 4},
    {64, 6},
    {280, 14}};

static ns_ble_pool_config_t webbleWsfBuffers = {
    .pool = webbleWSFBufferPool,
    .poolSize = sizeof(webbleWSFBufferPool),
    .desc = webbleBufferDescriptors,
    .descNum = WEBBLE_WSF_BUFFER_POOLS};

// An arbitrary base UUID
#define webbleUuid(uuid) "19690000" uuid "1234abcd5678aabb1011"

// BLE Structs, populated by webble_service_init()
ns_ble_service_t webbleService;          // Webble Service
ns_ble_characteristic_t webbleOpusAudio; // Opus-encoded Audio Characteristic
// ns_ble_characteristic_t audioFrameAvailable; // Doorbell`
ns_ble_characteristic_t bleSELatency;   // How long SE takes to run, per 10ms frame
ns_ble_characteristic_t bleOpusLatency; // How long Opus takes to run, per 10ms frame
ns_ble_characteristic_t bleSEEnabled;   // State of SE enablement

// Values
// extern unsigned char encodedDataBuffer[80];
// extern uint32_t seLatency;
// extern uint32_t opusLatency;
// extern bool enableSE;

int webbleNotifyHandler(ns_ble_service_t *s, struct ns_ble_characteristic *c) {
    ns_lp_printf("NNSE webbleNotifyHandler\n"); // really just a nop
    return NS_STATUS_SUCCESS;
}

int webbleReadHandler(ns_ble_service_t *s, struct ns_ble_characteristic *c, void *dest) {
    ns_lp_printf("webbleReadHandler\n");
    memcpy(dest, c->applicationValue, c->valueLen);
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

    ns_ble_create_characteristic(
        &bleSELatency, webbleUuid("5002"), &seLatency, sizeof(seLatency),
        NS_BLE_READ | NS_BLE_NOTIFY, NULL, NULL, &webbleNotifyHandler, 65000, true,
        &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &bleOpusLatency, webbleUuid("5003"), &opusLatency, sizeof(opusLatency),
        NS_BLE_READ | NS_BLE_NOTIFY, NULL, NULL, &webbleNotifyHandler, 65000, true,
        &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &bleSEEnabled, webbleUuid("5004"), &enableSE, sizeof(enableSE), NS_BLE_READ | NS_BLE_NOTIFY,
        NULL, NULL, &webbleNotifyHandler, 65000, true, &(webbleService.numAttributes));

    // Create the service
    webbleService.numCharacteristics = 4; // needed to allocate memory for characteristics
    ns_ble_create_service(&webbleService);
    ns_ble_add_characteristic(&webbleService, &webbleOpusAudio);
    ns_ble_add_characteristic(&webbleService, &bleSELatency);
    ns_ble_add_characteristic(&webbleService, &bleOpusLatency);
    ns_ble_add_characteristic(&webbleService, &bleSEEnabled);
    ns_ble_start_service(&webbleService); // Initialize BLE, create structs, start service
    return NS_STATUS_SUCCESS;
}

void RadioTask(void *pvParameters) {
    NS_TRY(audioWebbleServiceInit(), "BLE Service Init failed.\n");
    while (1) {
        wsfOsDispatcher();
    }
}
