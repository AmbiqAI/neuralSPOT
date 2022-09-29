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
#include "nemagfx_vg_ttf.h"

#include "graphics_power_control.h"
//*****************************************************************************
//
// Macro definitions
//
//*****************************************************************************
#define DEFAULT_FONT_SIZE   (14)

#define TITLE_POS_X                     (0)
#define TITLE_POS_Y                     (30)
#define TITLE_WIDTH                     (FB_RESX)
#define TITLE_HIGHT                     (60)
#define TITLE_BACKGROUND_COLOR          (0xff700070U)

#define STRING_SECTOR_WIDTH             (256)
#define STRING_SECTOR_HIGHT             (256)
#define STRING_SECTOR_POS_X             ((FB_RESX - STRING_SECTOR_WIDTH) / 2)
#define STRING_SECTOR_POS_Y             ((FB_RESY - STRING_SECTOR_HIGHT) / 2)
#define STRING_SECTOR_BACKGROUND_COLOR  (0xff707070U)

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
// Demo of drawing TTF font using VG features.
//
//*****************************************************************************
void vg_ttf_string(img_obj_t *des_img,
                   NEMA_VG_PAINT_HANDLE paint,
                   uint32_t align)
{
    //Bind framebuffer
    nema_bind_dst_tex(des_img->bo.base_phys,
                      des_img->w,
                      des_img->h,
                      des_img->format,
                      des_img->stride);

    //Set clip
    nema_set_clip(0, 0, des_img->w, des_img->h);

    //Set Blending Mode
    nema_clear(0x00000000);

    const char* x_align_string;

    uint32_t x_align = align & NEMA_VG_ALIGNX_MASK;

    if ( x_align == NEMA_VG_ALIGNX_LEFT )
    {
        x_align_string = "left";
    }
    else if ( x_align == NEMA_VG_ALIGNX_RIGHT )
    {
        x_align_string = "right";
    }
    else if ( x_align == NEMA_VG_ALIGNX_CENTER )
    {
        x_align_string = "center";
    }
    else if ( x_align == NEMA_VG_ALIGNX_JUSTIFY )
    {
        x_align_string = "justify";
    }
    else
    {
        x_align_string = "unknown";
    }

    const char* y_align_string;

    uint32_t y_align = align & NEMA_VG_ALIGNY_MASK;

    if ( y_align == NEMA_VG_ALIGNY_TOP )
    {
        y_align_string = "top";
    }
    else if ( y_align == NEMA_VG_ALIGNY_BOTTOM )
    {
        y_align_string = "bottom";
    }
    else if ( y_align == NEMA_VG_ALIGNY_CENTER )
    {
        y_align_string = "center";
    }
    else if ( y_align == NEMA_VG_ALIGNY_JUSTIFY )
    {
        y_align_string = "justify";
    }
    else
    {
        y_align_string = "unknown";
    }

    nema_buffer_t title_buffer = nema_buffer_create(256);
    snprintf((char*)title_buffer.base_virt, 256,
             "X Align:%s\nY Align:%s\n",
             x_align_string, y_align_string);

    //blend mode
    nema_set_blend_fill(NEMA_BL_SIMPLE);

    //Set title background
    nema_set_clip(TITLE_POS_X, TITLE_POS_Y, TITLE_WIDTH, TITLE_HIGHT);
    nema_fill_rect(TITLE_POS_X, TITLE_POS_Y, TITLE_WIDTH, TITLE_HIGHT,
                   TITLE_BACKGROUND_COLOR);

    //Draw title
    nema_vg_paint_set_paint_color(paint, nema_rgba(0, 0, 255, 255 ));
    nema_vg_print(paint, (char*)title_buffer.base_virt,
                  TITLE_POS_X, TITLE_POS_Y,
                  TITLE_WIDTH, TITLE_HIGHT,
                  NEMA_VG_ALIGNX_CENTER | NEMA_VG_TEXT_WRAP | NEMA_VG_ALIGNY_CENTER,
                  NULL);

    //Destroy buffer
    nema_buffer_destroy(&title_buffer);

    const char str[] = "AMBIQ - Hello World!!!\n"
                  "|----*******----!!!!!----------|\n"
                  "TEST TEST !\n"
                  "ABCDEFGhijklmnopqrestuvweeeeeeeeeeeeeeeeeeeeeeeeexyzd\n"
                  "υποστηρίζει ΚΑΙ Unicode χαρακτήρες!!!";

    //blend mode
    nema_set_blend_fill(NEMA_BL_SIMPLE);

    //Set string background
    nema_set_clip(STRING_SECTOR_POS_X, STRING_SECTOR_POS_Y,
                  STRING_SECTOR_WIDTH, STRING_SECTOR_HIGHT);
    nema_fill_rect(STRING_SECTOR_POS_X, STRING_SECTOR_POS_Y,
                   STRING_SECTOR_WIDTH, STRING_SECTOR_HIGHT,
                   STRING_SECTOR_BACKGROUND_COLOR);

    //Draw font
    nema_vg_paint_set_paint_color(paint, nema_rgba(255, 255, 0, 255 ));
    nema_vg_print(paint, str,
                  STRING_SECTOR_POS_X, STRING_SECTOR_POS_Y,
                  STRING_SECTOR_WIDTH, STRING_SECTOR_HIGHT,
                  align, NULL);

    uint32_t error = nema_vg_get_error();
    if ( error != NEMA_VG_ERR_NO_ERROR )
    {
        am_util_stdio_printf("error_code: 0x%X\n", error);
    }
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

    //Init NemaVG
    nema_vg_init(g_pFrameBufferGPU->w, g_pFrameBufferGPU->h);

    // Create GPU command list
    cl  = nema_cl_create_sized(10*1024);
    //Bind Command List
    nema_cl_bind_circular(&cl);

    //Create and set paint
    NEMA_VG_PAINT_HANDLE paint =  nema_vg_paint_create();
    nema_vg_paint_set_type(paint, NEMA_VG_PAINT_FILL);
    nema_vg_paint_set_stroke_width(paint, 0.f);

    //bind font
    nema_vg_bind_font(pDefaultFont);

    //set font size
    nema_vg_set_font_size(DEFAULT_FONT_SIZE);

    uint32_t start_tick = xTaskGetTickCount();

    uint32_t align_type[] =
    {
        NEMA_VG_ALIGNX_LEFT | NEMA_VG_TEXT_WRAP | NEMA_VG_ALIGNY_TOP,
        NEMA_VG_ALIGNX_RIGHT | NEMA_VG_TEXT_WRAP | NEMA_VG_ALIGNY_BOTTOM,
        NEMA_VG_ALIGNX_CENTER | NEMA_VG_TEXT_WRAP | NEMA_VG_ALIGNY_CENTER,
        NEMA_VG_ALIGNX_JUSTIFY | NEMA_VG_TEXT_WRAP | NEMA_VG_ALIGNY_JUSTIFY,
    };

    uint32_t align_type_pointer = 0;

    while(1)
    {
        //Wait start.
        xSemaphoreTake( g_semGPUStart, portMAX_DELAY);

#ifdef USE_DEBUG_PIN_IN_GUI_TASK
        am_hal_gpio_output_clear(DEBUG_PIN_3);
        am_hal_gpio_output_clear(DEBUG_PIN_6);
#endif

        //This API will directly return if GPU is still power on.
        //gpu_power_up();

        //Change the align type at each second.
        uint32_t current_tick = xTaskGetTickCount();
        if ( current_tick - start_tick > 5000 )
        {
            start_tick = current_tick;
            align_type_pointer ++;
            if ( align_type_pointer >= (sizeof(align_type) / sizeof(align_type[0])) )
            {
                align_type_pointer = 0;
            }
        }

        //Rewind the CL
        nema_cl_rewind(&cl);

        // Draw string
        vg_ttf_string(g_pFrameBufferGPU, paint,  align_type[align_type_pointer]);

        //start GPU, submit CL
        nema_cl_submit(&cl);
#ifdef USE_DEBUG_PIN_IN_GUI_TASK
        am_hal_gpio_output_set(DEBUG_PIN_1);
#endif

#ifdef GPU_WORK_TIME_PRINT
        time_start = xTaskGetTickCount();
#endif

        //Wait GPU
        nema_cl_wait(&cl);

#ifdef GPU_WORK_TIME_PRINT
        time_end = xTaskGetTickCount();

        am_util_stdio_printf("%d\n", time_end - time_start);
#endif

#ifdef USE_DEBUG_PIN_IN_GUI_TASK
        am_hal_gpio_output_clear(DEBUG_PIN_1);
#endif

        //Notify the GUI task.
        xSemaphoreGive(g_semGPUEnd);

        //Power Down
        //gpu_power_down();

#ifdef USE_DEBUG_PIN_IN_GUI_TASK
        am_hal_gpio_output_set(DEBUG_PIN_3);
#endif
    }

    //Destroy (free) Command List
    nema_cl_destroy(&cl);

    //de-initialize nema_vg
    nema_vg_paint_destroy(paint);
    nema_vg_deinit();

}

