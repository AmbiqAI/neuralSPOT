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
// This is part of revision release_sdk5p2-040c7863bb of the AmbiqSuite Development Package.
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
// COM_UART_TX pin: This pin is COM_UART and FTDI UART0 transmit.
//
//*****************************************************************************
#define AM_BSP_GPIO_COM_UART_TX             12
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_COM_UART_TX;

//*****************************************************************************
//
// COM_UART_RX pin: This pin is COM_UART and FTDI UART0 receive.
//
//*****************************************************************************
#define AM_BSP_GPIO_COM_UART_RX             14
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_COM_UART_RX;

//*****************************************************************************
//
// COM_UART_CTS pin: This pin is the COM_UART and FTDI UART0 CTS.
//
//*****************************************************************************
#define AM_BSP_GPIO_COM_UART_CTS            18
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_COM_UART_CTS;

//*****************************************************************************
//
// COM_UART_RTS pin: This pin is the COM_UART and FTDI UART0 RTS.
//
//*****************************************************************************
#define AM_BSP_GPIO_COM_UART_RTS            57
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_COM_UART_RTS;

//*****************************************************************************
//
// UART0_TX pin: This pin is UART0 transmit.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART0_TX                0
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART0_TX;

//*****************************************************************************
//
// UART0_RX pin: This pin is UART0 receive.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART0_RX                2
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART0_RX;

//*****************************************************************************
//
// UART0_CTS pin: This pin is the UART0 CTS.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART0_CTS               6
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART0_CTS;

//*****************************************************************************
//
// UART0_RTS pin: This pin is the UART0 RTS.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART0_RTS               4
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART0_RTS;

//*****************************************************************************
//
// UART0_FD_RX pin: This pin is UART0 receive.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART0_FD_RX             62
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART0_FD_RX;

//*****************************************************************************
//
// UART0_FD_RTS pin: This pin is the UART0 RTS.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART0_FD_RTS            58
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART0_FD_RTS;

//*****************************************************************************
//
// UART1_TX pin: This pin is UART1 transmit.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART1_TX                0
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART1_TX;

//*****************************************************************************
//
// UART1_RX pin: This pin is UART1 receive.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART1_RX                2
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART1_RX;

//*****************************************************************************
//
// UART1_CTS pin: This pin is the UART1 CTS.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART1_CTS               6
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART1_CTS;

//*****************************************************************************
//
// UART1_RTS pin: This pin is the UART1 RTS.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART1_RTS               4
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART1_RTS;

//*****************************************************************************
//
// UART3_TX pin: This pin is UART3 transmit.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART3_TX                61
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART3_TX;

//*****************************************************************************
//
// UART3_RX pin: This pin is UART3 receive.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART3_RX                63
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART3_RX;

//*****************************************************************************
//
// UART3_CTS pin: This pin is the UART3 CTS.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART3_CTS               60
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART3_CTS;

//*****************************************************************************
//
// UART3_RTS pin: This pin is the UART3 RTS.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART3_RTS               71
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART3_RTS;

//*****************************************************************************
//
// IOM0_CS pin: I/O Master 0 chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM0_CS                 50
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
#define AM_BSP_GPIO_IOM1_CS                 51
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
// IOM2_CS pin: I/O Master 2 chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM2_CS                 11
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM2_CS;
#define AM_BSP_IOM2_CS_CHNL                 0

//*****************************************************************************
//
// IOM2_MISO pin: I/O Master 2 SPI MISO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM2_MISO               27
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM2_MISO;

//*****************************************************************************
//
// IOM2_MOSI pin: I/O Master 2 SPI MOSI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM2_MOSI               26
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM2_MOSI;

//*****************************************************************************
//
// IOM2_SCK pin: I/O Master 2 SPI SCK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM2_SCK                25
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM2_SCK;

//*****************************************************************************
//
// IOM2_SCL pin: I/O Master 2 I2C clock signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM2_SCL                25
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM2_SCL;

//*****************************************************************************
//
// IOM2_SDA pin: I/O Master 2 I2C data signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM2_SDA                26
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM2_SDA;

