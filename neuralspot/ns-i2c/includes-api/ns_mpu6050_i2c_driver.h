/**
 * @file ns_mpu6050_i2c_driver.h
 * @author Adam Page
 * @brief Simple driver for Invensense MPU6050
 * @version 0.1
 * @date 2022-09-02
 *
 * @copyright Copyright (c) 2022
 *
 *  \addtogroup NeuralSPOT-MPU6050
 *  @{
 *  @ingroup NeuralSPOT-i2c
 */

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

/*! Gyroscope full-scale range */
typedef enum {
    GYRO_FS_250DPS = 0,  //!< +/- 250 º/s  -> 131 LSB/(º/s)
    GYRO_FS_500DPS = 1,  //!< +/- 500 º/s  -> 65.5 LSB/(º/s)
    GYRO_FS_1000DPS = 2, //!< +/- 1000 º/s -> 32.8 LSB/(º/s)
    GYRO_FS_2000DPS = 3  //!< +/- 2000 º/s -> 16.4 LSB/(º/s)
} mpu6050_gyro_fs_t;

/*! Accel full-scale range */
typedef enum {
    ACCEL_FS_2G = 0, //!< +/- 2 g  -> 16384 LSB/g
    ACCEL_FS_4G = 1, //!< +/- 4 g  -> 8192 LSB/g
    ACCEL_FS_8G = 2, //!< +/- 8 g  -> 4096 LSB/g
    ACCEL_FS_16G = 3 //!< +/- 16 g -> 2048 LSB/g
} mpu6050_accel_fs_t;

/*! Clock selection */
typedef enum {
    CLOCK_INTERNAL = 0,  // Internal oscillator: 20MHz for MPU6500 and 8MHz for MPU6050
    CLOCK_GX_PLL = 1,    // Gyroscope x axis PLL
    CLOCK_GY_PLL = 2,    // Gyroscope y axis PLL
    CLOCK_GZ_PLL = 3,    // Gyroscope y axis PLL (recommended)
    CLOCK_32_REF = 4,    // PLL with external 32.768kHz reference
    CLOCK_19_REF = 5,    // PLL with external 19.2MHz reference
    CLOCK_KEEP_RESET = 7 //!< Stops the clock and keeps timing generator in reset
} mpu6050_clock_src_t;

/*! FIFO configuration */
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

/*! Interrupt configuration */
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

/**
 * @brief Set sample rate divider
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param divider Sample rate divider
 * @return uint32_t status
 */
uint32_t
mpu6050_set_sample_rate_divider(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t divider);

/**
 * @brief Set target sample rate
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param rate Target rate in Hz
 * @return uint32_t status
 */
uint32_t
mpu6050_set_sample_rate(ns_i2c_config_t *cfg, uint32_t devAddr, uint16_t rate);

/**
 * @brief Set digital lowpass filter
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param value Filter selection
 * @return uint32_t status
 */
uint32_t
mpu6050_set_lowpass_filter(ns_i2c_config_t *cfg, uint32_t devAddr, mpu6050_dlpf_cfg_t value);

/**
 * @brief Set gyro full scale range
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param fsr Full scale range selection
 * @return uint32_t status
 */
uint32_t
mpu6050_set_gyro_full_scale(ns_i2c_config_t *cfg, uint32_t devAddr, mpu6050_gyro_fs_t fsr);

/**
 * @brief Set accel full scale range
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param fsr Full scale range selection
 * @return uint32_t
 */
uint32_t
mpu6050_set_accel_full_scale(ns_i2c_config_t *cfg, uint32_t devAddr, mpu6050_accel_fs_t fsr);

/**
 * @brief Configure FIFO
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param fifoConfig FIFO configuration
 * @return uint32_t status
 */
uint32_t
mpu6050_configure_fifo(ns_i2c_config_t *cfg, uint32_t devAddr, mpu6050_fifo_config_t *fifoConfig);

/**
 * @brief Enable FIFO
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param enable Enable FIFO flag
 * @return uint32_t status
 */
uint32_t
mpu6050_set_fifo_enable(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t enable);

/**
 * @brief Reset FIFO
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @return uint32_t
 */
uint32_t
mpu6050_reset_fifo(ns_i2c_config_t *cfg, uint32_t devAddr);

/**
 * @brief Get FIFO count
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param count Pointer to store count
 * @return uint32_t status
 */
uint32_t
mpu6050_get_fifo_count(ns_i2c_config_t *cfg, uint32_t devAddr, uint16_t *count);

/**
 * @brief Pop next value from FIFO
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param value Pointer to FIFO value
 * @return uint32_t status
 */
uint32_t
mpu6050_fifo_pop(ns_i2c_config_t *cfg, uint32_t devAddr, int16_t *value);

/**
 * @brief Configure interrupts
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param intConfig Interrupt configuration
 * @return uint32_t status
 */
uint32_t
mpu6050_configure_interrupt(ns_i2c_config_t *cfg, uint32_t devAddr,
                            mpu6050_int_config_t *intConfig);

/**
 * @brief Enable/disable interrupts
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param overflowEnable Overflow triggers interrupt
 * @param dataReadyEnable Data ready triggers interrupt
 * @return uint32_t status
 */
uint32_t
mpu6050_set_interrupt_enable(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t overflowEnable,
                             uint8_t dataReadyEnable);

/**
 * @brief Get interrupt status register
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param status Pointer to store status register
 * @return uint32_t
 */
uint32_t
mpu6050_get_interrupt_status(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t *status);

/**
 * @brief Reset signal paths
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @return uint32_t status
 */
uint32_t
mpu6050_reset_signal_paths(ns_i2c_config_t *cfg, uint32_t devAddr);

