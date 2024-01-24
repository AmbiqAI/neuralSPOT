//*****************************************************************************
//
//! @file uart_ble_bridge_dtm.c
//!
//! @brief Converts UART HCI commands to SPI.
//!
//! Purpose:  This example is primarily designed to enable DTM testing with the
//! Apollo3 EVB. The example accepts HCI commands over the UART at 115200 baud
//! and sends them using the BLEIF to the Apollo3 BLE Controller.  Responses from
//! the BLE Controller are accepted over the BLEIF and sent over the UART.
//!
//! Printing takes place over the ITM at 1M Baud.
//!
//
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

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "hci_api.h"
#include "ble_freertos_amota.h"

//*****************************************************************************
//
// Configuration options
//
//*****************************************************************************
//
// Define the UART module (0 or 1) to be used.
// Also define the max packet size
//
#define UART_HCI_BRIDGE                 0
#define MAX_UART_PACKET_SIZE            2048

//*****************************************************************************
//
// Custom data type.
// Note - am_uart_buffer was simply derived from the am_hal_iom_buffer macro.
//
//*****************************************************************************
#define am_uart_buffer(A)                                                 \
union                                                                     \
  {                                                                       \
      uint32_t words[(A + 3) >> 2];                                       \
      uint8_t bytes[A];                                                   \
  }

//*****************************************************************************
//
// Global Variables
//
//*****************************************************************************
uint8_t g_pui8UARTTXBuffer[MAX_UART_PACKET_SIZE];
am_uart_buffer(1024) g_psWriteData;
am_uart_buffer(1024) g_psReadData;

volatile uint32_t g_ui32UARTRxIndex = 0;
volatile bool g_bRxTimeoutFlag = false;
volatile bool g_bCmdProcessedFlag = false;

extern uint32_t g_ui32NumBytes;
extern uint32_t g_consumed_bytes;
extern void *BLE;

//*****************************************************************************
//
// Process "special" UART commands.  Format is:
//      'A'     Header
//      'M'
//      'Y'     Command (ASCII '0' - '2')
//       X      Value   (0 - 255)
//       X
//
//*****************************************************************************
void *g_pvBLEHandle;
void *g_pvUART;
uint8_t carrier_wave_mode = 0;

static void cmd_handler(uint8_t *pBuffer, uint32_t len)
{
    uint16_t value;

#ifndef AM_DEBUG_BLE_TIMING
    for ( uint32_t i = 0; i < len; i++ )
    {
        am_util_stdio_printf("%02x ", pBuffer[i]);
    }
    am_util_stdio_printf("\n");
#endif

    carrier_wave_mode = 0;

    if ( (NULL != pBuffer) && (pBuffer[0] == 0x01) && (pBuffer[1] == 0x1e) && (pBuffer[2] == 0x20) )
    {
        switch(pBuffer[6])
        {
            case 0x09:      // constant transmission mode
            {
                am_util_ble_set_constant_transmission(g_pvBLEHandle, true);  // set constant transmission
                pBuffer[6] = 0x00;
            }
            break;

            case 0x08:      // carrier wave mode
            {
                carrier_wave_mode = 1;  //set  carrier wave mode
                pBuffer[6] = 0x00;
            }
            break;

            /*added a new command for stopping carrier wave mode transmitting if needed*/
            case 0xff:
                am_util_ble_transmitter_control(g_pvBLEHandle, 0);           //disable carrier_wave_mode
                am_util_ble_set_constant_transmission(g_pvBLEHandle, false); //disable constant transmission mode

            break;

            default:
            break;
        }
    }


    //
    // Check the parameters and the UART command format.
    //
    if ( (NULL != pBuffer) && (len == 5) && (pBuffer[0] == 0x41) && (pBuffer[1] == 0x4d) )
    {
        //
        // Compute the value.
        //
        value = pBuffer[3] << 8 | pBuffer[4];

        //
        // Interpret the Command 'Y'
        //
        switch (pBuffer[2])
        {
            case 0x30:   // handle the tx power setting command.
            {
                // Check the TX power range value.
                if ((value > 0) && (value <= 0xF))
                {
                    am_hal_ble_tx_power_set(g_pvBLEHandle, (uint8_t)value);
#ifndef AM_DEBUG_BLE_TIMING
                    am_util_stdio_printf("TX Power Setting Command OK\n");
#endif
                }
                else
                {
#ifndef AM_DEBUG_BLE_TIMING
                    am_util_stdio_printf("Invalid TX Power Value %d\n", value);
#endif
                }
            }
            break;

            case 0x31:   // handle the 32MHz crystal trim setting command.
            {
                am_util_ble_crystal_trim_set(g_pvBLEHandle, value);
#ifndef AM_DEBUG_BLE_TIMING
                am_util_stdio_printf("32MHz Crystal Trim Command OK\n");
#endif
            }
            break;

            case 0x32:   // handle modulation index setting command.
            {
                am_hal_ble_transmitter_modex_set(g_pvBLEHandle, (uint8_t)value);
#ifndef AM_DEBUG_BLE_TIMING
                am_util_stdio_printf("Modulation Index Command OK\n");
#endif
            }
            break;

            case 0x33:  // handle the carrier wave output command
            {
                am_util_ble_transmitter_control_ex(g_pvBLEHandle, (uint8_t)value);
#ifndef AM_DEBUG_BLE_TIMING
                am_util_stdio_printf("generate carrier wave OK\n");
#endif
            }
            break;

            case 0x34:  // handle the continually transmitting output command
            {
                am_util_ble_set_constant_transmission_ex(g_pvBLEHandle, (uint8_t)value);
#ifndef AM_DEBUG_BLE_TIMING
                am_util_stdio_printf("generate constant moderated signal wave OK\n");
#endif
            }
            break;

            default:
            {
#ifndef AM_DEBUG_BLE_TIMING
                am_util_stdio_printf("Invalid UART Special Command %s\r\n", pBuffer);
#endif
            }
            break;

        }
        g_bCmdProcessedFlag = true;
    }
    else
    {
        g_bCmdProcessedFlag = false;
    }
}


