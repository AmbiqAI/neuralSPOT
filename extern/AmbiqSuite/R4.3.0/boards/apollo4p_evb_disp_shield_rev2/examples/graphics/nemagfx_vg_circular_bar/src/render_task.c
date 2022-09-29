//*****************************************************************************
//
//! @file render_task.c
//!
//! @brief Task to handle GPU render operations.
//!
//!
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

//*****************************************************************************
//
// Global includes for this project.
//
//*****************************************************************************
#include "nemagfx_vg_circular_bar.h"

#include "graphics_power_control.h"
#include "resource/DejaVuSans_ttf_kern.h"

#include "resource/power_tsvg.h"
#include "resource/step_tsvg.h"
#include "resource/calorie_tsvg.h"
//*****************************************************************************
//
// Macro definitions
//
//*****************************************************************************
#define INNER_RADIUS_CALORIE (140)
#define OUTER_RADIUS_CALORIE (190)
#define COLOR_START_CALORIE  (30.f / 360.f)
#define COLOR_END_CALORIE    (75.f / 360.f)
#define BG_LIGHT_CALORIE     (0.12f)
#define RANGE_CALORIE        (300)

#define INNER_RADIUS_STEP (80)
#define OUTER_RADIUS_STEP (130)
#define COLOR_START_STEP  (90.f / 360.f)
#define COLOR_END_STEP    (140.f / 360.f)
#define BG_LIGHT_STEP     (0.12f)
#define RANGE_STEP        (12000)

#define INNER_RADIUS_POWER (20)
#define OUTER_RADIUS_POWER (70)
#define COLOR_START_POWER  (165.f / 360.f)
#define COLOR_END_POWER    (205.f / 360.f)
#define BG_LIGHT_POWER     (0.12f)
#define RANGE_POWER        (100)

#define ICON_SIZE     (35)
#define FONT_SIZE     (36)
#define FONT_Y_OFFSET (25)
#define FONT_GAP      (26)
#define FONT_BAR_GAP  (15)

#define FONT_SIZE_TIME    (64)
#define TIME_X_POS        (FB_RESX / 2 + 35)
#define TIME_X_GAP        (55)
#define TIME_HOUR_Y_POS   (15)
#define TIME_MINUTE_Y_POS (125)

//#define USE_ARC_PATH
#define USE_CONIC_PAINT

//*****************************************************************************
//
// Render task handle.
//
//*****************************************************************************
TaskHandle_t RenderTaskHandle;

//*****************************************************************************
//
// Color format.
//
//*****************************************************************************
typedef struct rgb
{
    float r, g, b;
} RGB;

typedef struct hsl
{
    float h, s, l;
} HSL;

typedef struct _type_circular_
{
    uint32_t             inner_radius;
    uint32_t             outer_radius;
    float                color_start;
    float                color_end;
    float                light_bg;
    float                angle;
    NEMA_VG_PAINT_HANDLE paint_low_half;
    NEMA_VG_PATH_HANDLE  path_low_half;
    NEMA_VG_PAINT_HANDLE paint_high_half;
    NEMA_VG_PATH_HANDLE  path_high_half;
    NEMA_VG_PAINT_HANDLE paint_bg;
    NEMA_VG_PATH_HANDLE  path_bg;
    NEMA_VG_PAINT_HANDLE paint_font;
    NEMA_VG_PATH_HANDLE  path_font;
    NEMA_VG_GRAD_HANDLE  gradient;
    void *               icon;
    uint32_t             value;
    uint32_t             range;
    char                 unit;
} circular_t;

NEMA_VG_PAINT_HANDLE paint_time;
NEMA_VG_PATH_HANDLE  path_time;

