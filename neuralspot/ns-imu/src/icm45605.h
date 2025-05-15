
//
// This reference design is intended for demo purposes only.
//
#ifndef ICM45605_H
#define ICM45605_H
#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif



int8_t ICM45605_load_configs(uint8_t *buff);
int8_t ICM45605_get_accel_gyro(uint8_t *buff);
int8_t ICM45605_forced_mode(uint8_t *buff);

/**
 * @brief 6DOF IMU 16 description register.
 * @details Specified register for description of 6DOF IMU 16 Click driver.
 */
#define ICM45605_REG_ACCEL_DATA_X1_UI                         0x00
#define ICM45605_REG_ACCEL_DATA_X0_UI                         0x01
#define ICM45605_REG_ACCEL_DATA_Y1_UI                         0x02
#define ICM45605_REG_ACCEL_DATA_Y0_UI                         0x03
#define ICM45605_REG_ACCEL_DATA_Z1_UI                         0x04
#define ICM45605_REG_ACCEL_DATA_Z0_UI                         0x05
#define ICM45605_REG_GYRO_DATA_X1_UI                          0x06
#define ICM45605_REG_GYRO_DATA_X0_UI                          0x07
#define ICM45605_REG_GYRO_DATA_Y1_UI                          0x08
#define ICM45605_REG_GYRO_DATA_Y0_UI                          0x09
#define ICM45605_REG_GYRO_DATA_Z1_UI                          0x0A
#define ICM45605_REG_GYRO_DATA_Z0_UI                          0x0B
#define ICM45605_REG_TEMP_DATA1_UI                            0x0C
#define ICM45605_REG_TEMP_DATA0_UI                            0x0D
#define ICM45605_REG_TMST_FSYNCH                              0x0E
#define ICM45605_REG_TMST_FSYNCL                              0x0F
#define ICM45605_REG_PWR_MGMNT0                               0x10
#define ICM45605_REG_FIFO_COUNT_0                             0x12
#define ICM45605_REG_FIFO_COUNT_1                             0x13
#define ICM45605_REG_FIFO_DATA                                0x14
#define ICM45605_REG_INT1_CONFIG0                             0x16
#define ICM45605_REG_INT1_CONFIG1                             0x17
#define ICM45605_REG_INT1_CONFIG2                             0x18
#define ICM45605_REG_INT1_STATUS0                             0x19
#define ICM45605_REG_INT1_STATUS1                             0x1A
#define ICM45605_REG_ACCEL_CONFIG0                            0x1B
#define ICM45605_REG_GYRO_CONFIG0                             0x1C
#define ICM45605_REG_FIFO_CONFIG0                             0x1D
#define ICM45605_REG_FIFO_CONFIG1_0                           0x1E
#define ICM45605_REG_FIFO_CONFIG1_1                           0x1F
#define ICM45605_REG_FIFO_CONFIG2                             0x20
#define ICM45605_REG_FIFO_CONFIG3                             0x21
#define ICM45605_REG_FIFO_CONFIG4                             0x22
#define ICM45605_REG_TMST_WOM_CONFIG                          0x23
#define ICM45605_REG_FSYNC_CONFIG0                            0x24
#define ICM45605_REG_DMP_EXT_SEN_ODR_CFG                      0x27
#define ICM45605_REG_ODR_DECIMATE_CONFIG                      0x28
#define ICM45605_REG_EDMP_APEX_EN0                            0x29
#define ICM45605_REG_EDMP_APEX_EN1                            0x2A
#define ICM45605_REG_APEX_BUFFER_MGMT                         0x2B
#define ICM45605_REG_INTF_CONFIG0                             0x2C
#define ICM45605_REG_INTF_CONFIG1_OVRD                        0x2D
#define ICM45605_REG_IOC_PAD_SCENARIO                         0x2F
#define ICM45605_REG_IOC_PAD_SCENARIO_AUX_OVRD                0x30
#define ICM45605_REG_IOC_PAD_SCENARIO_OVRD                    0x31
#define ICM45605_REG_DRIVE_CONFIG0                            0x32
#define ICM45605_REG_DRIVE_CONFIG1                            0x33
#define ICM45605_REG_DRIVE_CONFIG2                            0x34
#define ICM45605_REG_MISC1                                    0x35
#define ICM45605_REG_INT_APEX_CONFIG0                         0x39
#define ICM45605_REG_INT_APEX_CONFIG1                         0x3A
#define ICM45605_REG_INT_APEX_STATUS0                         0x3B
#define ICM45605_REG_INT_APEX_STATUS1                         0x3C
#define ICM45605_REG_INT2_CONFIG0                             0x56
#define ICM45605_REG_INT2_CONFIG1                             0x57
#define ICM45605_REG_INT2_CONFIG2                             0x58
#define ICM45605_REG_INT2_STATUS0                             0x59
#define ICM45605_REG_INT2_STATUS1                             0x5A
#define ICM45605_REG_WHO_AM_I                                 0x72
#define ICM45605_REG_REG_HOST_MSG                             0x73
#define ICM45605_REG_IREG_ADDR_15_8                           0x7C
#define ICM45605_REG_IREG_ADDR_7_0                            0x7D
#define ICM45605_REG_IREG_DATA                                0x7E
#define ICM45605_REG_REG_MISC2                                0x7F

