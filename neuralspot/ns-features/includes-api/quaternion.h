/*
MPU6050 lib 0x05

copyright (c) Davide Gironi, 2012

Released under GPLv3.
Please refer to LICENSE file for licensing information.

References:
  - most of the code is a port of the arduino mpu6050 library by Jeff Rowberg
    https://github.com/jrowberg/i2cdevlib
  - Mahony complementary filter for attitude estimation
    http://www.x-io.co.uk
*/


#ifndef QUATERNION_H
#define QUATERNION_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

// #include "mpu6050registers.h"
#define mahonysampleFreq 61.0f // sample frequency in Hz
#define mahonytwoKpDef (2.0f * 0.5f) // 2 * proportional gain
#define mahonytwoKiDef (2.0f * 0.1f) // 2 * integral gain

extern void mahonyUpdate(float gx, float gy, float gz, float ax, float ay, float az);
extern void getQuaternion(double *qw, double *qx, double *qy, double *qz);
extern void getRollPitchYaw(double *pitch, double *roll, double *yaw);

#ifdef __cplusplus
}
#endif
#endif