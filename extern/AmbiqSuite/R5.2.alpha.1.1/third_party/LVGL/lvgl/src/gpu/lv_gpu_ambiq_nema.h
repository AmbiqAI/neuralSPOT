/**
 * @file lv_gpu_ambiq_nema.h
 *
 */

//*****************************************************************************
//
// Copyright (c) 2021, Ambiq Micro, Inc.
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
//*****************************************************************************

#ifndef LV_SRC_LV_GPU_LV_GPU_AMBIQ_NEMA_H_
#define LV_SRC_LV_GPU_LV_GPU_AMBIQ_NEMA_H_

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lvgl.h"


/*********************
 *      DEFINES
 *********************/



/**********************
 *      TYPEDEFS
 **********************/
typedef union
{
    uint32_t    u32;
    struct
    {
        uint32_t    Revision    : 8;    // [7:0]
        uint32_t    Minor       : 8;    // [15:8]
        uint32_t    Major       : 8;    // [23:16]
        uint32_t    resvd       : 8;    // [31:24]
    } s;
} lv_gpu_ambiq_version_t;

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
* Reset and initialize NEMA_GFX device. This function should be called as a part
* of display init sequence.
*
* @return LV_RES_OK: NEMA_GFX init ok; LV_RES_INV: init error. See error log for more information.
*/
lv_res_t lv_gpu_ambiq_nema_init(void);

/**
* Disable NEMA_GFX device. Should be called during display deinit sequence.
*/
void lv_gpu_ambiq_nema_deinit(void);

/**
 * Can be used as `gpu_wait_cb` in display driver to
 * let the MCU run while the GPU is working
 */
void lv_gpu_ambiq_nema_wait(lv_disp_drv_t * disp_drv);

/**
 * Fill an area with a color
 * @param disp_area the current display area (destination area)
 * @param disp_buf destination buffer
 * @param draw_area fill this area (relative to `disp_area`)
 * @param color fill color
 * @param opa overall opacity in 0x00..0xff range
 * @param mask a mask to apply on every pixel (uint8_t array with 0x00..0xff values).
 *                It fits into draw_area.
 * @param mask_res LV_MASK_RES_COVER: the mask has only 0xff values (no mask),
 *                 LV_MASK_RES_TRANSP: the mask has only 0x00 values (full transparent),
 *                 LV_MASK_RES_CHANGED: the mask has mixed values
 * @return LV_RES_OK, if we have drawn this object by GPU. LV_RES_INV, not supported.
 */
lv_res_t lv_gpu_ambiq_nema_fill(const lv_area_t * disp_area, lv_color_t * disp_buf,
                            const lv_area_t * draw_area,
                            lv_color_t color, lv_opa_t opa,
                            const lv_opa_t * mask, lv_draw_mask_res_t mask_res);

/**
 * Copy an image to an area
 * @param disp_area the current display area (destination area)
 * @param disp_buf destination buffer
 * @param map_area coordinates of the map (image) to copy. (absolute coordinates)
 * @param map_buf the pixel of the image
 * @param opa overall opacity in 0x00..0xff range
 * @param mask a mask to apply on every pixel (uint8_t array with 0x00..0xff values).
 *                It fits into draw_area.
 * @param mask_res LV_MASK_RES_COVER: the mask has only 0xff values (no mask),
 *                 LV_MASK_RES_TRANSP: the mask has only 0x00 values (full transparent),
 *                 LV_MASK_RES_CHANGED: the mask has mixed values
 * @return LV_RES_OK, if we have drawn this object by GPU. LV_RES_INV, not supported.
 */
lv_res_t lv_gpu_ambiq_nema_map(const lv_area_t * disp_area, lv_color_t * disp_buf,
                           const lv_area_t * draw_area,
                           const lv_area_t * map_area, const lv_color_t * map_buf, lv_opa_t opa,
                           const lv_opa_t * mask, lv_draw_mask_res_t mask_res);

/**
 * Blit an image
 * @param coords the coordinates of the image
 * @param clip_area the image will be drawn only in this area
 * @param src pointer to a lv_color_t array which contains the pixels of the image
 * @param dsc pointer to an initialized `lv_draw_img_dsc_t` variable
 * @return LV_RES_OK, if we have drawn this object by GPU. LV_RES_INV, not supported.
 */
lv_res_t lv_gpu_ambiq_nema_blit(const lv_area_t * coords, 
                            const lv_area_t * clip_area,
                            const void * src,
                            const lv_draw_img_dsc_t * dsc);

