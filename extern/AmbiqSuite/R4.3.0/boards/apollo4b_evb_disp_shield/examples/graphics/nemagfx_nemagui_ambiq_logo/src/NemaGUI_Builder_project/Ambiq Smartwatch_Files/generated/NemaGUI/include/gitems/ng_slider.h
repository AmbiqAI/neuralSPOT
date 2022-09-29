//*****************************************************************************
//
// @file ng_slider.h
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

#ifndef NG_GITEM_SLIDER_H__
#define NG_GITEM_SLIDER_H__
#include "ng_gitem.h"

/** Slider widget data struct */
typedef struct _gitem_slider_t
{
    BASE_STRUCT;              /**< Inherited attributes from gitem_base_t data struct*/
    float   value;            /**< Current value [0.f, 1.f]*/
    gitem_base_t *progress;   /**< Pointer to its "progress" child item*/
    gitem_base_t *indicator;  /**< Pointer to its "indicator" child item*/
} gitem_slider_t;

/** \brief Sets the current value (percent) of the slider
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param percent Value in range [0.f , 1.f]. If it is beyond the acceptable range, it is automatically clamped
 * \return void
 *
 */
void ng_slider_set_percent(gitem_base_t *git, float percent);

/** \brief Sets the horizontal slider's indicator horizontal position
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param x Coordinate x (relative)
 * \return void
 *
 */
void ng_slider_horizontal_set_indicator_x(gitem_base_t *git, int x);

/** \brief Sets the vertical slider's indicator vertical position
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param y Coordinate y (relative)
 * \return void
 *
 */
void ng_slider_vertical_set_indicator_y(gitem_base_t *git, int y);

#define NG_SLIDER(object) SAFE_CAST((object), gitem_base_t *, gitem_slider_t *) /**< Type caster from  base gitem_base_t struct to derived gitem_slider_t struct*/

#endif //NG_GITEM_SCREEN_H__
