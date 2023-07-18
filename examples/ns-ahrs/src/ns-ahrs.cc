/**
 @file har.cc

 @brief Human Activity Recognition using NeuralSPOT

 HAR is a model trained using the scripts found here:
https://github.com/AmbiqAI/Human-Activity-Recognition

**/

// #define RPC_ENABLED
// #define ENERGY_MONITOR_ENABLE
// #define LOWEST_POWER_MODE

// ARM perf requires ITM to be enabled, impacting power measurements.
// For profiling measurements to work, example must be compiled using the MLPROFILE=1 make parameter
#ifdef NS_MLPROFILE
    #define MEASURE_ARM_PERF true
#else
    #define MEASURE_ARM_PERF false
#endif

#include <math.h>

// #include "har.h"
// #include "har_model.h"
#include "har_peripherals.h"
#include "Adafruit_AHRS_NXPFusion.h"
#include "Adafruit_AHRS_NXPFusion.c"
/// NeuralSPOT Includes
#include "ns_ambiqsuite_harness.h"
#include "ns_core.h"
#include "ns_energy_monitor.h"
#include "ns_i2c_register_driver.h"
#include "ns_mpu6050_i2c_driver.h"
#include "ns_perf_profile.h"
#include "ns_peripherals_power.h"
#include "ns_peripherals_button.h"
#include "ns_rpc_generic_data.h"
#include "ns_usb.h"
#include <string.h>
#include <cstdlib>
#include <cstring>

// typedef enum { WAITING_TO_START_RPC_SERVER, WAITING_TO_RECORD} myState_e;

// HAR is trained to detect activity over a 10s window of 6 axis (accel+gyro) data, sampled at 20Hz
#define MPU_FRAME_SIZE 200
#define MPU_AXES 6
#define FILTER_UPDATE_RATE_HZ 100
#define NUMSAMPLES 10
// RPC Stuff
#define MY_USB_RX_BUFSIZE 2048
#define MY_USB_TX_BUFSIZE 2048
static uint8_t my_cdc_rx_ff_buf[MY_USB_RX_BUFSIZE];
static uint8_t my_cdc_tx_ff_buf[MY_USB_TX_BUFSIZE];
// End RPC Stuff
float g_sensorData[7]; // 3 axes for each of accel and gyro
float g_sensorMean[MPU_AXES];
float g_sensorStd[MPU_AXES];

// i2c Config for MPU6050
uint32_t mpuAddr = MPU_I2CADDRESS_AD0_LOW;
ns_i2c_config_t i2cConfig = {.api = &ns_i2c_V1_0_0, .iom = 1};
Adafruit_NXPSensorFusion filter;
/**
 * @brief Collect a frame of data, and perform mean and std deviation calculation afterwards
 *
 * @param i2cConfig
 * @param samples
 * @param delay
 * @return uint32_t
 */

/**
 * @brief Main HAR - infinite loop listening and inferring
 *
 * @return int
 */
