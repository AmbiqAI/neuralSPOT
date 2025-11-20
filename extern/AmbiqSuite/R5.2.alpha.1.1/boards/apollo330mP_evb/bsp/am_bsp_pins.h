//*****************************************************************************
//
//  am_bsp_pins.h
//! @file
//!
//! @brief BSP pin configuration definitions.
//!
//! @addtogroup BSP Board Support Package (BSP)
//! @ingroup BSP
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2025, Ambiq Micro, Inc.
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
// This is part of revision release_sdk5_2_a_1_1-c2486c8ef of the AmbiqSuite Development Package.
//
//*****************************************************************************

#ifndef AM_BSP_PINS_H
#define AM_BSP_PINS_H

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// COM_UART_TX pin: This pin is COM_UART.
//
//*****************************************************************************
#define AM_BSP_GPIO_COM_UART_TX             14
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_COM_UART_TX;

//*****************************************************************************
//
// COM_UART_RX pin: This pin is COM_UART.
//
//*****************************************************************************
#define AM_BSP_GPIO_COM_UART_RX             15
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_COM_UART_RX;

//*****************************************************************************
//
// COM_UART_CTS pin: This pin is the COM_UART.
//
//*****************************************************************************
#define AM_BSP_GPIO_COM_UART_CTS            12
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_COM_UART_CTS;

//*****************************************************************************
//
// COM_UART_RTS pin: This pin is the COM_UART.
//
//*****************************************************************************
#define AM_BSP_GPIO_COM_UART_RTS            13
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_COM_UART_RTS;

//*****************************************************************************
//
// LED0 pin: The LED nearest the inside of the board (LED4) Note: Active Low.
//
//*****************************************************************************
#define AM_BSP_GPIO_LED0                    97
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_LED0;

//*****************************************************************************
//
// LED1 pin: The middle LED (LED3). Note: Active Low.
//
//*****************************************************************************
#define AM_BSP_GPIO_LED1                    23
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_LED1;

//*****************************************************************************
//
// LED2 pin: The LED nearest the outside of the board (LED2). Note: Active Low.
//
//*****************************************************************************
#define AM_BSP_GPIO_LED2                    22
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_LED2;

//*****************************************************************************
//
// BUTTON0 pin: Labeled BTN0.
//
//*****************************************************************************
#define AM_BSP_GPIO_BUTTON0                 18
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_BUTTON0;

//*****************************************************************************
//
// BUTTON1 pin: Labeled BTN1.
//
//*****************************************************************************
#define AM_BSP_GPIO_BUTTON1                 52
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_BUTTON1;

//*****************************************************************************
//
// UART0_TX pin: This pin is UART0 transmit.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART0_TX                112
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART0_TX;

//*****************************************************************************
//
// UART0_RX pin: This pin is UART0 receive.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART0_RX                7
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART0_RX;

//*****************************************************************************
//
// UART0_CTS pin: This pin is the UART0 CTS.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART0_CTS               110
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART0_CTS;

//*****************************************************************************
//
// UART0_RTS pin: This pin is the UART0 RTS.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART0_RTS               119
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART0_RTS;

//*****************************************************************************
//
// UART1_TX pin: This pin is UART1 transmit.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART1_TX                110
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART1_TX;

//*****************************************************************************
//
// UART1_RX pin: This pin is UART1 receive.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART1_RX                111
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART1_RX;

//*****************************************************************************
//
// UART1_CTS pin: This pin is the UART1 CTS.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART1_CTS               100
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART1_CTS;

//*****************************************************************************
//
// UART1_RTS pin: This pin is the UART1 RTS.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART1_RTS               17
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART1_RTS;

//*****************************************************************************
//
// IOM0_CS pin: I/O Master 0 chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM0_CS                 17
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM0_CS;
#define AM_BSP_IOM0_CS_CHNL                 0

//*****************************************************************************
//
// IOM0_MISO pin: I/O Master 0 SPI MISO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM0_MISO               7
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM0_MISO;

