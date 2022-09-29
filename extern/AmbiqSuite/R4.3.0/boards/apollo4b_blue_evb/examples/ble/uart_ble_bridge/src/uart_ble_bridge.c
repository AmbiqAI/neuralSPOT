//*****************************************************************************
//
//! @file uart_ble_bridge.c
//!
//! @brief Converts UART HCI commands to SPI.
//!
//! This exapmle can be used as a way to communicate by using UART HCI between
//! the Apollo4 (host) and Cooper (controller).
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

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "am_devices_cooper.h"
#include "am_util.h"
#include "am_util_ble_cooper.h"
#include "wsf_types.h"
#include "dtm_api.h"
#include "am_bsp.h"

//*****************************************************************************
//
// Macro Definition.
//
//*****************************************************************************
#define UART_HCI_BRIDGE                 (0)
#define MAX_UART_PACKET_SIZE            (2048)
#define UART_RX_TIMEOUT_MS              (1)
#define MAX_READ_BYTES                  (23)

//*****************************************************************************
//
// Global Variables
//
//*****************************************************************************
void *g_pvUART;
uint8_t g_pui8UARTTXBuffer[MAX_UART_PACKET_SIZE];

//*****************************************************************************
//
// External variables declaration.
//
//*****************************************************************************
extern am_devices_cooper_buffer(1024) g_psWriteData;

//*****************************************************************************
//
// External function declaration.
//
//*****************************************************************************
extern volatile uint32_t g_ui32SerialRxIndex;
extern uint16_t serial_rx_hci_state_machine(uint8_t *pBuf, uint16_t len);

//*****************************************************************************
//
// Interrupt handler for the UART.
//
//*****************************************************************************
#if UART_HCI_BRIDGE == 0
void am_uart_isr(void)
#else
void am_uart1_isr(void)
#endif //UART_HCI_BRIDGE == 0
{
    uint32_t ui32Status;
    uint8_t * pData = (uint8_t *) &(g_psWriteData.bytes[g_ui32SerialRxIndex]);

    //
    // Read the masked interrupt status from the UART.
    //
    am_hal_uart_interrupt_status_get(g_pvUART, &ui32Status, true);
    am_hal_uart_interrupt_clear(g_pvUART, ui32Status);
    am_hal_uart_interrupt_service(g_pvUART, ui32Status);
    //
    // If there's an RX interrupt, handle it in a way that preserves the
    // timeout interrupt on gaps between packets.
    //
    if (ui32Status & (AM_HAL_UART_INT_RX_TMOUT | AM_HAL_UART_INT_RX))
    {
        uint32_t ui32BytesRead;
        serial_data_read(pData, &ui32BytesRead);
        (void)serial_rx_hci_state_machine(pData, ui32BytesRead);
        g_ui32SerialRxIndex += ui32BytesRead;
    }
}

//*****************************************************************************
//
// Initialize the UART.
//
//*****************************************************************************
void serial_interface_init(void)
{
    am_util_stdio_printf("Apollo4 UART to SPI Bridge\n");

    //
    // Start the UART.
    //
    am_hal_uart_config_t sUartConfig =
    {
        //
        // Standard UART settings: 115200-8-N-1
        //
        .ui32BaudRate    = 115200,
        //
        // Set TX and RX FIFOs to interrupt at three-quarters full.
        //
        .eDataBits    = AM_HAL_UART_DATA_BITS_8,
        .eParity      = AM_HAL_UART_PARITY_NONE,
        .eStopBits    = AM_HAL_UART_ONE_STOP_BIT,
        .eFlowControl = AM_HAL_UART_FLOW_CTRL_NONE,
        .eTXFifoLevel = AM_HAL_UART_FIFO_LEVEL_28,
        .eRXFifoLevel = AM_HAL_UART_FIFO_LEVEL_28,
    };

    am_hal_uart_initialize(UART_HCI_BRIDGE, &g_pvUART);
    am_hal_uart_power_control(g_pvUART, AM_HAL_SYSCTRL_WAKE, false);
    am_hal_uart_configure(g_pvUART, &sUartConfig);
    am_hal_uart_buffer_configure(g_pvUART, g_pui8UARTTXBuffer, sizeof(g_pui8UARTTXBuffer), NULL, 0);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_TX, g_AM_BSP_GPIO_COM_UART_TX);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_RX, g_AM_BSP_GPIO_COM_UART_RX);
    //
    // Make sure to enable the interrupts for RX, since the HAL doesn't already
    // know we intend to use them.
    //
    NVIC_SetPriority((IRQn_Type)(UART0_IRQn + UART_HCI_BRIDGE), AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn + UART_HCI_BRIDGE));
    am_hal_uart_interrupt_enable(g_pvUART, (AM_HAL_UART_INT_RX |
                                 AM_HAL_UART_INT_RX_TMOUT));
}

//*****************************************************************************
//
// Deinitialize the UART.
//
//*****************************************************************************
void serial_interface_deinit(void)
{
    am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_TX, am_hal_gpio_pincfg_disabled);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_RX, am_hal_gpio_pincfg_disabled);

    am_hal_uart_deinitialize(g_pvUART);
}

//*****************************************************************************
//
// Read UART data.
//
//*****************************************************************************
void serial_data_read(uint8_t* pui8Data, uint32_t* ui32Length)
{
    am_hal_uart_transfer_t sRead =
    {
        .eType = AM_HAL_UART_BLOCKING_READ,
        .ui32TimeoutMs = UART_RX_TIMEOUT_MS,
        .pui8Data = pui8Data,
        .ui32NumBytes = MAX_READ_BYTES,
        .pui32BytesTransferred = ui32Length,
    };

    am_hal_uart_transfer(g_pvUART, &sRead);
}

//*****************************************************************************
//
// Write UART data.
//
//*****************************************************************************
void serial_data_write(uint8_t* pui8Data, uint32_t ui32Length)
{
    uint32_t ui32BytesTransferred = 0;
    am_hal_uart_transfer_t sWrite =
    {
        .eType = AM_HAL_UART_BLOCKING_WRITE,
        .pui8Data = pui8Data,
        .ui32NumBytes = ui32Length,
        .ui32TimeoutMs = AM_HAL_UART_WAIT_FOREVER,
        .pui32BytesTransferred = &ui32BytesTransferred,
    };

    am_hal_uart_transfer(g_pvUART, &sWrite);
}

//*****************************************************************************
//
// Enable UART IRQ.
//
//*****************************************************************************
void serial_irq_enable(void)
{
    NVIC_SetPriority((IRQn_Type)(UART0_IRQn + UART_HCI_BRIDGE), AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn + UART_HCI_BRIDGE));
}

//*****************************************************************************
//
// Disable UART IRQ.
//
//*****************************************************************************
void serial_irq_disable(void)
{
    NVIC_DisableIRQ((IRQn_Type)(UART0_IRQn + UART_HCI_BRIDGE));
}

//*****************************************************************************
//
// Serial task to adapt different interface, do nothing for UART.
//
//*****************************************************************************
void serial_task(void)
{

}

//*****************************************************************************
//
// Main
//
//*****************************************************************************
int main(void)
{
    dtm_init();
    dtm_process();
}

