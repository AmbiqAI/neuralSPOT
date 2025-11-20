//*****************************************************************************
//
//! @file am_hal_pin.h
//!
//! @brief Function select information for Apollo510 GPIOs.
//!
//! @addtogroup pin_ap510L Pin Functionality
//! @ingroup apollo330P_hal
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


#ifndef AM_HAL_PIN_H
#define AM_HAL_PIN_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Define the number of actual and virtual GPIOs
//
//*****************************************************************************
#define AM_HAL_PIN_TOTAL_GPIOS                  (120)
#define AM_HAL_PIN_VIRTUAL_FIRST                (120)
#define AM_HAL_PIN_VIRTUAL_GPIOS                (0)

//*****************************************************************************
//
// Define the number of functions per GPIO.
//
//*****************************************************************************
#define AM_HAL_PIN_NUMFUNCS                     16

//*****************************************************************************
//
// Function select macros by pin.
//
//*****************************************************************************
// PIN 0 functions
#define AM_HAL_PIN_0_SWTRACECLK         0
#define AM_HAL_PIN_0_SLFDSCK            1
#define AM_HAL_PIN_0_DISP_QSPI_D0       2
#define AM_HAL_PIN_0_GPIO               3
#define AM_HAL_PIN_0_DISP_SPI_SD        4
#define AM_HAL_PIN_0_DISP_SPI_SDO       5
#define AM_HAL_PIN_0_CT0                6
#define AM_HAL_PIN_0_NCE0               7
#define AM_HAL_PIN_0_OBSBUS0            8
#define AM_HAL_PIN_0_RESERVED9          9
#define AM_HAL_PIN_0_RESERVED10         10
#define AM_HAL_PIN_0_FPIO               11
#define AM_HAL_PIN_0_SLFDSCL            12
#define AM_HAL_PIN_0_DISP_QSPI_D0_OUT   13
#define AM_HAL_PIN_0_RESERVED14         14
#define AM_HAL_PIN_0_RESERVED15         15

// PIN 1 functions
#define AM_HAL_PIN_1_SWTRACE0           0
#define AM_HAL_PIN_1_SLFDMOSI           1
#define AM_HAL_PIN_1_DISP_QSPI_D1       2
#define AM_HAL_PIN_1_GPIO               3
#define AM_HAL_PIN_1_DISP_SPI_DCX       4
#define AM_HAL_PIN_1_RESERVED5          5
#define AM_HAL_PIN_1_CT1                6
#define AM_HAL_PIN_1_NCE1               7
#define AM_HAL_PIN_1_OBSBUS1            8
#define AM_HAL_PIN_1_UART0TX            9
#define AM_HAL_PIN_1_UART1CTS           10
#define AM_HAL_PIN_1_FPIO               11
#define AM_HAL_PIN_1_SLFDSDAWIR3        12
#define AM_HAL_PIN_1_RESERVED13         13
#define AM_HAL_PIN_1_RESERVED14         14
#define AM_HAL_PIN_1_RESERVED15         15

// PIN 2 functions
#define AM_HAL_PIN_2_SWTRACE1           0
#define AM_HAL_PIN_2_SLFDMISO           1
#define AM_HAL_PIN_2_DISP_QSPI_SCK      2
#define AM_HAL_PIN_2_GPIO               3
#define AM_HAL_PIN_2_DISP_SPI_SCK       4
#define AM_HAL_PIN_2_RESERVED5          5
#define AM_HAL_PIN_2_CT2                6
#define AM_HAL_PIN_2_NCE2               7
#define AM_HAL_PIN_2_OBSBUS2            8
#define AM_HAL_PIN_2_UART0RX            9
#define AM_HAL_PIN_2_UART1RTS           10
#define AM_HAL_PIN_2_FPIO               11
#define AM_HAL_PIN_2_XT_EXT             12
#define AM_HAL_PIN_2_FLB_FCLK           13
#define AM_HAL_PIN_2_RESERVED14         14
#define AM_HAL_PIN_2_RESERVED15         15

// PIN 3 functions
#define AM_HAL_PIN_3_SWTRACE2           0
#define AM_HAL_PIN_3_SLFDnCE            1
#define AM_HAL_PIN_3_DISP_QSPI_D2       2
#define AM_HAL_PIN_3_GPIO               3
#define AM_HAL_PIN_3_DISP_SPI_SDI       4
#define AM_HAL_PIN_3_RESERVED5          5
#define AM_HAL_PIN_3_CT3                6
#define AM_HAL_PIN_3_NCE3               7
#define AM_HAL_PIN_3_OBSBUS3            8
#define AM_HAL_PIN_3_RESERVED9          9
#define AM_HAL_PIN_3_RESERVED10         10
#define AM_HAL_PIN_3_FPIO               11
#define AM_HAL_PIN_3_LFRC_EXT           12
#define AM_HAL_PIN_3_FLB_TDI            13
#define AM_HAL_PIN_3_MDA_TDI            14
#define AM_HAL_PIN_3_RESERVED15         15

// PIN 4 functions
#define AM_HAL_PIN_4_SWTRACE3           0
#define AM_HAL_PIN_4_SLFDINT            1
#define AM_HAL_PIN_4_DISP_QSPI_D3       2
#define AM_HAL_PIN_4_GPIO               3
#define AM_HAL_PIN_4_DISP_SPI_RST       4
#define AM_HAL_PIN_4_RESERVED5          5
#define AM_HAL_PIN_4_CT4                6
#define AM_HAL_PIN_4_NCE4               7
#define AM_HAL_PIN_4_OBSBUS4            8
#define AM_HAL_PIN_4_UART1TX            9
#define AM_HAL_PIN_4_UART0CTS           10
#define AM_HAL_PIN_4_FPIO               11
#define AM_HAL_PIN_4_HFRC_EXT           12
#define AM_HAL_PIN_4_RESERVED13         13
#define AM_HAL_PIN_4_RESERVED14         14
#define AM_HAL_PIN_4_RESERVED15         15

// PIN 5 functions
#define AM_HAL_PIN_5_M0SCL              0
#define AM_HAL_PIN_5_M0SCK              1
#define AM_HAL_PIN_5_PDMI2S0_SDOUT      2
#define AM_HAL_PIN_5_GPIO               3
#define AM_HAL_PIN_5_RESERVED4          4
#define AM_HAL_PIN_5_32KHzXT            5
#define AM_HAL_PIN_5_CT5                6
#define AM_HAL_PIN_5_NCE5               7
#define AM_HAL_PIN_5_OBSBUS5            8
#define AM_HAL_PIN_5_UART1RX            9
#define AM_HAL_PIN_5_UART0RTS           10
#define AM_HAL_PIN_5_FPIO               11
#define AM_HAL_PIN_5_I2S0_SDOUT         12
#define AM_HAL_PIN_5_I2S0_DATA          13
#define AM_HAL_PIN_5_RESERVED14         14
#define AM_HAL_PIN_5_OPCG_TRIG          15

// PIN 6 functions
#define AM_HAL_PIN_6_M0SDAWIR3          0
#define AM_HAL_PIN_6_M0MOSI             1
#define AM_HAL_PIN_6_PDMI2S0_WS         2
#define AM_HAL_PIN_6_GPIO               3
#define AM_HAL_PIN_6_CLKOUT             4
#define AM_HAL_PIN_6_RESERVED5          5
#define AM_HAL_PIN_6_CT6                6
#define AM_HAL_PIN_6_NCE6               7
#define AM_HAL_PIN_6_OBSBUS6            8
#define AM_HAL_PIN_6_UART0TX            9
#define AM_HAL_PIN_6_UART1CTS           10
#define AM_HAL_PIN_6_FPIO               11
#define AM_HAL_PIN_6_I2S0_WS            12
#define AM_HAL_PIN_6_FLB_TMS            13
#define AM_HAL_PIN_6_MDA_TMS            14
#define AM_HAL_PIN_6_SCANIN0            15

// PIN 7 functions
#define AM_HAL_PIN_7_M0MISO             0
#define AM_HAL_PIN_7_VCMPO              1
#define AM_HAL_PIN_7_RESERVED2          2
#define AM_HAL_PIN_7_GPIO               3
#define AM_HAL_PIN_7_TRIG0              4
#define AM_HAL_PIN_7_RESERVED5          5
#define AM_HAL_PIN_7_CT7                6
#define AM_HAL_PIN_7_NCE7               7
#define AM_HAL_PIN_7_OBSBUS7            8
#define AM_HAL_PIN_7_UART0RX            9
#define AM_HAL_PIN_7_UART1RTS           10
#define AM_HAL_PIN_7_FPIO               11
#define AM_HAL_PIN_7_I2S0_SDIN          12
#define AM_HAL_PIN_7_FLB_TRSTN          13
#define AM_HAL_PIN_7_MDA_TRSTN          14
#define AM_HAL_PIN_7_SCANIN1            15

// PIN 8 functions
#define AM_HAL_PIN_8_CMPRF1             0
#define AM_HAL_PIN_8_M1SCL              1
#define AM_HAL_PIN_8_M1SCK              2
#define AM_HAL_PIN_8_GPIO               3
#define AM_HAL_PIN_8_TRIG1              4
#define AM_HAL_PIN_8_AO_OBSBUS0         5
#define AM_HAL_PIN_8_CT8                6
#define AM_HAL_PIN_8_NCE8               7
#define AM_HAL_PIN_8_OBSBUS8            8
#define AM_HAL_PIN_8_UART1TX            9
#define AM_HAL_PIN_8_UART0CTS           10
#define AM_HAL_PIN_8_FPIO               11
#define AM_HAL_PIN_8_RESERVED12         12
#define AM_HAL_PIN_8_RESERVED13         13
#define AM_HAL_PIN_8_RESERVED14         14
#define AM_HAL_PIN_8_SCANIN2            15

// PIN 9 functions
#define AM_HAL_PIN_9_CMPRF0             0
#define AM_HAL_PIN_9_M1SDAWIR3          1
#define AM_HAL_PIN_9_M1MOSI             2
#define AM_HAL_PIN_9_GPIO               3
#define AM_HAL_PIN_9_TRIG2              4
#define AM_HAL_PIN_9_AO_OBSBUS1         5
#define AM_HAL_PIN_9_CT9                6
#define AM_HAL_PIN_9_NCE9               7
#define AM_HAL_PIN_9_OBSBUS9            8
#define AM_HAL_PIN_9_UART1RX            9
#define AM_HAL_PIN_9_UART0RTS           10
#define AM_HAL_PIN_9_FPIO               11
#define AM_HAL_PIN_9_RESERVED12         12
#define AM_HAL_PIN_9_RESERVED13         13
#define AM_HAL_PIN_9_RESERVED14         14
#define AM_HAL_PIN_9_SCANIN3            15

// PIN 10 functions
#define AM_HAL_PIN_10_CMPIN0            0
#define AM_HAL_PIN_10_M1MISO            1
#define AM_HAL_PIN_10_PDMI2S0_CLK       2
#define AM_HAL_PIN_10_GPIO              3
#define AM_HAL_PIN_10_TRIG3             4
#define AM_HAL_PIN_10_AO_OBSBUS2        5
#define AM_HAL_PIN_10_CT10              6
#define AM_HAL_PIN_10_NCE10             7
#define AM_HAL_PIN_10_OBSBUS10          8
#define AM_HAL_PIN_10_UART0TX           9
#define AM_HAL_PIN_10_UART1CTS          10
#define AM_HAL_PIN_10_FPIO              11
#define AM_HAL_PIN_10_I2S0_CLK          12
#define AM_HAL_PIN_10_RESERVED13        13
#define AM_HAL_PIN_10_RESERVED14        14
#define AM_HAL_PIN_10_SCANIN4           15

// PIN 11 functions
#define AM_HAL_PIN_11_CMPIN1            0
#define AM_HAL_PIN_11_RESERVED1         1
#define AM_HAL_PIN_11_PDMI2S0_CLK       2
#define AM_HAL_PIN_11_GPIO              3
#define AM_HAL_PIN_11_TRIG0             4
#define AM_HAL_PIN_11_AO_OBSBUS3        5
#define AM_HAL_PIN_11_CT11              6
#define AM_HAL_PIN_11_NCE11             7
#define AM_HAL_PIN_11_OBSBUS11          8
#define AM_HAL_PIN_11_UART0RX           9
#define AM_HAL_PIN_11_UART1RTS          10
#define AM_HAL_PIN_11_FPIO              11
#define AM_HAL_PIN_11_I2S0_CLK          12
#define AM_HAL_PIN_11_RESERVED13        13
#define AM_HAL_PIN_11_RESERVED14        14
#define AM_HAL_PIN_11_SCANIN5           15

// PIN 12 functions
#define AM_HAL_PIN_12_ADCSE7            0
#define AM_HAL_PIN_12_RESERVED1         1
#define AM_HAL_PIN_12_PDMI2S0_WS        2
#define AM_HAL_PIN_12_GPIO              3
#define AM_HAL_PIN_12_TRIG1             4
#define AM_HAL_PIN_12_CMPRF2            5
#define AM_HAL_PIN_12_CT12              6
#define AM_HAL_PIN_12_NCE12             7
#define AM_HAL_PIN_12_OBSBUS12          8
#define AM_HAL_PIN_12_UART1TX           9
#define AM_HAL_PIN_12_UART0CTS          10
#define AM_HAL_PIN_12_FPIO              11
#define AM_HAL_PIN_12_I2S0_WS           12
#define AM_HAL_PIN_12_AO_OBSBUS0        13
#define AM_HAL_PIN_12_RESERVED14        14
#define AM_HAL_PIN_12_SCANIN6           15

// PIN 13 functions
#define AM_HAL_PIN_13_ADCSE6            0
#define AM_HAL_PIN_13_RESERVED1         1
#define AM_HAL_PIN_13_PDMI2S0_SDOUT     2
#define AM_HAL_PIN_13_GPIO              3
#define AM_HAL_PIN_13_TRIG2             4
#define AM_HAL_PIN_13_AO_OBSBUS1        5
#define AM_HAL_PIN_13_CT13              6
#define AM_HAL_PIN_13_NCE13             7
#define AM_HAL_PIN_13_OBSBUS13          8
#define AM_HAL_PIN_13_UART1RX           9
#define AM_HAL_PIN_13_UART0RTS          10
#define AM_HAL_PIN_13_FPIO              11
#define AM_HAL_PIN_13_I2S0_SDOUT        12
#define AM_HAL_PIN_13_I2S0_DATA         13
#define AM_HAL_PIN_13_RESERVED14        14
#define AM_HAL_PIN_13_SCANIN7           15

// PIN 14 functions
#define AM_HAL_PIN_14_ADCSE5            0
#define AM_HAL_PIN_14_RESERVED1         1
#define AM_HAL_PIN_14_RESERVED2         2
#define AM_HAL_PIN_14_GPIO              3
#define AM_HAL_PIN_14_TRIG3             4
#define AM_HAL_PIN_14_AO_OBSBUS2        5
#define AM_HAL_PIN_14_CT14              6
#define AM_HAL_PIN_14_NCE14             7
#define AM_HAL_PIN_14_OBSBUS14          8
#define AM_HAL_PIN_14_UART0TX           9
#define AM_HAL_PIN_14_UART1CTS          10
#define AM_HAL_PIN_14_FPIO              11
#define AM_HAL_PIN_14_I2S0_SDIN         12
#define AM_HAL_PIN_14_RESERVED13        13
#define AM_HAL_PIN_14_RESERVED14        14
#define AM_HAL_PIN_14_SCANIN8           15

// PIN 15 functions
#define AM_HAL_PIN_15_ADCSE4            0
#define AM_HAL_PIN_15_REFCLK_EXT        1
#define AM_HAL_PIN_15_RESERVED2         2
#define AM_HAL_PIN_15_GPIO              3
#define AM_HAL_PIN_15_TRIG0             4
#define AM_HAL_PIN_15_AO_OBSBUS3        5
#define AM_HAL_PIN_15_CT15              6
#define AM_HAL_PIN_15_NCE15             7
#define AM_HAL_PIN_15_OBSBUS15          8
#define AM_HAL_PIN_15_UART0RX           9
#define AM_HAL_PIN_15_UART1RTS          10
#define AM_HAL_PIN_15_FPIO              11
#define AM_HAL_PIN_15_RESERVED12        12
#define AM_HAL_PIN_15_RESERVED13        13
#define AM_HAL_PIN_15_RESERVED14        14
#define AM_HAL_PIN_15_SCANIN9           15

// PIN 16 functions
#define AM_HAL_PIN_16_ADCSE3            0
#define AM_HAL_PIN_16_EN192MHz          1
#define AM_HAL_PIN_16_RESERVED2         2
#define AM_HAL_PIN_16_GPIO              3
#define AM_HAL_PIN_16_TRIG1             4
#define AM_HAL_PIN_16_AO_OBSBUS0        5
#define AM_HAL_PIN_16_CT16              6
#define AM_HAL_PIN_16_NCE16             7
#define AM_HAL_PIN_16_OBSBUS0           8
#define AM_HAL_PIN_16_UART0TX           9
#define AM_HAL_PIN_16_UART1CTS          10
#define AM_HAL_PIN_16_FPIO              11
#define AM_HAL_PIN_16_RESERVED12        12
#define AM_HAL_PIN_16_RESERVED13        13
#define AM_HAL_PIN_16_RESERVED14        14
#define AM_HAL_PIN_16_LPG_ENABLE        15

// PIN 17 functions
#define AM_HAL_PIN_17_ADCSE2            0
#define AM_HAL_PIN_17_CLK192MHz         1
#define AM_HAL_PIN_17_RESERVED2         2
#define AM_HAL_PIN_17_GPIO              3
#define AM_HAL_PIN_17_TRIG2             4
#define AM_HAL_PIN_17_AO_OBSBUS1        5
#define AM_HAL_PIN_17_CT17              6
#define AM_HAL_PIN_17_NCE17             7
#define AM_HAL_PIN_17_OBSBUS1           8
#define AM_HAL_PIN_17_UART0RX           9
#define AM_HAL_PIN_17_UART1RTS          10
#define AM_HAL_PIN_17_FPIO              11
#define AM_HAL_PIN_17_RESERVED12        12
#define AM_HAL_PIN_17_RESERVED13        13
#define AM_HAL_PIN_17_RESERVED14        14
#define AM_HAL_PIN_17_SCANRST           15

