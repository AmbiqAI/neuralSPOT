//*****************************************************************************
//
//  ng_checkbox.h
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

#ifndef NG_GITEM_CHECKBOX_H__
#define NG_GITEM_CHECKBOX_H__
#include "ng_gitem.h"
#include "ng_tree.h"
#include "ng_gestures.h"
#include "ng_utils.h"

/** Checkbox gestures data struct */
extern gitem_gestures_t gestures_checkbox;

/** Checkbox widget data struct */
typedef struct _gitem_checkbox_t
{
    BASE_STRUCT;                   /**< Inherited attributes from gitem_base_t data struct*/
    uint32_t   secondary_color;    /**< Secondary color (color when checkbox is pressed)*/
    img_obj_t *background_image;   /**< Pointer to background image asset*/
    img_obj_t *foreground_image;   /**< Pointer to foreground image asset (eg. a checkmark)*/
    uint16_t   pen_width;          /**< Pen width (for painting the checkbox outline)*/
} gitem_checkbox_t;

/** \brief Draw function
 *
 * \param *git 	Pointer to checkbox's base gitem (gitem_base_t data struct)
 * \param x_off Horizontal offset from its parent item
 * \param y_off Vertical offset from its parent item
 * \return void
 *
 */
DRAW_FUNC(ng_checkbox_draw);

/** \brief Sets the foreground image asset
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param *asset_ptr Pointer to image asset (casted to img_obj_t internally)
 * \return void
 *
 */
void ng_checkbox_set_image(gitem_base_t *git, void *asset_ptr);

/** \brief Sets the secondary color
 *
 * \param *git Pointer to target gitem (gitem_base_t data struct)
 * \param rgba Color value
 * \return void
 *
 */
void ng_checkbox_set_secondary_color(gitem_base_t *git, uint32_t rgba);

#define NG_CHECKBOX(object) SAFE_CAST((object), gitem_base_t *, gitem_checkbox_t *) /**< Type caster from  base gitem_base_t struct to derived gitem_checkbox_t struct*/

#endif //NG_GITEM_CHECKBOX_H__
