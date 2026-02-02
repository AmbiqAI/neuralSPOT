/**
 * @file quaternion.cc
 * @author Evan Chen
 * @brief Quaternion visualization using neuralSPOT with ICM IMU
 * @version 0.3
 * @date 2025-06-09
 *
 * @copyright Copyright (c) 2024
 */

// #include <math.h>
// #include <string.h>
// #include <cstdlib>
// #include <cstring>

// neuralSPOT Includes
#include "ns_peripherals_power.h"
#include "ns_peripherals_button.h"

#include "ns_rpc_generic_data.h"
#include "quaternion.h"

// ICM IMU Includes
#include "ns_ambiqsuite_harness.h"
#include "ns_core.h"
#include "ns_imu.h"
#include "imu/inv_imu_driver.h"

#define MY_USB_RX_BUFSIZE 2048
#define MY_USB_TX_BUFSIZE 2048
static uint8_t my_cdc_rx_ff_buf[MY_USB_RX_BUFSIZE];
static uint8_t my_cdc_tx_ff_buf[MY_USB_TX_BUFSIZE];

// Buffer to hold output quaternion (qw, qx, qy, qz)
static float g_sensorData[4];
// Flag set by ISR-based callback to indicate new data
static volatile int imuDataReady = 0;

// RPC Stuff
#if (configAPPLICATION_ALLOCATED_HEAP == 1)
size_t ucHeapSize = NS_RPC_MALLOC_SIZE_IN_K * 1024;
uint8_t ucHeap[NS_RPC_MALLOC_SIZE_IN_K * 1024] __attribute__((aligned(4)));
#endif

// Button global - will be set by neuralSPOT button helper
static int volatile buttonPressed = 0;

// Button Peripheral Config Struct
ns_button_config_t button_config = {.api = &ns_button_V1_0_0,
                                    .button_0_enable = true,
                                    .button_1_enable = false,
                                    .button_0_flag = &buttonPressed,
                                    .button_1_flag = NULL};

// RPC DataBlock for sending quaternions
static binary_t binaryBlock = {.data = (uint8_t *)g_sensorData,
    .dataLength = 4 * sizeof(float)};
static char msg_store[19] = "Quaternion Plotter";
static dataBlock outBlock = {
    .length = 4 * sizeof(float),
    .dType = float32_e,
    .description = msg_store,
    .cmd = write_cmd,
    .buffer = binaryBlock
};

// Mahony filter config
static ns_mahony_cfg_t mahony_cfg;
// IMU frame buffer (single sample)
static ns_imu_sensor_data_t imu_frame[1];

/**
 * @brief IMU frame callback (ISR context): process data and flag ready
 */
void imu_frame_cb(void *arg) {
    ns_imu_sensor_data_t *frame = &imu_frame[0];

    // Convert gyro DPS to rad/s
    float gx = frame->gyro_dps[0] * 0.0174533f;
    float gy = frame->gyro_dps[1] * 0.0174533f;
    float gz = frame->gyro_dps[2] * 0.0174533f;
    // Accelerometer data in g
    float ax = frame->accel_g[0];
    float ay = frame->accel_g[1];
    float az = frame->accel_g[2];

    // Perform Mahony update
    NS_TRY(ns_mahony_update(&mahony_cfg, gx, gy, gz, ax, ay, az), "Mahony update failed.\n");

    // Retrieve quaternion
    double qw, qx, qy, qz;
    NS_TRY(ns_get_quaternion(&mahony_cfg, &qw, &qx, &qy, &qz), "Get quaternion failed.\n");

    // Store into buffer
    g_sensorData[0] = (float)qw;
    g_sensorData[1] = (float)qx;
    g_sensorData[2] = (float)qy;
    g_sensorData[3] = (float)qz;

    // Signal main loop that data is ready
    imuDataReady = 1;
}

int main(void) {
    // Core & Power init
    ns_core_config_t core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&core_cfg), "Core init failed.\n");
    NS_TRY(ns_power_config(&ns_development_default), "Power init failed.\n");
    ns_itm_printf_enable();

    NS_TRY(ns_set_performance_mode(NS_MINIMUM_PERF), "Set CPU perf mode failed.");
    NS_TRY(ns_peripheral_button_init(&button_config), "Button init failed\n");
    ns_lp_printf("Quaternion example with ICM IMU running...\n");
    buttonPressed = false;
    ns_interrupt_master_enable();

    // Mahony filter init
    mahony_cfg.api = &ns_mahony_V0_0_1;
    NS_TRY(ns_mahony_init(&mahony_cfg), "Mahony init failed.\n");

    // Initialize Generic RPC Client
    ns_rpc_config_t rpcCfg = {
        .api = &ns_rpc_gdo_V1_0_0,
        .mode = NS_RPC_GENERICDATA_CLIENT,
        .rx_buf = my_cdc_rx_ff_buf,
        .rx_bufLength = MY_USB_RX_BUFSIZE,
        .tx_buf = my_cdc_tx_ff_buf,
        .tx_bufLength = MY_USB_TX_BUFSIZE,
        .sendBlockToEVB_cb = NULL,
        .fetchBlockFromEVB_cb = NULL,
        .computeOnEVB_cb = NULL
    };
    NS_TRY(ns_rpc_genericDataOperations_init(&rpcCfg), "RPC init failed.\n");

    // Configure ICM IMU
    ns_imu_config_t imu_cfg = {
        .api = &ns_imu_V1_0_0,
        .sensor = NS_IMU_SENSOR_ICM45605,
        .iom    = 0,
        .accel_fsr = ACCEL_CONFIG0_ACCEL_UI_FS_SEL_4_G,
        .gyro_fsr  = GYRO_CONFIG0_GYRO_UI_FS_SEL_2000_DPS,
        .accel_odr = ACCEL_CONFIG0_ACCEL_ODR_50_HZ,
        .gyro_odr  = GYRO_CONFIG0_GYRO_ODR_50_HZ,
        .accel_ln_bw = IPREG_SYS2_REG_131_ACCEL_UI_LPFBW_DIV_4,
        .gyro_ln_bw  = IPREG_SYS1_REG_172_GYRO_UI_LPFBW_DIV_4,
        .calibrate = true,
        .frame_available_cb = imu_frame_cb,
        .frame_size = 1,
        .frame_buffer = imu_frame
    };

    ns_lp_printf("Please avoid moving sensor until calibration is finished (up to 20s).\n");
    ns_lp_printf("Start PC-server application, then press Button 0 to begin calibration\n");

    // Wait for button press to start calibration
    while (buttonPressed == 0) {
        ns_deep_sleep();
    }
    buttonPressed = false;
    ns_lp_printf("Calibration started...\n");
    ns_rpc_data_remotePrintOnPC("IMU calibrating - keep still until complete.\n");
    NS_TRY(ns_imu_configure(&imu_cfg), "IMU init failed.\n");
    ns_rpc_data_remotePrintOnPC("IMU configured and calibrated.\n");

    ns_lp_printf("Sending quaternions to PC...\n");

    // Main loop: poll for new data and send via RPC
    while (1) {
        if (imuDataReady) {
            // ns_delay_us(1000); // Allow time for Mahony update to complete
            imuDataReady = 0;
            ns_rpc_data_sendBlockToPC(&outBlock);
            // ns_lp_printf("Quaternion: qw=%.4f, qx=%.4f, qy=%.4f, qz=%.4f\n",
            //              g_sensorData[0], g_sensorData[1], g_sensorData[2], g_sensorData[3]);
        }
        ns_deep_sleep();
    }
    return 0;
}