// PIN 18 functions
#define AM_HAL_PIN_18_ADCSE1            0
#define AM_HAL_PIN_18_ANATEST2          1
#define AM_HAL_PIN_18_RESERVED2         2
#define AM_HAL_PIN_18_GPIO              3
#define AM_HAL_PIN_18_TRIG3             4
#define AM_HAL_PIN_18_AO_OBSBUS2        5
#define AM_HAL_PIN_18_CT18              6
#define AM_HAL_PIN_18_NCE18             7
#define AM_HAL_PIN_18_OBSBUS2           8
#define AM_HAL_PIN_18_UART1TX           9
#define AM_HAL_PIN_18_UART0CTS          10
#define AM_HAL_PIN_18_FPIO              11
#define AM_HAL_PIN_18_RESERVED12        12
#define AM_HAL_PIN_18_RESERVED13        13
#define AM_HAL_PIN_18_MDA_SRST          14
#define AM_HAL_PIN_18_RESERVED15        15

// PIN 19 functions
#define AM_HAL_PIN_19_ADCSE0            0
#define AM_HAL_PIN_19_ANATEST1          1
#define AM_HAL_PIN_19_RESERVED2         2
#define AM_HAL_PIN_19_GPIO              3
#define AM_HAL_PIN_19_TRIG0             4
#define AM_HAL_PIN_19_AO_OBSBUS3        5
#define AM_HAL_PIN_19_CT19              6
#define AM_HAL_PIN_19_NCE19             7
#define AM_HAL_PIN_19_OBSBUS3           8
#define AM_HAL_PIN_19_UART1RX           9
#define AM_HAL_PIN_19_UART0RTS          10
#define AM_HAL_PIN_19_FPIO              11
#define AM_HAL_PIN_19_RESERVED12        12
#define AM_HAL_PIN_19_FLLOAD_STRB       13
#define AM_HAL_PIN_19_RESERVED14        14
#define AM_HAL_PIN_19_DFT_ISO           15

// PIN 20 functions
#define AM_HAL_PIN_20_SWDCK             0
#define AM_HAL_PIN_20_RESERVED1         1
#define AM_HAL_PIN_20_RESERVED2         2
#define AM_HAL_PIN_20_GPIO              3
#define AM_HAL_PIN_20_TRIG1             4
#define AM_HAL_PIN_20_RESERVED5         5
#define AM_HAL_PIN_20_CT20              6
#define AM_HAL_PIN_20_NCE20             7
#define AM_HAL_PIN_20_OBSBUS4           8
#define AM_HAL_PIN_20_UART1TX           9
#define AM_HAL_PIN_20_UART0CTS          10
#define AM_HAL_PIN_20_FPIO              11
#define AM_HAL_PIN_20_RESERVED12        12
#define AM_HAL_PIN_20_FLB_TCLK          13
#define AM_HAL_PIN_20_MDA_TCK           14
#define AM_HAL_PIN_20_SCANCLK           15

// PIN 21 functions
#define AM_HAL_PIN_21_SWDIO             0
#define AM_HAL_PIN_21_RESERVED1         1
#define AM_HAL_PIN_21_RESERVED2         2
#define AM_HAL_PIN_21_GPIO              3
#define AM_HAL_PIN_21_TRIG2             4
#define AM_HAL_PIN_21_RESERVED5         5
#define AM_HAL_PIN_21_CT21              6
#define AM_HAL_PIN_21_NCE21             7
#define AM_HAL_PIN_21_OBSBUS5           8
#define AM_HAL_PIN_21_UART1RX           9
#define AM_HAL_PIN_21_UART0RTS          10
#define AM_HAL_PIN_21_FPIO              11
#define AM_HAL_PIN_21_RESERVED12        12
#define AM_HAL_PIN_21_FLB_TDO           13
#define AM_HAL_PIN_21_MDA_TDO           14
#define AM_HAL_PIN_21_SCANSHFT          15

// PIN 22 functions
#define AM_HAL_PIN_22_M5SCL             0
#define AM_HAL_PIN_22_M5SCK             1
#define AM_HAL_PIN_22_RESERVED2         2
#define AM_HAL_PIN_22_GPIO              3
#define AM_HAL_PIN_22_TRIG3             4
#define AM_HAL_PIN_22_SWO               5
#define AM_HAL_PIN_22_CT22              6
#define AM_HAL_PIN_22_NCE22             7
#define AM_HAL_PIN_22_OBSBUS6           8
#define AM_HAL_PIN_22_UART0TX           9
#define AM_HAL_PIN_22_UART1CTS          10
#define AM_HAL_PIN_22_FPIO              11
#define AM_HAL_PIN_22_RF_XCVR_TST0      12
#define AM_HAL_PIN_22_RESERVED13        13
#define AM_HAL_PIN_22_RESERVED14        14
#define AM_HAL_PIN_22_OPCG_LOAD         15

// PIN 23 functions
#define AM_HAL_PIN_23_M5SDAWIR3         0
#define AM_HAL_PIN_23_M5MOSI            1
#define AM_HAL_PIN_23_BTDM_RXEN         2
#define AM_HAL_PIN_23_GPIO              3
#define AM_HAL_PIN_23_TRIG0             4
#define AM_HAL_PIN_23_SWO               5
#define AM_HAL_PIN_23_CT23              6
#define AM_HAL_PIN_23_NCE23             7
#define AM_HAL_PIN_23_OBSBUS7           8
#define AM_HAL_PIN_23_UART0RX           9
#define AM_HAL_PIN_23_UART1RTS          10
#define AM_HAL_PIN_23_FPIO              11
#define AM_HAL_PIN_23_ST_WCI_2W_RXD     12
#define AM_HAL_PIN_23_RESERVED13        13
#define AM_HAL_PIN_23_RESERVED14        14
#define AM_HAL_PIN_23_LPG_LOAD          15

// PIN 24 functions
#define AM_HAL_PIN_24_M5MISO            0
#define AM_HAL_PIN_24_RESERVED1         1
#define AM_HAL_PIN_24_BTDM_TXEN         2
#define AM_HAL_PIN_24_GPIO              3
#define AM_HAL_PIN_24_TRIG1             4
#define AM_HAL_PIN_24_SWO               5
#define AM_HAL_PIN_24_CT24              6
#define AM_HAL_PIN_24_NCE24             7
#define AM_HAL_PIN_24_OBSBUS8           8
#define AM_HAL_PIN_24_UART1TX           9
#define AM_HAL_PIN_24_UART0CTS          10
#define AM_HAL_PIN_24_FPIO              11
#define AM_HAL_PIN_24_ST_WCI_1W_RXD     12
#define AM_HAL_PIN_24_ST_WCI_2W_TXD     13
#define AM_HAL_PIN_24_RESERVED14        14
#define AM_HAL_PIN_24_EN96MHz           15

// PIN 25 functions
#define AM_HAL_PIN_25_M2SCL             0
#define AM_HAL_PIN_25_M2SCK             1
#define AM_HAL_PIN_25_VCMPO             2
#define AM_HAL_PIN_25_GPIO              3
#define AM_HAL_PIN_25_TRIG2             4
#define AM_HAL_PIN_25_RESERVED5         5
#define AM_HAL_PIN_25_CT25              6
#define AM_HAL_PIN_25_NCE25             7
#define AM_HAL_PIN_25_OBSBUS9           8
#define AM_HAL_PIN_25_UART1RX           9
#define AM_HAL_PIN_25_UART0RTS          10
#define AM_HAL_PIN_25_FPIO              11
#define AM_HAL_PIN_25_RESERVED12        12
#define AM_HAL_PIN_25_RESERVED13        13
#define AM_HAL_PIN_25_RESERVED14        14
#define AM_HAL_PIN_25_CLK96MHz          15

// PIN 26 functions
#define AM_HAL_PIN_26_M2SDAWIR3         0
#define AM_HAL_PIN_26_M2MOSI            1
#define AM_HAL_PIN_26_RESERVED2         2
#define AM_HAL_PIN_26_GPIO              3
#define AM_HAL_PIN_26_TRIG3             4
#define AM_HAL_PIN_26_SWTRACECTL        5
#define AM_HAL_PIN_26_CT26              6
#define AM_HAL_PIN_26_NCE26             7
#define AM_HAL_PIN_26_OBSBUS10          8
#define AM_HAL_PIN_26_UART0TX           9
#define AM_HAL_PIN_26_UART1CTS          10
#define AM_HAL_PIN_26_FPIO              11
#define AM_HAL_PIN_26_RESERVED12        12
#define AM_HAL_PIN_26_RESERVED13        13
#define AM_HAL_PIN_26_RESERVED14        14
#define AM_HAL_PIN_26_CME               15

// PIN 27 functions
#define AM_HAL_PIN_27_M2MISO            0
#define AM_HAL_PIN_27_I2S0_MCLK         1
#define AM_HAL_PIN_27_VCMPO             2
#define AM_HAL_PIN_27_GPIO              3
#define AM_HAL_PIN_27_TRIG0             4
#define AM_HAL_PIN_27_RESERVED5         5
#define AM_HAL_PIN_27_CT27              6
#define AM_HAL_PIN_27_NCE27             7
#define AM_HAL_PIN_27_OBSBUS11          8
#define AM_HAL_PIN_27_UART0RX           9
#define AM_HAL_PIN_27_UART1RTS          10
#define AM_HAL_PIN_27_FPIO              11
#define AM_HAL_PIN_27_RESERVED12        12
#define AM_HAL_PIN_27_RESERVED13        13
#define AM_HAL_PIN_27_RESERVED14        14
#define AM_HAL_PIN_27_CMLE              15

// PIN 28 functions
#define AM_HAL_PIN_28_SWO               0
#define AM_HAL_PIN_28_SWTRACECTL        1
#define AM_HAL_PIN_28_RESERVED2         2
#define AM_HAL_PIN_28_GPIO              3
#define AM_HAL_PIN_28_TRIG1             4
#define AM_HAL_PIN_28_RESERVED5         5
#define AM_HAL_PIN_28_CT28              6
#define AM_HAL_PIN_28_NCE28             7
#define AM_HAL_PIN_28_OBSBUS12          8
#define AM_HAL_PIN_28_UART1RX           9
#define AM_HAL_PIN_28_UART0RTS          10
#define AM_HAL_PIN_28_FPIO              11
#define AM_HAL_PIN_28_RESERVED12        12
#define AM_HAL_PIN_28_RESERVED13        13
#define AM_HAL_PIN_28_RESERVED14        14
#define AM_HAL_PIN_28_RESERVED15        15

// PIN 29 functions
#define AM_HAL_PIN_29_M3SCL             0
#define AM_HAL_PIN_29_M3SCK             1
#define AM_HAL_PIN_29_RESERVED2         2
#define AM_HAL_PIN_29_GPIO              3
#define AM_HAL_PIN_29_TRIG2             4
#define AM_HAL_PIN_29_RESERVED5         5
#define AM_HAL_PIN_29_CT29              6
#define AM_HAL_PIN_29_NCE29             7
#define AM_HAL_PIN_29_OBSBUS13          8
#define AM_HAL_PIN_29_UART1TX           9
#define AM_HAL_PIN_29_UART0CTS          10
#define AM_HAL_PIN_29_FPIO              11
#define AM_HAL_PIN_29_I2S0_SDIN         12
#define AM_HAL_PIN_29_RESERVED13        13
#define AM_HAL_PIN_29_MDA_HFRC_EXT      14
#define AM_HAL_PIN_29_DFT_RET           15

// PIN 30 functions
#define AM_HAL_PIN_30_M3SDAWIR3         0
#define AM_HAL_PIN_30_M3MOSI            1
#define AM_HAL_PIN_30_PDMI2S0_SDOUT     2
#define AM_HAL_PIN_30_GPIO              3
#define AM_HAL_PIN_30_TRIG3             4
#define AM_HAL_PIN_30_RESERVED5         5
#define AM_HAL_PIN_30_CT30              6
#define AM_HAL_PIN_30_NCE30             7
#define AM_HAL_PIN_30_OBSBUS14          8
#define AM_HAL_PIN_30_UART0TX           9
#define AM_HAL_PIN_30_UART1CTS          10
#define AM_HAL_PIN_30_FPIO              11
#define AM_HAL_PIN_30_I2S0_SDOUT        12
#define AM_HAL_PIN_30_I2S0_DATA         13
#define AM_HAL_PIN_30_RESERVED14        14
#define AM_HAL_PIN_30_SCANOUT0          15

// PIN 31 functions
#define AM_HAL_PIN_31_M3MISO            0
#define AM_HAL_PIN_31_CLKOUT            1
#define AM_HAL_PIN_31_PDMI2S0_WS        2
#define AM_HAL_PIN_31_GPIO              3
#define AM_HAL_PIN_31_TRIG0             4
#define AM_HAL_PIN_31_COEX_GRANT        5
#define AM_HAL_PIN_31_CT31              6
#define AM_HAL_PIN_31_NCE31             7
#define AM_HAL_PIN_31_OBSBUS15          8
#define AM_HAL_PIN_31_UART0RX           9
#define AM_HAL_PIN_31_UART1RTS          10
#define AM_HAL_PIN_31_FPIO              11
#define AM_HAL_PIN_31_I2S0_WS           12
#define AM_HAL_PIN_31_RESERVED13        13
#define AM_HAL_PIN_31_RESERVED14        14
#define AM_HAL_PIN_31_SCANOUT1          15

// PIN 32 functions
#define AM_HAL_PIN_32_M4SCL             0
#define AM_HAL_PIN_32_M4SCK             1
#define AM_HAL_PIN_32_RESERVED2         2
#define AM_HAL_PIN_32_GPIO              3
#define AM_HAL_PIN_32_TRIG1             4
#define AM_HAL_PIN_32_COEX_REQ          5
#define AM_HAL_PIN_32_CT32              6
#define AM_HAL_PIN_32_NCE32             7
#define AM_HAL_PIN_32_OBSBUS0           8
#define AM_HAL_PIN_32_UART1TX           9
#define AM_HAL_PIN_32_UART0CTS          10
#define AM_HAL_PIN_32_FPIO              11
#define AM_HAL_PIN_32_RESERVED12        12
#define AM_HAL_PIN_32_FLLOAD_DIR        13
#define AM_HAL_PIN_32_RESERVED14        14
#define AM_HAL_PIN_32_SCANOUT2          15

// PIN 33 functions
#define AM_HAL_PIN_33_M4SDAWIR3         0
#define AM_HAL_PIN_33_M4MOSI            1
#define AM_HAL_PIN_33_RESERVED2         2
#define AM_HAL_PIN_33_GPIO              3
#define AM_HAL_PIN_33_TRIG2             4
#define AM_HAL_PIN_33_COEX_PRIORITY     5
#define AM_HAL_PIN_33_CT33              6
#define AM_HAL_PIN_33_NCE33             7
#define AM_HAL_PIN_33_OBSBUS1           8
#define AM_HAL_PIN_33_UART1RX           9
#define AM_HAL_PIN_33_UART0RTS          10
#define AM_HAL_PIN_33_FPIO              11
#define AM_HAL_PIN_33_RESERVED12        12
#define AM_HAL_PIN_33_FLLOAD_ADDR0      13
#define AM_HAL_PIN_33_RESERVED14        14
#define AM_HAL_PIN_33_SCANOUT3          15

// PIN 34 functions
#define AM_HAL_PIN_34_M4MISO            0
#define AM_HAL_PIN_34_SWO               1
#define AM_HAL_PIN_34_RESERVED2         2
#define AM_HAL_PIN_34_GPIO              3
#define AM_HAL_PIN_34_TRIG3             4
#define AM_HAL_PIN_34_COEX_FREQ         5
#define AM_HAL_PIN_34_CT34              6
#define AM_HAL_PIN_34_NCE34             7
#define AM_HAL_PIN_34_OBSBUS2           8
#define AM_HAL_PIN_34_UART0TX           9
#define AM_HAL_PIN_34_UART1CTS          10
#define AM_HAL_PIN_34_FPIO              11
#define AM_HAL_PIN_34_RESERVED12        12
#define AM_HAL_PIN_34_FLLOAD_ADDR1      13
#define AM_HAL_PIN_34_RESERVED14        14
#define AM_HAL_PIN_34_SCANOUT4          15

// PIN 35 functions
#define AM_HAL_PIN_35_MSPI0_8           0
#define AM_HAL_PIN_35_SWTRACECLK        1
#define AM_HAL_PIN_35_RESERVED2         2
#define AM_HAL_PIN_35_GPIO              3
#define AM_HAL_PIN_35_RESERVED4         4
#define AM_HAL_PIN_35_RESERVED5         5
#define AM_HAL_PIN_35_CT35              6
#define AM_HAL_PIN_35_NCE35             7
#define AM_HAL_PIN_35_OBSBUS3           8
#define AM_HAL_PIN_35_UART0RX           9
#define AM_HAL_PIN_35_UART1RTS          10
#define AM_HAL_PIN_35_FPIO              11
#define AM_HAL_PIN_35_RESERVED12        12
#define AM_HAL_PIN_35_FLLOAD_ADDR2      13
#define AM_HAL_PIN_35_RESERVED14        14
#define AM_HAL_PIN_35_SCANOUT5          15

// PIN 36 functions
#define AM_HAL_PIN_36_MSPI0_0           0
#define AM_HAL_PIN_36_SWTRACE0          1
#define AM_HAL_PIN_36_RESERVED2         2
#define AM_HAL_PIN_36_GPIO              3
#define AM_HAL_PIN_36_RESERVED4         4
#define AM_HAL_PIN_36_RESERVED5         5
#define AM_HAL_PIN_36_CT36              6
#define AM_HAL_PIN_36_NCE36             7
#define AM_HAL_PIN_36_OBSBUS4           8
#define AM_HAL_PIN_36_UART1TX           9
#define AM_HAL_PIN_36_UART0CTS          10
#define AM_HAL_PIN_36_FPIO              11
#define AM_HAL_PIN_36_RESERVED12        12
#define AM_HAL_PIN_36_FLLOAD_DATA0      13
#define AM_HAL_PIN_36_RESERVED14        14
#define AM_HAL_PIN_36_SCANOUT6          15

