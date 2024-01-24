//*****************************************************************************
//
//! @file uart_ble_bridge.c
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

#define GPIO_COM_UART_TX                22
#define GPIO_COM_UART_RX                23

const am_hal_gpio_pincfg_t g_GpioUartTxCfg =
{
    .uFuncSel            = AM_HAL_PIN_22_UART0TX,
    .eDriveStrength      = AM_HAL_GPIO_PIN_DRIVESTRENGTH_2MA
};

const am_hal_gpio_pincfg_t g_GpioUartRxCfg =
{
    .ePullup             = AM_HAL_GPIO_PIN_PULLUP_WEAK,
    .uFuncSel            = AM_HAL_PIN_23_UART0RX
};

//*****************************************************************************
//
// Custom data type.
// Note - am_uart_buffer was simply derived from the am_hal_iom_buffer macro.
//
//*****************************************************************************
#define am_uart_buffer(A)                                                   \
union                                                                   \
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
                /* stop transmitting then  power of controller and reboot systerm. */
                am_util_ble_transmitter_control(g_pvBLEHandle, 0);           //disable carrier_wave_mode
                am_util_ble_set_constant_transmission(g_pvBLEHandle, false); //disable constant transmission mode

                am_hal_ble_power_control(g_pvBLEHandle, AM_HAL_BLE_POWER_OFF);
                while ( PWRCTRL->DEVPWREN_b.PWRBLEL );
                am_hal_ble_deinitialize(g_pvBLEHandle);
                am_hal_reset_control(AM_HAL_RESET_CONTROL_SWPOI, 0);
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
                    uint32_t status;
                    status = am_hal_ble_tx_power_set(g_pvBLEHandle, (uint8_t)value);
#ifndef AM_DEBUG_BLE_TIMING
                    if (!status)
                    {
                      am_util_stdio_printf("TX Power Setting Command OK\n");
                    }
                    else
                    {
                      am_util_stdio_printf("TX Power Setting Command Failed\n");
                    }
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

#define HCI_CMD_TYPE           1
#define HCI_ACL_TYPE           2
#define HCI_CMD_HEADER_LEN     3
#define HCI_ACL_HEADER_LEN     4
#define MAX_READ_BYTES         23
//compatible for WVT case requiring send more than 256 data(eg. case hci_cfc_tc_01 may send 265bytes data)
#define HCI_MAX_RX_PACKET      512
#define UART_RX_TIMEOUT_MS     1

typedef enum
{
  HCI_RX_STATE_IDLE,
  HCI_RX_STATE_HEADER,
  HCI_RX_STATE_DATA,
  HCI_RX_STATE_COMPLETE
} hciRxState_t;

#define BYTES_TO_UINT16(n, p)     { n = ((uint16_t)(p)[0] + ((uint16_t)(p)[1] << 8)); }


/*************************************************************************************************/
/*!
 *  \fn     hciUartRxIncoming
 *
 *  \brief  Receive function.  Gets called by external code when bytes are received.
 *
 *  \param  pBuf   Pointer to buffer of incoming bytes.
 *  \param  len    Number of bytes in incoming buffer.
 *
 *  \return The number of bytes consumed.
 */
