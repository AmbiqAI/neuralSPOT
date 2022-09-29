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
#include "nemagfx_vg_svg.h"

#include "nema_vg.h"
#include "nema_vg_tsvg.h"
#include "nema_hal.h"
#include "nema_math.h"
#include "nema_core.h"
#include "nema_utils.h"
#include "nema_event.h"
#include "nema_programHW.h"

#include "graphics_power_control.h"
//*****************************************************************************
//
// Macro definitions
//
//*****************************************************************************
#define CHARS_COUNT 100

//*****************************************************************************
//
// Render task handle.
//
//*****************************************************************************
TaskHandle_t RenderTaskHandle;

//*****************************************************************************
//
// Global Variables
//
//*****************************************************************************

//*****************************************************************************
//
// Demo of drawing SVG image using VG features.
//
//*****************************************************************************
void vg_svg(img_obj_t *des_img, void* image)
{
    //Bind framebuffer
    nema_bind_dst_tex(des_img->bo.base_phys,
                      des_img->w,
                      des_img->h,
                      des_img->format,
                      des_img->stride);

    //Set clip
    nema_set_clip(0, 0, des_img->w, des_img->h);

    //Clear
    nema_clear(0x00000000);

    // Draw the SVG
    nema_vg_draw_tsvg(image);

}

//*****************************************************************************
//
// Short Description.
//
//*****************************************************************************
void
RenderTask(void *pvParameters)
{
    int ret;
    static nema_cmdlist_t cl;
    float scale = 0.1f;
    float rot = 0.f;
    float default_scale = 0.f;
    uint32_t svg_w, svg_h;

#ifdef GPU_WORK_TIME_PRINT
    uint32_t time_start;
    uint32_t time_end;
#endif

    am_util_stdio_printf("Render task start!\n");

    //Power on GPU
    am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_GFX);

    //Initialize NemaGFX
    ret = nema_init();
    if (ret != 0)
    {
        am_util_stdio_printf("GPU init failed!\n");

        //suspend and delete this task.
        vTaskDelete(NULL);
    }

    //initialize nema_vg
    nema_vg_init(g_pFrameBufferGPU->w, g_pFrameBufferGPU->h);

    // Create GPU command list
    cl  = nema_cl_create_sized(10 * 1024);

    while(1)
    {
        //Wait start.
        xSemaphoreTake( g_semGPUStart, portMAX_DELAY);

#ifdef USE_DEBUG_PIN_IN_GUI_TASK
        am_hal_gpio_output_clear(DEBUG_PIN_3);
        //am_hal_gpio_output_clear(DEBUG_PIN_6);
#endif

        //This API will directly return if GPU is still power on.
        //gpu_power_up();

        //rewind and bind the CL
        nema_cl_bind_circular(&cl);
        //nema_cl_bind(&cl);
        nema_cl_rewind(&cl);

#ifdef GPU_WORK_TIME_PRINT
        time_start = xTaskGetTickCount();
#endif

#ifdef USE_DEBUG_PIN_IN_GUI_TASK
        am_hal_gpio_output_set(DEBUG_PIN_1);
#endif

        scale += 0.1f;

        if ( scale >= 3.0f )
        {
           scale = 0.1f;
        }

        rot += 10.f;
        if (rot >= 360.f)
        {
            rot = 0.f;
        }

        if ( default_scale == 0.f )
        {
            // Get resource width and hight.
            nema_vg_get_tsvg_resolution(g_pSvgBin, &svg_w, &svg_h);

            // Some svg file doesn't provide this information.
            if ( (svg_w == 0U) || (svg_h == 0U) )
            {
                svg_w = 498;
                svg_h = 600;
            }

            float scalex = g_pFrameBufferGPU->w / (float)svg_w;
            float scaley = g_pFrameBufferGPU->h / (float)svg_h;
            default_scale  = nema_min2(scalex, scaley);
        }

        //set matrix
        nema_matrix3x3_t m_path;
        nema_mat3x3_load_identity(m_path);
        nema_mat3x3_translate(m_path, -(svg_w*0.5f), -(svg_h*0.5f));
        nema_mat3x3_scale(m_path, default_scale * scale, default_scale * scale);
        nema_mat3x3_rotate(m_path, (float)rot);
        nema_mat3x3_translate(m_path, (g_pFrameBufferGPU->w*0.5f), (g_pFrameBufferGPU->h*0.5f));

        nema_vg_set_global_matrix(m_path);

        //Draw SVG image
        vg_svg(g_pFrameBufferGPU, g_pSvgBin);

        //start GPU, submit CL
        nema_cl_submit(&cl);

        //Wait GPU
        nema_cl_wait(&cl);

#ifdef USE_DEBUG_PIN_IN_GUI_TASK
        am_hal_gpio_output_clear(DEBUG_PIN_1);
#endif

#ifdef GPU_WORK_TIME_PRINT
        time_end = xTaskGetTickCount();
        am_util_stdio_printf("%d\n", time_end - time_start);
#endif

        //Notify the GUI task.
        xSemaphoreGive(g_semGPUEnd);

        //Power Down
        //gpu_power_down();

#ifdef USE_DEBUG_PIN_IN_GUI_TASK
        am_hal_gpio_output_set(DEBUG_PIN_3);
#endif
    }

    nema_cl_destroy(&cl);
    nema_vg_deinit();

}

