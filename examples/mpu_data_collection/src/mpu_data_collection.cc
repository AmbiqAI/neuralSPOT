/**
 * @file har.cc
 * @author Carlos Morales
 * @brief HAR, but just i2c invocation for now
 * @version 0.1
 * @date 2022-08-26
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <cstdlib>
#include <cstring>

#include "ns_ambiqsuite_harness.h"
#include "ns_i2c_register_driver.h"
#include "ns_mpu6050_i2c_driver.h"
#include "ns_peripherals_button.h"
#include "ns_rpc_generic_data.h"
// #include "ns_usb.h"
#include "ns_peripherals_power.h"

#define NUMSAMPLES 32
#define AXES 3
float g_sensorData[NUMSAMPLES * 2][7]; // 32 samples of gryo, accel and temp

ns_i2c_config_t i2cConfig = {.iom = 1};

uint32_t mpuAddr = MPU_I2CADDRESS_AD0_LOW;

uint32_t
mpu6050_init(ns_i2c_config_t *cfg, uint32_t devAddr) {
    if (mpu6050_device_reset(cfg, devAddr) ||
        mpu6050_set_clock_source(cfg, devAddr, CLOCK_GZ_PLL) ||
        mpu6050_set_lowpass_filter(cfg, devAddr, DLPF_044HZ) ||
        mpu6050_set_gyro_full_scale(cfg, devAddr, GYRO_FS_500DPS) ||
        mpu6050_set_accel_full_scale(cfg, devAddr, ACCEL_FS_4G) ||
        mpu6050_set_sample_rate(cfg, devAddr, 100) || mpu6050_set_sleep(cfg, devAddr, 0)) {
        return MPU6050_STATUS_ERROR;
    }

    return MPU6050_STATUS_SUCCESS;
}

int
main(void) {
    uint16_t sample = 0;
    int16_t temperatureVal;
    int16_t accelVals[AXES];
    int16_t gyroVals[AXES];
    uint32_t axis;

    ns_itm_printf_enable();
    ns_debug_printf_enable();
    am_hal_interrupt_master_enable();

    // DataBlock init
    binary_t binaryBlock = {.data = (uint8_t *)g_sensorData, // point this to audio buffer
                            .dataLength = NUMSAMPLES * 7 * sizeof(float)};
    char msg_store[30] = "MPU6050-Data-to-CSV";

    // Block sent to PC
    dataBlock outBlock = {.length = NUMSAMPLES * 7 * sizeof(float),
                          .dType = float32_e,
                          .description = msg_store,
                          .cmd = write_cmd,
                          .buffer = binaryBlock};

    // Initialize the Generic RPC Client interface
    ns_rpc_config_t rpcConfig = {.mode = NS_RPC_GENERICDATA_CLIENT,
                                 .sendBlockToEVB_cb = NULL,
                                 .fetchBlockFromEVB_cb = NULL,
                                 .computeOnEVB_cb = NULL};
    ns_rpc_genericDataOperations_init(&rpcConfig); // inits RPC and USB

    // Button global - will be set by neuralSPOT button helper
    int g_intButtonPressed = 0;

    // Button Peripheral Config Struct
    ns_button_config_t button_config = {.button_0_enable = true,
                                        .button_1_enable = false,
                                        .button_0_flag = &g_intButtonPressed,
                                        .button_1_flag = NULL};
    ns_peripheral_button_init(&button_config);

    ns_printf("Press Button 0 to begin calibration\n");
    ns_printf("Please place your sensor on a flat surface until calibration is finished\n");
    while (g_intButtonPressed == 0) {
        ns_delay_us(1000);
    }
    g_intButtonPressed = 0;

    ns_i2c_interface_init(&i2cConfig, 100000);
    mpu6050_init(&i2cConfig, mpuAddr);
    // TODO: Calibration doesnt work for Y and Z accel axis. Also too slow to converge
    // ns_printf("Beginning MPU Calibration..\n");
    // if (mpu6050_calibration(&i2cConfig, mpuAddr)) {
    //     ns_printf("Calibration Failed!\n");
    //     return 1;
    // }
    // ns_printf("MPU Calibration successful. Sampling data...\n");

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
            ns_rpc_data_sendBlockToPC(&outBlock);
        }
        ns_delay_us(5000);
    }
}
