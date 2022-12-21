/**
 * @file mpu6050_i2c_driver.c
 * @author Adam Page
 * @brief Driver of MPU6050
 * @version 0.1
 * @date 2022-08-29
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "ns_mpu6050_i2c_driver.h"
#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "ns_i2c_register_driver.h"
#include <limits.h>
#include <stdlib.h>

/*******************************************************************************
 * MPU commom registers for all models
 ******************************************************************************/
#define MPU6050_RA_XA_OFFS_H 0x06
#define MPU6050_RA_XA_OFFS_L 0x07
#define MPU6050_RA_YA_OFFS_H 0x08
#define MPU6050_RA_YA_OFFS_L 0x09
#define MPU6050_RA_ZA_OFFS_H 0x0A
#define MPU6050_RA_ZA_OFFS_L 0x0B
#define XG_OFFSET_H 0x13
#define XG_OFFSET_L 0x14
#define YG_OFFSET_H 0x15
#define YG_OFFSET_L 0x16
#define ZG_OFFSET_H 0x17
#define ZG_OFFSET_L 0x18
#define SMPLRT_DIV 0x19
//------------------------------------------------------------------------------
#define CONFIG 0x1A
#define CONFIG_FIFO_MODE_BIT 6
#define CONFIG_EXT_SYNC_SET_BIT 5 // [5:3]
#define CONFIG_EXT_SYNC_SET_LENGTH 3
#define CONFIG_DLPF_CFG_BIT 2 // [2:0]
#define CONFIG_DLPF_CFG_LENGTH 3
//------------------------------------------------------------------------------
#define GYRO_CONFIG 0x1B
#define GCONFIG_XG_ST_BIT 7
#define GCONFIG_YG_ST_BIT 6
#define GCONFIG_ZG_ST_BIT 5
#define GCONFIG_FS_SEL_BIT 4 // [4:3]
#define GCONFIG_FS_SEL_LENGTH 2
#define GCONFIG_FCHOICE_B 1 // [1:0]
#define GCONFIG_FCHOICE_B_LENGTH 2
//------------------------------------------------------------------------------
#define ACCEL_CONFIG 0x1C
#define ACONFIG_XA_ST_BIT 7
#define ACONFIG_YA_ST_BIT 6
#define ACONFIG_ZA_ST_BIT 5
#define ACONFIG_FS_SEL_BIT 4 // [4:3]
#define ACONFIG_FS_SEL_LENGTH 2
#define ACONFIG_HPF_BIT 2 // [2:0]
#define ACONFIG_HPF_LENGTH 3
//------------------------------------------------------------------------------
#define FF_THR 0x1D
#define FF_DUR 0x1E
#define MOTION_THR 0x1F // [7:0] // MPU9250_REG_WOM_THR
#define MOTION_DUR 0x20
#define ZRMOTION_THR 0x21
#define ZRMOTION_DUR 0x22
//------------------------------------------------------------------------------
#define FIFO_EN 0x23
#define FIFO_TEMP_EN_BIT 7
#define FIFO_XGYRO_EN_BIT 6
#define FIFO_YGYRO_EN_BIT 5
#define FIFO_ZGYRO_EN_BIT 4
#define FIFO_ACCEL_EN_BIT 3
#define FIFO_SLV_2_EN_BIT 2
#define FIFO_SLV_1_EN_BIT 1
#define FIFO_SLV_0_EN_BIT 0
//------------------------------------------------------------------------------
#define I2C_MST_CTRL 0x24
#define I2CMST_CTRL_MULT_EN_BIT 7
#define I2CMST_CTRL_WAIT_FOR_ES_BIT 6
#define I2CMST_CTRL_SLV_3_FIFO_EN_BIT 5
#define I2CMST_CTRL_P_NSR_BIT 4
#define I2CMST_CTRL_CLOCK_BIT 3 // [3:0]
#define I2CMST_CTRL_CLOCK_LENGTH 4
//------------------------------------------------------------------------------
#define I2C_SLV0_ADDR 0x25
#define I2C_SLV_RNW_BIT 7 // same for all I2C_SLV registers
#define I2C_SLV_ID_BIT 6  // [6:0]
#define I2C_SLV_ID_LENGTH 7
//------------------------------------------------------------------------------
#define I2C_SLV0_REG 0x26 // [7:0]
//------------------------------------------------------------------------------
#define I2C_SLV0_CTRL 0x27
#define I2C_SLV_EN_BIT 7 // same for all I2C_SLV registers
#define I2C_SLV_BYTE_SW_BIT 6
#define I2C_SLV_REG_DIS_BIT 5
#define I2C_SLV_GRP_BIT 4
#define I2C_SLV_LEN_BIT 3 // [3:0]
#define I2C_SLV_LEN_LENGTH 4
//------------------------------------------------------------------------------
#define I2C_SLV1_ADDR 0x28 // see SLV0 for bit defines
#define I2C_SLV1_REG 0x29
#define I2C_SLV1_CTRL 0x2A
#define I2C_SLV2_ADDR 0x2B // see SLV0 for bit defines
#define I2C_SLV2_REG 0x2C
#define I2C_SLV2_CTRL 0x2D
#define I2C_SLV3_ADDR 0x2E // see SLV0 for bit defines
#define I2C_SLV3_REG 0x2F
#define I2C_SLV3_CTRL 0x30
#define I2C_SLV4_ADDR 0x31 // see SLV0 for bit defines
#define I2C_SLV4_REG 0x32
#define I2C_SLV4_DO 0x33 // [7:0]
//------------------------------------------------------------------------------
#define I2C_SLV4_CTRL 0x34
#define I2C_SLV4_EN_BIT 7
#define I2C_SLV4_DONE_INT_BIT 6
#define I2C_SLV4_REG_DIS_BIT 5
#define I2C_SLV4_MST_DELAY_BIT 4 // [4:0]
#define I2C_SLV4_MST_DELAY_LENGTH 5
//------------------------------------------------------------------------------
#define I2C_SLV4_DI 0x35 // [7:0]
//------------------------------------------------------------------------------
#define I2C_MST_STATUS 0x36
#define I2CMST_STAT_PASS_THROUGH_BIT 7
#define I2CMST_STAT_SLV4_DONE_BIT 6
#define I2CMST_STAT_LOST_ARB_BIT 5
#define I2CMST_STAT_SLV4_NACK_BIT 4
#define I2CMST_STAT_SLV3_NACK_BIT 3
#define I2CMST_STAT_SLV2_NACK_BIT 2
#define I2CMST_STAT_SLV1_NACK_BIT 1
#define I2CMST_STAT_SLV0_NACK_BIT 0
//------------------------------------------------------------------------------
#define INT_PIN_CONFIG 0x37
#define INT_CFG_LEVEL_BIT 7
#define INT_CFG_OPEN_BIT 6
#define INT_CFG_LATCH_EN_BIT 5
#define INT_CFG_ANYRD_2CLEAR_BIT 4
#define INT_CFG_FSYNC_LEVEL_BIT 3
#define INT_CFG_FSYNC_INT_MODE_EN_BIT 2
#define INT_CFG_I2C_BYPASS_EN_BIT 1
#define INT_CFG_CLOCKOUT_EN_BIT 0
//------------------------------------------------------------------------------
#define INT_ENABLE 0x38
#define INT_ENABLE_FREEFALL_BIT 7
#define INT_ENABLE_MOTION_BIT 6
#define INT_ENABLE_ZEROMOT_BIT 5
#define INT_ENABLE_FIFO_OFLOW_BIT 4
#define INT_ENABLE_I2C_MST_FSYNC_BIT 3
#define INT_ENABLE_PLL_RDY_BIT 2
#define INT_ENABLE_DMP_RDY_BIT 1
#define INT_ENABLE_RAW_DATA_RDY_BIT 0
//------------------------------------------------------------------------------
#define DMP_INT_STATUS 0x39
#define DMP_INT_STATUS_0 0
#define DMP_INT_STATUS_1 1
#define DMP_INT_STATUS_2 2
#define DMP_INT_STATUS_3 3
#define DMP_INT_STATUS_4 4
#define DMP_INT_STATUS_5 5
//------------------------------------------------------------------------------
#define INT_STATUS 0x3A
#define INT_STATUS_FREEFALL_BIT 7
#define INT_STATUS_MOTION_BIT 6
#define INT_STATUS_ZEROMOT_BIT 5
#define INT_STATUS_FIFO_OFLOW_BIT 4
#define INT_STATUS_I2C_MST_BIT 3
#define INT_STATUS_PLL_RDY_BIT 2
#define INT_STATUS_DMP_RDY_BIT 1
#define INT_STATUS_RAW_DATA_RDY_BIT 0
//------------------------------------------------------------------------------
#define ACCEL_XOUT_H 0x3B // [15:0]
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D // [15:0]
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F // [15:0]
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H 0x41 // [15:0]
#define TEMP_OUT_L 0x42
#define GYRO_XOUT_H 0x43 // [15:0]
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45 // [15:0]
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47 // [15:0]
#define GYRO_ZOUT_L 0x48
#define EXT_SENS_DATA_00 0x49 // Stores data read from Slave 0, 1, 2, and 3
#define EXT_SENS_DATA_01 0x4A
#define EXT_SENS_DATA_02 0x4B
#define EXT_SENS_DATA_03 0x4C
#define EXT_SENS_DATA_04 0x4D
#define EXT_SENS_DATA_05 0x4E
#define EXT_SENS_DATA_06 0x4F
#define EXT_SENS_DATA_07 0x50
#define EXT_SENS_DATA_08 0x51
#define EXT_SENS_DATA_09 0x52
#define EXT_SENS_DATA_10 0x53
#define EXT_SENS_DATA_11 0x54
#define EXT_SENS_DATA_12 0x55
#define EXT_SENS_DATA_13 0x56
#define EXT_SENS_DATA_14 0x57
#define EXT_SENS_DATA_15 0x58
#define EXT_SENS_DATA_17 0x5A
#define EXT_SENS_DATA_18 0x5B
#define EXT_SENS_DATA_19 0x5C
#define EXT_SENS_DATA_20 0x5D
#define EXT_SENS_DATA_21 0x5E
#define EXT_SENS_DATA_22 0x5F
#define EXT_SENS_DATA_23 0x60
#define I2C_SLV0_DO 0x63
#define I2C_SLV1_DO 0x64
#define I2C_SLV2_DO 0x65
#define I2C_SLV3_DO 0x66
//------------------------------------------------------------------------------
#define I2C_MST_DELAY_CRTL 0x67
#define I2CMST_DLY_ES_SHADOW_BIT 7
#define I2CMST_DLY_SLV4_EN_BIT 4
#define I2CMST_DLY_SLV3_EN_BIT 3
#define I2CMST_DLY_SLV2_EN_BIT 2
#define I2CMST_DLY_SLV1_EN_BIT 1
#define I2CMST_DLY_SLV0_EN_BIT 0
//------------------------------------------------------------------------------
#define SIGNAL_PATH_RESET 0x68
#define SPATH_GYRO_RST_BIT 2
#define SPATH_ACCEL_RST_BIT 1
#define SPATH_TEMP_RST_BIT 0
//------------------------------------------------------------------------------
#define USER_CTRL 0x6A
#define USERCTRL_DMP_EN_BIT 7
#define USERCTRL_FIFO_EN_BIT 6
#define USERCTRL_I2C_MST_EN_BIT 5
#define USERCTRL_I2C_IF_DIS_BIT 4
#define USERCTRL_DMP_RESET_BIT 3
#define USERCTRL_FIFO_RESET_BIT 2
#define USERCTRL_I2C_MST_RESET_BIT 1
#define USERCTRL_SIG_COND_RESET_BIT 0
//------------------------------------------------------------------------------
#define PWR_MGMT1 0x6B
#define PWR1_DEVICE_RESET_BIT 7
#define PWR1_SLEEP_BIT 6
#define PWR1_CYCLE_BIT 5
#define PWR1_GYRO_STANDBY_BIT 4
#define PWR1_TEMP_DIS_BIT 3
#define PWR1_CLKSEL_BIT 2
#define PWR1_CLKSEL_LENGTH 3
//------------------------------------------------------------------------------
#define PWR_MGMT2 0x6C
#define PWR2_LP_WAKE_CTRL_BIT 7
#define PWR2_LP_WAKE_CTRL_LENGTH 2
#define PWR2_STBY_XA_BIT 5
#define PWR2_STBY_YA_BIT 4
#define PWR2_STBY_ZA_BIT 3
#define PWR2_STBY_XG_BIT 2
#define PWR2_STBY_YG_BIT 1
#define PWR2_STBY_ZG_BIT 0
#define PWR2_STBY_XYZA_BITS 1 << PWR2_STBY_XA_BIT | 1 << PWR2_STBY_YA_BIT | 1 << PWR2_STBY_ZA_BIT
#define PWR2_STBY_XYZG_BITS 1 << PWR2_STBY_XG_BIT | 1 << PWR2_STBY_YG_BIT | 1 << PWR2_STBY_ZG_BIT
//------------------------------------------------------------------------------
#define BANK_SEL 0x6D
#define BANKSEL_PRFTCH_EN_BIT 6
#define BANKSEL_CFG_USER_BANK_BIT 5
#define BANKSEL_MEM_SEL_BIT 4
#define BANKSEL_MEM_SEL_LENGTH 5
//------------------------------------------------------------------------------
#define MEM_START_ADDR 0x6E
#define MEM_R_W 0x6F
#define PRGM_START_H 0x70
#define PRGM_START_L 0x71
#define FIFO_COUNT_H 0x72 // [15:0]
#define FIFO_COUNT_L 0x73
#define FIFO_R_W 0x74
#define WHO_AM_I 0x75

