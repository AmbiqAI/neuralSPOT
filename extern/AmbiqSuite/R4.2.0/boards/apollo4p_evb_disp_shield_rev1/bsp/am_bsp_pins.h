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
// This is part of revision release_sdk_4_2_0-0a31245a64 of the AmbiqSuite Development Package.
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
// LED0 pin: The LED nearest the inside of the board (D4). Note: Active Low.
//
//*****************************************************************************
#define AM_BSP_GPIO_LED0                    90
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_LED0;

//*****************************************************************************
//
// LED1 pin: The middle LED (D3). Note: Active Low.
//
//*****************************************************************************
#define AM_BSP_GPIO_LED1                    30
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_LED1;

//*****************************************************************************
//
// LED2 pin: The LED nearest the outside of the board (D5). Note: Active Low.
//
//*****************************************************************************
#define AM_BSP_GPIO_LED2                    97
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_LED2;

//*****************************************************************************
//
// BUTTON0 pin: Labeled BTN0 on the Apollo4 Blue EVB.
//
//*****************************************************************************
#define AM_BSP_GPIO_BUTTON0                 18
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_BUTTON0;

//*****************************************************************************
//
// BUTTON1 pin: Labeled BTN1 on the Apollo4 Blue EVB.
//
//*****************************************************************************
#define AM_BSP_GPIO_BUTTON1                 19
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_BUTTON1;

//*****************************************************************************
//
// COM_UART_TX pin: This pin is UART0 transmit used by J-Link. Note: LED1 cannot be used simultaneously.
//
//*****************************************************************************
#define AM_BSP_GPIO_COM_UART_TX             60
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_COM_UART_TX;

//*****************************************************************************
//
// COM_UART_RX pin: This pin is UART0 receive used by J-Link. Note: IOM5 clock cannot be used simultaneously.
//
//*****************************************************************************
#define AM_BSP_GPIO_COM_UART_RX             47
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_COM_UART_RX;

//*****************************************************************************
//
// COM_UART_CTS pin: This pin is the UART0 CTS used by J-Link.
//
//*****************************************************************************
#define AM_BSP_GPIO_COM_UART_CTS            59
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_COM_UART_CTS;

//*****************************************************************************
//
// COM_UART_RTS pin: This pin is the UART0 RTS used by J-Link.
//
//*****************************************************************************
#define AM_BSP_GPIO_COM_UART_RTS            58
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_COM_UART_RTS;

//*****************************************************************************
//
// UART1_TX pin: This pin is UART1 transmit.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART1_TX                12
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART1_TX;

//*****************************************************************************
//
// UART1_RX pin: This pin is UART1 receive.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART1_RX                14
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART1_RX;

//*****************************************************************************
//
// UART1_CTS pin: This pin is the UART1 CTS.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART1_CTS               51
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART1_CTS;

//*****************************************************************************
//
// UART1_RTS pin: This pin is the UART1 RTS.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART1_RTS               49
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART1_RTS;

//*****************************************************************************
//
// UART2_TX pin: This pin is UART2 transmit.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART2_TX                13
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART2_TX;

//*****************************************************************************
//
// UART2_RX pin: This pin is UART2 receive.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART2_RX                11
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART2_RX;

//*****************************************************************************
//
// UART2_CTS pin: This pin is the UART2 CTS.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART2_CTS               52
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART2_CTS;

//*****************************************************************************
//
// UART2_RTS pin: This pin is the UART2 RTS.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART2_RTS               50
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART2_RTS;

//*****************************************************************************
//
// UART3_TX pin: This pin is UART3 transmit. Note: IOM6 clock cannot be used simultaneously.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART3_TX                61
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART3_TX;

//*****************************************************************************
//
// UART3_RX pin: This pin is UART3 receive. Note, IOM6 MISO cannot be used simultaneously.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART3_RX                63
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART3_RX;

//*****************************************************************************
//
// UART3_CTS pin: This pin is the UART3 CTS. Note: ADC0 cannot be used simultaneously.
//
//*****************************************************************************
#define AM_BSP_GPIO_UART3_CTS               19
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_UART3_CTS;

