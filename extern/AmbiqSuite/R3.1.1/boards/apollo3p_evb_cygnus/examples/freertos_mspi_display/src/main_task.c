//*****************************************************************************
//
//! @file main_task.c
//!
//! @brief Task to handle main operation.
//!
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2023, Ambiq Micro, Inc.
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
// This is part of revision release_sdk_3_1_1-10cda4b5e0 of the AmbiqSuite Development Package.
//
//*****************************************************************************

//*****************************************************************************
//
// Global includes for this project.
//
//*****************************************************************************
#include "freertos_mspi_display.h"

//*****************************************************************************
//
// Global Variables
//
//*****************************************************************************
//extern const unsigned char g_ambiq_logo_bmp0[];
//extern const unsigned char g_ambiq_logo_bmp2[];
//uint8_t                    *img0 = (uint8_t *)g_ambiq_logo_bmp0;
//uint8_t                    *img1 = (uint8_t *)g_ambiq_logo_bmp2;

extern const uint8_t acambiq_logo[];
extern const uint8_t watch_background[];
uint8_t                    *img0 = (uint8_t *)acambiq_logo;
uint8_t                    *img1 = (uint8_t *)watch_background;

uint32_t        fb1;
uint32_t        fb2;

//*****************************************************************************
//
// Composition task handle.
//
//*****************************************************************************
TaskHandle_t main_task_handle;

//*****************************************************************************
//
// Handle for Compose-related events.
//
//*****************************************************************************
EventGroupHandle_t xMainEventHandle;


//*****************************************************************************
//
// Interrupt handler for the GPIO pins.
//
//*****************************************************************************
void am_gpio_isr(void)
{
    //
    // Read and clear the GPIO interrupt status.
    //
#if defined(AM_PART_APOLLO3P)
    AM_HAL_GPIO_MASKCREATE(GpioIntStatusMask);

    am_hal_gpio_interrupt_status_get(false, pGpioIntStatusMask);
    am_hal_gpio_interrupt_clear(pGpioIntStatusMask);
    am_hal_gpio_interrupt_service(pGpioIntStatusMask);
#elif defined(AM_PART_APOLLO3)
    uint64_t ui64Status;

    am_hal_gpio_interrupt_status_get(false, &ui64Status);
    am_hal_gpio_interrupt_clear(ui64Status);
    am_hal_gpio_interrupt_service(ui64Status);
#else
    #error Unknown device.
#endif
}

//*****************************************************************************
//
// Perform initial setup for the main task.
//
//*****************************************************************************
void
MainTaskSetup(void)
{
    int iRet;

    am_util_debug_printf("MainTask: setup\r\n");
    //
    // Create an event handle for our wake-up events.
    //
    xMainEventHandle = xEventGroupCreate();

    //
    // Make sure we actually allocated space for the events we need.
    //
    while (xMainEventHandle == NULL);

    NVIC_SetPriority(DISPLAY_MSPI_IRQn, NVIC_configMAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(GPIO_IRQn, NVIC_configMAX_SYSCALL_INTERRUPT_PRIORITY);

    // Initialize the frame buffer pointers
    fb1 = (uint32_t)img0;
    fb2 = (uint32_t)img1;

    // Initialize the MSPI Display

    iRet = display_init();
    if (iRet)
    {
        DEBUG_PRINT("Unable to initialize Display\n");
        while(1);
    }

    am_hal_interrupt_master_enable();

}

typedef enum
{
    DISPLAY_IDLE,
    DISPLAY_SCHEDULED,
    DISPLAY_STARTED
} display_state_e;

//*****************************************************************************
//
// Short Description.
//
//*****************************************************************************
void
MainTask(void *pvParameters)
{
    uint32_t eventMask;
    display_state_e displayState = DISPLAY_IDLE;

    while (1)
    {
        //
        // Wait for an event to be posted to the Radio Event Handle.
        //
        eventMask = xEventGroupWaitBits(xMainEventHandle, 0xF, pdTRUE,
                            pdFALSE, portMAX_DELAY);
        if (eventMask != 0)
        {
            if (eventMask & MAIN_EVENT_DISPLAY_DONE)
            {
                displayState = DISPLAY_IDLE;
            }
            if ((displayState == DISPLAY_IDLE) && (eventMask & MAIN_EVENT_TE))
            {
                displayState = DISPLAY_STARTED;
                // Initiate new frame transfer
                xEventGroupSetBits(xRenderEventHandle, RENDER_EVENT_START_NEW_FRAME);
            }
        }
    }
}