int
main(void) {
    uint16_t sample = 0;
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    // myState_e state = WAITING_TO_RECORD;

    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");

#ifdef ENERGY_MONITOR_ENABLE
    ns_init_power_monitor_state();
#endif
    ns_set_power_monitor_state(NS_IDLE); // no-op if ns_init_power_monitor_state not called
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed.\n");
    NS_TRY(ns_set_performance_mode(NS_MINIMUM_PERF), "Set CPU Perf mode failed.");

#ifndef LOWEST_POWER_MODE
    ns_itm_printf_enable();
#endif

#ifdef NS_MLPROFILE
    NS_TRY(ns_timer_init(&basic_tickTimer), "Timer init failed.\n");
#endif
    // model_init();
   // DataBlock init
    binary_t binaryBlock = {.data = (uint8_t *)g_sensorData, // point this to audio buffer
                            .dataLength = 1 * 7 * sizeof(float)};
    char msg_store[30] = "MPU6050-Data-to-CSV";

    // Block sent to PC
    dataBlock outBlock = {.length = 1 * 7 * sizeof(float),
                          .dType = float32_e,
                          .description = msg_store,
                          .cmd = write_cmd,
                          .buffer = binaryBlock};

    // Initialize the Generic RPC Client interface
    ns_rpc_config_t rpcConfig = {
        .api = &ns_rpc_gdo_V1_0_0,
        .mode = NS_RPC_GENERICDATA_CLIENT,
        .rx_buf = my_cdc_rx_ff_buf,
        .rx_bufLength = MY_USB_RX_BUFSIZE,
        .tx_buf = my_cdc_tx_ff_buf,
        .tx_bufLength = MY_USB_TX_BUFSIZE,
        .sendBlockToEVB_cb = NULL,
        .fetchBlockFromEVB_cb = NULL,
        .computeOnEVB_cb = NULL};
    NS_TRY(ns_rpc_genericDataOperations_init(&rpcConfig), "RPC Init Failed\n"); // init RPC and USB

    // Button global - will be set by neuralSPOT button helper
    int volatile g_intButtonPressed = 0;

    // Button Peripheral Config Struct
    ns_button_config_t button_config = {.api = &ns_button_V1_0_0,
                                        .button_0_enable = true,
                                        .button_1_enable = false,
                                        .button_0_flag = &g_intButtonPressed,
                                        .button_1_flag = NULL};
    NS_TRY(ns_peripheral_button_init(&button_config), "Button init failed\n");
    ns_interrupt_master_enable();

    // Calibrate the MPU
    ns_lp_printf("Please avoid moving sensor until calibration is finished (~20s).\n");
    ns_lp_printf("Press Button 0 to begin calibration\n");
    while (g_intButtonPressed == 0) {
        ns_deep_sleep();
    }
    g_intButtonPressed = false;
    ns_lp_printf("Running calibration...\n");

    mpu6050_config_t mpu_config = {.clock_src = CLOCK_GZ_PLL,
                                   .dlpf_cfg = DLPF_044HZ,
                                   .gyro_fullscale_range = GYRO_FS_500DPS,
                                   .accel_fullscale_range = ACCEL_FS_4G,
                                   .sample_rate = 20,
                                   .sleep_cfg = 0};

    NS_TRY(ns_i2c_interface_init(&i2cConfig, 100000), "i2c Interface Init Failed.\n");
    NS_TRY(mpu6050_init(&i2cConfig, &mpu_config, mpuAddr), "MPU6050 Init Failed.\n");
    NS_TRY(mpu6050_calibrate(&i2cConfig, mpuAddr), "MPU6050 Calibration Failed.\n");

// #ifdef RPC_ENABLED
    // ns_lp_printf("Start the PC-side server, then press Button 0 to get started\n");
// #else
    ns_lp_printf("Calibration Successful. Sending samples over RPC now...\n");
    // state = WAITING_TO_RECORD;
    buttonPressed = false;
    nxp_begin_args sampleFrequency;
    sampleFrequency.sampleFrequency = FILTER_UPDATE_RATE_HZ;
    nxp_begin(&filter, sampleFrequency);

// #endif

    // Event loop
    while (1) {
        // switch (state) {
        // case WAITING_TO_START_RPC_SERVER:
        //     if (buttonPressed) {
        //         state = WAITING_TO_RECORD;
        //         ns_lp_printf("Press Button 0 to start recording...\n");
        //         buttonPressed = false;
        //     }
        //     break;

        // case WAITING_TO_RECORD:
        //     if (buttonPressed) {
                float roll, pitch, heading;
                ns_delay_us(100000); // delay 1000 ms
                // state = INFERING;
                ns_set_power_monitor_state(NS_DATA_COLLECTION);
                buttonPressed = false;
                int16_t accelVals[MPU_AXES];
                int16_t gyroVals[MPU_AXES];
                mpu6050_get_accel_values(&i2cConfig, mpuAddr, &accelVals[0], &accelVals[1], &accelVals[2]);
                mpu6050_get_gyro_values(&i2cConfig, mpuAddr, &gyroVals[0], &gyroVals[1], &gyroVals[2]);
                for (int axis = 0; axis < (MPU_AXES / 2); axis++) {
                    // g_sensorData[0][axis + MPU_AXES / 2] =
                    //     mpu6050_gyro_to_deg_per_sec(gyroVals[axis], GYRO_FS_500DPS);
                    gyroVals[axis] = mpu6050_gyro_to_deg_per_sec(gyroVals[axis], GYRO_FS_500DPS);
                }
                ns_lp_printf("accel values: %d, %d, %d\n", accelVals[0], accelVals[1], accelVals[2]);
                ns_lp_printf("gyro values: %f, %f, %f\n", gyroVals[0], gyroVals[1], gyroVals[2]);
                nxp_update(&filter, gyroVals[0], gyroVals[1], gyroVals[2], accelVals[0], accelVals[1], accelVals[2], 1, 1, 1);
                // print the heading, pitch and roll
                roll = nxp_getRoll(&filter);
                pitch = nxp_getPitch(&filter);
                heading = nxp_getYaw(&filter);
                ns_lp_printf("Orientation: ");
                ns_lp_printf("%f, ", heading);
                ns_lp_printf("%f, ", pitch);
                ns_lp_printf("%f\n", roll);

                float qw, qx, qy, qz;
                nxp_getQuaternion(&filter, &qw, &qx, &qy, &qz);
                ns_lp_printf("Quaternion: ");
                ns_lp_printf("%f,", qw);
                ns_lp_printf("%f,",qx);
                ns_lp_printf("%f,", qy);
                ns_lp_printf("%f\n", qz); 
                // Capture data in RPC buffer
                g_sensorData[0] = qw;
                g_sensorData[1] = qx;
                g_sensorData[2] = qy;
                g_sensorData[3] = qz;
                g_sensorData[4] = heading;
                g_sensorData[5] = pitch;
                g_sensorData[6] = roll;
                // Send to PC every NUMSAMPLES have been captured
                sample = (sample + 1) % NUMSAMPLES;
                if (sample == 0) {
                    ns_lp_printf(".\n");
                    ns_rpc_data_sendBlockToPC(&outBlock);
                }
                ns_delay_us(5000);
        //     }
        //     break;
        // } 
    }
}