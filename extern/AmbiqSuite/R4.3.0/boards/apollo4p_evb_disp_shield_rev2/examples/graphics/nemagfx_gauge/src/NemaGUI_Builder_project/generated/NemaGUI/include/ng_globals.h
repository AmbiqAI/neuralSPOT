//*****************************************************************************
//
//! @file ng_globals.h
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

#ifndef NG_GLOBALS_H
#define NG_GLOBALS_H

//NemaGFX
#include "nema_core.h"
#include "nema_font.h"
#include "nema_dc_hal.h"
#include "nema_dc.h"
#include "nema_easing.h"
#include "nema_transitions.h"
#include "nema_utils.h"

//NemaGUI
#include "ng_gitem.h"
#include "ng_tuples.h"
#include "ng_gestures.h"
#include "ng_tree.h"
#include "ng_progress_bar.h"
#include "ng_utils.h"
#include "ng_animation.h"
#include "ng_callbacks.h"

#include "ng_event.h"
#include "ng_event_transition.h"
#include "ng_event_oneshot.h"
#include "ng_event_periodic.h"
#include "ng_event_periodic_transition.h"

//All Gitems/Widgets
#include "ng_button.h"
#include "ng_checkbox.h"
#include "ng_circle.h"
#include "ng_circular_progress.h"
#include "ng_container.h"
#include "ng_digimeter.h"
#include "ng_gauge.h"
#include "ng_image.h"
#include "ng_label.h"
#include "ng_needle.h"
#include "ng_progress_bar.h"
#include "ng_radio_button.h"
#include "ng_rect.h"
#include "ng_rounded_rect.h"
#include "ng_screen.h"
#include "ng_slider.h"
#include "ng_slider_hor.h"
#include "ng_slider_ver.h"
#include "ng_watchface.h"
#include "ng_icon.h"
#include "ng_window.h"
#include "ng_toggle_button.h"
#include "ng_swipe_window.h"
#include "ng_digital_clock.h"

/// @cond NODOC
#ifndef EXTERN
#define EXTERN extern
#define REMEMBER_TO_UNDEFINE_EXTERN
#endif

#ifdef DEBUG_PROC_ENTRY
#define PROC_ENTRY printf("%s:%s\r\n", __FILE__, __func__)
#else
#define PROC_ENTRY
#endif
/// @endcond

EXTERN int NG_RESX; /**< Application's horizontal resolution */
EXTERN int NG_RESY; /**< Application's vertical resolution */

EXTERN tree_node_t *popup_node; /**< Pointer to the pop-up tree node (valid when displaying a pop-up)*/
EXTERN int popup_off_x;         /**< Pop-up's horizontal offset */
EXTERN int popup_off_y;         /**< Pop-up's vertical offset */

EXTERN bool NG_DISPLAY_UPDATE;  /**< Initialized as false in the beggining of every iteration of the main loop and updated afterwards. When true the GPU will update the framebuffer at the end of the main loop*/

#define NG_LAYOUT_HOR 0 /**< Horizontal Layout */
#define NG_LAYOUT_VER 1 /**< Vertical Layout */

EXTERN ng_event_base_t *NG_SCREEN_TRANSITION_EVENT; /**< Pointer to the event used for performing screen transitions */

#define SCREEN_TRANSITION_PAUSED \
    ( ng_event_is_paused(NG_SCREEN_TRANSITION_EVENT) ) /**< Checks if the screen transition event is paused (returns true/false)*/

#define SCREEN_TRANSITION_RUNNING \
    ( ng_event_is_running(NG_SCREEN_TRANSITION_EVENT) ) /**< Checks if the screen transition event is running (returns true/false)*/

#define SCREEN_TRANSITION_STOPPED \
    ( ng_event_is_stopped(NG_SCREEN_TRANSITION_EVENT) ) /**< Checks if the screen transition event is stopped (returns true/false)*/

#define DOING_SCREEN_TRANSITION \
     ( !SCREEN_TRANSITION_STOPPED ) /**< Checks if a screen transition is stopped (returns true/false)*/

EXTERN int               NG_EVENT_LIST_SIZE;         /**< Size of the generated event list*/
EXTERN int               NG_TEMP_ANIMATIONS_COUNT;   /**< Maximum count of temporary animations (eg. swipe window animation)*/
EXTERN ng_event_base_t **NG_EVENT_LIST;              /**< Pointer to the generated event list*/
EXTERN float             NG_WALL_TIME;               /**< System's wall time (updated by the API, the user should use it as read-only)*/