//*****************************************************************************
//
// Global Variables
//
//*****************************************************************************
circular_t annular_calorie = {
    .inner_radius = INNER_RADIUS_CALORIE,
    .outer_radius = OUTER_RADIUS_CALORIE,
    .color_start  = COLOR_START_CALORIE,
    .color_end    = COLOR_END_CALORIE,
    .light_bg     = BG_LIGHT_CALORIE,
    .angle        = 0,
    .range        = RANGE_CALORIE,
    .unit         = 0,
    .icon         = calorie_tsvg,
};

circular_t annular_step = {
    .inner_radius = INNER_RADIUS_STEP,
    .outer_radius = OUTER_RADIUS_STEP,
    .color_start  = COLOR_START_STEP,
    .color_end    = COLOR_END_STEP,
    .light_bg     = BG_LIGHT_STEP,
    .angle        = 0,
    .range        = RANGE_STEP,
    .unit         = 0,
    .icon         = step_tsvg,
};

circular_t annular_power = {
    .inner_radius = INNER_RADIUS_POWER,
    .outer_radius = OUTER_RADIUS_POWER,
    .color_start  = COLOR_START_POWER,
    .color_end    = COLOR_END_POWER,
    .light_bg     = BG_LIGHT_POWER,
    .angle        = 0,
    .range        = RANGE_POWER,
    .unit         = '%',
    .icon         = power_tsvg,
};

//*****************************************************************************
//
// Convert HUE format to RGB format
// Origin: https://gist.github.com/ciembor/1494530
// By: Maciej Ciemborowicz
//
//*****************************************************************************
/*
 * Converts an HUE to r, g or b.
 * returns float in the set [0, 1].
 */
float hue2rgb(float p, float q, float t)
{

    if (t < 0)
    {
        t += 1;
    }
    if (t > 1)
    {
        t -= 1;
    }
    if (t < 1. / 6)
    {
        return p + (q - p) * 6 * t;
    }
    if (t < 1. / 2)
    {
        return q;
    }
    if (t < 2. / 3)
    {
        return p + (q - p) * (2. / 3 - t) * 6;
    }

    return p;
}

//*****************************************************************************
//
// Convert HSL format to RGB format
// Origin: https://gist.github.com/ciembor/1494530
// By: Maciej Ciemborowicz
//
//*****************************************************************************
/*
 * Converts an HSL color value to RGB. Conversion formula
 * adapted from http://en.wikipedia.org/wiki/HSL_color_space.
 * Assumes h, s, and l are contained in the set [0, 1] and
 * returns RGB in the set [0, 255].
 */
RGB hsl2rgb(float h, float s, float l)
{

    RGB result;

    if (0 == s)
    {
        result.r = result.g = result.b = l; // achromatic
    }
    else
    {
        float q  = l < 0.5 ? l * (1 + s) : l + s - l * s;
        float p  = 2 * l - q;
        result.r = hue2rgb(p, q, h + 1. / 3) * 255;
        result.g = hue2rgb(p, q, h) * 255;
        result.b = hue2rgb(p, q, h - 1. / 3) * 255;
    }

    return result;
}