//*****************************************************************************
//
// UART3_RTS pin: This pin is the UART3 RTS.Note: ADC2 cannot be used simultaneously
//
//*****************************************************************************
#define AM_BSP_GPIO_UART3_RTS               17
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
#define AM_BSP_GPIO_IOM1_CS                 11
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
#define AM_BSP_GPIO_IOM2_CS                 37
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
#define AM_BSP_GPIO_IOM3_CS                 85
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
#define AM_BSP_GPIO_IOM4_CS                 93
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
#define AM_BSP_GPIO_IOM5_CS                 60
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
#define AM_BSP_GPIO_IOM6_CS                 30
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
#define AM_BSP_GPIO_IOM7_CS                 100
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
#define AM_BSP_GPIO_IOS_CE                  3
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOS_CE;

//*****************************************************************************
//
// IOS_MISO pin: I/O Slave SPI MISO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOS_MISO                2
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOS_MISO;

//*****************************************************************************
//
// IOS_MOSI pin: I/O Slave SPI MOSI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOS_MOSI                1
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOS_MOSI;

//*****************************************************************************
//
// IOS_SCK pin: I/O Slave SPI SCK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOS_SCK                 0
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOS_SCK;

//*****************************************************************************
//
// IOS_SCL pin: I/O Slave I2C clock signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOS_SCL                 0
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOS_SCL;

//*****************************************************************************
//
// IOS_SDA pin: I/O Slave I2C data signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOS_SDA                 1
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOS_SDA;

//*****************************************************************************
//
// IOS_INT pin: I/O Slave interrupt.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOS_INT                 4
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_IOS_INT;

//*****************************************************************************
//
// MSPI2_CE0 pin: MSPI2 chip select for device 0
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_CE0               0
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_CE0;
#define AM_BSP_MSPI2_CE0_CHNL               0

//*****************************************************************************
//
// MSPI2_CE1 pin: MSPI2 chip select for device 1
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_CE1               86
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_CE1;
#define AM_BSP_MSPI2_CE1_CHNL               1

//*****************************************************************************
//
// MSPI2_D0 pin: MSPI2 data 0.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_D0                74
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_D0;

//*****************************************************************************
//
// MSPI2_D1 pin: MSPI2 data 1.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_D1                75
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_D1;

//*****************************************************************************
//
// MSPI2_D2 pin: MSPI2 data 2.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_D2                76
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_D2;

//*****************************************************************************
//
// MSPI2_D3 pin: MSPI2 data 3.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_D3                77
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_D3;

//*****************************************************************************
//
// MSPI2_D4 pin: MSPI2 data 4.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_D4                78
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_D4;

//*****************************************************************************
//
// MSPI2_D5 pin: MSPI2 data 5.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_D5                79
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_D5;

//*****************************************************************************
//
// MSPI2_D6 pin: MSPI2 data 6.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_D6                80
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_D6;

//*****************************************************************************
//
// MSPI2_D7 pin: MSPI2 data 7.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_D7                81
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_D7;

//*****************************************************************************
//
// MSPI2_SCK pin: MSPI2 clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_SCK               82
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_SCK;

//*****************************************************************************
//
// MSPI2_DQSDM pin: MSPI2 DQS DM.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI2_DQSDM             83
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_MSPI2_DQSDM;

//*****************************************************************************
//
// DISP_DSI_RES pin: Display DSI Reset.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_DSI_RES            29
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_DSI_RES;

//*****************************************************************************
//
// DISP_DSI_TE pin: Display DSI TE.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_DSI_TE             33
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_DSI_TE;

//*****************************************************************************
//
// VDD18_SWITCH pin: MIPI 1.8V Power Switch.
//
//*****************************************************************************
#define AM_BSP_GPIO_VDD18_SWITCH            104
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_VDD18_SWITCH;

//*****************************************************************************
//
// VDDUSB33_SWITCH pin: USB 3.3V Power Switch.
//
//*****************************************************************************
#define AM_BSP_GPIO_VDDUSB33_SWITCH         103
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_VDDUSB33_SWITCH;

//*****************************************************************************
//
// VDDUSB0P9_SWITCH pin: USB 0.9V Power Switch.
//
//*****************************************************************************
#define AM_BSP_GPIO_VDDUSB0P9_SWITCH        101
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_VDDUSB0P9_SWITCH;

//*****************************************************************************
//
// ITM_SWO pin: ITM Serial Wire Output.
//
//*****************************************************************************
#define AM_BSP_GPIO_ITM_SWO                 28
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_ITM_SWO;

