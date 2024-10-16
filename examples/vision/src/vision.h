/**
 * @file main.h
 * @author Adam Page (adam.page@ambiq.com)
 * @brief Main application
 * @version 0.1
 * @date 2022-11-09
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __MAIN_H
#define __MAIN_H

// #include "ns_camera_constants.h"
// Camera interface

#ifdef apollo510_evb
    #define CAM_SPI_IOM 2
#else
    #define CAM_SPI_IOM 1
#endif
#define CAM_SPI_SPEED AM_HAL_IOM_8MHZ

// Camera definitions
// Following includes camera specific header

#define RGB_WIDTH 96
#define RGB_HEIGHT 96
#define RGB_CH_SIZE (RGB_WIDTH * RGB_HEIGHT)

// This is RGB565, so 2 bytes per pixel
#define RGB_BUFF_SIZE (2 * RGB_CH_SIZE)

#define JPG_WIDTH 320
#define JPG_HEIGHT 320
#define JPG_CH_SIZE (JPG_WIDTH * JPG_HEIGHT)
#define JPG_BUFF_SIZE (JPG_CH_SIZE)

// // #define CAM_IMAGE_MODE NS_CAM_IMAGE_MODE_QVGA
// #define CAM_IMAGE_MODE NS_CAM_IMAGE_MODE_96X96
// #if CAM_IMAGE_MODE == NS_CAM_IMAGE_MODE_96X96
//     #define CAM_WIDTH 96
//     #define CAM_HEIGHT 96
//     #define CAM_CH_SIZE (CAM_WIDTH * CAM_HEIGHT)
//     #define CAM_BUFF_SIZE (2 * CAM_CH_SIZE)
// #elif CAM_IMAGE_MODE == NS_CAM_IMAGE_MODE_320
//     #define CAM_WIDTH 320
//     #define CAM_HEIGHT 320
//     #define CAM_CH_SIZE (CAM_WIDTH * CAM_HEIGHT)
//     #define CAM_BUFF_SIZE (CAM_CH_SIZE) // Assume 66% reduction
// #endif

#endif
