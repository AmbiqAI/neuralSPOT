//*****************************************************************************
//
// @file ng_digimeter.h
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

#ifndef NG_GITEM_DIGIMETER_H__
#define NG_GITEM_DIGIMETER_H__
#include "ng_gitem.h"
#include "ng_utils.h"
#include "ng_draw_prim.h"

/** Digital meter widget data struct */
typedef struct _gitem_digimeter_t
{
    BASE_STRUCT;                     /**< Inherited attributes from gitem_base_t data struct*/
    uint32_t     text_color;         /**< Text color*/
    float        value;              /**< Current value*/
    float        max_value;          /**< Maximum value*/
    float        min_value;          /**< Minimum value*/
    float        step;               /**< Step (increase/decrease by)*/
    nema_font_t *font;               /**< Pointer to font asset*/
    uint32_t     alignment;          /**< Horizontal/Vertical alignment (bitfields)*/
    uint8_t      dec_precision;      /**< Decimal digits*/
    uint8_t      int_precision;      /**< Integer digits. If this is bigger than zero, padding with zeros takes place on empty digits)*/
    char        *suffix;             /**< Suffix string*/
} gitem_digimeter_t;

/** \brief Draw function
 *
 * \param *git 	Pointer to digimeter's base gitem (gitem_base_t data struct)
 * \param x_off Horizontal offset from its parent item
 * \param y_off Vertical offset from its parent item
 * \return void
 *
 */
DRAW_FUNC(ng_digimeter_draw);

/** \brief Sets the current value of the digital meter
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param value Value in range [min_value , max_value]. If it is beyond the acceptable range, it is automatically clamped
 * \return void
 *
 */
void ng_digimeter_set_value(gitem_base_t *git, float value);

/** \brief Sets the current value of the digital meter by percent (value = percent*(max_val-min_val) + min_val). Percent must be within [0.f, 1.f]
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param percent Percentage in range [0.f, 1.f]. If it is beyond the acceptable range, it is automatically clamped
 * \return void
 *
 */
void ng_digimeter_set_percent(gitem_base_t *git, float percent);

/** \brief Count up, increase the digital meter's value by its step
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \return void
 *
 */
void ng_digimeter_count_up(gitem_base_t *git);

/** \brief Count down, decrease the digital meter's value by its step
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \return void
 *
 */
void ng_digimeter_count_down(gitem_base_t *git);

/** \brief Sets the digital meter's text color
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \param rgba Color value
 * \return void
 *
 */
void ng_digimeter_set_text_color(gitem_base_t *git, uint32_t rgba);

#define NG_DIGIMETER(object) SAFE_CAST((object), gitem_base_t *, gitem_digimeter_t *) /**< Type caster from  base gitem_base_t struct to derived gitem_digimeter_t struct*/

#endif //NG_GITEM_DIGIMETER_H__