//*****************************************************************************
//
// EMT_TRACE0 pin: ETM Serial Wire Debug Trace Out 0.
//
//*****************************************************************************
#define AM_BSP_GPIO_EMT_TRACE0              51
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_EMT_TRACE0;

//*****************************************************************************
//
// ETM_TRACE1 pin: ETM Serial Wire Debug Trace Out 1.
//
//*****************************************************************************
#define AM_BSP_GPIO_ETM_TRACE1              52
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_ETM_TRACE1;

//*****************************************************************************
//
// ETM_TRACE2 pin: ETM Serial Wire Debug Trace Out 2.
//
//*****************************************************************************
#define AM_BSP_GPIO_ETM_TRACE2              53
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_ETM_TRACE2;

//*****************************************************************************
//
// ETM_TRACE3 pin: ETM Serial Wire Debug Trace Out 3.
//
//*****************************************************************************
#define AM_BSP_GPIO_ETM_TRACE3              54
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_ETM_TRACE3;

//*****************************************************************************
//
// ETM_TRACECLK pin: ETM Serial Wire Debug Trace Clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_ETM_TRACECLK            50
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_ETM_TRACECLK;

//*****************************************************************************
//
// ETM_TRACECTL pin: ETM Serial Wire Debug Trace Control.
//
//*****************************************************************************
#define AM_BSP_GPIO_ETM_TRACECTL            55
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_ETM_TRACECTL;

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
// I2S0_SDIN pin: I2S0 Data In.
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S0_SDIN               4
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S0_SDIN;

//*****************************************************************************
//
// I2S1_DATA pin: I2S1 Bidirectional Data.
//
//*****************************************************************************
#define AM_BSP_GPIO_I2S1_DATA               17
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_I2S1_DATA;

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
// PDM0_CLK pin: PDM 0 Clock Signal. Note: ETM_TRACECLK cannot be used simultaneously with this signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_PDM0_CLK                50
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PDM0_CLK;

//*****************************************************************************
//
// PDM0_DATA pin: PDM 0 Data Signal. Note: ETM_TRACE0 cannot be used simultaneously with this signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_PDM0_DATA               51
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PDM0_DATA;

//*****************************************************************************
//
// PDM1_CLK pin: PDM 1 Clock Signal. Note: ETM_TRACE1 cannot be used simultaneously with this signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_PDM1_CLK                52
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PDM1_CLK;

//*****************************************************************************
//
// PDM1_DATA pin: PDM 1 Data Signal. Note: ETM_TRACE2 cannot be used simultaneously with this signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_PDM1_DATA               53
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PDM1_DATA;

//*****************************************************************************
//
// PDM2_CLK pin: PDM 2 Clock Signal. Note: ETM_TRACE3 cannot be used simultaneously with this signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_PDM2_CLK                54
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PDM2_CLK;

//*****************************************************************************
//
// PDM2_DATA pin: PDM 2 Data Signal. Note: ETM_TRACECTL cannot be used simultaneously with this signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_PDM2_DATA               55
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PDM2_DATA;

//*****************************************************************************
//
// PDM3_CLK pin: PDM 3 Clock Signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_PDM3_CLK                56
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PDM3_CLK;

//*****************************************************************************
//
// PDM3_DATA pin: PDM 3 Data Signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_PDM3_DATA               57
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PDM3_DATA;

//*****************************************************************************
//
// SWDIO pin: Cortex Serial Wire DIO.
//
//*****************************************************************************
#define AM_BSP_GPIO_SWDIO                   21
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SWDIO;

//*****************************************************************************
//
// SWDCK pin: Cortex Serial Wire DCK.
//
//*****************************************************************************
#define AM_BSP_GPIO_SWDCK                   20
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SWDCK;

//*****************************************************************************
//
// PSRAM_CE0 pin: PSRAM I/F using MSPI0 chip select for device 0.
//
//*****************************************************************************
#define AM_BSP_GPIO_PSRAM_CE0               57
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PSRAM_CE0;
#define AM_BSP_PSRAM_CE0_CHNL               0

//*****************************************************************************
//
// PSRAM_D0 pin: PSRAM I/F using MSPI0 data 0.
//
//*****************************************************************************
#define AM_BSP_GPIO_PSRAM_D0                64
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PSRAM_D0;

//*****************************************************************************
//
// PSRAM_D1 pin: PSRAM I/F using MSPI0 data 1.
//
//*****************************************************************************
#define AM_BSP_GPIO_PSRAM_D1                65
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PSRAM_D1;