//*****************************************************************************
//
// IOM0_SPI_DATA pin: I/O Master 0 SPI Data I/O(3 wire) signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM0_SPI_DATA           6
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM0_SPI_DATA;

//*****************************************************************************
//
// IOM0_MOSI pin: I/O Master 0 SPI MOSI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM0_MOSI               6
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM0_MOSI;

//*****************************************************************************
//
// IOM0_SCK pin: I/O Master 0 SPI SCK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM0_SCK                5
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM0_SCK;

//*****************************************************************************
//
// IOM0_SCL pin: I/O Master 0 I2C clock signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM0_SCL                5
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM0_SCL;

//*****************************************************************************
//
// IOM0_SDA pin: I/O Master 0 I2C data signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM0_SDA                6
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM0_SDA;

//*****************************************************************************
//
// IOM1_CS pin: I/O Master 1 chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM1_CS                 24
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM1_CS;
#define AM_BSP_IOM1_CS_CHNL                 0

//*****************************************************************************
//
// IOM1_MISO pin: I/O Master 1 SPI MISO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM1_MISO               10
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM1_MISO;

//*****************************************************************************
//
// IOM1_MOSI pin: I/O Master 1 SPI MOSI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM1_MOSI               9
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM1_MOSI;

//*****************************************************************************
//
// IOM1_SPI_DATA pin: I/O Master 1 SPI Data I/O(3 wire) signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM1_SPI_DATA           9
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM1_SPI_DATA;

//*****************************************************************************
//
// IOM1_SCK pin: I/O Master 1 SPI SCK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM1_SCK                8
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM1_SCK;

//*****************************************************************************
//
// IOM1_SCL pin: I/O Master 1 I2C clock signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM1_SCL                8
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM1_SCL;

//*****************************************************************************
//
// IOM1_SDA pin: I/O Master 1 I2C data signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM1_SDA                9
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM1_SDA;

//*****************************************************************************
//
// IOM2_SCL_CB pin: I/O Master 2 I2C clock signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM2_SCL_CB             25
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM2_SCL_CB;

//*****************************************************************************
//
// IOM2_SDA_CB pin: I/O Master 2 I2C data signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM2_SDA_CB             26
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM2_SDA_CB;

//*****************************************************************************
//
// IOM4_CS pin: I/O Master 4 chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM4_CS                 106
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM4_CS;
#define AM_BSP_IOM4_CS_CHNL                 0

//*****************************************************************************
//
// IOM4_MISO_CB pin: I/O Master 4 SPI MISO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM4_MISO_CB            34
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM4_MISO_CB;

//*****************************************************************************
//
// IOM4_SPI_DATA_CB pin: I/O Master 4 SPI Data I/O(3 wire) signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM4_SPI_DATA_CB        33
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM4_SPI_DATA_CB;

//*****************************************************************************
//
// IOM4_MOSI_CB pin: I/O Master 4 SPI MOSI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM4_MOSI_CB            33
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM4_MOSI_CB;

//*****************************************************************************
//
// IOM4_SCK_CB pin: I/O Master 4 SPI SCK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM4_SCK_CB             32
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM4_SCK_CB;

//*****************************************************************************
//
// IOM4_SCL_CB pin: I/O Master 4 I2C clock signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM4_SCL_CB             32
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM4_SCL_CB;

//*****************************************************************************
//
// IOM4_SDA_CB pin: I/O Master 4 I2C data signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM4_SDA_CB             33
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM4_SDA_CB;

//*****************************************************************************
//
// UART1_TX_CB pin: This pin is UART1 transmit.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART1_TX_CB             102
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART1_TX_CB;

//*****************************************************************************
//
// UART1_RX_CB pin: This pin is UART1 receive.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART1_RX_CB             103
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART1_RX_CB;

//*****************************************************************************
//
// INT_CB pin: I/O interrupt.
//
//*****************************************************************************
#define AM_BSP_GPIO_INT_CB                  107
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_INT_CB;

