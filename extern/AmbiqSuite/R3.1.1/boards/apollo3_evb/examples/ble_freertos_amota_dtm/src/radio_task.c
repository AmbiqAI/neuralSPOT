//*****************************************************************************
//
//! @file radio_task.c
//!
//! @brief Task to handle radio operation.
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
#include "ble_freertos_amota.h"

//*****************************************************************************
//
// WSF standard includes.
//
//*****************************************************************************
#include "wsf_types.h"
#include "wsf_trace.h"
#include "wsf_buf.h"
#include "wsf_timer.h"

//*****************************************************************************
//
// Includes for operating the ExactLE stack.
//
//*****************************************************************************
#include "hci_handler.h"
#include "dm_handler.h"
#include "l2c_handler.h"
#include "att_handler.h"
#include "smp_handler.h"
#include "l2c_api.h"
#include "att_api.h"
#include "smp_api.h"
#include "app_api.h"
#include "hci_core.h"
#include "hci_drv.h"
#include "hci_drv_apollo.h"
#include "hci_drv_apollo3.h"

#include "am_mcu_apollo.h"
#include "am_util.h"
#include "am_bsp.h"

#include "hci_apollo_config.h"
#include "wsf_msg.h"

//*****************************************************************************
//
// Includes for the ExactLE "fit" profile.
//
//*****************************************************************************
#include "amota_api.h"
#include "amotas_api.h"
#include "app_ui.h"

uint8_t enter_dtm_flag = 0;

//*****************************************************************************
//
// Radio task handle.
//
//*****************************************************************************
TaskHandle_t radio_task_handle;

//*****************************************************************************
//
// Function prototypes
//
//*****************************************************************************
void exactle_stack_init(void);

//*****************************************************************************
//
// WSF buffer pools.
//
//*****************************************************************************
#define WSF_BUF_POOLS               4

// Important note: the size of g_pui32BufMem should includes both overhead of internal
// buffer management structure, wsfBufPool_t (up to 16 bytes for each pool), and pool
// description (e.g. g_psPoolDescriptors below).

// Memory for the buffer pool
// extra AMOTA_PACKET_SIZE bytes for OTA handling
static uint32_t g_pui32BufMem[
        (WSF_BUF_POOLS*16
         + 16*8 + 32*4 + 64*6 + 280*8) / sizeof(uint32_t)];

// Default pool descriptor.
static wsfBufPoolDesc_t g_psPoolDescriptors[WSF_BUF_POOLS] =
{
    {  16,  8 },
    {  32,  4 },
    {  64,  6 },
    { 280,  8 }
};

//*****************************************************************************
//
// Tracking variable for the scheduler timer.
//
//*****************************************************************************

void radio_timer_handler(void);



//*****************************************************************************
//
// Initialization for the ExactLE stack.
//
//*****************************************************************************
void
exactle_stack_init(void)
{
    wsfHandlerId_t handlerId;
    uint16_t       wsfBufMemLen;
    //
    // Set up timers for the WSF scheduler.
    //
    WsfOsInit();
    WsfTimerInit();

    //
    // Initialize a buffer pool for WSF dynamic memory needs.
    //
    wsfBufMemLen = WsfBufInit(sizeof(g_pui32BufMem), (uint8_t *)g_pui32BufMem, WSF_BUF_POOLS,
               g_psPoolDescriptors);

    if (wsfBufMemLen > sizeof(g_pui32BufMem))
    {
        am_util_debug_printf("Memory pool is too small by %d\r\n",
                             wsfBufMemLen - sizeof(g_pui32BufMem));
    }

    //
    // Initialize the WSF security service.
    //
    SecInit();
    SecAesInit();
    SecCmacInit();
    SecEccInit();

    //
    // Set up callback functions for the various layers of the ExactLE stack.
    //
    handlerId = WsfOsSetNextHandler(HciHandler);
    HciHandlerInit(handlerId);

    handlerId = WsfOsSetNextHandler(DmHandler);
    DmDevVsInit(0);
    DmAdvInit();
    DmConnInit();
    DmConnSlaveInit();
    DmSecInit();
    DmSecLescInit();
    DmPrivInit();
    DmHandlerInit(handlerId);

    handlerId = WsfOsSetNextHandler(L2cSlaveHandler);
    L2cSlaveHandlerInit(handlerId);
    L2cInit();
    L2cSlaveInit();

    handlerId = WsfOsSetNextHandler(AttHandler);
    AttHandlerInit(handlerId);
    AttsInit();
    AttsIndInit();
    AttcInit();

    handlerId = WsfOsSetNextHandler(SmpHandler);
    SmpHandlerInit(handlerId);
    SmprInit();
    SmprScInit();
    HciSetMaxRxAclLen(251);

    handlerId = WsfOsSetNextHandler(AppHandler);
    AppHandlerInit(handlerId);

    handlerId = WsfOsSetNextHandler(AmotaHandler);
    AmotaHandlerInit(handlerId);

    handlerId = WsfOsSetNextHandler(HciDrvHandler);
    HciDrvHandlerInit(handlerId);
}

//*****************************************************************************
//
// UART interrupt handler.
//
//*****************************************************************************
#if 0
void
am_uart_isr(void)
{
    uint32_t ui32Status;

    //
    // Read and save the interrupt status, but clear out the status register.
    //
    ui32Status = UARTn(0)->MIS;
    UARTn(0)->IEC = ui32Status;

    //
    // Allow the HCI driver to respond to the interrupt.
    //
    //HciDrvUartISR(ui32Status);

    // Signal radio task to run

    WsfTaskSetReady(0, 0);
}
#endif
//*****************************************************************************
//
// Interrupt handler for BLE
//
//*****************************************************************************
void
am_ble_isr(void)
{

    HciDrvIntService();

    // Signal radio task to run

    WsfTaskSetReady(0, 0);
}

//*****************************************************************************
//
// Perform initial setup for the radio task.
//
//*****************************************************************************
void
RadioTaskSetup(void)
{
    am_util_debug_printf("RadioTask: setup\r\n");


    NVIC_SetPriority(BLE_IRQn, NVIC_configMAX_SYSCALL_INTERRUPT_PRIORITY);

    //
    // Boot the radio.
    //
    HciDrvRadioBoot(1);
}

//*****************************************************************************
//
// Short Description.
//
//*****************************************************************************
void
RadioTask(void *pvParameters)
{
#if WSF_TRACE_ENABLED == TRUE
    //
    // Enable ITM
    //
    am_util_debug_printf("Starting wicentric trace:\n\n");
#endif

    //
    // Initialize the main ExactLE stack.
    //
    exactle_stack_init();

    //
    // Start the "Amota" profile.
    //
    AmotaStart();

    while (1)
    {
        //
        // Enter DTM mode and process,
        // loop there.
        //
        if (DTM_MODE)
        {
            ResetBleAndEnterDTM();
            DtmModeProc();
        }

        //
        // Calculate the elapsed time from our free-running timer, and update
        // the software timers in the WSF scheduler.
        //
        wsfOsDispatcher();

    }
}
