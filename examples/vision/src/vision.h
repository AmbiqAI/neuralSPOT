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

#include "ns_camera_constants.h"

typedef enum {
    IDLE_STATE,
    START_STATE,
    CAPTURE_STATE,
    PREPROCESS_STATE,
    INFERENCE_STATE,
    DISPLAY_STATE,
    FAIL_STATE,
    STOP_STATE
} app_state_e;

typedef enum { SENSOR_DATA_COLLECT, CLIENT_DATA_COLLECT } data_collect_mode_e;

void setup(void);
void loop(void);

#endif