/*! @} */ // ICM45605_reg

/**
 * @defgroup ICM45605_set 6DOF IMU 16 Registers Settings
 * @brief Settings for registers of 6DOF IMU 16 Click driver.
 */

/**
 * @addtogroup ICM45605_set
 * @{
 */

/**
 * @brief 6DOF IMU 16 description setting.
 * @details Specified setting for description of 6DOF IMU 16 Click driver.
 */
#define ICM45605_GYRO_MODE_OFF                                0x00
#define ICM45605_GYRO_MODE_STANDBY                            0x04
#define ICM45605_GYRO_MODE_LOW_POWER                          0x08
#define ICM45605_GYRO_MODE_LOW_NOISE                          0x0C
#define ICM45605_ACCEL_MODE_OFF                               0x00
#define ICM45605_ACCEL_MODE_STANDBY                           0x01
#define ICM45605_ACCEL_MODE_LOW_POWER                         0x02
#define ICM45605_ACCEL_MODE_LOW_NOISE                         0x03

/**
 * @brief 6DOF IMU 16 gyro full-scale setting.
 * @details Specified setting for gyro full-scale of 6DOF IMU 16 Click driver.
 */
#define ICM45605_GYRO_UI_FS_SEL_MASK                          0xF0
#define ICM45605_GYRO_UI_FS_SEL_2000DPS                       0x10
#define ICM45605_GYRO_UI_FS_SEL_1000DPS                       0x20
#define ICM45605_GYRO_UI_FS_SEL_500DPS                        0x30
#define ICM45605_GYRO_UI_FS_SEL_250DPS                        0x40
#define ICM45605_GYRO_UI_FS_SEL_125DPS                        0x50
#define ICM45605_GYRO_UI_FS_SEL_62_5DPS                       0x60
#define ICM45605_GYRO_UI_FS_SEL_31_25DPS                      0x70
#define ICM45605_GYRO_UI_FS_SEL_15_625DPS                     0x80

/**
 * @brief 6DOF IMU 16 accel full-scale setting.
 * @details Specified setting for accel full-scale of 6DOF IMU 16 Click driver.
 */
#define ICM45605_ACCEL_UI_FS_SEL_MASK                         0x70
#define ICM45605_ACCEL_UI_FS_SEL_16g                          0x10
#define ICM45605_ACCEL_UI_FS_SEL_8g                           0x20
#define ICM45605_ACCEL_UI_FS_SEL_4g                           0x30
#define ICM45605_ACCEL_UI_FS_SEL_2g                           0x40

/**
 * @brief 6DOF IMU 16 accel ODR setting.
 * @details Specified setting for accel ODR of 6DOF IMU 16 Click driver.
 */
