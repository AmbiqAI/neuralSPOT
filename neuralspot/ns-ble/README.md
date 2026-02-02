# neuralSPOT BLE Library Subsystem

This library simplifies the instantiation of BLE (Bluetooth Low Energy) services on bluetooth-enable Ambiq EVBs in order to enable BLE-based demonstration applications. Based on the Cordio BLE software stack, it adds a API and wrapping software to make creating BLE services easier. Features include:

- Easy definition of service behaviors and discoverability
- Easy definition of data elements ("characteristics" in BLE terms) that can be read, written, or subsribed to by BLE clients.
- Easy handling of read, write, and subscription event handling via application-definable callbacks

Current Limitations

- Only 1 service.
- Only 1 connection at a time.
- No "Out of band" connection protocol (in other words, the service will pair with the first client that requests it, rather than asking for a pin or other out-of-band confirmation).

**NOTE**: this library is in experimental phase - it is missing many features such as error handling and disconnections.

## Using NS BLE to Create a BLE Service

Creating a service involves:

1. Creating and configuring basic Service settings
2. Defining BLE "characteristics" (data sources/sinks), including read/write handlers
3. Adding the characteristics to the service
4. Starting the service.

#### A bit of BLE Terminology

A *BLE Server* is a peripheral that serves data (think heartrate monitors, thermometers, etc). Conversely, a *BLE Client* is the device that is interacting with that peripheral (think PC, smartphones, etc).

A *Service* runs on the BLE Server, and is a collection of data sources and sinks (aka *Characteristics*). It has a unique UUID.

A *Characteristic* is a data source (e.g. thermometer reading) or sink (e.g. thermometer units setting) associated with a service.

* There can be many characteristics associated with a Service, but each Characteristic can only be associated with one Service
* A data source can be read from the BLE Client, and a data sink can be written to. A Characteristic may be read-only, write-only, or read-write.
* A third type of Characteristics can be subscribed to by the Client - we call these *Notify* characteristics for simplicity. Once subscribed to, the Server will send value updates to the Client periodically.

### Creating a Service

The code snippet below creates a minimal Service, with a read-only, a read-write, and a notify Characteristic ('webble' refers to WebBLE, but this example can be connected to by any Client). For a full example, see [here](https://github.com/AmbiqAI/neuralSPOT/tree/main/examples/web_ble).

```c
#include "ns_ble.h"
#include "FreeRTOS.h"
#include "task.h"

float temperature = 0.0;          // Read Only
float accel[3] = {0.0, 0.0, 0.0}; // Notify
uint8_t rgb[] = {0, 0, 0};        // Read/Write
float x = 0.0; // mock-up sensor 'time'

/**
 * @brief Dummy up some interesting sensor values
 * (called every time the Notify period expires)
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
    temperature = arm_sin_f32(x / 2);
}

// WSF buffer pools are a bit of black magic. More
// development needed.
#define WEBBLE_WSF_BUFFER_POOLS 4
#define WEBBLE_WSF_BUFFER_SIZE                                                            (WEBBLE_WSF_BUFFER_POOLS * 16 + 16 * 8 + 32 * 4 + 64 * 6 + 280 * 14) / sizeof(uint32_t)

static uint32_t webbleWSFBufferPool[WEBBLE_WSF_BUFFER_SIZE];
static wsfBufPoolDesc_t webbleBufferDescriptors[WEBBLE_WSF_BUFFER_POOLS] = {
    {16, 8}, // 16 bytes, 8 buffers
    {32, 4},
    {64, 6},
    {512, 14}};

// WSF/Cordio is built on top of FreeRTOS. We need to create a task
TaskHandle_t radio_task_handle;
TaskHandle_t my_xSetupTask;

#define webbleUuid(uuid) "19b10000" uuid "537e4f6cd104768a1214"

// BLE Structs, populated by webble_service_init()
ns_ble_service_t webbleService; // Webble Service

// Webble Service Characteristics
ns_ble_characteristic_t webbleTemperature;
ns_ble_characteristic_t webbleAccel;
ns_ble_characteristic_t webbleRgb;

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
    ns_lp_printf("webbleWriteHandler value %x\n", *(uint8_t *)src);
    memcpy(c->applicationValue, src, c->valueLen);
    return NS_STATUS_SUCCESS;
}

/**
 * @brief Handle a notification from the client. Invoked when notification timer expires.
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

/ Must be invoked from the main RadioTask
int webble_service_init(void) {

    char webbleName[] = "Webble";

    // Customize Service-specific fields
    ns_ble_char2uuid(webbleUuid("0000"), &(webbleService.uuid128));
    memcpy(webbleService.name, webbleName, sizeof(webbleService.name));
    webbleService.nameLen = sizeof(webbleName) - 1; // exclude null terminator
    webbleService.baseHandle = 0x0800;
    webbleService.poolConfig = &webbleWsfBuffers;
    webbleService.numAttributes = 0;

    // Define characteristics to add to service.
  	// The ns_ble_create_characteristics parameters are:
  	/*
     c - config struct, populated by this function
     uuidString - a 16-byte UUID string
     applicationValue - a pointer to the application's value store
     valueLength - the length of the value store, in bytes
     properties - a bitmask of properties for the characteristic, from the enum above
     readHandlerCb - a callback function for read requests if the characteristic is readable
     writeHandlerCb - a callback function for write requests i
     notifyHandlerCb - a callback function for notify requests
     periodMs - the period of the notify timer, in milliseconds
     attributeCount - a pointer to the service's attribute count, incremented in function.
     */

    ns_ble_create_characteristic(
        &webbleTemperature, webbleUuid("2001"),
        &temperature, sizeof(temperature), NS_BLE_READ,
        &webbleReadHandler, NULL, NULL, 0, &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &webbleAccel, webbleUuid("5001"), accel, sizeof(accel),
      	NS_BLE_READ | NS_BLE_NOTIFY, NULL,
        NULL, &webbleNotifyHandler, 200, &(webbleService.numAttributes));

    ns_ble_create_characteristic(
        &webbleRgb, webbleUuid("8001"), rgb, sizeof(rgb), NS_BLE_READ | NS_BLE_WRITE,
        &webbleReadHandler, &webbleWriteHandler, NULL, 0, &(webbleService.numAttributes));

    // Create the service
    webbleService.numCharacteristics = 3; // needed to allocate memory for characteristics
    ns_ble_create_service(&webbleService);

    // Add characteristics defined above to the service
    ns_ble_add_characteristic(&webbleService, &webbleTemperature);
    ns_ble_add_characteristic(&webbleService, &webbleAccel);
    ns_ble_add_characteristic(&webbleService, &webbleRgb);

    ns_ble_start_service(&webbleService); // Initialize BLE, create structs, start service
    return NS_STATUS_SUCCESS;
}

void RadioTask(void *pvParameters) {
    webble_service_init();
    while (1) {
        wsfOsDispatcher();
    }
}

void setup_task(void *pvParameters) {
    ns_lp_printf("setup_task\n");
    ns_ble_pre_init(); // Set NVIC priorities
    xTaskCreate(RadioTask, "RadioTask", 512, 0, 3, &radio_task_handle);
    vTaskSuspend(NULL);
    while (1);
}

int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\b");
    am_bsp_low_power_init();
    ns_itm_printf_enable();
    ns_interrupt_master_enable();

    xTaskCreate(setup_task, "Setup", 512, 0, 3, &my_xSetupTask);
    vTaskStartScheduler();

  	while (1);
}
```
