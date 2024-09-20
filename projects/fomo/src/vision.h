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
#define CAM_SPI_IOM 1
#define CAM_SPI_SPEED AM_HAL_IOM_8MHZ

// Camera definitions
// Following includes camera specific header

// #define CAM_IMAGE_MODE NS_CAM_IMAGE_MODE_QVGA
#define CAM_IMAGE_MODE NS_CAM_IMAGE_MODE_96X96
#if CAM_IMAGE_MODE == NS_CAM_IMAGE_MODE_96X96
    #define CAM_WIDTH 96
    #define CAM_HEIGHT 96
    #define CAM_CH_SIZE (CAM_WIDTH * CAM_HEIGHT)
    #define CAM_BUFF_SIZE (2 * CAM_CH_SIZE)
#elif CAM_IMAGE_MODE == NS_CAM_IMAGE_MODE_QVGA
    #define CAM_WIDTH 320
    #define CAM_HEIGHT 240
    #define CAM_CH_SIZE (CAM_WIDTH * CAM_HEIGHT)
    #define CAM_BUFF_SIZE (CAM_CH_SIZE) // Assume 66% reduction
#endif

typedef enum {
    TAKING_RGB_IMAGE,
    TAKING_JPG_IMAGE,
    TRANSFERRING_RGB_IMAGE,
    TRANSFERRING_JPG_IMAGE,
    INFERENCING,
    DISPLAYING,
} app_state_e;

#endif