static int8_t
get_high_bits(int16_t regReading) {
    return (regReading & 0xFF00) >> 8;
}

static int8_t
get_low_bits(int16_t regReading) {
    return regReading & 0x00FF;
}

static uint32_t
read_word_register(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t reg, uint16_t *value) {
    uint8_t buffer[2];
    if (ns_i2c_read_sequential_regs(cfg, devAddr, reg, buffer, 2)) {
        return MPU6050_STATUS_ERROR;
    }
    *value = (uint16_t)(buffer[0] << 8 | buffer[1]);
    return MPU6050_STATUS_SUCCESS;
}

/**
 * @brief Set sample rate divider
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param divider Sample rate divider
 * @return uint32_t status
 */
uint32_t
mpu6050_set_sample_rate_divider(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t divider) {
    return ns_i2c_write_reg(cfg, devAddr, SMPLRT_DIV, divider, 0xFF);
}

/**
 * @brief Set target sample rate
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param rate Target rate in Hz
 * @return uint32_t status
 */
uint32_t
mpu6050_set_sample_rate(ns_i2c_config_t *cfg, uint32_t devAddr, uint16_t rate) {
    rate = rate < 4 ? 4 : rate > 1000 ? 1000 : rate;
    uint32_t internalSampleRate = 1000;
    uint8_t divider = internalSampleRate / rate - 1;
    return mpu6050_set_sample_rate_divider(cfg, devAddr, divider);
}

