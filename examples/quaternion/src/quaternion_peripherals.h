/**
 * @file quaternion_peripherals.h
 * @author Evan Chen
 * @brief Basic peripheral stuff (buttons, etc)
 * @version 0.1
 * @date 2024-06-21
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ns_peripherals_button.h"
#include "ns_timer.h"

// Button global - will be set by neuralSPOT button helper
static int volatile buttonPressed = 0;

// Button Peripheral Config Struct
ns_button_config_t button_config = {.api = &ns_button_V1_0_0,
                                    .button_0_enable = true,
                                    .button_1_enable = false,
                                    .button_0_flag = &buttonPressed,
                                    .button_1_flag = NULL};
