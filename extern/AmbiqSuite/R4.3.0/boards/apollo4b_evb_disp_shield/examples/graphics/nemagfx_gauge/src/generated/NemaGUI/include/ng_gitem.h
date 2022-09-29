//*****************************************************************************
//
//! @file ng_gitem.h
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

#ifndef NG_GITEM_H__
#define NG_GITEM_H__

#include "nema_core.h"
#include "nema_utils.h"
#include "nema_font.h"

#include "ng_typedefs.h"

#define GITEMF_ALWAYS            0x00000001U /**< Reserved */
#define GITEMF_PRESS             0x00000002U /**< The graphics item accepts mouse press events */
#define GITEMF_RELEASE           0x00000004U /**< The graphics item accepts mouse release events */
#define GITEMF_HOLD              0x00000008U /**< The graphics item accepts mouse hold events (reserved for future use)*/

#define GITEMF_DRAG              0x00000010U /**< The graphics item accepts mouse drag (swipe) events */
#define GITEMF_FILL_COLOR        0x00000020U /**< The graphics item draw function fills a color in the item's geometry */
#define GITEMF_FILL_IMAGE        0x00000040U /**< The graphics item draw function blits a texture in the item's geometry */
#define GITEMF_FILL_OUTLINE      0x00000080U /**< The graphics item draw function draw's the outline of the item */

#define GITEMF_HIGHLIGHTED       0x00000100U /**<The graphics item is highlighted */
#define GITEMF_CHECKED           0x00000200U /**<The graphics item is checked */
#define GITEMF_STOP_RECUR        0x00000400U /**< Stop a recursion flag */
#define GITEMF_HIDDEN            0x00000800U /**<The graphics item is hidden */

#define GITEMF_ANIMATED          0x00001000U /**<The graphics item is animated (displayed using a back buffer) */
#define GITEMF_PRESS_SCALE       0x00002000U /**<The graphics item scales itself (size) when pressed */
#define GITEMF_CONTAINS_WINDOW   0x00003000U /**<Apllicable to "Screen", indicates whether the screen contains a window or not*/

/**
 * @brief Enumerator that contains the various graphics items types. Each type is documented in the next section (widgets)
 *
 */
typedef enum
{
    GITEM_MAIN_SCREEN,             /**< Main screen */
    GITEM_CONTAINER,               /**< Container */
    GITEM_CIRCLE,                  /**< Circle */
    GITEM_RECT,                    /**< Rectangle */
    GITEM_ROUNDED_RECT,            /**< Rounded rectangle */
    GITEM_IMAGE,                   /**< Image */
    GITEM_LABEL,                   /**< Label */
    GITEM_LABEL_BUTTON,            /**< Label button */
    GITEM_CHECKBOX,                /**< Checkbox */
    GITEM_RADIO_BUTTON,            /**< Radio button */
    GITEM_HORIZONTAL_SLIDER,       /**< Horizontal slider */
    GITEM_VERTICAL_SLIDER,         /**< Vertical slider */
    GITEM_DIGITAL_METER,           /**< Digital meter */
    GITEM_WINDOW_SCREEN,           /**< Window (secondary) screen*/
    GITEM_ICON,                    /**< Icon */
    GITEM_TABLE,                   /**< Table */
    GITEM_HORIZONTAL_PROGRESS_BAR, /**< Horizontal progress bar */
    GITEM_GAUGE,                   /**< Gauge */
    GITEM_NEEDLE,                  /**< Needle */
    GITEM_ICON_BUTTON,             /**< Icon button */
    GITEM_WINDOW,                  /**< Window */
    GITEM_CIRCULAR_PROGRESS,       /**< Circular progress */
    GITEM_WATCH_FACE,              /**< Watch-face */
    GITEM_DIGITAL_CLOCK,           /**< Digital clock */
    GITEM_VERTICAL_PROGRESS_BAR,   /**< Vertical progress bar */
    GITEM_SWIPE_WINDOW,            /**< Swipe window */
    GITEM_TOGGLE_BUTTON,           /**< Toggle button */
    GITEM_TYPE_COUNT               /**< Reserved */
} gitem_type_e;

struct _ng_event_base_t;

/**
 * @brief Define that contains all the \a gitem_base_t data struct attributes that are common among all different graphics items (widgets)
 *
 * These attributes are: \n
 * \a ng_event_base_t *event: Pointer to an event assigned to the graphics item \n
 * \a draw_f *draw:           Pointer to the draw function \n
 * \a uint32_t flags:         Graphics item's flags (as defined earlier) \n
 * \a int x:                  Horizontal (x) offset (with respect to its parent) \n
 * \a int y:                  Vertical (y) offset (with respect to its parent) \n
 * \a uint16_t w:             Width \n
 * \a uint16_t h:             Height \n
 * \a int id:                 Unique identification number \n
 * \a gitem_type_e type:      Enumerator that indicates the type of the graphics items \n
 * \a uint32_t color:         Base color (contains the item's opacity) \n
 */

#define BASE_STRUCT                                                     \
    struct _ng_event_base_t *event;                                     \
    draw_f draw;                                                        \
    struct _gitem_gestures_t   *gestures;                               \
    uint32_t          flags;                                            \
    int               x;                                                \
    int               y;                                                \
    uint16_t          w;                                                \
    uint16_t          h;                                                \
    int               id;                                               \
    gitem_type_e      type;                                             \
    uint32_t          color

/**
 * @brief Graphics item base struct definition
 *
 */
typedef struct _gitem_base_t
{
    BASE_STRUCT;  /**< Atribbutes as defined in the description of BASE_STRUCT (event pointer, draw function pointer, gestures pointer, flags, x, y, w, h, id, type, color)*/
} __gitem_base_t;

