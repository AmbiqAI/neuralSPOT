/*
MPU6050 lib 0x05

copyright (c) Davide Gironi, 2012

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/


#include <stdlib.h>
#include <string.h>
#include "quaternion.h"
#include <stdio.h>
#include <assert.h>
#include <math.h> 
#include "ns_core.h"

const ns_core_api_t ns_mahony_V0_0_1 = {.apiId = NS_MAHONY_API_ID, .version = NS_MAHONY_V0_0_1};
const ns_core_api_t ns_mahony_oldest_supported_version = {
    .apiId = NS_MAHONY_API_ID, .version = NS_MAHONY_V0_0_1};
const ns_core_api_t ns_mahony_current_version = {.apiId = NS_MAHONY_API_ID, .version = NS_MAHONY_V0_0_1};

uint16_t ns_mahony_init(ns_mahony_cfg_t *cfg) {
	#ifndef NS_DISABLE_API_VALIDATION
		if (cfg == NULL) {
			return NS_STATUS_INVALID_HANDLE;
		}

		if (ns_core_check_api(cfg->api, &ns_mahony_oldest_supported_version, &ns_mahony_current_version)) {
			return NS_STATUS_INVALID_VERSION;
		}
	#endif
	cfg->q0 = 1.0f;
	cfg->q1 = 0.0f;
	cfg->q2 = 0.0f;
	cfg->q3 = 0.0f;
	cfg->integralFBx = 0.0f;
	cfg->integralFBy = 0.0f;
	cfg->integralFBz = 0.0f;
	return NS_STATUS_SUCCESS;
}


/*
 * Mahony update function (for 6DOF)
 */
uint16_t ns_mahony_update(ns_mahony_cfg_t *cfg, float gx, float gy, float gz, float ax, float ay, float az) {
	if(cfg == NULL) {
		return NS_STATUS_INVALID_HANDLE;
	}
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
		halfvx = cfg->q1 * cfg->q3 - cfg->q0 * cfg->q2;
		halfvy = cfg->q0 * cfg->q1 + cfg->q2 * cfg->q3;
		halfvz = cfg->q0 * cfg->q0 - 0.5f + cfg->q3 * cfg->q3;

		// Error is sum of cross product between estimated and measured direction of gravity
		halfex = (ay * halfvz - az * halfvy);
		halfey = (az * halfvx - ax * halfvz);
		halfez = (ax * halfvy - ay * halfvx);

		// Compute and apply integral feedback if enabled
		if(mahonytwoKiDef > 0.0f) {
			cfg->integralFBx += mahonytwoKiDef * halfex * (1.0f / mahonysampleFreq);	// integral error scaled by Ki
			cfg->integralFBy += mahonytwoKiDef * halfey * (1.0f / mahonysampleFreq);
			cfg->integralFBz += mahonytwoKiDef * halfez * (1.0f / mahonysampleFreq);
			gx += cfg->integralFBx;	// apply integral feedback
			gy += cfg->integralFBy;
			gz += cfg->integralFBz;
		} else {
			cfg->integralFBx = 0.0f;	// prevent integral windup
			cfg->integralFBy = 0.0f;
			cfg->integralFBz = 0.0f;
		}

		// Apply proportional feedback
		gx += mahonytwoKpDef * halfex;
		gy += mahonytwoKpDef * halfey;
		gz += mahonytwoKpDef * halfez;
	}

	// Integrate rate of change of quaternion
	gx *= (0.5f * (1.0f / mahonysampleFreq));		// pre-multiply common factors
	gy *= (0.5f * (1.0f / mahonysampleFreq));
	gz *= (0.5f * (1.0f / mahonysampleFreq));
	qa = cfg->q0;
	qb = cfg->q1;
	qc = cfg->q2;
	cfg->q0 += (-qb * gx - qc * gy - cfg->q3 * gz);
	cfg->q1 += (qa * gx + qc * gz - cfg->q3 * gy);
	cfg->q2 += (qa * gy - qb * gz + cfg->q3 * gx);
	cfg->q3 += (qa * gz + qb * gy - qc * gx);

	// Normalise quaternion
	norm = sqrt(cfg->q0 * cfg->q0 + cfg->q1 * cfg->q1 + cfg->q2 * cfg->q2 + cfg->q3 * cfg->q3);
	cfg->q0 /= norm;
	cfg->q1 /= norm;
	cfg->q2 /= norm;
	cfg->q3 /= norm;
	return NS_STATUS_SUCCESS;
}


/*
 * update timer for attitude
 */
// MPU6050_TIMERUPDATE

/*
 * get quaternion
 */
uint16_t ns_get_quaternion(ns_mahony_cfg_t *cfg, double *qw, double *qx, double *qy, double *qz) {
	if(cfg == NULL || qw == NULL || qx == NULL || qy == NULL || qz == NULL) {
		return NS_STATUS_INVALID_HANDLE;
	}

	*qw = cfg->q0;
	*qx = cfg->q1;
	*qy = cfg->q2;
	*qz = cfg->q3;
	return NS_STATUS_SUCCESS;
}

/*
 * get euler angles
 * aerospace sequence, to obtain sensor attitude:
 * 1. rotate around sensor Z plane by yaw
 * 2. rotate around sensor Y plane by pitch
 * 3. rotate around sensor X plane by roll
 */
uint16_t ns_get_RollPitchYaw(ns_mahony_cfg_t *cfg, double *roll, double *pitch, double *yaw) {
	if(cfg == NULL || roll == NULL || pitch == NULL || yaw == NULL) {
		return NS_STATUS_INVALID_HANDLE;
	}
	*yaw = atan2(2 * cfg->q1 * cfg->q2 - 2 * cfg->q0 * cfg->q3, 2*cfg->q0 * cfg->q0 + 2 * cfg->q1 * cfg->q1 - 1 + + 1e-4);
	*pitch = -asin(2 * cfg->q1 * cfg->q3 + 2 * cfg->q0 * cfg->q2);
	*roll = atan2(2 * cfg->q2 * cfg->q3 - 2 * cfg->q0 * cfg->q1, 2 * cfg->q0 * cfg->q0 + 2 * cfg->q3 * cfg->q3 - 1 + 1e-4);
	return NS_STATUS_SUCCESS;
}