/*************************************************************************************************/
uint16_t hciUartRxIncoming(uint8_t *pBuf, uint16_t len)
{
    static uint8_t    stateRx = HCI_RX_STATE_IDLE;
    static uint8_t    pktIndRx;
    static uint16_t   iRx;
    static uint8_t    hdrRx[HCI_ACL_HEADER_LEN];
    static uint8_t    pPktRx[HCI_MAX_RX_PACKET];
    static uint8_t    *pDataRx;
    uint8_t   dataByte;
    uint16_t  consumed_bytes;

    consumed_bytes = 0;
    /* loop until all bytes of incoming buffer are handled */
    while (len)
    {
        /* read single byte from incoming buffer and advance to next byte */
        dataByte = *pBuf;

        /* --- Idle State --- */
        if (stateRx == HCI_RX_STATE_IDLE)
        {
            /* save the packet type */
            pktIndRx = dataByte;
            iRx      = 0;
            stateRx  = HCI_RX_STATE_HEADER;
            pBuf++;
            consumed_bytes++;
            len--;
        }
        /* --- Header State --- */
        else if (stateRx == HCI_RX_STATE_HEADER)
        {
            uint8_t  hdrLen = 0;
            uint16_t dataLen = 0;

            /* determine header length based on packet type */
            if (pktIndRx == HCI_CMD_TYPE)
            {
                hdrLen = HCI_CMD_HEADER_LEN;
            }
            else if (pktIndRx == HCI_ACL_TYPE)
            {
                hdrLen = HCI_ACL_HEADER_LEN;
            }
            /*-- 'A'---*/
            else if (pktIndRx == 0x41)
            {
                hdrLen = 4;
            }
            else
            {
                /* invalid packet type */
                stateRx = HCI_RX_STATE_IDLE;
                return consumed_bytes;
            }

            if (iRx != hdrLen)
            {
                /* copy current byte into the temp header buffer */
                hdrRx[iRx++] = dataByte;
                pBuf++;
                consumed_bytes++;
                len--;
            }

            /* see if entire header has been read */
            if (iRx == hdrLen)
            {
                uint8_t  i = 0;
                /* extract data length from header */
                if (pktIndRx == HCI_CMD_TYPE)
                {
                    dataLen = hdrRx[2];
                }
                else if (pktIndRx == HCI_ACL_TYPE)
                {
                    BYTES_TO_UINT16(dataLen, &hdrRx[2]);
                }

                pDataRx = pPktRx;

                /* copy header into data packet (note: memcpy is not so portable) */
                for (i = 0; i < hdrLen; i++)
                {
                    *pDataRx++ = hdrRx[i];
                }

                /* save number of bytes left to read */
                iRx = dataLen;
                if (iRx == 0)
                {
                    stateRx = HCI_RX_STATE_COMPLETE;
                }
                else
                {
                    stateRx = HCI_RX_STATE_DATA;
                }
            }
        }
        /* --- Data State --- */
        else if (stateRx == HCI_RX_STATE_DATA)
        {
            /* write incoming byte to allocated buffer */
            *pDataRx++ = dataByte;

            /* determine if entire packet has been read */
            iRx--;
            if (iRx == 0)
            {
                stateRx = HCI_RX_STATE_COMPLETE;
            }
            pBuf++;
            consumed_bytes++;
            len--;
        }

        /* --- Complete State --- */
        /* ( Note Well!  There is no else-if construct by design. ) */
        if (stateRx == HCI_RX_STATE_COMPLETE)
        {
            /* deliver data */
            if (pPktRx != NULL)
            {
                NVIC_DisableIRQ((IRQn_Type)(UART0_IRQn + UART_HCI_BRIDGE));
                g_bRxTimeoutFlag = true;
                cmd_handler(g_psWriteData.bytes, consumed_bytes);
            }

            /* reset state machine */
            stateRx = HCI_RX_STATE_IDLE;
        }
    }
    return consumed_bytes;
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
  uint8_t * pData = (uint8_t *) &(g_psWriteData.bytes[g_ui32UARTRxIndex]);

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
      .ui32NumBytes = MAX_READ_BYTES,
      .ui32TimeoutMs = UART_RX_TIMEOUT_MS,
      .pui32BytesTransferred = &ui32BytesRead,
    };

    am_hal_uart_transfer(g_pvUART, &sRead);

    hciUartRxIncoming(pData, ui32BytesRead);
    g_ui32UARTRxIndex += ui32BytesRead;
  }
}