/**
 * @brief Set digital lowpass filter
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param value Filter selection
 * @return uint32_t status
 */
uint32_t
mpu6050_set_lowpass_filter(ns_i2c_config_t *cfg, uint32_t devAddr, mpu6050_dlpf_cfg_t value) {
    return ns_i2c_write_reg(cfg, devAddr, CONFIG, value, 0x07);
}

/**
 * @brief Set gyro full scale range
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param fsr Full scale range selection
 * @return uint32_t status
 */
uint32_t
mpu6050_set_gyro_full_scale(ns_i2c_config_t *cfg, uint32_t devAddr, mpu6050_gyro_fs_t fsr) {
    return ns_i2c_write_reg(cfg, devAddr, GYRO_CONFIG, fsr << GCONFIG_FS_SEL_BIT, 0x18);
}

/**
 * @brief Set accel full scale range
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param fsr Full scale range selection
 * @return uint32_t
 */
uint32_t
mpu6050_set_accel_full_scale(ns_i2c_config_t *cfg, uint32_t devAddr, mpu6050_accel_fs_t fsr) {
    return ns_i2c_write_reg(cfg, devAddr, ACCEL_CONFIG, fsr << ACONFIG_FS_SEL_BIT, 0x18);
}

/**
 * @brief Configure FIFO
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param fifoConfig FIFO configuration
 * @return uint32_t status
 */
