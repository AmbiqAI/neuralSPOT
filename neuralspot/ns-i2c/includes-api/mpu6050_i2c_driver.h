#ifndef AI_MPU
#define AI_MPU


#ifdef __cplusplus
extern "C"
{
#endif

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

#define AM_HAL_MAGIC_IOM            0x123456
#define AM_HAL_IOM_CHK_HANDLE(h)    ((h) && ((am_hal_handle_prefix_t *)(h))->s.bInit && (((am_hal_handle_prefix_t *)(h))->s.magic == AM_HAL_MAGIC_IOM))

typedef enum
{
    AM_DEVICES_mpu6050_STATUS_SUCCESS,
    AM_DEVICES_mpu6050_STATUS_ERROR
} am_devices_tma525_status_t;

typedef unsigned long                   rt_ubase_t;
typedef rt_ubase_t                      rt_size_t;
typedef signed   char                   rt_int8_t;
typedef signed   short                  rt_int16_t;
typedef signed   int                    rt_int32_t;
typedef unsigned char                   rt_uint8_t;
typedef unsigned short                  rt_uint16_t;
typedef unsigned int                    rt_uint32_t;

struct rt_i2c_msg
{
    uint16_t addr;
    uint16_t flags;
    uint16_t len;
    uint8_t* buf;
};

/*! Gyroscope full-scale range */
typedef enum {
    GYRO_FS_250DPS  = 0,  //!< +/- 250 º/s  -> 131 LSB/(º/s)
    GYRO_FS_500DPS  = 1,  //!< +/- 500 º/s  -> 65.5 LSB/(º/s)
    GYRO_FS_1000DPS = 2,  //!< +/- 1000 º/s -> 32.8 LSB/(º/s)
    GYRO_FS_2000DPS = 3   //!< +/- 2000 º/s -> 16.4 LSB/(º/s)
} gyro_fs_t;

/*! Accel full-scale range */
typedef enum {
    ACCEL_FS_2G  = 0,  //!< +/- 2 g  -> 16.384 LSB/g
    ACCEL_FS_4G  = 1,  //!< +/- 4 g  -> 8.192 LSB/g
    ACCEL_FS_8G  = 2,  //!< +/- 8 g  -> 4.096 LSB/g
    ACCEL_FS_16G = 3   //!< +/- 16 g -> 2.048 LSB/g
} accel_fs_t;

typedef enum {
    CLOCK_INTERNAL = 0,  //!< Internal oscillator: 20MHz for MPU6500 and 8MHz for MPU6050
    CLOCK_PLL      = 3,  //!< Selects automatically best pll source (recommended)
    CLOCK_KEEP_RESET = 7  //!< Stops the clock and keeps timing generator in reset
} clock_src_t;

#define AM_DEVICES_mpu6050_I2C_WR                    0x0000
#define AM_DEVICES_mpu6050_I2C_RD                    (1u << 0)
#define MPU_I2CADDRESS_AD0_LOW                       0x68
#define MPU_I2CADDRESS_AD0_HIGH                      0x69

/*******************************************************************************
 * MPU commom registers for all models
 ******************************************************************************/
#define MPU6050_RA_XA_OFFS_H        0x06
#define MPU6050_RA_XA_OFFS_L        0x07
#define MPU6050_RA_YA_OFFS_H        0x08
#define MPU6050_RA_YA_OFFS_L        0x09  
#define MPU6050_RA_ZA_OFFS_H        0x0A
#define MPU6050_RA_ZA_OFFS_L        0x0B
#define   XG_OFFSET_H  0x13
#define   XG_OFFSET_L  0x14
#define   YG_OFFSET_H  0x15
#define   YG_OFFSET_L  0x16
#define   ZG_OFFSET_H 0x17
#define   ZG_OFFSET_L  0x18
#define   SMPLRT_DIV   0x19  // [7:0]
//------------------------------------------------------------------------------
#define   CONFIG                     0x1A
#define   CONFIG_FIFO_MODE_BIT        6
#define   CONFIG_EXT_SYNC_SET_BIT    5  // [5:3]
#define   CONFIG_EXT_SYNC_SET_LENGTH  3
#define   CONFIG_DLPF_CFG_BIT        2  // [2:0]
#define   CONFIG_DLPF_CFG_LENGTH     3
//------------------------------------------------------------------------------
#define   GYRO_CONFIG               0x001B
#define   GCONFIG_XG_ST_BIT         7
#define   GCONFIG_YG_ST_BIT         6
#define   GCONFIG_ZG_ST_BIT         5
#define   GCONFIG_FS_SEL_BIT        4  // [4:3]
#define   GCONFIG_FS_SEL_LENGTH     2
#define   GCONFIG_FCHOICE_B         1  // [1:0]
#define   GCONFIG_FCHOICE_B_LENGTH  2
//------------------------------------------------------------------------------
#define   ACCEL_CONFIG           0x001C
#define   ACONFIG_XA_ST_BIT      7
#define   ACONFIG_YA_ST_BIT      6
#define   ACONFIG_ZA_ST_BIT      5
#define   ACONFIG_FS_SEL_BIT     4  // [4:3]
#define   ACONFIG_FS_SEL_LENGTH  2
#define   ACONFIG_HPF_BIT        2  // [2:0]
#define   ACONFIG_HPF_LENGTH     3
//------------------------------------------------------------------------------
#define   FF_THR        0x1D
#define   FF_DUR        0x1E
#define   MOTION_THR    0x1F  // [7:0] // MPU9250_REG_WOM_THR
#define   MOTION_DUR    0x20
#define   ZRMOTION_THR  0x21
#define   ZRMOTION_DUR  0x22
//------------------------------------------------------------------------------
#define   FIFO_EN            0x23
#define   FIFO_TEMP_EN_BIT   7
#define   FIFO_XGYRO_EN_BIT  6
#define   FIFO_YGYRO_EN_BIT  5
#define   FIFO_ZGYRO_EN_BIT  4
#define   FIFO_ACCEL_EN_BIT  3
#define   FIFO_SLV_2_EN_BIT  2
#define   FIFO_SLV_1_EN_BIT  1
#define   FIFO_SLV_0_EN_BIT  0
//------------------------------------------------------------------------------
#define   I2C_MST_CTRL                   0x24
#define   I2CMST_CTRL_MULT_EN_BIT        7
#define   I2CMST_CTRL_WAIT_FOR_ES_BIT    6
#define   I2CMST_CTRL_SLV_3_FIFO_EN_BIT  5
#define   I2CMST_CTRL_P_NSR_BIT          4
#define   I2CMST_CTRL_CLOCK_BIT          3  // [3:0]
#define   I2CMST_CTRL_CLOCK_LENGTH       4
//------------------------------------------------------------------------------
#define   I2C_SLV0_ADDR      0x25
#define   I2C_SLV_RNW_BIT    7  // same for all I2C_SLV registers
#define   I2C_SLV_ID_BIT     6  // [6:0]
#define   I2C_SLV_ID_LENGTH  7
//------------------------------------------------------------------------------
#define   I2C_SLV0_REG  0x26  // [7:0]
//------------------------------------------------------------------------------
#define   I2C_SLV0_CTRL        0x27
#define   I2C_SLV_EN_BIT       7  // same for all I2C_SLV registers
#define   I2C_SLV_BYTE_SW_BIT  6
#define   I2C_SLV_REG_DIS_BIT  5
#define   I2C_SLV_GRP_BIT      4
#define   I2C_SLV_LEN_BIT      3  // [3:0]
#define   I2C_SLV_LEN_LENGTH   4
//------------------------------------------------------------------------------
#define   I2C_SLV1_ADDR  0x28  // see SLV0 for bit defines
#define   I2C_SLV1_REG   0x29
#define   I2C_SLV1_CTRL  0x2A
#define   I2C_SLV2_ADDR  0x2B  // see SLV0 for bit defines
#define   I2C_SLV2_REG   0x2C
#define   I2C_SLV2_CTRL  0x2D
#define   I2C_SLV3_ADDR  0x2E  // see SLV0 for bit defines
#define   I2C_SLV3_REG   0x2F
#define   I2C_SLV3_CTRL  0x30
#define   I2C_SLV4_ADDR  0x31  // see SLV0 for bit defines
#define   I2C_SLV4_REG   0x32
#define   I2C_SLV4_DO    0x33  // [7:0]
//------------------------------------------------------------------------------
#define   I2C_SLV4_CTRL              0x34
#define   I2C_SLV4_EN_BIT            7
#define   I2C_SLV4_DONE_INT_BIT      6
#define   I2C_SLV4_REG_DIS_BIT       5
#define   I2C_SLV4_MST_DELAY_BIT     4  // [4:0]
#define   I2C_SLV4_MST_DELAY_LENGTH  5
//------------------------------------------------------------------------------
#define   I2C_SLV4_DI  0x35  // [7:0]
//------------------------------------------------------------------------------
#define   I2C_MST_STATUS                0x36
#define   I2CMST_STAT_PASS_THROUGH_BIT  7
#define   I2CMST_STAT_SLV4_DONE_BIT     6
#define   I2CMST_STAT_LOST_ARB_BIT      5
#define   I2CMST_STAT_SLV4_NACK_BIT     4
#define   I2CMST_STAT_SLV3_NACK_BIT     3
#define   I2CMST_STAT_SLV2_NACK_BIT     2
#define   I2CMST_STAT_SLV1_NACK_BIT     1
#define   I2CMST_STAT_SLV0_NACK_BIT     0
//------------------------------------------------------------------------------
#define   INT_PIN_CONFIG                 0x37
#define   INT_CFG_LEVEL_BIT              7
#define   INT_CFG_OPEN_BIT               6
#define   INT_CFG_LATCH_EN_BIT           5
#define   INT_CFG_ANYRD_2CLEAR_BIT       4
#define   INT_CFG_FSYNC_LEVEL_BIT        3
#define   INT_CFG_FSYNC_INT_MODE_EN_BIT  2
#define   INT_CFG_I2C_BYPASS_EN_BIT      1
#define   INT_CFG_CLOCKOUT_EN_BIT        0
//------------------------------------------------------------------------------
#define   INT_ENABLE                    0x38
#define   INT_ENABLE_FREEFALL_BIT       7
#define   INT_ENABLE_MOTION_BIT         6
#define   INT_ENABLE_ZEROMOT_BIT        5
#define   INT_ENABLE_FIFO_OFLOW_BIT     4
#define   INT_ENABLE_I2C_MST_FSYNC_BIT  3
#define   INT_ENABLE_PLL_RDY_BIT        2
#define   INT_ENABLE_DMP_RDY_BIT        1
#define   INT_ENABLE_RAW_DATA_RDY_BIT   0
//------------------------------------------------------------------------------
#define   DMP_INT_STATUS    0x39
#define   DMP_INT_STATUS_0  0
#define   DMP_INT_STATUS_1  1
#define   DMP_INT_STATUS_2  2
#define   DMP_INT_STATUS_3  3
#define   DMP_INT_STATUS_4  4
#define   DMP_INT_STATUS_5  5
//------------------------------------------------------------------------------
#define   INT_STATUS                   0x3A
#define   INT_STATUS_FREEFALL_BIT      7
#define   INT_STATUS_MOTION_BIT        6
#define   INT_STATUS_ZEROMOT_BIT       5
#define   INT_STATUS_FIFO_OFLOW_BIT    4
#define   INT_STATUS_I2C_MST_BIT       3
#define   INT_STATUS_PLL_RDY_BIT       2
#define   INT_STATUS_DMP_RDY_BIT       1
#define   INT_STATUS_RAW_DATA_RDY_BIT  0
//------------------------------------------------------------------------------
#define   ACCEL_XOUT_H      0x3B  // [15:0]
#define   ACCEL_XOUT_L      0x3C
#define   ACCEL_YOUT_H      0x3D  // [15:0]
#define   ACCEL_YOUT_L      0x3E
#define   ACCEL_ZOUT_H      0x3F  // [15:0]
#define   ACCEL_ZOUT_L      0x40
#define   TEMP_OUT_H        0x41  // [15:0]
#define   TEMP_OUT_L        0x42
#define   GYRO_XOUT_H       0x43  // [15:0]
#define   GYRO_XOUT_L       0x44
#define   GYRO_YOUT_H       0x45  // [15:0]
#define   GYRO_YOUT_L       0x46
#define   GYRO_ZOUT_H       0x47  // [15:0]
#define   GYRO_ZOUT_L       0x48
#define   EXT_SENS_DATA_00  0x49  // Stores data read from Slave 0, 1, 2, and 3
#define   EXT_SENS_DATA_01  0x4A
#define   EXT_SENS_DATA_02  0x4B
#define   EXT_SENS_DATA_03  0x4C
#define   EXT_SENS_DATA_04  0x4D
#define   EXT_SENS_DATA_05  0x4E
#define   EXT_SENS_DATA_06  0x4F
#define   EXT_SENS_DATA_07  0x50
#define   EXT_SENS_DATA_08  0x51
#define   EXT_SENS_DATA_09  0x52
#define   EXT_SENS_DATA_10  0x53
#define   EXT_SENS_DATA_11  0x54
#define   EXT_SENS_DATA_12  0x55
#define   EXT_SENS_DATA_13  0x56
#define   EXT_SENS_DATA_14  0x57
#define   EXT_SENS_DATA_15  0x58
#define   EXT_SENS_DATA_17  0x5A
#define   EXT_SENS_DATA_18  0x5B
#define   EXT_SENS_DATA_19  0x5C
#define   EXT_SENS_DATA_20  0x5D
#define   EXT_SENS_DATA_21  0x5E
#define   EXT_SENS_DATA_22  0x5F
#define   EXT_SENS_DATA_23  0x60
#define   I2C_SLV0_DO       0x63
#define   I2C_SLV1_DO       0x64
#define   I2C_SLV2_DO       0x65
#define   I2C_SLV3_DO       0x66
//------------------------------------------------------------------------------
#define   I2C_MST_DELAY_CRTL        0x67
#define   I2CMST_DLY_ES_SHADOW_BIT  7
#define  I2CMST_DLY_SLV4_EN_BIT    4
#define  I2CMST_DLY_SLV3_EN_BIT    3
#define  I2CMST_DLY_SLV2_EN_BIT    2
#define  I2CMST_DLY_SLV1_EN_BIT    1
#define  I2CMST_DLY_SLV0_EN_BIT    0
//------------------------------------------------------------------------------
#define  SIGNAL_PATH_RESET    0x68
#define  SPATH_GYRO_RST_BIT   2
#define  SPATH_ACCEL_RST_BIT  1
#define  SPATH_TEMP_RST_BIT   0 
//------------------------------------------------------------------------------
#define  USER_CTRL                    0x6A
#define  USERCTRL_DMP_EN_BIT          7
#define  USERCTRL_FIFO_EN_BIT         6
#define  USERCTRL_I2C_MST_EN_BIT      5
#define  USERCTRL_I2C_IF_DIS_BIT      4
#define  USERCTRL_DMP_RESET_BIT       3
#define  USERCTRL_FIFO_RESET_BIT      2
#define  USERCTRL_I2C_MST_RESET_BIT   1
#define  USERCTRL_SIG_COND_RESET_BIT  0
//------------------------------------------------------------------------------
#define PWR_MGMT1     0x006B             
#define  PWR1_DEVICE_RESET_BIT  7
#define  PWR1_SLEEP_BIT         6
#define  PWR1_CYCLE_BIT         5
#define  PWR1_GYRO_STANDBY_BIT  4
#define PWR1_TEMP_DIS_BIT      3
#define PWR1_CLKSEL_BIT      2
#define PWR1_CLKSEL_LENGTH 3
//------------------------------------------------------------------------------
#define  PWR_MGMT2                 0x6C
#define  PWR2_LP_WAKE_CTRL_BIT     7
#define  PWR2_LP_WAKE_CTRL_LENGTH  2
#define  PWR2_STBY_XA_BIT          5
#define  PWR2_STBY_YA_BIT          4
#define  PWR2_STBY_ZA_BIT          3
#define  PWR2_STBY_XG_BIT          2
#define  PWR2_STBY_YG_BIT          1
#define  PWR2_STBY_ZG_BIT          0
#define  PWR2_STBY_XYZA_BITS       1 << PWR2_STBY_XA_BIT | 1 << PWR2_STBY_YA_BIT | 1 << PWR2_STBY_ZA_BIT
#define  PWR2_STBY_XYZG_BITS       1 << PWR2_STBY_XG_BIT | 1 << PWR2_STBY_YG_BIT | 1 << PWR2_STBY_ZG_BIT
//------------------------------------------------------------------------------
#define  BANK_SEL                   0x6D
#define  BANKSEL_PRFTCH_EN_BIT      6
#define  BANKSEL_CFG_USER_BANK_BIT  5
#define  BANKSEL_MEM_SEL_BIT        4
#define  BANKSEL_MEM_SEL_LENGTH     5
//------------------------------------------------------------------------------
#define  MEM_START_ADDR  0x6E
#define  MEM_R_W         0x6F
#define  PRGM_START_H    0x70
#define  PRGM_START_L    0x71
#define  FIFO_COUNT_H    0x72  // [15:0]
#define  FIFO_COUNT_L    0x73
#define  FIFO_R_W        0x74
#define  WHO_AM_I        0x75

extern uint32_t
read_sensors(uint8_t *buffer);

extern uint32_t
rotation(int16_t* x, int16_t* y, int16_t* z, rt_uint8_t *buffer);

extern void 
accelGravity(float* store, int x, int y, int z, accel_fs_t range);

extern void 
gyroDegPerSec(float* store, int x, int y, int z, gyro_fs_t range);

extern uint32_t mpu6050_finish_init(void *h);

extern uint32_t setAccelOffsetX(int offset);

extern uint32_t setAccelOffsetY(int offset);

extern uint32_t setAccelOffsetZ(int offset);

extern uint32_t setGyroOffsetX(int offset);

extern uint32_t setGyroOffsetY(int offset);

extern uint32_t setGyroOffsetZ(int offset);

extern uint32_t meanSensors(int* meanAX, int* meanAY, int* meanAZ, int* meanGX, int* meanGY, int* meanGZ);

extern uint32_t calibrate(int meanAX, int meanAY, int meanAZ, int meanGX, int meanGY, int meanGZ);

extern uint32_t mpu6050_calibration();

extern int8_t getLowBits(int16_t regReading);

extern int8_t getHighBits(int16_t regReading);
#ifdef __cplusplus
}
#endif


#endif // MPU6050