//*****************************************************************************
//
//! @file ng_event.h
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

#ifndef __NG_EVENT_H__
#define __NG_EVENT_H__

#include "ng_gitem.h"
#include "ng_typedefs.h"
#include "ng_animation.h"

#define EV_TRIGGER_NULL           0  /**< Reserved */
#define EV_TRIGGER_PRESS          1  /**< Press trigger */
#define EV_TRIGGER_RELEASE        2  /**< Release trigger */
#define EV_TRIGGER_HOLD           3  /**< Hold trigger (reserved for future use)*/
#define EV_TRIGGER_DRAG           4  /**< Drag (swipe) trigger */
#define EV_TRIGGER_VALUE_CHANGED  5  /**< Value changed trigger */
#define EV_TRIGGER_STATE_CHANGED  6  /**< State changed trigger */
#define EV_TRIGGER_SCREEN_ENTERED 7  /**< Screen enetered trigger */
#define EV_TRIGGER_APP_START      8  /**< Application start-up trigger */
#define EV_TRIGGER_CUSTOM         9  /**< Custom trigger */
#define EV_TRIGGER_TIMER          10 /**< Timmer trigger */

//Retrigger defines
#define EV_RETRIGGER_IGNORE        0x0                                        /**< Ignore the retriger (continue execution normally) */
#define EV_RETRIGGER_PAUSE         0x1                                        /**< Pause a running event */
#define EV_RETRIGGER_RESUME        0x2                                        /**< Resume a paused event */
#define EV_RETRIGGER_PAUSE_TOGGLE  (EV_RETRIGGER_PAUSE | EV_RETRIGGER_RESUME) /**< Ignore the retriiger (continue execution normally) */
#define EV_RETRIGGER_REVERSE       0x4                                        /**< Return to the initial state by running the event in reverse order (cur_progress to 0.f) */
#define EV_RETRIGGER_RESET         0x8                                        /**< Reset the event to its initial state */
#define EV_RETRIGGER_FINISH        0x10                                       /**< Go to final state (transition) */
#define EV_RETRIGGER_RESTART       0x20                                       /**< On retrigger, restart periodic/transitional event */
#define EV_RETRIGGER_STOP          0x40                                       /**< Go to final state and stop (periodic transition) */

//Event status
#define EV_STATUS_STOPPED    0x0   /**< The event is stopped */
#define EV_STATUS_RUNNING    0x1   /**< The event is running */
#define EV_STATUS_PAUSED     0x2   /**< The event is paused */
#define EV_STATUS_MASK       0x3   /**< Helper mask */
#define EV_STATUS_REVERSED   0x4   /**< The event is performing a backwards transition (from progress: p1 to 0) */
#define EV_STATUS_FIRST_RUN  0x8   /**< Indicates the first run of the event */
#define EV_STATUS_LAST_RUN   0x10  /**< Indicates the last run of the event */

//forward declaration
struct _gitem_base_t;

//typedef action's function pointers
typedef void (*act_gitptr_float_f   )(struct _gitem_base_t *gitem, float value);                 /**< Typedef function pointer that takes a pointer to a gitem_base_t and a float as arguments*/
typedef void (*act_gitptr_ptr_f     )(struct _gitem_base_t *gitem, void *ptr);                   /**< Typedef function pointer that takes a pointer to a gitem_base_t and a void pointer as arguments*/
typedef void (*act_gitptr_int_f     )(struct _gitem_base_t *gitem, int value);                   /**< Typedef function pointer that takes a pointer to a gitem_base_t and a int as arguments*/
typedef void (*act_gitptr_uint_f    )(struct _gitem_base_t *gitem, uint32_t value);              /**< Typedef function pointer that takes a pointer to a gitem_base_t and a uint32_t as arguments*/
typedef void (*act_gitptr_f         )(struct _gitem_base_t *gitem);                              /**< Typedef function pointer that takes a pointer to a gitem_base_t as argument*/
typedef void (*act_gitptr_int_int_f )(struct _gitem_base_t *gitem, int a, int b);                /**< Typedef function pointer that takes a pointer to a gitem_base_t and two int as arguments*/
typedef void (*act_nodeptr_f        )(struct _tree_node_t  *node);                               /**< Typedef function pointer that takes a tree_node_t pointer as argument*/
typedef void (*act_nodeptr_nodeptr_f)(struct _tree_node_t  *node0, struct _tree_node_t  *node1); /**< Typedef function pointer that takes two tree_node_t pointers as arguments*/
typedef void (*act_animptr_f        )(ng_animation_data_t *data);                                /**< Typedef function pointer that takes a pointer to ng_animation_data_t as argument*/
typedef void (*act_void_f           )(void);                                                     /**< Typedef function pointer that takes no arguments*/