// PIN 37 functions
#define AM_HAL_PIN_37_MSPI0_1           0
#define AM_HAL_PIN_37_SWTRACE1          1
#define AM_HAL_PIN_37_RESERVED2         2
#define AM_HAL_PIN_37_GPIO              3
#define AM_HAL_PIN_37_RESERVED4         4
#define AM_HAL_PIN_37_32KHzXT           5
#define AM_HAL_PIN_37_CT37              6
#define AM_HAL_PIN_37_NCE37             7
#define AM_HAL_PIN_37_OBSBUS5           8
#define AM_HAL_PIN_37_UART1RX           9
#define AM_HAL_PIN_37_UART0RTS          10
#define AM_HAL_PIN_37_FPIO              11
#define AM_HAL_PIN_37_RESERVED12        12
#define AM_HAL_PIN_37_FLLOAD_DATA1      13
#define AM_HAL_PIN_37_RESERVED14        14
#define AM_HAL_PIN_37_SCANOUT7          15

// PIN 38 functions
#define AM_HAL_PIN_38_MSPI0_2           0
#define AM_HAL_PIN_38_SWTRACE2          1
#define AM_HAL_PIN_38_RESERVED2         2
#define AM_HAL_PIN_38_GPIO              3
#define AM_HAL_PIN_38_RESERVED4         4
#define AM_HAL_PIN_38_RESERVED5         5
#define AM_HAL_PIN_38_CT38              6
#define AM_HAL_PIN_38_NCE38             7
#define AM_HAL_PIN_38_OBSBUS6           8
#define AM_HAL_PIN_38_RESERVED9         9
#define AM_HAL_PIN_38_RESERVED10        10
#define AM_HAL_PIN_38_FPIO              11
#define AM_HAL_PIN_38_RESERVED12        12
#define AM_HAL_PIN_38_FLLOAD_DATA2      13
#define AM_HAL_PIN_38_RESERVED14        14
#define AM_HAL_PIN_38_SCANOUT8          15

// PIN 39 functions
#define AM_HAL_PIN_39_MSPI0_3           0
#define AM_HAL_PIN_39_SWTRACE3          1
#define AM_HAL_PIN_39_RESERVED2         2
#define AM_HAL_PIN_39_GPIO              3
#define AM_HAL_PIN_39_RESERVED4         4
#define AM_HAL_PIN_39_RESERVED5         5
#define AM_HAL_PIN_39_CT39              6
#define AM_HAL_PIN_39_NCE39             7
#define AM_HAL_PIN_39_OBSBUS7           8
#define AM_HAL_PIN_39_RESERVED9         9
#define AM_HAL_PIN_39_RESERVED10        10
#define AM_HAL_PIN_39_FPIO              11
#define AM_HAL_PIN_39_RESERVED12        12
#define AM_HAL_PIN_39_FLLOAD_DATA3      13
#define AM_HAL_PIN_39_RESERVED14        14
#define AM_HAL_PIN_39_SCANOUT9          15

// PIN 40 functions
#define AM_HAL_PIN_40_MNCE1_0           0
#define AM_HAL_PIN_40_RESERVED1         1
#define AM_HAL_PIN_40_RESERVED2         2
#define AM_HAL_PIN_40_GPIO              3
#define AM_HAL_PIN_40_RESERVED4         4
#define AM_HAL_PIN_40_RESERVED5         5
#define AM_HAL_PIN_40_CT40              6
#define AM_HAL_PIN_40_NCE40             7
#define AM_HAL_PIN_40_OBSBUS8           8
#define AM_HAL_PIN_40_UART1TX           9
#define AM_HAL_PIN_40_UART0CTS          10
#define AM_HAL_PIN_40_FPIO              11
#define AM_HAL_PIN_40_RESERVED12        12
#define AM_HAL_PIN_40_RESERVED13        13
#define AM_HAL_PIN_40_RESERVED14        14
#define AM_HAL_PIN_40_RESERVED15        15

// PIN 41 functions
#define AM_HAL_PIN_41_MNCE2_0           0
#define AM_HAL_PIN_41_RESERVED1         1
#define AM_HAL_PIN_41_RESERVED2         2
#define AM_HAL_PIN_41_GPIO              3
#define AM_HAL_PIN_41_RESERVED4         4
#define AM_HAL_PIN_41_RESERVED5         5
#define AM_HAL_PIN_41_CT41              6
#define AM_HAL_PIN_41_NCE41             7
#define AM_HAL_PIN_41_OBSBUS9           8
#define AM_HAL_PIN_41_UART1RX           9
#define AM_HAL_PIN_41_UART0RTS          10
#define AM_HAL_PIN_41_FPIO              11
#define AM_HAL_PIN_41_RESERVED12        12
#define AM_HAL_PIN_41_RESERVED13        13
#define AM_HAL_PIN_41_RESERVED14        14
#define AM_HAL_PIN_41_RESERVED15        15

// PIN 42 functions
#define AM_HAL_PIN_42_MNCE1_1           0
#define AM_HAL_PIN_42_DISP_TE           1
#define AM_HAL_PIN_42_RESERVED2         2
#define AM_HAL_PIN_42_GPIO              3
#define AM_HAL_PIN_42_ST_WCI_1W_RXD     4
#define AM_HAL_PIN_42_ST_WCI_2W_TXD     5
#define AM_HAL_PIN_42_CT42              6
#define AM_HAL_PIN_42_NCE42             7
#define AM_HAL_PIN_42_OBSBUS10          8
#define AM_HAL_PIN_42_RESERVED9         9
#define AM_HAL_PIN_42_RESERVED10        10
#define AM_HAL_PIN_42_FPIO              11
#define AM_HAL_PIN_42_BTDM_RXEN         12
#define AM_HAL_PIN_42_RESERVED13        13
#define AM_HAL_PIN_42_RESERVED14        14
#define AM_HAL_PIN_42_RESERVED15        15

// PIN 43 functions
#define AM_HAL_PIN_43_CM4_SWDCK         0
#define AM_HAL_PIN_43_SDIF1_DAT4        1
#define AM_HAL_PIN_43_COEX_GRANT        2
#define AM_HAL_PIN_43_GPIO              3
#define AM_HAL_PIN_43_PDMI2S0_SDOUT     4
#define AM_HAL_PIN_43_RESERVED5         5
#define AM_HAL_PIN_43_CT43              6
#define AM_HAL_PIN_43_NCE43             7
#define AM_HAL_PIN_43_OBSBUS11          8
#define AM_HAL_PIN_43_UART1TX           9
#define AM_HAL_PIN_43_UART0CTS          10
#define AM_HAL_PIN_43_FPIO              11
#define AM_HAL_PIN_43_I2S0_SDOUT        12
#define AM_HAL_PIN_43_I2S0_DATA         13
#define AM_HAL_PIN_43_RESERVED14        14
#define AM_HAL_PIN_43_RESERVED15        15

// PIN 44 functions
#define AM_HAL_PIN_44_CM4_SWDIO         0
#define AM_HAL_PIN_44_SDIF1_DAT5        1
#define AM_HAL_PIN_44_COEX_REQ          2
#define AM_HAL_PIN_44_GPIO              3
#define AM_HAL_PIN_44_PDMI2S0_WS        4
#define AM_HAL_PIN_44_SWO               5
#define AM_HAL_PIN_44_CT44              6
#define AM_HAL_PIN_44_NCE44             7
#define AM_HAL_PIN_44_OBSBUS12          8
#define AM_HAL_PIN_44_UART1RX           9
#define AM_HAL_PIN_44_UART0RTS          10
#define AM_HAL_PIN_44_FPIO              11
#define AM_HAL_PIN_44_I2S0_WS           12
#define AM_HAL_PIN_44_RESERVED13        13
#define AM_HAL_PIN_44_RESERVED14        14
#define AM_HAL_PIN_44_RESERVED15        15

// PIN 45 functions
#define AM_HAL_PIN_45_CM4_SWO           0
#define AM_HAL_PIN_45_SDIF1_DAT6        1
#define AM_HAL_PIN_45_COEX_PRIORITY     2
#define AM_HAL_PIN_45_GPIO              3
#define AM_HAL_PIN_45_RESERVED4         4
#define AM_HAL_PIN_45_32KHzXT           5
#define AM_HAL_PIN_45_CT45              6
#define AM_HAL_PIN_45_NCE45             7
#define AM_HAL_PIN_45_OBSBUS13          8
#define AM_HAL_PIN_45_UART1TX           9
#define AM_HAL_PIN_45_UART0CTS          10
#define AM_HAL_PIN_45_FPIO              11
#define AM_HAL_PIN_45_I2S0_SDIN         12
#define AM_HAL_PIN_45_RESERVED13        13
#define AM_HAL_PIN_45_RESERVED14        14
#define AM_HAL_PIN_45_RESERVED15        15

// PIN 46 functions
#define AM_HAL_PIN_46_I2S0_MCLK         0
#define AM_HAL_PIN_46_SDIF1_DAT7        1
#define AM_HAL_PIN_46_COEX_FREQ         2
#define AM_HAL_PIN_46_GPIO              3
#define AM_HAL_PIN_46_TRIG3             4
#define AM_HAL_PIN_46_SWO               5
#define AM_HAL_PIN_46_CT46              6
#define AM_HAL_PIN_46_NCE46             7
#define AM_HAL_PIN_46_OBSBUS14          8
#define AM_HAL_PIN_46_UART1RX           9
#define AM_HAL_PIN_46_UART0RTS          10
#define AM_HAL_PIN_46_FPIO              11
#define AM_HAL_PIN_46_RESERVED12        12
#define AM_HAL_PIN_46_RESERVED13        13
#define AM_HAL_PIN_46_RESERVED14        14
#define AM_HAL_PIN_46_RESERVED15        15

// PIN 47 functions
#define AM_HAL_PIN_47_MSPI1_4           0
#define AM_HAL_PIN_47_SWTRACECLK        1
#define AM_HAL_PIN_47_CLKOUT            2
#define AM_HAL_PIN_47_GPIO              3
#define AM_HAL_PIN_47_TRIG0             4
#define AM_HAL_PIN_47_RESERVED5         5
#define AM_HAL_PIN_47_CT47              6
#define AM_HAL_PIN_47_NCE47             7
#define AM_HAL_PIN_47_OBSBUS15          8
#define AM_HAL_PIN_47_RESERVED9         9
#define AM_HAL_PIN_47_RESERVED10        10
#define AM_HAL_PIN_47_FPIO              11
#define AM_HAL_PIN_47_RESERVED12        12
#define AM_HAL_PIN_47_RESERVED13        13
#define AM_HAL_PIN_47_RESERVED14        14
#define AM_HAL_PIN_47_RESERVED15        15

// PIN 48 functions
#define AM_HAL_PIN_48_MSPI1_0           0
#define AM_HAL_PIN_48_SWTRACE0          1
#define AM_HAL_PIN_48_CLKOUT            2
#define AM_HAL_PIN_48_GPIO              3
#define AM_HAL_PIN_48_TRIG1             4
#define AM_HAL_PIN_48_RESERVED5         5
#define AM_HAL_PIN_48_CT48              6
#define AM_HAL_PIN_48_NCE48             7
#define AM_HAL_PIN_48_OBSBUS0           8
#define AM_HAL_PIN_48_UART0TX           9
#define AM_HAL_PIN_48_UART1CTS          10
#define AM_HAL_PIN_48_FPIO              11
#define AM_HAL_PIN_48_RESERVED12        12
#define AM_HAL_PIN_48_RESERVED13        13
#define AM_HAL_PIN_48_RESERVED14        14
#define AM_HAL_PIN_48_RESERVED15        15

// PIN 49 functions
#define AM_HAL_PIN_49_MSPI1_1           0
#define AM_HAL_PIN_49_SWTRACE1          1
#define AM_HAL_PIN_49_RESERVED2         2
#define AM_HAL_PIN_49_GPIO              3
#define AM_HAL_PIN_49_TRIG2             4
#define AM_HAL_PIN_49_RESERVED5         5
#define AM_HAL_PIN_49_CT49              6
#define AM_HAL_PIN_49_NCE49             7
#define AM_HAL_PIN_49_OBSBUS1           8
#define AM_HAL_PIN_49_UART0RX           9
#define AM_HAL_PIN_49_UART1RTS          10
#define AM_HAL_PIN_49_FPIO              11
#define AM_HAL_PIN_49_RESERVED12        12
#define AM_HAL_PIN_49_RESERVED13        13
#define AM_HAL_PIN_49_RESERVED14        14
#define AM_HAL_PIN_49_RESERVED15        15

// PIN 50 functions
#define AM_HAL_PIN_50_MSPI1_2           0
#define AM_HAL_PIN_50_SWTRACE2          1
#define AM_HAL_PIN_50_RESERVED2         2
#define AM_HAL_PIN_50_GPIO              3
#define AM_HAL_PIN_50_TRIG3             4
#define AM_HAL_PIN_50_RESERVED5         5
#define AM_HAL_PIN_50_CT50              6
#define AM_HAL_PIN_50_NCE50             7
#define AM_HAL_PIN_50_OBSBUS2           8
#define AM_HAL_PIN_50_UART1TX           9
#define AM_HAL_PIN_50_UART0CTS          10
#define AM_HAL_PIN_50_FPIO              11
#define AM_HAL_PIN_50_RESERVED12        12
#define AM_HAL_PIN_50_RESERVED13        13
#define AM_HAL_PIN_50_RESERVED14        14
#define AM_HAL_PIN_50_RESERVED15        15

// PIN 51 functions
#define AM_HAL_PIN_51_MSPI1_3           0
#define AM_HAL_PIN_51_SWTRACE3          1
#define AM_HAL_PIN_51_RESERVED2         2
#define AM_HAL_PIN_51_GPIO              3
#define AM_HAL_PIN_51_TRIG0             4
#define AM_HAL_PIN_51_RESERVED5         5
#define AM_HAL_PIN_51_CT51              6
#define AM_HAL_PIN_51_NCE51             7
#define AM_HAL_PIN_51_OBSBUS3           8
#define AM_HAL_PIN_51_UART1RX           9
#define AM_HAL_PIN_51_UART0RTS          10
#define AM_HAL_PIN_51_FPIO              11
#define AM_HAL_PIN_51_RESERVED12        12
#define AM_HAL_PIN_51_RESERVED13        13
#define AM_HAL_PIN_51_RESERVED14        14
#define AM_HAL_PIN_51_RESERVED15        15

// PIN 52 functions
#define AM_HAL_PIN_52_MSPI1_5           0
#define AM_HAL_PIN_52_SWTRACECTL        1
#define AM_HAL_PIN_52_CLKOUT            2
#define AM_HAL_PIN_52_GPIO              3
#define AM_HAL_PIN_52_TRIG1             4
#define AM_HAL_PIN_52_RESERVED5         5
#define AM_HAL_PIN_52_CT52              6
#define AM_HAL_PIN_52_NCE52             7
#define AM_HAL_PIN_52_OBSBUS4           8
#define AM_HAL_PIN_52_UART0TX           9
#define AM_HAL_PIN_52_UART1CTS          10
#define AM_HAL_PIN_52_FPIO              11
#define AM_HAL_PIN_52_RESERVED12        12
#define AM_HAL_PIN_52_RESERVED13        13
#define AM_HAL_PIN_52_RESERVED14        14
#define AM_HAL_PIN_52_RESERVED15        15

// PIN 53 functions
#define AM_HAL_PIN_53_MSPI2_8           0
#define AM_HAL_PIN_53_RESERVED1         1
#define AM_HAL_PIN_53_RESERVED2         2
#define AM_HAL_PIN_53_GPIO              3
#define AM_HAL_PIN_53_RESERVED4         4
#define AM_HAL_PIN_53_SWO               5
#define AM_HAL_PIN_53_CT53              6
#define AM_HAL_PIN_53_NCE53             7
#define AM_HAL_PIN_53_OBSBUS5           8
#define AM_HAL_PIN_53_UART0RX           9
#define AM_HAL_PIN_53_UART1RTS          10
#define AM_HAL_PIN_53_FPIO              11
#define AM_HAL_PIN_53_RESERVED12        12
#define AM_HAL_PIN_53_RESERVED13        13
#define AM_HAL_PIN_53_RESERVED14        14
#define AM_HAL_PIN_53_RESERVED15        15

// PIN 54 functions
#define AM_HAL_PIN_54_MSPI2_0           0
#define AM_HAL_PIN_54_RESERVED1         1
#define AM_HAL_PIN_54_32KHzXT           2
#define AM_HAL_PIN_54_GPIO              3
#define AM_HAL_PIN_54_RESERVED4         4
#define AM_HAL_PIN_54_RESERVED5         5
#define AM_HAL_PIN_54_CT54              6
#define AM_HAL_PIN_54_NCE54             7
#define AM_HAL_PIN_54_OBSBUS6           8
#define AM_HAL_PIN_54_UART1TX           9
#define AM_HAL_PIN_54_UART0CTS          10
#define AM_HAL_PIN_54_FPIO              11
#define AM_HAL_PIN_54_RESERVED12        12
#define AM_HAL_PIN_54_RESERVED13        13
#define AM_HAL_PIN_54_RESERVED14        14
#define AM_HAL_PIN_54_RESERVED15        15

// PIN 55 functions
#define AM_HAL_PIN_55_MSPI2_1           0
#define AM_HAL_PIN_55_RESERVED1         1
#define AM_HAL_PIN_55_32KHzXT           2
#define AM_HAL_PIN_55_GPIO              3
#define AM_HAL_PIN_55_RESERVED4         4
#define AM_HAL_PIN_55_RESERVED5         5
#define AM_HAL_PIN_55_CT55              6
#define AM_HAL_PIN_55_NCE55             7
#define AM_HAL_PIN_55_OBSBUS7           8
#define AM_HAL_PIN_55_UART1RX           9
#define AM_HAL_PIN_55_UART0RTS          10
#define AM_HAL_PIN_55_FPIO              11
#define AM_HAL_PIN_55_RESERVED12        12
#define AM_HAL_PIN_55_RESERVED13        13
#define AM_HAL_PIN_55_RESERVED14        14
#define AM_HAL_PIN_55_RESERVED15        15

// PIN 56 functions
#define AM_HAL_PIN_56_MSPI2_2           0
#define AM_HAL_PIN_56_RESERVED1         1
#define AM_HAL_PIN_56_CLKOUT            2
#define AM_HAL_PIN_56_GPIO              3
#define AM_HAL_PIN_56_RESERVED4         4
#define AM_HAL_PIN_56_RESERVED5         5
#define AM_HAL_PIN_56_CT56              6
#define AM_HAL_PIN_56_NCE56             7
#define AM_HAL_PIN_56_OBSBUS8           8
#define AM_HAL_PIN_56_UART0TX           9
#define AM_HAL_PIN_56_UART1CTS          10
#define AM_HAL_PIN_56_FPIO              11
#define AM_HAL_PIN_56_RESERVED12        12
#define AM_HAL_PIN_56_RESERVED13        13
#define AM_HAL_PIN_56_RESERVED14        14
#define AM_HAL_PIN_56_RESERVED15        15

