//*****************************************************************************
//
// @file ng_event_transition.h
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
 * @brief Transition event type.
 *
 * Transition is derived from the base event type \a ng_event_base_t and contains additional attributes regarding its timing as well as specific functions for controlling it ( \a handler \a start \a stop \a pause_toggle ).
 *
 */

#ifndef __NG_EVENT_TRANSITION_H__
#define __NG_EVENT_TRANSITION_H__

#include "ng_event.h"
#include "ng_utils.h"

/** Data struct that contains a transition's data */
typedef struct _ng_transition_t
{
    EVENT_BASE_STRUCT; /**< Inherited attributes from ng_event_base_t data struct */
    float start_time;  /**< Start time of the periodic transition */
    float duration;    /**< Duration in seconds*/
    float progress;    /**< Progress [0.f, 1.f] */
} ng_transition_t;

/**
 * @brief Handler function of a transition
 *
 * @param *event Pointer to the base struct \a ng_event_base_t of the event that needs to be handled (casted internally to \a ng_transition_t)
 * @param trigger The trigger that iniated the execution of the event
 */
void ng_transition_handler(ng_event_base_t *event, uint32_t trigger);

/**
 * @brief Start function for starting a periodic transition
 *
 * @param *event Pointer to the base struct \a ng_event_base_t of the event that needs to be started (casted internally to \a ng_transition_t)
 */
void ng_transition_start(ng_event_base_t *event);

/**
 * @brief Stop function for stopping a transition
 *
 * @param *event Pointer to the base struct \a ng_event_base_t of the event that needs to be stopped (casted internally to \a ng_transition_t)
 * @param force_finish if this is true, the periodic trasnition will go to its final state (progress = 1.f) and stop, otherwise it will reset to its initial state (progress = 0.f) and stop
 */
void ng_transition_stop(ng_event_base_t *event, bool force_finish);

/**
 * @brief Function for pausing or resuming a transition
 *
 * @param  *event Pointer to the base struct \a ng_event_base_t of the event that needs to be paused/resumed (casted internally to \a ng_transition_t)
 * @param pause if true, the tranitions will explicitly pause, otherwise if the transition is paused, it will resume its execution
 */
void ng_transition_pause_toggle(ng_event_base_t *event, bool pause);

/**
 * @brief Reverts the transition progress once (do not use this function to re-revert a transition)
 *
 * @param *event Pointer to the base struct \a ng_event_base_t of the transition that needs to be reverted (casted internally to \a ng_transition_t)
 */
void ng_transition_revert(ng_event_base_t *event);

/**
 * @brief  Reverts the transition progress
 *
 * @param *event Pointer to the base struct \a ng_event_base_t of the transition that needs to be reverted (casted internally to \a ng_transition_t)
 * @param set if this is equal to zero, the transition's final progress is 1.f, otherwise the final progress is 0.f
 */
void ng_transition_revert_force(ng_event_base_t *event, int set);

#define NG_TRANSITION(object) SAFE_CAST((object), ng_event_base_t *, ng_transition_t *) /**< Type caster from  base ng_event_base_t struct to derived ng_transition_t struct*/

#endif //__NG_EVENT_TRANSITION_H__
