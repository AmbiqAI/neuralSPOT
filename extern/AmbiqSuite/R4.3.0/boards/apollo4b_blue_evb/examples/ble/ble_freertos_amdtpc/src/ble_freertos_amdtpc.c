//*****************************************************************************
//
//! @file ble_freertos_amdtpc.c
//!
//! @brief ARM Cordio BLE - AMDTP Client (Master) Example.
//!
//! Purpose: This example is the client (master) for the BLE Ambiq Micro
//! Data Transfer Protocol. This example is meant to run on an Apollo4 EVB
//! along with another Apollo4 EVB running ble_freertos_amdtps as the server.
//! This example provides a UART command line interface with a simple menu
//! that allows the user to scan, connect and initiate data transfers from
//! either M->S or S->M direction.
//!
//! Printing takes place over the ITM at 1M Baud.
//!
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

//*****************************************************************************
//
// This application has a large number of common include files. For
// convenience, we'll collect them all together in a single header and include
// that everywhere.
//
//*****************************************************************************
#include "ble_freertos_amdtpc.h"
#include "ble_menu.h"
#include "rtos.h"

#ifdef BLE_MENU
void *UART;

//*****************************************************************************
//
// Enable printing to the console.
//
//*****************************************************************************
void
enable_print_interface(void)
{
#if 0
    //
    // Initialize the printf interface for UART output.
    //
    am_bsp_uart_printf_enable();
#else
    //
    // Initialize a debug printing interface.
    //
    am_bsp_itm_printf_enable();
#endif
    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();
}
#define MAX_UART_PACKET_SIZE            2048
uint8_t g_pui8UARTTXBuffer[MAX_UART_PACKET_SIZE];
//*****************************************************************************
//
// UART configuration settings.
//
//*****************************************************************************
void
setup_serial(int32_t i32Module)
{
    //
    // Enable a UART to use for the menu.
    //
    const am_hal_uart_config_t sUartConfig =
    {
        //
        // Standard UART settings: 115200-8-N-1
        //
        .ui32BaudRate = 460800,
        .eDataBits    = AM_HAL_UART_DATA_BITS_8,
        .eParity = AM_HAL_UART_PARITY_NONE,
        .eStopBits = AM_HAL_UART_ONE_STOP_BIT,
        .eFlowControl = AM_HAL_UART_FLOW_CTRL_NONE,

        //
        // Set TX and RX FIFOs to interrupt at half-full.
        //
        .eTXFifoLevel = AM_HAL_UART_FIFO_LEVEL_28,
        .eRXFifoLevel = AM_HAL_UART_FIFO_LEVEL_28,

        //
        // Buffers
        //
        //.pui8TxBuffer = 0,
        //.ui32TxBufferSize = 0,
        //.pui8RxBuffer = 0,
        //.ui32RxBufferSize = 0,
    };
    am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_TX, g_AM_BSP_GPIO_COM_UART_TX);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_RX, g_AM_BSP_GPIO_COM_UART_RX);

    am_hal_uart_initialize(0, &UART);
    am_hal_uart_power_control(UART, AM_HAL_SYSCTRL_WAKE, false);
    am_hal_uart_configure(UART, &sUartConfig);

    am_hal_uart_buffer_configure(UART, g_pui8UARTTXBuffer, sizeof(g_pui8UARTTXBuffer), NULL, 0);

    //
    // Make sure the UART interrupt priority is set low enough to allow
    // FreeRTOS API calls.
    //
    NVIC_SetPriority(UART0_IRQn, NVIC_configMAX_SYSCALL_INTERRUPT_PRIORITY);

    //am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_TX, g_AM_BSP_GPIO_COM_UART_TX);
    //am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_RX, g_AM_BSP_GPIO_COM_UART_RX);

    //
    // Enable UART RX interrupts manually.
    //
    //am_hal_uart_interrupt_clear(UART, AM_HAL_UART_INT_RX | AM_HAL_UART_INT_RX_TMOUT);
    //am_hal_uart_interrupt_enable(UART, AM_HAL_UART_INT_RX | AM_HAL_UART_INT_RX_TMOUT);
    //NVIC_EnableIRQ(UART0_IRQn);
    NVIC_SetPriority((IRQn_Type)UART0_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ((IRQn_Type)UART0_IRQn);
    am_hal_uart_interrupt_enable(UART, (AM_HAL_UART_INT_RX |
                                 AM_HAL_UART_INT_RX_TMOUT));

    am_hal_interrupt_master_enable();
}

// buffer for printf
static char g_prfbuf[AM_PRINTF_BUFSIZE];

uint32_t
am_menu_printf(const char *pcFmt, ...)
{
    uint32_t ui32NumChars;
    uint32_t ui32BytesTransferred = 0;

    //
    // Convert to the desired string.
    //
    va_list pArgs;
    va_start(pArgs, pcFmt);
    ui32NumChars = am_util_stdio_vsprintf(g_prfbuf, pcFmt, pArgs);
    va_end(pArgs);

    //
    // This is where we print the buffer to the configured interface.
    //
    am_hal_uart_transfer_t sSend =
    {
        .eType = AM_HAL_UART_BLOCKING_WRITE,
        .pui8Data = (uint8_t *)g_prfbuf,
        .ui32NumBytes = ui32NumChars,
        .ui32TimeoutMs = AM_HAL_UART_WAIT_FOREVER,
        .pui32BytesTransferred = &ui32BytesTransferred,
    };

    am_hal_uart_transfer(UART, &sSend);

    //
    // Add extra delay to avoid failure in continous sending scenario.
    //
    am_util_delay_ms(1);

    //
    // return the number of characters printed.
    //
    return ui32NumChars;
}
#else

uint32_t
am_menu_printf(const char *pcFmt, ...)
{
    return 0;
}

#endif

//*****************************************************************************
//
// Main Function
//
//*****************************************************************************
int
main(void)
{
    //
    // Set the clock frequency
    //
#ifndef NOFPU
    //
    // Enable the floating point module, and configure the core for lazy
    // stacking.
    //
    am_hal_sysctrl_fpu_enable();
    am_hal_sysctrl_fpu_stacking_enable(true);
#else
    am_hal_sysctrl_fpu_disable();
#endif

    //
    // Configure the board for low power.
    //
    am_bsp_low_power_init();

    //
    // Enable printing to the console.
    //
#ifdef AM_DEBUG_PRINTF
    enable_print_interface();
#endif

    //
    // Initialize plotting interface.
    //
    am_util_debug_printf("FreeRTOS AMDTP Example\n");

#ifdef BLE_MENU
    setup_serial(0);
#endif

    //
    // Run the application.
    //
    run_tasks();

    //
    // We shouldn't ever get here.
    //
    while (1)
    {
    }

}

