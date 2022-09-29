//*****************************************************************************
//
//  ng_draw_prim.h
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

/**
 * @file
 * @brief Primitive drawing (rectangle, circle, rounded rectanle, quadrilateral and image)
 *
 * Drawing functions used commonly used accross several graphics items (by their drawing functions)
 */

#ifndef NG_DRAW_PRIM_H_
#define NG_DRAW_PRIM_H_

#include "nema_sys_defs.h"

/**
 * @brief Fills a rectangular area with a color (that can contain opacity)
 *
 * @param x Aboslute x position inside the framebuffer
 * @param y Aboslute y position inside the framebuffer
 * @param w Rectangle's width
 * @param h Rectangle's height
 * @param color Rectangle's color
 * @param override_blend If zero, the parameters needed for blending (blending mode, const color, texture color) are implicitly calculated whithin this function, otherwise they need to be configured manually before calling this function
 */
void ng_fill_rect(int x, int y, int w, int h, uint32_t color, int override_blend);

/**
 * @brief Draws a rectangle (outline) with a certain color (that can contain opacity) and pen width
 *
 * @param x Aboslute x position inside the framebuffer
 * @param y Aboslute y position inside the framebuffer
 * @param w Rectangle's width
 * @param h Rectangle's height
 * @param color  Rectangle's color
 * @param pen_width Pen width
 * @param override_blend If zero, the parameters needed for blending (blending mode, const color, texture color) are implicitly calculated whithin this function, otherwise they need to be configured manually before calling this function
 */
void ng_draw_primitive_rect(int x, int y, int w, int h, uint32_t color, int pen_width, int override_blend);

/**
 * @brief Draws a rounded rectangle with a certain color (that can contain opacity) and radius
 *
 * @param x Aboslute x position inside the framebuffer
 * @param y Aboslute y position inside the framebuffer
 * @param w Rectangle's width
 * @param h Rectangle's height
 * @param color  Rectangle's color
 * @param radius Radius
 * @param override_blend If zero, the parameters needed for blending (blending mode, const color, texture color) are implicitly calculated whithin this function, otherwise they need to be configured manually before calling this function
 */
void ng_draw_primitive_rounded_rect(int x, int y, int w, int h, uint32_t color, int radius, int override_blend);

/**
 * @brief Fills a rounded rectangle with a certain color (that can contain opacity) and radius
 *
 * @param x Aboslute x position inside the framebuffer
 * @param y Aboslute y position inside the framebuffer
 * @param w Rectangle's width
 * @param h Rectangle's height
 * @param color  Rectangle's color
 * @param radius Radius
 * @param override_blend If zero, the parameters needed for blending (blending mode, const color, texture color) are implicitly calculated whithin this function, otherwise they need to be configured manually before calling this function
 */
void ng_fill_primitive_rounded_rect(int x, int y, int w, int h, uint32_t color, int radius, int override_blend);

/**
 * @brief Draws a circle with a specific color (that contains the opacity) and radius
 *
 * @param x Aboslute x position inside the framebuffer
 * @param y Aboslute y position inside the framebuffer
 * @param r Radius
 * @param color Circle's color
 * @param override_blend  If zero, the parameters needed for blending (blending mode, const color, texture color) are implicitly calculated whithin this function, otherwise they need to be configured manually before calling this function
 */
void ng_draw_primitive_circle(int x, int y, int r, uint32_t color, int override_blend);

/**
 * @brief Fills a circle with a specific color (that can contain opacity) and radius
 *
 * @param x Aboslute x position inside the framebuffer
 * @param y Aboslute y position inside the framebuffer
 * @param r Radius
 * @param color Circle's color
 * @param override_blend  If zero, the parameters needed for blending (blending mode, const color, texture color) are implicitly calculated whithin this function, otherwise they need to be configured manually before calling this function
 */
void ng_fill_primitive_circle(int x, int y, int r, uint32_t color, int override_blend);

/**
 * @brief Blits a source image by fitting it into a rectangular area
 *
 * @param img Pointer to the source image
 * @param x Aboslute x position inside the framebuffer
 * @param y Aboslute y position inside the framebuffer
 * @param w Rectangle's width
 * @param h Rectangle's height
 * @param override_blend If zero, the parameters needed for blending (blending mode, const color, texture color) are implicitly calculated whithin this function, otherwise they need to be configured manually before calling this function
 * @param opacity Opacity [0, 255]
 */
void ng_blit_rect_fit(img_obj_t *img,
                      int x, int y,
                      int w, int h,
                      int override_blend,
                      uint8_t opacity
);

/**
 * @brief Blits a source image by fitting it into a quadrilateral area
 *
 * @param img Pointer to the source image
 * @param x0 x position of the first vertex of the quaqrilateral
 * @param y0 y position of the first vertex of the quaqrilateral
 * @param x1 x position of the second vertex of the quaqrilateral
 * @param y1 y position of the second vertex of the quaqrilateral
 * @param x2 x position of the third vertex of the quaqrilateral
 * @param y2 y position of the third vertex of the quaqrilateral
 * @param x3 x position of the fourth vertex of the quaqrilateral
 * @param y3 y position of the fourth vertex of the quaqrilateral
 * @param override_blend If zero, the parameters needed for blending (blending mode, const color, texture color) are implicitly calculated whithin this function, otherwise they need to be configured manually before calling this function
 * @param opacity Opacity [0, 255]
 */
void ng_blit_quad_fit(img_obj_t *img,
                      float x0, float y0,
                      float x1, float y1,
                      float x2, float y2,
                      float x3, float y3,
                      int override_blend,
                      uint8_t opacity
);

/**
 * @brief Fills a quadrilateral area with a cirtain color (that can contain opacity)
 *
 * @param x0 x position of the first vertex of the quaqrilateral
 * @param y0 y position of the first vertex of the quaqrilateral
 * @param x1 x position of the second vertex of the quaqrilateral
 * @param y1 y position of the second vertex of the quaqrilateral
 * @param x2 x position of the third vertex of the quaqrilateral
 * @param y2 y position of the third vertex of the quaqrilateral
 * @param x3 x position of the fourth vertex of the quaqrilateral
 * @param y3 y position of the fourth vertex of the quaqrilateral
 * @param override_blend If zero, the parameters needed for blending (blending mode, const color, texture color) are implicitly calculated whithin this function, otherwise they need to be configured manually before calling this function
 * @param color Color (contains opacity information)
 */
void ng_fill_quad( float x0, float y0,
                   float x1, float y1,
                   float x2, float y2,
                   float x3, float y3,
                   int override_blend,
                   uint32_t color
    );

#endif //NG_DRAW_PRIM_H_