EXTERN int                NG_SCREEN_GROUPS_COUNT;    /**< Count of the screen groups (application specific)*/
EXTERN int                NG_POPUP_COUNT;            /**< Count of the pop-ups (application specific)*/
EXTERN int               *NG_SCREENS_PER_GROUP;      /**< Array that contains how many screens belong to each screen group (application specific)*/
EXTERN tree_node_t     ***NG_NODES_PER_GROUP;        /**< Array that contains pointers to arrays that contain the tree nodes of each screen group*/
EXTERN nema_transition_t *NG_EFFECT_PER_GROUP;       /**< Transition effect per screen goup (application specific)*/
EXTERN uint8_t           *NG_LAYOUT_PER_GROUP;       /**< Layout (horizontal-vertical) per screen group (application specific)*/
EXTERN tree_node_t      **NG_POPUP_NODES;            /**< Array that contains the pointers of all the pop-up tree nodes (application specific)*/
EXTERN int                NG_CUR_SCREEN_GROUP_INDEX; /**< Index of the current screen group*/
EXTERN int                NG_CUR_SCREEN_NODE_INDEX;  /**< Index of the current screen*/
EXTERN tree_node_t      **NG_CUR_SCREEN_GROUP_NODES; /**< Array with pointers to the tree nodes of the current screen group*/


//framebuffers
EXTERN int             NG_FRAMEBUFFER_COUNT; /**< Count of the (front) framebuffers (application specific)*/
EXTERN int             NG_BACKBUFFER_COUNT;  /**< Count of the backbuffers (application specific)*/
EXTERN img_obj_t      *NG_FRAMEBUFFER;       /**< Array that contains the framebuffers*/
EXTERN img_obj_t      *NG_BACKBUFFER;        /**< Array that contains the backbuffers*/
EXTERN nemadc_layer_t *NG_DC_LAYER;          /**< Array that contains the display controller layers*/

/**
 * @brief Sets the resolution of the applciation (NG_RESX and NG_RESY variables)
 *
 * @param resx Horizontal resolution
 * @param resy Vertical resolution
 */
void ng_globals_set_resolution(int resx, int resy);

/**
 * @brief Registers the screen transition event to the API. By default, this event is the first event of the generated event list
 *
 * @param event Pointer to the event that will be used for the screen transitions
 */
void ng_globals_register_screen_transition_event(ng_event_base_t *event);

/**
 * @brief Registers the generated event list to the API
 *
 * @param **event_list     List (array) with pointers to the generated events
 * @param list_size        Size of the event list
 * @param temp_animations  Maximum count of temporary animations (eg. swipe window animation)
 */
void ng_globals_register_event_list(ng_event_base_t **event_list, int list_size, int temp_animations);

/**
 * @brief Registers the generated screen groups to the API
 *
 * @param group_count         Total count of the screen groups
 * @param popup_count         Total count the pop-ups
 * @param *screens_per_group  Array that contains how many screens belong to each screen group
 * @param ***nodes_per_group  Array that contains pointers to arrays that contain the tree nodes of each screen group
 * @param *effect_per_group   Transition effect per screen goup
 * @param *layout_per_group   Layout (horizontal-vertical) per screen group (application specific)
 * @param **popup_nodes       Array that contains the pointers of all the pop-up tree nodes
 * @param cur_group           Index of the current screen group
 * @param cur_screen          Index of the current screen
 * @param **cur_group_nodes   Array with pointers to the tree nodes of the current screen group
 */
void ng_globals_register_screen_groups(int group_count, int popup_count, int *screens_per_group, tree_node_t ***nodes_per_group, nema_transition_t *effect_per_group, uint8_t *layout_per_group, tree_node_t **popup_nodes, int cur_group, int cur_screen, tree_node_t **cur_group_nodes);

/**
 * @brief Registers the framebuffers to the API
 *
 * @param frame_buffer_count  Count of the (front) framebuffers
 * @param frame_buffers       Array that contains the framebuffers
 * @param back_buffer_count   Count of the backbuffers
 * @param back_buffers        Array that contains the backbuffers
 * @param layers              Array that contains the display controller layers
 */
void ng_globals_register_framebuffers(int frame_buffer_count, img_obj_t *frame_buffers, int back_buffer_count, img_obj_t *back_buffers, nemadc_layer_t *layers);

/**
 * @brief Performs a sanity check, that the generated project's parameters have been properly registered to the API
 *
 * @return int Zero if everything has been registred corectly, otherwize a positive number that indicates the error
 */
int ng_globals_sanity_check();

/// @cond NODOC
#ifdef REMEMBER_TO_UNDEFINE_EXTERN
#undef EXTERN
#undef REMEMBER_TO_UNDEFINE_EXTERN
#endif
/// @endcond

#endif // NG_GLOBALS_
