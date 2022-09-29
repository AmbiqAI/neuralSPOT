//*****************************************************************************
//
// @file ng_timer.h
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2022, Ambiq Micro, Inc.
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
// This is part of revision release_sdk_4_3_0-0ca7d78a2b of the AmbiqSuite Development Package.
//
//*****************************************************************************

/*******************************************************************************
 * Copyright (c) 2021 Think Silicon S.A.
 *
   Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this header file and/or associated documentation files to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the
 * Materials, and to permit persons to whom the Materials are furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * MODIFICATIONS TO THIS FILE MAY MEAN IT NO LONGER ACCURATELY REFLECTS
 * NEMAGUI API.
 *
 *  The software is provided 'as is', without warranty of any kind, express or
 *  implied, including but not limited to the warranties of merchantability,
 *  fitness for a particular purpose and noninfringement. In no event shall
 *  Think Silicon S.A. be liable for any claim, damages or other liability, whether
 *  in an action of contract, tort or otherwise, arising from, out of or in
 *  connection with the software or the use or other dealings in the software.
 ******************************************************************************/

/**
 * @file
 * @brief Timer module
 *
 * Provides the functions needed to create a control the application timer. An NemaGUI application requires one periodic timer.
 *
 */
#ifndef NG_TIMER_H__
#define NG_TIMER_H__

#include "nema_core.h" //bool
#include "ng_event.h"

/**
 * @brief Creates the periodic timer needed by the application
 *
 * @return int The timer's ID
 */
int  ng_timer_create();

/**
 * @brief Sets the period of the timer
 *
 * @param ms Period in ms
 */
void ng_timer_set_period(int ms);

/**
 * @brief Starts the timer
 *
 */
void ng_timer_start();

/**
 * @brief Stops the timer
 *
 */
void ng_timer_stop();

/**
 * @brief Function executed each time the timer ticks. Runs all the \a running events
 *
 */
void ng_timer_handler();

/**
 * @brief Gets the timer ID
 *
 * @return int Timer ID
 */
int  ng_timer_get();

/**
 * @brief Checks if the timer is currently running
 *
 * @return bool true if the timer is running, otherwise false
 */
bool ng_timer_is_running();

/**
 * @brief Gets the period of the timer
 *
 * @return int Timer period in ms
 */
int ng_timer_get_period();

/**
 * @brief Gets the timer frequency in Hz
 *
 * @return float Timer frequency in Hz
 */
float ng_timer_get_frequency();

// void ng_timer_set_event_list(ng_event_base_t** list, int size);

#endif //NG_TIMER_H__
