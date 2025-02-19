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
#define AM_BSP_GPIO_COM_UART_TX             30
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_COM_UART_TX;

//*****************************************************************************
//
// COM_UART_RX pin: This pin is COM_UART and FTDI UART0 receive.
//
//*****************************************************************************
#define AM_BSP_GPIO_COM_UART_RX             55
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_COM_UART_RX;

//*****************************************************************************
//
// COM_UART_CTS pin: This pin is the COM_UART and FTDI UART0 CTS.
//
//*****************************************************************************
#define AM_BSP_GPIO_COM_UART_CTS            59
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
#define AM_BSP_GPIO_IOM0_CS                 60
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
#define AM_BSP_GPIO_IOM1_CS                 92
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
#define AM_BSP_GPIO_IOM2_CS                 58
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
#define AM_BSP_GPIO_IOM3_CS                 29
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
#define AM_BSP_GPIO_IOM4_CS                 139
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
#define AM_BSP_GPIO_IOM5_CS                 93
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
#define AM_BSP_GPIO_IOM6_CS                 89
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
#define AM_BSP_GPIO_IOM7_CS                 94
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
// IOS_CE pin: I/O Slave chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOS_CE                  13
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOS_CE;

//*****************************************************************************
//
// IOS_MISO pin: I/O Slave SPI MISO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOS_MISO                83
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOS_MISO;

//*****************************************************************************
//
// IOS_MOSI pin: I/O Slave SPI MOSI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOS_MOSI                52
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOS_MOSI;

//*****************************************************************************
//
// IOS_SCK pin: I/O Slave SPI SCK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOS_SCK                 11
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOS_SCK;

//*****************************************************************************
//
// IOS_SCL pin: I/O Slave I2C clock signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOS_SCL                 11
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOS_SCL;

//*****************************************************************************
//
// IOS_SDA pin: I/O Slave I2C data signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOS_SDA                 52
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOS_SDA;

//*****************************************************************************
//
// IOS_INT pin: I/O Slave interrupt.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOS_INT                 162
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOS_INT;

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
// IOSFD1_CE pin: I/O Slave Full Duplex chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOSFD1_CE               3
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOSFD1_CE;

//*****************************************************************************
//
// IOSFD1_MOSI pin: I/O Slave Full Duplex SPI MOSI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOSFD1_MOSI             1
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOSFD1_MOSI;

//*****************************************************************************
//
// IOSFD1_MISO pin: I/O Slave Full Duplex SPI MISO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOSFD1_MISO             2
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOSFD1_MISO;

//*****************************************************************************
//
// IOSFD1_SCK pin: I/O Slave Full Duplex SPI SCK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOSFD1_SCK              0
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOSFD1_SCK;

//*****************************************************************************
//
// IOSFD1_INT pin: I/O Slave Full Duplex interrupt.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOSFD1_INT              4
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOSFD1_INT;

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
#define AM_BSP_GPIO_I2S0_DATA               6
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S0_DATA;

//*****************************************************************************
//
// I2S0_SDOUT pin: I2S0 Bidirectional Data.
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S0_SDOUT              6
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S0_SDOUT;

//*****************************************************************************
//
// I2S0_SDIN pin: I2S0 Bidirectional Data.
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S0_SDIN               14
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S0_SDIN;

//*****************************************************************************
//
// I2S0_CLK pin: I2S0 Bit Clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S0_CLK                5
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S0_CLK;

//*****************************************************************************
//
// I2S0_WS pin: I2S0 L/R Clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S0_WS                 7
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S0_WS;

//*****************************************************************************
//
// I2S1_DATA pin: I2S1 Bidirectional Data.
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S1_DATA               17
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S1_DATA;

//*****************************************************************************
//
// I2S1_SDOUT pin: I2S1 Data Out
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S1_SDOUT              17
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S1_SDOUT;

//*****************************************************************************
//
// I2S1_SDIN pin: I2S1 Data In
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S1_SDIN               19
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S1_SDIN;