uint32_t
mpu6050_configure_fifo(ns_i2c_config_t *cfg, uint32_t devAddr, mpu6050_fifo_config_t *fifoConfig) {
    uint8_t val = 0;
    val |= (fifoConfig->tempEnable << 7);
    val |= (fifoConfig->xgEnable << 6);
    val |= (fifoConfig->ygEnable << 5);
    val |= (fifoConfig->zgEnable << 4);
    val |= (fifoConfig->accelEnable << 3);
    val |= (fifoConfig->slv2Enable << 2);
    val |= (fifoConfig->slv1Enable << 1);
    val |= (fifoConfig->slv0Enable << 0);
    return ns_i2c_write_reg(cfg, devAddr, FIFO_EN, val, 0xFF);
}

/**
 * @brief Enable FIFO
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param enable Enable FIFO flag
 * @return uint32_t status
 */
uint32_t
mpu6050_set_fifo_enable(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t enable) {
    return ns_i2c_write_reg(cfg, devAddr, USER_CTRL, enable << 6, 1 << 6);
}

/**
 * @brief Reset FIFO
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @return uint32_t
 */
uint32_t
mpu6050_reset_fifo(ns_i2c_config_t *cfg, uint32_t devAddr) {
    return ns_i2c_write_reg(cfg, devAddr, USER_CTRL, 1 << 2, 1 << 2);
}