//*****************************************************************************
//
// IOM3_CS pin: I/O Master 3 chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM3_CS                 13
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM3_CS;
#define AM_BSP_IOM3_CS_CHNL                 0

//*****************************************************************************
//
// IOM3_MISO pin: I/O Master 3 SPI MISO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM3_MISO               33
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM3_MISO;

//*****************************************************************************
//
// IOM3_MOSI pin: I/O Master 3 SPI MOSI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM3_MOSI               32
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM3_MOSI;

//*****************************************************************************
//
// IOM3_SCK pin: I/O Master 3 SPI SCK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM3_SCK                31
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM3_SCK;

//*****************************************************************************
//
// IOM3_SCL pin: I/O Master 3 I2C clock signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM3_SCL                31
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM3_SCL;

//*****************************************************************************
//
// IOM3_SDA pin: I/O Master 3 I2C data signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM3_SDA                32
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM3_SDA;

//*****************************************************************************
//
// IOM4_CS pin: I/O Master 4 chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM4_CS                 16
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM4_CS;
#define AM_BSP_IOM4_CS_CHNL                 0

//*****************************************************************************
//
// IOM4_MISO pin: I/O Master 4 SPI MISO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM4_MISO               36
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM4_MISO;

//*****************************************************************************
//
// IOM4_MOSI pin: I/O Master 4 SPI MOSI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM4_MOSI               35
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM4_MOSI;

//*****************************************************************************
//
// IOM4_SCK pin: I/O Master 4 SPI SCK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM4_SCK                34
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM4_SCK;

//*****************************************************************************
//
// IOM4_SCL pin: I/O Master 4 I2C clock signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM4_SCL                34
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM4_SCL;

//*****************************************************************************
//
// IOM4_SDA pin: I/O Master 4 I2C data signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM4_SDA                35
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM4_SDA;

//*****************************************************************************
//
// IOM5_CS pin: I/O Master 5 chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM5_CS                 17
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM5_CS;
#define AM_BSP_IOM5_CS_CHNL                 0

//*****************************************************************************
//
// IOM5_MISO pin: I/O Master 5 SPI MISO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM5_MISO               49
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM5_MISO;

//*****************************************************************************
//
// IOM5_MOSI pin: I/O Master 5 SPI MOSI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM5_MOSI               48
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM5_MOSI;

//*****************************************************************************
//
// IOM5_SCK pin: I/O Master 5 SPI SCK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM5_SCK                47
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM5_SCK;

//*****************************************************************************
//
// IOM5_SCL pin: I/O Master 5 I2C clock signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM5_SCL                47
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM5_SCL;

//*****************************************************************************
//
// IOM5_SDA pin: I/O Master 5 I2C data signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM5_SDA                48
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM5_SDA;

//*****************************************************************************
//
// IOM6_CS pin: I/O Master 6 chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM6_CS                 117
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM6_CS;
#define AM_BSP_IOM6_CS_CHNL                 0

//*****************************************************************************
//
// IOM6_MISO pin: I/O Master 6 SPI MISO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM6_MISO               63
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM6_MISO;

//*****************************************************************************
//
// IOM6_MOSI pin: I/O Master 6 SPI MOSI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM6_MOSI               62
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM6_MOSI;

//*****************************************************************************
//
// IOM6_SCK pin: I/O Master 6 SPI SCK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM6_SCK                61
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM6_SCK;

//*****************************************************************************
//
// IOM6_SCL pin: I/O Master 6 I2C clock signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM6_SCL                61
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM6_SCL;

//*****************************************************************************
//
// IOM6_SDA pin: I/O Master 6 I2C data signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM6_SDA                62
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM6_SDA;

//*****************************************************************************
//
// IOM7_CS pin: I/O Master 7 chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM7_CS                 19
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM7_CS;
#define AM_BSP_IOM7_CS_CHNL                 0

//*****************************************************************************
//
// IOM7_MISO pin: I/O Master 7 SPI MISO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM7_MISO               24
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM7_MISO;

//*****************************************************************************
//
// IOM7_MOSI pin: I/O Master 7 SPI MOSI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM7_MOSI               23
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM7_MOSI;