//*****************************************************************************
//
// RST_CB pin: CLICKBOARD Reset.
//
//*****************************************************************************
#define AM_BSP_GPIO_RST_CB                  104
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_RST_CB;

//*****************************************************************************
//
// PDM0_CLK_CB pin: PDM 0 Clock Signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_PDM0_CLK_CB             107
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PDM0_CLK_CB;

//*****************************************************************************
//
// PDM0_DATA_CB pin: PDM 0 Data Signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_PDM0_DATA_CB            108
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PDM0_DATA_CB;

//*****************************************************************************
//
// IOM3_CS pin: I/O Master 3 chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM3_CS                 118
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM3_CS;
#define AM_BSP_IOM3_CS_CHNL                 0

//*****************************************************************************
//
// IOM3_MISO pin: I/O Master 3 SPI MISO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM3_MISO               31
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM3_MISO;

//*****************************************************************************
//
// IOM3_MOSI pin: I/O Master 3 SPI MOSI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM3_MOSI               30
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM3_MOSI;

//*****************************************************************************
//
// IOM3_SPI_DATA pin: I/O Master 3 SPI Data I/O(3 wire) signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM3_SPI_DATA           30
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM3_SPI_DATA;

//*****************************************************************************
//
// IOM3_SCK pin: I/O Master 3 SPI SCK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM3_SCK                29
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM3_SCK;

//*****************************************************************************
//
// IOM3_SCL pin: I/O Master 3 I2C clock signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM3_SCL                29
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM3_SCL;

//*****************************************************************************
//
// IOM3_SDA pin: I/O Master 3 I2C data signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM3_SDA                30
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM3_SDA;

//*****************************************************************************
//
// IOSFD0_CE pin: I/O Subordinant Full Duplex chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOSFD0_CE               3
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOSFD0_CE;

//*****************************************************************************
//
// IOSFD0_MOSI pin: I/O Subordinant Full Duplex SPI MOSI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOSFD0_MOSI             1
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOSFD0_MOSI;

//*****************************************************************************
//
// IOSFD0_MISO pin: I/O Subordinant Full Duplex SPI MISO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOSFD0_MISO             2
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOSFD0_MISO;

//*****************************************************************************
//
// IOSFD0_SCK pin: I/O Subordinant Full Duplex SPI SCK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOSFD0_SCK              0
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOSFD0_SCK;

//*****************************************************************************
//
// IOSFD0_SCL pin: I/O Subordinant I2C clock signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOSFD0_SCL              0
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOSFD0_SCL;

//*****************************************************************************
//
// IOSFD0_SDA pin: I/O Subordinant I2C data signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOSFD0_SDA              1
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOSFD0_SDA;

//*****************************************************************************
//
// IOSFD0_INT pin: I/O Subordinant Full Duplex interrupt.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOSFD0_INT              4
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOSFD0_INT;

//*****************************************************************************
//
// ITM_SWO pin: ITM Serial Wire Output.
//
//*****************************************************************************
#define AM_BSP_GPIO_ITM_SWO                 28
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_ITM_SWO;

//*****************************************************************************
//
// I2S0_DATA pin: I2S0 Bidirectional Data.
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S0_DATA               30
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S0_DATA;

//*****************************************************************************
//
// I2S0_SDOUT pin: I2S0 Bidirectional Data.
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S0_SDOUT              30
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S0_SDOUT;

//*****************************************************************************
//
// I2S0_SDIN pin: I2S0 Bidirectional Data.
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S0_SDIN               29
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S0_SDIN;

//*****************************************************************************
//
// I2S0_CLK pin: I2S0 Bit Clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S0_CLK                109
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S0_CLK;

//*****************************************************************************
//
// I2S0_WS pin: I2S0 L/R Clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S0_WS                 31
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S0_WS;

//*****************************************************************************
//
// I2S0_MCLK pin: I2S0 Master Clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S0_MCLK               118
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S0_MCLK;

