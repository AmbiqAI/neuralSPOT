//*****************************************************************************
//
//! @file gui_task.c
//!
//! @brief Task to handle GUI operations.
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

#include "psram.h"

#include "resource/DejaVuSans_ttf_kern.h"

//*****************************************************************************
//
// Macro definitions
//
//*****************************************************************************

#define PSRAM_TTF_FONT (MSPI_XIP_BASE_ADDRESS + 0)

//*****************************************************************************
//
// GUI task handle.
//
//*****************************************************************************
TaskHandle_t GuiTaskHandle;

//*****************************************************************************
//
// Semaphores used to sync Gui task with render task, display task.
//
//*****************************************************************************
SemaphoreHandle_t g_semDCStart  = NULL;
SemaphoreHandle_t g_semGPUStart = NULL;
SemaphoreHandle_t g_semDCEnd    = NULL;
SemaphoreHandle_t g_semGPUEnd   = NULL;

//*****************************************************************************
//
// Texture.
//
//*****************************************************************************
void *pFont = NULL;

//*****************************************************************************
//
// Global Variables
//
//*****************************************************************************

// Frame buffer
static img_obj_t g_sFrameBuffer[2] = {
    {{0}, FB_RESX, FB_RESY, -1, 1, NEMA_RGB565, NEMA_FILTER_BL},
    {{0}, FB_RESX, FB_RESY, -1, 1, NEMA_RGB565, NEMA_FILTER_BL},
};

// Pointer to frame buffer
img_obj_t *g_pFrameBufferGPU = &g_sFrameBuffer[0];
img_obj_t *g_pFrameBufferDC  = &g_sFrameBuffer[1];

//*****************************************************************************
//
//! @brief Allocate buffer in SSRAM using tsi_malloc
//!
//! @param img - img to be hold on the buffer.
//!
//! @return 0: success, -1: failed.
//!
//*****************************************************************************
int SSRAM_buffer_alloc(img_obj_t *img)
{
    uint32_t size;

    size = nema_texture_size(img->format, 0, img->w, img->h);

    img->bo = nema_buffer_create(size);

    if (img->bo.base_virt == NULL)
    {
        am_util_stdio_printf("TSI buffer Malloc failed!\n");
        return -1;
    }

    memset(img->bo.base_virt, 0, size);

    return 0;
}

//*****************************************************************************
//!
//! @brief load font.
//!
//! @return 0: success, -1: failed.
//!
//!
//*****************************************************************************
int ttf_font_load(void)
{
    // load font data
    memcpy((void*)PSRAM_TTF_FONT,
           DejaVuSans_ttf_kern.data,
           DejaVuSans_ttf_kern.data_length*4);

    DejaVuSans_ttf_kern.data = (void*)PSRAM_TTF_FONT;

    return 0;
}

//*****************************************************************************
//!
//! @brief Frame buffer init.
//!
//! @return 0: success, -1: failed.
//!
//*****************************************************************************
int texture_framebuffer_init(void)
{
    int ret;

    // alloc framebuffer space in SSRAM
    ret = SSRAM_buffer_alloc(&g_sFrameBuffer[0]);
    if (ret < 0)
    {
        return ret;
    }

    ret = SSRAM_buffer_alloc(&g_sFrameBuffer[1]);
    if (ret < 0)
    {
        return ret;
    }

    return 0;
}

//*****************************************************************************
//!
//! @brief Frame buffer swap.
//!
//! @return None.
//!
//*****************************************************************************
void frameBuffer_swap(void)
{
    img_obj_t *temp;

    temp              = g_pFrameBufferGPU;
    g_pFrameBufferGPU = g_pFrameBufferDC;
    g_pFrameBufferDC  = temp;
}

