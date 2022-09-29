//*****************************************************************************
//
//  ng_event_periodic.h
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
/**
 * @file
 * @brief Periodic event type.
 *
 * Periodic is derived from the base event type \a ng_event_base_t and contains additional attributes regarding its timing as well as specific functions for controlling it ( \a handler \a start \a stop \a pause_toggle ).
 *
 */

#ifndef __NG_EVENT_PERIODIC_H__
#define __NG_EVENT_PERIODIC_H__

#include "ng_event.h"
#include "ng_utils.h"
#include "ng_globals.h"

/** Data struct that contains a periodic event's data */
typedef struct _ng_periodic_t
{
    EVENT_BASE_STRUCT;  /**< Inherited attributes from ng_event_base_t data struct */
    float start_time;   /**< Start time of the periodic transition */
    float period;       /**< Period in seconds */
} ng_periodic_t;

/**
 * @brief Handler function of a periodic event
 *
 * @param *event Pointer to the base struct \a ng_event_base_t of the event that needs to be handled (casted internally to \a ng_periodic_t)
 * @param trigger The trigger that iniated the execution of the event
 */
void ng_periodic_handler(ng_event_base_t *event, uint32_t trigger);

/**
 * @brief Start function for starting a periodic event
 *
 * @param *event Pointer to the base struct \a ng_event_base_t of the event that needs to be started (casted internally to \a ng_periodic_t)
 */
void ng_periodic_start(ng_event_base_t *event);

/**
 * @brief Stop function for stopping a periodic transition
 *
 * @param *event Pointer to the base struct \a ng_event_base_t of the event that needs to be stopped (casted internally to \a ng_periodic_t)
 * @param force_finish unused (needed for function's signature)
 */
void ng_periodic_stop(ng_event_base_t *event, bool force_finish);

/**
 * @brief Function for pausing or resuming a periodic event
 *
 * @param  *event Pointer to the base struct \a ng_event_base_t of the event that needs to be paused/resumed (casted internally to \a ng_periodic_t)
 * @param pause if true, the periodic event will explicitly pause, otherwise if the periodic event is paused, it will resume its execution
 */
void ng_periodic_pause_toggle (ng_event_base_t *event, bool pause);

#define NG_PERIODIC(object) SAFE_CAST((object), ng_event_base_t *, ng_periodic_t *) /**< Type caster from  base ng_event_base_t struct to derived ng_periodic_t struct*/

#endif //__NG_EVENT_PERIODIC_H__
