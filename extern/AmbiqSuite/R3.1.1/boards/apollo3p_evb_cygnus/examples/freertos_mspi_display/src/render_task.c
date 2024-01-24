//*****************************************************************************
//
//! @file render_task.c
//!
//! @brief Task to handle rendering operation.
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
#include "freertos_mspi_display.h"


//*****************************************************************************
//
// Global Variables
//
//*****************************************************************************
volatile bool         g_bDisplayDone = false;
volatile bool         g_bTEInt = false;
void                  *g_MSPIDisplayHandle;
void                  *g_DisplayHandle;

//*****************************************************************************
//
// Local Variables
//
//*****************************************************************************
// Buffer for non-blocking transactions for Display MSPI - Needs to be big enough to accomodate
// all the transactions
static uint32_t        g_MspiDisplayQBuffer[(AM_HAL_MSPI_CQ_ENTRY_SIZE / 4) * 12];

// Display MSPI configuration
static am_devices_mspi_rm69330_config_t QuadDisplayMSPICfg =
{
    .eDeviceConfig            = AM_HAL_MSPI_FLASH_QUAD_CE0_1_4_4,
    .eClockFreq               = AM_HAL_MSPI_CLK_48MHZ,
    .ui32NBTxnBufLength       = sizeof(g_MspiDisplayQBuffer) / sizeof(uint32_t),
    .pNBTxnBuf                = g_MspiDisplayQBuffer,
};

//! MSPI interrupts.
static const IRQn_Type mspi_display_interrupts[] =
{
    MSPI0_IRQn,
#if defined(AM_PART_APOLLO3P)
    MSPI1_IRQn,
    MSPI2_IRQn,
#endif
};

//
// Take over the interrupt handler for whichever MSPI we're using.
//
#define display_mspi_isr                                                          \
    am_mspi_isr1(DISPLAY_MSPI_MODULE)
#define am_mspi_isr1(n)                                                        \
    am_mspi_isr(n)
#define am_mspi_isr(n)                                                         \
    am_mspi ## n ## _isr

//*****************************************************************************
//
// MSPI ISRs.
//
//*****************************************************************************
void display_mspi_isr(void)
{
    uint32_t      ui32Status;

    am_hal_mspi_interrupt_status_get(g_MSPIDisplayHandle, &ui32Status, false);

    am_hal_mspi_interrupt_clear(g_MSPIDisplayHandle, ui32Status);

    am_hal_mspi_interrupt_service(g_MSPIDisplayHandle, ui32Status);
}

//*****************************************************************************
//
// Render task handle.
//
//*****************************************************************************
TaskHandle_t render_task_handle;

//*****************************************************************************
//
// Handle for Render-related events.
//
//*****************************************************************************
EventGroupHandle_t xRenderEventHandle;

uint32_t g_numDisplay = 0;