//*****************************************************************************
//
// I2S1_CLK pin: I2S1 Bit Clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S1_CLK                16
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S1_CLK;

//*****************************************************************************
//
// I2S1_WS pin: I2S1 L/R Clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S1_WS                 18
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
// MSPI0_RST pin: MSPI0 Reset.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI0_RST               215
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI0_RST;

//*****************************************************************************
//
// MSPI3_CE0 pin: MSPI3 chip select for x16 AP PSRAM
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI3_CE0               114
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI3_CE0;

//*****************************************************************************
//
// MSPI3_D0 pin: MSPI3 data 0.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI3_D0                115
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI3_D0;

//*****************************************************************************
//
// MSPI3_D1 pin: MSPI3 data 1.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI3_D1                116
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI3_D1;

//*****************************************************************************
//
// MSPI3_D2 pin: MSPI3 data 2.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI3_D2                117
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI3_D2;

//*****************************************************************************
//
// MSPI3_D3 pin: MSPI3 data 3.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI3_D3                118
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI3_D3;

//*****************************************************************************
//
// MSPI3_D4 pin: MSPI3 data 4.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI3_D4                119
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI3_D4;

//*****************************************************************************
//
// MSPI3_D5 pin: MSPI3 data 5.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI3_D5                120
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI3_D5;

//*****************************************************************************
//
// MSPI3_D6 pin: MSPI3 data 6.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI3_D6                121
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI3_D6;

//*****************************************************************************
//
// MSPI3_D7 pin: MSPI3 data 7.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI3_D7                122
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI3_D7;

//*****************************************************************************
//
// MSPI3_SCK pin: MSPI3 clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI3_SCK               123
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI3_SCK;

//*****************************************************************************
//
// MSPI3_DQSDM pin: MSPI3 DQS DM.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI3_DQSDM             124
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI3_DQSDM;

//*****************************************************************************
//
// MSPI3_D8 pin: MSPI3 data 8.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI3_D8                105
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI3_D8;

//*****************************************************************************
//
// MSPI3_D9 pin: MSPI3 data 9.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI3_D9                106
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI3_D9;

//*****************************************************************************
//
// MSPI3_D10 pin: MSPI3 data 10.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI3_D10               107
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI3_D10;

//*****************************************************************************
//
// MSPI3_D11 pin: MSPI3 data 11.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI3_D11               108
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI3_D11;

//*****************************************************************************
//
// MSPI3_D12 pin: MSPI3 data 12.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI3_D12               109
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI3_D12;

//*****************************************************************************
//
// MSPI3_D13 pin: MSPI3 data 13.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI3_D13               110
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI3_D13;

//*****************************************************************************
//
// MSPI3_D14 pin: MSPI3 data 14.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI3_D14               111
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI3_D14;

//*****************************************************************************
//
// MSPI3_D15 pin: MSPI3 data 15.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI3_D15               112
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI3_D15;

//*****************************************************************************
//
// MSPI3_DQS1DM1 pin: MSPI3 DQS1 DM1.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI3_DQS1DM1           113
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI3_DQS1DM1;

//*****************************************************************************
//
// MSPI3_D4_CLK pin: MSPI3 data 4 replace MSPI3_8(CLK).
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI3_D4_CLK            119
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI3_D4_CLK;

//*****************************************************************************
//
// MSPI3_RST pin: MSPI3 Reset.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI3_RST               216
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI3_RST;

//*****************************************************************************
//
// MSPI1_CE0 pin: MSPI1 chip select 0
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI1_CE0               53
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI1_CE0;

//*****************************************************************************
//
// MSPI1_CE1 pin: MSPI1 chip select 1
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI1_CE1               54
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI1_CE1;

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
#define AM_BSP_GPIO_MSPI1_RST               54
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
#define AM_BSP_GPIO_MSPI1_LS_EN             208
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI1_LS_EN;

//*****************************************************************************
//
// MSPI2_CE0 pin: MSPI2 chip select 0
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_CE0               63
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_CE0;

