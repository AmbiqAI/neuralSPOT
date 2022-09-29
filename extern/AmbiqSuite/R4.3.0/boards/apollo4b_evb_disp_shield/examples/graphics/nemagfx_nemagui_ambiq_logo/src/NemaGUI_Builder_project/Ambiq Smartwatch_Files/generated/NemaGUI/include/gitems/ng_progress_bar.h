//*****************************************************************************
//
// @file ng_progress_bar.h
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

#ifndef NG_GITEM_PROGRESS_BAR_H__
#define NG_GITEM_PROGRESS_BAR_H__
#include "ng_gitem.h"
#include "ng_utils.h"

/** Progress bar widget data struct*/
typedef struct _gitem_progress_bar_t
{
    BASE_STRUCT;                     /**< Inherited attributes from gitem_base_t data struct*/
    uint32_t foreground_color;       /**< Foreground color*/
    img_obj_t *background_image;     /**< Pointer to background image asset*/
    img_obj_t *foreground_image;     /**< Pointer to foreground image asset*/
    float      value;                /**< Current value [0.f, 1.f]*/
    uint16_t   pen_width;            /**< Pen width*/
} gitem_progress_bar_t;

/** \brief Draw function (horizontal progress bar)
 *
 * \param *git 	Pointer to needle's base gitem (gitem_base_t data struct)
 * \param x_off Horizontal offset from its parent item
 * \param y_off Vertical offset from its parent item
 * \return void
 *
 */
DRAW_FUNC(ng_horizontal_progress_bar_draw);

/** \brief Draw function (vertical progress bar)
 *
 * \param *git 	Pointer to progress bar's base gitem (gitem_base_t data struct)
 * \param x_off Horizontal offset from its parent item
 * \param y_off Vertical offset from its parent item
 * \return void
 *
 */
DRAW_FUNC(ng_vertical_progress_bar_draw);

/** \brief Sets the current value (percent) of the progress bar
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param percent Value in range [0.f , 1.f]. If it is beyond the acceptable range, it is automatically clamped
 * \return void
 *
 */
void ng_progress_bar_set_percent(gitem_base_t *git, float percent);

/** \brief Sets the background image asset
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param *asset_ptr Pointer to background image asset (casted to img_obj_t internally)
 * \return void
 *
 */
void ng_progress_bar_set_background_image(gitem_base_t *git, void *asset_ptr);

/** \brief Sets the foreground image asset
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param *asset_ptr Pointer to foreground image asset (casted to img_obj_t internally)
 * \return void
 *
 */
void ng_progress_bar_set_foreground_image(gitem_base_t *git, void *asset_ptr);

/** \brief Sets the foreground color
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param rgba  Foreground color value
 * \return void
 *
 */
void ng_progress_bar_set_foreground_color(gitem_base_t *git, uint32_t rgba);

#define NG_PROGRESS_BAR(object) SAFE_CAST((object), gitem_base_t *, gitem_progress_bar_t *) /**< Type caster from  base gitem_base_t struct to derived gitem_progress_bar_t struct*/

#endif //NG_GITEM_PROGRESS_BAR_H__
