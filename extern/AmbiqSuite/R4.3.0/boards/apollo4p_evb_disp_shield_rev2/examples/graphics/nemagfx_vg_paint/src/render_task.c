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
#include "nemagfx_vg_paint.h"

#include "graphics_power_control.h"
#include "texture/paint.h"

//*****************************************************************************
//
// Macro definitions
//
//*****************************************************************************

#define LINEAR_GRADIENT_START_X (0)
#define LINEAR_GRADIENT_START_Y (120)

#define ONE_STAR_START_X        (150)
#define ONE_STAR_START_Y        (180)

#define STAR_WIDTH              (80)
#define STAR_HIGHT              (100)
#define STAR_X_OFFSET           (10)
#define STAR_Y_OFFSET           (10)

//*****************************************************************************
//
// Render task handle.
//
//*****************************************************************************
TaskHandle_t RenderTaskHandle;

//*****************************************************************************
//
// Function to show different paint.
//
//*****************************************************************************
typedef void (*paint_show_func_t)(img_obj_t *des_img, NEMA_VG_PATH_HANDLE path,
                                  NEMA_VG_PAINT_HANDLE paint,
                                  NEMA_VG_GRAD_HANDLE  gradient,
                                  uint8_t              fill_rule);

typedef struct
{
    paint_show_func_t func;
    uint8_t           fill_rule;
} paint_demo_t;

//*****************************************************************************
//
//! @brief calculate matrix
//!
//
//*****************************************************************************
static void matrixCalculate(nema_matrix3x3_t m, float angle_degrees, float x,
                            float y)
{
    float cosa = nema_cos(angle_degrees);
    float sina = nema_sin(angle_degrees);

    m[0][0] = cosa;
    m[0][1] = sina;
    m[0][2] = x - (x * cosa) - (y * sina);
    m[1][0] = -sina;
    m[1][1] = cosa;
    m[1][2] = y + (x * sina) - (y * cosa);
}

//*****************************************************************************
//
//! @brief Demo of using the paint feature in VG.
//!
//
//*****************************************************************************
void vg_paint_linear(img_obj_t *des_img, NEMA_VG_PATH_HANDLE path,
                     NEMA_VG_PAINT_HANDLE paint, NEMA_VG_GRAD_HANDLE gradient,
                     uint8_t fill_rule)
{

    // clear framebuffer
    nema_bind_dst_tex(des_img->bo.base_phys, des_img->w, des_img->h,
                      des_img->format, des_img->stride);
    nema_set_clip(0, 0, des_img->w, des_img->h);
    nema_clear(nema_rgba(0xff, 0xff, 0x00, 0xff)); // yellow

    // Set path
    nema_vg_path_set_shape(path, 6, cmds_star, 12, data_star_small);

    // Set gradient.
    nema_vg_grad_set(gradient, COLOR_STOPS, stops, colors);

    // Set up VG context
    nema_vg_set_fill_rule(fill_rule);
    nema_vg_set_blend(NEMA_BL_SRC_OVER);

    // Set paint to linear gradient
    nema_vg_paint_set_type(paint, NEMA_VG_PAINT_GRAD_LINEAR);

    // Path offset
    float x_pos = LINEAR_GRADIENT_START_X;
    float y_pos = LINEAR_GRADIENT_START_Y;

    // Matrix
    nema_matrix3x3_t m_path;

    for (int i = 0; i < sizeof(grad) / sizeof(grad[0]); i++)
    {
        nema_mat3x3_load_identity(m_path);
        nema_mat3x3_translate(m_path, x_pos, y_pos);
        nema_vg_path_set_matrix(path, m_path);

        nema_vg_paint_set_grad_linear(paint, gradient, grad[i][0] + x_pos,
                                      grad[i][1] + y_pos, grad[i][2] + x_pos,
                                      grad[i][3] + y_pos,
                                      NEMA_TEX_CLAMP | NEMA_FILTER_BL);
        nema_vg_draw_path(path, paint);

        x_pos += STAR_DIST_X;
        if ((x_pos + STAR_DIST_X) > des_img->w)
        {
            x_pos = LINEAR_GRADIENT_START_X;
            y_pos += STAR_DIST_Y;

            if ((y_pos + STAR_DIST_Y) > des_img->h)
            {
                break;
            }
        }
    }
}

