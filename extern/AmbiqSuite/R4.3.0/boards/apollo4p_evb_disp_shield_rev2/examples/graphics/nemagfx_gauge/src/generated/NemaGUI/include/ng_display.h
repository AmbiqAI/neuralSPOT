//*****************************************************************************
//
//! @file ng_display.h
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
 * @brief Display
 *
 * The display module, provides functions for updating the framebuffer according to the current context.
 * This can either be displaying a simple screen, a screen transition or a pop-up window on top of a main screen.
 */

#ifndef __NEMAGUI_DISPLAY_H__
#define __NEMAGUI_DISPLAY_H__

#include "nema_core.h"
#include "nema_transitions.h"
#include "ng_gitem.h"

/// @cond NODOC

#ifndef EXTERN
#define EXTERN extern
#define REMEMBER_TO_UNDEFINE_EXTERN
#endif

/// @endcond

#define    DISPLAY_SCREEN            0 /**< Display mode for displaying a screen (default mode)*/
#define    DISPLAY_SCREEN_TRANSITION 1 /**< Display mode when performing a screen transition */
#define    DISPLAY_POPUP             2 /**< Display mode for a pop-up */

/**
 * @brief Draws a screen tree node (screen gitem along with its children) to a designated buffer
 *
 * @param *fb_img Pointer to the designated buffer
 * @param *screen_node Pointer of the screen tree node that needs to be drawn
 * @param x_off x offset
 * @param y_off y offset
 */
void ng_display_screen_node_to_fb(img_obj_t *fb_img, tree_node_t *screen_node, int x_off, int y_off);

/**
 * @brief Clears the current framebuffer
 *
 * @param wait if this is equal to zero, the command list is submitted whithout waiting for an interrupt
 */
void ng_display_screen_clear(int wait);

/**
 * @brief Binds the transition buffers (if available) to the GPU
 *
 */
void ng_display_bind_transition_buffers(void);

/**
 * @brief Updates the display (redraws current screen)
 *
 */
void ng_display(void);

/**
 * @brief Initializes the display module (framebuffer(s), command lists)
 *
 */
void ng_display_init(void);

/**
 * @brief Sets the event used for performing a screen transition
 *
 * @param event Pointer to the screen transition event data struct (casted to ng_transition_t struct internally)
 */
void ng_display_set_event(ng_event_base_t *event);

/**
 * @brief Sets the mode of the display
 *
 * @param mode This can either be DISPLAY_SCREEN, DISPLAY_SCREEN_TRANSITION or DISPLAY_POPUP
 */
void ng_display_set_mode(int mode);

/**
 * @brief Gets the current display mode
 *
 * @return int This should be DISPLAY_SCREEN, DISPLAY_SCREEN_TRANSITION or DISPLAY_POPUP
 */
int ng_display_get_mode();

/**
 * @brief Sets the pop-up tree node to be diplayed along with the display mode (DISPLAY_POPUP)
 *
 * @param node pop-up node to be siplayed
 */
void ng_display_set_popup(tree_node_t *node);

/**
 * @brief Controlls if the display should perform a "clear screen" before updating it
 *
 * @param clear If true, the display will clear its current content before updating it. Otherwise it will draw the new content on top of the old one
 */
void ng_display_set_clear(bool clear);

/**
 * @brief Checks if the buckbuffer "index" is locked (currently not available)
 *
 * @param index Index of the back buffer to check
 * @return bool True if the back buffer is locked, otherwise false
 */
bool ng_back_buffer_is_locked(int index);

/**
 * @brief Locks the back buffer "index"
 *
 * @param index Index of the back buffer to lock
 */
void ng_back_buffer_lock(int index);

/**
 * @brief Unlocks the back buffer "index"
 *
 * @param index Index of the buck buffer to unlock
 */
void ng_back_buffer_unlock(int index);

/**
 * @brief Screen transition effect
 *
 */
EXTERN nema_transition_t global_screen_trans_effect;

/**
 * @brief Command lists used for updating the background of the current screen
 *
 * This is the first of the two command lists that are currently used for rendering a complete frame.
 *
 */
EXTERN nema_cmdlist_t cl_screen;

/**
 * @brief Command lists used for updating the children of the current screen
 *
 * This is the second command list, used for rendering a complete frame.
 *
 */
EXTERN nema_cmdlist_t cl;


#ifdef REMEMBER_TO_UNDEFINE_EXTERN
#undef EXTERN
#undef REMEMBER_TO_UNDEFINE_EXTERN
#endif

#endif //__NEMAGUI_DISPLAY_H__
