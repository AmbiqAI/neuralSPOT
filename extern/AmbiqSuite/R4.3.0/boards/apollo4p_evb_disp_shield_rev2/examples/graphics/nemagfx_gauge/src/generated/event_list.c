//*****************************************************************************
//
//! @file event_list.c
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

#include "gui_tree.h"
#include "event_list.h"
#include "ng_screen_trans.h"
#include "ng_globals.h"
#include "ng_timer.h"
#include "ng_display.h"
#include "ng_callbacks.h"
#include "nema_easing.h"
#include "custom_callbacks.h"
#include "images.h"

#define EVENT_LIST_SIZE   7
#define TEMP_ANIMATIONS   5

#define TIMER_PERIOD_MS 16

//Temprary animations - events reserved by the project (do not edit)
//----------------------------------------------------------------------------------------------------------------
static ng_node_effect_direction_t screen_transition_data = {NULL, NEMA_TRANS_LINEAR_H, 0};
static ng_act_ptr dummy_action1;
static ng_act_ptr dummy_action2;
static ng_act_ptr dummy_action3;
static ng_act_ptr dummy_action4;

static ng_transition_t temp_anim_0  = {EV_TRIGGER_NULL, EV_RETRIGGER_IGNORE, NULL, ng_screen_trans_swipable, NULL, NG_CALLBACK_DATA(&screen_transition_data), -1, EV_STATUS_STOPPED, ng_transition_handler, ng_transition_start, ng_transition_stop, ng_transition_pause_toggle, NULL, 0.f, 1.f, 2.f};
static ng_transition_t temp_anim_1  = {EV_TRIGGER_NULL, EV_RETRIGGER_IGNORE, NULL, NULL, &dummy_action1, NULL, -1, EV_STATUS_STOPPED, ng_transition_handler, ng_transition_start, ng_transition_stop, ng_transition_pause_toggle, NULL, 0.f, 1.f, 2.f};
static ng_transition_t temp_anim_2  = {EV_TRIGGER_NULL, EV_RETRIGGER_IGNORE, NULL, NULL, &dummy_action2, NULL, -1, EV_STATUS_STOPPED, ng_transition_handler, ng_transition_start, ng_transition_stop, ng_transition_pause_toggle, NULL, 0.f, 1.f, 2.f};
static ng_transition_t temp_anim_3  = {EV_TRIGGER_NULL, EV_RETRIGGER_IGNORE, NULL, NULL, &dummy_action3, NULL, -1, EV_STATUS_STOPPED, ng_transition_handler, ng_transition_start, ng_transition_stop, ng_transition_pause_toggle, NULL, 0.f, 1.f, 2.f};
static ng_transition_t temp_anim_4  = {EV_TRIGGER_NULL, EV_RETRIGGER_IGNORE, NULL, NULL, &dummy_action4, NULL, -1, EV_STATUS_STOPPED, ng_transition_handler, ng_transition_start, ng_transition_stop, ng_transition_pause_toggle, NULL, 0.f, 1.f, 2.f};
//----------------------------------------------------------------------------------------------------------------

//on__application_start actions
static ng_git_float_float_ez_t data_set_value_0 = { NG_GITEM(&_1244Gauge1), 0.f, 8.f, nema_ez_sin_in_out};
static ng_act_ptr action_set_value_0 = {.act_gitptr_float = ng_gauge_set_value};
static ng_periodic_transition_t set_value_0 = {EV_TRIGGER_APP_START, EV_RETRIGGER_PAUSE_TOGGLE, NULL, ng_animate_float, &action_set_value_0, &data_set_value_0, 1149, EV_STATUS_STOPPED, ng_periodic_transition_handler, ng_periodic_transition_start, ng_periodic_transition_stop, ng_periodic_transition_pause_toggle, NULL, START_TIME(0.f), DURATION(3.f), PROGRESS(0.f), PERIOD(3.f)};

static ng_git_float_float_ez_t data_set_value_1 = { NG_GITEM(&_1251Digital_Meter2), 0.f, 15.f, nema_ez_sin_in_out};
static ng_act_ptr action_set_value_1 = {.act_gitptr_float = ng_digimeter_set_value};
static ng_periodic_transition_t set_value_1 = {EV_TRIGGER_APP_START, EV_RETRIGGER_PAUSE_TOGGLE, NULL, ng_animate_float, &action_set_value_1, &data_set_value_1, 1149, EV_STATUS_STOPPED, ng_periodic_transition_handler, ng_periodic_transition_start, ng_periodic_transition_stop, ng_periodic_transition_pause_toggle, NULL, START_TIME(0.f), DURATION(3.f), PROGRESS(0.f), PERIOD(3.f)};


static ng_event_base_t* event_list[EVENT_LIST_SIZE];

void event_list_init()
{
    event_list[0] = (NG_EVENT(&temp_anim_0));
    event_list[1] = (NG_EVENT(&temp_anim_1));
    event_list[2] = (NG_EVENT(&temp_anim_2));
    event_list[3] = (NG_EVENT(&temp_anim_3));
    event_list[4] = (NG_EVENT(&temp_anim_4));

    event_list[5] = (NG_EVENT(&set_value_0));
    event_list[6] = (NG_EVENT(&set_value_1));


    ng_globals_register_screen_transition_event(event_list[0]);
    ng_globals_register_event_list(event_list, EVENT_LIST_SIZE, TEMP_ANIMATIONS);
    ng_timer_set_period(TIMER_PERIOD_MS);
}