/**
 * @brief Get FIFO count
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param count Pointer to store count
 * @return uint32_t status
 */
uint32_t
mpu6050_get_fifo_count(ns_i2c_config_t *cfg, uint32_t devAddr, uint16_t *count) {
    if (read_word_register(cfg, devAddr, FIFO_COUNT_H, count)) {
        return MPU6050_STATUS_ERROR;
    }
    return MPU6050_STATUS_SUCCESS;
}

/**
 * @brief Pop next value from FIFO
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param value Pointer to FIFO value
 * @return uint32_t status
 */
uint32_t
mpu6050_fifo_pop(ns_i2c_config_t *cfg, uint32_t devAddr, int16_t *value) {
    if (read_word_register(cfg, devAddr, FIFO_COUNT_H, (uint16_t *)value)) {
        return MPU6050_STATUS_ERROR;
    }
    return MPU6050_STATUS_SUCCESS;
}

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
                            mpu6050_int_config_t *intConfig) {
    uint8_t val = 0;
    val |= (intConfig->intLevel << 7);
    val |= (intConfig->intOpen << 6);
    val |= (intConfig->latchEnable << 5);
    val |= (intConfig->rdClear << 4);
    val |= (intConfig->fsyncLevel << 3);
    val |= (intConfig->fsyncEnable << 2);
    return ns_i2c_write_reg(cfg, devAddr, INT_PIN_CONFIG, val, 0xFF);
}

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
                             uint8_t dataReadyEnable) {
    uint8_t val = (overflowEnable << 4) | (dataReadyEnable << 0);
    return ns_i2c_write_reg(cfg, devAddr, INT_ENABLE, val, 0xFF);
}

/**
 * @brief Get interrupt status register
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param status Pointer to store status register
 * @return uint32_t
 */
uint32_t
mpu6050_get_interrupt_status(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t *status) {
    return ns_i2c_read_reg(cfg, devAddr, INT_STATUS, status, 0x19);
}

/**
 * @brief Reset signal paths
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @return uint32_t status
 */
uint32_t
mpu6050_reset_signal_paths(ns_i2c_config_t *cfg, uint32_t devAddr) {
    return ns_i2c_write_reg(cfg, devAddr, SIGNAL_PATH_RESET, 0x07, 0x07);
}

/**
 * @brief Reset signal conditions
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @return uint32_t status
 */
uint32_t
mpu6050_reset_signal_conds(ns_i2c_config_t *cfg, uint32_t devAddr) {
    return ns_i2c_write_reg(cfg, devAddr, USER_CTRL, 0x01, 0x01);
}

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
                         int16_t *z) {
    uint8_t buffer[6];
    if (ns_i2c_read_sequential_regs(cfg, devAddr, ACCEL_XOUT_H, buffer, 6)) {
        return MPU6050_STATUS_ERROR;
    }
    *x = (int16_t)(buffer[0] << 8 | buffer[1]);
    *y = (int16_t)(buffer[2] << 8 | buffer[3]);
    *z = (int16_t)(buffer[4] << 8 | buffer[5]);
    return MPU6050_STATUS_SUCCESS;
}

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
mpu6050_get_gyro_values(ns_i2c_config_t *cfg, uint32_t devAddr, int16_t *x, int16_t *y,
                        int16_t *z) {
    uint8_t buffer[6];
    if (ns_i2c_read_sequential_regs(cfg, devAddr, GYRO_XOUT_H, buffer, 6)) {
        return MPU6050_STATUS_ERROR;
    }
    *x = (int16_t)(buffer[0] << 8 | buffer[1]);
    *y = (int16_t)(buffer[2] << 8 | buffer[3]);
    *z = (int16_t)(buffer[4] << 8 | buffer[5]);
    return MPU6050_STATUS_SUCCESS;
}

/**
 * @brief Get current temperature value
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param t Pointer to store Temperature
 * @return uint32_t status
 */
uint32_t
mpu6050_get_temperature(ns_i2c_config_t *cfg, uint32_t devAddr, int16_t *t) {
    if (read_word_register(cfg, devAddr, TEMP_OUT_H, (uint16_t *)t)) {
        return MPU6050_STATUS_ERROR;
    }
    return MPU6050_STATUS_SUCCESS;
}

