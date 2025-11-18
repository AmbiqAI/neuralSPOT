//*****************************************************************************
//
//! @file freertos_sample.h
//!
//! @brief Example program which demonstrates using FreeRTOS
//!
//! @addtogroup peripheral_examples Peripheral Examples
//
//! @defgroup freertos_sample FreeRTOS Example
//! @ingroup peripheral_examples
//! @{
//!
//! The following GPIOs are configured for observation:
//!
//!   GPIO           Toggles
//! --------- ---------------------------------
//!    0     on each iteration of Hello Task loop
//!    1     on each iteration of Idle Task
//!    2     before deepsleep entry
//!    3     immediately following deepsleep exit
//!    4     on each Timer ISR entry
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2025, Ambiq Micro, Inc.
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
// This is part of revision release_sdk5_2_a_1_1-c2486c8ef of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef FREERTOS_SAMPLE_H
#define FREERTOS_SAMPLE_H

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

//*****************************************************************************
//
// Task include files.
//
//*****************************************************************************
#include "rtos.h"
#include "print_task.h"
#include "hello_task.h"
#include "timer_task.h"

//*****************************************************************************
//
// Preprocessor Definitions and Global Variables
//
//*****************************************************************************
// Toggle these GPIO to indicate task entry
#define GPIO_HELLO_TASK_PIN     0
#define GPIO_IDLE_TASK_PIN      1
#define GPIO_SLEEP_ENTRY_PIN    2
#define GPIO_SLEEP_EXIT_PIN     3
#define GPIO_TIMER_ISR_PIN      4

#define HELLO_TASK_COUNT    10
#define TIMER_TASK_COUNT    10

//*****************************************************************************
//
// Task priority definition (https://www.freertos.org/RTOS-task-priority.html)
// - Priorities can be from 0 to configMAX_PRIORITIES, where 0 is lowest priority
// - configMAX_PRIORITIES is defined in FreeRTOSConfig.h
// - Idle task has priority 0
//
//*****************************************************************************
#define TASK_PRIORITY_LOW               (4)
#define TASK_PRIORITY_MIDDLE            (TASK_PRIORITY_LOW + 1)
#define TASK_PRIORITY_HIGH              (TASK_PRIORITY_MIDDLE + 1)
#define TASK_PRIORITY_HIGHEST           (TASK_PRIORITY_MIDDLE + 2)

//*****************************************************************************
//
// ISR priority definition (https://www.freertos.org/RTOS-Cortex-M3-M4.html)
// - FreeRTOS tasks ending in "FromISR" cannot be called from an interrupt whose
//   logical priority is higher than configMAX_SYSCALL_INTERRUPT_PRIORITY
// - configMAX_SYSCALL_INTERRUPT_PRIORITY is defined in FreeRTOSConfig.h
// - For Cortex-M interrupts, the highest priority is 0, which is default for
//   all interrupts.
// - Interrupts which call FreeRTOS tasks ending in "FromISR" must be numerically
//   greater than or equal to NCIV_configMAX_SYSCALL_INTERRUPT_PRIORITY
//
//*****************************************************************************
#define TIMER_ISR_PRIORITY              NVIC_configMAX_SYSCALL_INTERRUPT_PRIORITY

//*****************************************************************************
//
// Function templates
//
//*****************************************************************************
extern void globalTearDown(void);
#endif

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