// PIN 57 functions
#define AM_HAL_PIN_57_MSPI2_3           0
#define AM_HAL_PIN_57_RESERVED1         1
#define AM_HAL_PIN_57_CLKOUT            2
#define AM_HAL_PIN_57_GPIO              3
#define AM_HAL_PIN_57_RESERVED4         4
#define AM_HAL_PIN_57_RESERVED5         5
#define AM_HAL_PIN_57_CT57              6
#define AM_HAL_PIN_57_NCE57             7
#define AM_HAL_PIN_57_OBSBUS9           8
#define AM_HAL_PIN_57_UART0RX           9
#define AM_HAL_PIN_57_UART1RTS          10
#define AM_HAL_PIN_57_FPIO              11
#define AM_HAL_PIN_57_RESERVED12        12
#define AM_HAL_PIN_57_RESERVED13        13
#define AM_HAL_PIN_57_RESERVED14        14
#define AM_HAL_PIN_57_RESERVED15        15

// PIN 58 functions
#define AM_HAL_PIN_58_MSPI2_9           0
#define AM_HAL_PIN_58_RESERVED1         1
#define AM_HAL_PIN_58_RESERVED2         2
#define AM_HAL_PIN_58_GPIO              3
#define AM_HAL_PIN_58_RESERVED4         4
#define AM_HAL_PIN_58_RESERVED5         5
#define AM_HAL_PIN_58_CT58              6
#define AM_HAL_PIN_58_NCE58             7
#define AM_HAL_PIN_58_OBSBUS10          8
#define AM_HAL_PIN_58_UART1TX           9
#define AM_HAL_PIN_58_UART0CTS          10
#define AM_HAL_PIN_58_FPIO              11
#define AM_HAL_PIN_58_RESERVED12        12
#define AM_HAL_PIN_58_RESERVED13        13
#define AM_HAL_PIN_58_RESERVED14        14
#define AM_HAL_PIN_58_RESERVED15        15

// PIN 59 functions
#define AM_HAL_PIN_59_SDIF1_DAT0        0
#define AM_HAL_PIN_59_RESERVED1         1
#define AM_HAL_PIN_59_RESERVED2         2
#define AM_HAL_PIN_59_GPIO              3
#define AM_HAL_PIN_59_TRIG2             4
#define AM_HAL_PIN_59_RESERVED5         5
#define AM_HAL_PIN_59_CT59              6
#define AM_HAL_PIN_59_NCE59             7
#define AM_HAL_PIN_59_OBSBUS11          8
#define AM_HAL_PIN_59_UART1RX           9
#define AM_HAL_PIN_59_UART0RTS          10
#define AM_HAL_PIN_59_FPIO              11
#define AM_HAL_PIN_59_RESERVED12        12
#define AM_HAL_PIN_59_RESERVED13        13
#define AM_HAL_PIN_59_RESERVED14        14
#define AM_HAL_PIN_59_RESERVED15        15

// PIN 60 functions
#define AM_HAL_PIN_60_SDIF1_DAT1        0
#define AM_HAL_PIN_60_RESERVED1         1
#define AM_HAL_PIN_60_32KHzXT           2
#define AM_HAL_PIN_60_GPIO              3
#define AM_HAL_PIN_60_TRIG3             4
#define AM_HAL_PIN_60_SWO               5
#define AM_HAL_PIN_60_CT60              6
#define AM_HAL_PIN_60_NCE60             7
#define AM_HAL_PIN_60_OBSBUS12          8
#define AM_HAL_PIN_60_UART0TX           9
#define AM_HAL_PIN_60_UART1CTS          10
#define AM_HAL_PIN_60_FPIO              11
#define AM_HAL_PIN_60_RESERVED12        12
#define AM_HAL_PIN_60_RESERVED13        13
#define AM_HAL_PIN_60_RESERVED14        14
#define AM_HAL_PIN_60_RESERVED15        15

// PIN 61 functions
#define AM_HAL_PIN_61_SDIF1_DAT2        0
#define AM_HAL_PIN_61_RESERVED1         1
#define AM_HAL_PIN_61_32KHzXT           2
#define AM_HAL_PIN_61_GPIO              3
#define AM_HAL_PIN_61_TRIG0             4
#define AM_HAL_PIN_61_SWO               5
#define AM_HAL_PIN_61_CT61              6
#define AM_HAL_PIN_61_NCE61             7
#define AM_HAL_PIN_61_OBSBUS13          8
#define AM_HAL_PIN_61_UART0RX           9
#define AM_HAL_PIN_61_UART1RTS          10
#define AM_HAL_PIN_61_FPIO              11
#define AM_HAL_PIN_61_RESERVED12        12
#define AM_HAL_PIN_61_RESERVED13        13
#define AM_HAL_PIN_61_RESERVED14        14
#define AM_HAL_PIN_61_RESERVED15        15

// PIN 62 functions
#define AM_HAL_PIN_62_SDIF1_DAT3        0
#define AM_HAL_PIN_62_RESERVED1         1
#define AM_HAL_PIN_62_32KHzXT           2
#define AM_HAL_PIN_62_GPIO              3
#define AM_HAL_PIN_62_TRIG1             4
#define AM_HAL_PIN_62_SWO               5
#define AM_HAL_PIN_62_CT62              6
#define AM_HAL_PIN_62_NCE62             7
#define AM_HAL_PIN_62_OBSBUS14          8
#define AM_HAL_PIN_62_UART1TX           9
#define AM_HAL_PIN_62_UART0CTS          10
#define AM_HAL_PIN_62_FPIO              11
#define AM_HAL_PIN_62_RESERVED12        12
#define AM_HAL_PIN_62_RESERVED13        13
#define AM_HAL_PIN_62_RESERVED14        14
#define AM_HAL_PIN_62_RESERVED15        15

// PIN 63 functions
#define AM_HAL_PIN_63_SDIF1_CLKOUT      0
#define AM_HAL_PIN_63_RESERVED1         1
#define AM_HAL_PIN_63_RESERVED2         2
#define AM_HAL_PIN_63_GPIO              3
#define AM_HAL_PIN_63_TRIG2             4
#define AM_HAL_PIN_63_SWO               5
#define AM_HAL_PIN_63_CT63              6
#define AM_HAL_PIN_63_NCE63             7
#define AM_HAL_PIN_63_OBSBUS15          8
#define AM_HAL_PIN_63_UART1RX           9
#define AM_HAL_PIN_63_UART0RTS          10
#define AM_HAL_PIN_63_FPIO              11
#define AM_HAL_PIN_63_RESERVED12        12
#define AM_HAL_PIN_63_RESERVED13        13
#define AM_HAL_PIN_63_RESERVED14        14
#define AM_HAL_PIN_63_RESERVED15        15

// PIN 64 functions
#define AM_HAL_PIN_64_SDIF1_CMD         0
#define AM_HAL_PIN_64_RESERVED1         1
#define AM_HAL_PIN_64_RESERVED2         2
#define AM_HAL_PIN_64_GPIO              3
#define AM_HAL_PIN_64_TRIG3             4
#define AM_HAL_PIN_64_SWO               5
#define AM_HAL_PIN_64_CT64              6
#define AM_HAL_PIN_64_NCE64             7
#define AM_HAL_PIN_64_OBSBUS0           8
#define AM_HAL_PIN_64_UART0TX           9
#define AM_HAL_PIN_64_UART1CTS          10
#define AM_HAL_PIN_64_FPIO              11
#define AM_HAL_PIN_64_RESERVED12        12
#define AM_HAL_PIN_64_RESERVED13        13
#define AM_HAL_PIN_64_RESERVED14        14
#define AM_HAL_PIN_64_RESERVED15        15

// PIN 65 functions
#define AM_HAL_PIN_65_MSPI0_4           0
#define AM_HAL_PIN_65_RESERVED1         1
#define AM_HAL_PIN_65_CLKOUT            2
#define AM_HAL_PIN_65_GPIO              3
#define AM_HAL_PIN_65_RESERVED4         4
#define AM_HAL_PIN_65_RESERVED5         5
#define AM_HAL_PIN_65_CT65              6
#define AM_HAL_PIN_65_NCE65             7
#define AM_HAL_PIN_65_OBSBUS1           8
#define AM_HAL_PIN_65_UART0TX           9
#define AM_HAL_PIN_65_UART1CTS          10
#define AM_HAL_PIN_65_FPIO              11
#define AM_HAL_PIN_65_RESERVED12        12
#define AM_HAL_PIN_65_RESERVED13        13
#define AM_HAL_PIN_65_RESERVED14        14
#define AM_HAL_PIN_65_RESERVED15        15

// PIN 66 functions
#define AM_HAL_PIN_66_MSPI0_5           0
#define AM_HAL_PIN_66_RESERVED1         1
#define AM_HAL_PIN_66_32KHzXT           2
#define AM_HAL_PIN_66_GPIO              3
#define AM_HAL_PIN_66_RESERVED4         4
#define AM_HAL_PIN_66_RESERVED5         5
#define AM_HAL_PIN_66_CT66              6
#define AM_HAL_PIN_66_NCE66             7
#define AM_HAL_PIN_66_OBSBUS2           8
#define AM_HAL_PIN_66_UART0RX           9
#define AM_HAL_PIN_66_UART1RTS          10
#define AM_HAL_PIN_66_FPIO              11
#define AM_HAL_PIN_66_RESERVED12        12
#define AM_HAL_PIN_66_RESERVED13        13
#define AM_HAL_PIN_66_RESERVED14        14
#define AM_HAL_PIN_66_RESERVED15        15

// PIN 67 functions
#define AM_HAL_PIN_67_MSPI0_6           0
#define AM_HAL_PIN_67_RESERVED1         1
#define AM_HAL_PIN_67_32KHzXT           2
#define AM_HAL_PIN_67_GPIO              3
#define AM_HAL_PIN_67_RESERVED4         4
#define AM_HAL_PIN_67_RESERVED5         5
#define AM_HAL_PIN_67_CT67              6
#define AM_HAL_PIN_67_NCE67             7
#define AM_HAL_PIN_67_OBSBUS3           8
#define AM_HAL_PIN_67_UART1TX           9
#define AM_HAL_PIN_67_UART0CTS          10
#define AM_HAL_PIN_67_FPIO              11
#define AM_HAL_PIN_67_RESERVED12        12
#define AM_HAL_PIN_67_RESERVED13        13
#define AM_HAL_PIN_67_RESERVED14        14
#define AM_HAL_PIN_67_RESERVED15        15

// PIN 68 functions
#define AM_HAL_PIN_68_MSPI0_7           0
#define AM_HAL_PIN_68_RESERVED1         1
#define AM_HAL_PIN_68_RESERVED2         2
#define AM_HAL_PIN_68_GPIO              3
#define AM_HAL_PIN_68_RESERVED4         4
#define AM_HAL_PIN_68_SWO               5
#define AM_HAL_PIN_68_CT68              6
#define AM_HAL_PIN_68_NCE68             7
#define AM_HAL_PIN_68_OBSBUS4           8
#define AM_HAL_PIN_68_UART1RX           9
#define AM_HAL_PIN_68_UART0RTS          10
#define AM_HAL_PIN_68_FPIO              11
#define AM_HAL_PIN_68_RESERVED12        12
#define AM_HAL_PIN_68_RESERVED13        13
#define AM_HAL_PIN_68_RESERVED14        14
#define AM_HAL_PIN_68_RESERVED15        15

// PIN 69 functions
#define AM_HAL_PIN_69_MSPI0_10          0
#define AM_HAL_PIN_69_RESERVED1         1
#define AM_HAL_PIN_69_32KHzXT           2
#define AM_HAL_PIN_69_GPIO              3
#define AM_HAL_PIN_69_MSPI2_0           4
#define AM_HAL_PIN_69_SWO               5
#define AM_HAL_PIN_69_CT69              6
#define AM_HAL_PIN_69_NCE69             7
#define AM_HAL_PIN_69_OBSBUS5           8
#define AM_HAL_PIN_69_UART0RX           9
#define AM_HAL_PIN_69_UART1RTS          10
#define AM_HAL_PIN_69_FPIO              11
#define AM_HAL_PIN_69_RESERVED12        12
#define AM_HAL_PIN_69_RESERVED13        13
#define AM_HAL_PIN_69_RESERVED14        14
#define AM_HAL_PIN_69_RESERVED15        15

// PIN 70 functions
#define AM_HAL_PIN_70_MSPI0_11          0
#define AM_HAL_PIN_70_SWTRACE0          1
#define AM_HAL_PIN_70_RESERVED2         2
#define AM_HAL_PIN_70_GPIO              3
#define AM_HAL_PIN_70_MSPI2_1           4
#define AM_HAL_PIN_70_COEX_GRANT        5
#define AM_HAL_PIN_70_CT70              6
#define AM_HAL_PIN_70_NCE70             7
#define AM_HAL_PIN_70_OBSBUS6           8
#define AM_HAL_PIN_70_UART1TX           9
#define AM_HAL_PIN_70_UART0CTS          10
#define AM_HAL_PIN_70_FPIO              11
#define AM_HAL_PIN_70_RESERVED12        12
#define AM_HAL_PIN_70_RESERVED13        13
#define AM_HAL_PIN_70_RESERVED14        14
#define AM_HAL_PIN_70_RESERVED15        15

// PIN 71 functions
#define AM_HAL_PIN_71_MSPI0_12          0
#define AM_HAL_PIN_71_SWTRACE1          1
#define AM_HAL_PIN_71_RESERVED2         2
#define AM_HAL_PIN_71_GPIO              3
#define AM_HAL_PIN_71_MSPI2_2           4
#define AM_HAL_PIN_71_COEX_REQ          5
#define AM_HAL_PIN_71_CT71              6
#define AM_HAL_PIN_71_NCE71             7
#define AM_HAL_PIN_71_OBSBUS7           8
#define AM_HAL_PIN_71_UART1RX           9
#define AM_HAL_PIN_71_UART0RTS          10
#define AM_HAL_PIN_71_FPIO              11
#define AM_HAL_PIN_71_RESERVED12        12
#define AM_HAL_PIN_71_RESERVED13        13
#define AM_HAL_PIN_71_RESERVED14        14
#define AM_HAL_PIN_71_RESERVED15        15

// PIN 72 functions
#define AM_HAL_PIN_72_MSPI0_13          0
#define AM_HAL_PIN_72_SWTRACE2          1
#define AM_HAL_PIN_72_RESERVED2         2
#define AM_HAL_PIN_72_GPIO              3
#define AM_HAL_PIN_72_MSPI2_3           4
#define AM_HAL_PIN_72_COEX_PRIORITY     5
#define AM_HAL_PIN_72_CT72              6
#define AM_HAL_PIN_72_NCE72             7
#define AM_HAL_PIN_72_OBSBUS8           8
#define AM_HAL_PIN_72_UART0TX           9
#define AM_HAL_PIN_72_UART1CTS          10
#define AM_HAL_PIN_72_FPIO              11
#define AM_HAL_PIN_72_RESERVED12        12
#define AM_HAL_PIN_72_RESERVED13        13
#define AM_HAL_PIN_72_RESERVED14        14
#define AM_HAL_PIN_72_RESERVED15        15

// PIN 73 functions
#define AM_HAL_PIN_73_MSPI0_14          0
#define AM_HAL_PIN_73_SWTRACE3          1
#define AM_HAL_PIN_73_RESERVED2         2
#define AM_HAL_PIN_73_GPIO              3
#define AM_HAL_PIN_73_MSPI2_4           4
#define AM_HAL_PIN_73_COEX_FREQ         5
#define AM_HAL_PIN_73_CT73              6
#define AM_HAL_PIN_73_NCE73             7
#define AM_HAL_PIN_73_OBSBUS9           8
#define AM_HAL_PIN_73_UART0RX           9
#define AM_HAL_PIN_73_UART1RTS          10
#define AM_HAL_PIN_73_FPIO              11
#define AM_HAL_PIN_73_RESERVED12        12
#define AM_HAL_PIN_73_RESERVED13        13
#define AM_HAL_PIN_73_RESERVED14        14
#define AM_HAL_PIN_73_RESERVED15        15

// PIN 74 functions
#define AM_HAL_PIN_74_MSPI0_15          0
#define AM_HAL_PIN_74_RESERVED1         1
#define AM_HAL_PIN_74_RESERVED2         2
#define AM_HAL_PIN_74_GPIO              3
#define AM_HAL_PIN_74_MSPI2_5           4
#define AM_HAL_PIN_74_RESERVED5         5
#define AM_HAL_PIN_74_CT74              6
#define AM_HAL_PIN_74_NCE74             7
#define AM_HAL_PIN_74_OBSBUS10          8
#define AM_HAL_PIN_74_UART1TX           9
#define AM_HAL_PIN_74_UART0CTS          10
#define AM_HAL_PIN_74_FPIO              11
#define AM_HAL_PIN_74_RESERVED12        12
#define AM_HAL_PIN_74_RESERVED13        13
#define AM_HAL_PIN_74_RESERVED14        14
#define AM_HAL_PIN_74_RESERVED15        15

// PIN 75 functions
#define AM_HAL_PIN_75_MSPI0_16          0
#define AM_HAL_PIN_75_RESERVED1         1
#define AM_HAL_PIN_75_CLKOUT            2
#define AM_HAL_PIN_75_GPIO              3
#define AM_HAL_PIN_75_MSPI2_6           4
#define AM_HAL_PIN_75_RESERVED5         5
#define AM_HAL_PIN_75_CT75              6
#define AM_HAL_PIN_75_NCE75             7
#define AM_HAL_PIN_75_OBSBUS11          8
#define AM_HAL_PIN_75_UART1RX           9
#define AM_HAL_PIN_75_UART0RTS          10
#define AM_HAL_PIN_75_FPIO              11
#define AM_HAL_PIN_75_RESERVED12        12
#define AM_HAL_PIN_75_RESERVED13        13
#define AM_HAL_PIN_75_RESERVED14        14
#define AM_HAL_PIN_75_RESERVED15        15

