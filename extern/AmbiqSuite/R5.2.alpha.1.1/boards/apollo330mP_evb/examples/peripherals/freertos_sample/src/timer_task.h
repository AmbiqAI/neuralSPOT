//*****************************************************************************
//
//! @file timer_task.h
//!
//! @brief Simple task with a while() loop that waits for a semaphore from the
//!   timer ISR.  After TIMER_TASK_COUNT iterations (set in
//!   src/freertos_sample_test_cases.h), the task breaks out of the while(1)
//!   loop, sets bTimerTaskComplete to "true", and suspends/deletes itself.
//!   The bTimerTaskComplete variable is monitored by the ApplicationIdleTask.
//!
//!   Timer clock source is LFRC, which is a frequency of ~900Hz.
//!   The timer is configured in continuous upcount mode with a compare value
//!   of 900, thus the Timer ISR should fire every 1 seconds. In other words,
//!   timer_isr_freq = (1/timer_freq)*cycles = (1/900)*900 = 1 second
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

#ifndef TIMER_TASK_H
#define TIMER_TASK_H

//*****************************************************************************
//
// Timer task handle.
//
//*****************************************************************************
extern TaskHandle_t timer_task_handle;

//*****************************************************************************
//
// External function definitions.
//
//*****************************************************************************
extern void TimerTaskSetup(void);
extern void TimerTask(void *pvParameters);

#endif // TIMER_TASK_H