/**
 * Draw rectangle background color
 * @param clip_area the image will be drawn only in this area
 * @param draw_area the area to be draw, rectangle background area.
 * @param rout the radias of the rounded corner.
 * @param color the color of the background.
 * @param opa the background color opacity.
 * @param blend_mode only normal blend mode supported.
 * @return LV_RES_OK, if we have drawn this object by GPU. LV_RES_INV, not supported.
 */
lv_res_t lv_gpu_ambiq_nema_draw_bg(const lv_area_t * clip_area, const lv_area_t * draw_area, 
                         lv_coord_t rout, lv_color_t color, lv_opa_t opa, lv_blend_mode_t blend_mode);

/**
 * Draw rectangle background image
 * @param coords the coordinates of the rectangle
 * @param mask the rectangle will be drawn only in this mask
 * @param dsc_rect pointer to an initialized `lv_draw_rect_dsc_t` variable.
 * @param dsc_img  pointer to an initialized `lv_draw_img_dsc_t` variable.
 * @return LV_RES_OK, if we have drawn this object by GPU. LV_RES_INV, not supported.
 */
lv_res_t lv_gpu_ambiq_nema_draw_bg_img(const lv_area_t * coords, const lv_area_t * mask,
                                       const lv_draw_rect_dsc_t * dsc_rect, 
                                       const lv_draw_img_dsc_t * dsc_img);

/**
 * Do the preparation operations required by GPU
 * @param clip_area the image will be drawn only in this area
 * @param blend_mode only normal blend mode supported.
 * @return LV_RES_OK, if we have drawn this object by GPU. LV_RES_INV, not supported.
 * Note: This API should be used in pair with lv_gpu_ambiq_nema_draw_label_post
 */
lv_res_t lv_gpu_ambiq_nema_draw_label_pre(const lv_area_t * clip_area, lv_blend_mode_t blend_mode);

/**
 * Do the close operations required by GPU
 * Note: This API should be used in pair with lv_gpu_ambiq_nema_draw_label_pre
 */
void lv_gpu_ambiq_nema_draw_label_post(void);

/**
 * Do the preparation operations required by GPU
 * @param clip_area the image will be drawn only in this area
 * @param blend_mode only normal blend mode supported.
 * @return LV_RES_OK, if we have drawn this object by GPU. LV_RES_INV, not supported.
 * Note: This API should be used in pair with lv_gpu_ambiq_nema_draw_letter_post
 */
lv_res_t lv_gpu_ambiq_nema_draw_letter_pre(const lv_area_t * clip_area, lv_blend_mode_t blend_mode);

/**
 * Do the close operations required by GPU
 * Note: This API should be used in pair with lv_gpu_ambiq_nema_draw_letter_pre
 */
void lv_gpu_ambiq_nema_draw_letter_post(void);
/**
 * Draw letter
 * @param pos_x position on x axis
 * @param pos_y position on y axis
 * @param g glyph
 * @param clip_area the image will be drawn only in this area
 * @param map_p glyph bitmap
 * @param color the color of the border.
 * @param opa the border color opacity.
 * @param blend_mode only normal blend mode supported.
 * @return LV_RES_OK, if we have drawn this object by GPU. LV_RES_INV, not supported.
 */
lv_res_t lv_gpu_ambiq_nema_draw_letter(lv_coord_t pos_x, lv_coord_t pos_y, lv_font_glyph_dsc_t * g,
                                             const lv_area_t * clip_area,
                                             const uint8_t * map_p, lv_color_t color, lv_opa_t opa, lv_blend_mode_t blend_mode);

/**
 * Draw rounded rectangle border
 * @param clip_area the image will be drawn only in this area
 * @param outer_area the outer area of the boarder.
 * @param inner_area the inner_area of the boarder.
 * @param rout the corner radias of the outer area.
 * @param rin the corner radias of the inner area.
 * @param dsc pointer to an initialized `lv_draw_rect_dsc_t` variable.
 * @return LV_RES_OK, if we have drawn this object by GPU. LV_RES_INV, not supported.
 */
lv_res_t lv_gpu_ambiq_nema_draw_border(const lv_area_t * clip_area, const lv_area_t * outer_area, const lv_area_t * inner_area,
                         lv_coord_t rout, lv_coord_t rin, const lv_draw_rect_dsc_t * dsc);