//*****************************************************************************
//
// PSRAM_D2 pin: PSRAM I/F using MSPI0 data 2.
//
//*****************************************************************************
#define AM_BSP_GPIO_PSRAM_D2                66
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PSRAM_D2;

//*****************************************************************************
//
// PSRAM_D3 pin: PSRAM I/F using MSPI0 data 3.
//
//*****************************************************************************
#define AM_BSP_GPIO_PSRAM_D3                67
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PSRAM_D3;

//*****************************************************************************
//
// PSRAM_D4 pin: PSRAM I/F using MSPI0 data 4.
//
//*****************************************************************************
#define AM_BSP_GPIO_PSRAM_D4                68
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PSRAM_D4;

//*****************************************************************************
//
// PSRAM_D5 pin: PSRAM I/F using MSPI0 data 5.
//
//*****************************************************************************
#define AM_BSP_GPIO_PSRAM_D5                69
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PSRAM_D5;

//*****************************************************************************
//
// PSRAM_D6 pin: PSRAM I/F using MSPI0 data 6.
//
//*****************************************************************************
#define AM_BSP_GPIO_PSRAM_D6                70
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PSRAM_D6;

//*****************************************************************************
//
// PSRAM_D7 pin: PSRAM I/F using MSPI0 data 7.
//
//*****************************************************************************
#define AM_BSP_GPIO_PSRAM_D7                71
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PSRAM_D7;

//*****************************************************************************
//
// PSRAM_SCK pin: PSRAM I/F using MSPI0 clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_PSRAM_SCK               72
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PSRAM_SCK;

//*****************************************************************************
//
// PSRAM_DQS pin: PSRAM I/F using MSPI0 DQS.
//
//*****************************************************************************
#define AM_BSP_GPIO_PSRAM_DQS               73
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_PSRAM_DQS;

//*****************************************************************************
//
// NOR_CE0 pin: NOR Flash I/F using MSPI1 chip select for device 0.
//
//*****************************************************************************
#define AM_BSP_GPIO_NOR_CE0                 89
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_NOR_CE0;
#define AM_BSP_NOR_CE0_CHNL                 0

//*****************************************************************************
//
// NOR_D0 pin: NOR Flash I/F using MSPI1 data 0.
//
//*****************************************************************************
#define AM_BSP_GPIO_NOR_D0                  37
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_NOR_D0;

//*****************************************************************************
//
// NOR_D1 pin: NOR Flash I/F using MSPI1 data 1.
//
//*****************************************************************************
#define AM_BSP_GPIO_NOR_D1                  38
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_NOR_D1;

//*****************************************************************************
//
// NOR_D2 pin: NOR Flash I/F using MSPI1 data 2.
//
//*****************************************************************************
#define AM_BSP_GPIO_NOR_D2                  39
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_NOR_D2;

//*****************************************************************************
//
// NOR_D3 pin: NOR Flash I/F using MSPI1 data 3.
//
//*****************************************************************************
#define AM_BSP_GPIO_NOR_D3                  40
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_NOR_D3;

//*****************************************************************************
//
// NOR_D4 pin: NOR Flash I/F using MSPI1 data 4.
//
//*****************************************************************************
#define AM_BSP_GPIO_NOR_D4                  41
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_NOR_D4;

//*****************************************************************************
//
// NOR_D5 pin: NOR Flash I/F using MSPI1 data 5.
//
//*****************************************************************************
#define AM_BSP_GPIO_NOR_D5                  42
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_NOR_D5;

//*****************************************************************************
//
// NOR_D6 pin: NOR Flash I/F using MSPI1 data 6.
//
//*****************************************************************************
#define AM_BSP_GPIO_NOR_D6                  43
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_NOR_D6;

//*****************************************************************************
//
// NOR_D7 pin: NOR Flash I/F using MSPI1 data 7.
//
//*****************************************************************************
#define AM_BSP_GPIO_NOR_D7                  44
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_NOR_D7;

//*****************************************************************************
//
// NOR_SCK pin: NOR Flash I/F using MSPI1 clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_NOR_SCK                 45
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_NOR_SCK;

//*****************************************************************************
//
// NOR_DQS pin: NOR Flash I/F using MSPI1 DQS.
//
//*****************************************************************************
#define AM_BSP_GPIO_NOR_DQS                 46
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_NOR_DQS;

