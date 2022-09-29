//*****************************************************************************
//
// @file ng_gestures.h
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

/*
 * "Gestures" is responsible for handling user gestures on the display.
 * a) handle press, release, swipe on an item
 * b) start, pause, update a screen transition
 * c) provide default function definitions that can be adopted in widgets
 */
#ifndef NG_GESTURES_H
#define NG_GESTURES_H

#include "ng_tree.h"
#include "ng_globals.h"
#include "nema_event.h"

/**
 * @brief Gesture's abort function signature
 *
 */
#define GESTURE_FUNC_ABORT_PRESS(NAME)  void NAME (tree_node_t *node)

/**
 * @brief Gesture's release function signature
 *
 */
#define GESTURE_FUNC_RELEASE(NAME)  void NAME (tree_node_t *node)

/**
 * @brief Gesture's press function signature
 *
 */
#define GESTURE_FUNC_PRESS(NAME)  void NAME (tree_node_t *node, int x, int y)

/**
 * @brief Gesture's swipe function signature
 *
 */
#define GESTURE_FUNC_SWIPE(NAME)  void NAME (tree_node_t *node, int x, int y, int dx, int dy)

/**
 * @brief Gesture's release function definition
 *
 */
typedef GESTURE_FUNC_RELEASE(release_gesture_func_t);

/**
 * @brief Gesture's press function definition
 *
 */
typedef GESTURE_FUNC_PRESS(press_gesture_func_t);

/**
 * @brief Gesture's swipe function definition
 *
 */
typedef GESTURE_FUNC_SWIPE(swipe_gesture_func_t);

/**
 * @brief Gesture's abort function definition
 *
 */
typedef GESTURE_FUNC_ABORT_PRESS(abort_gesture_func_t);

/** Data struct that contains function pointers to gestures */
typedef struct _gitem_gestures_t
{
    press_gesture_func_t   *press;   /**< Function pointer to press function */
    release_gesture_func_t *release; /**< Function pointer to release function */
    swipe_gesture_func_t   *swipe;   /**< Function pointer to swipe function */
    abort_gesture_func_t   *abort;   /**< Function pointer to abort function */
} gitem_gestures_t;

/**
 * @brief Function executed on mouse/finger press
 *
 * @param *event Pointer to the press event as forwarded by the main loop
 * @param event_press_x The x position (absolute coordinate) of the press event
 * @param event_press_y The y position (absolute coordinate) of the press event
 * @return tree_node_t* The tree node that accepted the "press" if its graphics item supports press, otherwise NULL
 */
tree_node_t* ng_gestures_press(nema_event_t *event, int event_press_x, int event_press_y);

/**
 * @brief Function executed on mouse/finger release
 *
 * @param *event Pointer to the release event as forwarded by the main loop
 */
void ng_gestures_release(nema_event_t *event);

/**
 * @brief  Function executed on mouse/finger swipe/drag
 *
 * @param *event Pointer to the release event as forwarded by the main loop
 * @param mouse_dx Horizontal difference in pixels to the previously captured event
 * @param mouse_dy Vertical difference in pixels to the previously captured event
 */
void ng_gestures_swipe(nema_event_t *event, int mouse_dx, int mouse_dy);

/**
 * @brief
 *
 * @param x Gesture's horizontal position
 * @param y Gesture's vertical position
 * @return bool True if the gesture was performed inside a pop-up, otherwise false
 */
bool ng_gestures_is_inside_popup(int x, int y);

// /**
//  * @brief
//  *
//  * @param event
//  */
// void ng_gestures_set_screen_transition_event(ng_event_base_t *event);

#endif // NG_GESTURES_H