/**
 * @brief Union that groups together pointers to all possible actions that are supported in the event mechanism
 *
 */
typedef union _ng_act_ptr
{
  act_gitptr_float_f act_gitptr_float;       /**< Function pointer to a act_gitptr_float_f function*/
  act_gitptr_ptr_f act_gitptr_ptr;           /**< Function pointer to a act_gitptr_ptr_f function*/
  act_gitptr_int_f act_gitptr_int;           /**< Function pointer to a act_gitptr_int_f function*/
  act_gitptr_uint_f act_gitptr_uint;         /**< Function pointer to a act_gitptr_uint_f function*/
  act_gitptr_f act_gitptr;                   /**< Function pointer to a act_gitptr_f function*/
  act_gitptr_int_int_f act_gitptr_int_int;   /**< Function pointer to a act_gitptr_int_int_f function*/
  act_nodeptr_f act_nodeptr;                 /**< Function pointer to a act_nodeptr_f function*/
  act_nodeptr_nodeptr_f act_nodeptr_nodeptr; /**< Function pointer to a act_nodeptr_nodeptr_f function*/
  act_animptr_f act_animptr;                 /**< Function pointer to a act_animptr_f function*/
  act_void_f act_void;                       /**< Function pointer to a act_void_f function*/
} ng_act_ptr;

//Event data struct function pointers
typedef void (*handler_f) (struct _ng_event_base_t *event, uint32_t trigger);   /**< Function pointer to an event handler */
typedef void (*start_f) (struct _ng_event_base_t *event);                       /**< Function pointer to an event start function */
typedef void (*stop_f) (struct _ng_event_base_t *event, bool force_finish);     /**< Function pointer to an event stop function */
typedef void (*pause_toggle_f) (struct _ng_event_base_t *event, bool pause);    /**< Function pointer to an event pause-toggle function */

/**
 * @brief Define that contains all the \a ng_event_base_t data struct attributes that are common among all different event types (oneshot, periodic, transition, periodic transition)
 *
 * These attributes are: \n
 * \a uint32_t trigger:            Pointer to an event assigned to the graphics item \n
 * \a uint32_t retrigger:          Pointer to the draw function \n
 * \a gitem_base_t *src_gitem:     Pointer to the source graphics item \n
 * \a callback_f callback:         Function pointer to callback function \n
 * \a union ng_act_ptr (*action):  Function pointer to action function \n
 * \a void *action_data:           Pointer to the action's data \n
 * \a int affected_screen_id:      ID of the screen that is affected by the event \n
 * \a uint32_t status:             Event status \n
 * \a handler_f handler:           Function pointer the event handler function \n
 * \a start_f start:               Function pointer to the event start function \n
 * \a stop_f stop:                 Function pointer to the evetn stop function \n
 * \a pause_toggle_f pause_toggle: Function pointer to the event pause-toggle function \n
 * \a ng_event_base_t *next:       Pointer to the next event (used by graphics items that accept multiple events) \n
 *
 */
