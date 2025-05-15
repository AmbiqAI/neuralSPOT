/**
 * @file ns_imu.c
 * @author Ambiq
 * @brief Library for IMU sensors
 * @version 0.1
 * @date 2025-05-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "ns_imu.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_core.h"

const ns_core_api_t ns_imu_V0_0_1 = {.apiId = NS_IMU_API_ID, .version = NS_IMU_V0_0_1};



// Global variable to hold the IMU configuration
ns_imu_config_t *g_ns_imu_config; // for ISR if needed

// Function to initialize the IMU
uint32_t ns_imu_init(ns_imu_config_t *cfg) {
    // Check if the configuration is valid

    // Set the global configuration
    g_ns_imu_config = cfg;

    // Initialize the IMU sensor based on the selected sensor
    switch (cfg->sensor) {
        case NS_IMU_SENSOR_ICM45605:
            // Initialize the ICM45605 sensor
            ns_imu_ICM45605_init(0);
            break;
        case NS_IMU_SENSOR_MAX86150:
            // Initialize the MAX86150 sensor
            ns_lp_printf("MAX86150 not implemented, use driver directly instead\n");
            break;
        default:
            return NS_STATUS_FAILURE;
    }

    return NS_STATUS_SUCCESS;
}

uint32_t ns_read_sensor_data(ns_imu_config_t *cfg, void *data, uint32_t size) {

    if (cfg == NULL || data == NULL || size != 6) {
        return NS_STATUS_INVALID_HANDLE;
    }
    // Read data from the sensor
    switch (cfg->sensor) {
        case NS_IMU_SENSOR_ICM45605:
            // Read data from the ICM45605 sensor
            ns_imu_ICM45605_read(data, size);
            break;
        case NS_IMU_SENSOR_MAX86150:
            // Read data from the MAX86150 sensor
            ns_lp_printf("MAX86150 not implemented, use driver directly instead\n");
            break;
        default:
            return NS_STATUS_FAILURE
    }

    return NS_STATUS_SUCCESS;
}

// Example of how I want this used

// void minimain() {
//     ns_imu_config_t imu_config = {
//         .api = &ns_imu_V0_0_1,
//         .sensor = NS_IMU_SENSOR_BNO055,
//         .sampleRate = 100,
//         .bufferSize = 256,
//         .callback = my_frame_available_callback, // called with bufferSize samples have been collected
//         .buffer = NULL
//     };

//     // Initialize the IMU
//     ns_imu_init(&imu_config);

//     // Start it
//     ns_imu_start(&imu_config);

//     // Start the IMU
//     while(true) {
//         // Wait for data to be available
//         if (my_frame_available_flag) { // set in the callback
//             // Process the data
//             ns_imu_process_frame(mybuffer);
//             my_frame_available_flag = false; // reset the flag
            
//         }
//     }

// }