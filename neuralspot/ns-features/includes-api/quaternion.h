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
#include "ns_core.h"
#define NS_MAHONY_V0_0_1                                                                          \
    { .major = 0, .minor = 0, .revision = 1 }
#define NS_MAHONY_OLDEST_SUPPORTED_VERSION NS_MAHONY_V0_0_1
#define NS_MAHONY_CURRENT_VERSION NS_MAHONY_V0_0_1
#define NS_MAHONY_API_ID 0xCA0009

extern const ns_core_api_t ns_mahony_V0_0_1;
extern const ns_core_api_t ns_mahony_oldest_supported_version;
extern const ns_core_api_t ns_mahony_current_version;

typedef struct {
  const ns_core_api_t *api;
  float q0;
  float q1;
  float q2;
  float q3;
  float integralFBx;
  float integralFBy;
  float integralFBz;
} ns_mahony_cfg_t;

// #include "mpu6050registers.h"
#define mahonysampleFreq 61.0f // sample frequency in Hz
#define mahonytwoKpDef (2.0f * 0.5f) // 2 * proportional gain
#define mahonytwoKiDef (2.0f * 0.1f) // 2 * integral gain
extern uint16_t ns_mahony_init(ns_mahony_cfg_t *cfg);
extern uint16_t ns_mahony_update(ns_mahony_cfg_t *cfg, float gx, float gy, float gz, float ax, float ay, float az);
extern uint16_t ns_get_quaternion(ns_mahony_cfg_t *cfg,double *qw, double *qx, double *qy, double *qz);
extern uint16_t ns_get_RollPitchYaw(ns_mahony_cfg_t *cfg, double *pitch, double *roll, double *yaw);

#ifdef __cplusplus
}
#endif
#endif