//*****************************************************************************
//
// NOR_RST pin: NOR Flash Reset.
//
//*****************************************************************************
#define AM_BSP_GPIO_NOR_RST                 102
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_NOR_RST;

//*****************************************************************************
//
// SDIO_DAT0 pin: eMMC NAND Flash I/F using SDIO data 0.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO_DAT0               84
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO_DAT0;

//*****************************************************************************
//
// SDIO_DAT1 pin: eMMC NAND Flash I/F using SDIO data 1.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO_DAT1               85
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO_DAT1;

//*****************************************************************************
//
// SDIO_DAT2 pin: eMMC NAND Flash I/F using SDIO data 2.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO_DAT2               86
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO_DAT2;

//*****************************************************************************
//
// SDIO_DAT3 pin: eMMC NAND Flash I/F using SDIO data 3.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO_DAT3               87
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO_DAT3;

//*****************************************************************************
//
// SDIO_DAT4 pin: eMMC NAND Flash I/F using SDIO data 4.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO_DAT4               79
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO_DAT4;

//*****************************************************************************
//
// SDIO_DAT5 pin: eMMC NAND Flash I/F using SDIO data 5.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO_DAT5               80
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO_DAT5;

//*****************************************************************************
//
// SDIO_DAT6 pin: eMMC NAND Flash I/F using SDIO data 6.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO_DAT6               81
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO_DAT6;

//*****************************************************************************
//
// SDIO_DAT7 pin: eMMC NAND Flash I/F using SDIO data 7.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO_DAT7               82
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO_DAT7;

//*****************************************************************************
//
// SDIO_CMD pin: eMMC NAND Flash I/F using SDIO CMD
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO_CMD                83
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO_CMD;

//*****************************************************************************
//
// SDIO_CLK pin: eMMC NAND Flash I/F using SDIO clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO_CLK                88
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO_CLK;

//*****************************************************************************
//
// SDIO_RST pin: eMMC NAND Flash I/F using GPIO for Reset.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO_RST                96
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_SDIO_RST;

//*****************************************************************************
//
// DISP_SPI_SD pin: Display SPI Data In/Out (3-wire).
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_SPI_SD             74
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_SPI_SD;

//*****************************************************************************
//
// DISP_SPI_SDO pin: Display SPI Data Out (4-wire)
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_SPI_SDO            74
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_SPI_SDO;

//*****************************************************************************
//
// DISP_SDI pin: Display SPI Data In (4-wire).
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_SDI                79
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_SDI;

//*****************************************************************************
//
// DISP_SPI_DCX pin: Display SPI DCx.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_SPI_DCX            75
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_SPI_DCX;

//*****************************************************************************
//
// DISP_SPI_SCK pin: Display SPI Clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_SPI_SCK            78
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_SPI_SCK;

//*****************************************************************************
//
// DISP_SPI_CS pin: Display SPI CS.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_SPI_CS             25
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_SPI_CS;

//*****************************************************************************
//
// DISP_SPI_RES pin: Display SPI Reset.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_SPI_RES            29
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_SPI_RES;

//*****************************************************************************
//
// DISP_SPI_TE pin: Display SPI TE.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_SPI_TE             33
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_SPI_TE;

//*****************************************************************************
//
// DISP_DSPI_D0 pin: Display DSPI Data Out 0.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_DSPI_D0            74
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_DSPI_D0;

//*****************************************************************************
//
// DISP_DSPI_D1 pin: Display DSPI Data Out 1.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_DSPI_D1            75
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_DSPI_D1;

//*****************************************************************************
//
// DISP_DSPI_SCK pin: Display DSPI Clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_DSPI_SCK           78
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_DSPI_SCK;

//*****************************************************************************
//
// DISP_DSPI_CS pin: Display DSPI CS.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_DSPI_CS            25
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_DSPI_CS;

//*****************************************************************************
//
// DISP_DSPI_RES pin: Display DSPI Reset.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_DSPI_RES           29
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_DSPI_RES;

//*****************************************************************************
//
// DISP_DSPI_TE pin: Display DSPI TE.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_DSPI_TE            33
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_DSPI_TE;

//*****************************************************************************
//
// DISP_QSPI_D0 pin: Display QSPI Data Out 0.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_QSPI_D0            74
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_QSPI_D0;

//*****************************************************************************
//
// DISP_QSPI_D1 pin: Display QSPI Data Out 1.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_QSPI_D1            75
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_QSPI_D1;