//*****************************************************************************
//
// MSPI2_CE1 pin: MSPI2 chip select 1
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_CE1               156
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_CE1;

//*****************************************************************************
//
// MSPI2_D0 pin: NOR Flash I/F using MSPI2 data 0.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_D0                74
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_D0;

//*****************************************************************************
//
// MSPI2_D1 pin: NOR Flash I/F using MSPI2 data 1.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_D1                75
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_D1;

//*****************************************************************************
//
// MSPI2_D2 pin: NOR Flash I/F using MSPI2 data 2.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_D2                76
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_D2;

//*****************************************************************************
//
// MSPI2_D3 pin: NOR Flash I/F using MSPI2 data 3.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_D3                77
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_D3;

//*****************************************************************************
//
// MSPI2_D4 pin: NOR Flash I/F using MSPI2 data 4.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_D4                78
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_D4;

//*****************************************************************************
//
// MSPI2_D5 pin: NOR Flash I/F using MSPI2 data 5.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_D5                79
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_D5;

//*****************************************************************************
//
// MSPI2_D6 pin: NOR Flash I/F using MSPI2 data 6.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_D6                80
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_D6;

//*****************************************************************************
//
// MSPI2_D7 pin: NOR Flash I/F using MSPI2 data 7.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_D7                81
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_D7;

//*****************************************************************************
//
// MSPI2_SCK pin: NOR Flash I/F using MSPI2 clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_SCK               82
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_SCK;

//*****************************************************************************
//
// MSPI2_DQSDM pin: NOR Flash I/F using MSPI2 DQS.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_DQSDM             83
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_DQSDM;

//*****************************************************************************
//
// MSPI2_RST pin: NOR Flash Reset.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_RST               56
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_RST;

//*****************************************************************************
//
// MSPI2_D4_CLK pin: MSPI2 data 4 replace MSPI2_8(CLK).
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_D4_CLK            78
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_D4_CLK;

//*****************************************************************************
//
// MSPI2_LS_EN pin: MSPI2 Load Switch Control.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_LS_EN             11
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_LS_EN;

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
#define AM_BSP_GPIO_SDIO0_RST               12
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO0_RST;

//*****************************************************************************
//
// SD0_CD pin: SD Card using GPIO for card detection.
//
//*****************************************************************************
#define AM_BSP_GPIO_SD0_CD                  90
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SD0_CD;

//*****************************************************************************
//
// SD0_WP pin: SD Card using GPIO for write protection.
//
//*****************************************************************************
#define AM_BSP_GPIO_SD0_WP                  158
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SD0_WP;

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
// SDIO1_DAT4 pin: eMMC1 I/F using SDIO data 4.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO1_DAT4              130
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO1_DAT4;

//*****************************************************************************
//
// SDIO1_DAT5 pin: eMMC1 I/F using SDIO data 5.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO1_DAT5              131
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO1_DAT5;

//*****************************************************************************
//
// SDIO1_DAT6 pin: eMMC1 I/F using SDIO data 6.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO1_DAT6              132
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO1_DAT6;

//*****************************************************************************
//
// SDIO1_DAT7 pin: eMMC1 I/F using SDIO data 7.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO1_DAT7              133
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO1_DAT7;

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
// SD1_CD pin: SD Card using GPIO for card detection.
//
//*****************************************************************************
#define AM_BSP_GPIO_SD1_CD                  133
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SD1_CD;

//*****************************************************************************
//
// SD1_WP pin: SD Card using GPIO for write protection.
//
//*****************************************************************************
#define AM_BSP_GPIO_SD1_WP                  132
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SD1_WP;

//*****************************************************************************
//
// SD_LEVEL_SHIFT_SEL pin: SD Card using GPIO for switching supply voltage.
//
//*****************************************************************************
#define AM_BSP_GPIO_SD_LEVEL_SHIFT_SEL      157
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
// M2_DISP_SPI_SD pin: Display SPI Data In/Out (3-wire).
//
//*****************************************************************************
#define AM_BSP_GPIO_M2_DISP_SPI_SD          79
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M2_DISP_SPI_SD;