//*****************************************************************************
//
// IOM7_SCK pin: I/O Master 7 SPI SCK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM7_SCK                22
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM7_SCK;

//*****************************************************************************
//
// IOM7_SCL pin: I/O Master 7 I2C clock signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM7_SCL                22
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM7_SCL;

//*****************************************************************************
//
// IOM7_SDA pin: I/O Master 7 I2C data signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM7_SDA                23
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOM7_SDA;

//*****************************************************************************
//
// IOSFD0_CE pin: I/O Slave Full Duplex chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOSFD0_CE               3
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOSFD0_CE;

//*****************************************************************************
//
// IOSFD0_MOSI pin: I/O Slave Full Duplex SPI MOSI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOSFD0_MOSI             1
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOSFD0_MOSI;

//*****************************************************************************
//
// IOSFD0_MISO pin: I/O Slave Full Duplex SPI MISO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOSFD0_MISO             2
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOSFD0_MISO;

//*****************************************************************************
//
// IOSFD0_SCK pin: I/O Slave Full Duplex SPI SCK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOSFD0_SCK              0
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOSFD0_SCK;

//*****************************************************************************
//
// IOSFD0_INT pin: I/O Slave Full Duplex interrupt.
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
#define AM_BSP_GPIO_I2S0_DATA               32
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S0_DATA;

//*****************************************************************************
//
// I2S0_SDOUT pin: I2S0 Bidirectional Data.
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S0_SDOUT              32
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S0_SDOUT;

//*****************************************************************************
//
// I2S0_SDIN pin: I2S0 Bidirectional Data.
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S0_SDIN               4
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S0_SDIN;

//*****************************************************************************
//
// I2S0_CLK pin: I2S0 Bit Clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S0_CLK                31
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S0_CLK;

//*****************************************************************************
//
// I2S0_WS pin: I2S0 L/R Clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S0_WS                 33
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S0_WS;

//*****************************************************************************
//
// I2S1_DATA pin: I2S1 Bidirectional Data.
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S1_DATA               35
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S1_DATA;

//*****************************************************************************
//
// I2S1_SDOUT pin: I2S1 Data Out
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S1_SDOUT              35
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S1_SDOUT;

//*****************************************************************************
//
// I2S1_SDIN pin: I2S1 Data In
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S1_SDIN               3
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S1_SDIN;

//*****************************************************************************
//
// I2S1_CLK pin: I2S1 Bit Clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S1_CLK                34
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S1_CLK;

//*****************************************************************************
//
// I2S1_WS pin: I2S1 L/R Clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S1_WS                 36
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S1_WS;

//*****************************************************************************
//
// PDM0_CLK pin: PDM 0 Clock Signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_PDM0_CLK                50
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PDM0_CLK;

//*****************************************************************************
//
// PDM0_DATA pin: PDM 0 Data Signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_PDM0_DATA               51
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PDM0_DATA;

//*****************************************************************************
//
// MSPI0_CE0 pin: MSPI0 chip select for x16 AP PSRAM
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_CE0               199
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_CE0;

//*****************************************************************************
//
// MSPI0_D0 pin: MSPI0 data 0.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D0                64
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D0;

//*****************************************************************************
//
// MSPI0_D1 pin: MSPI0 data 1.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D1                65
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D1;

//*****************************************************************************
//
// MSPI0_D2 pin: MSPI0 data 2.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D2                66
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D2;

//*****************************************************************************
//
// MSPI0_D3 pin: MSPI0 data 3.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D3                67
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D3;

//*****************************************************************************
//
// MSPI0_D4 pin: MSPI0 data 4.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D4                68
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D4;

//*****************************************************************************
//
// MSPI0_D5 pin: MSPI0 data 5.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D5                69
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D5;

//*****************************************************************************
//
// MSPI0_D6 pin: MSPI0 data 6.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D6                70
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D6;

//*****************************************************************************
//
// MSPI0_D7 pin: MSPI0 data 7.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D7                71
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D7;

//*****************************************************************************
//
// MSPI0_SCK pin: MSPI0 clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_SCK               72
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_SCK;

