/**
 * @file constants.h
 * @author Adam Page (adam.page@ambiq.com)
 * @brief Store global app constants
 * @version 0.1
 * @date 2022-11-02
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __TVD_CONSTANTS_H
#define __TVD_CONSTANTS_H

#define TFLM_ENGINE 0
#define TINY_ENGINE 1
#define INFERENCE_ENGINE TFLM_ENGINE

// #define AM_PART_APOLLO4
#ifdef __cplusplus
extern "C" {
#endif
#include "ArducamCamera.h"

// Camera interface
#define CAM_SPI_IOM 1
#define CAM_SPI_SPEED AM_HAL_IOM_8MHZ

// Camera definitions
// Following includes camera specific header
#define JPEG_SIZE CAM_IMAGE_MODE_QVGA // 320x240
#define CAM_WIDTH 320
#define CAM_HEIGHT 240
#define CAM_CH_SIZE (CAM_WIDTH * CAM_HEIGHT)
// BUFF needs to be big enough to store JPEG (assume 66% reduction)
#define CAM_BUFF_SIZE (1 * CAM_CH_SIZE)

// Image/model constants
// TODO: Generate these in deploy step
#define IMG_NUM_CH 3
#define IMG_WIDTH 80
#define IMG_HEIGHT 80
#define IMG_SCALE 3 // min(floor(CAM_HEIGHT/IMG_HEIGHT), floor(CAM_WIDTH/IMG_WIDTH))
#define IMG_CH_SIZE (IMG_WIDTH * IMG_HEIGHT)
#define IMG_SIZE (IMG_NUM_CH * IMG_CH_SIZE)
#define NUM_CLASSES (2)

typedef int8_t img_t;

// #define MIN(a, b) ((a) < (b) ? (a) : (b))
// #define MAX(a, b) ((a) > (b) ? (a) : (b))

#ifdef __cplusplus
}
#endif
#endif // __TVD_CONSTANTS_H
