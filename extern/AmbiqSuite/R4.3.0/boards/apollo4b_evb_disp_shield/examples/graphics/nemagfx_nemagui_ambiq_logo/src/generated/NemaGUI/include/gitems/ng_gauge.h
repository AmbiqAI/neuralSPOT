//*****************************************************************************
//
//  ng_gauge.h
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
#ifndef NG_GITEM_GAUGE_H__
#define NG_GITEM_GAUGE_H__
#include "ng_gitem.h"
#include "ng_gestures.h"
#include "ng_utils.h"

/** Gauge gestures data struct */
extern gitem_gestures_t gestures_gauge;

/** Gauge widget data struct */
typedef struct _gitem_gauge_t
{
    BASE_STRUCT;                 /**< Inherited attributes from gitem_base_t data struct*/
    img_obj_t    *image;         /**< Pointer to image asset*/
    gitem_base_t *needle;        /**< Pointer to its "needle child item"*/
    float         value;         /**< Current value*/
    float         max_value;     /**< Maximum value*/
    float         min_value;     /**< Minimum value*/
    float         angle;         /**< Needle's current angle*/
    float         max_angle;     /**< Maximum angle*/
    float         min_angle;     /**< Minimum angle*/
    int           x_rot;         /**< Rotation x relative coordinate*/
    int           y_rot;         /**< Rotation y relative coordinate*/
    uint16_t      pen_width;     /**< Pen width (used when gauge is outlined)*/
} gitem_gauge_t;

/** \brief Draw function
 *
 * \param *git 	Pointer to gauge's base gitem (gitem_base_t data struct)
 * \param x_off Horizontal offset from its parent item
 * \param y_off Vertical offset from its parent item
 * \return void
 *
 */
DRAW_FUNC(ng_gauge_draw);

/** \brief Sets the current value of thegauge
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param value Value
 * \return void
 *
 */
void ng_gauge_set_value(gitem_base_t *git, float value);

/** \brief Sets the current value of the gauge by percent (value = percent*(max_val-min_val) + min_val). Percent must be within [0.f, 1.f]
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param percent Percentage value
 * \return void
 *
 */
void ng_gauge_set_percent(gitem_base_t *git, float percent);

/** \brief Sets the image asset
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param *asset_ptr Pointer to image asset (casted to img_obj_t internally)
 * \return void
 *
 */
void ng_gauge_set_image(gitem_base_t *git, void *asset_ptr);

#define NG_GAUGE(object) SAFE_CAST((object), gitem_base_t *, gitem_gauge_t *) /**< Type caster from  base gitem_base_t struct to derived gitem_gauge_t struct*/

#endif //NG_GITEM_GAUGE_H__