//*****************************************************************************
//
// PDMI2S0_SDOUT pin: PDMI2S 0 Data Out Signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_PDMI2S0_SDOUT           5
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PDMI2S0_SDOUT;

//*****************************************************************************
//
// PDMI2S0_WS pin: PDMI2S 0 L/R Clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_PDMI2S0_WS              6
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PDMI2S0_WS;

//*****************************************************************************
//
// PDMI2S0_CLK pin: PDMI2S 0 Bit Clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_PDMI2S0_CLK             109
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PDMI2S0_CLK;

//*****************************************************************************
//
// MSPI0_CE0 pin: MSPI0 chip select for x16 AP PSRAM
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_CE0               78
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_CE0;

//*****************************************************************************
//
// MSPI0_D0 pin: MSPI0 data 0.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D0                36
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D0;

//*****************************************************************************
//
// MSPI0_D1 pin: MSPI0 data 1.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D1                37
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D1;

//*****************************************************************************
//
// MSPI0_D2 pin: MSPI0 data 2.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D2                38
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D2;

//*****************************************************************************
//
// MSPI0_D3 pin: MSPI0 data 3.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D3                39
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D3;

//*****************************************************************************
//
// MSPI0_D4 pin: MSPI0 data 4.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D4                65
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D4;

//*****************************************************************************
//
// MSPI0_D5 pin: MSPI0 data 5.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D5                66
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D5;

//*****************************************************************************
//
// MSPI0_D6 pin: MSPI0 data 6.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D6                67
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D6;

//*****************************************************************************
//
// MSPI0_D7 pin: MSPI0 data 7.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D7                68
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D7;

//*****************************************************************************
//
// MSPI0_SCK pin: MSPI0 clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_SCK               35
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_SCK;

//*****************************************************************************
//
// MSPI0_DQSDM pin: MSPI0 DQS DM.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_DQSDM             81
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_DQSDM;

//*****************************************************************************
//
// MSPI0_D8 pin: MSPI0 data 8.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D8                69
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D8;

//*****************************************************************************
//
// MSPI0_D9 pin: MSPI0 data 9.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D9                70
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D9;

//*****************************************************************************
//
// MSPI0_D10 pin: MSPI0 data 10.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D10               71
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D10;

//*****************************************************************************
//
// MSPI0_D11 pin: MSPI0 data 11.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D11               72
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D11;

//*****************************************************************************
//
// MSPI0_D12 pin: MSPI0 data 12.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D12               73
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D12;

//*****************************************************************************
//
// MSPI0_D13 pin: MSPI0 data 13.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D13               74
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D13;

//*****************************************************************************
//
// MSPI0_D14 pin: MSPI0 data 14.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D14               75
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D14;

//*****************************************************************************
//
// MSPI0_D15 pin: MSPI0 data 15.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D15               76
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D15;

//*****************************************************************************
//
// MSPI0_DQS1DM1 pin: MSPI0 DQS1 DM1.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_DQS1DM1           77
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_DQS1DM1;

//*****************************************************************************
//
// MSPI2_X4_NOR_CE0 pin: MSPI2 chip select 1
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_X4_NOR_CE0        41
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_X4_NOR_CE0;

//*****************************************************************************
//
// MSPI2_D0 pin: MSPI2 data 0.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_D0                54
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_D0;

//*****************************************************************************
//
// MSPI2_D1 pin: MSPI2 data 1.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_D1                55
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_D1;

//*****************************************************************************
//
// MSPI2_D2 pin: MSPI2 data 2.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_D2                56
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_D2;

//*****************************************************************************
//
// MSPI2_D3 pin: MSPI2 data 3.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_D3                57
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_D3;

//*****************************************************************************
//
// MSPI2_SCK pin: MSPI2 clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_SCK               53
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_SCK;

//*****************************************************************************
//
// MSPI2_DQSDM pin: MSPI2 DQS.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_DQSDM             58
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_DQSDM;

//*****************************************************************************
//
// SDIO0_DAT0 pin: SDIF0 data 0.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO0_DAT0              83
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO0_DAT0;

