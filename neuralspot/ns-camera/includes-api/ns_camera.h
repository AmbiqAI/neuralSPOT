/**
 * @file camera.h
 * @author Adam Page (adam.page@ambiq.com)
 * @brief Camera controller
 * @version 0.1
 * @date 2022-11-02
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __CAMERA_H
#define __CAMERA_H

#include "arm_math.h"
#include <stdint.h>

int init_camera(void);
int start_camera(void);
int stop_camera(void);
int trigger_camera_capture();
int is_camera_capturing();
uint32_t camera_capture(uint8_t *camBuf, uint32_t bufLen);
int camera_decode_image(
    uint8_t *camBuf, uint32_t camLen, int8_t *imgBuf, uint32_t imgWidth, uint32_t imgHeight,
    uint32_t scaleFactor);

#endif // __CAMERA_H
