//*****************************************************************************
//
//! @file uart_example_timer_control.h
//!
//! @brief periodic timer setup
//!
//! @addtogroup uart_stream
//! @{
//! @ingroup uart_timer_control_c UART Timer Control
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
// This is part of revision release_sdk5_2_a_1_1-c2486c8ef of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef UART_EXAMPLE_TIMER_CONTROL_H
#define UART_EXAMPLE_TIMER_CONTROL_H

#include "am_mcu_apollo.h"

//
//! declare the timer callback type
//
typedef void (*pfTimerCallback_t)(uint32_t x);

typedef struct
{
    uint64_t ui32TimerCount;        //!< Accumulating Number of timer interrupts
    uint32_t ui32TimerPeriodMs;     //!< Timer interrupt rate in milliseconds
    uint32_t ui32TimerDivider;      //!< number of timer interrupts before call to timer function
    uint32_t ui32TimerDividerCount; //!< Accumulating counter from 0 to ui32TimerDivider
}
uart_examp_timer_params_t;

#define PRD_TIMER_FREQ 375000  // hfrc / 256

//*****************************************************************************
//
//! @breif sets up and (optionally) starts periodic timer
//!
//! @param psTimerParams  pointer to timer params
//! @param pfTimerCallbacki function called when timer expires
//! @param bStartTimer      starts timer when true
//!
//! @return                 hal status
//
//*****************************************************************************

extern uint32_t uart_timer_init(uart_examp_timer_params_t *psTimerParams,
                                pfTimerCallback_t pfTimerCallbacki, bool bStartTimer);
//*****************************************************************************
//
//! @breif starts or stops timer
//!
//! @param bStartTimer  start timer when true, stops when false
//
//*****************************************************************************
extern uint32_t timerStart(bool bStartTimer);

#endif // UART_EXAMPLE_TIMER_CONTROL_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//! @}
//
//*****************************************************************************