//*****************************************************************************
//!
//! @brief Gui task init.
//!
//! Create frame buffer and load texture and font from MRAM to PSRAM.
//!
//! @return 0: success, -1: failed.
//!
//*****************************************************************************
int GuiTaskInit(void)
{
#ifdef USE_DEBUG_PIN_IN_GUI_TASK
    // GPIODEBUG_PIN_1 is used to measure the GPU work time.
    // GPIODEBUG_PIN_2 is used to measure the DC work time.
    // GPIODEBUG_PIN_3 is used to measure the GPU sleep time.
    // GPIODEBUG_PIN_4 is used to measure the DC sleep time.
    // GPIODEBUG_PIN_5 is the DC start signal.
    // GPIODEBUG_PIN_6 is the GPU start signal.

    am_hal_gpio_pinconfig(DEBUG_PIN_1, am_hal_gpio_pincfg_output);
    am_hal_gpio_pinconfig(DEBUG_PIN_2, am_hal_gpio_pincfg_output);
    am_hal_gpio_pinconfig(DEBUG_PIN_3, am_hal_gpio_pincfg_output);
    am_hal_gpio_pinconfig(DEBUG_PIN_4, am_hal_gpio_pincfg_output);
    am_hal_gpio_pinconfig(DEBUG_PIN_5, am_hal_gpio_pincfg_output);
    am_hal_gpio_pinconfig(DEBUG_PIN_6, am_hal_gpio_pincfg_output);
#endif

#ifdef DEBUG_PIN_GPU_ISR
    // GPIODEBUG_PIN_1 is used to measure the GPU work time.
    am_hal_gpio_pinconfig(DEBUG_PIN_GPU_ISR, am_hal_gpio_pincfg_output);
#endif

    // Power on GPU
    am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_GFX);

    // Initialize NemaGFX
    int ret = nema_init();
    if (ret != 0)
    {
        am_util_stdio_printf("GPU init failed!\n");
        return ret;
    }

    // Init NemaVG
    nema_vg_init(g_pFrameBufferGPU->w, g_pFrameBufferGPU->h);
    if ( nema_vg_get_error() != NEMA_VG_ERR_NO_ERROR )
    {
        am_util_stdio_printf("NemaVG init failed!\n");
        return ret;
    }

    // Allocate memory for framebuffer and textures
    ret = texture_framebuffer_init();
    if (ret < 0)
    {
        am_util_stdio_printf("frame buffer init failed!\n");
        return ret;
    }

    ret = ttf_font_load();
    if (ret < 0)
    {
        am_util_stdio_printf("TTF font load failed!\n");
        return ret;
    }

    //
    // Create the semaphone used to sync GUI task with render task
    //
    g_semDCStart = xSemaphoreCreateBinary();
    g_semDCEnd = xSemaphoreCreateBinary();

    //
    // Create the semaphone used to sync GUI task with display task
    //
    g_semGPUStart = xSemaphoreCreateBinary();
    g_semGPUEnd = xSemaphoreCreateBinary();

    if ( (!g_semDCStart) || (!g_semDCEnd) || (!g_semGPUStart) || (!g_semGPUEnd) )
    {
        am_util_stdio_printf("Semphone created failed!\n");
        ret = -1;
        return ret;
    }

    return 0;
}

//*****************************************************************************
//!
//! @brief Gui task.
//!
//! @param pvParameters - parameters passed to this task.
//!
//! @return None.
//!
//*****************************************************************************
void GuiTask(void *pvParameters)
{
    am_util_stdio_printf("GUI task start!\n");

    while (1)
    {
        // Start GPU
#ifdef USE_DEBUG_PIN_IN_GUI_TASK
        am_hal_gpio_output_set(DEBUG_PIN_6);
#endif
        xSemaphoreGive(g_semGPUStart);

        // Start DC/DSI
#ifdef USE_DEBUG_PIN_IN_GUI_TASK
        am_hal_gpio_output_set(DEBUG_PIN_5);
#endif
        xSemaphoreGive(g_semDCStart);

        // wait GPU
        xSemaphoreTake(g_semGPUEnd, portMAX_DELAY);

        // wait DC
        xSemaphoreTake(g_semDCEnd, portMAX_DELAY);

        // Swap framebuffer
        frameBuffer_swap();
    }
}
