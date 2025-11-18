//*****************************************************************************
//
//! @file coremark_pro.h
//!
//! @brief Global includes for the coremark_pro example.
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

#ifndef CORE_MARK_PRO_H
#define CORE_MARK_PRO_H

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
#ifdef SYSTEM_VIEW
#include "SEGGER_SYSVIEW_FreeRTOS.h"
#endif
#if defined(AM_FREERTOS)
#include "FreeRTOS.h"
#include "rtos.h"
#include "task.h"
#include "portmacro.h"
#include "portable.h"
#include "semphr.h"
#include "event_groups.h"
#endif

#ifdef SYSTEM_VIEW
#include "SEGGER_SYSVIEW.h"
#endif

#if defined(AM_BARE_METAL)
#include "am_time.h"
#endif
#if defined(HEAP_IN_PSRAM) || defined(HEAP_IN_SSRAM_AND_PSRAM)
#include "mspi_device_init.h"
#endif // defined(HEAP_IN_PSRAM) || defined(HEAP_IN_SSRAM_AND_PSRAM)

#include "ambiq_core_config.h"

#ifdef ENABLE_PMU
  #define pmu_enable am_util_pmu_enable
  #define pmu_get_profiling( x, y ) am_util_pmu_get_profiling( x, y )
  #define pmu_disable am_util_pmu_disable
#else
  #define pmu_enable()
  #define pmu_get_profiling( x, y )
  #define pmu_disable()
#endif

//*****************************************************************************
//
// PSRAM size and address definition.
//
//*****************************************************************************

#if defined(BAREMETAL) && defined(SYSTEM_VIEW)
#error SYSTEM_VIEW cannot be used in baremetal!
#endif

#define MSPI_PSRAM_MODULE           0

#if (MSPI_PSRAM_MODULE == 0)
#define MSPI_XIP_BASE_ADDRESS       (MSPI0_APERTURE_START_ADDR)
#elif (MSPI_PSRAM_MODULE == 1)
#define MSPI_XIP_BASE_ADDRESS       (MSPI1_APERTURE_START_ADDR)
#elif (MSPI_PSRAM_MODULE == 2)
#define MSPI_XIP_BASE_ADDRESS       (MSPI2_APERTURE_START_ADDR)
#endif // #if (MSPI_PSRAM_MODULE == 0)

#define MSPI_PSRAM_SIZE             (0x800000)

//*****************************************************************************
//
// ISR priority definition.
//
//*****************************************************************************
#define PSRAM_ISR_PRIORITY              (4 + 2)

#endif // CORE_MARK_PRO_H