// use conic paint to fill the path
void vg_draw_annular(circular_t *ptr_annular)
{
    HSL hsl_color_start =
    {
        .h = ptr_annular->color_start,
        .s = 0.5f,
        .l = 0.5f,
    };

    HSL hsl_color_end =
    {
        .h = ptr_annular->color_end,
        .s = 0.5f,
        .l = 0.5f,
    };

    HSL hsl_color_bg =
    {
        .h = (ptr_annular->color_start + ptr_annular->color_end) / 2,
        .s = 0.5f,
        .l = ptr_annular->light_bg,
    };

    RGB rgb_color_start =
        hsl2rgb(hsl_color_start.h, hsl_color_start.s, hsl_color_start.l);
    RGB rgb_color_end =
        hsl2rgb(hsl_color_end.h, hsl_color_end.s, hsl_color_end.l);
    RGB rgb_color_bg = hsl2rgb(hsl_color_bg.h, hsl_color_bg.s, hsl_color_bg.l);

    float       stops[4]        = {0.0f, 0.25f, 0.75f, 1.0f};
    color_var_t colors_front[4] =
    {
        {rgb_color_start.r, rgb_color_start.g, rgb_color_start.b, 255},
        {rgb_color_start.r, rgb_color_start.g, rgb_color_start.b, 255},
        {rgb_color_end.r, rgb_color_end.g, rgb_color_end.b, 255},
        {rgb_color_end.r, rgb_color_end.g, rgb_color_end.b, 255},
    };
    nema_vg_grad_set(ptr_annular->gradient, 4, stops, colors_front);

    // set background paint, const color
    nema_vg_paint_set_type(ptr_annular->paint_bg, NEMA_VG_PAINT_FILL);
    nema_vg_paint_set_paint_color(
        ptr_annular->paint_bg,
        nema_rgba((uint8_t)rgb_color_bg.r, (uint8_t)rgb_color_bg.g,
        (uint8_t)rgb_color_bg.b, 0xff));

    // set width
    nema_vg_paint_set_stroke_width(
        ptr_annular->paint_bg,
        ptr_annular->outer_radius - ptr_annular->inner_radius);

    // draw ring
    nema_vg_draw_ring(FB_RESX / 2, FB_RESY / 2,
                      (ptr_annular->inner_radius + ptr_annular->outer_radius) /
                          2,
                      90 + ptr_annular->angle, 270, ptr_annular->paint_bg);

    // set front color
    nema_vg_paint_set_type(ptr_annular->paint_low_half,
                           NEMA_VG_PAINT_GRAD_CONICAL);
    nema_vg_paint_set_grad_conical(ptr_annular->paint_low_half,
                                   ptr_annular->gradient,
                                   FB_RESX / 2,
                                   FB_RESY / 2,
                                   NEMA_TEX_CLAMP | NEMA_FILTER_BL);

    // set width
    nema_vg_paint_set_stroke_width(
        ptr_annular->paint_low_half,
        ptr_annular->outer_radius - ptr_annular->inner_radius);

    // draw ring
    nema_vg_draw_ring(FB_RESX / 2,
                      FB_RESY / 2,
                      (ptr_annular->inner_radius + ptr_annular->outer_radius) /
                          2,
                      90,
                      90 + ptr_annular->angle,
                      ptr_annular->paint_low_half);

    uint32_t svg_width;
    uint32_t svg_hight;
    nema_vg_get_tsvg_resolution(ptr_annular->icon, &svg_width, &svg_hight);
    uint32_t svg_size = (svg_width > svg_hight) ? svg_width : svg_hight;

    //am_util_stdio_printf("width:%d,hight:%d,size:%d\n", svg_width, svg_hight, svg_size);

    // draw SVG
    nema_matrix3x3_t m_path;

    float scale   = ICON_SIZE / (float)svg_size;
    float shift_x = -ICON_SIZE / 2.0f + 0 + FB_RESX / 2;
    float shift_y =
        -ICON_SIZE / 2.0f +
        (ptr_annular->inner_radius + ptr_annular->outer_radius) / 2.0f +
        FB_RESY / 2.0f;

    nema_mat3x3_load_identity(m_path);
    nema_mat3x3_scale(m_path, scale, scale);
    nema_mat3x3_translate(m_path, shift_x, shift_y);

    nema_vg_set_global_matrix(m_path);

    // Draw the SVG
    nema_vg_draw_tsvg(ptr_annular->icon);

    nema_vg_reset_global_matrix();

    // Draw TTF
    char value_str[8];
    uint32_t color = nema_rgba((uint8_t)rgb_color_start.r, (uint8_t)rgb_color_start.g,
                               (uint8_t)rgb_color_start.b, 0xff);
    uint32_t char_cnt =
        am_util_stdio_snprintf(value_str, 6, "%d", ptr_annular->value);

    value_str[char_cnt++] = ptr_annular->unit;
    value_str[char_cnt++] = 0;

    // Set font color
    nema_vg_paint_set_type(ptr_annular->paint_font, NEMA_VG_PAINT_FILL);
    nema_vg_paint_set_paint_color(ptr_annular->paint_font, color);

    // Set font size
    nema_vg_set_font_size(FONT_SIZE);

    uint32_t y_off = (ptr_annular->inner_radius + ptr_annular->outer_radius) / 2 +
            FB_RESY / 2 - FONT_Y_OFFSET;
    uint32_t x_off = (ptr_annular->outer_radius - ptr_annular->inner_radius) / 2 +
            FB_RESX / 2 + FONT_BAR_GAP;
    nema_vg_print(ptr_annular->paint_font, value_str, x_off, y_off,
                  100, 100,
                  NEMA_VG_ALIGNX_LEFT | NEMA_VG_TEXT_WRAP | NEMA_VG_ALIGNY_TOP,
                  NULL);
}