//*****************************************************************************
//
// M2_DISP_SPI_SDO pin: Display SPI Data Out (4-wire)
//
//*****************************************************************************
#define AM_BSP_GPIO_M2_DISP_SPI_SDO         79
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M2_DISP_SPI_SDO;

//*****************************************************************************
//
// M2_DISP_SDI pin: Display SPI Data In (4-wire).
//
//*****************************************************************************
#define AM_BSP_GPIO_M2_DISP_SDI             82
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M2_DISP_SDI;

//*****************************************************************************
//
// M2_DISP_SPI_DCX pin: Display SPI DCx.
//
//*****************************************************************************
#define AM_BSP_GPIO_M2_DISP_SPI_DCX         80
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M2_DISP_SPI_DCX;

//*****************************************************************************
//
// M2_DISP_SPI_SCK pin: Display SPI Clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_M2_DISP_SPI_SCK         81
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M2_DISP_SPI_SCK;

//*****************************************************************************
//
// M2_DISP_SPI_CS pin: Display SPI CS.
//
//*****************************************************************************
#define AM_BSP_GPIO_M2_DISP_SPI_CS          14
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M2_DISP_SPI_CS;

//*****************************************************************************
//
// M2_DISP_DEVICE_EN pin: Display Device Enable (VCI_EN).
//
//*****************************************************************************
#define AM_BSP_GPIO_M2_DISP_DEVICE_EN       13
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M2_DISP_DEVICE_EN;

//*****************************************************************************
//
// M2_DISP_SPI_RES pin: Display SPI Reset.
//
//*****************************************************************************
#define AM_BSP_GPIO_M2_DISP_SPI_RES         11
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M2_DISP_SPI_RES;

//*****************************************************************************
//
// M2_DISP_SPI_TE pin: Display SPI TE.
//
//*****************************************************************************
#define AM_BSP_GPIO_M2_DISP_SPI_TE          33
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M2_DISP_SPI_TE;

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
// M2_DISP_QSPI_D0 pin: Display QSPI Data Out 0.
//
//*****************************************************************************
#define AM_BSP_GPIO_M2_DISP_QSPI_D0         79
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M2_DISP_QSPI_D0;

//*****************************************************************************
//
// M2_DISP_QSPI_D1 pin: Display QSPI Data Out 1.
//
//*****************************************************************************
#define AM_BSP_GPIO_M2_DISP_QSPI_D1         80
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M2_DISP_QSPI_D1;

//*****************************************************************************
//
// M2_DISP_QSPI_D2 pin: Display QSPI Data Out 2.
//
//*****************************************************************************
#define AM_BSP_GPIO_M2_DISP_QSPI_D2         82
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M2_DISP_QSPI_D2;

//*****************************************************************************
//
// M2_DISP_QSPI_D3 pin: Display QSPI Data Out 3.
//
//*****************************************************************************
#define AM_BSP_GPIO_M2_DISP_QSPI_D3         83
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M2_DISP_QSPI_D3;

//*****************************************************************************
//
// M2_DISP_QSPI_SCK pin: Display QSPI Clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_M2_DISP_QSPI_SCK        81
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M2_DISP_QSPI_SCK;

//*****************************************************************************
//
// M2_DISP_QSPI_CS pin: Display CSX chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_M2_DISP_QSPI_CS         14
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_M2_DISP_QSPI_CS;

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
// DISP_GEN pin: GATE ENABLE SIGNAL
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_GEN                209
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_GEN;

//*****************************************************************************
//
// DISP_INTB pin: INITIAL SIGNAL FOR BINARY/GATE DRIVER
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_INTB               210
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_INTB;

//*****************************************************************************
//
// DISP_R1 pin: RED OF ODD PIXELS
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_R1                 211
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_R1;