// PIN 76 functions
#define AM_HAL_PIN_76_MSPI0_17          0
#define AM_HAL_PIN_76_RESERVED1         1
#define AM_HAL_PIN_76_CLKOUT            2
#define AM_HAL_PIN_76_GPIO              3
#define AM_HAL_PIN_76_MSPI2_7           4
#define AM_HAL_PIN_76_RESERVED5         5
#define AM_HAL_PIN_76_CT76              6
#define AM_HAL_PIN_76_NCE76             7
#define AM_HAL_PIN_76_OBSBUS12          8
#define AM_HAL_PIN_76_UART0TX           9
#define AM_HAL_PIN_76_UART1CTS          10
#define AM_HAL_PIN_76_FPIO              11
#define AM_HAL_PIN_76_RESERVED12        12
#define AM_HAL_PIN_76_RESERVED13        13
#define AM_HAL_PIN_76_RESERVED14        14
#define AM_HAL_PIN_76_RESERVED15        15

// PIN 77 functions
#define AM_HAL_PIN_77_MSPI0_18          0
#define AM_HAL_PIN_77_RESERVED1         1
#define AM_HAL_PIN_77_32KHzXT           2
#define AM_HAL_PIN_77_GPIO              3
#define AM_HAL_PIN_77_MSPI2_9           4
#define AM_HAL_PIN_77_RESERVED5         5
#define AM_HAL_PIN_77_CT77              6
#define AM_HAL_PIN_77_NCE77             7
#define AM_HAL_PIN_77_OBSBUS13          8
#define AM_HAL_PIN_77_UART0RX           9
#define AM_HAL_PIN_77_UART1RTS          10
#define AM_HAL_PIN_77_FPIO              11
#define AM_HAL_PIN_77_RESERVED12        12
#define AM_HAL_PIN_77_RESERVED13        13
#define AM_HAL_PIN_77_RESERVED14        14
#define AM_HAL_PIN_77_RESERVED15        15

// PIN 78 functions
#define AM_HAL_PIN_78_MNCE0_0           0
#define AM_HAL_PIN_78_RESERVED1         1
#define AM_HAL_PIN_78_RESERVED2         2
#define AM_HAL_PIN_78_GPIO              3
#define AM_HAL_PIN_78_RESERVED4         4
#define AM_HAL_PIN_78_RESERVED5         5
#define AM_HAL_PIN_78_CT78              6
#define AM_HAL_PIN_78_NCE78             7
#define AM_HAL_PIN_78_OBSBUS14          8
#define AM_HAL_PIN_78_UART1TX           9
#define AM_HAL_PIN_78_UART0CTS          10
#define AM_HAL_PIN_78_FPIO              11
#define AM_HAL_PIN_78_RESERVED12        12
#define AM_HAL_PIN_78_RESERVED13        13
#define AM_HAL_PIN_78_RESERVED14        14
#define AM_HAL_PIN_78_RESERVED15        15

// PIN 79 functions
#define AM_HAL_PIN_79_MNCE0_1           0
#define AM_HAL_PIN_79_DISP_TE           1
#define AM_HAL_PIN_79_ST_WCI_2W_TXD     2
#define AM_HAL_PIN_79_GPIO              3
#define AM_HAL_PIN_79_ST_WCI_1W_RXD     4
#define AM_HAL_PIN_79_RESERVED5         5
#define AM_HAL_PIN_79_CT79              6
#define AM_HAL_PIN_79_NCE79             7
#define AM_HAL_PIN_79_OBSBUS15          8
#define AM_HAL_PIN_79_UART1RX           9
#define AM_HAL_PIN_79_UART0RTS          10
#define AM_HAL_PIN_79_FPIO              11
#define AM_HAL_PIN_79_BTDM_TXEN         12
#define AM_HAL_PIN_79_RESERVED13        13
#define AM_HAL_PIN_79_RESERVED14        14
#define AM_HAL_PIN_79_RESERVED15        15

// PIN 80 functions
#define AM_HAL_PIN_80_MNCE2_1           0
#define AM_HAL_PIN_80_DISP_TE           1
#define AM_HAL_PIN_80_ST_WCI_2W_TXD     2
#define AM_HAL_PIN_80_GPIO              3
#define AM_HAL_PIN_80_ST_WCI_1W_RXD     4
#define AM_HAL_PIN_80_RESERVED5         5
#define AM_HAL_PIN_80_CT80              6
#define AM_HAL_PIN_80_NCE80             7
#define AM_HAL_PIN_80_OBSBUS0           8
#define AM_HAL_PIN_80_UART0TX           9
#define AM_HAL_PIN_80_UART1CTS          10
#define AM_HAL_PIN_80_FPIO              11
#define AM_HAL_PIN_80_RESERVED12        12
#define AM_HAL_PIN_80_RESERVED13        13
#define AM_HAL_PIN_80_RESERVED14        14
#define AM_HAL_PIN_80_RESERVED15        15

// PIN 81 functions
#define AM_HAL_PIN_81_MSPI0_9           0
#define AM_HAL_PIN_81_RESERVED1         1
#define AM_HAL_PIN_81_RESERVED2         2
#define AM_HAL_PIN_81_GPIO              3
#define AM_HAL_PIN_81_RESERVED4         4
#define AM_HAL_PIN_81_RESERVED5         5
#define AM_HAL_PIN_81_CT81              6
#define AM_HAL_PIN_81_NCE81             7
#define AM_HAL_PIN_81_OBSBUS1           8
#define AM_HAL_PIN_81_UART0RX           9
#define AM_HAL_PIN_81_UART1RTS          10
#define AM_HAL_PIN_81_FPIO              11
#define AM_HAL_PIN_81_RESERVED12        12
#define AM_HAL_PIN_81_RESERVED13        13
#define AM_HAL_PIN_81_RESERVED14        14
#define AM_HAL_PIN_81_RESERVED15        15

// PIN 82 functions
#define AM_HAL_PIN_82_SDIF0_CLKOUT      0
#define AM_HAL_PIN_82_RESERVED1         1
#define AM_HAL_PIN_82_RESERVED2         2
#define AM_HAL_PIN_82_GPIO              3
#define AM_HAL_PIN_82_TRIG0             4
#define AM_HAL_PIN_82_RESERVED5         5
#define AM_HAL_PIN_82_CT82              6
#define AM_HAL_PIN_82_NCE82             7
#define AM_HAL_PIN_82_OBSBUS2           8
#define AM_HAL_PIN_82_UART1TX           9
#define AM_HAL_PIN_82_UART0CTS          10
#define AM_HAL_PIN_82_FPIO              11
#define AM_HAL_PIN_82_RESERVED12        12
#define AM_HAL_PIN_82_RESERVED13        13
#define AM_HAL_PIN_82_RESERVED14        14
#define AM_HAL_PIN_82_RESERVED15        15

// PIN 83 functions
#define AM_HAL_PIN_83_SDIF0_DAT0        0
#define AM_HAL_PIN_83_RESERVED1         1
#define AM_HAL_PIN_83_RESERVED2         2
#define AM_HAL_PIN_83_GPIO              3
#define AM_HAL_PIN_83_TRIG1             4
#define AM_HAL_PIN_83_RESERVED5         5
#define AM_HAL_PIN_83_CT83              6
#define AM_HAL_PIN_83_NCE83             7
#define AM_HAL_PIN_83_OBSBUS3           8
#define AM_HAL_PIN_83_UART1RX           9
#define AM_HAL_PIN_83_UART0RTS          10
#define AM_HAL_PIN_83_FPIO              11
#define AM_HAL_PIN_83_RESERVED12        12
#define AM_HAL_PIN_83_RESERVED13        13
#define AM_HAL_PIN_83_RESERVED14        14
#define AM_HAL_PIN_83_RESERVED15        15

// PIN 84 functions
#define AM_HAL_PIN_84_SDIF0_DAT1        0
#define AM_HAL_PIN_84_RESERVED1         1
#define AM_HAL_PIN_84_RESERVED2         2
#define AM_HAL_PIN_84_GPIO              3
#define AM_HAL_PIN_84_TRIG2             4
#define AM_HAL_PIN_84_RESERVED5         5
#define AM_HAL_PIN_84_CT84              6
#define AM_HAL_PIN_84_NCE84             7
#define AM_HAL_PIN_84_OBSBUS4           8
#define AM_HAL_PIN_84_UART0TX           9
#define AM_HAL_PIN_84_UART1CTS          10
#define AM_HAL_PIN_84_FPIO              11
#define AM_HAL_PIN_84_RESERVED12        12
#define AM_HAL_PIN_84_RESERVED13        13
#define AM_HAL_PIN_84_RESERVED14        14
#define AM_HAL_PIN_84_RESERVED15        15

// PIN 85 functions
#define AM_HAL_PIN_85_SDIF0_DAT2        0
#define AM_HAL_PIN_85_RESERVED1         1
#define AM_HAL_PIN_85_RESERVED2         2
#define AM_HAL_PIN_85_GPIO              3
#define AM_HAL_PIN_85_TRIG3             4
#define AM_HAL_PIN_85_RESERVED5         5
#define AM_HAL_PIN_85_CT85              6
#define AM_HAL_PIN_85_NCE85             7
#define AM_HAL_PIN_85_OBSBUS5           8
#define AM_HAL_PIN_85_UART0RX           9
#define AM_HAL_PIN_85_UART1RTS          10
#define AM_HAL_PIN_85_FPIO              11
#define AM_HAL_PIN_85_RESERVED12        12
#define AM_HAL_PIN_85_RESERVED13        13
#define AM_HAL_PIN_85_RESERVED14        14
#define AM_HAL_PIN_85_RESERVED15        15

// PIN 86 functions
#define AM_HAL_PIN_86_SDIF0_DAT3        0
#define AM_HAL_PIN_86_RESERVED1         1
#define AM_HAL_PIN_86_RESERVED2         2
#define AM_HAL_PIN_86_GPIO              3
#define AM_HAL_PIN_86_TRIG0             4
#define AM_HAL_PIN_86_RESERVED5         5
#define AM_HAL_PIN_86_CT86              6
#define AM_HAL_PIN_86_NCE86             7
#define AM_HAL_PIN_86_OBSBUS6           8
#define AM_HAL_PIN_86_UART1TX           9
#define AM_HAL_PIN_86_UART0CTS          10
#define AM_HAL_PIN_86_FPIO              11
#define AM_HAL_PIN_86_RESERVED12        12
#define AM_HAL_PIN_86_RESERVED13        13
#define AM_HAL_PIN_86_RESERVED14        14
#define AM_HAL_PIN_86_RESERVED15        15

// PIN 87 functions
#define AM_HAL_PIN_87_SDIF0_CMD         0
#define AM_HAL_PIN_87_RESERVED1         1
#define AM_HAL_PIN_87_RESERVED2         2
#define AM_HAL_PIN_87_GPIO              3
#define AM_HAL_PIN_87_TRIG1             4
#define AM_HAL_PIN_87_RESERVED5         5
#define AM_HAL_PIN_87_CT87              6
#define AM_HAL_PIN_87_NCE87             7
#define AM_HAL_PIN_87_OBSBUS7           8
#define AM_HAL_PIN_87_UART1RX           9
#define AM_HAL_PIN_87_UART0RTS          10
#define AM_HAL_PIN_87_FPIO              11
#define AM_HAL_PIN_87_RESERVED12        12
#define AM_HAL_PIN_87_RESERVED13        13
#define AM_HAL_PIN_87_RESERVED14        14
#define AM_HAL_PIN_87_RESERVED15        15

// PIN 88 functions
#define AM_HAL_PIN_88_DISP_ENB          0
#define AM_HAL_PIN_88_RESERVED1         1
#define AM_HAL_PIN_88_32KHzXT           2
#define AM_HAL_PIN_88_GPIO              3
#define AM_HAL_PIN_88_TRIG2             4
#define AM_HAL_PIN_88_COEX_GRANT        5
#define AM_HAL_PIN_88_CT88              6
#define AM_HAL_PIN_88_NCE88             7
#define AM_HAL_PIN_88_OBSBUS8           8
#define AM_HAL_PIN_88_UART0TX           9
#define AM_HAL_PIN_88_UART1CTS          10
#define AM_HAL_PIN_88_FPIO              11
#define AM_HAL_PIN_88_CM4_SWDCK         12
#define AM_HAL_PIN_88_DBIB_CSX          13
#define AM_HAL_PIN_88_RESERVED14        14
#define AM_HAL_PIN_88_RESERVED15        15

// PIN 89 functions
#define AM_HAL_PIN_89_DISP_XRST         0
#define AM_HAL_PIN_89_RESERVED1         1
#define AM_HAL_PIN_89_RESERVED2         2
#define AM_HAL_PIN_89_GPIO              3
#define AM_HAL_PIN_89_TRIG3             4
#define AM_HAL_PIN_89_COEX_REQ          5
#define AM_HAL_PIN_89_CT89              6
#define AM_HAL_PIN_89_NCE89             7
#define AM_HAL_PIN_89_OBSBUS9           8
#define AM_HAL_PIN_89_UART0RX           9
#define AM_HAL_PIN_89_UART1RTS          10
#define AM_HAL_PIN_89_FPIO              11
#define AM_HAL_PIN_89_CM4_SWDIO         12
#define AM_HAL_PIN_89_DBIB_DCX          13
#define AM_HAL_PIN_89_RESERVED14        14
#define AM_HAL_PIN_89_RESERVED15        15

// PIN 90 functions
#define AM_HAL_PIN_90_DISP_R1           0
#define AM_HAL_PIN_90_RESERVED1         1
#define AM_HAL_PIN_90_CLKOUT            2
#define AM_HAL_PIN_90_GPIO              3
#define AM_HAL_PIN_90_TRIG0             4
#define AM_HAL_PIN_90_COEX_PRIORITY     5
#define AM_HAL_PIN_90_CT90              6
#define AM_HAL_PIN_90_NCE90             7
#define AM_HAL_PIN_90_OBSBUS10          8
#define AM_HAL_PIN_90_UART1TX           9
#define AM_HAL_PIN_90_UART0CTS          10
#define AM_HAL_PIN_90_FPIO              11
#define AM_HAL_PIN_90_CM4_SWO           12
#define AM_HAL_PIN_90_DBIB_WRX          13
#define AM_HAL_PIN_90_RESERVED14        14
#define AM_HAL_PIN_90_RESERVED15        15

// PIN 91 functions
#define AM_HAL_PIN_91_DISP_R2           0
#define AM_HAL_PIN_91_RESERVED1         1
#define AM_HAL_PIN_91_CLKOUT            2
#define AM_HAL_PIN_91_GPIO              3
#define AM_HAL_PIN_91_TRIG1             4
#define AM_HAL_PIN_91_COEX_FREQ         5
#define AM_HAL_PIN_91_CT91              6
#define AM_HAL_PIN_91_NCE91             7
#define AM_HAL_PIN_91_OBSBUS11          8
#define AM_HAL_PIN_91_UART1RX           9
#define AM_HAL_PIN_91_UART0RTS          10
#define AM_HAL_PIN_91_FPIO              11
#define AM_HAL_PIN_91_RESERVED12        12
#define AM_HAL_PIN_91_DBIB_RDX          13
#define AM_HAL_PIN_91_RESERVED14        14
#define AM_HAL_PIN_91_RESERVED15        15

// PIN 92 functions
#define AM_HAL_PIN_92_DISP_G1           0
#define AM_HAL_PIN_92_CM4_TIMING0       1
#define AM_HAL_PIN_92_DISP_QSPI_D0      2
#define AM_HAL_PIN_92_GPIO              3
#define AM_HAL_PIN_92_TRIG2             4
#define AM_HAL_PIN_92_DISP_SPI_SD       5
#define AM_HAL_PIN_92_CT92              6
#define AM_HAL_PIN_92_NCE92             7
#define AM_HAL_PIN_92_OBSBUS12          8
#define AM_HAL_PIN_92_UART0TX           9
#define AM_HAL_PIN_92_UART1CTS          10
#define AM_HAL_PIN_92_FPIO              11
#define AM_HAL_PIN_92_DISP_SPI_SDO      12
#define AM_HAL_PIN_92_DISP_QSPI_D0_OUT  13
#define AM_HAL_PIN_92_RESERVED14        14
#define AM_HAL_PIN_92_RESERVED15        15

// PIN 93 functions
#define AM_HAL_PIN_93_DISP_G2           0
#define AM_HAL_PIN_93_CM4_TIMING1       1
#define AM_HAL_PIN_93_DISP_QSPI_D1      2
#define AM_HAL_PIN_93_GPIO              3
#define AM_HAL_PIN_93_TRIG3             4
#define AM_HAL_PIN_93_DISP_SPI_DCX      5
#define AM_HAL_PIN_93_CT93              6
#define AM_HAL_PIN_93_NCE93             7
#define AM_HAL_PIN_93_OBSBUS13          8
#define AM_HAL_PIN_93_UART0RX           9
#define AM_HAL_PIN_93_UART1RTS          10
#define AM_HAL_PIN_93_FPIO              11
#define AM_HAL_PIN_93_RESERVED12        12
#define AM_HAL_PIN_93_DBIB_D0           13
#define AM_HAL_PIN_93_RESERVED14        14
#define AM_HAL_PIN_93_RESERVED15        15

// PIN 94 functions
#define AM_HAL_PIN_94_DISP_B1           0
#define AM_HAL_PIN_94_CM4_TIMING2       1
#define AM_HAL_PIN_94_DISP_QSPI_SCK     2
#define AM_HAL_PIN_94_GPIO              3
#define AM_HAL_PIN_94_TRIG0             4
#define AM_HAL_PIN_94_DISP_SPI_SCK      5
#define AM_HAL_PIN_94_CT94              6
#define AM_HAL_PIN_94_NCE94             7
#define AM_HAL_PIN_94_OBSBUS14          8
#define AM_HAL_PIN_94_UART1TX           9
#define AM_HAL_PIN_94_UART0CTS          10
#define AM_HAL_PIN_94_FPIO              11
#define AM_HAL_PIN_94_RESERVED12        12
#define AM_HAL_PIN_94_DBIB_D1           13
#define AM_HAL_PIN_94_RESERVED14        14
#define AM_HAL_PIN_94_RESERVED15        15