//*****************************************************************************
//
// MSPI0_DQSDM pin: MSPI0 DQS DM.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_DQSDM             73
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_DQSDM;

//*****************************************************************************
//
// MSPI0_D8 pin: MSPI0 data 8.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D8                37
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D8;

//*****************************************************************************
//
// MSPI0_D9 pin: MSPI0 data 9.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D9                38
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D9;

//*****************************************************************************
//
// MSPI0_D10 pin: MSPI0 data 10.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D10               39
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D10;

//*****************************************************************************
//
// MSPI0_D11 pin: MSPI0 data 11.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D11               40
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D11;

//*****************************************************************************
//
// MSPI0_D12 pin: MSPI0 data 12.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D12               41
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D12;

//*****************************************************************************
//
// MSPI0_D13 pin: MSPI0 data 13.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D13               42
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D13;

//*****************************************************************************
//
// MSPI0_D14 pin: MSPI0 data 14.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D14               43
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D14;

//*****************************************************************************
//
// MSPI0_D15 pin: MSPI0 data 15.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D15               44
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D15;

//*****************************************************************************
//
// MSPI0_DQS1DM1 pin: MSPI0 DQS1 DM1.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_DQS1DM1           45
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_DQS1DM1;

//*****************************************************************************
//
// MSPI0_D4_CLK pin: MSPI0 data 4 replace MSPI0_8(CLK).
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_D4_CLK            68
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_D4_CLK;

//*****************************************************************************
//
// MSPI0_RST pin: Dummy MSPI0 Reset.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_RST               147
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_RST;

//*****************************************************************************
//
// MSPI1_CE0 pin: MSPI1 chip select 0
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI1_CE0               49
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI1_CE0;

//*****************************************************************************
//
// MSPI1_D0 pin: Octal Flash I/F using MSPI1 data 0.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI1_D0                95
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI1_D0;

//*****************************************************************************
//
// MSPI1_D1 pin: Octal Flash I/F using MSPI1 data 1.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI1_D1                96
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI1_D1;

//*****************************************************************************
//
// MSPI1_D2 pin: Octal Flash I/F using MSPI1 data 2.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI1_D2                97
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI1_D2;

//*****************************************************************************
//
// MSPI1_D3 pin: Octal Flash I/F using MSPI1 data 3.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI1_D3                98
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI1_D3;

//*****************************************************************************
//
// MSPI1_D4 pin: Octal Flash I/F using MSPI1 data 4.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI1_D4                99
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI1_D4;

//*****************************************************************************
//
// MSPI1_D5 pin: Octal Flash I/F using MSPI1 data 5.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI1_D5                100
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI1_D5;

//*****************************************************************************
//
// MSPI1_D6 pin: Octal Flash I/F using MSPI1 data 6.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI1_D6                101
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI1_D6;

//*****************************************************************************
//
// MSPI1_D7 pin: Octal Flash I/F using MSPI1 data 7.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI1_D7                102
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI1_D7;

//*****************************************************************************
//
// MSPI1_SCK pin: Octal Flash I/F using MSPI1 clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI1_SCK               103
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI1_SCK;

//*****************************************************************************
//
// MSPI1_DQSDM pin: Octal Flash I/F using MSPI1 DQS.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI1_DQSDM             104
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI1_DQSDM;

//*****************************************************************************
//
// MSPI1_RST pin: Octal Flash Reset.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI1_RST               17
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI1_RST;

//*****************************************************************************
//
// MSPI1_D4_CLK pin: MSPI1 data 4 replace MSPI1_8(CLK).
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI1_D4_CLK            99
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI1_D4_CLK;

//*****************************************************************************
//
// MSPI1_LS_EN pin: MSPI1 Load Switch Control.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI1_LS_EN             200
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI1_LS_EN;

//*****************************************************************************
//
// SDIO0_DAT0 pin: SDIF0 data 0.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO0_DAT0              84
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO0_DAT0;

//*****************************************************************************
//
// SDIO0_DAT1 pin: SDIF0 data 1.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO0_DAT1              85
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO0_DAT1;