//*****************************************************************************
//
// DISP_R2 pin: RED OF EVEN PIXELS
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_R2                 212
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_R2;

//*****************************************************************************
//
// DISP_G1 pin: GREEN OF ODD PIXELS
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_G1                 213
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_G1;

//*****************************************************************************
//
// DISP_G2 pin: GREEN OF EVEN PIXELS
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_G2                 214
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_G2;

//*****************************************************************************
//
// DISP_B1 pin: BLUE OF ODD PIXELS
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_B1                 215
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_B1;

//*****************************************************************************
//
// DISP_B2 pin: BLUE OF EVEN PIXELS
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_B2                 216
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_B2;

//*****************************************************************************
//
// DISP_BSP pin: START SIGNAL OF BINARY DRIVER
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_BSP                217
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_BSP;

//*****************************************************************************
//
// DISP_GSP pin: START SIGNAL OF GATE DRIVER
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_GSP                218
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_GSP;

//*****************************************************************************
//
// DISP_BCK pin: CLOCK OF BINARY DRIVER
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_BCK                219
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_BCK;

//*****************************************************************************
//
// DISP_GCK pin: CLOCK OF GATE DRIVER
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_GCK                220
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_GCK;

//*****************************************************************************
//
// DISP_VA pin: OPPOSITE PHASE SIGNAL TO VCOM
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_VA                 221
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_VA;

//*****************************************************************************
//
// DISP_VB pin: INPHASE SIGNAL TO VCOM
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_VB                 222
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_VB;

//*****************************************************************************
//
// DISP_VCOM pin: PHASE SIGNAL TO VCOM
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_VCOM               223
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_VCOM;

//*****************************************************************************
//
// DISP_D15 pin: Data pin 15
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_D15                136
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_D15;

//*****************************************************************************
//
// DISP_D16 pin: Data pin 16
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_D16                140
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_D16;

//*****************************************************************************
//
// DISP_D17 pin: Data pin 17
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_D17                141
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_D17;

//*****************************************************************************
//
// DISP_D5 pin: Data pin 5
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_D5                 148
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_D5;

//*****************************************************************************
//
// DISP_D6 pin: Data pin 6
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_D6                 149
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_D6;

//*****************************************************************************
//
// DISP_D7 pin: Data pin 7
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_D7                 150
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_D7;

//*****************************************************************************
//
// DISP_D8 pin: DData pin 8
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_D8                 151
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_D8;

//*****************************************************************************
//
// DISP_D9 pin: Data pin 9
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_D9                 152
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_D9;

//*****************************************************************************
//
// DISP_D10 pin: Data pin 10
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_D10                153
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_D10;

//*****************************************************************************
//
// DISP_D11 pin: Data pin 11
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_D11                154
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_D11;

//*****************************************************************************
//
// DISP_D12 pin: Data pin 12
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_D12                155
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_D12;

//*****************************************************************************
//
// DISP_SD pin: Shutdown signal of DPI
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_SD                 186
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_SD;

//*****************************************************************************
//
// DISP_DE pin: Display Enable signal of DPI
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_DE                 187
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_DE;

//*****************************************************************************
//
// DISP_CM pin: Color Mode signal of DPI
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_CM                 188
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_CM;

//*****************************************************************************
//
// DISP_PCLK pin: Clock signal of DPI
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_PCLK               189
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_PCLK;

//*****************************************************************************
//
// DISP_D1 pin: Data pin 1
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_D1                 209
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_D1;

//*****************************************************************************
//
// DISP_D4 pin: Data pin 4
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_D4                 210
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_D4;

//*****************************************************************************
//
// DISP_D23 pin: Data pin 23
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_D23                211
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_D23;

//*****************************************************************************
//
// DISP_D22 pin: Data pin 22
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_D22                212
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_D22;

//*****************************************************************************
//
// DISP_D21 pin: Data pin 21
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_D21                213
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_D21;

//*****************************************************************************
//
// DISP_D20 pin: Data pin 20
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_D20                214
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_D20;

