//*****************************************************************************
//
//  ng_toggle_button.h
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
#ifndef NG_GITEM_TOGGLE_BUTTON_H__
#define NG_GITEM_TOGGLE_BUTTON_H__
#include "ng_gitem.h"

/**
 * @file
 * @brief Toggle button widget interface
 *
 */

/** Toggle button gestures data struct */
extern gitem_gestures_t gestures_toggle_button;

/** Toggle button widget data struct */
typedef struct _gitem_toggle_button_t
{
    BASE_STRUCT;            /**< Inherited attributes from gitem_base_t data struct*/
    uint16_t cur_state;     /**< Current state's index*/
    uint16_t max_state;     /**< State count*/
    img_obj_t**    images;  /**< Array of pointers to image assets, each image corresponds to each state*/
} gitem_toggle_button_t;

/** \brief Draw function
 *
 * \param *git 	Pointer to toggle button's base gitem (gitem_base_t data struct)
 * \param x_off Horizontal offset from its parent item
 * \param y_off Vertical offset from its parent item
 * \return void
 *
 */
DRAW_FUNC(ng_toggle_button_draw);

#define NG_TOGGLE_BUTTON(object) SAFE_CAST((object), gitem_base_t *, gitem_toggle_button_t *) /**< Type caster from  base gitem_base_t struct to derived gitem_toggle_button_t struct*/

#endif //NG_GITEM_TOGGLE_BUTTON_H__