//*****************************************************************************
//
// SDIO0_DAT2 pin: SDIF0 data 2.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO0_DAT2              86
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO0_DAT2;

//*****************************************************************************
//
// SDIO0_DAT3 pin: SDIF0 data 3.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO0_DAT3              87
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO0_DAT3;

//*****************************************************************************
//
// SDIO0_DAT4 pin: eMMC0 I/F using SDIO data 4.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO0_DAT4              156
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO0_DAT4;

//*****************************************************************************
//
// SDIO0_DAT5 pin: eMMC0 I/F using SDIO data 5.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO0_DAT5              157
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO0_DAT5;

//*****************************************************************************
//
// SDIO0_DAT6 pin: eMMC0 I/F using SDIO data 6.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO0_DAT6              158
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO0_DAT6;

//*****************************************************************************
//
// SDIO0_DAT7 pin: eMMC0 I/F using SDIO data 7.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO0_DAT7              159
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO0_DAT7;

//*****************************************************************************
//
// SDIO0_CMD pin: eMMC0 I/F using SDIO CMD
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO0_CMD               160
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO0_CMD;

//*****************************************************************************
//
// SDIO0_CLK pin: eMMC0 I/F using SDIO clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO0_CLK               88
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO0_CLK;

//*****************************************************************************
//
// SDIO0_RST pin: eMMC0 I/F using GPIO for Reset.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO0_RST               11
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO0_RST;

//*****************************************************************************
//
// SDIO1_DAT0 pin: SDIF1 data 0.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO1_DAT0              125
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO1_DAT0;

//*****************************************************************************
//
// SDIO1_DAT1 pin: SDIF1 data 1.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO1_DAT1              126
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO1_DAT1;

//*****************************************************************************
//
// SDIO1_DAT2 pin: SDIF1 data 2.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO1_DAT2              127
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO1_DAT2;

//*****************************************************************************
//
// SDIO1_DAT3 pin: SDIF1 data 3.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO1_DAT3              128
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO1_DAT3;

//*****************************************************************************
//
// SDIO1_CMD pin: eMMC1 I/F using SDIO CMD
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO1_CMD               134
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO1_CMD;

//*****************************************************************************
//
// SDIO1_CLK pin: eMMC1 I/F using SDIO clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO1_CLK               129
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO1_CLK;

//*****************************************************************************
//
// SDIO1_RST pin: eMMC1 I/F using GPIO for Reset.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO1_RST               93
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO1_RST;

//*****************************************************************************
//
// SDIO1_CD pin: SD Card using GPIO for card detection.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO1_CD                27
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO1_CD;

//*****************************************************************************
//
// SDIO1_WP pin: SD Card using GPIO for write protection.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO1_WP                26
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO1_WP;

//*****************************************************************************
//
// SD_LEVEL_SHIFT_SEL pin: SD Card using GPIO for switching supply voltage.
//
//*****************************************************************************
#define AM_BSP_GPIO_SD_LEVEL_SHIFT_SEL      25
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SD_LEVEL_SHIFT_SEL;

//*****************************************************************************
//
// SD_POWER_CTRL pin: SD Card using GPIO for power cycle or power on/off.
//
//*****************************************************************************
#define AM_BSP_GPIO_SD_POWER_CTRL           48
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SD_POWER_CTRL;

//*****************************************************************************
//
// VCCQ_EMMC0 pin: SDIO Power Control Switch VCCQ EMMC
//
//*****************************************************************************
#define AM_BSP_GPIO_VCCQ_EMMC0              220
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_VCCQ_EMMC0;

//*****************************************************************************
//
// VCC_EMMC0 pin: SDIO Power Control Switch VCC EMMC
//
//*****************************************************************************
#define AM_BSP_GPIO_VCC_EMMC0               221
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_VCC_EMMC0;

//*****************************************************************************
//
// VCCQ_EMMC1 pin: SDIO Power Control Switch VCCQ EMMC
//
//*****************************************************************************
#define AM_BSP_GPIO_VCCQ_EMMC1              222
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_VCCQ_EMMC1;