//setting transmission mode and fix channel 1 bug in DTM mode
static void fix_trans_mode(uint8_t *recvdata)
{
    if ( carrier_wave_mode == 1 )
    {
        am_util_ble_transmitter_control(g_pvBLEHandle, 1);   //set carrier wave mode
    }
    else if (APOLLO3_A0 || APOLLO3_A1)
    {
      // intercept HCI Reset command complete event
      if ((recvdata[4] == 0x03) && (recvdata[5] == 0x0c))
      {
          am_util_ble_init_rf_channel(g_pvBLEHandle);        //fix channel 1 bug
      }
    }
}


//*****************************************************************************
//
// Interrupt handler for the UART.
//
//*****************************************************************************
#if UART_HCI_BRIDGE == 0
void am_uart_isr(void)
#else
void am_uart1_isr(void)
#endif
{
  uint32_t ui32Status;

  //
  // Read the masked interrupt status from the UART.
  //
  am_hal_uart_interrupt_status_get(g_pvUART, &ui32Status, true);
  am_hal_uart_interrupt_clear(g_pvUART, ui32Status);
  am_hal_uart_interrupt_service(g_pvUART, ui32Status, 0);

  //
  // If there's an RX interrupt, handle it in a way that preserves the
  // timeout interrupt on gaps between packets.
  //
  if (ui32Status & (AM_HAL_UART_INT_RX_TMOUT | AM_HAL_UART_INT_RX))
  {
    uint32_t ui32BytesRead;

    am_hal_uart_transfer_t sRead =
    {
      .ui32Direction = AM_HAL_UART_READ,
      .pui8Data = (uint8_t *) &(g_psWriteData.bytes[g_ui32UARTRxIndex]),
      .ui32NumBytes = 23,
      .ui32TimeoutMs = 0,
      .pui32BytesTransferred = &ui32BytesRead,
    };

    am_hal_uart_transfer(g_pvUART, &sRead);

    g_ui32UARTRxIndex += ui32BytesRead;

    //
    // If there is a TMOUT interrupt, assume we have a compete packet, and
    // send it over SPI.
    //
    if (ui32Status & (AM_HAL_UART_INT_RX_TMOUT))
    {
      NVIC_DisableIRQ((IRQn_Type)(UART0_IRQn + UART_HCI_BRIDGE));
      cmd_handler(g_psWriteData.bytes, g_ui32UARTRxIndex);
      g_bRxTimeoutFlag = true;
    }
  }
}

