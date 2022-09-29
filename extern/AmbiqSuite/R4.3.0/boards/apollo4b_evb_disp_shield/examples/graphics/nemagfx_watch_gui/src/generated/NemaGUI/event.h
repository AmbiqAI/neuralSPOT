//*****************************************************************************
//
//! @file event.h
//!
//! @brief NemaGFX example.
//!
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

#ifndef __EVENT_H__
#define __EVENT_H__

typedef enum
{
    EVENT_NULL,          //0
    EVENT_PRESS,         //1
    EVENT_RELEASE,       //2
    EVENT_HOLD,          //3
    EVENT_DRAG,          //4
    EVENT_VALUE_CHANGED, //5
    EVENT_TIMER          //6
} event_e;

typedef enum
{
    ACTION_NULL,            //0
    ACTION_TOGGLE,          //1
    ACTION_SET_CHECKED,     //2
    ACTION_SET_UNCHECKED,   //3
    ACTION_SET_PRESSED,     //4
    ACTION_SET_RELEASED,    //5
    ACTION_COUNT_UP,        //6
    ACTION_COUNT_DOWN,      //7
    ACTION_SET_VALUE,       //8
    ACTION_RESET,           //9
    ACTION_POP_UP,          //10
    ACTION_SHOW_SCREEN,     //11
    ACTION_HIDE,            //12
    ACTION_SHOW,            //13
    ACTION_CUSTOM           //14
} action_e;

typedef enum
{
    EVENT_ONESHOT,            //0
    EVENT_PERIODIC,           //1
    EVENT_TRANSITION,         //2
    EVENT_PERIODIC_TRANSITION //3
} event_type_e;

typedef enum
{
    EVENT_IGNORE,     //0
    EVENT_RESET,      //1
    EVENT_PAUSE,      //2
} event_re_trigger_e;

typedef enum
{
    EV_STATUS_STOPPED,     //0
    EV_STATUS_RUNNING,     //1
    EV_STATUS_PAUSED ,     //2
} event_state_e;

typedef enum
{
    //------------ SCREEN EFFECTS ---------
    EV_SCREEN_EFFECT_NONE,             //0
    EV_SCREEN_EFFECT_FADE,             //1
    EV_SCREEN_EFFECT_LINEAR_HOR_LEFT,  //2
    EV_SCREEN_EFFECT_LINEAR_HOR_RIGHT, //3
    EV_SCREEN_EFFECT_LINEAR_VER_UP,    //4
    EV_SCREEN_EFFECT_LINEAR_VER_DOWN,  //5
    EV_SCREEN_EFFECT_CUBE_HOR_LEFT,    //6
    EV_SCREEN_EFFECT_CUBE_HOR_RIGHT,   //7
    EV_SCREEN_EFFECT_FADE_ZOOM,        //8
    EV_SCREEN_EFFECT_CLOCKWISE,        //9
    EV_SCREEN_EFFECT_COUNT,            //10
    //----------- GENERIC EFFECTS ---------
    EV_EFFECT_NONE,                    //11
    EV_EFFECT_FADE,                    //12
    EV_EFFECT_FADE_ZOOM,               //13
    EV_EFFECT_FLY,                     //14
    EV_EFFECT_CUBE_LEFT,               //15
    EV_EFFECT_CUBE_RIGHT,              //16
    EV_EFFECT_CUBE_UP,                 //17
    EV_EFFECT_CUBE_DOWN,               //18
    EV_EFFECT_FLIP_HOR,                //19
    EV_EFFECT_FLIP_VER,                //20
} event_trans_effect_e;

typedef struct _event_t
{
    gitem_t *src_gitem;
    tree_node_t *target_tree_node;
    event_e trigger_event;
    action_e action;
    struct _event_t *next;
    void (*callback)(struct _event_t* event);
    float (*ez_func)(float p);
    event_state_e status; //boolean
    double progress;
    event_type_e type;
    float start_time;
    float duration;
    float period;
    event_re_trigger_e re_trigger;
    float pause_time;
    event_trans_effect_e effect;
} event_t;

#endif //__EVENT_H__