#define EVENT_BASE_STRUCT            \
    uint32_t trigger;                \
    uint32_t retrigger;              \
    struct _gitem_base_t *src_gitem; \
    callback_f callback;             \
    union _ng_act_ptr (*action);     \
    void *action_data;               \
    int affected_screen_id;          \
    uint32_t status;                 \
    handler_f handler;               \
    start_f start;                   \
    stop_f stop;                     \
    pause_toggle_f pause_toggle;     \
    struct _ng_event_base_t *next

/**
 * @brief Event base struct definition
 *
 */
typedef struct _ng_event_base_t
{
    EVENT_BASE_STRUCT; /**< Atributes as defined in the description of EVENT_BASE_STRUCT */
} __ng_event_base_t;

/**
 * @brief Assignes events to graphics items (as generated) and creates the application's timer
 *
 */
void ng_event_init (void);

/**
 * @brief Handles an \a event according to the \a trigger that triggered it
 *
 * @param *event Pointer to the event that needs to be handled
 * @param trigger_event Triggers that caused the the event
 */
void ng_event_handle(ng_event_base_t *event, uint32_t trigger_event);

/**
 * @brief Runs the callback function of the event
 *
 * @param *event Pointer to the event whose callback needs to run
 * @param status_flags Allows the callback to run with specific flags
 */
void ng_event_run_callback(ng_event_base_t *event, int status_flags);

/**
 * @brief Set the status (stopped, running, paused) of an event
 *
 * @param *event Pointer to the event
 * @param status Status to be set
 */
void ng_event_set_status(ng_event_base_t *event, uint32_t status);

/**
 * @brief Checks if a retrigger flag of an event is set or not
 *
 * @param *event Pointer to the event
 * @param flag Flag to be checked
 * @return bool True is the flag is set, otherwize false
 */
bool ng_event_check_retrigger_flag(ng_event_base_t *event, int flag);

/**
 * @brief Checks the status of an event
 *
 * @param *event Pointer to the event
 * @param flag   Flag to be checked
 * @return bool  True is the flag is set, otherwize false
 */
bool ng_event_check_status_flag(ng_event_base_t *event, int flag);

/**
 * @brief Sets a status flag of an event
 *
 * @param *event Pointer to the event
 * @param flag   Flag to be set
 */
void ng_event_set_status_flag(ng_event_base_t *event, int flag);

/**
 * @brief Unets a status flag of an event
 *
 * @param *event Pointer to the event
 * @param flag   Flag to be unset
 */
void ng_event_unset_status_flag(ng_event_base_t *event, int flag);

/**
 * @brief Flips (inverts) a status flag of an event
 *
 * @param *event Pointer to the event
 * @param flag   Flag to be flipped
 */
void ng_event_flip_status_flag(ng_event_base_t *event, int flag);

#define ng_event_is_stopped(ev_ptr) \
    (((ev_ptr)->status & EV_STATUS_MASK) == EV_STATUS_STOPPED) /**< Checks if a screen transition is stopped (returns true/false)*/

#define ng_event_is_running(ev_ptr) \
    (((ev_ptr)->status & EV_STATUS_MASK) == EV_STATUS_RUNNING)  /**< Checks if a screen transition is running (returns true/false)*/

#define ng_event_is_paused(ev_ptr) \
    (((ev_ptr)->status & EV_STATUS_MASK) == EV_STATUS_PAUSED) /**< Checks if an event is paused (returns true/false)*/

#define NG_EVENT(object) SAFE_CAST((object), void *, ng_event_base_t *) /**< Type caster from a derived event data structs (transition, periodic etc.) to the base ng_event_base_t data struct*/

#define PROGRESS(t)              (t)    /**< Redability helper */
#define DURATION(t)              (t)    /**< Redability helper */
#define PERIOD(t)                (t)    /**< Redability helper */
#define START_TIME(t)            (t)    /**< Redability helper */
#define	NG_CALLBACK_DATA(data)	 (data) /**< Redability helper*/

#endif //__NG_EVENT_H__
