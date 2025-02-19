//*****************************************************************************
//
//! @file am_hal_debug.h
//!
//! @brief Functions for general debug operations.
//!
//! @addtogroup debug
//! @ingroup apollo5b_hal
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

#ifndef AM_HAL_DEBUG_H
#define AM_HAL_DEBUG_H

#ifdef __cplusplus
extern "C"
{
#endif


//*****************************************************************************
//
// Function prototypes
//
//*****************************************************************************

//*****************************************************************************
//
//! am_hal_debug_enable()
//! Perform steps necessary for general enabling for debug.
//
//*****************************************************************************
extern uint32_t am_hal_debug_enable(void);


//*****************************************************************************
//
//! am_hal_debug_disable()
//!Perform steps necessary to disable from debug.
//
//*****************************************************************************
extern uint32_t am_hal_debug_disable(void);


//*****************************************************************************
//
//! Debug power.
//! This function will power up the debug domain if not already powered.
//! It will also power down the domain if it were forced to power up.
//!
//! @param  bPowerUp - If true, determines whether the debug domain is already
//!                    powered up. If not, will power it up.
//!                    If false, will power down if it was initially powered
//!                    down and subsequently forced to power up.
//
//*****************************************************************************
extern uint32_t am_hal_debug_power(bool bPowerup);


//*****************************************************************************
//
//! Enable debug tracing.
//!
//! This function enables tracing, e.g. DEMCR->TRCENA.
//! TRCENA is required when using:
//!     DWT (Data Watchpoint and Trace)
//!     ITM (Instrumentation Trace Macrocell)
//!     PMU (Performance Monitoring Unit)
//! Arm recommends TRCENA when using:
//!     ETM (Embedded Trace Macrocell)- See section D1.2.37 DEMCR of Arm v8-M
//!                                     Architecture Reference Manual DDI0553B.y
//
//*****************************************************************************
extern uint32_t am_hal_debug_trace_enable(void);

//*****************************************************************************
//
//! Disable debug tracing.
//!
//! This function disables tracing, e.g. DEMCR->TRCENA.
//
//*****************************************************************************
extern uint32_t am_hal_debug_trace_disable(void);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_DEBUG_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