//*****************************************************************************
//
// SDIO0_DAT1 pin: SDIF0 data 1.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO0_DAT1              84
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO0_DAT1;

//*****************************************************************************
//
// SDIO0_DAT2 pin: SDIF0 data 2.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO0_DAT2              85
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO0_DAT2;

//*****************************************************************************
//
// SDIO0_DAT3 pin: SDIF0 data 3.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO0_DAT3              86
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO0_DAT3;

//*****************************************************************************
//
// SDIO0_DAT4 pin: eMMC0 I/F using SDIO data 4.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO0_DAT4              114
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO0_DAT4;

//*****************************************************************************
//
// SDIO0_DAT5 pin: eMMC0 I/F using SDIO data 5.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO0_DAT5              115
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO0_DAT5;

//*****************************************************************************
//
// SDIO0_DAT6 pin: eMMC0 I/F using SDIO data 6.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO0_DAT6              116
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO0_DAT6;

//*****************************************************************************
//
// SDIO0_DAT7 pin: eMMC0 I/F using SDIO data 7.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO0_DAT7              117
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO0_DAT7;

//*****************************************************************************
//
// SDIO0_CMD pin: eMMC0 I/F using SDIO CMD
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO0_CMD               87
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO0_CMD;

//*****************************************************************************
//
// SDIO0_CLK pin: eMMC0 I/F using SDIO clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO0_CLK               82
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO0_CLK;

//*****************************************************************************
//
// SDIO0_RST pin: eMMC0 I/F using GPIO for Reset.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO0_RST               27
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO0_RST;

//*****************************************************************************
//
// SDIO1_DAT0 pin: SDIO1 data 0.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO1_DAT0              59
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO1_DAT0;

//*****************************************************************************
//
// SDIO1_DAT1 pin: SDIO1 data 1.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO1_DAT1              60
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO1_DAT1;

//*****************************************************************************
//
// SDIO1_DAT2 pin: SDIO1 data 2.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO1_DAT2              61
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO1_DAT2;

//*****************************************************************************
//
// SDIO1_DAT3 pin: SDIO1 data 3.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO1_DAT3              62
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO1_DAT3;

//*****************************************************************************
//
// SDIO1_DAT4 pin: SDIO1 data 4.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO1_DAT4              43
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO1_DAT4;

//*****************************************************************************
//
// SDIO1_DAT5 pin: SDIO1 data 5.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO1_DAT5              44
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO1_DAT5;

//*****************************************************************************
//
// SDIO1_DAT6 pin: SDIO1 data 6.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO1_DAT6              45
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO1_DAT6;

//*****************************************************************************
//
// SDIO1_DAT7 pin: SDIO1 data 7.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO1_DAT7              46
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO1_DAT7;

//*****************************************************************************
//
// SDIO1_CMD pin: SDIO1 CMD
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO1_CMD               64
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO1_CMD;

//*****************************************************************************
//
// SDIO1_CLK pin: SDIO1 clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO1_CLK               63
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO1_CLK;

//*****************************************************************************
//
// SDIO1_RST pin: SDIO1 reset.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO1_RST               40
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO1_RST;

//*****************************************************************************
//
// CLKOUT pin: GPIO CLKOUT
//
//*****************************************************************************
#define AM_BSP_GPIO_CLKOUT                  90
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_CLKOUT;

//*****************************************************************************
//
// VDDUSB33_SWITCH pin: USB 3.3V Power Switch.
//
//*****************************************************************************
#define AM_BSP_GPIO_VDDUSB33_SWITCH         105
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_VDDUSB33_SWITCH;

//*****************************************************************************
//
// RXEN pin: RXEN
//
//*****************************************************************************
#define AM_BSP_GPIO_RXEN                    42
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_RXEN;

//*****************************************************************************
//
// TXEN pin: TXEN
//
//*****************************************************************************
#define AM_BSP_GPIO_TXEN                    79
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_TXEN;

#ifdef __cplusplus
}
#endif

#endif // AM_BSP_PINS_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