//*****************************************************************************
//
// EnterDtmMode: only enter once.
//
//*****************************************************************************
void ResetBleAndEnterDTM(void)
{
    //BLE_HEARTBEAT_STOP();

    //reset ble.
    //AppAdvStop();
    HciResetCmd();

    NVIC_DisableIRQ(BLE_IRQn);
    g_pvBLEHandle = BLE;
    g_ui32NumBytes   = 0;
    g_consumed_bytes = 0;

    am_hal_ble_int_clear(g_pvBLEHandle, BLEIF_INTSTAT_BLECIRQ_Msk);

    //
    // Start the UART.
    //
    am_hal_uart_config_t sUartConfig =
    {
        //
        // Standard UART settings: 115200-8-N-1
        //
        .ui32BaudRate    = 115200,
        .ui32DataBits    = AM_HAL_UART_DATA_BITS_8,
        .ui32Parity      = AM_HAL_UART_PARITY_NONE,
        .ui32StopBits    = AM_HAL_UART_ONE_STOP_BIT,
        .ui32FlowControl = AM_HAL_UART_FLOW_CTRL_NONE,

        //
        // Set TX and RX FIFOs to interrupt at three-quarters full.
        //
        .ui32FifoLevels = (AM_HAL_UART_TX_FIFO_3_4 |
                           AM_HAL_UART_RX_FIFO_3_4),

        //
        // This code will use the standard interrupt handling for UART TX, but
        // we will have a custom routine for UART RX.
        //
        .pui8TxBuffer = g_pui8UARTTXBuffer,
        .ui32TxBufferSize = sizeof(g_pui8UARTTXBuffer),
        .pui8RxBuffer = 0,
        .ui32RxBufferSize = 0,
    };

    am_hal_uart_initialize(UART_HCI_BRIDGE, &g_pvUART);
    am_hal_uart_power_control(g_pvUART, AM_HAL_SYSCTRL_WAKE, false);
    am_hal_uart_configure(g_pvUART, &sUartConfig);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_TX, g_AM_BSP_GPIO_COM_UART_TX);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_RX, g_AM_BSP_GPIO_COM_UART_RX);

    //
    // Make sure to enable the interrupts for RX, since the HAL doesn't already
    // know we intend to use them.
    //
    NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn + UART_HCI_BRIDGE));
    am_hal_uart_interrupt_enable(g_pvUART, (AM_HAL_UART_INT_RX |
                                 AM_HAL_UART_INT_RX_TMOUT));

    am_hal_interrupt_master_enable();
}

void DtmModeProc(void)
{
    uint32_t ui32NumChars;
    uint32_t ui32Status;
    uint32_t ui32IntStatus;

    //
    // Loop forever.
    //
    while (1)
    {
      //
      // Check for incoming traffic from either the UART or the BLE interface.
      //
      ui32IntStatus = am_hal_ble_int_status(g_pvBLEHandle, false);
      am_hal_ble_int_clear(g_pvBLEHandle, ui32IntStatus);

      if ( ui32IntStatus & BLEIF_INTSTAT_BLECIRQ_Msk )
      {
        //
        // If we have incoming BLE traffic, read it into a buffer.
        //
        ui32Status = am_hal_ble_blocking_hci_read(g_pvBLEHandle,
                                                  g_psReadData.words,
                                                  &ui32NumChars);

        //
        // Clocking workaround for the BLE IRQ signal.
        //
        BLEIF->BLEDBG_b.IOCLKON = 1;

        //
        // If the read was successful, echo it back out over the UART.
        //
        if ( ui32Status == AM_HAL_STATUS_SUCCESS )
        {
            if (ui32NumChars > 0)
            {
                am_hal_uart_transfer_t sWrite =
                {
                    .ui32Direction = AM_HAL_UART_WRITE,
                    .pui8Data = g_psReadData.bytes,
                    .ui32NumBytes = ui32NumChars,
                    .ui32TimeoutMs = AM_HAL_UART_WAIT_FOREVER,
                    .pui32BytesTransferred = 0,
                };

                //please enable here for transmission mode set
                fix_trans_mode(g_psReadData.bytes);

                //then send the response to UART
                am_hal_uart_transfer(g_pvUART, &sWrite);
                am_util_delay_ms(1);
            }
        }

        else
        {
            //
            // Handle the error here.
            //
            am_util_stdio_printf("Read from BLE Controller failed\n");
            while(1);
        }
      }
      else if (g_bRxTimeoutFlag)
      {
        //
        // If we have incoming UART traffic, the interrupt handler will
        // read it out for us, but we will need to echo it back out to the
        // radio manually.
        //
        if (false == g_bCmdProcessedFlag)
        {
            am_hal_ble_blocking_hci_write(g_pvBLEHandle, AM_HAL_BLE_RAW,
                                          g_psWriteData.words,
                                          g_ui32UARTRxIndex);

            //am_util_stdio_printf("\r\nWaiting response...");
            uint32_t wakeupCount = 0;

            while( !BLEIF->BSTATUS_b.BLEIRQ )
            {

                am_util_delay_ms(5);
                am_hal_ble_wakeup_set(g_pvBLEHandle, 1);
                am_util_delay_ms(5);
                am_hal_ble_wakeup_set(g_pvBLEHandle, 0);
                if ( wakeupCount++ > 1000 )
                {
                    am_util_stdio_printf("\r\n BLE controller response timeout! wakeupCount=%d,", wakeupCount);
                    while(1);
                };
            } //waiting command response
        }

        g_ui32UARTRxIndex = 0;
        g_bRxTimeoutFlag = false;
        NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn + UART_HCI_BRIDGE));
      }
    }

}