//*****************************************************************************
//
//! @brief Demo of using the paint feature in VG.
//!
//
//*****************************************************************************
void vg_paint_texture(img_obj_t *des_img, NEMA_VG_PATH_HANDLE path,
                      NEMA_VG_PAINT_HANDLE paint, NEMA_VG_GRAD_HANDLE gradient,
                      uint8_t fill_rule)
{

    // clear framebuffer
    nema_bind_dst_tex(des_img->bo.base_phys, des_img->w, des_img->h,
                      des_img->format, des_img->stride);
    nema_set_clip(0, 0, des_img->w, des_img->h);
    nema_clear(nema_rgba(0xff, 0xff, 0x00, 0xff)); // yellow

    // Set path
    nema_vg_path_set_shape(path, 6, cmds_star, 12, data_star_small);

    // Set gradient.
    nema_vg_grad_set(gradient, COLOR_STOPS, stops, colors);

    // Set up VG context
    nema_vg_set_fill_rule(fill_rule);
    nema_vg_set_blend(NEMA_BL_SRC_OVER);

    // Set paint to linear gradient
    nema_vg_paint_set_type(paint, NEMA_VG_PAINT_TEXTURE);
    nema_vg_paint_set_tex(paint, &g_sRef);

    // Path offset
    float x_pos = LINEAR_GRADIENT_START_X;
    float y_pos = LINEAR_GRADIENT_START_Y;

    // Matrix
    nema_matrix3x3_t m_path;
    nema_matrix3x3_t m_paint;

    float degree[STARS_IN_ONE_LINE] = {
                                        0.f,   45.f,  90.f,  135.f,
                                        180.f, 225.f, 270.f, 315.f
                                      };

    for (int i = 0; i < sizeof(grad) / sizeof(grad[0]); i++)
    {
        // Set path matrix
        nema_mat3x3_load_identity(m_path);
        nema_mat3x3_translate(m_path, x_pos, y_pos);
        nema_vg_path_set_matrix(path, m_path);

        // Set paint Matrix
        nema_mat3x3_load_identity(m_paint);
        matrixCalculate(m_paint, degree[i], REF_IMG_W / 2, REF_IMG_H / 2);
        nema_mat3x3_translate(m_paint, x_pos + STAR_X_OFFSET,
                              y_pos + STAR_Y_OFFSET);
        nema_vg_paint_set_tex_matrix(paint, m_paint);

        nema_vg_draw_path(path, paint);

        x_pos += STAR_DIST_X;
        if ((x_pos + STAR_DIST_X) > des_img->w)
        {
            x_pos = LINEAR_GRADIENT_START_X;
            y_pos += STAR_DIST_Y;

            if ((y_pos + STAR_DIST_Y) > des_img->h)
            {
                break;
            }
        }
    }
}

//*****************************************************************************
//
//! @brief Demo of using the paint feature in VG.
//!
//
//*****************************************************************************
void vg_paint_conic(img_obj_t *des_img, NEMA_VG_PATH_HANDLE path,
                    NEMA_VG_PAINT_HANDLE paint, NEMA_VG_GRAD_HANDLE gradient,
                    uint8_t fill_rule)
{

    // clear framebuffer
    nema_bind_dst_tex(des_img->bo.base_phys, des_img->w, des_img->h,
                      des_img->format, des_img->stride);
    nema_set_clip(0, 0, des_img->w, des_img->h);
    nema_clear(nema_rgba(0xff, 0xff, 0x00, 0xff)); // yellow

    // Set path
    nema_vg_path_set_shape(path, 6, cmds_star, 12, data_star_small);

    // Set gradient.
    nema_vg_grad_set(gradient, COLOR_STOPS, stops, colors);

    // Set up VG context
    nema_vg_set_fill_rule(fill_rule);
    nema_vg_set_blend(NEMA_BL_SRC_OVER);

    float x_pos         = ONE_STAR_START_X;
    float y_pos         = ONE_STAR_START_Y;
    float x_star_center = x_pos + STAR_WIDTH / 2 + STAR_X_OFFSET;
    float y_star_center = y_pos + STAR_HIGHT / 2 + STAR_Y_OFFSET + 12.f;

    nema_matrix3x3_t m_path;

    // calculate and set path matrix

    nema_mat3x3_load_identity(m_path);
    nema_mat3x3_translate(m_path, x_pos, y_pos);
    nema_vg_path_set_matrix(path, m_path);

    // Set conic paint
    nema_vg_paint_set_type(paint, NEMA_VG_PAINT_GRAD_CONICAL);
    nema_vg_paint_set_grad_conical(paint, gradient, x_star_center,
                                   y_star_center,
                                   NEMA_TEX_CLAMP | NEMA_FILTER_BL);

    // Draw path
    nema_vg_draw_path(path, paint);
}