/**
 * @brief Text attributes data struct
 *
 */
typedef struct _attr_text_t
{
    uint8_t        index;   /**< Current index that helps identifying the graphics item's current text and font*/
    nema_font_t  **fonts;   /**< Pointer to an array of font pointer */
    char         **texts;   /**< Pointer to an array of strings */
    uint32_t     alignment; /**< Text alignment (bitwise operator) */
} attr_text_t;

/**
 * @brief Gets the current value of the graphics item (if supported)
 *
 * @param git Pointer to the graphics item that its value should be returned
 * @return float Value of the graphics item. If the item does not support "value", returns 0.f
 */
float ng_gitem_get_value(gitem_base_t *git);

/**
 * @brief Sets a flag of a graphics item
 *
 * @param git Pointer to the graphics item
 * @param flag Flag to be set
 */
void ng_gitem_set_flag(gitem_base_t *git, uint32_t flag);

/**
 * @brief Unsets a graphics item's flag
 *
 * @param git Pointer to the graphics item
 * @param flag Flag to be unset
 */
void ng_gitem_unset_flag(gitem_base_t *git, uint32_t flag);

/**
 * @brief Makes a graphics item visible (if was previously hidden)
 *
 * @param git Pointer to the graphics item
 */
void ng_gitem_set_visible(gitem_base_t *git);

/**
 * @brief Hides a graphics item
 *
 * @param git Pointer to the graphics item
 */
void ng_gitem_set_hidden(gitem_base_t *git);

/**
 * @brief Set the opacity (alpha channel) of a graphics item
 *
 * @param git Pointer to the graphics item
 * @param alpha Opacity value
 */
void ng_gitem_set_alpha(gitem_base_t *git, uint32_t alpha);

/**
 * @brief Sets the color of a graphics item
 *
 * @param git Pointer to the graphics item
 * @param rgba Color value
 */
void ng_gitem_set_color(gitem_base_t *git, uint32_t rgba);

/**
 * @brief Sets the position of a graphics item
 *
 * @param git Pointer to the graphics item
 * @param x Horizontal offset
 * @param y Vertical offset
 */
void ng_gitem_set_position(gitem_base_t *git, int x, int y);

/**
 * @brief Sets the x-position of a graphics item
 *
 * @param git Pointer to the graphics item
 * @param x Horizontal offset
 */
void ng_gitem_set_x(gitem_base_t *git, int x);

/**
 * @brief Sets the y-position of a graphics item
 *
 * @param git Pointer to the graphics item
 * @param y Vertical position
 */
void ng_gitem_set_y(gitem_base_t *git, int y);

/**
 * @brief Sets the size of a graphics item
 *
 * @param git Pointer to the graphics item
 * @param w Width
 * @param h Height
 */
void ng_gitem_set_size(gitem_base_t *git, int w, int h);

/// @cond NODOC
#define _G_TYPE_CIC(ip, gt, ct)       ((ct*) ip)
#define G_TYPE_CHECK_INSTANCE_CAST(instance, g_type, c_type)    (_G_TYPE_CIC ((instance), (g_type), c_type))
/// @endcond
#define NG_GITEM(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), gitem_type_e, gitem_base_t)) /**< Type caster from a derived gitem data struct to the base gitem_base_t*/

#define DRAW_FUNC(NAME) void NAME (gitem_base_t *git, int x_off, int y_off) /**< Draw function definition*/

#define ID(v)             (v) /**< Redability helper */
#define X(v)              (v) /**< Redability helper */
#define Y(v)              (v) /**< Redability helper */
#define W(v)              (v) /**< Redability helper */
#define H(v)              (v) /**< Redability helper */
#define COLOR(v)          (v) /**< Redability helper */
#define EVENT(v)          (v) /**< Redability helper */
#define IMAGE(v)          (v) /**< Redability helper */
#define CUR_VAL(v)        (v) /**< Redability helper */
#define MAX_VAL(v)        (v) /**< Redability helper */
#define MIN_VAL(v)        (v) /**< Redability helper */
#define FLAGS(v)          (v) /**< Redability helper */
#define GESTURES(v)       (v) /**< Redability helper */
#define PEN_WIDTH(v)      (v) /**< Redability helper */
#define TEXT_COLOR(v)     (v) /**< Redability helper */
#define SEC_COLOR(v)      (v) /**< Redability helper */
#define SEC_IMAGE(v)      (v) /**< Redability helper */
#define ANGLE(v)          (v) /**< Redability helper */
#define MAX_ANGLE(v)      (v) /**< Redability helper */
#define MIN_ANGLE(v)      (v) /**< Redability helper */
#define STEP(v)           (v) /**< Redability helper */
#define X_ROT(v)          (v) /**< Redability helper */
#define Y_ROT(v)          (v) /**< Redability helper */
#define NEEDLE(v)         (v) /**< Redability helper */
#define HOUR(v)           (v) /**< Redability helper */
#define MINUTE(v)         (v) /**< Redability helper */
#define SECOND(v)         (v) /**< Redability helper */
#define RADIUS(v)         (v) /**< Redability helper */
#define SUFFIX(v)         (v) /**< Redability helper */
#define PAGE_COUNT(v)     (v) /**< Redability helper */
#define SPACING(v)        (v) /**< Redability helper */
#define CUR_STATE(v)      (v) /**< Redability helper */
#define STATE_COUNT(v)    (v) /**< Redability helper */
#define CUR_PAGE(v)       (v) /**< Redability helper */
#define INT_PRECISION(v)  (v) /**< Redability helper */
#define DEC_PRECISION(v)  (v) /**< Redability helper */
#define TIME_FORMAT(v)    (v) /**< Redability helper */

#endif // NG_GITEM_H__
