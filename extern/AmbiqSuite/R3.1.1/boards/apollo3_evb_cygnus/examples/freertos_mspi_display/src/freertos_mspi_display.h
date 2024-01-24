//*****************************************************************************
//
//! @file freertos_mspi_iom_display.h
//!
//! @brief Global includes for the freertos_mspi_iom_display example.
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2023, Ambiq Micro, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision release_sdk_3_1_1-10cda4b5e0 of the AmbiqSuite Development Package.
//
//*****************************************************************************

#ifndef FREERTOS_MSPI_MSPI_DISPLAY_H
#define FREERTOS_MSPI_MSPI_DISPLAY_H

//*****************************************************************************
//
// Required built-ins.
//
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

//*****************************************************************************
//
// Standard AmbiqSuite includes.
//
//*****************************************************************************
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

//*****************************************************************************
//
// FreeRTOS include files.
//
//*****************************************************************************
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"
#include "portable.h"
#include "semphr.h"
#include "event_groups.h"
#include "timers.h"
//#include "rtos.h"

//*****************************************************************************
//
// Task include files.
//
//*****************************************************************************
//#include "compose_task.h"
#include "main_task.h"
#include "render_task.h"

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_devices_mspi_rm67162.h"
#include "am_devices_mspi_rm69330.h"
#include "am_devices_mspi_psram_aps6404l.h"
#include "am_util.h"

//*****************************************************************************
// Customize the following for the test
//*****************************************************************************
//#define ENABLE_LOGGING
// To test image sanity with with FPGA, as the refresh rate is too high
#define TE_DELAY               20 // 1

//#define DISPLAY_MSPI_MODULE    AM_BSP_MSPI_DISPLAY_INST
#define DISPLAY_MSPI_MODULE    0
#define PSRAM_MSPI_MODULE      AM_BSP_MSPI_PSRAM_INST

// Whether to set the display render window
// This example always uses full screen, and hence this can be disabled for optimization
//#define CONFIG_DISPLAY_WINDOW


//*****************************************************************************
//
// Macros
//
//*****************************************************************************
#if defined(AM_PART_APOLLO3)
#define DISPLAY_MSPI_IRQn        MSPI0_IRQn
#elif defined(AM_PART_APOLLO3P)
#define DISPLAY_MSPI_IRQn        MSPI_IRQn(DISPLAY_MSPI_MODULE)
#else
#warning "This example only runs with Apollo3Blue or Apollo3BluePlus devices."
#endif

// Display Characteristics
#define ROW_NUM                   AM_DEVICES_RM69330_NUM_ROWS
#define COLUMN_NUM                AM_DEVICES_RM69330_NUM_COLUMNS
#define PIXEL_SIZE                AM_DEVICES_RM69330_PIXEL_SIZE
#define ROW_SIZE                  (COLUMN_NUM * PIXEL_SIZE)
#define FRAME_SIZE                ROW_NUM * ROW_SIZE // Display device is limited to its resolution ratio// Temp Buffer in SRAM to read PSRAM data to, and write DISPLAY data from


//*****************************************************************************
//
// Debug
//
//*****************************************************************************
#define DBG1_GPIO          33
#define DBG2_GPIO          32

#define DEBUG_GPIO_HIGH(gpio)   am_hal_gpio_state_write(gpio, AM_HAL_GPIO_OUTPUT_SET)
#define DEBUG_GPIO_LOW(gpio)    am_hal_gpio_state_write(gpio, AM_HAL_GPIO_OUTPUT_CLEAR)

#ifdef ENABLE_LOGGING
#define DEBUG_PRINT am_util_stdio_printf
#else
#define DEBUG_PRINT(...)
#endif
#define DEBUG_PRINT_SUCCESS(...)

#define RENDER_EVENT_START_NEW_FRAME        0x1

//*****************************************************************************
//
// Global externs
//
//*****************************************************************************
extern void                  *g_MSPIDisplayHandle;

extern volatile bool         g_bTEInt;
extern volatile bool         g_bDisplayDone;
extern volatile bool         g_bNewDisplay;

extern uint32_t              fb1;
extern uint32_t              fb2;


extern EventGroupHandle_t xMainEventHandle;
extern EventGroupHandle_t xRenderEventHandle;

//*****************************************************************************
//
// External function definitions
//
//*****************************************************************************
uint32_t display_init(void);

#endif // FREERTOS_MSPI_MSPI_DISPLAY_H
