/**
 * @file ns_imu.h
 * @author Ambiq
 * @brief IMU Driver Wrapper
 * @version 0.1
 * @date 2025-05-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef NS_IMU
#define NS_IMU

#ifdef __cplusplus
extern "C" {
#endif

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "ns_core.h"
#include "ns_spi.h"
#include "ns_i2c.h"

#define NS_IMU_V0_0_1                                                                          \
    { .major = 0, .minor = 0, .revision = 1 }
#define NS_IMU_V1_0_0                                                                          \
    { .major = 1, .minor = 0, .revision = 0 }

#define NS_IMU_OLDEST_SUPPORTED_VERSION NS_IMU_V0_0_1
#define NS_IMU_CURRENT_VERSION NS_IMU_V1_0_0
#define NS_IMU_API_ID 0xCA000C

extern const ns_core_api_t ns_imu_V0_0_1;
extern const ns_core_api_t ns_imu_V1_0_0;
extern const ns_core_api_t ns_imu_oldest_supported_version;
extern const ns_core_api_t ns_imu_current_version;

// Define the callback function type
typedef void (*ns_imu_frame_available_cb)(void *arg); // arg is the ns_imu_config_t struct

typedef enum {
    NS_IMU_SENSOR_ICM45605,
    NS_IMU_SENSOR_MAX86150,
    NS_IMU_SENSOR_MAX
} ns_imu_sensor_e;

typedef struct {
    float accel_g[3];
    float gyro_dps[3];
    float temp_degc;
} ns_imu_sensor_data_t;

typedef struct {
    const ns_core_api_t *api;
    ns_imu_sensor_e     sensor;
    uint32_t            iom; // only for SPI sensors

    // IMU Configuration
    uint32_t       accel_fsr;
    uint32_t       gyro_fsr;
    uint32_t       accel_odr;
    uint32_t       gyro_odr;
    uint32_t       accel_ln_bw;
    uint32_t       gyro_ln_bw;
    uint32_t       calibrate; // true to calibrate the IMU

    // Frame mode configuration - set callback to enable, NULL to disable
    ns_imu_frame_available_cb frame_available_cb; /// Called when frame_size samples have been collected
    uint32_t       frame_size;
    ns_imu_sensor_data_t *frame_buffer;

    // Internal state
    void            *imu_dev_handle; // IMU device handle
    ns_spi_config_t *spi_cfg;        // SPI configuration
    uint32_t        calibrated;
    float           accel_bias[3];   // bias to subtract from accel_g
    float           gyro_bias[3];    // bias to subtract from gyro_dps
} ns_imu_config_t;


// API functions

/**
 * @brief Configure the EVB, SPI and IMU
 * 
 * @param cfg 
 */
uint32_t ns_imu_configure(ns_imu_config_t *cfg); // Will soft reset and configure the IMU

/**
 * @brief Retrieve 6DOF data from configured IMU
 * 
 * @param cfg Config struct
 * @param data Data retrieved in floating point natural units (g, dps, degC)
 * @return uin32_t status
 */
uint32_t ns_imu_get_data(ns_imu_config_t *cfg, ns_imu_sensor_data_t *data);
uint32_t ns_imu_get_raw_data(ns_imu_config_t *cfg, ns_imu_sensor_data_t *data);

#ifdef __cplusplus
}
#endif
#endif // NS_IMU