/**
 * Draw rounded rectangle outline
 * @param clip_area the image will be drawn only in this area
 * @param outer_area the outer area of the boarder.
 * @param inner_area the inner_area of the boarder.
 * @param rout the corner radias of the outer area.
 * @param rin the corner radias of the inner area.
 * @param dsc pointer to an initialized `lv_draw_rect_dsc_t` variable.
 * @return LV_RES_OK, if we have drawn this object by GPU. LV_RES_INV, not supported.
 */
lv_res_t lv_gpu_ambiq_nema_draw_outline(const lv_area_t * clip_area, const lv_area_t * outer_area, const lv_area_t * inner_area,
                         lv_coord_t rout, lv_coord_t rin, const lv_draw_rect_dsc_t * dsc);

/**
 * Draw a line
 * @param point1 first point of the line
 * @param point2 second point of the line
 * @param clip the line will be drawn only in this area
 * @param dsc pointer to an initialized `lv_draw_line_dsc_t` variable
 * @return LV_RES_OK, if we have drawn this object by GPU. LV_RES_INV, not supported.
 */
lv_res_t lv_gpu_ambiq_nema_draw_line(const lv_point_t * point1, 
                                    const lv_point_t * point2, 
                                    const lv_area_t * clip,
                                    const lv_draw_line_dsc_t * dsc);

/**
 * Draw a polygon. Only convex polygons are supported
 * @param points an array of points
 * @param point_cnt number of points
 * @param clip_area polygon will be drawn only in this area
 * @param draw_dsc pointer to an initialized `lv_draw_rect_dsc_t` variable
 * @return LV_RES_OK, if we have drawn this object by GPU. LV_RES_INV, not supported.
 */
lv_res_t lv_gpu_ambiq_nema_draw_polygon(const lv_point_t points[],
                                    uint16_t point_cnt,
                                    const lv_area_t * clip_area,
                                    const lv_draw_rect_dsc_t * draw_dsc);

/**
 * Draw arc.
 * @param center_x arc center position on x axis
 * @param center_y arc center position on y axis
 * @param radius arc radius
 * @param width arc width
 * @param start_angle arc start angle
 * @param end_angle arc end angle
 * @return LV_RES_OK, if we have drawn this object by GPU. LV_RES_INV, not supported.
 */
lv_res_t lv_gpu_ambiq_nema_draw_arc(lv_coord_t center_x,
                                    lv_coord_t center_y,
                                    uint16_t radius,
                                    uint16_t start_angle,
                                    uint16_t end_angle,
                                    const lv_area_t * clip_area,
                                    const lv_draw_arc_dsc_t * dsc);

/**
 * Judge if the draw buffer format is only supported by GPU, CPU can't draw this buffer.
 * @param disp display info in the lv_disp_t
 * @return LV_RES_OK, only supported by GPU. LV_RES_INV, supported by CPU and GPU.
 */
lv_res_t lv_gpu_ambiq_nema_gpu_only_fb(lv_disp_t* disp);

/**
 * Set the frame buffer's description structure.
 * @param fb_s the structure to be filled.
 * @param cf this buffer's color format.
 * @param w buffer's width.
 * @param h buffer's hight.
 * @param buf buffer's pointer.
 * @param buff_size buffer's size.
 * @return LV_RES_OK, the color format is supported. LV_RES_INV, not supported format.
 */
lv_res_t lv_gpu_ambiq_nema_fb_set(void* fb_s, lv_img_cf_t cf, lv_coord_t w, lv_coord_t h,
                                  void* buf, uint32_t buff_size);

/**
 * Clear the frame buffer.
 * @param fb the fb to be cleared.
 * @return LV_RES_OK, the clear operation is OK. LV_RES_INV, failed.
 */
lv_res_t lv_gpu_ambiq_nema_fb_clear(void* fb);

/**
 * Get the needed memory size.
 * @param cf this buffer's color format.
 * @param w buffer's width.
 * @param h buffer's hight.
 * @return needed size in byte.
 */
uint32_t lv_gpu_ambiq_nema_fb_size_needed(lv_img_cf_t cf, lv_coord_t w, lv_coord_t h);

/**
 * Check if the color format can be used as frame buffer's format.
 * @param lvgl_cf this buffer's color format.
 * @return LV_RES_OK, can be used as a destination buffer. LV_RES_INV, cannot be used.
 */
lv_res_t lv_gpu_ambiq_nema_fb_support_format(lv_img_cf_t lvgl_cf);

/**********************
 *   STATIC FUNCTIONS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SRC_LV_GPU_LV_GPU_AMBIQ_NEMA_H_*/
