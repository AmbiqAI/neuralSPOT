

# File ns\_mpu6050\_i2c\_driver.h

[**File List**](files.md) **>** [**includes-api**](dir_ead24bed0d4d5fb1b23b4266f13c1ec5.md) **>** [**ns\_mpu6050\_i2c\_driver.h**](ns__mpu6050__i2c__driver_8h.md)

[Go to the documentation of this file](ns__mpu6050__i2c__driver_8h.md)

```C++


#ifndef NS_MPU6050
#define NS_MPU6050

#ifdef __cplusplus
extern "C" {
#endif

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "ns_i2c.h"

typedef enum { MPU6050_STATUS_SUCCESS = 0, MPU6050_STATUS_ERROR = 1 } mpu6050_status_e;

typedef enum {
    DLPF_260HZ = 0, // Accel: 260 Hz, Gyro: 256 Hz, Delay: 1 ms
    DLPF_184HZ = 1, // Accel: 184 Hz, Gyro: 188 Hz, Delay: 2 ms
    DLPF_094HZ = 2, // Accel:  94 Hz, Gyro:  98 Hz, Delay: 3 ms
    DLPF_044HZ = 3, // Accel:  44 Hz, Gyro:  42 Hz, Delay: 5 ms
    DLPF_021HZ = 4, // Accel:  21 Hz, Gyro:  20 Hz, Delay: 9 ms
    DLPF_010HZ = 5, // Accel:  10 Hz, Gyro:  10 Hz, Delay: 14 ms
    DLPF_005HZ = 6, // Accel:   5 Hz, Gyro:   5 Hz, Delay: 19 ms

} mpu6050_dlpf_cfg_t;

typedef enum {
    GYRO_FS_250DPS = 0,  
    GYRO_FS_500DPS = 1,  
    GYRO_FS_1000DPS = 2, 
    GYRO_FS_2000DPS = 3  
} mpu6050_gyro_fs_t;

typedef enum {
    ACCEL_FS_2G = 0, 
    ACCEL_FS_4G = 1, 
    ACCEL_FS_8G = 2, 
    ACCEL_FS_16G = 3 
} mpu6050_accel_fs_t;

typedef enum {
    CLOCK_INTERNAL = 0,  // Internal oscillator: 20MHz for MPU6500 and 8MHz for MPU6050
    CLOCK_GX_PLL = 1,    // Gyroscope x axis PLL
    CLOCK_GY_PLL = 2,    // Gyroscope y axis PLL
    CLOCK_GZ_PLL = 3,    // Gyroscope y axis PLL (recommended)
    CLOCK_32_REF = 4,    // PLL with external 32.768kHz reference
    CLOCK_19_REF = 5,    // PLL with external 19.2MHz reference
    CLOCK_KEEP_RESET = 7 
} mpu6050_clock_src_t;

typedef struct {
    uint8_t tempEnable;
    uint8_t xgEnable;
    uint8_t ygEnable;
    uint8_t zgEnable;
    uint8_t accelEnable;
    uint8_t slv2Enable;
    uint8_t slv1Enable;
    uint8_t slv0Enable;
} mpu6050_fifo_config_t;

typedef struct {
    uint8_t intLevel;
    uint8_t intOpen;
    uint8_t latchEnable;
    uint8_t rdClear;
    uint8_t fsyncLevel;
    uint8_t fsyncEnable;
} mpu6050_int_config_t;

#define MPU_I2CADDRESS_AD0_LOW 0x68
#define MPU_I2CADDRESS_AD0_HIGH 0x69

typedef struct {
    mpu6050_clock_src_t clock_src;
    mpu6050_dlpf_cfg_t dlpf_cfg;
    mpu6050_gyro_fs_t gyro_fullscale_range;
    mpu6050_accel_fs_t accel_fullscale_range;
    uint16_t sample_rate;
    uint16_t sleep_cfg;
} mpu6050_config_t;

uint32_t mpu6050_set_sample_rate_divider(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t divider);

uint32_t mpu6050_set_sample_rate(ns_i2c_config_t *cfg, uint32_t devAddr, uint16_t rate);

uint32_t
mpu6050_set_lowpass_filter(ns_i2c_config_t *cfg, uint32_t devAddr, mpu6050_dlpf_cfg_t value);

uint32_t mpu6050_set_gyro_full_scale(ns_i2c_config_t *cfg, uint32_t devAddr, mpu6050_gyro_fs_t fsr);

uint32_t
mpu6050_set_accel_full_scale(ns_i2c_config_t *cfg, uint32_t devAddr, mpu6050_accel_fs_t fsr);

uint32_t
mpu6050_configure_fifo(ns_i2c_config_t *cfg, uint32_t devAddr, mpu6050_fifo_config_t *fifoConfig);

uint32_t mpu6050_set_fifo_enable(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t enable);

uint32_t mpu6050_reset_fifo(ns_i2c_config_t *cfg, uint32_t devAddr);

uint32_t mpu6050_get_fifo_count(ns_i2c_config_t *cfg, uint32_t devAddr, uint16_t *count);

uint32_t mpu6050_fifo_pop(ns_i2c_config_t *cfg, uint32_t devAddr, int16_t *value);

uint32_t mpu6050_configure_interrupt(
    ns_i2c_config_t *cfg, uint32_t devAddr, mpu6050_int_config_t *intConfig);

uint32_t mpu6050_set_interrupt_enable(
    ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t overflowEnable, uint8_t dataReadyEnable);

uint32_t mpu6050_get_interrupt_status(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t *status);

uint32_t mpu6050_reset_signal_paths(ns_i2c_config_t *cfg, uint32_t devAddr);

uint32_t mpu6050_reset_signal_conds(ns_i2c_config_t *cfg, uint32_t devAddr);

uint32_t mpu6050_get_accel_values(
    ns_i2c_config_t *cfg, uint32_t devAddr, int16_t *x, int16_t *y, int16_t *z);

uint32_t
mpu6050_get_gyro_values(ns_i2c_config_t *cfg, uint32_t devAddr, int16_t *x, int16_t *y, int16_t *z);

uint32_t mpu6050_get_temperature(ns_i2c_config_t *cfg, uint32_t devAddr, int16_t *t);

uint32_t mpu6050_set_sleep(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t sleep);

uint32_t mpu6050_set_temperature_disable(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t disable);

uint32_t mpu6050_test_connection(ns_i2c_config_t *cfg, uint32_t devAddr);

uint32_t mpu6050_set_lowpower_accel_mode(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t wakeFreq);

uint32_t mpu6050_device_reset(ns_i2c_config_t *cfg, uint32_t devAddr);

uint32_t mpu6050_set_clock_source(ns_i2c_config_t *cfg, uint32_t devAddr, mpu6050_clock_src_t sel);

uint32_t mpu6050_read_sensors(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t *buffer);

uint32_t mpu6050_accel_fsr_value(mpu6050_accel_fs_t range);

float mpu6050_accel_resolution(mpu6050_accel_fs_t range);

float mpu6050_accel_to_gravity(int16_t val, mpu6050_accel_fs_t range);

float mpu6050_temperature_to_celsius(int16_t val);

uint32_t mpu6050_gyro_fsr_value(const mpu6050_gyro_fs_t range);

float mpu6050_gyro_resolution(const mpu6050_gyro_fs_t range);

float mpu6050_gyro_to_deg_per_sec(int val, mpu6050_gyro_fs_t range);

uint32_t mpu6050_set_accel_offset(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t axis, int offset);

uint32_t mpu6050_set_gyro_offset(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t axis, int offset);

uint32_t mpu6050_mean_sensors(
    ns_i2c_config_t *cfg, uint32_t devAddr, int *meanAX, int *meanAY, int *meanAZ, int *meanGX,
    int *meanGY, int *meanGZ);

uint32_t mpu6050_calibrate(ns_i2c_config_t *cfg, uint32_t devAddr);

extern uint32_t mpu6050_init(ns_i2c_config_t *cfg, mpu6050_config_t *c, uint32_t devAddr);

#ifdef __cplusplus
}
#endif
#endif // NS_MPU6050

```