//*****************************************************************************
//
// VCC_EMMC1 pin: SDIO Power Control Switch VCC EMMC
//
//*****************************************************************************
#define AM_BSP_GPIO_VCC_EMMC1               223
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_VCC_EMMC1;

//*****************************************************************************
//
// M1_DISP_SPI_SD pin: Display SPI Data In/Out (3-wire).
//
//*****************************************************************************
#define AM_BSP_GPIO_M1_DISP_SPI_SD          100
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M1_DISP_SPI_SD;

//*****************************************************************************
//
// M1_DISP_SPI_SDO pin: Display SPI Data Out (4-wire)
//
//*****************************************************************************
#define AM_BSP_GPIO_M1_DISP_SPI_SDO         100
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M1_DISP_SPI_SDO;

//*****************************************************************************
//
// M1_DISP_SDI pin: Display SPI Data In (4-wire).
//
//*****************************************************************************
#define AM_BSP_GPIO_M1_DISP_SDI             103
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M1_DISP_SDI;

//*****************************************************************************
//
// M1_DISP_SPI_DCX pin: Display SPI DCx.
//
//*****************************************************************************
#define AM_BSP_GPIO_M1_DISP_SPI_DCX         101
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M1_DISP_SPI_DCX;

//*****************************************************************************
//
// M1_DISP_SPI_SCK pin: Display SPI Clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_M1_DISP_SPI_SCK         102
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M1_DISP_SPI_SCK;

//*****************************************************************************
//
// M1_DISP_SPI_CS pin: Display SPI CS.
//
//*****************************************************************************
#define AM_BSP_GPIO_M1_DISP_SPI_CS          51
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M1_DISP_SPI_CS;

//*****************************************************************************
//
// M1_DISP_DEVICE_EN pin: Display Device Enable (VCI_EN).
//
//*****************************************************************************
#define AM_BSP_GPIO_M1_DISP_DEVICE_EN       94
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M1_DISP_DEVICE_EN;

//*****************************************************************************
//
// M1_DISP_SPI_RES pin: Display SPI Reset.
//
//*****************************************************************************
#define AM_BSP_GPIO_M1_DISP_SPI_RES         92
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M1_DISP_SPI_RES;

//*****************************************************************************
//
// M1_DISP_SPI_TE pin: Display SPI TE.
//
//*****************************************************************************
#define AM_BSP_GPIO_M1_DISP_SPI_TE          50
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M1_DISP_SPI_TE;

//*****************************************************************************
//
// M1_DISP_QSPI_D0 pin: Display QSPI Data Out 0.
//
//*****************************************************************************
#define AM_BSP_GPIO_M1_DISP_QSPI_D0         79
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M1_DISP_QSPI_D0;

//*****************************************************************************
//
// M1_DISP_QSPI_D1 pin: Display QSPI Data Out 1.
//
//*****************************************************************************
#define AM_BSP_GPIO_M1_DISP_QSPI_D1         80
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M1_DISP_QSPI_D1;

//*****************************************************************************
//
// M1_DISP_QSPI_D2 pin: Display QSPI Data Out 2.
//
//*****************************************************************************
#define AM_BSP_GPIO_M1_DISP_QSPI_D2         82
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M1_DISP_QSPI_D2;

//*****************************************************************************
//
// M1_DISP_QSPI_D3 pin: Display QSPI Data Out 3.
//
//*****************************************************************************
#define AM_BSP_GPIO_M1_DISP_QSPI_D3         83
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M1_DISP_QSPI_D3;

//*****************************************************************************
//
// M1_DISP_QSPI_SCK pin: Display QSPI Clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_M1_DISP_QSPI_SCK        81
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M1_DISP_QSPI_SCK;

//*****************************************************************************
//
// M1_DISP_QSPI_CS pin: Display CSX chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_M1_DISP_QSPI_CS         51
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M1_DISP_QSPI_CS;

//*****************************************************************************
//
// DISP_VDDIO_EN pin: Display VDDIO enable.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_VDDIO_EN           92
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_VDDIO_EN;

//*****************************************************************************
//
// DISP_PWR_EN pin: Display PWR enable.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_PWR_EN             10
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_PWR_EN;