static void teInt_handler(void)
{
    BaseType_t xHigherPriorityTaskWoken, xResult;
    // Signal main task that TE has arrived
    static uint32_t teCount = 0;
    if (teCount++ < TE_DELAY)
    {
        return;
    }
    teCount = 0;
    xHigherPriorityTaskWoken = pdFALSE;

    xResult = xEventGroupSetBitsFromISR(xMainEventHandle, MAIN_EVENT_TE,
                                        &xHigherPriorityTaskWoken);

    if (xResult != pdFAIL)
    {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

// Initialization
uint32_t
display_init(void)
{
    uint32_t ui32Status;

    NVIC_SetPriority(DISPLAY_MSPI_IRQn, 0x4);


    //
    // Initialize the display specific GPIO signals.
    //
    am_hal_gpio_pinconfig(AM_BSP_GPIO_DSPL_RESET, g_AM_HAL_GPIO_OUTPUT);
    am_hal_gpio_output_set(AM_BSP_GPIO_DSPL_RESET);

    am_hal_gpio_pinconfig(AM_BSP_GPIO_DSPL_TE, g_AM_BSP_GPIO_DSPL_TE);

    am_hal_gpio_pinconfig(AM_BSP_GPIO_DSPL0_OLED_EN, g_AM_HAL_GPIO_INPUT);

    am_hal_gpio_pinconfig(AM_BSP_GPIO_DSPL0_OLED_PWER_EN, g_AM_HAL_GPIO_OUTPUT);
    am_hal_gpio_output_clear(AM_BSP_GPIO_DSPL0_OLED_PWER_EN);

    am_hal_gpio_pinconfig(AM_BSP_GPIO_DSPL0_VIO_EN, g_AM_HAL_GPIO_OUTPUT);
    am_hal_gpio_output_set(AM_BSP_GPIO_DSPL0_VIO_EN);

    am_hal_gpio_pinconfig(AM_BSP_GPIO_DSPL0_DSPL_3V3_EN, g_AM_HAL_GPIO_OUTPUT);
    am_hal_gpio_output_set(AM_BSP_GPIO_DSPL0_DSPL_3V3_EN);

    // Initialize the MSPI Display
    ui32Status = am_devices_mspi_rm69330_init(DISPLAY_MSPI_MODULE, &QuadDisplayMSPICfg, &g_DisplayHandle, &g_MSPIDisplayHandle);
    if (AM_DEVICES_MSPI_RM69330_STATUS_SUCCESS != ui32Status)
    {
      DEBUG_PRINT("Failed to init Display device\n");
    }

    NVIC_EnableIRQ(mspi_display_interrupts[DISPLAY_MSPI_MODULE]);

    AM_HAL_GPIO_MASKCREATE(GpioIntMask);
    am_hal_gpio_interrupt_clear( AM_HAL_GPIO_MASKBIT(pGpioIntMask, AM_BSP_GPIO_DSPL_TE));
    am_hal_gpio_interrupt_register(AM_BSP_GPIO_DSPL_TE, teInt_handler);
    am_hal_gpio_interrupt_enable(AM_HAL_GPIO_MASKBIT(pGpioIntMask, AM_BSP_GPIO_DSPL_TE));
    NVIC_EnableIRQ(GPIO_IRQn);

    am_hal_interrupt_master_enable();

    am_devices_mspi_rm69330_display_on(g_DisplayHandle);

    return ui32Status;
}

void
display_write_complete(void *pCallbackCtxt, uint32_t transactionStatus)
{
    BaseType_t xHigherPriorityTaskWoken, xResult;
    if (transactionStatus != AM_HAL_STATUS_SUCCESS)
    {
        DEBUG_PRINT("\nDisplay# %d:FRAM Write Failed 0x%x\n", g_numDisplay, transactionStatus);
    }
    else
    {
        DEBUG_PRINT_SUCCESS("\nDisplay# %d:FRAM Write Done 0x%x\n", g_numDisplay, transactionStatus);
    }
    g_numDisplay++;
    // Signal main task that display is done
    xHigherPriorityTaskWoken = pdFALSE;

    xResult = xEventGroupSetBitsFromISR(xMainEventHandle, MAIN_EVENT_DISPLAY_DONE,
                                        &xHigherPriorityTaskWoken);

    if (xResult != pdFAIL)
    {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}


// Rendering
uint32_t
start_mspi_xfer(uint32_t address, uint32_t ui32NumBytes)
{
    uint32_t      ui32Status = 0;
    am_hal_mspi_callback_t  mspiSinkCb = 0;

    //DEBUG_GPIO_HIGH(DBG1_GPIO);
    mspiSinkCb = display_write_complete;
    DEBUG_PRINT("\nInitiating MSPI Transfer\n");

    ui32Status = am_devices_mspi_rm69330_nonblocking_write_adv(g_DisplayHandle,
                                               (uint8_t *)address,
                                               ui32NumBytes,
                                               0,
                                               0,
                                               mspiSinkCb,
                                               0);
    if (ui32Status)
    {
       DEBUG_PRINT("\nFailed to queue up MSPI Write transaction\n");
    }
    return ui32Status;
}


void
RenderTaskSetup(void)
{
    am_util_debug_printf("RenderTask: setup\r\n");
    //
    // Create an event handle for our wake-up events.
    //
    xRenderEventHandle = xEventGroupCreate();

    //
    // Make sure we actually allocated space for the events we need.
    //
    while (xRenderEventHandle == NULL);

   am_devices_mspi_rm69330_set_transfer_window(g_DisplayHandle, 0, COLUMN_NUM, 0, ROW_NUM);

    if (start_mspi_xfer(fb1, FRAME_SIZE))
    {
        while(1);
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
    uint32_t eventMask;

    while (1)
    {
        //
        // Wait for an event to be posted to the Radio Event Handle.
        //
        eventMask = xEventGroupWaitBits(xRenderEventHandle, 0xF, pdTRUE,
                            pdFALSE, portMAX_DELAY);
        if (eventMask != 0)
        {
            if (eventMask & RENDER_EVENT_START_NEW_FRAME)
            {
                if (start_mspi_xfer((g_numDisplay & 0x1) ? fb1 : fb2, FRAME_SIZE))
                {
                    while(1);
                }
            }
        }
    }
}