//*****************************************************************************
//
// DISP_D19 pin: Data pin 19
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_D19                215
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_D19;

//*****************************************************************************
//
// DISP_D18 pin: Data pin 18
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_D18                216
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_D18;

//*****************************************************************************
//
// DISP_HS pin: HSYNC of DPI
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_HS                 217
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_HS;

//*****************************************************************************
//
// DISP_VS pin: VSYNC of DPI
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_VS                 218
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_VS;

//*****************************************************************************
//
// DISP_D3 pin: Data pin 3
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_D3                 219
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_D3;

//*****************************************************************************
//
// DISP_D0 pin: Data pin 0
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_D0                 220
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_D0;

//*****************************************************************************
//
// DISP_D2 pin: Data pin 2
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_D2                 221
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_D2;

//*****************************************************************************
//
// DISP_D13 pin: Data pin 13
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_D13                222
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_D13;

//*****************************************************************************
//
// DISP_D14 pin: Data pin 14
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_D14                223
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_D14;

//*****************************************************************************
//
// DISP_RD1 pin: Red pixel data(MSB)
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_RD1                136
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_RD1;

//*****************************************************************************
//
// DISP_GD0 pin: Green pixel data(LSB)
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_GD0                152
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_GD0;

//*****************************************************************************
//
// DISP_GD1 pin: Green pixel data(MSB)
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_GD1                153
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_GD1;

//*****************************************************************************
//
// DISP_ON_OFF pin: Display on/off control
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_ON_OFF             186
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_ON_OFF;

//*****************************************************************************
//
// DISP_COMFC pin: COM frequency control
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_COMFC              187
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_COMFC;

//*****************************************************************************
//
// DISP_MODE pin: Scan direction control
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_MODE               188
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_MODE;

//*****************************************************************************
//
// DISP_BD1 pin: Blue pixel data(MSB)
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_BD1                210
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_BD1;

//*****************************************************************************
//
// DISP_BD0 pin: Blue pixel data(LSB)
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_BD0                219
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_BD0;

//*****************************************************************************
//
// DISP_COMIN pin: COM inversion signal
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_COMIN              221
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_COMIN;

//*****************************************************************************
//
// DISP_RD0 pin: Red pixel data(LSB)
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_RD0                223
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_RD0;

//*****************************************************************************
//
// DBIB_RESX pin: DBI-B Reset pins
//
//*****************************************************************************
#define AM_BSP_GPIO_DBIB_RESX               136
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DBIB_RESX;

//*****************************************************************************
//
// DBIB_CSX pin: DBI-B Chip Selection pins
//
//*****************************************************************************
#define AM_BSP_GPIO_DBIB_CSX                209
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DBIB_CSX;

//*****************************************************************************
//
// DBIB_DCX pin: DBI-B Data/Command pins
//
//*****************************************************************************
#define AM_BSP_GPIO_DBIB_DCX                210
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DBIB_DCX;

//*****************************************************************************
//
// DBIB_WRX pin: DBI-B Write signal pins
//
//*****************************************************************************
#define AM_BSP_GPIO_DBIB_WRX                211
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DBIB_WRX;

//*****************************************************************************
//
// DBIB_RDX pin: DBI-B Read signal pins
//
//*****************************************************************************
#define AM_BSP_GPIO_DBIB_RDX                212
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DBIB_RDX;

//*****************************************************************************
//
// DBIB_D0 pin: DBI-B Data 0
//
//*****************************************************************************
#define AM_BSP_GPIO_DBIB_D0                 213
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DBIB_D0;

//*****************************************************************************
//
// DBIB_D1 pin: DBI-B Data 1
//
//*****************************************************************************
#define AM_BSP_GPIO_DBIB_D1                 214
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DBIB_D1;

//*****************************************************************************
//
// DBIB_D2 pin: DBI-B Data 2
//
//*****************************************************************************
#define AM_BSP_GPIO_DBIB_D2                 215
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DBIB_D2;

