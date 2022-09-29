//*****************************************************************************
//
// @file ng_screen_trans.h
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

#ifndef NG_SCREEN_TRANS_H__
#define NG_SCREEN_TRANS_H__

#ifndef SCREEN_TRANSITION_DURATION_SECS
#define SCREEN_TRANSITION_DURATION_SECS 1.f /**< Defines the maximum duration of the screen transition's event */
#endif

#include "ng_globals.h"
#include "ng_event_transition.h"
#include "ng_utils.h"
#include "nema_transitions.h"

/**
 * @brief Initializes a screen transition
 *
 * @param *event Pointer to the event struct that controls the transition
 * @param *to_screen Pointer to the final screen's tree node
 * @param *from_screen Pointer to the initial screen's tree node
 * @param effect Transition effect
 * @param go_right Parameter for the direction of the transition
 * @param initial_progress Screen transition's event initial progress
 */
void ng_screen_trans_initialize(ng_event_base_t *event, tree_node_t *to_screen, tree_node_t *from_screen, nema_transition_t effect, int go_right, float initial_progress);

/**
 * @brief Function executed when swiping during a screen transition
 *
 * @param progress_diff Progress difference of the screen transition's event
 */
void ng_screen_trans_swipe(float progress_diff);

/**
 * @brief Rusumes an active screen transition after mouse/finger release (uses the timer)
 *
 * @param *event Pointer to the event of the screen transition
 * @param duration Remaining duration
 * @param abort If this different than zero, the screen transition will return to its initial screen
 */
void ng_screen_trans_resume(ng_event_base_t *event, float duration, int abort);

/**
 * @brief Pauses the screen transition's event
 *
 * @param *event Pointer to the screen transition's event
 */
void ng_screen_trans_pause(ng_event_base_t *event);

/**
 * @brief Callback function assigned to the screen transition event
 *
 * @param *event Pointer to the screen transition event
 * @param *data Screen transition data (casted to ng_node_effect_direction_t* type internaly)
 */
void ng_screen_trans_swipable(ng_event_base_t *event, void *data);

/**
 * @brief Callback function executed when the screen transition's event is triggered by the timer
 *
 * @param *event Pointer to the screen transition's event
 * @param *data Transition data (casted to \a ng_node_effect_direction_t data struct internally)
 */
void ng_callback_show_screen(ng_event_base_t *event, void *data);

/**
 * @brief Callback function for setting instantly the current screen
 *
 * @param *event Pointer to the event with this action
 * @param *data Pointer to the screen's tree node that needs to be set as current screen (casted to \a ng_tree_node_ptr_t internally)
 */
void ng_callback_set_screen(ng_event_base_t *event, void *data);

#endif //NG_SCREEN_TRANS_H__
