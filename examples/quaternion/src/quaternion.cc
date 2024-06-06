/**
 @file quaternion.cc

 @brief Quaternion visualization using NeuralSPOT

**/


// ARM perf requires ITM to be enabled, impacting power measurements.
// For profiling measurements to work, example must be compiled using the MLPROFILE=1 make parameter
#ifdef NS_MLPROFILE
    #define MEASURE_ARM_PERF true
#else
    #define MEASURE_ARM_PERF false
#endif

#include <math.h>

/// NeuralSPOT Includes
#include "ns_energy_monitor.h"
#include "ns_mpu6050_i2c_driver.h"
#include "ns_peripherals_power.h"
#include "ns_rpc_generic_data.h"
#include "mpu6050.h"
#include "har_peripherals.h"

#include <string.h>
#include <cstdlib>
#include <cstring>

#define MPU_AXES 6

// RPC Stuff
#if (configAPPLICATION_ALLOCATED_HEAP == 1)
// RPC uses malloc internally, so we need to declare it here
size_t ucHeapSize = NS_RPC_MALLOC_SIZE_IN_K * 1024;
uint8_t ucHeap[NS_RPC_MALLOC_SIZE_IN_K * 1024] __attribute__((aligned(4)));
#endif

#define MY_USB_RX_BUFSIZE 2048
#define MY_USB_TX_BUFSIZE 2048
static uint8_t my_cdc_rx_ff_buf[MY_USB_RX_BUFSIZE];
static uint8_t my_cdc_tx_ff_buf[MY_USB_TX_BUFSIZE];
// End RPC Stuff

// 3 axes for each of accel and gyro
float g_sensorData[4];

// i2c Config for MPU6050
uint32_t mpuAddr = MPU_I2CADDRESS_AD0_LOW;
ns_i2c_config_t i2cConfig = {.api = &ns_i2c_V1_0_0, .iom = 1};


/**
 * @brief Main quaternion - infinite loop listening and calculating
 *
 * @return int
 */
int main(void) {
    float finalGyro[3];
    float finalAccel[3];
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};

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

   // DataBlock init
    binary_t binaryBlock = {.data = (uint8_t *)g_sensorData,
        .dataLength = 1 * 4 * sizeof(float)};
    char msg_store[19] = "Quaternion Plotter";

    // Block sent to PC
    dataBlock outBlock = {.length = 1 * 4 * sizeof(float),
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


    ns_lp_printf("This Quaternion example repeatedly collects MPU data, then performs sensor fusion\n");
    ns_lp_printf("to derive the corresponding quaternion values\n");


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
    ns_lp_printf("Calibration Successful. Sending samples over RPC now...\n");
    buttonPressed = false;

    // Event loop
    while (1) {
        ns_set_power_monitor_state(NS_DATA_COLLECTION);
        buttonPressed = false;
        int16_t accelVals[MPU_AXES];
        int16_t gyroVals[MPU_AXES];
        mpu6050_get_accel_values(&i2cConfig, mpuAddr, &accelVals[0], &accelVals[1], &accelVals[2]);
        mpu6050_get_gyro_values(&i2cConfig, mpuAddr, &gyroVals[0], &gyroVals[1], &gyroVals[2]);
        // convert accelerometer to gravity units and gyro to degrees per second
        for (int axis = 0; axis < (MPU_AXES / 2); axis++) {
            finalGyro[axis] = mpu6050_gyro_to_deg_per_sec(gyroVals[axis], GYRO_FS_500DPS);
            finalAccel[axis] = mpu6050_accel_to_gravity(accelVals[axis], ACCEL_FS_4G);
        }
        // convert from deg/s to rad/s
        for(int i = 0; i < 3; i++) {
            finalGyro[i] *= 0.0174533;
        }
        ns_lp_printf("accel values: %f, %f, %f\n", finalAccel[0], finalAccel[1], finalAccel[2]);
        ns_lp_printf("gyro values: %f, %f, %f\n",finalGyro[0], finalGyro[1], finalGyro[2]);
        
        // calculate and update quaternion values
        mpu6050_mahonyUpdate(finalGyro[0], finalGyro[1], finalGyro[2], finalAccel[0], finalAccel[1], finalAccel[2]);

        double qw, qx, qy, qz;
        mpu6050_getQuaternion(&qw, &qx, &qy, &qz);
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
        ns_lp_printf(".\n");
        ns_rpc_data_sendBlockToPC(&outBlock);

    }
}