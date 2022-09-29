//*****************************************************************************
//
// @file ng_swipe_window.h
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

#ifndef NG_GITEM_SWIPE_WINDOW_H__
#define NG_GITEM_SWIPE_WINDOW_H__
#include "ng_gitem.h"
#include "ng_globals.h"
#include "ng_event_transition.h"
#include "ng_tuples.h"
#include "ng_tree.h"

/** Swipe window gestures data struct */
extern gitem_gestures_t gestures_swipe_window;

/** Swipe widnow widget data struct */
typedef struct _gitem_swipe_window_t
{
    BASE_STRUCT;                         /**< Inherited attributes from gitem_base_t data struct*/
    tree_node_t *indicators_parent;      /**< Pointer to the tree node placeholder of the indicators (if applicable)*/
    uint16_t cur_page_index;             /**< Curretn page index*/
    uint16_t page_count;                 /**< Total page count*/
    uint16_t spacing;                    /**< Spacing between pages*/
    uint8_t layout;                      /**< Layout (horizontal or vertical)*/
    ng_transition_t *animation;          /**< Pointer to a transition event (used for animating the swipe window)*/
    ng_git_int_int_ez_t animation_data;  /**< Animation data (start point, end point and easing function)*/
} gitem_swipe_window_t;

#define NG_SWIPE_WINDOW(object) SAFE_CAST((object), gitem_base_t *, gitem_swipe_window_t *) /**< Type caster from  base gitem_base_t struct to derived gitem_swipe_window_t struct*/

#endif //NG_GITEM_SWIPE_WINDOW_H__