//*****************************************************************************
//
//! @brief Demo of using the paint feature in VG.
//!
//
//*****************************************************************************
void vg_paint_radial(img_obj_t *des_img, NEMA_VG_PATH_HANDLE path,
                     NEMA_VG_PAINT_HANDLE paint, NEMA_VG_GRAD_HANDLE gradient,
                     uint8_t fill_rule)
{

    // clear framebuffer
    nema_bind_dst_tex(des_img->bo.base_phys, des_img->w, des_img->h,
                      des_img->format, des_img->stride);
    nema_set_clip(0, 0, des_img->w, des_img->h);
    nema_clear(nema_rgba(0xff, 0xff, 0x00, 0xff)); // yellow

    // Set path
    nema_vg_path_set_shape(path, 6, cmds_star, 12, data_star_small);

    // Set gradient.
    nema_vg_grad_set(gradient, COLOR_STOPS, stops, colors);

    // Set up VG context
    nema_vg_set_fill_rule(fill_rule);
    nema_vg_set_blend(NEMA_BL_SRC_OVER);

    float x_pos         = ONE_STAR_START_X;
    float y_pos         = ONE_STAR_START_Y;
    float x_star_center = x_pos + STAR_WIDTH / 2 + STAR_X_OFFSET;
    float y_star_center = y_pos + STAR_HIGHT / 2 + STAR_Y_OFFSET + 12.f;

    nema_matrix3x3_t m_path;

    // calculate and set path matrix
    nema_mat3x3_load_identity(m_path);
    nema_mat3x3_translate(m_path, x_pos, y_pos);
    nema_vg_path_set_matrix(path, m_path);

    // Set radial paint
    nema_vg_paint_set_type(paint, NEMA_VG_PAINT_GRAD_RADIAL);
    nema_vg_paint_set_grad_radial(paint, gradient, x_star_center, y_star_center,
                                  STAR_WIDTH / 2,
                                  NEMA_TEX_CLAMP | NEMA_FILTER_BL);

    // Draw path
    nema_vg_draw_path(path, paint);
}

//*****************************************************************************
//
//! @brief Demo of using the paint feature in VG.
//!
//
//*****************************************************************************
void vg_paint_fill(img_obj_t *des_img, NEMA_VG_PATH_HANDLE path,
                   NEMA_VG_PAINT_HANDLE paint, NEMA_VG_GRAD_HANDLE gradient,
                   uint8_t fill_rule)
{

    // clear framebuffer
    nema_bind_dst_tex(des_img->bo.base_phys, des_img->w, des_img->h,
                      des_img->format, des_img->stride);
    nema_set_clip(0, 0, des_img->w, des_img->h);
    nema_clear(nema_rgba(0xff, 0xff, 0x00, 0xff)); // yellow

    // Set path
    nema_vg_path_set_shape(path, 6, cmds_star, 12, data_star_small);

    // Set gradient.
    nema_vg_grad_set(gradient, COLOR_STOPS, stops, colors);

    // Set up VG context
    nema_vg_set_fill_rule(fill_rule);
    nema_vg_set_blend(NEMA_BL_SRC_OVER);

    nema_matrix3x3_t m_path;

    // calculate and set path matrix
    nema_mat3x3_load_identity(m_path);
    nema_mat3x3_translate(m_path, ONE_STAR_START_X, ONE_STAR_START_Y);
    nema_vg_path_set_matrix(path, m_path);

    // Set paint
    nema_vg_paint_set_type(paint, NEMA_VG_PAINT_FILL);
    nema_vg_paint_set_paint_color(paint,
                                  nema_rgba(0x00, 0xff, 0x000, 0xff)); // green

    // Draw path
    nema_vg_draw_path(path, paint);
}