void draw_time(uint32_t hour, uint32_t minute)
{
    uint32_t color = 0xffffffff;
    char     value_str[6];

    hour = (hour > 24) ? 24 : hour;
    minute = (minute > 60) ? 60 : minute;

    uint32_t char_cnt =
        am_util_stdio_snprintf(value_str, 6, "%02d\n%02d", hour, minute);

    value_str[char_cnt++] = 0;

    nema_vg_paint_set_type(paint_time, NEMA_VG_PAINT_FILL);
    nema_vg_paint_set_paint_color(paint_time, color);

    nema_vg_set_font_size(FONT_SIZE_TIME);

    nema_vg_print(paint_time, value_str, TIME_X_POS, TIME_HOUR_Y_POS,
                 (FB_RESX - TIME_X_POS), (FB_RESY / 2 - TIME_HOUR_Y_POS),
                 NEMA_VG_ALIGNX_LEFT | NEMA_VG_TEXT_WRAP | NEMA_VG_ALIGNY_TOP,
                 NULL);
}

void vg_health_status(img_obj_t *des_img)
{
    // Bind framebuffer
    nema_bind_dst_tex(des_img->bo.base_phys, des_img->w, des_img->h,
                      des_img->format, des_img->stride);

    // Set clip
    nema_set_clip(0, 0, des_img->w, des_img->h);

    // Clear
    nema_clear(0x00000000);

    nema_vg_set_blend(NEMA_BL_SRC_OVER);

    nema_vg_set_fill_rule(NEMA_VG_FILL_EVEN_ODD);

    annular_calorie.angle =
        ((float)annular_calorie.value / annular_calorie.range) * 180.f;
    annular_step.angle =
        ((float)annular_step.value / annular_step.range) * 180.f;
    annular_power.angle =
        ((float)annular_power.value / annular_power.range) * 180.f;
    ;

    if (annular_calorie.angle > 180.f)
    {
        annular_calorie.angle = 180.f;
    }

    if (annular_step.angle > 180.f)
    {
        annular_step.angle = 180.f;
    }

    if (annular_power.angle > 180.f)
    {
        annular_power.angle = 180.f;
    }

    vg_draw_annular(&annular_calorie);
    vg_draw_annular(&annular_step);
    vg_draw_annular(&annular_power);

    draw_time(8, 16);
}