//*****************************************************************************
//
// Main
//
//*****************************************************************************
int
main(void)
{
    uint32_t ui32NumChars;
    uint32_t ui32Status;
    uint32_t ui32IntStatus;

    //
    // Default setup.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);
    am_bsp_low_power_init();

#ifdef AM_DEBUG_BLE_TIMING
    //
    // Enable debug pins.
    //
    // 30.6 - SCLK
    // 31.6 - MISO
    // 32.6 - MOSI
    // 33.4 - CSN
    // 35.7 - SPI_STATUS
    //
    am_hal_gpio_pincfg_t pincfg = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    pincfg.uFuncSel = 6;
    am_hal_gpio_pinconfig(30, pincfg);
    am_hal_gpio_pinconfig(31, pincfg);
    am_hal_gpio_pinconfig(32, pincfg);
    pincfg.uFuncSel = 4;
    am_hal_gpio_pinconfig(33, pincfg);
    pincfg.uFuncSel = 7;
    am_hal_gpio_pinconfig(35, pincfg);
    pincfg.uFuncSel = 1;
    am_hal_gpio_pinconfig(41, pincfg);
    am_hal_gpio_pinconfig(BLE_DEBUG_TRACE_08, g_AM_HAL_GPIO_OUTPUT);
#else  // !AM_DEBUG_BLE_TIMING
    //
    // Enable the ITM
    //
    am_bsp_itm_printf_enable();
    am_util_stdio_printf("Apollo3 UART to SPI Bridge\n");
#endif // AM_DEBUG_BLE_TIMING
    //
    // Start the UART.
    //
    am_hal_uart_config_t sUartConfig =
    {
        //
        // Standard UART settings: 115200-8-N-1
        //
        .ui32BaudRate   = 115200,
        .ui32DataBits   = AM_HAL_UART_DATA_BITS_8,
        .ui32Parity     = AM_HAL_UART_PARITY_NONE,
        .ui32StopBits   = AM_HAL_UART_ONE_STOP_BIT,
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
   am_hal_gpio_pinconfig(GPIO_COM_UART_TX, g_GpioUartTxCfg);
   am_hal_gpio_pinconfig(GPIO_COM_UART_RX, g_GpioUartRxCfg);

   //
   // Make sure to enable the interrupts for RX, since the HAL doesn't already
   // know we intend to use them.
   //
   NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn + UART_HCI_BRIDGE));
   am_hal_uart_interrupt_enable(g_pvUART, (AM_HAL_UART_INT_RX |
                                AM_HAL_UART_INT_RX_TMOUT));

   am_hal_interrupt_master_enable();

    //
    // Start the BLE interface.
    //
    am_hal_ble_initialize(0, &g_pvBLEHandle);
    am_hal_ble_power_control(g_pvBLEHandle, AM_HAL_BLE_POWER_ACTIVE);
    am_hal_ble_config(g_pvBLEHandle, &am_hal_ble_default_config);

    /*delay 1s for 32768Hz clock stability*/
    am_util_delay_ms(1000);


#if (AM_PART_APOLLO3)
    if (APOLLO3_A0 || APOLLO3_A1)  //for B0 chip, don't load copy patch
    {
        am_hal_ble_default_copy_patch_apply(g_pvBLEHandle);
    }
    else if (APOLLO3_B0)
    {
        am_hal_ble_patch_preload(g_pvBLEHandle); //for B0 chip, apply preload patch
    }
#endif

#if (AM_PART_APOLLO3P)
    if (APOLLO3_B0 || APOLLO3_GE_B0)
    {
        am_hal_ble_patch_preload(g_pvBLEHandle); //for B0 chip, apply preload patch
    }
#endif


    am_hal_ble_default_trim_set_ramcode(g_pvBLEHandle);
    am_hal_ble_default_patch_apply(g_pvBLEHandle);
    am_hal_ble_patch_complete(g_pvBLEHandle);

    //
    // Setting the TX power to the highest power value.
    //
    am_hal_ble_tx_power_set(g_pvBLEHandle, 0xf);

    am_hal_ble_int_clear(g_pvBLEHandle, BLEIF_INTSTAT_BLECIRQ_Msk);

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
                     if (wakeupCount++ >1000)
                     {
                        am_util_stdio_printf("\r\n BLE controller response timeout! wakeupCount=%d,", wakeupCount);
                        //while(1); // the infinite loop is for debug, comment out it for real use case
                     };

                  } //waiting command response



            }

            g_ui32UARTRxIndex = 0;
            g_bRxTimeoutFlag = false;
            NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn + UART_HCI_BRIDGE));
        }
    }
}