//*****************************************************************************
//
// DISP_SWIRE pin: Swire control pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_SWIRE              8
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_SWIRE;

//*****************************************************************************
//
// DISP_RST pin: Display reset.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_RST                7
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_RST;

//*****************************************************************************
//
// DISP_DSI_TE pin: Display DSI TE.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_DSI_TE             50
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_DSI_TE;

//*****************************************************************************
//
// TOUCH_ALS_M0SCL pin: Touch ALS I/O Master 0 I2C clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_TOUCH_ALS_M0SCL         5
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_TOUCH_ALS_M0SCL;

//*****************************************************************************
//
// TOUCH_ALS_M0SDA pin: Touch ALS I/O Master 0 I2C data.
//
//*****************************************************************************
#define AM_BSP_GPIO_TOUCH_ALS_M0SDA         6
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_TOUCH_ALS_M0SDA;

//*****************************************************************************
//
// TOUCH_INT pin: Touch panel interrupt.
//
//*****************************************************************************
#define AM_BSP_GPIO_TOUCH_INT               16
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_TOUCH_INT;

//*****************************************************************************
//
// ALS_INT pin: DISP ALS INT
//
//*****************************************************************************
#define AM_BSP_GPIO_ALS_INT                 17
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_ALS_INT;

//*****************************************************************************
//
// TOUCH_RST pin: Touch panel reset
//
//*****************************************************************************
#define AM_BSP_GPIO_TOUCH_RST               91
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_TOUCH_RST;

//*****************************************************************************
//
// DISP_IFSEL1 pin: Display Interface Select 1
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_IFSEL1             57
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_IFSEL1;

//*****************************************************************************
//
// DISP_IFSEL2 pin: Display Interface Select 2
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_IFSEL2             90
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_IFSEL2;

//*****************************************************************************
//
// DISP_DEVICE_RST pin: Display Device Reset.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_DEVICE_RST         92
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_DEVICE_RST;

//*****************************************************************************
//
// DISP_CARD_VERSION pin: Display card version check.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_CARD_VERSION       10
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_CARD_VERSION;

//*****************************************************************************
//
// VDD18_SWITCH pin: MIPI 1.8V Power Switch.
//
//*****************************************************************************
#define AM_BSP_GPIO_VDD18_SWITCH            83
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_VDD18_SWITCH;

//*****************************************************************************
//
// VDDUSB33_SWITCH pin: USB 3.3V Power Switch.
//
//*****************************************************************************
#define AM_BSP_GPIO_VDDUSB33_SWITCH         29
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_VDDUSB33_SWITCH;

//*****************************************************************************
//
// VDDUSB0P9_SWITCH pin: USB 0.9V Power Switch.
//
//*****************************************************************************
#define AM_BSP_GPIO_VDDUSB0P9_SWITCH        46
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_VDDUSB0P9_SWITCH;

//*****************************************************************************
//
// EM9305_CS pin: I/O Master 6 chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_EM9305_CS               149
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_EM9305_CS;
#define AM_BSP_EM9305_CS_CHNL               0

//*****************************************************************************
//
// EM9305_INT pin: EM9305 interrupt.
//
//*****************************************************************************
#define AM_BSP_GPIO_EM9305_INT              117
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_EM9305_INT;

//*****************************************************************************
//
// AP5_12M_CLKREQ pin: EM9305 Clock Request
//
//*****************************************************************************
#define AM_BSP_GPIO_AP5_12M_CLKREQ          136
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_AP5_12M_CLKREQ;

//*****************************************************************************
//
// EM9305_32K_CLK pin: EM9305 32k Clock Export
//
//*****************************************************************************
#define AM_BSP_GPIO_EM9305_32K_CLK          138
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_EM9305_32K_CLK;

//*****************************************************************************
//
// EM9305_CM pin: EM9305 CM Enable
//
//*****************************************************************************
#define AM_BSP_GPIO_EM9305_CM               15
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_EM9305_CM;

//*****************************************************************************
//
// EM9305_EN pin: EM9305 Enable Pin
//
//*****************************************************************************
#define AM_BSP_GPIO_EM9305_EN               93
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_EM9305_EN;

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