#define ICM45605_ACCEL_ODR_6_4_KHZ                            0x03
#define ICM45605_ACCEL_ODR_3_2_KHZ                            0x04
#define ICM45605_ACCEL_ODR_1_6_KHZ                            0x05
#define ICM45605_ACCEL_ODR_800_HZ                             0x06
#define ICM45605_ACCEL_ODR_400_HZ                             0x07
#define ICM45605_ACCEL_ODR_200_HZ                             0x08
#define ICM45605_ACCEL_ODR_100_HZ                             0x09
#define ICM45605_ACCEL_ODR_50_HZ                              0x0A
#define ICM45605_ACCEL_ODR_25_HZ                              0x0B
#define ICM45605_ACCEL_ODR_12_5_HZ                            0x0C
#define ICM45605_ACCEL_ODR_6_25_HZ                            0x0D
#define ICM45605_ACCEL_ODR_3_125_HZ                           0x0E
#define ICM45605_ACCEL_ODR_1_5625_HZ                          0x0F

/**
 * @brief 6DOF IMU 16 gyro ODR setting.
 * @details Specified setting for gyro ODR of 6DOF IMU 16 Click driver.
 */
#define ICM45605_GYRO_ODR_6_4_KHZ                             0x03
#define ICM45605_GYRO_ODR_3_2_KHZ                             0x04
#define ICM45605_GYRO_ODR_1_6_KHZ                             0x05
#define ICM45605_GYRO_ODR_800_HZ                              0x06
#define ICM45605_GYRO_ODR_400_HZ                              0x07
#define ICM45605_GYRO_ODR_200_HZ                              0x08
#define ICM45605_GYRO_ODR_100_HZ                              0x09
#define ICM45605_GYRO_ODR_50_HZ                               0x0A
#define ICM45605_GYRO_ODR_25_HZ                               0x0B
#define ICM45605_GYRO_ODR_12_5_HZ                             0x0C
#define ICM45605_GYRO_ODR_6_25_HZ                             0x0D
#define ICM45605_GYRO_ODR_3_125_HZ                            0x0E
#define ICM45605_GYRO_ODR_1_5625_HZ                           0x0F

/**
 * @brief 6DOF IMU 16 software reset setting.
 * @details Specified setting for software reset of 6DOF IMU 16 Click driver.
 */
#define ICM45605_SOFTWARE_RESET                               0x02

/**
 * @brief 6DOF IMU 16 device ID setting.
 * @details Specified setting for device ID of 6DOF IMU 16 Click driver.
 */
#define ICM45605_DEVICE_ID                                    0xE5

/**
 * @brief 6DOF IMU 16 data conversion factor setting.
 * @details The specified value of data conversion factor of
 * 6DOF IMU 16 Click driver.
 */
#define ICM45605_ACCEL_2G_CONV_MUL                            16384u
#define ICM45605_ACCEL_4G_CONV_MUL                            8192u
#define ICM45605_ACCEL_8G_CONV_MUL                            4096u
#define ICM45605_ACCEL_16G_CONV_MUL                           2048u
#define ICM45605_GYRO_2000DPS_CONV_MUL                        16.4f
#define ICM45605_GYRO_1000DPS_CONV_MUL                        32.8f
#define ICM45605_GYRO_500DPS_CONV_MUL                         65.5f
#define ICM45605_GYRO_250DPS_CONV_MUL                         131.0f
#define ICM45605_GYRO_125DPS_CONV_MUL                         262.0f
#define ICM45605_GYRO_62_5DPS_CONV_MUL                        524.3f
#define ICM45605_GYRO_31_25DPS_CONV_MUL                       1048.6f
#define ICM45605_GYRO_15_625DPS_CONV_MUL                      2097.2f

/**
 * @brief 6DOF IMU 16 device address setting.
 * @details Specified setting for device slave address selection of
 * 6DOF IMU 16 Click driver.
 */
#define ICM45605_DEVICE_ADDRESS_0                             0x68
#define ICM45605_DEVICE_ADDRESS_1                             0x69


#ifdef __cplusplus
}
#endif
#endif