/**
 * @brief Enable/disable sleep mode
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param sleep Sleep flag
 * @return uint32_t status
 */
uint32_t
mpu6050_set_sleep(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t sleep) {
    return ns_i2c_write_reg(cfg, devAddr, PWR_MGMT1, sleep << 6, 1 << 6);
}

/**
 * @brief Disable temperature readings
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param disable Disable temperature flag
 * @return uint32_t status
 */
uint32_t
mpu6050_set_temperature_disable(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t disable) {
    return ns_i2c_write_reg(cfg, devAddr, PWR_MGMT1, disable << 3, 1 << 3);
}

/**
 * @brief Verify device by reading WHO_AM_I register
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @return uint32_t status
 */
uint32_t
mpu6050_test_connection(ns_i2c_config_t *cfg, uint32_t devAddr) {
    uint8_t regValue;
    if (ns_i2c_read_reg(cfg, devAddr, WHO_AM_I, &regValue, 0x7F)) {
        return MPU6050_STATUS_ERROR;
    }
    return regValue != 0x68;
}

/**
 * @brief Put device into low-power acceleration-only mode
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param wakeFreq Wake-up frequency 0=1.25Hz, 1=5Hz 2=20Hz 3=40Hz
 * @return uint32_t status
 */
uint32_t
mpu6050_set_lowpower_accel_mode(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t wakeFreq) {
    return (
        // sleep=0,cycle=1,temp_dis=1
        ns_i2c_write_reg(cfg, devAddr, PWR_MGMT1, 0x28, 0x78) ||
        // Disable gyro and set wake-up frequency
        ns_i2c_write_reg(cfg, devAddr, PWR_MGMT2, (wakeFreq << 6) | 0x7, 0xFF));
}

/**
 * @brief Hard reset device
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @return uint32_t status
 */
uint32_t
mpu6050_device_reset(ns_i2c_config_t *cfg, uint32_t devAddr) {
    uint8_t val = 0x80;
    ns_i2c_write_reg(cfg, devAddr, PWR_MGMT1, val, 0x80);
    am_hal_delay_us(1000);
    // Wait for device to clear reset
    while (val) {
        am_hal_delay_us(100);
        ns_i2c_read_reg(cfg, devAddr, PWR_MGMT1, &val, 0x80);
    }
    return MPU6050_STATUS_SUCCESS;
}

/**
 * @brief Set clock source
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param sel Clock selection
 * @return uint32_t status
 */
uint32_t
mpu6050_set_clock_source(ns_i2c_config_t *cfg, uint32_t devAddr, mpu6050_clock_src_t sel) {
    return ns_i2c_write_reg(cfg, devAddr, PWR_MGMT1, sel, 0x07);
}

/**
 * @brief Reads MPU sensor values (3-axis accel, 1 temp, 3-axis gyro)
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @param buffer 14-byte buffer to store values
 * @return uint32_t status
 */
uint32_t
mpu6050_read_sensors(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t *buffer) {
    if (ns_i2c_read_sequential_regs(cfg, devAddr, ACCEL_XOUT_H, buffer, 14)) {
        return MPU6050_STATUS_ERROR;
    }
    return MPU6050_STATUS_SUCCESS;
}

/**
 * @brief Acceleration scale in units of G
 *
 * @param range Full scale range selection
 * @return uint32_t Scale in G
 */
uint32_t
mpu6050_accel_fsr_value(mpu6050_accel_fs_t range) {
    return 2 << range;
}

/**
 * @brief Acceleration scale LSB resolution
 *
 * @param range Full scale range selection
 * @return float Resolution (G)
 */
float
mpu6050_accel_resolution(mpu6050_accel_fs_t range) {
    return (float)mpu6050_accel_fsr_value(range) / (float)INT16_MAX;
}

/**
 * @brief Gyroscope scale in units dps
 *
 * @param range Full scale range selection
 * @return uint32_t Scale (°/s)
 */
uint32_t
mpu6050_gyro_fsr_value(const mpu6050_gyro_fs_t range) {
    return 250 << range;
}

/**
 * @brief Gyroscope scale LSB resolution
 *
 * @param range Full scale range selection
 * @return float Resolution (°/s)
 */
float
mpu6050_gyro_resolution(const mpu6050_gyro_fs_t range) {
    return (float)mpu6050_gyro_fsr_value(range) / (float)INT16_MAX;
}

