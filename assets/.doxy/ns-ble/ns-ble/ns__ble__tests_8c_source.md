

# File ns\_ble\_tests.c

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-ble**](dir_ec3c5c5ea2d338d436d6fa61f38fc381.md) **>** [**tests**](dir_8983aa85a9e91ac6633429bf8b0fa928.md) **>** [**ns\_ble\_tests.c**](ns__ble__tests_8c.md)

[Go to the documentation of this file](ns__ble__tests_8c.md)

```C++

#include "unity/unity.h"
#include "ns_ble.h"
#define configTOTAL_HEAP_SIZE NS_MALLOC_HEAP_SIZE_IN_K * 1024
uint8_t ucHeap[NS_MALLOC_HEAP_SIZE_IN_K * 1024] __attribute__ ((aligned (4)));
size_t const ucHeapSize = configTOTAL_HEAP_SIZE;
#define webbleUuid(uuid) "19b10000" uuid "537e4f6cd104768a1214"
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
ns_ble_service_t webbleService; // Webble Service
float temperature = 0.0;
uint16_t humidity = 2;
// Webble Service Characteristics
ns_ble_characteristic_t webbleTemperature, webbleHumidity, webblePressure;
ns_ble_characteristic_t webbleAccel, webbleGyro, webbleQuat;
ns_ble_characteristic_t webbleRgb, webbleBsec, webbleCo2, webbleGas;
void ns_ble_tests_pre_test_hook() {
    // pre test hook if needed
}

void ns_ble_tests_post_test_hook() {
    // post test hook if needed
}

static int webbleReadHandler(ns_ble_service_t *s, struct ns_ble_characteristic *c, void *dest) {
    // ns_lp_printf("webblehandler\n");
    memcpy(dest, c->applicationValue, c->valueLen);
    return NS_STATUS_SUCCESS;
}

static int webbleWriteHandler(ns_ble_service_t *s, struct ns_ble_characteristic *c, void *src) {
    // ns_lp_printf("webbleWriteHandler value %x\n", *(uint8_t *)src);
    memcpy(c->applicationValue, src, c->valueLen);
    return NS_STATUS_SUCCESS;
}

// Basic test to check if the service is created
void ns_ble_create_service_test() {
    char ble_name[] = "abcabcabcabcabca";
    NS_TRY(ns_ble_char2uuid(webbleUuid("0000"), &(webbleService.uuid128)), "Failed to convert UUID\n");
    memcpy(webbleService.name, ble_name, sizeof(ble_name));
    webbleService.nameLen = sizeof(ble_name) - 1; // exclude null terminator
    webbleService.baseHandle = 0x0800;
    webbleService.numAttributes = 0;
    ns_ble_create_characteristic(
        &webbleTemperature, webbleUuid("2001"), &temperature, sizeof(temperature), NS_BLE_READ,
        &webbleReadHandler, NULL, NULL, 0, false, &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &webbleHumidity, webbleUuid("3001"), &humidity, sizeof(humidity), NS_BLE_READ,
        &webbleReadHandler, NULL, NULL, 0, false, &(webbleService.numAttributes));
    webbleService.poolConfig = &webbleWsfBuffers;
    webbleService.numCharacteristics = 2;
    int status = ns_ble_create_service(&webbleService);
    ns_lp_printf("Service created\n");
    TEST_ASSERT_EQUAL(NS_STATUS_SUCCESS, status);
}

// Service with no characteristics - should fail
void ns_ble_create_service_test_no_characteristics() {
    ns_ble_service_t service;
    char ble_name[] = "abcabcabcabcabca";
    NS_TRY(ns_ble_char2uuid(ble_name, &(service.uuid128)), "Failed to convert UUID\n");
    memcpy(service.name, ble_name, sizeof(ble_name));
    service.nameLen = sizeof(ble_name) - 1; // exclude null terminator
    service.baseHandle = 0x0800;
    service.numAttributes = 0;
    service.numCharacteristics = 0;
    int status = ns_ble_create_service(&service);
    TEST_ASSERT_EQUAL(NS_STATUS_FAILURE, status);
}

// Should return failure if service is NULL
void ns_ble_create_null_service_test() {
    int status = ns_ble_create_service(NULL);
    TEST_ASSERT_EQUAL(NS_STATUS_FAILURE, status);
}

// Should return Failure
void ns_ble_negative_attribute_test() {
    ns_ble_service_t service;
    char ble_name[] = "abcabcabcabcabca";
    NS_TRY(ns_ble_char2uuid(ble_name, &(service.uuid128)), "Failed to convert UUID\n");
    memcpy(service.name, ble_name, sizeof(ble_name));
    service.nameLen = sizeof(ble_name) - 1; // exclude null terminator
    service.baseHandle = 0x0800;
    service.numAttributes = -1;
    service.numCharacteristics = 10;
    int status = ns_ble_create_service(&service);
    TEST_ASSERT_EQUAL(NS_STATUS_FAILURE, status);
}

// Test to check if the service is created with different values
void ns_ble_create_different_service_test() {
    ns_ble_service_t service;
    char customName[] = "differentService";
    NS_TRY(ns_ble_char2uuid(customName, &(service.uuid128)), "Failed to convert UUID\n");
    memcpy(service.name, customName, sizeof(customName));
    service.nameLen = sizeof(customName) - 1; // exclude null terminator
    service.baseHandle = 0x1000;
    service.numAttributes = 0;
    service.numCharacteristics = 5;
    int status = ns_ble_create_service(&service);
    TEST_ASSERT_EQUAL(NS_STATUS_SUCCESS, status);
}

// Test creating and adding one characteristic
void ns_ble_characteristic_test() {
    ns_ble_service_t service;
    ns_ble_characteristic_t temp;
    float temperature = 0.0;
    char ble_name[] = "abcabcabcabcabca";
    NS_TRY(ns_ble_char2uuid(ble_name, &(service.uuid128)), "Failed to convert UUID\n");
    memcpy(service.name, ble_name, sizeof(ble_name));
    service.nameLen = sizeof(ble_name) - 1; // exclude null terminator
    service.baseHandle = 0x0800;
    int status = ns_ble_create_characteristic(
        &temp, ble_name, &temperature, sizeof(temperature), NS_BLE_READ, &webbleReadHandler,
        NULL, NULL, 0, false, &(service.numAttributes));
    TEST_ASSERT_EQUAL(NS_STATUS_SUCCESS, status);
    status = ns_ble_create_service(&service);
    TEST_ASSERT_EQUAL(NS_STATUS_SUCCESS, status);
    service.numAttributes = 0;
    service.numCharacteristics = 1;
    status = ns_ble_add_characteristic(&service, &temp);
    TEST_ASSERT_EQUAL(NS_STATUS_SUCCESS, status);
}

// Test creating and adding multiple characteristics
void ns_ble_multiple_characteristics_test() {
    ns_ble_service_t service;
    ns_ble_characteristic_t temp1, temp2;
    float temperature1 = 0.0, temperature2 = 0.0;
    char ble_name[] = "abcabcabcabcabca";
    NS_TRY(ns_ble_char2uuid(ble_name, &(service.uuid128)), "Failed to convert UUID\n");
    memcpy(service.name, ble_name, sizeof(ble_name));
    service.nameLen = sizeof(ble_name) - 1; // exclude null terminator
    service.baseHandle = 0x0800;

    // Create first characteristic
    int status = ns_ble_create_characteristic(
        &temp1, "characteristic0", &temperature1, sizeof(temperature1), NS_BLE_READ,
        &webbleReadHandler, NULL, NULL, 0, false, &(service.numAttributes));
    TEST_ASSERT_EQUAL(NS_STATUS_SUCCESS, status);

    // Create second characteristic
    status = ns_ble_create_characteristic(
        &temp2, "characteristic1", &temperature2, sizeof(temperature2), NS_BLE_WRITE, NULL,
        &webbleWriteHandler, NULL, 0, false, &(service.numAttributes));
    TEST_ASSERT_EQUAL(NS_STATUS_SUCCESS, status);

    status = ns_ble_create_service(&service);
    TEST_ASSERT_EQUAL(NS_STATUS_SUCCESS, status);

    service.numAttributes = 0;
    service.numCharacteristics = 2; // Number of characteristics being added

    // Add characteristics to the service
    status = ns_ble_add_characteristic(&service, &temp1);
    TEST_ASSERT_EQUAL(NS_STATUS_SUCCESS, status);

    status = ns_ble_add_characteristic(&service, &temp2);
    TEST_ASSERT_EQUAL(NS_STATUS_SUCCESS, status);
}

// Should fail due to characteristic mismatch
void ns_ble_multiple_characteristics_fail_test() {
    ns_ble_service_t service;
    ns_ble_characteristic_t temp1, temp2;
    float temperature1 = 0.0, temperature2 = 0.0;
    char ble_name[] = "abcabcabcabcabca";
    NS_TRY(ns_ble_char2uuid(ble_name, &(service.uuid128)), "Failed to convert UUID\n");
    memcpy(service.name, ble_name, sizeof(ble_name));
    service.nameLen = sizeof(ble_name) - 1; // exclude null terminator
    service.baseHandle = 0x0800;

    // Create first characteristic
    int status = ns_ble_create_characteristic(
        &temp1, "characteristic0", &temperature1, sizeof(temperature1), NS_BLE_READ,
        &webbleReadHandler, NULL, NULL, 0, false, &(service.numAttributes));
    TEST_ASSERT_EQUAL(NS_STATUS_SUCCESS, status);

    // Create second characteristic
    status = ns_ble_create_characteristic(
        &temp2, "characteristic1", &temperature2, sizeof(temperature2), NS_BLE_WRITE, NULL,
        &webbleWriteHandler, NULL, 0, false, &(service.numAttributes));
    TEST_ASSERT_EQUAL(NS_STATUS_SUCCESS, status);

    status = ns_ble_create_service(&service);
    TEST_ASSERT_EQUAL(NS_STATUS_SUCCESS, status);

    service.numAttributes = 0;
    service.numCharacteristics = 1; // Mismatch

    // Add characteristics to the service
    status = ns_ble_add_characteristic(&service, &temp1);
    TEST_ASSERT_EQUAL(NS_STATUS_SUCCESS, status);

    // Attempt to add the second characteristic
    status = ns_ble_add_characteristic(&service, &temp2);
    TEST_ASSERT_EQUAL(NS_STATUS_FAILURE, status);
}

// Empty service when adding characteristic - should fail
void ns_ble_empty_service_add_characteristic_test() {
    ns_ble_service_t service;
    ns_ble_characteristic_t temp1;
    float temperature1 = 0.0;
    char ble_name[] = "abcabcabcabcabca";
    NS_TRY(ns_ble_char2uuid(ble_name, &(service.uuid128)), "Failed to convert UUID\n");
    memcpy(service.name, ble_name, sizeof(ble_name));
    service.nameLen = sizeof(ble_name) - 1; // exclude null terminator
    service.baseHandle = 0x0800;

    // Create first characteristic
    int status = ns_ble_create_characteristic(
        &temp1, "characteristic0", &temperature1, sizeof(temperature1), NS_BLE_READ,
        &webbleReadHandler, NULL, NULL, 0, false, &(service.numAttributes));
    TEST_ASSERT_EQUAL(NS_STATUS_SUCCESS, status);

    service.numAttributes = 0;
    service.numCharacteristics = 1;

    // Add characteristics to the EMPTY service
    status = ns_ble_add_characteristic(NULL, &temp1);
    TEST_ASSERT_EQUAL(NS_STATUS_FAILURE, status);
}

// Test starting a service
void ns_ble_start_service_test() {
    ns_ble_service_t service;
    ns_ble_characteristic_t temp1;
    float temperature1 = 0.0;
    char ble_name[] = "abcabcabcabcabca";
    NS_TRY(ns_ble_char2uuid(ble_name, &(service.uuid128)), "Failed to convert UUID\n");
    memcpy(service.name, ble_name, sizeof(ble_name));
    service.nameLen = sizeof(ble_name) - 1; // exclude null terminator
    service.baseHandle = 0x0800;

    // Create first characteristic
    int status = ns_ble_create_characteristic(
        &temp1, "characteristic0", &temperature1, sizeof(temperature1), NS_BLE_READ,
        &webbleReadHandler, NULL, NULL, 0, false, &(service.numAttributes));
    TEST_ASSERT_EQUAL(NS_STATUS_SUCCESS, status);

    service.numAttributes = 0;
    service.numCharacteristics = 1;

    status = ns_ble_add_characteristic(&service, &temp1);
    TEST_ASSERT_EQUAL(NS_STATUS_SUCCESS, status);

    status = ns_ble_start_service(&service);
    TEST_ASSERT_EQUAL(NS_STATUS_SUCCESS, status);
}

```