//*****************************************************************************
//
//! @brief Demo of using the paint feature in VG.
//!
//
//*****************************************************************************
void vg_paint_outline(img_obj_t *des_img, NEMA_VG_PATH_HANDLE path,
                      NEMA_VG_PAINT_HANDLE paint, NEMA_VG_GRAD_HANDLE gradient,
                      uint8_t fill_rule)
{
    // Unused
    (void)fill_rule;

    // clear framebuffer
    nema_bind_dst_tex(des_img->bo.base_phys, des_img->w, des_img->h,
                      des_img->format, des_img->stride);
    nema_set_clip(0, 0, des_img->w, des_img->h);
    nema_clear(nema_rgba(0xff, 0xff, 0x00, 0xff)); // yellow

    // Set path
    nema_vg_path_set_shape(path, 6, cmds_star, 12, data_star_small);

    // Set gradient.
    nema_vg_grad_set(gradient, COLOR_STOPS, stops, colors);

    // Use this fill rule to draw outline
    nema_vg_set_fill_rule(NEMA_VG_FILL_DRAW);
    // Set up VG context
    nema_vg_set_blend(NEMA_BL_SRC_OVER);

    nema_matrix3x3_t m_path;

    // calculate and set path matrix
    nema_mat3x3_load_identity(m_path);
    nema_mat3x3_translate(m_path, ONE_STAR_START_X, ONE_STAR_START_Y);
    nema_vg_path_set_matrix(path, m_path);

    // Set paint
    nema_vg_paint_set_type(paint, NEMA_VG_PAINT_FILL);
    nema_vg_paint_set_paint_color(paint,
                                  nema_rgba(0x00, 0xff, 0x000, 0xff)); // green

    // Draw path
    nema_vg_draw_path(path, paint);
}

//*****************************************************************************
//
//! @brief Demo of using the paint feature in VG.
//!
//
//*****************************************************************************
void vg_paint_outline_width(img_obj_t *des_img, NEMA_VG_PATH_HANDLE path,
                            NEMA_VG_PAINT_HANDLE paint,
                            NEMA_VG_GRAD_HANDLE gradient, uint8_t fill_rule)
{
    // Unused
    (void)fill_rule;

    // clear framebuffer
    nema_bind_dst_tex(des_img->bo.base_phys, des_img->w, des_img->h,
                      des_img->format, des_img->stride);
    nema_set_clip(0, 0, des_img->w, des_img->h);
    nema_clear(nema_rgba(0xff, 0xff, 0x00, 0xff)); // yellow

    // Set path
    nema_vg_path_set_shape(path, 6, cmds_star, 12, data_star_small);

    // Set gradient.
    nema_vg_grad_set(gradient, COLOR_STOPS, stops, colors);

    // Use this fill rule to draw outline
    nema_vg_set_fill_rule(NEMA_VG_FILL_DRAW);
    // Set up VG context
    nema_vg_set_blend(NEMA_BL_SRC_OVER);

    nema_matrix3x3_t m_path;

    // calculate and set path matrix
    nema_mat3x3_load_identity(m_path);
    nema_mat3x3_translate(m_path, ONE_STAR_START_X, ONE_STAR_START_Y);
    nema_vg_path_set_matrix(path, m_path);

    // Set paint
    nema_vg_paint_set_type(paint, NEMA_VG_PAINT_FILL);
    nema_vg_paint_set_paint_color(paint, nema_rgba(0x00, 0xff, 0x000, 0xff));
    // Set outline width
    nema_vg_paint_set_stroke_width(paint, 5.f);

    // Draw path
    nema_vg_draw_path(path, paint);
}