/**
 * @brief Convert acceleration value to G
 *
 * @param val Accel register value
 * @param range Full-scale range used
 * @return Acceleration (G)
 */
float
mpu6050_accel_to_gravity(int16_t val, mpu6050_accel_fs_t range) {
    return (float)val * mpu6050_accel_resolution(range);
}

/**
 * @brief Convert temperature value to Celsius
 *
 * @param val Temperature register
 * @return float Temperature (°C)
 */
float
mpu6050_temperature_to_celsius(int16_t val) {
    return val / 340.0 + 36.53;
}

/**
 * @brief Convert gyroscope value to degrees/second
 *
 * @param val Gyroscope register value
 * @param range Full-scale range used
 * @return float Gyroscope (°/s)
 */
float
mpu6050_gyro_to_deg_per_sec(int val, mpu6050_gyro_fs_t range) {
    return (float)val * mpu6050_gyro_resolution(range);
}

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
mpu6050_set_accel_offset(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t axis, int offset) {
    // 0x06 6 XA_OFFSET_H R/W XA_OFFSET [14:7]
    // 0x07 7 XA_OFFSET_L R/W XA_OFFSET [6:0]

    // 0000_0000_0000_0000
    // 0111_1111_1000_0000
    // 0000_0000_0111_1111
    uint8_t msb14_7 = (offset & 0xFF00) >> 8; // (offset & 0x7F80) >> 7;
    uint8_t lsb6_0 = (offset & 0xFF);         // 0x7f);
    // uint8_t msb14_7 = (offset & 0x7F80) >> 7;
    // uint8_t lsb6_0 = (offset & 0x7f);

    return (ns_i2c_write_reg(cfg, devAddr, MPU6050_RA_XA_OFFS_H + (axis * 2), msb14_7, 0xFF) ||
            ns_i2c_write_reg(cfg, devAddr, MPU6050_RA_XA_OFFS_L + (axis * 2), lsb6_0, 0xFF));
}

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
mpu6050_set_gyro_offset(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t axis, int offset) {

    return (ns_i2c_write_reg(cfg, devAddr, (axis * 2) + XG_OFFSET_H, get_high_bits(offset), 0xFF) ||
            ns_i2c_write_reg(cfg, devAddr, (axis * 2) + XG_OFFSET_L, get_low_bits(offset), 0xFF));
}

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
                     int *meanGX, int *meanGY, int *meanGZ) {
    uint16_t i;
    uint16_t numReadings = 200;
    int16_t aVal[3];
    int16_t gVal[3];
    int64_t aBuf[3] = {0, 0, 0};
    int64_t gBuf[3] = {0, 0, 0};

    // Stabilize? (dont actually know why this code is here)
    for (i = 0; i < 50; i++) {
        mpu6050_get_accel_values(cfg, devAddr, &aVal[0], &aVal[1], &aVal[2]);
        mpu6050_get_gyro_values(cfg, devAddr, &gVal[0], &gVal[1], &gVal[2]);
        am_hal_delay_us(2);
    }

    for (i = 0; i < numReadings; i++) {
        if (mpu6050_get_accel_values(cfg, devAddr, &aVal[0], &aVal[1], &aVal[2]) ||
            mpu6050_get_gyro_values(cfg, devAddr, &gVal[0], &gVal[1], &gVal[2])) {
            return MPU6050_STATUS_ERROR;
        }
        for (int a = 0; a < 3; a++) {
            aBuf[a] += aVal[a];
            gBuf[a] += gVal[a];
        }
        am_hal_delay_us(2);
    }

    *meanAX = aBuf[0] / numReadings;
    *meanAY = aBuf[1] / numReadings;
    *meanAZ = aBuf[2] / numReadings;
    *meanGX = gBuf[0] / numReadings;
    *meanGY = gBuf[1] / numReadings;
    *meanGZ = gBuf[2] / numReadings;
    return MPU6050_STATUS_SUCCESS;
}

uint32_t
mpu6050_get_accel_offset(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t axis, uint16_t *offset) {
    uint8_t lsb = 0, msb = 0;
    ns_i2c_read_reg(cfg, devAddr, MPU6050_RA_XA_OFFS_H + (axis * 2), &msb, 0xff);
    ns_i2c_read_reg(cfg, devAddr, MPU6050_RA_XA_OFFS_L + (axis * 2), &lsb, 0xff);
    *offset = msb;
    *offset = (*offset << 8) + lsb;
    *offset += lsb;

    return MPU6050_STATUS_SUCCESS;
}

