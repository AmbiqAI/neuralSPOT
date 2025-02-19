//*****************************************************************************
//
//! @file am_mcu_apollo.h
//!
//! @brief Top Include for Apollo3 Plus class devices.
//!
//! This file provides all the includes necessary for an apollo device.
//!
//! @addtogroup hal Hardware Abstraction Layer (HAL)
//
//! @defgroup apollo3p_hal apollo3p
//! @ingroup hal
//! @{
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

#ifndef AM_MCU_APOLLO_H
#define AM_MCU_APOLLO_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// AM_PART_APOLLO3_API indicates that this device uses the Apollo3 API.
//
//*****************************************************************************
#define AM_PART_APOLLO3_API     1

//*****************************************************************************
//
// Define AM_CMSIS_REGS to indicate that CMSIS registers are supported.
//
//*****************************************************************************
#define AM_CMSIS_REGS           1

//*****************************************************************************
//
// C99
//
//*****************************************************************************
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#if AM_CMSIS_REGS
#include "apollo3p.h"
#else // AM_CMSIS_REGS
#ifdef __IAR_SYSTEMS_ICC__
#include "intrinsics.h"     // __CLZ() and other intrinsics
#endif // AM_CMSIS_REGS
#endif

//*****************************************************************************
//
// Global HAL
//
//*****************************************************************************
//
// Define this macro to disable and remove parameter validation in functions
// throughout the HAL.
//
//#define AM_HAL_DISABLE_API_VALIDATION

//
// Define the following macro to disable assert messaging.
// Defining this macro will result in smaller, more efficient HAL code, but
// will eliminate debug messaging.
//
//#define AM_HAL_DEBUG_NO_ASSERT

//*****************************************************************************
//
// Registers
//
//*****************************************************************************
#include "regs/am_reg_base_addresses.h"
#include "regs/am_reg_macros.h"
#include "regs/am_reg.h"
#include "regs/am_reg_m4.h"
#include "regs/am_reg_jedec.h"
#include "regs/am_mcu_apollo3p_info0.h"

//*****************************************************************************
//
// HAL
//
//*****************************************************************************
#include "hal/am_hal_status.h"
#include "hal/am_hal_sysctrl.h"
#include "hal/am_hal_adc.h"
#include "hal/am_hal_ble.h"
#include "hal/am_hal_ble_patch_b0.h"
#include "hal/am_hal_burst.h"
#include "hal/am_hal_cachectrl.h"
#include "hal/am_hal_clkgen.h"
#include "hal/am_hal_cmdq.h"
#include "hal/am_hal_ctimer.h"
#include "hal/am_hal_debug.h"
#include "hal/am_hal_flash.h"
#include "hal/am_hal_global.h"
#include "hal/am_hal_gpio.h"
#include "hal/am_hal_interrupt.h"
#include "hal/am_hal_iom.h"
#include "hal/am_hal_ios.h"
#include "hal/am_hal_itm.h"
#include "hal/am_hal_mcuctrl.h"
#include "hal/am_hal_mspi.h"
#include "hal/am_hal_pdm.h"
#include "hal/am_hal_pin.h"
#include "hal/am_hal_pwrctrl.h"
#include "hal/am_hal_pwrctrl_helper.h"
#include "hal/am_hal_queue.h"
#include "hal/am_hal_reset.h"
#include "hal/am_hal_rtc.h"
#include "hal/am_hal_scard.h"
#include "hal/am_hal_secure_ota.h"
#include "hal/am_hal_security.h"
#include "hal/am_hal_stimer.h"
#include "hal/am_hal_systick.h"
#include "hal/am_hal_tpiu.h"
#include "hal/am_hal_entropy.h"
#include "hal/am_hal_uart.h"
#include "hal/am_hal_wdt.h"

#ifdef __cplusplus
}
#endif

#endif // AM_MCU_APOLLO_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