// PIN 95 functions
#define AM_HAL_PIN_95_DISP_B2           0
#define AM_HAL_PIN_95_CM4_TIMING3       1
#define AM_HAL_PIN_95_DISP_QSPI_D2      2
#define AM_HAL_PIN_95_GPIO              3
#define AM_HAL_PIN_95_TRIG1             4
#define AM_HAL_PIN_95_DISP_SPI_SDI      5
#define AM_HAL_PIN_95_CT95              6
#define AM_HAL_PIN_95_NCE95             7
#define AM_HAL_PIN_95_OBSBUS15          8
#define AM_HAL_PIN_95_UART1RX           9
#define AM_HAL_PIN_95_UART0RTS          10
#define AM_HAL_PIN_95_FPIO              11
#define AM_HAL_PIN_95_RF_XCVR_TST1      12
#define AM_HAL_PIN_95_DBIB_D2           13
#define AM_HAL_PIN_95_RESERVED14        14
#define AM_HAL_PIN_95_RESERVED15        15

// PIN 96 functions
#define AM_HAL_PIN_96_DISP_HST          0
#define AM_HAL_PIN_96_CM4_TIMING4       1
#define AM_HAL_PIN_96_DISP_QSPI_D3      2
#define AM_HAL_PIN_96_GPIO              3
#define AM_HAL_PIN_96_TRIG2             4
#define AM_HAL_PIN_96_DISP_SPI_RST      5
#define AM_HAL_PIN_96_CT96              6
#define AM_HAL_PIN_96_NCE96             7
#define AM_HAL_PIN_96_OBSBUS0           8
#define AM_HAL_PIN_96_UART0TX           9
#define AM_HAL_PIN_96_UART1CTS          10
#define AM_HAL_PIN_96_FPIO              11
#define AM_HAL_PIN_96_RF_XCVR_TST2      12
#define AM_HAL_PIN_96_DBIB_D3           13
#define AM_HAL_PIN_96_RESERVED14        14
#define AM_HAL_PIN_96_RESERVED15        15

// PIN 97 functions
#define AM_HAL_PIN_97_DISP_VST          0
#define AM_HAL_PIN_97_CM4_TIMING5       1
#define AM_HAL_PIN_97_RESERVED2         2
#define AM_HAL_PIN_97_GPIO              3
#define AM_HAL_PIN_97_TRIG3             4
#define AM_HAL_PIN_97_SWO               5
#define AM_HAL_PIN_97_CT97              6
#define AM_HAL_PIN_97_NCE97             7
#define AM_HAL_PIN_97_OBSBUS1           8
#define AM_HAL_PIN_97_UART0RX           9
#define AM_HAL_PIN_97_UART1RTS          10
#define AM_HAL_PIN_97_FPIO              11
#define AM_HAL_PIN_97_RF_XCVR_TST3      12
#define AM_HAL_PIN_97_DBIB_D4           13
#define AM_HAL_PIN_97_RESERVED14        14
#define AM_HAL_PIN_97_RESERVED15        15

// PIN 98 functions
#define AM_HAL_PIN_98_DISP_HCK          0
#define AM_HAL_PIN_98_CM4_TIMING6       1
#define AM_HAL_PIN_98_RESERVED2         2
#define AM_HAL_PIN_98_GPIO              3
#define AM_HAL_PIN_98_TRIG0             4
#define AM_HAL_PIN_98_SWO               5
#define AM_HAL_PIN_98_CT98              6
#define AM_HAL_PIN_98_NCE98             7
#define AM_HAL_PIN_98_OBSBUS2           8
#define AM_HAL_PIN_98_UART1TX           9
#define AM_HAL_PIN_98_UART0CTS          10
#define AM_HAL_PIN_98_FPIO              11
#define AM_HAL_PIN_98_RESERVED12        12
#define AM_HAL_PIN_98_DBIB_D5           13
#define AM_HAL_PIN_98_RESERVED14        14
#define AM_HAL_PIN_98_RESERVED15        15

// PIN 99 functions
#define AM_HAL_PIN_99_DISP_VCK          0
#define AM_HAL_PIN_99_CM4_TIMING7       1
#define AM_HAL_PIN_99_ST_WCI_2W_RXD     2
#define AM_HAL_PIN_99_GPIO              3
#define AM_HAL_PIN_99_TRIG1             4
#define AM_HAL_PIN_99_SWO               5
#define AM_HAL_PIN_99_CT99              6
#define AM_HAL_PIN_99_NCE99             7
#define AM_HAL_PIN_99_OBSBUS3           8
#define AM_HAL_PIN_99_UART1RX           9
#define AM_HAL_PIN_99_UART0RTS          10
#define AM_HAL_PIN_99_FPIO              11
#define AM_HAL_PIN_99_BTDM_TXEN         12
#define AM_HAL_PIN_99_DBIB_D6           13
#define AM_HAL_PIN_99_RESERVED14        14
#define AM_HAL_PIN_99_RESERVED15        15

// PIN 100 functions
#define AM_HAL_PIN_100_DISP_TE          0
#define AM_HAL_PIN_100_ST_WCI_1W_RXD    1
#define AM_HAL_PIN_100_ST_WCI_2W_TXD    2
#define AM_HAL_PIN_100_GPIO             3
#define AM_HAL_PIN_100_TRIG2            4
#define AM_HAL_PIN_100_RESERVED5        5
#define AM_HAL_PIN_100_CT100            6
#define AM_HAL_PIN_100_NCE100           7
#define AM_HAL_PIN_100_OBSBUS4          8
#define AM_HAL_PIN_100_UART0TX          9
#define AM_HAL_PIN_100_UART1CTS         10
#define AM_HAL_PIN_100_FPIO             11
#define AM_HAL_PIN_100_BTDM_RXEN        12
#define AM_HAL_PIN_100_DBIB_D7          13
#define AM_HAL_PIN_100_RESERVED14       14
#define AM_HAL_PIN_100_RESERVED15       15

// PIN 101 functions
#define AM_HAL_PIN_101_DISP_QSPI_CS_N   0
#define AM_HAL_PIN_101_RESERVED1        1
#define AM_HAL_PIN_101_BTDM_TXEN        2
#define AM_HAL_PIN_101_GPIO             3
#define AM_HAL_PIN_101_TRIG3            4
#define AM_HAL_PIN_101_DISP_SPI_CS_N    5
#define AM_HAL_PIN_101_CT101            6
#define AM_HAL_PIN_101_NCE101           7
#define AM_HAL_PIN_101_OBSBUS5          8
#define AM_HAL_PIN_101_UART0RX          9
#define AM_HAL_PIN_101_UART1RTS         10
#define AM_HAL_PIN_101_FPIO             11
#define AM_HAL_PIN_101_ST_WCI_1W_RXD    12
#define AM_HAL_PIN_101_ST_WCI_2W_TXD    13
#define AM_HAL_PIN_101_RESERVED14       14
#define AM_HAL_PIN_101_RESERVED15       15

// PIN 102 functions
#define AM_HAL_PIN_102_SLFDSCK          0
#define AM_HAL_PIN_102_SLFDSCL          1
#define AM_HAL_PIN_102_PDMI2S0_CLK      2
#define AM_HAL_PIN_102_GPIO             3
#define AM_HAL_PIN_102_TRIG0            4
#define AM_HAL_PIN_102_RESERVED5        5
#define AM_HAL_PIN_102_CT102            6
#define AM_HAL_PIN_102_NCE102           7
#define AM_HAL_PIN_102_OBSBUS6          8
#define AM_HAL_PIN_102_UART1TX          9
#define AM_HAL_PIN_102_UART0CTS         10
#define AM_HAL_PIN_102_FPIO             11
#define AM_HAL_PIN_102_RESERVED12       12
#define AM_HAL_PIN_102_DBIB_D8          13
#define AM_HAL_PIN_102_RESERVED14       14
#define AM_HAL_PIN_102_RESERVED15       15

// PIN 103 functions
#define AM_HAL_PIN_103_SLFDMOSI         0
#define AM_HAL_PIN_103_SLFDSDAWIR3      1
#define AM_HAL_PIN_103_PDMI2S0_SDOUT    2
#define AM_HAL_PIN_103_GPIO             3
#define AM_HAL_PIN_103_TRIG1            4
#define AM_HAL_PIN_103_RESERVED5        5
#define AM_HAL_PIN_103_CT103            6
#define AM_HAL_PIN_103_NCE103           7
#define AM_HAL_PIN_103_OBSBUS7          8
#define AM_HAL_PIN_103_UART1RX          9
#define AM_HAL_PIN_103_UART0RTS         10
#define AM_HAL_PIN_103_FPIO             11
#define AM_HAL_PIN_103_RESERVED12       12
#define AM_HAL_PIN_103_DBIB_D9          13
#define AM_HAL_PIN_103_RESERVED14       14
#define AM_HAL_PIN_103_RESERVED15       15

// PIN 104 functions
#define AM_HAL_PIN_104_SLFDMISO         0
#define AM_HAL_PIN_104_BTDM_TXEN        1
#define AM_HAL_PIN_104_PDMI2S0_WS       2
#define AM_HAL_PIN_104_GPIO             3
#define AM_HAL_PIN_104_TRIG2            4
#define AM_HAL_PIN_104_RESERVED5        5
#define AM_HAL_PIN_104_CT104            6
#define AM_HAL_PIN_104_NCE104           7
#define AM_HAL_PIN_104_OBSBUS8          8
#define AM_HAL_PIN_104_UART0TX          9
#define AM_HAL_PIN_104_UART1CTS         10
#define AM_HAL_PIN_104_FPIO             11
#define AM_HAL_PIN_104_RESERVED12       12
#define AM_HAL_PIN_104_DBIB_D10         13
#define AM_HAL_PIN_104_RESERVED14       14
#define AM_HAL_PIN_104_RESERVED15       15

// PIN 105 functions
#define AM_HAL_PIN_105_SLFDnCE          0
#define AM_HAL_PIN_105_BTDM_RXEN        1
#define AM_HAL_PIN_105_RESERVED2        2
#define AM_HAL_PIN_105_GPIO             3
#define AM_HAL_PIN_105_TRIG3            4
#define AM_HAL_PIN_105_RESERVED5        5
#define AM_HAL_PIN_105_CT105            6
#define AM_HAL_PIN_105_NCE105           7
#define AM_HAL_PIN_105_OBSBUS9          8
#define AM_HAL_PIN_105_UART0RX          9
#define AM_HAL_PIN_105_UART1RTS         10
#define AM_HAL_PIN_105_FPIO             11
#define AM_HAL_PIN_105_RESERVED12       12
#define AM_HAL_PIN_105_DBIB_D11         13
#define AM_HAL_PIN_105_RESERVED14       14
#define AM_HAL_PIN_105_RESERVED15       15

// PIN 106 functions
#define AM_HAL_PIN_106_SLFDINT          0
#define AM_HAL_PIN_106_I2S0_MCLK        1
#define AM_HAL_PIN_106_RESERVED2        2
#define AM_HAL_PIN_106_GPIO             3
#define AM_HAL_PIN_106_TRIG0            4
#define AM_HAL_PIN_106_ST_WCI_2W_RXD    5
#define AM_HAL_PIN_106_CT106            6
#define AM_HAL_PIN_106_NCE106           7
#define AM_HAL_PIN_106_OBSBUS10         8
#define AM_HAL_PIN_106_UART1TX          9
#define AM_HAL_PIN_106_UART0CTS         10
#define AM_HAL_PIN_106_FPIO             11
#define AM_HAL_PIN_106_RESERVED12       12
#define AM_HAL_PIN_106_DBIB_D12         13
#define AM_HAL_PIN_106_RESERVED14       14
#define AM_HAL_PIN_106_RESERVED15       15

// PIN 107 functions
#define AM_HAL_PIN_107_PDM0_CLK         0
#define AM_HAL_PIN_107_RESERVED1        1
#define AM_HAL_PIN_107_RESERVED2        2
#define AM_HAL_PIN_107_GPIO             3
#define AM_HAL_PIN_107_TRIG1            4
#define AM_HAL_PIN_107_RESERVED5        5
#define AM_HAL_PIN_107_CT107            6
#define AM_HAL_PIN_107_NCE107           7
#define AM_HAL_PIN_107_OBSBUS11         8
#define AM_HAL_PIN_107_UART1RX          9
#define AM_HAL_PIN_107_UART0RTS         10
#define AM_HAL_PIN_107_FPIO             11
#define AM_HAL_PIN_107_RESERVED12       12
#define AM_HAL_PIN_107_DBIB_D13         13
#define AM_HAL_PIN_107_RESERVED14       14
#define AM_HAL_PIN_107_RESERVED15       15

// PIN 108 functions
#define AM_HAL_PIN_108_PDM0_DATA        0
#define AM_HAL_PIN_108_RESERVED1        1
#define AM_HAL_PIN_108_RESERVED2        2
#define AM_HAL_PIN_108_GPIO             3
#define AM_HAL_PIN_108_TRIG2            4
#define AM_HAL_PIN_108_RESERVED5        5
#define AM_HAL_PIN_108_CT108            6
#define AM_HAL_PIN_108_NCE108           7
#define AM_HAL_PIN_108_OBSBUS12         8
#define AM_HAL_PIN_108_UART0TX          9
#define AM_HAL_PIN_108_UART1CTS         10
#define AM_HAL_PIN_108_FPIO             11
#define AM_HAL_PIN_108_RESERVED12       12
#define AM_HAL_PIN_108_DBIB_D14         13
#define AM_HAL_PIN_108_RESERVED14       14
#define AM_HAL_PIN_108_RESERVED15       15

// PIN 109 functions
#define AM_HAL_PIN_109_I2S0_CLK         0
#define AM_HAL_PIN_109_PDMI2S0_CLK      1
#define AM_HAL_PIN_109_RESERVED2        2
#define AM_HAL_PIN_109_GPIO             3
#define AM_HAL_PIN_109_TRIG3            4
#define AM_HAL_PIN_109_RESERVED5        5
#define AM_HAL_PIN_109_CT109            6
#define AM_HAL_PIN_109_NCE109           7
#define AM_HAL_PIN_109_OBSBUS13         8
#define AM_HAL_PIN_109_UART0RX          9
#define AM_HAL_PIN_109_UART1RTS         10
#define AM_HAL_PIN_109_FPIO             11
#define AM_HAL_PIN_109_RESERVED12       12
#define AM_HAL_PIN_109_DBIB_D15         13
#define AM_HAL_PIN_109_RESERVED14       14
#define AM_HAL_PIN_109_RESERVED15       15

// PIN 110 functions
#define AM_HAL_PIN_110_MNCE2_0          0
#define AM_HAL_PIN_110_RESERVED1        1
#define AM_HAL_PIN_110_RESERVED2        2
#define AM_HAL_PIN_110_GPIO             3
#define AM_HAL_PIN_110_TRIG0            4
#define AM_HAL_PIN_110_RESERVED5        5
#define AM_HAL_PIN_110_CT110            6
#define AM_HAL_PIN_110_NCE110           7
#define AM_HAL_PIN_110_OBSBUS14         8
#define AM_HAL_PIN_110_UART1TX          9
#define AM_HAL_PIN_110_UART0CTS         10
#define AM_HAL_PIN_110_FPIO             11
#define AM_HAL_PIN_110_RESERVED12       12
#define AM_HAL_PIN_110_RESERVED13       13
#define AM_HAL_PIN_110_RESERVED14       14
#define AM_HAL_PIN_110_RESERVED15       15

// PIN 111 functions
#define AM_HAL_PIN_111_MNCE2_1          0
#define AM_HAL_PIN_111_RESERVED1        1
#define AM_HAL_PIN_111_RESERVED2        2
#define AM_HAL_PIN_111_GPIO             3
#define AM_HAL_PIN_111_TRIG1            4
#define AM_HAL_PIN_111_ST_WCI_2W_RXD    5
#define AM_HAL_PIN_111_CT111            6
#define AM_HAL_PIN_111_NCE111           7
#define AM_HAL_PIN_111_OBSBUS15         8
#define AM_HAL_PIN_111_UART1RX          9
#define AM_HAL_PIN_111_UART0RTS         10
#define AM_HAL_PIN_111_FPIO             11
#define AM_HAL_PIN_111_BTDM_RXEN        12
#define AM_HAL_PIN_111_RESERVED13       13
#define AM_HAL_PIN_111_RESERVED14       14
#define AM_HAL_PIN_111_RESERVED15       15

// PIN 112 functions
#define AM_HAL_PIN_112_MSPI2_8          0
#define AM_HAL_PIN_112_RESERVED1        1
#define AM_HAL_PIN_112_RESERVED2        2
#define AM_HAL_PIN_112_GPIO             3
#define AM_HAL_PIN_112_TRIG2            4
#define AM_HAL_PIN_112_RESERVED5        5
#define AM_HAL_PIN_112_CT112            6
#define AM_HAL_PIN_112_NCE112           7
#define AM_HAL_PIN_112_OBSBUS0          8
#define AM_HAL_PIN_112_UART0TX          9
#define AM_HAL_PIN_112_UART1CTS         10
#define AM_HAL_PIN_112_FPIO             11
#define AM_HAL_PIN_112_RESERVED12       12
#define AM_HAL_PIN_112_RESERVED13       13
#define AM_HAL_PIN_112_RESERVED14       14
#define AM_HAL_PIN_112_RESERVED15       15

// PIN 113 functions
#define AM_HAL_PIN_113_DISP_QSPI_CS_N   0
#define AM_HAL_PIN_113_RESERVED1        1
#define AM_HAL_PIN_113_ST_WCI_2W_RXD    2
#define AM_HAL_PIN_113_GPIO             3
#define AM_HAL_PIN_113_TRIG3            4
#define AM_HAL_PIN_113_DISP_SPI_CS_N    5
#define AM_HAL_PIN_113_CT113            6
#define AM_HAL_PIN_113_NCE113           7
#define AM_HAL_PIN_113_OBSBUS1          8
#define AM_HAL_PIN_113_UART0RX          9
#define AM_HAL_PIN_113_UART1RTS         10
#define AM_HAL_PIN_113_FPIO             11
#define AM_HAL_PIN_113_BTDM_TXEN        12
#define AM_HAL_PIN_113_RESERVED13       13
#define AM_HAL_PIN_113_RESERVED14       14
#define AM_HAL_PIN_113_RESERVED15       15

