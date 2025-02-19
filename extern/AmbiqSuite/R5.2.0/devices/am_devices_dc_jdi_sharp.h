//*****************************************************************************
//
//! @file am_devices_dc_jdi_sharp.h
//!
//! @brief Generic sharp JDI(LS014B7DD01) driver.
//!
//! @addtogroup dc_jdi DC JDI Driver
//! @ingroup devices
//! @{
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
// This is part of revision release_sdk5p2-040c7863bb of the AmbiqSuite Development Package.
//
//*****************************************************************************

#ifndef AM_DEVICES_DC_JDI_SHARP_H
#define AM_DEVICES_DC_JDI_SHARP_H

#include "stdint.h"
#include "nema_dc.h"
#include "nema_dc_hal.h"
#include "nema_dc_mipi.h"
#include "nema_dc_regs.h"
#include "nema_dc_intern.h"
#include "nema_sys_defs.h"
#include "am_devices_display_types.h"
#include "am_hal_timer.h"
#ifdef __cplusplus
extern "C"
{
#endif

#define SHARP_VA_VCOM_TIMER         7
#define SHARP_VA_VCOM_FREQUENCY     60

typedef struct
{
   uint32_t ui32TimerNum;
   uint32_t ui32Frequency;
   am_hal_timer_clock_e eTimerClock;
}am_devices_dc_jdi_timer_config_t;

//*****************************************************************************
//
//! External function definitions.
//
//*****************************************************************************

//*****************************************************************************
//
//! @brief Initialize sharp model type LS014B7DD01
//!
//! @param pTimerConfig     - jdi timer configuration structure.
//! @param pDCConfig        - jdi panel configuration structure.
//!
//! @note It should be called before NemaDC is initialized and configured.
//!
//! @return status.
//
//*****************************************************************************
extern uint32_t am_devices_dc_jdi_sharp_init(am_devices_dc_jdi_timer_config_t *pTimerConfig, nemadc_initial_config_t *pDCConfig);

//*****************************************************************************
//
//! @brief start generate PWM signal for JDI timer.
//!
//! This function generate two PWM signal one is inphase and another is opposite
//! phase to VCOM.
//!
//! @return None.
//
//*****************************************************************************
extern void am_devices_dc_jdi_sharp_timer_start(void);

//*****************************************************************************
//
//! @brief stop generate PWM signal for JDI timer.
//!
//! This function stop the two PWM signal.
//!
//! @return None.
//
//*****************************************************************************
extern void am_devices_dc_jdi_sharp_timer_stop(void);

#ifdef __cplusplus
}
#endif

#endif // AM_DEVICES_DC_JDI_SHARP_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