//*****************************************************************************
//
// Short Description.
//
//*****************************************************************************
void RenderTask(void *pvParameters)
{
#ifdef GPU_WORK_TIME_PRINT
    uint32_t time_start;
    uint32_t time_end;
#endif

    am_util_stdio_printf("Render task start!\n");

    // Bond font
    nema_vg_bind_font(&DejaVuSans_ttf_kern);

    // Create GPU command list
    nema_cmdlist_t cl = nema_cl_create_sized(16 * 1024);
    if ( cl.bo.base_virt == NULL )
    {
        am_util_stdio_printf("CL create failed!\n");

        // suspend and delete this task.
        vTaskDelete(NULL);
    }

    //bind cl
    nema_cl_bind_circular(&cl);

    // Alloc buffer for path and paint
    annular_calorie.paint_low_half  = nema_vg_paint_create();
    annular_calorie.paint_high_half = nema_vg_paint_create();
    annular_calorie.paint_bg        = nema_vg_paint_create();
    annular_calorie.paint_font      = nema_vg_paint_create();
    annular_calorie.path_low_half   = nema_vg_path_create();
    annular_calorie.path_high_half  = nema_vg_path_create();
    annular_calorie.path_bg         = nema_vg_path_create();
    annular_calorie.path_font       = nema_vg_path_create();
    annular_calorie.gradient        = nema_vg_grad_create();

    annular_step.paint_low_half  = nema_vg_paint_create();
    annular_step.paint_high_half = nema_vg_paint_create();
    annular_step.paint_bg        = nema_vg_paint_create();
    annular_step.paint_font      = nema_vg_paint_create();
    annular_step.path_low_half   = nema_vg_path_create();
    annular_step.path_high_half  = nema_vg_path_create();
    annular_step.path_bg         = nema_vg_path_create();
    annular_step.path_font       = nema_vg_path_create();
    annular_step.gradient        = nema_vg_grad_create();

    annular_power.paint_low_half  = nema_vg_paint_create();
    annular_power.paint_high_half = nema_vg_paint_create();
    annular_power.paint_bg        = nema_vg_paint_create();
    annular_power.paint_font      = nema_vg_paint_create();
    annular_power.path_low_half   = nema_vg_path_create();
    annular_power.path_high_half  = nema_vg_path_create();
    annular_power.path_bg         = nema_vg_path_create();
    annular_power.path_font       = nema_vg_path_create();
    annular_power.gradient        = nema_vg_grad_create();

    annular_calorie.value = 0;
    annular_step.value    = 0;
    annular_power.value   = 0;

    path_time  = nema_vg_path_create();
    paint_time = nema_vg_paint_create();

    float    percent = 0.f;
    uint32_t count   = 0;

    while (1)
    {
        // Wait start.
        xSemaphoreTake(g_semGPUStart, portMAX_DELAY);

#ifdef USE_DEBUG_PIN_IN_GUI_TASK
        am_hal_gpio_output_clear(DEBUG_PIN_3);
        am_hal_gpio_output_clear(DEBUG_PIN_6);
#endif

        // This API will directly return if GPU is still power on.
        // gpu_power_up();

        // rewind the CL
        nema_cl_rewind(&cl);

#if 1
        if ( count < 100 )
        {
            count ++;
            percent = count * 0.01f;
        }
        else if ( (count >= 100) && (count < 200) )
        {
            count ++;
            percent = 1.0f;
        }
        else
        {
            count = 0;
            percent = 0.f;
        }

        annular_calorie.value = (int)(annular_calorie.range*percent);
        annular_step.value = (int)(annular_step.range*percent);
        annular_power.value = (int)(annular_power.range*percent);
#else
        annular_calorie.value = 234;
        annular_step.value    = 10000;
        annular_power.value   = 80;
#endif

#ifdef GPU_WORK_TIME_PRINT
        time_start = xTaskGetTickCount();
#endif

        vg_health_status(g_pFrameBufferGPU);

        if ( nema_vg_get_error() != NEMA_VG_ERR_NO_ERROR )
        {
            am_util_stdio_printf("VG error!\n");
            break;
        }

        // start GPU, submit CL
        nema_cl_submit(&cl);
#ifdef USE_DEBUG_PIN_IN_GUI_TASK
        am_hal_gpio_output_set(DEBUG_PIN_1);
#endif

        // Wait GPU
        nema_cl_wait(&cl);

#ifdef GPU_WORK_TIME_PRINT
        time_end = xTaskGetTickCount();

        am_util_stdio_printf("%d\n", time_end - time_start);
#endif

        nema_vg_paint_clear(annular_calorie.paint_low_half);
        nema_vg_paint_clear(annular_calorie.paint_high_half);
        nema_vg_paint_clear(annular_calorie.paint_bg);
        nema_vg_paint_clear(annular_calorie.paint_font);
        nema_vg_path_clear(annular_calorie.path_low_half);
        nema_vg_path_clear(annular_calorie.path_high_half);
        nema_vg_path_clear(annular_calorie.path_bg);
        nema_vg_path_clear(annular_calorie.path_font);

        nema_vg_paint_clear(annular_step.paint_low_half);
        nema_vg_paint_clear(annular_step.paint_high_half);
        nema_vg_paint_clear(annular_step.paint_bg);
        nema_vg_paint_clear(annular_step.paint_font);
        nema_vg_path_clear(annular_step.path_low_half);
        nema_vg_path_clear(annular_step.path_high_half);
        nema_vg_path_clear(annular_step.path_bg);
        nema_vg_path_clear(annular_step.path_font);

        nema_vg_paint_clear(annular_power.paint_low_half);
        nema_vg_paint_clear(annular_power.paint_high_half);
        nema_vg_paint_clear(annular_power.paint_bg);
        nema_vg_paint_clear(annular_power.paint_font);
        nema_vg_path_clear(annular_power.path_low_half);
        nema_vg_path_clear(annular_power.path_high_half);
        nema_vg_path_clear(annular_power.path_bg);
        nema_vg_path_clear(annular_power.path_font);

        nema_vg_path_clear(path_time);
        nema_vg_paint_clear(paint_time);

#ifdef USE_DEBUG_PIN_IN_GUI_TASK
        am_hal_gpio_output_clear(DEBUG_PIN_1);
#endif

        // Notify the GUI task.
        xSemaphoreGive(g_semGPUEnd);

        // Power Down
        // gpu_power_down();

#ifdef USE_DEBUG_PIN_IN_GUI_TASK
        am_hal_gpio_output_set(DEBUG_PIN_3);
#endif
    }

    nema_vg_paint_destroy(annular_calorie.paint_low_half);
    nema_vg_paint_destroy(annular_calorie.paint_high_half);
    nema_vg_paint_destroy(annular_calorie.paint_bg);
    nema_vg_paint_destroy(annular_calorie.paint_font);
    nema_vg_path_destroy(annular_calorie.path_low_half);
    nema_vg_path_destroy(annular_calorie.path_high_half);
    nema_vg_path_destroy(annular_calorie.path_bg);
    nema_vg_path_destroy(annular_calorie.path_font);
    nema_vg_grad_destroy(annular_calorie.gradient);

    nema_vg_paint_destroy(annular_step.paint_low_half);
    nema_vg_paint_destroy(annular_step.paint_high_half);
    nema_vg_paint_destroy(annular_step.paint_bg);
    nema_vg_paint_destroy(annular_step.paint_font);
    nema_vg_path_destroy(annular_step.path_low_half);
    nema_vg_path_destroy(annular_step.path_high_half);
    nema_vg_path_destroy(annular_step.path_bg);
    nema_vg_path_destroy(annular_step.path_font);
    nema_vg_grad_destroy(annular_step.gradient);

    nema_vg_paint_destroy(annular_power.paint_low_half);
    nema_vg_paint_destroy(annular_power.paint_high_half);
    nema_vg_paint_destroy(annular_power.paint_bg);
    nema_vg_paint_destroy(annular_power.paint_font);
    nema_vg_path_destroy(annular_power.path_low_half);
    nema_vg_path_destroy(annular_power.path_high_half);
    nema_vg_path_destroy(annular_power.path_bg);
    nema_vg_path_destroy(annular_power.path_font);
    nema_vg_grad_destroy(annular_power.gradient);

    nema_vg_path_destroy(path_time);
    nema_vg_paint_destroy(paint_time);

    nema_vg_deinit();
}