// PIN 114 functions
#define AM_HAL_PIN_114_SDIF0_DAT4       0
#define AM_HAL_PIN_114_CM4_TIMING0      1
#define AM_HAL_PIN_114_BTDM_TXEN        2
#define AM_HAL_PIN_114_GPIO             3
#define AM_HAL_PIN_114_TRIG0            4
#define AM_HAL_PIN_114_RESERVED5        5
#define AM_HAL_PIN_114_CT114            6
#define AM_HAL_PIN_114_NCE114           7
#define AM_HAL_PIN_114_OBSBUS2          8
#define AM_HAL_PIN_114_UART1TX          9
#define AM_HAL_PIN_114_UART0CTS         10
#define AM_HAL_PIN_114_FPIO             11
#define AM_HAL_PIN_114_RESERVED12       12
#define AM_HAL_PIN_114_RESERVED13       13
#define AM_HAL_PIN_114_RESERVED14       14
#define AM_HAL_PIN_114_RESERVED15       15

// PIN 115 functions
#define AM_HAL_PIN_115_SDIF0_DAT5       0
#define AM_HAL_PIN_115_CM4_TIMING1      1
#define AM_HAL_PIN_115_BTDM_RXEN        2
#define AM_HAL_PIN_115_GPIO             3
#define AM_HAL_PIN_115_TRIG1            4
#define AM_HAL_PIN_115_RESERVED5        5
#define AM_HAL_PIN_115_CT115            6
#define AM_HAL_PIN_115_NCE115           7
#define AM_HAL_PIN_115_OBSBUS3          8
#define AM_HAL_PIN_115_UART1RX          9
#define AM_HAL_PIN_115_UART0RTS         10
#define AM_HAL_PIN_115_FPIO             11
#define AM_HAL_PIN_115_RESERVED12       12
#define AM_HAL_PIN_115_RESERVED13       13
#define AM_HAL_PIN_115_RESERVED14       14
#define AM_HAL_PIN_115_RESERVED15       15

// PIN 116 functions
#define AM_HAL_PIN_116_SDIF0_DAT6       0
#define AM_HAL_PIN_116_CM4_TIMING2      1
#define AM_HAL_PIN_116_RESERVED2        2
#define AM_HAL_PIN_116_GPIO             3
#define AM_HAL_PIN_116_TRIG2            4
#define AM_HAL_PIN_116_SWO              5
#define AM_HAL_PIN_116_CT116            6
#define AM_HAL_PIN_116_NCE116           7
#define AM_HAL_PIN_116_OBSBUS4          8
#define AM_HAL_PIN_116_UART0TX          9
#define AM_HAL_PIN_116_UART1CTS         10
#define AM_HAL_PIN_116_FPIO             11
#define AM_HAL_PIN_116_RESERVED12       12
#define AM_HAL_PIN_116_RESERVED13       13
#define AM_HAL_PIN_116_RESERVED14       14
#define AM_HAL_PIN_116_RESERVED15       15

// PIN 117 functions
#define AM_HAL_PIN_117_SDIF0_DAT7       0
#define AM_HAL_PIN_117_CM4_TIMING3      1
#define AM_HAL_PIN_117_RESERVED2        2
#define AM_HAL_PIN_117_GPIO             3
#define AM_HAL_PIN_117_TRIG3            4
#define AM_HAL_PIN_117_SWO              5
#define AM_HAL_PIN_117_CT117            6
#define AM_HAL_PIN_117_NCE117           7
#define AM_HAL_PIN_117_OBSBUS5          8
#define AM_HAL_PIN_117_UART0RX          9
#define AM_HAL_PIN_117_UART1RTS         10
#define AM_HAL_PIN_117_FPIO             11
#define AM_HAL_PIN_117_RESERVED12       12
#define AM_HAL_PIN_117_RESERVED13       13
#define AM_HAL_PIN_117_RESERVED14       14
#define AM_HAL_PIN_117_RESERVED15       15

// PIN 118 functions
#define AM_HAL_PIN_118_I2S0_MCLK        0
#define AM_HAL_PIN_118_CM4_TIMING4      1
#define AM_HAL_PIN_118_PDMI2S0_CLK      2
#define AM_HAL_PIN_118_GPIO             3
#define AM_HAL_PIN_118_TRIG0            4
#define AM_HAL_PIN_118_SWO              5
#define AM_HAL_PIN_118_CT118            6
#define AM_HAL_PIN_118_NCE118           7
#define AM_HAL_PIN_118_OBSBUS6          8
#define AM_HAL_PIN_118_UART1TX          9
#define AM_HAL_PIN_118_UART0CTS         10
#define AM_HAL_PIN_118_FPIO             11
#define AM_HAL_PIN_118_I2S0_CLK         12
#define AM_HAL_PIN_118_RESERVED13       13
#define AM_HAL_PIN_118_RESERVED14       14
#define AM_HAL_PIN_118_RESERVED15       15

// PIN 119 functions
#define AM_HAL_PIN_119_RESERVED0        0
#define AM_HAL_PIN_119_RESERVED1        1
#define AM_HAL_PIN_119_BTDM_RXEN        2
#define AM_HAL_PIN_119_GPIO             3
#define AM_HAL_PIN_119_TRIG1            4
#define AM_HAL_PIN_119_SWO              5
#define AM_HAL_PIN_119_CT119            6
#define AM_HAL_PIN_119_NCE119           7
#define AM_HAL_PIN_119_OBSBUS7          8
#define AM_HAL_PIN_119_UART1RX          9
#define AM_HAL_PIN_119_UART0RTS         10
#define AM_HAL_PIN_119_FPIO             11
#define AM_HAL_PIN_119_ST_WCI_2W_RXD    12
#define AM_HAL_PIN_119_RESERVED13       13
#define AM_HAL_PIN_119_RESERVED14       14
#define AM_HAL_PIN_119_RESERVED15       15