//*****************************************************************************
//
// DISP_QSPI_D2 pin: Display QSPI Data Out 2.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_QSPI_D2            76
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_QSPI_D2;

//*****************************************************************************
//
// DISP_QSPI_D3 pin: Display QSPI Data Out 3.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_QSPI_D3            77
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_QSPI_D3;

//*****************************************************************************
//
// DISP_QSPI_SCK pin: Display QSPI Clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_QSPI_SCK           78
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_QSPI_SCK;

//*****************************************************************************
//
// DISP_QSPI_CS pin: Display CSX chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_QSPI_CS            25
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_QSPI_CS;

//*****************************************************************************
//
// DISP_QSPI_RES pin: Display QSPI Reset.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_QSPI_RES           29
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_QSPI_RES;

//*****************************************************************************
//
// DISP_QSPI_TE pin: Display TE.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_QSPI_TE            33
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_QSPI_TE;

//*****************************************************************************
//
// DISP_IFSEL1 pin: Display Interface Select 1.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_IFSEL1             95
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_IFSEL1;

//*****************************************************************************
//
// DISP_IFSEL2 pin: Display Interface Select 2.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_IFSEL2             98
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_IFSEL2;

//*****************************************************************************
//
// DISP_DEVICE_EN pin: Display Device Enable (VCI_EN).
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_DEVICE_EN          99
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_DEVICE_EN;

//*****************************************************************************
//
// DISP_DEVICE_RST pin: Display Device Reset.
//
//*****************************************************************************
#define AM_BSP_GPIO_DISP_DEVICE_RST         29
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISP_DEVICE_RST;

//*****************************************************************************
//
// TOUCH_ALS_M0SDA pin: Touch ALS I/O Master 0 I2C data.
//
//*****************************************************************************
#define AM_BSP_GPIO_TOUCH_ALS_M0SDA         6
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_TOUCH_ALS_M0SDA;

//*****************************************************************************
//
// TOUCH_ALS_M0SCL pin: Touch ALS I/O Master 0 I2C clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_TOUCH_ALS_M0SCL         5
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_TOUCH_ALS_M0SCL;

//*****************************************************************************
//
// TOUCH_RST pin: Touch panel reset.
//
//*****************************************************************************
#define AM_BSP_GPIO_TOUCH_RST               26
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_TOUCH_RST;

//*****************************************************************************
//
// TOUCH_INT pin: Touch panel interrupt.
//
//*****************************************************************************
#define AM_BSP_GPIO_TOUCH_INT               56
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_TOUCH_INT;

//*****************************************************************************
//
// ALS_INT pin: ALS interrupt.
//
//*****************************************************************************
#define AM_BSP_GPIO_ALS_INT                 92
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_ALS_INT;

//*****************************************************************************
//
// ACCEL_CS pin: Accelerometer I/O Master 0 SPI chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_ACCEL_CS                11
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_ACCEL_CS;
#define AM_BSP_ACCEL_CS_CHNL                0

//*****************************************************************************
//
// ACCEL_MISO pin: Accelerometer I/O Master 0 SPI MISO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_ACCEL_MISO              10
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_ACCEL_MISO;

//*****************************************************************************
//
// ACCEL_MOSI pin: Accelerometer I/O Master 0 SPI MOSI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_ACCEL_MOSI              9
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_ACCEL_MOSI;

//*****************************************************************************
//
// ACCEL_SCK pin: Accelerometer I/O Master 0 SPI CLK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_ACCEL_SCK               8
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_ACCEL_SCK;

//*****************************************************************************
//
// ACCEL_INT1 pin: Accelerometer interrupt 1.
//
//*****************************************************************************
#define AM_BSP_GPIO_ACCEL_INT1              31
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_ACCEL_INT1;

//*****************************************************************************
//
// ACCEL_INT2 pin: Accelerometer interrupt 2.
//
//*****************************************************************************
#define AM_BSP_GPIO_ACCEL_INT2              32
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_ACCEL_INT2;

//*****************************************************************************
//
// ACCEL_DEVICE_EN pin: Accelerometer Device Enable.
//
//*****************************************************************************
#define AM_BSP_GPIO_ACCEL_DEVICE_EN         91
extern am_hal_gpio_pincfg_t g_AM_BSP_GPIO_ACCEL_DEVICE_EN;

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
