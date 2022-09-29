//*****************************************************************************
//
// @file ng_digital_clock.h
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

#ifndef NG_GITEM_DIGITAL_CLOCK_H__
#define NG_GITEM_DIGITAL_CLOCK_H__
#include "ng_gitem.h"
#include "ng_utils.h"
#include "ng_draw_prim.h"

#define NG_TIME_HH_MM_SS 0 /**< Time format eg. "09:45:18"*/
#define NG_TIME_HH_MM    1 /**< Time format eg. "09:45"*/
#define NG_TIME_H_MM     2 /**< Time format eg. "9:45"*/
#define NG_TIME_HH       3 /**< Time format eg. "09"*/
#define NG_TIME_H        4 /**< Time format eg. "9"*/
#define NG_TIME_MM       5 /**< Time format eg. "45"*/
#define NG_TIME_SS       6 /**< Time format eg. "18"*/

/** Digital clock data struct */
typedef struct _gitem_digital_clock_t
{
    BASE_STRUCT;                /**< Inherited attributes from gitem_base_t data struct*/
    uint32_t     text_color;    /**< Text color*/
    nema_font_t *font;          /**< Pointer to font asset*/
    uint32_t     alignment;     /**< Horizontal/Vertical alignment (bitfields)*/
    uint32_t     time_format;   /**< Time format*/
} gitem_digital_clock_t;

/** \brief Draw function
 *
 * \param *git 	Pointer to digital clocks's base gitem (gitem_base_t data struct)
 * \param x_off Horizontal offset from its parent item
 * \param y_off Vertical offset from its parent item
 * \return void
 *
 */
DRAW_FUNC(ng_digital_clock_draw);

/** \brief Updates the clock's text according to the current time. System's wall time is used by default.
 *
 * In order to use a different time update method (not the system's wall time), the define WALL_TIME_CLOCKS (defined in the
 * compiler flags of the generated Makefile) needs to be undefined and the time needs to be updated inside the #else segment of this function.
 *
 * \param *git 	Pointer to target gitem (gitem_base_t data struct)
 * \return void
 *
 */
void ng_digital_clock_update(gitem_base_t *git);

#define NG_DIGITAL_CLOCK(object) SAFE_CAST((object), gitem_base_t *, gitem_digital_clock_t *) /**< Type caster from  base gitem_base_t struct to derived gitem_digital_clock_t struct*/

#endif //NG_GITEM_DIGITAL_CLOCK_H__
