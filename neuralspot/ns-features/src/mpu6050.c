/*
MPU6050 lib 0x05

copyright (c) Davide Gironi, 2012

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/


#include <stdlib.h>
#include <string.h>
#include "mpu6050.h"
#include <stdio.h>
#include <assert.h>
#include <math.h> 

// path to i2c fleury lib
// #include MPU6050_I2CFLEURYPATH

volatile uint8_t buffer[14];

 volatile float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;
 volatile float integralFBx = 0.0f,  integralFBy = 0.0f, integralFBz = 0.0f;
/*
 * Mahony update function (for 6DOF)
 */
void mpu6050_mahonyUpdate(float gx, float gy, float gz, float ax, float ay, float az) {
	float norm;
	float halfvx, halfvy, halfvz;
	float halfex, halfey, halfez;
	float qa, qb, qc;

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

		// Normalise accelerometer measurement
		norm = sqrt(ax * ax + ay * ay + az * az);
		ax /= norm;
		ay /= norm;
		az /= norm;

		// Estimated direction of gravity and vector perpendicular to magnetic flux
		halfvx = q1 * q3 - q0 * q2;
		halfvy = q0 * q1 + q2 * q3;
		halfvz = q0 * q0 - 0.5f + q3 * q3;

		// Error is sum of cross product between estimated and measured direction of gravity
		halfex = (ay * halfvz - az * halfvy);
		halfey = (az * halfvx - ax * halfvz);
		halfez = (ax * halfvy - ay * halfvx);

		// Compute and apply integral feedback if enabled
		if(mpu6050_mahonytwoKiDef > 0.0f) {
			integralFBx += mpu6050_mahonytwoKiDef * halfex * (1.0f / mpu6050_mahonysampleFreq);	// integral error scaled by Ki
			integralFBy += mpu6050_mahonytwoKiDef * halfey * (1.0f / mpu6050_mahonysampleFreq);
			integralFBz += mpu6050_mahonytwoKiDef * halfez * (1.0f / mpu6050_mahonysampleFreq);
			gx += integralFBx;	// apply integral feedback
			gy += integralFBy;
			gz += integralFBz;
		} else {
			integralFBx = 0.0f;	// prevent integral windup
			integralFBy = 0.0f;
			integralFBz = 0.0f;
		}

		// Apply proportional feedback
		gx += mpu6050_mahonytwoKpDef * halfex;
		gy += mpu6050_mahonytwoKpDef * halfey;
		gz += mpu6050_mahonytwoKpDef * halfez;
	}

	// Integrate rate of change of quaternion
	gx *= (0.5f * (1.0f / mpu6050_mahonysampleFreq));		// pre-multiply common factors
	gy *= (0.5f * (1.0f / mpu6050_mahonysampleFreq));
	gz *= (0.5f * (1.0f / mpu6050_mahonysampleFreq));
	qa = q0;
	qb = q1;
	qc = q2;
	q0 += (-qb * gx - qc * gy - q3 * gz);
	q1 += (qa * gx + qc * gz - q3 * gy);
	q2 += (qa * gy - qb * gz + q3 * gx);
	q3 += (qa * gz + qb * gy - qc * gx);

	// Normalise quaternion
	norm = sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 /= norm;
	q1 /= norm;
	q2 /= norm;
	q3 /= norm;
}


/*
 * update timer for attitude
 */
// MPU6050_TIMERUPDATE

/*
 * get quaternion
 */
void mpu6050_getQuaternion(double *qw, double *qx, double *qy, double *qz) {
	*qw = q0;
	*qx = q1;
	*qy = q2;
	*qz = q3;
}

/*
 * get euler angles
 * aerospace sequence, to obtain sensor attitude:
 * 1. rotate around sensor Z plane by yaw
 * 2. rotate around sensor Y plane by pitch
 * 3. rotate around sensor X plane by roll
 */
void mpu6050_getRollPitchYaw(double *roll, double *pitch, double *yaw) {
	*yaw = atan2(2*q1*q2 - 2*q0*q3, 2*q0*q0 + 2*q1*q1 - 1);
	*pitch = -asin(2*q1*q3 + 2*q0*q2);
	*roll = atan2(2*q2*q3 - 2*q0*q1, 2*q0*q0 + 2*q3*q3 - 1);
}