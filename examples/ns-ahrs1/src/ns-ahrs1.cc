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
#include "ns_usb.h"
#include <string.h>

typedef enum { WAITING_TO_START_RPC_SERVER, WAITING_TO_RECORD} myState_e;

// HAR is trained to detect activity over a 10s window of 6 axis (accel+gyro) data, sampled at 20Hz
#define MPU_FRAME_SIZE 200
#define MPU_AXES 6
#define FILTER_UPDATE_RATE_HZ 100
float g_sensorData[MPU_FRAME_SIZE][MPU_AXES]; // 3 axes for each of accel and gyro
float g_sensorMean[MPU_AXES];
float g_sensorStd[MPU_AXES];
constexpr size_t kCategoryCount = 6;
const char *kCategoryLabels[kCategoryCount] = {"Walking", "Jogging", "Stairs", "Sitting",
                                               "Standing"};

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
uint32_t
collect_mpu_frame(ns_i2c_config_t *i2cConfig, int samples, uint32_t delay) {
    // Collects a frame of MPU data and puts it in g_sensorData global
    // Calculates the mean for each axis and we go along
    int16_t accelVals[MPU_AXES];
    int16_t gyroVals[MPU_AXES];
    memset(g_sensorMean, 0, sizeof(g_sensorMean));
    for (int sample = 0; sample < samples; sample++) {
        mpu6050_get_accel_values(i2cConfig, mpuAddr, &accelVals[0], &accelVals[1], &accelVals[2]);
        ns_lp_printf("accel values: %f, %f, %f\n", accelVals[0], accelVals[1], accelVals[2]);
        mpu6050_get_gyro_values(i2cConfig, mpuAddr, &gyroVals[0], &gyroVals[1], &gyroVals[2]);
        // Capture data in RPC buffer
        for (int axis = 0; axis < (MPU_AXES / 2); axis++) {
            g_sensorData[sample][axis] = mpu6050_accel_to_gravity(accelVals[axis], ACCEL_FS_4G);
            g_sensorData[sample][axis + MPU_AXES / 2] =
                mpu6050_gyro_to_deg_per_sec(gyroVals[axis], GYRO_FS_500DPS);
            g_sensorMean[axis] += g_sensorData[sample][axis];
            g_sensorMean[axis + MPU_AXES / 2] += g_sensorData[sample][axis + MPU_AXES / 2];
        }
        ns_delay_us(delay);
    }

    return NS_STATUS_SUCCESS;
}

/**
 * @brief Main HAR - infinite loop listening and inferring
 *
 * @return int
 */