//*****************************************************************************
//
// DBIB_D3 pin: DBI-B Data 3
//
//*****************************************************************************
#define AM_BSP_GPIO_DBIB_D3                 216
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DBIB_D3;

//*****************************************************************************
//
// DBIB_D4 pin: DBI-B Data 4
//
//*****************************************************************************
#define AM_BSP_GPIO_DBIB_D4                 217
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DBIB_D4;

//*****************************************************************************
//
// DBIB_D5 pin: DBI-B Data 5
//
//*****************************************************************************
#define AM_BSP_GPIO_DBIB_D5                 218
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DBIB_D5;

//*****************************************************************************
//
// DBIB_D6 pin: DBI-B Data 6
//
//*****************************************************************************
#define AM_BSP_GPIO_DBIB_D6                 219
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DBIB_D6;

//*****************************************************************************
//
// DBIB_D7 pin: DBI-B Data 7
//
//*****************************************************************************
#define AM_BSP_GPIO_DBIB_D7                 220
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DBIB_D7;

//*****************************************************************************
//
// DBIB_D8 pin: DBI-B Data 8
//
//*****************************************************************************
#define AM_BSP_GPIO_DBIB_D8                 148
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DBIB_D8;

//*****************************************************************************
//
// DBIB_D9 pin: DBI-B Data 9
//
//*****************************************************************************
#define AM_BSP_GPIO_DBIB_D9                 149
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DBIB_D9;

//*****************************************************************************
//
// DBIB_D10 pin: DBI-B Data 10
//
//*****************************************************************************
#define AM_BSP_GPIO_DBIB_D10                150
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DBIB_D10;

//*****************************************************************************
//
// DBIB_D11 pin: DBI-B Data 11
//
//*****************************************************************************
#define AM_BSP_GPIO_DBIB_D11                151
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DBIB_D11;

//*****************************************************************************
//
// DBIB_D12 pin: DBI-B Data 12
//
//*****************************************************************************
#define AM_BSP_GPIO_DBIB_D12                152
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DBIB_D12;

//*****************************************************************************
//
// DBIB_D13 pin: DBI-B Data 13
//
//*****************************************************************************
#define AM_BSP_GPIO_DBIB_D13                153
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DBIB_D13;

//*****************************************************************************
//
// DBIB_D14 pin: DBI-B Data 14
//
//*****************************************************************************
#define AM_BSP_GPIO_DBIB_D14                154
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DBIB_D14;

//*****************************************************************************
//
// DBIB_D15 pin: DBI-B Data 15
//
//*****************************************************************************
#define AM_BSP_GPIO_DBIB_D15                155
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DBIB_D15;

//*****************************************************************************
//
// BL pin: backlight brightness
//
//*****************************************************************************
#define AM_BSP_GPIO_BL                      19
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_BL;

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
#define AM_BSP_GPIO_VDD18_SWITCH            52
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_VDD18_SWITCH;

//*****************************************************************************
//
// VDDUSB33_SWITCH pin: USB 3.3V Power Switch.
//
//*****************************************************************************
#define AM_BSP_GPIO_VDDUSB33_SWITCH         91
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_VDDUSB33_SWITCH;

//*****************************************************************************
//
// VDDUSB0P9_SWITCH pin: USB 0.9V Power Switch.
//
//*****************************************************************************
#define AM_BSP_GPIO_VDDUSB0P9_SWITCH        90
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_VDDUSB0P9_SWITCH;

//*****************************************************************************
//
// DISP_DSI_TE pin: Display SPI TE.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_DSI_TE             33
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_DSI_TE;

//*****************************************************************************
//
// EM9305_CS pin: I/O Master 5 chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_EM9305_CS               187
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_EM9305_CS;
#define AM_BSP_EM9305_CS_CHNL               0

//*****************************************************************************
//
// EM9305_INT pin: EM9305 interrupt.
//
//*****************************************************************************
#define AM_BSP_GPIO_EM9305_INT              149
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
