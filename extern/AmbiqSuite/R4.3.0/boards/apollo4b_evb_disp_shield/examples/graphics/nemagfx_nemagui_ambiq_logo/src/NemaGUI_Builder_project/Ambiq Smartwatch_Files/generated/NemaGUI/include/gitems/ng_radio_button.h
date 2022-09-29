//*****************************************************************************
//
// @file ng_radio_button.h
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

#ifndef NG_GITEM_RADIO_BUTTON_H__
#define NG_GITEM_RADIO_BUTTON_H__
#include "ng_gitem.h"
#include "ng_gestures.h"
#include "ng_utils.h"

/** Radio button gestures data struct */
extern gitem_gestures_t gestures_radio_button;

/** Radio button widget data struct */
typedef struct _gitem_radio_button_t
{
    BASE_STRUCT;                   /**< Inherited attributes from gitem_base_t data struct*/
    uint32_t   secondary_color;    /**< Secondary (pressed) color*/
    img_obj_t *background_image;   /**< Pointer to background (not selected) image asset*/
    img_obj_t *foreground_image;   /**< Pointer to foreground (selected) image asset*/
    uint16_t   radius;             /**< Inner circle (if applicable) radius*/
} gitem_radio_button_t;

/** \brief Draw function
 *
 * \param *git 	Pointer to radio button's base gitem (gitem_base_t data struct)
 * \param x_off Horizontal offset from its parent item
 * \param y_off Vertical offset from its parent item
 * \return void
 *
 */
DRAW_FUNC(ng_radio_button_draw);

/** \brief Toggles all radio buttons inside a table
 *
 * \param *node Pointer to the radio button's parent tree node
 * \return void
 *
 */
void ng_radio_button_toggle(tree_node_t *node);

/** \brief Sets the secondary color
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param rgba  Secondary (pressed) color value
 * \return void
 *
 */
void ng_radio_button_set_secondary_color(gitem_base_t *git, uint32_t rgba);

#define NG_RADIO_BUTTON(object) SAFE_CAST((object), gitem_base_t *, gitem_radio_button_t *) /**< Type caster from  base gitem_base_t struct to derived gitem_radio_button_t struct*/

#endif //NG_GITEM_RADIO_BUTTON_H__
