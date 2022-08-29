/**
 * @file mpu6050_i2c_driver.c
 * @author Rohan
 * @brief Driver of MPU6050
 * @version 0.1
 * @date 2022-08-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdlib.h>
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
//#include "am_hal_iom.h"
#include "ns_i2c_register_driver.h"
#include "mpu6050_i2c_driver.h"
#include <limits.h>

static void *mpu6050Handle;

int8_t 
getHighBits(int16_t regReading) {
    return (regReading & 65280) >> 8;
}

int8_t
getLowBits(int16_t regReading) {
    return regReading & 255;
}

uint32_t
setSampleRate(uint16_t rate) {
    if (rate < 4) {
        rate = 4;
    } else if (rate > 1000){
        rate = 1000;
    }

    uint32_t internalSampleRate = 1000;
    uint32_t divider = internalSampleRate / rate - 1;
    // Check for rate match
    // uint32_t finalRate = (internalSampleRate / (1 + divider));
    // Write divider to register
    return ns_i2c_reg_write(mpu6050Handle, SMPLRT_DIV,  &divider, 1); 
}

uint32_t
testConnection() {
    uint32_t regValue;
    if (ns_i2c_reg_read(mpu6050Handle, WHO_AM_I, &regValue, 1)) {
        return AM_DEVICES_mpu6050_STATUS_ERROR;
    }
    return (int) regValue == 0x68;
}

uint32_t
setAccelFullScale(accel_fs_t fsr)
{
    return ns_i2c_write_bits(mpu6050Handle, ACCEL_CONFIG, fsr, ACONFIG_FS_SEL_BIT, ACONFIG_FS_SEL_LENGTH);
}

uint32_t
setGyroFullScale(accel_fs_t fsr)
{
    return ns_i2c_write_bits(mpu6050Handle, GYRO_CONFIG,  fsr, GCONFIG_FS_SEL_BIT, GCONFIG_FS_SEL_LENGTH);
}

uint32_t
mpu6050_finish_init(void *h)
{    
    mpu6050Handle = h;

    // Toggle reset bit, then set sleep and clock bits
    uint32_t val = 0x80;
    ns_i2c_reg_write(mpu6050Handle, PWR_MGMT1, &val,1);
    am_hal_delay_us(10000);

    val = 0x03; int i;
    for (i=0;i<1;i++) {
        ns_i2c_reg_write(mpu6050Handle, PWR_MGMT1, &val,1);
        ns_i2c_reg_read(mpu6050Handle, PWR_MGMT1, &val, 1);
        am_util_stdio_printf("PWR VAL: 0x%x\n", val); 
        am_hal_delay_us(100000);
        ns_i2c_reg_write(mpu6050Handle, CONFIG, &val,1);
    }

    ns_i2c_reg_read(mpu6050Handle, WHO_AM_I, &val, 1);
    am_util_stdio_printf("MPU ID: 0x%x\n", val); 

    am_hal_delay_us(100000);

    if (setGyroFullScale(GYRO_FS_500DPS)|| setAccelFullScale(ACCEL_FS_4G) || setSampleRate(20)) {
        return AM_DEVICES_mpu6050_STATUS_ERROR;
    } 

    return AM_DEVICES_mpu6050_STATUS_SUCCESS;
}

uint32_t
read_sensors(uint8_t *buffer) {
    if (ns_i2c_reg_read(mpu6050Handle, ACCEL_XOUT_H, buffer,14))
    {
        return AM_DEVICES_mpu6050_STATUS_ERROR;
    }

    if (buffer[12] != 0xfe || buffer[13] != 0xfe) {
        // Occasionally, the MPU returns 0xfefe on last two registers
        // throw those values out if you like
        return AM_DEVICES_mpu6050_STATUS_SUCCESS;
    } else {
        return AM_DEVICES_mpu6050_STATUS_ERROR;
    }
}
//Math functions to convert raw sensor data to useful data
uint8_t accelFSRvalue(accel_fs_t range)
{
    return 2 << range;
}
float accelResolution(accel_fs_t range) 
{
    return (float)accelFSRvalue(range) / (float)INT16_MAX;
}

void accelGravity(float* store, int x, int y, int z, accel_fs_t range) {
    store[0] = (float)x * accelResolution(range);
    store[1] = (float)y * accelResolution(range);
    store[2] = (float)z * accelResolution(range);
}

uint8_t gyroFSRvalue(const gyro_fs_t range)
{
    return 250 << range;
}

float gyroResolution(const gyro_fs_t range)
{
    return (float)gyroFSRvalue(range) / (float)INT16_MAX;
}

void gyroDegPerSec(float* store, int x, int y, int z, gyro_fs_t range)
{
    store[0] = (float)x * gyroResolution(range);
    store[1] = (float)y * gyroResolution(range);
    store[2] = (float)z * gyroResolution(range);
}

uint32_t setAccelOffsetX(int offset) {
    int8_t low = getLowBits(offset);
    int8_t high = getHighBits(offset);
    return (ns_i2c_reg_write(mpu6050Handle, MPU6050_RA_XA_OFFS_H, &high, 1) ||
        ns_i2c_reg_write(mpu6050Handle, MPU6050_RA_XA_OFFS_L , &low, 1));
}

uint32_t setAccelOffsetY(int offset) {
    int8_t low = getLowBits(offset);
    int8_t high = getHighBits(offset);
    return (ns_i2c_reg_write(mpu6050Handle, MPU6050_RA_YA_OFFS_H, &high, 1) ||
        ns_i2c_reg_write(mpu6050Handle, MPU6050_RA_YA_OFFS_L , &low, 1));
}

uint32_t setAccelOffsetZ(int offset) {
    int8_t low = getLowBits(offset);
    int8_t high = getHighBits(offset);
    return (ns_i2c_reg_write(mpu6050Handle, MPU6050_RA_ZA_OFFS_H, &high, 1) ||
        ns_i2c_reg_write(mpu6050Handle, MPU6050_RA_ZA_OFFS_L , &low, 1));
}

uint32_t setGyroOffsetX(int offset) {
    int8_t low = getLowBits(offset);
    int8_t high = getHighBits(offset);
    return (ns_i2c_reg_write(mpu6050Handle, XG_OFFSET_H , &high, 1) ||
        ns_i2c_reg_write(mpu6050Handle, XG_OFFSET_L , &low, 1));
}

uint32_t setGyroOffsetY(int offset) {
    int8_t low = getLowBits(offset);
    int8_t high = getHighBits(offset);
    return (ns_i2c_reg_write(mpu6050Handle, YG_OFFSET_H , &high, 1) ||
        ns_i2c_reg_write(mpu6050Handle, YG_OFFSET_L , &low, 1));
}

uint32_t setGyroOffsetZ(int offset) {
    int8_t low = getLowBits(offset);
    int8_t high = getHighBits(offset);
    return (ns_i2c_reg_write(mpu6050Handle, ZG_OFFSET_H , &high, 1) ||
        ns_i2c_reg_write(mpu6050Handle, ZG_OFFSET_L , &low, 1));
}

uint32_t meanSensors(int* meanAX, int* meanAY, int* meanAZ, int* meanGX, int* meanGY, int* meanGZ) {
     int numReadings = 1000;
     long i=0,sum_ax=0,sum_ay=0,sum_az=0,sum_gx=0,sum_gy=0,sum_gz=0;
     int16_t ax, ay, az, gx, gy, gz;
     uint8_t buffer[32];
     while (i<(numReadings+101)){
        
        if(read_sensors(buffer)) {
            return AM_DEVICES_mpu6050_STATUS_ERROR;
        }

        ax = buffer[0] << 8 | buffer[1];
        ay = buffer[2] << 8 | buffer[3];
        az = buffer[4] << 8 | buffer[5];

        gx = buffer[8] << 8 | buffer[9];
        gy = buffer[10] << 8 | buffer[11];
        gz = buffer[12] << 8 | buffer[13];
    
        if (i>101 && i<(numReadings+101)){ //First 100 measures are discarded
        sum_ax=sum_ax+ax;
        sum_ay=sum_ay+ay;
        sum_az=sum_az+az;
        sum_gx=sum_gx+gx;
        sum_gy=sum_gy+gy;
        sum_gz=sum_gz+gz;
        }
        if (i==(numReadings+100)){
        *meanAX=sum_ax/numReadings;
        *meanAY=sum_ay/numReadings;
        *meanAZ=sum_az/numReadings;
        *meanGX=sum_gx/numReadings;
        *meanGY=sum_gy/numReadings;
        *meanGZ=sum_gz/numReadings;
        }
        i++;
        am_hal_delay_us(2); //Needed so we don't get repeated measures
    }
    return AM_DEVICES_mpu6050_STATUS_SUCCESS;
}
uint32_t calibrate(int meanAX, int meanAY, int meanAZ, int meanGX, int meanGY, int meanGZ) {
  int ready=0;
  int acel_deadzone=8; 
  int gyro_deadzone=1; 
  int ax_offset=-meanAX/8;
  int ay_offset=-meanAY/8;
  int az_offset=(16384-meanAZ)/8;

  int gx_offset=-meanGX/4;
  int gy_offset=-meanGY;
  int gz_offset=-meanGZ/4;
  while (1){
    if(setAccelOffsetX(ax_offset)||
    setAccelOffsetY(ay_offset)||
    setAccelOffsetZ(az_offset)||
    setGyroOffsetX(gx_offset)||
    setGyroOffsetY(gy_offset)||
    setGyroOffsetZ(gz_offset)) 
    {
        return AM_DEVICES_mpu6050_STATUS_ERROR;
    }
    if(meanSensors(&meanAX, &meanAY, &meanAZ, &meanGX, &meanGY, &meanGZ)) {
        return AM_DEVICES_mpu6050_STATUS_ERROR;
    }

    if (abs(meanAX)<=acel_deadzone) ready++;
    else ax_offset=ax_offset-meanAX/acel_deadzone;

    if (abs(meanAY)<=acel_deadzone) ready++;
    else ay_offset=ay_offset-meanAY/acel_deadzone;

    if (abs(16384-meanAZ)<=acel_deadzone) ready++;
    else az_offset=az_offset+(16384-meanAZ)/acel_deadzone;

    if (abs(meanGX)<=gyro_deadzone) ready++;
    else gx_offset=gx_offset-meanGX/(gyro_deadzone+1);

    if (abs(meanGY)<=gyro_deadzone) ready++;
    else gy_offset= 2 * gy_offset-meanGY/(gyro_deadzone+1);

    if (abs(meanGZ)<=gyro_deadzone) ready++;
    else gz_offset=gz_offset-meanGZ/(gyro_deadzone+1);

    if (ready>=10) break;
  }
  return AM_DEVICES_mpu6050_STATUS_SUCCESS;
}

uint32_t mpu6050_calibration() {
    if(setAccelOffsetX(0)||
    setAccelOffsetY(0)||
    setAccelOffsetZ(0)||
    setGyroOffsetX(0)||
    setGyroOffsetY(0)||
    setGyroOffsetZ(0)) {
        return AM_DEVICES_mpu6050_STATUS_ERROR;
    }
    int meanAX;
    int meanAY;
    int meanAZ;
    int meanGX;
    int meanGY;
    int meanGZ;
    int i = 0;
    am_util_stdio_printf("Calibrating...\n");
    while (i < 2) {
        if (i==0){
            if(meanSensors(&meanAX, &meanAY, &meanAZ, &meanGX, &meanGY, &meanGZ)) {
                return AM_DEVICES_mpu6050_STATUS_ERROR;
            }
            i++;
            am_hal_delay_us(1000);
        }

        if (i==1) {
            if(calibrate(meanAX, meanAY, meanAZ, meanGX, meanGY, meanGZ)) {
                return AM_DEVICES_mpu6050_STATUS_ERROR;
            }
            i++;
            am_hal_delay_us(1000);
        }
    }
    return AM_DEVICES_mpu6050_STATUS_SUCCESS;
}