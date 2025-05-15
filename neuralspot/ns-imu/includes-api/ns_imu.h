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
    #include "icm45605.h"

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

typedef enum {
    NS_IMU_SENSOR_ICM45605,
    NS_IMU_SENSOR_MAX86150,
    NS_IMU_SENSOR_MAX
} ns_imu_sensor_e;

// Configuration struct
typedef struct {
    ns_core_api_t *api;
    ns_imu_sensor_e sensor;
    uint32_t sampleRate;
    uint32_t bufferSize;
    // ns_imu_frame_available_cb callback;
    void *buffer;
} ns_imu_config_t;

uint32_t ns_imu_init(ns_imu_config_t *cfg);
uint32_t ns_read_sensor_data(ns_imu_config_t *cfg, void *data, uint32_t size);

// ICM45605 prototypes
int8_t ns_imu_ICM45605_init(port);
int8_t ns_imu_ICM45605_get_accel_gyro(uint8_t *buff);

#ifdef __cplusplus
}
#endif
#endif // NS_IMU