int
main(void) {
    // float tmp = 0;
    // float output[kCategoryCount];
    // uint8_t output_max = 0;
    // float max_val = 0.0;
    // bool measure_first_inference = MEASURE_ARM_PERF;
    // ns_perf_counters_t pp;
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    myState_e state = WAITING_TO_RECORD;

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

    NS_TRY(ns_peripheral_button_init(&button_config), "Button initialization failed.\n")

    ns_interrupt_master_enable();

    // Calibrate the MPU
    ns_lp_printf("Please avoid moving sensor until calibration is finished (~20s).\n");
    ns_lp_printf("Press Button 0 to begin calibration\n");
    // while (buttonPressed == 0) {
    //     ns_deep_sleep();
    // }
    // buttonPressed = false;
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

#ifdef RPC_ENABLED
    NS_TRY(ns_rpc_genericDataOperations_init(&rpcConfig), "RPC Init Failed\n"); // init RPC and USB
    ns_lp_printf("Start the PC-side server, then press Button 0 to get started\n");
#else
    ns_lp_printf("Calibration successful. Press Button 0 to start classifying...\n");
    state = WAITING_TO_RECORD;
    buttonPressed = false;
    nxp_begin_args sampleFrequency;
    sampleFrequency.sampleFrequency = FILTER_UPDATE_RATE_HZ;
    nxp_begin(&filter, sampleFrequency);

#endif

    // Event loop
    while (1) {
        switch (state) {
        case WAITING_TO_START_RPC_SERVER:
            if (buttonPressed) {
                state = WAITING_TO_RECORD;
                ns_lp_printf("Press Button 0 to start recording...\n");
                buttonPressed = false;
            }
            break;

        case WAITING_TO_RECORD:
            if (1 /*buttonPressed*/) {
                float roll, pitch, heading;
                ns_delay_us(100000); // delay 1000 ms
                // state = INFERING;
                ns_set_power_monitor_state(NS_DATA_COLLECTION);
                buttonPressed = false;
                int16_t accelVals[MPU_AXES];
                int16_t gyroVals[MPU_AXES];
                mpu6050_get_accel_values(&i2cConfig, mpuAddr, &accelVals[0], &accelVals[1], &accelVals[2]);
                mpu6050_get_gyro_values(&i2cConfig, mpuAddr, &gyroVals[0], &gyroVals[1], &gyroVals[2]);
                // Capture data in RPC buffer
                for (int axis = 0; axis < (MPU_AXES / 2); axis++) {
                    g_sensorData[0][axis + MPU_AXES / 2] =
                        mpu6050_gyro_to_deg_per_sec(gyroVals[axis], GYRO_FS_500DPS);
                }
                ns_lp_printf("accel values: %d, %d, %d\n", accelVals[0], accelVals[1], accelVals[2]);
                ns_lp_printf("gyro values: %f, %f, %f\n", g_sensorData[0][3], g_sensorData[0][4], g_sensorData[0][5]);
                nxp_update(&filter, -229.651779, 44.419689, -25.818659, 713, 6726, 1954, 1, 1, 1);
                // nxp_update(&filter, g_sensorData[0][3], g_sensorData[0][4], g_sensorData[0][5], accelVals[0], accelVals[1], accelVals[2], 1.0, 1.0, 1.0);
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
                ns_deep_sleep();
                // // different mag values
                // float new_roll = nxp_getRoll(&new_filter);
                // float new_pitch = nxp_getPitch(&new_filter);
                // float new_heading = nxp_getYaw(&new_filter);
                // ns_lp_printf("New Orientation: ");
                // ns_lp_printf("%f, ", new_heading);
                // ns_lp_printf("%f, ", new_pitch);
                // ns_lp_printf("%f\n", new_roll);

                // float qw1, qx1, qy1, qz1;
                // nxp_getQuaternion(&new_filter, &qw1, &qx1, &qy1, &qz1);
                // ns_lp_printf("New Quaternion: ");
                // ns_lp_printf("%f,", qw1);
                // ns_lp_printf("%f,",qx1);
                // ns_lp_printf("%f,", qy1);
                // ns_lp_printf("%f\n", qz1);  

            }
            break;

// #ifdef NS_MLPROFILE
//             profiler->LogCsv();
// #endif

//             if (measure_first_inference) {
//                 measure_first_inference = false;
//                 ns_stop_perf_profiler();
//                 ns_capture_perf_profiler(&pp);
//                 ns_print_perf_profile(&pp);
//             }
//             ns_set_power_monitor_state(NS_IDLE);

//             if (invoke_status != kTfLiteOk) {
//                 ns_lp_printf("Invoke failed\n");
//                 while (1) {
//                 }; // hang
//             }

//             max_val = 0.0;
//             for (uint8_t i = 0; i < kCategoryCount; i = i + 1) {
//                 output[i] = (model_output->data.int8[i] - model_output->params.zero_point) *
//                             model_output->params.scale;
//                 if (output[i] > 0.3) {
//                     ns_lp_printf("\n[%s] with %d%% certainty\n", kCategoryLabels[i],
//                                  (uint8_t)(output[i] * 100));
//                 }

//                 if (output[i] > max_val) {
//                     max_val = output[i];
//                     output_max = i;
//                 }
//             }

//             ns_lp_printf("\n**** Most probably: [%s]\n\n", kCategoryLabels[output_max]);
//             ns_lp_printf("Press Button 0 to start listening...\n");

//             buttonPressed = false; // thoroughly debounce the button
//             state = WAITING_TO_RECORD;
//             break;
//         }
        // if (state != INFERING)
        //     ns_deep_sleep();
    } 
}
}