//*****************************************************************************
//
//! @brief Render task.
//!
//
//*****************************************************************************
void RenderTask(void *pvParameters)
{
    int ret;

#ifdef GPU_WORK_TIME_PRINT
    uint32_t time_start;
    uint32_t time_end;
#endif

    am_util_stdio_printf("Render task start!\n");

    // Power on GPU
    am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_GFX);

    // Initialize NemaGFX
    ret = nema_init();
    if (ret != 0)
    {
        am_util_stdio_printf("GPU init failed!\n");

        // suspend and delete this task.
        vTaskDelete(NULL);
    }

    // Init vg
    nema_vg_init(g_pFrameBufferGPU->w, g_pFrameBufferGPU->h);

    // Create GPU command list
    nema_cmdlist_t cl = nema_cl_create();
    nema_cl_bind(&cl);

    // Create path, this path will be reused with different offset.
    NEMA_VG_PATH_HANDLE path = nema_vg_path_create();

    // Create paint
    NEMA_VG_PAINT_HANDLE paint = nema_vg_paint_create();

    // Create gradient, this gradient will be reused by different paint.
    NEMA_VG_GRAD_HANDLE gradient = nema_vg_grad_create();

    paint_demo_t paint_demo[] =
    {
        {vg_paint_linear, NEMA_VG_FILL_EVEN_ODD},
        {vg_paint_linear, NEMA_VG_FILL_NON_ZERO},
        {vg_paint_texture, NEMA_VG_FILL_EVEN_ODD},
        {vg_paint_texture, NEMA_VG_FILL_NON_ZERO},
        {vg_paint_conic, NEMA_VG_FILL_EVEN_ODD},
        {vg_paint_conic, NEMA_VG_FILL_NON_ZERO},
        // {vg_paint_radial, NEMA_VG_FILL_EVEN_ODD},
        // {vg_paint_radial, NEMA_VG_FILL_NON_ZERO},
        {vg_paint_fill, NEMA_VG_FILL_EVEN_ODD},
        {vg_paint_fill, NEMA_VG_FILL_NON_ZERO},
        {vg_paint_outline, NEMA_VG_FILL_EVEN_ODD},
        {vg_paint_outline_width, NEMA_VG_FILL_NON_ZERO},
    };

    uint32_t start_tick         = xTaskGetTickCount();
    uint32_t paint_type_pointer = 0;

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

        // rewind and bind the CL
        nema_cl_rewind(&cl);

        // Change the paint type at each second.
        uint32_t current_tick = xTaskGetTickCount();
        if (current_tick - start_tick > 5000)
        {
            start_tick = current_tick;
            paint_type_pointer++;
            if (paint_type_pointer >=
                (sizeof(paint_demo) / sizeof(paint_demo[0])) )
            {
                paint_type_pointer = 0;
            }
        }

        // Draw the star with paint.
        paint_show_func_t func      = paint_demo[paint_type_pointer].func;
        uint8_t           fill_rule = paint_demo[paint_type_pointer].fill_rule;
        func(g_pFrameBufferGPU, path, paint, gradient, fill_rule);

        // start GPU, submit CL
        nema_cl_submit(&cl);
#ifdef USE_DEBUG_PIN_IN_GUI_TASK
        am_hal_gpio_output_set(DEBUG_PIN_1);
#endif

#ifdef GPU_WORK_TIME_PRINT
        time_start = xTaskGetTickCount();
#endif

        // Wait GPU
        nema_cl_wait(&cl);

#ifdef GPU_WORK_TIME_PRINT
        time_end = xTaskGetTickCount();

        am_util_stdio_printf("%d\n", time_end - time_start);
#endif

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

    // Release resource
    nema_vg_paint_destroy(paint);
    nema_vg_path_destroy(path);
    nema_vg_grad_destroy(gradient);

    nema_vg_deinit();
}