//*****************************************************************************
//
// List of all function selects.
//
//*****************************************************************************
typedef enum
{
    AM_HAL_PIN_FN_ADCSE0,
    AM_HAL_PIN_FN_ADCSE1,
    AM_HAL_PIN_FN_ADCSE2,
    AM_HAL_PIN_FN_ADCSE3,
    AM_HAL_PIN_FN_ADCSE4,
    AM_HAL_PIN_FN_ADCSE5,
    AM_HAL_PIN_FN_ADCSE6,
    AM_HAL_PIN_FN_ADCSE7,
    AM_HAL_PIN_FN_ANATEST1,
    AM_HAL_PIN_FN_ANATEST2,
    AM_HAL_PIN_FN_AO_OBSBUS0,
    AM_HAL_PIN_FN_AO_OBSBUS1,
    AM_HAL_PIN_FN_AO_OBSBUS2,
    AM_HAL_PIN_FN_AO_OBSBUS3,
    AM_HAL_PIN_FN_BTDM_RXEN,
    AM_HAL_PIN_FN_BTDM_TXEN,
    AM_HAL_PIN_FN_CLK96MHz,
    AM_HAL_PIN_FN_CLK192MHz,
    AM_HAL_PIN_FN_CLKOUT,
    AM_HAL_PIN_FN_CME,
    AM_HAL_PIN_FN_CMLE,
    AM_HAL_PIN_FN_CMPIN0,
    AM_HAL_PIN_FN_CMPIN1,
    AM_HAL_PIN_FN_CMPRF0,
    AM_HAL_PIN_FN_CMPRF1,
    AM_HAL_PIN_FN_CMPRF2,
    AM_HAL_PIN_FN_CM4_SWDCK,
    AM_HAL_PIN_FN_CM4_SWDIO,
    AM_HAL_PIN_FN_CM4_SWO,
    AM_HAL_PIN_FN_CM4_TIMING0,
    AM_HAL_PIN_FN_CM4_TIMING1,
    AM_HAL_PIN_FN_CM4_TIMING2,
    AM_HAL_PIN_FN_CM4_TIMING3,
    AM_HAL_PIN_FN_CM4_TIMING4,
    AM_HAL_PIN_FN_CM4_TIMING5,
    AM_HAL_PIN_FN_CM4_TIMING6,
    AM_HAL_PIN_FN_CM4_TIMING7,
    AM_HAL_PIN_FN_COEX_FREQ,
    AM_HAL_PIN_FN_COEX_GRANT,
    AM_HAL_PIN_FN_COEX_PRIORITY,
    AM_HAL_PIN_FN_COEX_REQ,
    AM_HAL_PIN_FN_CT0,
    AM_HAL_PIN_FN_CT1,
    AM_HAL_PIN_FN_CT2,
    AM_HAL_PIN_FN_CT3,
    AM_HAL_PIN_FN_CT4,
    AM_HAL_PIN_FN_CT5,
    AM_HAL_PIN_FN_CT6,
    AM_HAL_PIN_FN_CT7,
    AM_HAL_PIN_FN_CT8,
    AM_HAL_PIN_FN_CT9,
    AM_HAL_PIN_FN_CT10,
    AM_HAL_PIN_FN_CT11,
    AM_HAL_PIN_FN_CT12,
    AM_HAL_PIN_FN_CT13,
    AM_HAL_PIN_FN_CT14,
    AM_HAL_PIN_FN_CT15,
    AM_HAL_PIN_FN_CT16,
    AM_HAL_PIN_FN_CT17,
    AM_HAL_PIN_FN_CT18,
    AM_HAL_PIN_FN_CT19,
    AM_HAL_PIN_FN_CT20,
    AM_HAL_PIN_FN_CT21,
    AM_HAL_PIN_FN_CT22,
    AM_HAL_PIN_FN_CT23,
    AM_HAL_PIN_FN_CT24,
    AM_HAL_PIN_FN_CT25,
    AM_HAL_PIN_FN_CT26,
    AM_HAL_PIN_FN_CT27,
    AM_HAL_PIN_FN_CT28,
    AM_HAL_PIN_FN_CT29,
    AM_HAL_PIN_FN_CT30,
    AM_HAL_PIN_FN_CT31,
    AM_HAL_PIN_FN_CT32,
    AM_HAL_PIN_FN_CT33,
    AM_HAL_PIN_FN_CT34,
    AM_HAL_PIN_FN_CT35,
    AM_HAL_PIN_FN_CT36,
    AM_HAL_PIN_FN_CT37,
    AM_HAL_PIN_FN_CT38,
    AM_HAL_PIN_FN_CT39,
    AM_HAL_PIN_FN_CT40,
    AM_HAL_PIN_FN_CT41,
    AM_HAL_PIN_FN_CT42,
    AM_HAL_PIN_FN_CT43,
    AM_HAL_PIN_FN_CT44,
    AM_HAL_PIN_FN_CT45,
    AM_HAL_PIN_FN_CT46,
    AM_HAL_PIN_FN_CT47,
    AM_HAL_PIN_FN_CT48,
    AM_HAL_PIN_FN_CT49,
    AM_HAL_PIN_FN_CT50,
    AM_HAL_PIN_FN_CT51,
    AM_HAL_PIN_FN_CT52,
    AM_HAL_PIN_FN_CT53,
    AM_HAL_PIN_FN_CT54,
    AM_HAL_PIN_FN_CT55,
    AM_HAL_PIN_FN_CT56,
    AM_HAL_PIN_FN_CT57,
    AM_HAL_PIN_FN_CT58,
    AM_HAL_PIN_FN_CT59,
    AM_HAL_PIN_FN_CT60,
    AM_HAL_PIN_FN_CT61,
    AM_HAL_PIN_FN_CT62,
    AM_HAL_PIN_FN_CT63,
    AM_HAL_PIN_FN_CT64,
    AM_HAL_PIN_FN_CT65,
    AM_HAL_PIN_FN_CT66,
    AM_HAL_PIN_FN_CT67,
    AM_HAL_PIN_FN_CT68,
    AM_HAL_PIN_FN_CT69,
    AM_HAL_PIN_FN_CT70,
    AM_HAL_PIN_FN_CT71,
    AM_HAL_PIN_FN_CT72,
    AM_HAL_PIN_FN_CT73,
    AM_HAL_PIN_FN_CT74,
    AM_HAL_PIN_FN_CT75,
    AM_HAL_PIN_FN_CT76,
    AM_HAL_PIN_FN_CT77,
    AM_HAL_PIN_FN_CT78,
    AM_HAL_PIN_FN_CT79,
    AM_HAL_PIN_FN_CT80,
    AM_HAL_PIN_FN_CT81,
    AM_HAL_PIN_FN_CT82,
    AM_HAL_PIN_FN_CT83,
    AM_HAL_PIN_FN_CT84,
    AM_HAL_PIN_FN_CT85,
    AM_HAL_PIN_FN_CT86,
    AM_HAL_PIN_FN_CT87,
    AM_HAL_PIN_FN_CT88,
    AM_HAL_PIN_FN_CT89,
    AM_HAL_PIN_FN_CT90,
    AM_HAL_PIN_FN_CT91,
    AM_HAL_PIN_FN_CT92,
    AM_HAL_PIN_FN_CT93,
    AM_HAL_PIN_FN_CT94,
    AM_HAL_PIN_FN_CT95,
    AM_HAL_PIN_FN_CT96,
    AM_HAL_PIN_FN_CT97,
    AM_HAL_PIN_FN_CT98,
    AM_HAL_PIN_FN_CT99,
    AM_HAL_PIN_FN_CT100,
    AM_HAL_PIN_FN_CT101,
    AM_HAL_PIN_FN_CT102,
    AM_HAL_PIN_FN_CT103,
    AM_HAL_PIN_FN_CT104,
    AM_HAL_PIN_FN_CT105,
    AM_HAL_PIN_FN_CT106,
    AM_HAL_PIN_FN_CT107,
    AM_HAL_PIN_FN_CT108,
    AM_HAL_PIN_FN_CT109,
    AM_HAL_PIN_FN_CT110,
    AM_HAL_PIN_FN_CT111,
    AM_HAL_PIN_FN_CT112,
    AM_HAL_PIN_FN_CT113,
    AM_HAL_PIN_FN_CT114,
    AM_HAL_PIN_FN_CT115,
    AM_HAL_PIN_FN_CT116,
    AM_HAL_PIN_FN_CT117,
    AM_HAL_PIN_FN_CT118,
    AM_HAL_PIN_FN_CT119,
    AM_HAL_PIN_FN_DBIB_CSX,
    AM_HAL_PIN_FN_DBIB_D0,
    AM_HAL_PIN_FN_DBIB_D1,
    AM_HAL_PIN_FN_DBIB_D2,
    AM_HAL_PIN_FN_DBIB_D3,
    AM_HAL_PIN_FN_DBIB_D4,
    AM_HAL_PIN_FN_DBIB_D5,
    AM_HAL_PIN_FN_DBIB_D6,
    AM_HAL_PIN_FN_DBIB_D7,
    AM_HAL_PIN_FN_DBIB_D8,
    AM_HAL_PIN_FN_DBIB_D9,
    AM_HAL_PIN_FN_DBIB_D10,
    AM_HAL_PIN_FN_DBIB_D11,
    AM_HAL_PIN_FN_DBIB_D12,
    AM_HAL_PIN_FN_DBIB_D13,
    AM_HAL_PIN_FN_DBIB_D14,
    AM_HAL_PIN_FN_DBIB_D15,
    AM_HAL_PIN_FN_DBIB_DCX,
    AM_HAL_PIN_FN_DBIB_RDX,
    AM_HAL_PIN_FN_DBIB_WRX,
    AM_HAL_PIN_FN_DFT_ISO,
    AM_HAL_PIN_FN_DFT_RET,
    AM_HAL_PIN_FN_DISP_B1,
    AM_HAL_PIN_FN_DISP_B2,
    AM_HAL_PIN_FN_DISP_ENB,
    AM_HAL_PIN_FN_DISP_G1,
    AM_HAL_PIN_FN_DISP_G2,
    AM_HAL_PIN_FN_DISP_HCK,
    AM_HAL_PIN_FN_DISP_HST,
    AM_HAL_PIN_FN_DISP_QSPI_CS_N,
    AM_HAL_PIN_FN_DISP_QSPI_D0,
    AM_HAL_PIN_FN_DISP_QSPI_D1,
    AM_HAL_PIN_FN_DISP_QSPI_D2,
    AM_HAL_PIN_FN_DISP_QSPI_D3,
    AM_HAL_PIN_FN_DISP_QSPI_D0_OUT,
    AM_HAL_PIN_FN_DISP_QSPI_SCK,
    AM_HAL_PIN_FN_DISP_R1,
    AM_HAL_PIN_FN_DISP_R2,
    AM_HAL_PIN_FN_DISP_SPI_CS_N,
    AM_HAL_PIN_FN_DISP_SPI_DCX,
    AM_HAL_PIN_FN_DISP_SPI_RST,
    AM_HAL_PIN_FN_DISP_SPI_SCK,
    AM_HAL_PIN_FN_DISP_SPI_SD,
    AM_HAL_PIN_FN_DISP_SPI_SDI,
    AM_HAL_PIN_FN_DISP_SPI_SDO,
    AM_HAL_PIN_FN_DISP_TE,
    AM_HAL_PIN_FN_DISP_VCK,
    AM_HAL_PIN_FN_DISP_VST,
    AM_HAL_PIN_FN_DISP_XRST,
    AM_HAL_PIN_FN_EN96MHz,
    AM_HAL_PIN_FN_EN192MHz,
    AM_HAL_PIN_FN_FLB_FCLK,
    AM_HAL_PIN_FN_FLB_TCLK,
    AM_HAL_PIN_FN_FLB_TDI,
    AM_HAL_PIN_FN_FLB_TDO,
    AM_HAL_PIN_FN_FLB_TMS,
    AM_HAL_PIN_FN_FLB_TRSTN,
    AM_HAL_PIN_FN_FLLOAD_ADDR0,
    AM_HAL_PIN_FN_FLLOAD_ADDR1,
    AM_HAL_PIN_FN_FLLOAD_ADDR2,
    AM_HAL_PIN_FN_FLLOAD_DATA0,
    AM_HAL_PIN_FN_FLLOAD_DATA1,
    AM_HAL_PIN_FN_FLLOAD_DATA2,
    AM_HAL_PIN_FN_FLLOAD_DATA3,
    AM_HAL_PIN_FN_FLLOAD_DIR,
    AM_HAL_PIN_FN_FLLOAD_STRB,
    AM_HAL_PIN_FN_FPIO,
    AM_HAL_PIN_FN_GPIO,
    AM_HAL_PIN_FN_HFRC_EXT,
    AM_HAL_PIN_FN_I2S0_CLK,
    AM_HAL_PIN_FN_I2S0_DATA,
    AM_HAL_PIN_FN_I2S0_MCLK,
    AM_HAL_PIN_FN_I2S0_SDIN,
    AM_HAL_PIN_FN_I2S0_SDOUT,
    AM_HAL_PIN_FN_I2S0_WS,
    AM_HAL_PIN_FN_32KHzXT,
    AM_HAL_PIN_FN_LFRC_EXT,
    AM_HAL_PIN_FN_LPG_ENABLE,
    AM_HAL_PIN_FN_LPG_LOAD,
    AM_HAL_PIN_FN_MDA_HFRC_EXT,
    AM_HAL_PIN_FN_MDA_SRST,
    AM_HAL_PIN_FN_MDA_TCK,
    AM_HAL_PIN_FN_MDA_TDI,
    AM_HAL_PIN_FN_MDA_TDO,
    AM_HAL_PIN_FN_MDA_TMS,
    AM_HAL_PIN_FN_MDA_TRSTN,
    AM_HAL_PIN_FN_M0MISO,
    AM_HAL_PIN_FN_M1MISO,
    AM_HAL_PIN_FN_M2MISO,
    AM_HAL_PIN_FN_M3MISO,
    AM_HAL_PIN_FN_M4MISO,
    AM_HAL_PIN_FN_M5MISO,
    AM_HAL_PIN_FN_M0MOSI,
    AM_HAL_PIN_FN_M1MOSI,
    AM_HAL_PIN_FN_M2MOSI,
    AM_HAL_PIN_FN_M3MOSI,
    AM_HAL_PIN_FN_M4MOSI,
    AM_HAL_PIN_FN_M5MOSI,
    AM_HAL_PIN_FN_MNCE0_0,
    AM_HAL_PIN_FN_MNCE0_1,
    AM_HAL_PIN_FN_MNCE1_0,
    AM_HAL_PIN_FN_MNCE1_1,
    AM_HAL_PIN_FN_MNCE2_0,
    AM_HAL_PIN_FN_MNCE2_1,
    AM_HAL_PIN_FN_M0SCK,
    AM_HAL_PIN_FN_M1SCK,
    AM_HAL_PIN_FN_M2SCK,
    AM_HAL_PIN_FN_M3SCK,
    AM_HAL_PIN_FN_M4SCK,
    AM_HAL_PIN_FN_M5SCK,
    AM_HAL_PIN_FN_M0SCL,
    AM_HAL_PIN_FN_M1SCL,
    AM_HAL_PIN_FN_M2SCL,
    AM_HAL_PIN_FN_M3SCL,
    AM_HAL_PIN_FN_M4SCL,
    AM_HAL_PIN_FN_M5SCL,
    AM_HAL_PIN_FN_M0SDAWIR3,
    AM_HAL_PIN_FN_M1SDAWIR3,
    AM_HAL_PIN_FN_M2SDAWIR3,
    AM_HAL_PIN_FN_M3SDAWIR3,
    AM_HAL_PIN_FN_M4SDAWIR3,
    AM_HAL_PIN_FN_M5SDAWIR3,
    AM_HAL_PIN_FN_MSPI0_0,
    AM_HAL_PIN_FN_MSPI0_1,
    AM_HAL_PIN_FN_MSPI0_2,
    AM_HAL_PIN_FN_MSPI0_3,
    AM_HAL_PIN_FN_MSPI0_4,
    AM_HAL_PIN_FN_MSPI0_5,
    AM_HAL_PIN_FN_MSPI0_6,
    AM_HAL_PIN_FN_MSPI0_7,
    AM_HAL_PIN_FN_MSPI0_8,
    AM_HAL_PIN_FN_MSPI0_9,
    AM_HAL_PIN_FN_MSPI1_0,
    AM_HAL_PIN_FN_MSPI0_10,
    AM_HAL_PIN_FN_MSPI1_1,
    AM_HAL_PIN_FN_MSPI0_11,
    AM_HAL_PIN_FN_MSPI1_2,
    AM_HAL_PIN_FN_MSPI0_12,
    AM_HAL_PIN_FN_MSPI1_3,
    AM_HAL_PIN_FN_MSPI0_13,
    AM_HAL_PIN_FN_MSPI1_4,
    AM_HAL_PIN_FN_MSPI0_14,
    AM_HAL_PIN_FN_MSPI1_5,
    AM_HAL_PIN_FN_MSPI0_15,
    AM_HAL_PIN_FN_MSPI0_16,
    AM_HAL_PIN_FN_MSPI0_17,
    AM_HAL_PIN_FN_MSPI0_18,
    AM_HAL_PIN_FN_MSPI2_0,
    AM_HAL_PIN_FN_MSPI2_1,
    AM_HAL_PIN_FN_MSPI2_2,
    AM_HAL_PIN_FN_MSPI2_3,
    AM_HAL_PIN_FN_MSPI2_4,
    AM_HAL_PIN_FN_MSPI2_5,
    AM_HAL_PIN_FN_MSPI2_6,
    AM_HAL_PIN_FN_MSPI2_7,
    AM_HAL_PIN_FN_MSPI2_8,
    AM_HAL_PIN_FN_MSPI2_9,
    AM_HAL_PIN_FN_NCE0,
    AM_HAL_PIN_FN_NCE1,
    AM_HAL_PIN_FN_NCE2,
    AM_HAL_PIN_FN_NCE3,
    AM_HAL_PIN_FN_NCE4,
    AM_HAL_PIN_FN_NCE5,
    AM_HAL_PIN_FN_NCE6,
    AM_HAL_PIN_FN_NCE7,
    AM_HAL_PIN_FN_NCE8,
    AM_HAL_PIN_FN_NCE9,
    AM_HAL_PIN_FN_NCE10,
    AM_HAL_PIN_FN_NCE11,
    AM_HAL_PIN_FN_NCE12,
    AM_HAL_PIN_FN_NCE13,
    AM_HAL_PIN_FN_NCE14,
    AM_HAL_PIN_FN_NCE15,
    AM_HAL_PIN_FN_NCE16,
    AM_HAL_PIN_FN_NCE17,
    AM_HAL_PIN_FN_NCE18,
    AM_HAL_PIN_FN_NCE19,
    AM_HAL_PIN_FN_NCE20,
    AM_HAL_PIN_FN_NCE21,
    AM_HAL_PIN_FN_NCE22,
    AM_HAL_PIN_FN_NCE23,
    AM_HAL_PIN_FN_NCE24,
    AM_HAL_PIN_FN_NCE25,
    AM_HAL_PIN_FN_NCE26,
    AM_HAL_PIN_FN_NCE27,
    AM_HAL_PIN_FN_NCE28,
    AM_HAL_PIN_FN_NCE29,
    AM_HAL_PIN_FN_NCE30,
    AM_HAL_PIN_FN_NCE31,
    AM_HAL_PIN_FN_NCE32,
    AM_HAL_PIN_FN_NCE33,
    AM_HAL_PIN_FN_NCE34,
    AM_HAL_PIN_FN_NCE35,
    AM_HAL_PIN_FN_NCE36,
    AM_HAL_PIN_FN_NCE37,
    AM_HAL_PIN_FN_NCE38,
    AM_HAL_PIN_FN_NCE39,
    AM_HAL_PIN_FN_NCE40,
    AM_HAL_PIN_FN_NCE41,
    AM_HAL_PIN_FN_NCE42,
    AM_HAL_PIN_FN_NCE43,
    AM_HAL_PIN_FN_NCE44,
    AM_HAL_PIN_FN_NCE45,
    AM_HAL_PIN_FN_NCE46,
    AM_HAL_PIN_FN_NCE47,
    AM_HAL_PIN_FN_NCE48,
    AM_HAL_PIN_FN_NCE49,
    AM_HAL_PIN_FN_NCE50,
    AM_HAL_PIN_FN_NCE51,
    AM_HAL_PIN_FN_NCE52,
    AM_HAL_PIN_FN_NCE53,
    AM_HAL_PIN_FN_NCE54,
    AM_HAL_PIN_FN_NCE55,
    AM_HAL_PIN_FN_NCE56,
    AM_HAL_PIN_FN_NCE57,
    AM_HAL_PIN_FN_NCE58,
    AM_HAL_PIN_FN_NCE59,
    AM_HAL_PIN_FN_NCE60,
    AM_HAL_PIN_FN_NCE61,
    AM_HAL_PIN_FN_NCE62,
    AM_HAL_PIN_FN_NCE63,
    AM_HAL_PIN_FN_NCE64,
    AM_HAL_PIN_FN_NCE65,
    AM_HAL_PIN_FN_NCE66,
    AM_HAL_PIN_FN_NCE67,
    AM_HAL_PIN_FN_NCE68,
    AM_HAL_PIN_FN_NCE69,
    AM_HAL_PIN_FN_NCE70,
    AM_HAL_PIN_FN_NCE71,
    AM_HAL_PIN_FN_NCE72,
    AM_HAL_PIN_FN_NCE73,
    AM_HAL_PIN_FN_NCE74,
    AM_HAL_PIN_FN_NCE75,
    AM_HAL_PIN_FN_NCE76,
    AM_HAL_PIN_FN_NCE77,
    AM_HAL_PIN_FN_NCE78,
    AM_HAL_PIN_FN_NCE79,
    AM_HAL_PIN_FN_NCE80,
    AM_HAL_PIN_FN_NCE81,
    AM_HAL_PIN_FN_NCE82,
    AM_HAL_PIN_FN_NCE83,
    AM_HAL_PIN_FN_NCE84,
    AM_HAL_PIN_FN_NCE85,
    AM_HAL_PIN_FN_NCE86,
    AM_HAL_PIN_FN_NCE87,
    AM_HAL_PIN_FN_NCE88,
    AM_HAL_PIN_FN_NCE89,
    AM_HAL_PIN_FN_NCE90,
    AM_HAL_PIN_FN_NCE91,
    AM_HAL_PIN_FN_NCE92,
    AM_HAL_PIN_FN_NCE93,
    AM_HAL_PIN_FN_NCE94,
    AM_HAL_PIN_FN_NCE95,
    AM_HAL_PIN_FN_NCE96,
    AM_HAL_PIN_FN_NCE97,
    AM_HAL_PIN_FN_NCE98,
    AM_HAL_PIN_FN_NCE99,
    AM_HAL_PIN_FN_NCE100,
    AM_HAL_PIN_FN_NCE101,
    AM_HAL_PIN_FN_NCE102,
    AM_HAL_PIN_FN_NCE103,
    AM_HAL_PIN_FN_NCE104,
    AM_HAL_PIN_FN_NCE105,
    AM_HAL_PIN_FN_NCE106,
    AM_HAL_PIN_FN_NCE107,
    AM_HAL_PIN_FN_NCE108,
    AM_HAL_PIN_FN_NCE109,
    AM_HAL_PIN_FN_NCE110,
    AM_HAL_PIN_FN_NCE111,
    AM_HAL_PIN_FN_NCE112,
    AM_HAL_PIN_FN_NCE113,
    AM_HAL_PIN_FN_NCE114,
    AM_HAL_PIN_FN_NCE115,
    AM_HAL_PIN_FN_NCE116,
    AM_HAL_PIN_FN_NCE117,
    AM_HAL_PIN_FN_NCE118,
    AM_HAL_PIN_FN_NCE119,
    AM_HAL_PIN_FN_OBSBUS0,
    AM_HAL_PIN_FN_OBSBUS1,
    AM_HAL_PIN_FN_OBSBUS2,
    AM_HAL_PIN_FN_OBSBUS3,
    AM_HAL_PIN_FN_OBSBUS4,
    AM_HAL_PIN_FN_OBSBUS5,
    AM_HAL_PIN_FN_OBSBUS6,
    AM_HAL_PIN_FN_OBSBUS7,
    AM_HAL_PIN_FN_OBSBUS8,
    AM_HAL_PIN_FN_OBSBUS9,
    AM_HAL_PIN_FN_OBSBUS10,
    AM_HAL_PIN_FN_OBSBUS11,
    AM_HAL_PIN_FN_OBSBUS12,
    AM_HAL_PIN_FN_OBSBUS13,
    AM_HAL_PIN_FN_OBSBUS14,
    AM_HAL_PIN_FN_OBSBUS15,
    AM_HAL_PIN_FN_OPCG_LOAD,
    AM_HAL_PIN_FN_OPCG_TRIG,
    AM_HAL_PIN_FN_PDMI2S0_CLK,
    AM_HAL_PIN_FN_PDMI2S0_SDOUT,
    AM_HAL_PIN_FN_PDMI2S0_WS,
    AM_HAL_PIN_FN_PDM0_CLK,
    AM_HAL_PIN_FN_PDM0_DATA,
    AM_HAL_PIN_FN_REFCLK_EXT,
    AM_HAL_PIN_FN_RESERVED0,
    AM_HAL_PIN_FN_RESERVED1,
    AM_HAL_PIN_FN_RESERVED2,
    AM_HAL_PIN_FN_RESERVED4,
    AM_HAL_PIN_FN_RESERVED5,
    AM_HAL_PIN_FN_RESERVED9,
    AM_HAL_PIN_FN_RESERVED10,
    AM_HAL_PIN_FN_RESERVED12,
    AM_HAL_PIN_FN_RESERVED13,
    AM_HAL_PIN_FN_RESERVED14,
    AM_HAL_PIN_FN_RESERVED15,
    AM_HAL_PIN_FN_RF_XCVR_TST0,
    AM_HAL_PIN_FN_RF_XCVR_TST1,
    AM_HAL_PIN_FN_RF_XCVR_TST2,
    AM_HAL_PIN_FN_RF_XCVR_TST3,
    AM_HAL_PIN_FN_SCANCLK,
    AM_HAL_PIN_FN_SCANIN0,
    AM_HAL_PIN_FN_SCANIN1,
    AM_HAL_PIN_FN_SCANIN2,
    AM_HAL_PIN_FN_SCANIN3,
    AM_HAL_PIN_FN_SCANIN4,
    AM_HAL_PIN_FN_SCANIN5,
    AM_HAL_PIN_FN_SCANIN6,
    AM_HAL_PIN_FN_SCANIN7,
    AM_HAL_PIN_FN_SCANIN8,
    AM_HAL_PIN_FN_SCANIN9,
    AM_HAL_PIN_FN_SCANOUT0,
    AM_HAL_PIN_FN_SCANOUT1,
    AM_HAL_PIN_FN_SCANOUT2,
    AM_HAL_PIN_FN_SCANOUT3,
    AM_HAL_PIN_FN_SCANOUT4,
    AM_HAL_PIN_FN_SCANOUT5,
    AM_HAL_PIN_FN_SCANOUT6,
    AM_HAL_PIN_FN_SCANOUT7,
    AM_HAL_PIN_FN_SCANOUT8,
    AM_HAL_PIN_FN_SCANOUT9,
    AM_HAL_PIN_FN_SCANRST,
    AM_HAL_PIN_FN_SCANSHFT,
    AM_HAL_PIN_FN_SDIF0_CLKOUT,
    AM_HAL_PIN_FN_SDIF1_CLKOUT,
    AM_HAL_PIN_FN_SDIF0_CMD,
    AM_HAL_PIN_FN_SDIF1_CMD,
    AM_HAL_PIN_FN_SDIF0_DAT0,
    AM_HAL_PIN_FN_SDIF0_DAT1,
    AM_HAL_PIN_FN_SDIF0_DAT2,
    AM_HAL_PIN_FN_SDIF0_DAT3,
    AM_HAL_PIN_FN_SDIF0_DAT4,
    AM_HAL_PIN_FN_SDIF0_DAT5,
    AM_HAL_PIN_FN_SDIF0_DAT6,
    AM_HAL_PIN_FN_SDIF0_DAT7,
    AM_HAL_PIN_FN_SDIF1_DAT0,
    AM_HAL_PIN_FN_SDIF1_DAT1,
    AM_HAL_PIN_FN_SDIF1_DAT2,
    AM_HAL_PIN_FN_SDIF1_DAT3,
    AM_HAL_PIN_FN_SDIF1_DAT4,
    AM_HAL_PIN_FN_SDIF1_DAT5,
    AM_HAL_PIN_FN_SDIF1_DAT6,
    AM_HAL_PIN_FN_SDIF1_DAT7,
    AM_HAL_PIN_FN_SLFDINT,
    AM_HAL_PIN_FN_SLFDMISO,
    AM_HAL_PIN_FN_SLFDMOSI,
    AM_HAL_PIN_FN_SLFDSCK,
    AM_HAL_PIN_FN_SLFDSCL,
    AM_HAL_PIN_FN_SLFDSDAWIR3,
    AM_HAL_PIN_FN_SLFDnCE,
    AM_HAL_PIN_FN_ST_WCI_1W_RXD,
    AM_HAL_PIN_FN_ST_WCI_2W_RXD,
    AM_HAL_PIN_FN_ST_WCI_2W_TXD,
    AM_HAL_PIN_FN_SWDCK,
    AM_HAL_PIN_FN_SWDIO,
    AM_HAL_PIN_FN_SWO,
    AM_HAL_PIN_FN_SWTRACE0,
    AM_HAL_PIN_FN_SWTRACE1,
    AM_HAL_PIN_FN_SWTRACE2,
    AM_HAL_PIN_FN_SWTRACE3,
    AM_HAL_PIN_FN_SWTRACECLK,
    AM_HAL_PIN_FN_SWTRACECTL,
    AM_HAL_PIN_FN_TRIG0,
    AM_HAL_PIN_FN_TRIG1,
    AM_HAL_PIN_FN_TRIG2,
    AM_HAL_PIN_FN_TRIG3,
    AM_HAL_PIN_FN_UART0CTS,
    AM_HAL_PIN_FN_UART1CTS,
    AM_HAL_PIN_FN_UART0RTS,
    AM_HAL_PIN_FN_UART1RTS,
    AM_HAL_PIN_FN_UART0RX,
    AM_HAL_PIN_FN_UART1RX,
    AM_HAL_PIN_FN_UART0TX,
    AM_HAL_PIN_FN_UART1TX,
    AM_HAL_PIN_FN_VCMPO,
    AM_HAL_PIN_FN_XT_EXT
} am_hal_pin_function_e;

//*****************************************************************************
//
// Function selects by pin.
//
//*****************************************************************************
extern const uint16_t am_hal_pin_fn_list[AM_HAL_PIN_TOTAL_GPIOS][AM_HAL_PIN_NUMFUNCS];

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_PIN_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