uint32_t
mpu6050_get_gyro_offset(ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t axis, uint16_t *offset) {
    uint8_t lsb = 0, msb = 0;
    ns_i2c_read_reg(cfg, devAddr, XG_OFFSET_H + (axis * 2), &msb, 0xff);
    ns_i2c_read_reg(cfg, devAddr, XG_OFFSET_L + (axis * 2), &lsb, 0xff);
    *offset = msb;
    *offset = (*offset << 8) + lsb;

    return MPU6050_STATUS_SUCCESS;
}

/**
 * @brief Internal calibration routine
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @return uint32_t status
 */
uint32_t
mpu6050_calibrate(ns_i2c_config_t *cfg, uint32_t devAddr) {
    int accelDeadzone = 8, gyroDeadzone = 4;
    int accelStep = 10, gyroStep = 4;
    int aMean[3] = {0, 0, 0};
    int gMean[3] = {0, 0, 0};
    int aOffset[3] = {0, 0, 0};
    int gOffset[3] = {0, 0, 0};
    bool good = false;

    if (mpu6050_mean_sensors(cfg, devAddr, &aMean[0], &aMean[1], &aMean[2], &gMean[0], &gMean[1],
                             &gMean[2])) {
        return MPU6050_STATUS_ERROR;
    }
    for (int a = 0; a < 3; a++) {
        mpu6050_get_accel_offset(cfg, devAddr, a, (uint16_t *)&aOffset[a]);
        mpu6050_get_gyro_offset(cfg, devAddr, a, (uint16_t *)&gOffset[a]);
    }

    while (1) {
        // Set new offsets
        for (int a = 0; a < 3; a++) {
            if (mpu6050_set_accel_offset(cfg, devAddr, a, aOffset[a]) ||
                mpu6050_set_gyro_offset(cfg, devAddr, a, gOffset[a]))
                return MPU6050_STATUS_ERROR;
        }

        am_hal_delay_us(1000);
        if (mpu6050_mean_sensors(cfg, devAddr, &aMean[0], &aMean[1], &aMean[2], &gMean[0],
                                 &gMean[1], &gMean[2])) {
            return MPU6050_STATUS_ERROR;
        }
        am_hal_delay_us(1000);

        // Adjust offsets to reduce Mean readings
        good = true;
        for (int a = 0; a < 3; a++) {
            if (abs(aMean[a]) > accelDeadzone) {
                good = false;
                aOffset[a] -= aMean[a] / accelStep;
            }

            if (abs(gMean[a]) > gyroDeadzone) {
                good = false;
                gOffset[a] -= gMean[a] / gyroStep;
            }
        }
        // for (int a = 0; a < 3; a++) {
        //     ns_lp_printf("Axis[%i]: Accel %i, %i | Gyro %i, %i\n", a, aMean[a], aOffset[a],
        //                  gMean[a], gOffset[a]);
        // }
        if (good)
            break;
    }
    // ns_lp_printf("Success!\n");
    return MPU6050_STATUS_SUCCESS;
}

/**
 * @brief Calibrate device offsets. Device must be still on a flat surface.
 *
 * @param cfg I2C configuration
 * @param devAddr Device I2C address
 * @return uint32_t status
 */
uint32_t
mpu6050_calibration(ns_i2c_config_t *cfg, uint32_t devAddr) {
    if (mpu6050_calibrate(cfg, devAddr)) {
        return MPU6050_STATUS_ERROR;
    }
    am_hal_delay_us(1000);
    return MPU6050_STATUS_SUCCESS;
}

uint32_t
mpu6050_init(ns_i2c_config_t *cfg, mpu6050_config_t *c, uint32_t devAddr) {
    if (mpu6050_device_reset(cfg, devAddr) ||
        mpu6050_set_clock_source(cfg, devAddr, c->clock_src) ||
        mpu6050_set_lowpass_filter(cfg, devAddr, c->dlpf_cfg) ||
        mpu6050_set_gyro_full_scale(cfg, devAddr, c->gyro_fullscale_range) ||
        mpu6050_set_accel_full_scale(cfg, devAddr, c->accel_fullscale_range) ||
        mpu6050_set_sample_rate(cfg, devAddr, c->sample_rate) ||
        mpu6050_set_sleep(cfg, devAddr, c->sleep_cfg)) {
        return MPU6050_STATUS_ERROR;
    }

    return MPU6050_STATUS_SUCCESS;
}
