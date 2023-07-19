/**
 * @file mpu_data_collection.cc
 * @author Carlos Morales
 * @brief Reads MPU data and sends it to PC using RPC
 * @version 0.1
 * @date 2022-08-26
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <cstdlib>
#include <cstring>

#include "ns_ambiqsuite_harness.h"
#include "ns_core.h"
#include "ns_i2c_register_driver.h"
#include "ns_mpu6050_i2c_driver.h"
#include "ns_peripherals_button.h"
#include "ns_peripherals_power.h"
#include "ns_rpc_generic_data.h"

#define NUMSAMPLES 32
#define AXES 3
float g_sensorData[NUMSAMPLES * 2][7]; // 32 samples of gryo, accel and temp

ns_i2c_config_t i2cConfig = {.api = &ns_i2c_V1_0_0, .iom = 1};

uint32_t mpuAddr = MPU_I2CADDRESS_AD0_LOW;

// RPC Stuff
#define MY_USB_RX_BUFSIZE 2048
#define MY_USB_TX_BUFSIZE 2048
static uint8_t my_cdc_rx_ff_buf[MY_USB_RX_BUFSIZE];
static uint8_t my_cdc_tx_ff_buf[MY_USB_TX_BUFSIZE];
// End RPC Stuff

int main(void) {
    uint16_t sample = 0;
    int16_t temperatureVal;
    int16_t accelVals[AXES];
    int16_t gyroVals[AXES];
    uint32_t axis;
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};

    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\b");
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed\n");
    ns_itm_printf_enable();
    ns_interrupt_master_enable();

    // DataBlock init
    binary_t binaryBlock = {
        .data = (uint8_t *)g_sensorData, // point this to audio buffer
        .dataLength = NUMSAMPLES * 7 * sizeof(float)};
    char msg_store[30] = "MPU6050-Data-to-CSV";

    // Block sent to PC
    dataBlock outBlock = {
        .length = NUMSAMPLES * 7 * sizeof(float),
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
    NS_TRY(ns_rpc_genericDataOperations_init(&rpcConfig), "RPC Init Failed\n"); // inits RPC and USB

    // Button global - will be set by neuralSPOT button helper
    int volatile g_intButtonPressed = 0;

    // Button Peripheral Config Struct
    ns_button_config_t button_config = {
        .api = &ns_button_V1_0_0,
        .button_0_enable = true,
        .button_1_enable = false,
        .button_0_flag = &g_intButtonPressed,
        .button_1_flag = NULL};
    NS_TRY(ns_peripheral_button_init(&button_config), "Button init failed\n");

    ns_lp_printf("Please avoid moving sensor until calibration is finished (~20s).\n");
    ns_lp_printf("Press Button 0 to begin calibration\n");
    while (g_intButtonPressed == 0) {
        ns_deep_sleep();
    }
    g_intButtonPressed = 0;
    ns_lp_printf("Running calibration...\n");

    mpu6050_config_t mpu_config = {
        .clock_src = CLOCK_GZ_PLL,
        .dlpf_cfg = DLPF_044HZ,
        .gyro_fullscale_range = GYRO_FS_500DPS,
        .accel_fullscale_range = ACCEL_FS_4G,
        .sample_rate = 100,
        .sleep_cfg = 0};

    NS_TRY(ns_i2c_interface_init(&i2cConfig, 100000), "i2c Interface Init Failed.\n");
    NS_TRY(mpu6050_init(&i2cConfig, &mpu_config, mpuAddr), "MPU6050 Init Failed.\n");
    NS_TRY(mpu6050_calibrate(&i2cConfig, mpuAddr), "MPU6050 Calibration Failed.\n");
    ns_lp_printf("Calibration Successful. Sending samples over RPC now...\n");

    while (1) {
        mpu6050_get_accel_values(&i2cConfig, mpuAddr, &accelVals[0], &accelVals[1], &accelVals[2]);
        mpu6050_get_temperature(&i2cConfig, mpuAddr, &temperatureVal);
        mpu6050_get_gyro_values(&i2cConfig, mpuAddr, &gyroVals[0], &gyroVals[1], &gyroVals[2]);
        // Capture data in RPC buffer
        for (axis = 0; axis < AXES; axis++) {
            g_sensorData[sample][axis] = mpu6050_accel_to_gravity(accelVals[axis], ACCEL_FS_4G);
            g_sensorData[sample][axis + AXES] =
                mpu6050_gyro_to_deg_per_sec(gyroVals[axis], GYRO_FS_500DPS);
        }
        g_sensorData[sample][6] = mpu6050_temperature_to_celsius(temperatureVal);

        // Send to PC every NUMSAMPLES have been captured
        sample = (sample + 1) % NUMSAMPLES;
        if (sample == 0) {
            ns_lp_printf(".");
            ns_rpc_data_sendBlockToPC(&outBlock);
        }
        ns_delay_us(5000);
    }
}