/**
 * @brief Reset signal conditions
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @return uint32_t status
 */
uint32_t
mpu6050_reset_signal_conds(ns_i2c_config_t *cfg, uint32_t devAddr);

/**
 * @brief Get current acceleration values
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param x Pointer to store x axis
 * @param y Pointer to store y axis
 * @param z Pointer to store z axis
 * @return uint32_t status
 */
uint32_t
mpu6050_get_accel_values(ns_i2c_config_t *cfg, uint32_t devAddr, int16_t *x, int16_t *y,
                         int16_t *z);

/**
 * @brief Get current gyro values
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param x Pointer to store x axis
 * @param y Pointer to store y axis
 * @param z Pointer to store z axis
 * @return uint32_t status
 */
uint32_t
mpu6050_get_gyro_values(ns_i2c_config_t *cfg, uint32_t devAddr, int16_t *x, int16_t *y, int16_t *z);

/**
 * @brief Get current temperature value
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param t Pointer to store Temperature
 * @return uint32_t status
 */
uint32_t
mpu6050_get_temperature(ns_i2c_config_t *cfg, uint32_t devAddr, int16_t *t);

/**
 * @brief Enable/disable sleep mode
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param sleep Sleep flag
 * @return uint32_t status
 */
uint32_t
mpu6050_set_sleep(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t sleep);

/**
 * @brief Disable temperature readings
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param disable Disable temperature flag
 * @return uint32_t status
 */
uint32_t
mpu6050_set_temperature_disable(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t disable);

/**
 * @brief Verify device by reading WHO_AM_I register
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @return uint32_t status
 */
uint32_t
mpu6050_test_connection(ns_i2c_config_t *cfg, uint32_t devAddr);

/**
 * @brief Put device into low-power acceleration-only mode
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param wakeFreq Wake-up frequency 0=1.25Hz, 1=5Hz 2=20Hz 3=40Hz
 * @return uint32_t status
 */
uint32_t
mpu6050_set_lowpower_accel_mode(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t wakeFreq);

/**
 * @brief Hard reset device
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @return uint32_t status
 */
uint32_t
mpu6050_device_reset(ns_i2c_config_t *cfg, uint32_t devAddr);

/**
 * @brief Set clock source
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param sel Clock selection
 * @return uint32_t status
 */
uint32_t
mpu6050_set_clock_source(ns_i2c_config_t *cfg, uint32_t devAddr, mpu6050_clock_src_t sel);

/**
 * @brief Reads MPU sensor values (3-axis accel, 1 temp, 3-axis gyro)
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param buffer 14-byte buffer to store values
 * @return uint32_t status
 */
uint32_t
mpu6050_read_sensors(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t *buffer);

/**
 * @brief Acceleration scale in units of G
 *
 * @param range Full scale range selection
 * @return uint32_t Scale in G
 */
uint32_t
mpu6050_accel_fsr_value(mpu6050_accel_fs_t range);

/**
 * @brief Acceleration scale LSB resolution
 *
 * @param range Full scale range selection
 * @return float Resolution (G)
 */
float
mpu6050_accel_resolution(mpu6050_accel_fs_t range);

/**
 * @brief Convert acceleration value to G
 *
 * @param val Accel register value
 * @param range Full-scale range used
 * @return Acceleration (G)
 */
float
mpu6050_accel_to_gravity(int16_t val, mpu6050_accel_fs_t range);

/**
 * @brief Convert temperature value to Celsius
 *
 * @param val Temperature register
 * @return float Temperature (°C)
 */
float
mpu6050_temperature_to_celsius(int16_t val);

/**
 * @brief Gyroscope scale in units dps
 *
 * @param range Full scale range selection
 * @return uint32_t Scale (°/s)
 */
uint32_t
mpu6050_gyro_fsr_value(const mpu6050_gyro_fs_t range);

/**
 * @brief Gyroscope scale LSB resolution
 *
 * @param range Full scale range selection
 * @return float Resolution (°/s)
 */
float
mpu6050_gyro_resolution(const mpu6050_gyro_fs_t range);

/**
 * @brief Convert gyroscope value to degrees/second
 *
 * @param val Gyroscope register value
 * @param range Full-scale range used
 * @return float Gyroscope (°/s)
 */
float
mpu6050_gyro_to_deg_per_sec(int val, mpu6050_gyro_fs_t range);

/**
 * @brief Set acceleration offset for axis
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param axis x=0, y=1, z=2
 * @param offset Offset value
 * @return uint32_t status
 */
uint32_t
mpu6050_set_accel_offset(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t axis, int offset);

/**
 * @brief Set gyroscope offset for axis
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param axis x=0, y=1, z=2
 * @param offset Offset value
 * @return uint32_t status
 */
uint32_t
mpu6050_set_gyro_offset(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t axis, int offset);

/**
 * @brief Get average sensor values
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param meanAX Pointer to store mean X accel
 * @param meanAY Pointer to store mean Y accel
 * @param meanAZ Pointer to store mean Z accel
 * @param meanGX Pointer to store mean X gyro
 * @param meanGY Pointer to store mean Y gyro
 * @param meanGZ Pointer to store mean Z gyro
 * @return uint32_t status
 */
uint32_t
mpu6050_mean_sensors(ns_i2c_config_t *cfg, uint32_t devAddr, int *meanAX, int *meanAY, int *meanAZ,
                     int *meanGX, int *meanGY, int *meanGZ);

/**
 * @brief Calibrate device offsets. Device must be still on a flat surface.
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @return uint32_t status
 */
uint32_t
mpu6050_calibration(ns_i2c_config_t *cfg, uint32_t devAddr);

#ifdef __cplusplus
}
#endif
/** @}*/
#endif // NS_MPU6050
