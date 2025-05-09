//*****************************************************************************
//
//! @file am_hal_pin.h
//!
//! @brief Function select information for Apollo5b GPIOs
//!
//! Date: 02/13/2024
//!
//! @addtogroup pin Pin Functionality
//! @ingroup apollo5b_hal
//! @{
//
// This is a generated file, do not modify!
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
#define AM_HAL_PIN_TOTAL_GPIOS                  (224)
#define AM_HAL_PIN_VIRTUAL_FIRST                (224)
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
#define AM_HAL_PIN_0_RESERVED2          2
#define AM_HAL_PIN_0_GPIO               3
#define AM_HAL_PIN_0_UART0TX            4
#define AM_HAL_PIN_0_UART1TX            5
#define AM_HAL_PIN_0_CT0                6
#define AM_HAL_PIN_0_NCE0               7
#define AM_HAL_PIN_0_OBSBUS0            8
#define AM_HAL_PIN_0_VCMPO              9
#define AM_HAL_PIN_0_RESERVED10         10
#define AM_HAL_PIN_0_FPIO               11
#define AM_HAL_PIN_0_RESERVED12         12
#define AM_HAL_PIN_0_RESERVED13         13
#define AM_HAL_PIN_0_RESERVED14         14
#define AM_HAL_PIN_0_RESERVED15         15

// PIN 1 functions
#define AM_HAL_PIN_1_SWTRACE0           0
#define AM_HAL_PIN_1_SLFDMOSI           1
#define AM_HAL_PIN_1_SLFDWIR3           2
#define AM_HAL_PIN_1_GPIO               3
#define AM_HAL_PIN_1_UART2TX            4
#define AM_HAL_PIN_1_UART3TX            5
#define AM_HAL_PIN_1_CT1                6
#define AM_HAL_PIN_1_NCE1               7
#define AM_HAL_PIN_1_OBSBUS1            8
#define AM_HAL_PIN_1_VCMPO              9
#define AM_HAL_PIN_1_SLFD1WIR3          10
#define AM_HAL_PIN_1_FPIO               11
#define AM_HAL_PIN_1_RESERVED12         12
#define AM_HAL_PIN_1_RESERVED13         13
#define AM_HAL_PIN_1_RESERVED14         14
#define AM_HAL_PIN_1_SCANOUT0           15

// PIN 2 functions
#define AM_HAL_PIN_2_SWTRACE1           0
#define AM_HAL_PIN_2_SLFDMISO           1
#define AM_HAL_PIN_2_TRIG1              2
#define AM_HAL_PIN_2_GPIO               3
#define AM_HAL_PIN_2_UART0RX            4
#define AM_HAL_PIN_2_UART1RX            5
#define AM_HAL_PIN_2_CT2                6
#define AM_HAL_PIN_2_NCE2               7
#define AM_HAL_PIN_2_OBSBUS2            8
#define AM_HAL_PIN_2_VCMPO              9
#define AM_HAL_PIN_2_SLFD1MISO          10
#define AM_HAL_PIN_2_FPIO               11
#define AM_HAL_PIN_2_RESERVED12         12
#define AM_HAL_PIN_2_RESERVED13         13
#define AM_HAL_PIN_2_RESERVED14         14
#define AM_HAL_PIN_2_SCANRST            15

// PIN 3 functions
#define AM_HAL_PIN_3_SWTRACE2           0
#define AM_HAL_PIN_3_SLFDnCE            1
#define AM_HAL_PIN_3_SWO                2
#define AM_HAL_PIN_3_GPIO               3
#define AM_HAL_PIN_3_UART2RX            4
#define AM_HAL_PIN_3_UART3RX            5
#define AM_HAL_PIN_3_CT3                6
#define AM_HAL_PIN_3_NCE3               7
#define AM_HAL_PIN_3_OBSBUS3            8
#define AM_HAL_PIN_3_RESERVED9          9
#define AM_HAL_PIN_3_I2S1_SDIN          10
#define AM_HAL_PIN_3_FPIO               11
#define AM_HAL_PIN_3_RESERVED12         12
#define AM_HAL_PIN_3_RESERVED13         13
#define AM_HAL_PIN_3_RESERVED14         14
#define AM_HAL_PIN_3_SCANIN5            15

// PIN 4 functions
#define AM_HAL_PIN_4_SWTRACE3           0
#define AM_HAL_PIN_4_SLFDINT            1
#define AM_HAL_PIN_4_32KHzXT            2
#define AM_HAL_PIN_4_GPIO               3
#define AM_HAL_PIN_4_UART0RTS           4
#define AM_HAL_PIN_4_UART1RTS           5
#define AM_HAL_PIN_4_CT4                6
#define AM_HAL_PIN_4_NCE4               7
#define AM_HAL_PIN_4_OBSBUS4            8
#define AM_HAL_PIN_4_I2S0_SDIN          9
#define AM_HAL_PIN_4_SLFD1INT           10
#define AM_HAL_PIN_4_FPIO               11
#define AM_HAL_PIN_4_FLB_TDO            12
#define AM_HAL_PIN_4_FLLOAD_DIR         13
#define AM_HAL_PIN_4_MDA_TDO            14
#define AM_HAL_PIN_4_SCANIN6            15

// PIN 5 functions
#define AM_HAL_PIN_5_M0SCL              0
#define AM_HAL_PIN_5_M0SCK              1
#define AM_HAL_PIN_5_I2S0_CLK           2
#define AM_HAL_PIN_5_GPIO               3
#define AM_HAL_PIN_5_UART2RTS           4
#define AM_HAL_PIN_5_UART3RTS           5
#define AM_HAL_PIN_5_CT5                6
#define AM_HAL_PIN_5_NCE5               7
#define AM_HAL_PIN_5_OBSBUS5            8
#define AM_HAL_PIN_5_RESERVED9          9
#define AM_HAL_PIN_5_RESERVED10         10
#define AM_HAL_PIN_5_FPIO               11
#define AM_HAL_PIN_5_FLB_TDI            12
#define AM_HAL_PIN_5_FLLOAD_DATA        13
#define AM_HAL_PIN_5_MDA_SRST           14
#define AM_HAL_PIN_5_SCANOUT2           15

// PIN 6 functions
#define AM_HAL_PIN_6_M0SDAWIR3          0
#define AM_HAL_PIN_6_M0MOSI             1
#define AM_HAL_PIN_6_I2S0_DATA          2
#define AM_HAL_PIN_6_GPIO               3
#define AM_HAL_PIN_6_UART0CTS           4
#define AM_HAL_PIN_6_UART1CTS           5
#define AM_HAL_PIN_6_CT6                6
#define AM_HAL_PIN_6_NCE6               7
#define AM_HAL_PIN_6_OBSBUS6            8
#define AM_HAL_PIN_6_I2S0_SDOUT         9
#define AM_HAL_PIN_6_RESERVED10         10
#define AM_HAL_PIN_6_FPIO               11
#define AM_HAL_PIN_6_FLB_TDO_1          12
#define AM_HAL_PIN_6_RESERVED13         13
#define AM_HAL_PIN_6_RESERVED14         14
#define AM_HAL_PIN_6_SCANOUT1           15

// PIN 7 functions
#define AM_HAL_PIN_7_M0MISO             0
#define AM_HAL_PIN_7_TRIG0              1
#define AM_HAL_PIN_7_I2S0_WS            2
#define AM_HAL_PIN_7_GPIO               3
#define AM_HAL_PIN_7_UART2CTS           4
#define AM_HAL_PIN_7_UART3CTS           5
#define AM_HAL_PIN_7_CT7                6
#define AM_HAL_PIN_7_NCE7               7
#define AM_HAL_PIN_7_OBSBUS7            8
#define AM_HAL_PIN_7_MNCE2_0            9
#define AM_HAL_PIN_7_MNCE3_1            10
#define AM_HAL_PIN_7_FPIO               11
#define AM_HAL_PIN_7_FLB_TDI_1          12
#define AM_HAL_PIN_7_RESERVED13         13
#define AM_HAL_PIN_7_RESERVED14         14
#define AM_HAL_PIN_7_SCANIN7            15

// PIN 8 functions
#define AM_HAL_PIN_8_CMPRF1             0
#define AM_HAL_PIN_8_TRIG1              1
#define AM_HAL_PIN_8_RESERVED2          2
#define AM_HAL_PIN_8_GPIO               3
#define AM_HAL_PIN_8_M1SCL              4
#define AM_HAL_PIN_8_M1SCK              5
#define AM_HAL_PIN_8_CT8                6
#define AM_HAL_PIN_8_NCE8               7
#define AM_HAL_PIN_8_OBSBUS8            8
#define AM_HAL_PIN_8_RESERVED9          9
#define AM_HAL_PIN_8_I2S1_CLK           10
#define AM_HAL_PIN_8_FPIO               11
#define AM_HAL_PIN_8_FLB_TCLK_1         12
#define AM_HAL_PIN_8_RESERVED13         13
#define AM_HAL_PIN_8_RESERVED14         14
#define AM_HAL_PIN_8_SCANOUT4           15

// PIN 9 functions
#define AM_HAL_PIN_9_CMPRF0             0
#define AM_HAL_PIN_9_TRIG2              1
#define AM_HAL_PIN_9_RESERVED2          2
#define AM_HAL_PIN_9_GPIO               3
#define AM_HAL_PIN_9_M1SDAWIR3          4
#define AM_HAL_PIN_9_M1MOSI             5
#define AM_HAL_PIN_9_CT9                6
#define AM_HAL_PIN_9_NCE9               7
#define AM_HAL_PIN_9_OBSBUS9            8
#define AM_HAL_PIN_9_I2S1_DATA          9
#define AM_HAL_PIN_9_I2S1_SDOUT         10
#define AM_HAL_PIN_9_FPIO               11
#define AM_HAL_PIN_9_FLB_TMS_1          12
#define AM_HAL_PIN_9_RESERVED13         13
#define AM_HAL_PIN_9_RESERVED14         14
#define AM_HAL_PIN_9_SCANOUT5           15

// PIN 10 functions
#define AM_HAL_PIN_10_CMPIN0            0
#define AM_HAL_PIN_10_TRIG3             1
#define AM_HAL_PIN_10_MNCE0_0           2
#define AM_HAL_PIN_10_GPIO              3
#define AM_HAL_PIN_10_M1MISO            4
#define AM_HAL_PIN_10_MNCE2_0           5
#define AM_HAL_PIN_10_CT10              6
#define AM_HAL_PIN_10_NCE10             7
#define AM_HAL_PIN_10_OBSBUS10          8
#define AM_HAL_PIN_10_DISP_TE           9
#define AM_HAL_PIN_10_I2S1_WS           10
#define AM_HAL_PIN_10_FPIO              11
#define AM_HAL_PIN_10_MNCE2_1           12
#define AM_HAL_PIN_10_RESERVED13        13
#define AM_HAL_PIN_10_RESERVED14        14
#define AM_HAL_PIN_10_OPCG_LOAD         15

// PIN 11 functions
#define AM_HAL_PIN_11_CMPIN1            0
#define AM_HAL_PIN_11_TRIG0             1
#define AM_HAL_PIN_11_I2S0_CLK          2
#define AM_HAL_PIN_11_GPIO              3
#define AM_HAL_PIN_11_UART2RX           4
#define AM_HAL_PIN_11_UART3RX           5
#define AM_HAL_PIN_11_CT11              6
#define AM_HAL_PIN_11_NCE11             7
#define AM_HAL_PIN_11_OBSBUS11          8
#define AM_HAL_PIN_11_SLSCL             9
#define AM_HAL_PIN_11_SLSCK             10
#define AM_HAL_PIN_11_FPIO              11
#define AM_HAL_PIN_11_FLB_TCLK          12
#define AM_HAL_PIN_11_FLLOAD_ADDR       13
#define AM_HAL_PIN_11_MDA_TCK           14
#define AM_HAL_PIN_11_SCANIN4           15

// PIN 12 functions
#define AM_HAL_PIN_12_ADCSE7            0
#define AM_HAL_PIN_12_TRIG1             1
#define AM_HAL_PIN_12_I2S0_DATA         2
#define AM_HAL_PIN_12_GPIO              3
#define AM_HAL_PIN_12_UART0TX           4
#define AM_HAL_PIN_12_UART1TX           5
#define AM_HAL_PIN_12_CT12              6
#define AM_HAL_PIN_12_NCE12             7
#define AM_HAL_PIN_12_OBSBUS12          8
#define AM_HAL_PIN_12_CMPRF2            9
#define AM_HAL_PIN_12_I2S0_SDOUT        10
#define AM_HAL_PIN_12_FPIO              11
#define AM_HAL_PIN_12_RESERVED12        12
#define AM_HAL_PIN_12_RESERVED13        13
#define AM_HAL_PIN_12_RESERVED14        14
#define AM_HAL_PIN_12_SCANIN3           15

// PIN 13 functions
#define AM_HAL_PIN_13_ADCSE6            0
#define AM_HAL_PIN_13_TRIG2             1
#define AM_HAL_PIN_13_I2S0_WS           2
#define AM_HAL_PIN_13_GPIO              3
#define AM_HAL_PIN_13_UART2TX           4
#define AM_HAL_PIN_13_UART3TX           5
#define AM_HAL_PIN_13_CT13              6
#define AM_HAL_PIN_13_NCE13             7
#define AM_HAL_PIN_13_OBSBUS13          8
#define AM_HAL_PIN_13_SLnCE             9
#define AM_HAL_PIN_13_RESERVED10        10
#define AM_HAL_PIN_13_FPIO              11
#define AM_HAL_PIN_13_FLB_FCLK          12
#define AM_HAL_PIN_13_FLLOAD_DATA       13
#define AM_HAL_PIN_13_MDA_TDI           14
#define AM_HAL_PIN_13_SCANIN0           15

// PIN 14 functions
#define AM_HAL_PIN_14_ADCSE5            0
#define AM_HAL_PIN_14_TRIG3             1
#define AM_HAL_PIN_14_RESERVED2         2
#define AM_HAL_PIN_14_GPIO              3
#define AM_HAL_PIN_14_RESERVED4         4
#define AM_HAL_PIN_14_UART1RX           5
#define AM_HAL_PIN_14_CT14              6
#define AM_HAL_PIN_14_NCE14             7
#define AM_HAL_PIN_14_OBSBUS14          8
#define AM_HAL_PIN_14_RESERVED9         9
#define AM_HAL_PIN_14_I2S0_SDIN         10
#define AM_HAL_PIN_14_FPIO              11
#define AM_HAL_PIN_14_RESERVED12        12
#define AM_HAL_PIN_14_FLLOAD_ADDR       13
#define AM_HAL_PIN_14_MDA_TRSTN         14
#define AM_HAL_PIN_14_SCANIN2           15

// PIN 15 functions
#define AM_HAL_PIN_15_ADCSE4            0
#define AM_HAL_PIN_15_TRIG0             1
#define AM_HAL_PIN_15_RESERVED2         2
#define AM_HAL_PIN_15_GPIO              3
#define AM_HAL_PIN_15_RESERVED4         4
#define AM_HAL_PIN_15_UART3RX           5
#define AM_HAL_PIN_15_CT15              6
#define AM_HAL_PIN_15_NCE15             7
#define AM_HAL_PIN_15_OBSBUS15          8
#define AM_HAL_PIN_15_RESERVED9         9
#define AM_HAL_PIN_15_REFCLK_EXT        10
#define AM_HAL_PIN_15_FPIO              11
#define AM_HAL_PIN_15_RESERVED12        12
#define AM_HAL_PIN_15_FLLOAD_DATA       13
#define AM_HAL_PIN_15_RESERVED14        14
#define AM_HAL_PIN_15_SCANIN1           15

// PIN 16 functions
#define AM_HAL_PIN_16_ADCSE3            0
#define AM_HAL_PIN_16_TRIG1             1
#define AM_HAL_PIN_16_I2S1_CLK          2
#define AM_HAL_PIN_16_GPIO              3
#define AM_HAL_PIN_16_RESERVED4         4
#define AM_HAL_PIN_16_UART1RTS          5
#define AM_HAL_PIN_16_CT16              6
#define AM_HAL_PIN_16_NCE16             7
#define AM_HAL_PIN_16_OBSBUS0           8
#define AM_HAL_PIN_16_RESERVED9         9
#define AM_HAL_PIN_16_EN192MHz          10
#define AM_HAL_PIN_16_FPIO              11
#define AM_HAL_PIN_16_RESERVED12        12
#define AM_HAL_PIN_16_RESERVED13        13
#define AM_HAL_PIN_16_RESERVED14        14
#define AM_HAL_PIN_16_DFT_RET           15

// PIN 17 functions
#define AM_HAL_PIN_17_ADCSE2            0
#define AM_HAL_PIN_17_TRIG2             1
#define AM_HAL_PIN_17_I2S1_DATA         2
#define AM_HAL_PIN_17_GPIO              3
#define AM_HAL_PIN_17_RESERVED4         4
#define AM_HAL_PIN_17_UART3RTS          5
#define AM_HAL_PIN_17_CT17              6
#define AM_HAL_PIN_17_NCE17             7
#define AM_HAL_PIN_17_OBSBUS1           8
#define AM_HAL_PIN_17_I2S1_SDOUT        9
#define AM_HAL_PIN_17_CLK192MHz         10
#define AM_HAL_PIN_17_FPIO              11
#define AM_HAL_PIN_17_RESERVED12        12
#define AM_HAL_PIN_17_FLLOAD_STRB       13
#define AM_HAL_PIN_17_MDA_TMS           14
#define AM_HAL_PIN_17_RESERVED15        15

// PIN 18 functions
#define AM_HAL_PIN_18_ADCSE1            0
#define AM_HAL_PIN_18_ANATEST2          1
#define AM_HAL_PIN_18_I2S1_WS           2
#define AM_HAL_PIN_18_GPIO              3
#define AM_HAL_PIN_18_UART0CTS          4
#define AM_HAL_PIN_18_UART1CTS          5
#define AM_HAL_PIN_18_CT18              6
#define AM_HAL_PIN_18_NCE18             7
#define AM_HAL_PIN_18_OBSBUS2           8
#define AM_HAL_PIN_18_RESERVED9         9
#define AM_HAL_PIN_18_RESERVED10        10
#define AM_HAL_PIN_18_FPIO              11
#define AM_HAL_PIN_18_FLB_TMS           12
#define AM_HAL_PIN_18_FLLOAD_DATA       13
#define AM_HAL_PIN_18_MDA_HFRC_EXT      14
#define AM_HAL_PIN_18_OPCG_TRIG         15

// PIN 19 functions
#define AM_HAL_PIN_19_ADCSE0            0
#define AM_HAL_PIN_19_ANATEST1          1
#define AM_HAL_PIN_19_RESERVED2         2
#define AM_HAL_PIN_19_GPIO              3
#define AM_HAL_PIN_19_UART2CTS          4
#define AM_HAL_PIN_19_UART3CTS          5
#define AM_HAL_PIN_19_CT19              6
#define AM_HAL_PIN_19_NCE19             7
#define AM_HAL_PIN_19_OBSBUS3           8
#define AM_HAL_PIN_19_I2S1_SDIN         9
#define AM_HAL_PIN_19_RESERVED10        10
#define AM_HAL_PIN_19_FPIO              11
#define AM_HAL_PIN_19_FLB_TRSTN         12
#define AM_HAL_PIN_19_FLLOAD_ADDR       13
#define AM_HAL_PIN_19_RESERVED14        14
#define AM_HAL_PIN_19_DFT_ISO           15

// PIN 20 functions
#define AM_HAL_PIN_20_SWDCK             0
#define AM_HAL_PIN_20_TRIG1             1
#define AM_HAL_PIN_20_RESERVED2         2
#define AM_HAL_PIN_20_GPIO              3
#define AM_HAL_PIN_20_UART0TX           4
#define AM_HAL_PIN_20_UART1TX           5
#define AM_HAL_PIN_20_CT20              6
#define AM_HAL_PIN_20_NCE20             7
#define AM_HAL_PIN_20_OBSBUS4           8
#define AM_HAL_PIN_20_RESERVED9         9
#define AM_HAL_PIN_20_RESERVED10        10
#define AM_HAL_PIN_20_FPIO              11
#define AM_HAL_PIN_20_RESERVED12        12
#define AM_HAL_PIN_20_RESERVED13        13
#define AM_HAL_PIN_20_RESERVED14        14
#define AM_HAL_PIN_20_SCANCLK           15

// PIN 21 functions
#define AM_HAL_PIN_21_SWDIO             0
#define AM_HAL_PIN_21_TRIG2             1
#define AM_HAL_PIN_21_RESERVED2         2
#define AM_HAL_PIN_21_GPIO              3
#define AM_HAL_PIN_21_UART0RX           4
#define AM_HAL_PIN_21_UART1RX           5
#define AM_HAL_PIN_21_CT21              6
#define AM_HAL_PIN_21_NCE21             7
#define AM_HAL_PIN_21_OBSBUS5           8
#define AM_HAL_PIN_21_RESERVED9         9
#define AM_HAL_PIN_21_RESERVED10        10
#define AM_HAL_PIN_21_FPIO              11
#define AM_HAL_PIN_21_RESERVED12        12
#define AM_HAL_PIN_21_RESERVED13        13
#define AM_HAL_PIN_21_RESERVED14        14
#define AM_HAL_PIN_21_SCANSHFT          15

// PIN 22 functions
#define AM_HAL_PIN_22_M7SCL             0
#define AM_HAL_PIN_22_M7SCK             1
#define AM_HAL_PIN_22_SWO               2
#define AM_HAL_PIN_22_GPIO              3
#define AM_HAL_PIN_22_UART2TX           4
#define AM_HAL_PIN_22_UART3TX           5
#define AM_HAL_PIN_22_CT22              6
#define AM_HAL_PIN_22_NCE22             7
#define AM_HAL_PIN_22_OBSBUS6           8
#define AM_HAL_PIN_22_VCMPO             9
#define AM_HAL_PIN_22_RESERVED10        10
#define AM_HAL_PIN_22_FPIO              11
#define AM_HAL_PIN_22_RESERVED12        12
#define AM_HAL_PIN_22_RESERVED13        13
#define AM_HAL_PIN_22_RESERVED14        14
#define AM_HAL_PIN_22_SCANOUT3          15

// PIN 23 functions
#define AM_HAL_PIN_23_M7SDAWIR3         0
#define AM_HAL_PIN_23_M7MOSI            1
#define AM_HAL_PIN_23_SWO               2
#define AM_HAL_PIN_23_GPIO              3
#define AM_HAL_PIN_23_UART2RX           4
#define AM_HAL_PIN_23_UART3RX           5
#define AM_HAL_PIN_23_CT23              6
#define AM_HAL_PIN_23_NCE23             7
#define AM_HAL_PIN_23_OBSBUS7           8
#define AM_HAL_PIN_23_VCMPO             9
#define AM_HAL_PIN_23_RESERVED10        10
#define AM_HAL_PIN_23_FPIO              11
#define AM_HAL_PIN_23_RESERVED12        12
#define AM_HAL_PIN_23_RESERVED13        13
#define AM_HAL_PIN_23_RESERVED14        14
#define AM_HAL_PIN_23_SCANOUT6          15

// PIN 24 functions
#define AM_HAL_PIN_24_M7MISO            0
#define AM_HAL_PIN_24_TRIG3             1
#define AM_HAL_PIN_24_SWO               2
#define AM_HAL_PIN_24_GPIO              3
#define AM_HAL_PIN_24_UART0RTS          4
#define AM_HAL_PIN_24_UART1RTS          5
#define AM_HAL_PIN_24_CT24              6
#define AM_HAL_PIN_24_NCE24             7
#define AM_HAL_PIN_24_OBSBUS8           8
#define AM_HAL_PIN_24_MNCE0_0           9
#define AM_HAL_PIN_24_MNCE0_1           10
#define AM_HAL_PIN_24_FPIO              11
#define AM_HAL_PIN_24_RESERVED12        12
#define AM_HAL_PIN_24_RESERVED13        13
#define AM_HAL_PIN_24_RESERVED14        14
#define AM_HAL_PIN_24_SCANOUT7          15

// PIN 25 functions
#define AM_HAL_PIN_25_M2SCL             0
#define AM_HAL_PIN_25_M2SCK             1
#define AM_HAL_PIN_25_RESERVED2         2
#define AM_HAL_PIN_25_GPIO              3
#define AM_HAL_PIN_25_LFRC_EXT          4
#define AM_HAL_PIN_25_UART1TX           5
#define AM_HAL_PIN_25_CT25              6
#define AM_HAL_PIN_25_NCE25             7
#define AM_HAL_PIN_25_OBSBUS9           8
#define AM_HAL_PIN_25_RESERVED9         9
#define AM_HAL_PIN_25_RESERVED10        10
#define AM_HAL_PIN_25_FPIO              11
#define AM_HAL_PIN_25_RESERVED12        12
#define AM_HAL_PIN_25_RESERVED13        13
#define AM_HAL_PIN_25_RESERVED14        14
#define AM_HAL_PIN_25_SCANIN8           15

// PIN 26 functions
#define AM_HAL_PIN_26_M2SDAWIR3         0
#define AM_HAL_PIN_26_M2MOSI            1
#define AM_HAL_PIN_26_RESERVED2         2
#define AM_HAL_PIN_26_GPIO              3
#define AM_HAL_PIN_26_HFRC_EXT          4
#define AM_HAL_PIN_26_UART1RX           5
#define AM_HAL_PIN_26_CT26              6
#define AM_HAL_PIN_26_NCE26             7
#define AM_HAL_PIN_26_OBSBUS10          8
#define AM_HAL_PIN_26_VCMPO             9
#define AM_HAL_PIN_26_RESERVED10        10
#define AM_HAL_PIN_26_FPIO              11
#define AM_HAL_PIN_26_RESERVED12        12
#define AM_HAL_PIN_26_RESERVED13        13
#define AM_HAL_PIN_26_RESERVED14        14
#define AM_HAL_PIN_26_SCANIN9           15

// PIN 27 functions
#define AM_HAL_PIN_27_M2MISO            0
#define AM_HAL_PIN_27_TRIG0             1
#define AM_HAL_PIN_27_MNCE3_0           2
#define AM_HAL_PIN_27_GPIO              3
#define AM_HAL_PIN_27_XT_EXT            4
#define AM_HAL_PIN_27_UART1CTS          5
#define AM_HAL_PIN_27_CT27              6
#define AM_HAL_PIN_27_NCE27             7
#define AM_HAL_PIN_27_OBSBUS11          8
#define AM_HAL_PIN_27_RESERVED9         9
#define AM_HAL_PIN_27_RESERVED10        10
#define AM_HAL_PIN_27_FPIO              11
#define AM_HAL_PIN_27_RESERVED12        12
#define AM_HAL_PIN_27_RESERVED13        13
#define AM_HAL_PIN_27_RESERVED14        14
#define AM_HAL_PIN_27_RESERVED15        15

// PIN 28 functions
#define AM_HAL_PIN_28_SWO               0
#define AM_HAL_PIN_28_VCMPO             1
#define AM_HAL_PIN_28_RESERVED2         2
#define AM_HAL_PIN_28_GPIO              3
#define AM_HAL_PIN_28_UART2CTS          4
#define AM_HAL_PIN_28_RESERVED5         5
#define AM_HAL_PIN_28_CT28              6
#define AM_HAL_PIN_28_NCE28             7
#define AM_HAL_PIN_28_OBSBUS12          8
#define AM_HAL_PIN_28_RESERVED9         9
#define AM_HAL_PIN_28_RESERVED10        10
#define AM_HAL_PIN_28_FPIO              11
#define AM_HAL_PIN_28_RESERVED12        12
#define AM_HAL_PIN_28_RESERVED13        13
#define AM_HAL_PIN_28_RESERVED14        14
#define AM_HAL_PIN_28_CME               15

// PIN 29 functions
#define AM_HAL_PIN_29_TRIG0             0
#define AM_HAL_PIN_29_VCMPO             1
#define AM_HAL_PIN_29_RESERVED2         2
#define AM_HAL_PIN_29_GPIO              3
#define AM_HAL_PIN_29_UART1CTS          4
#define AM_HAL_PIN_29_RESERVED5         5
#define AM_HAL_PIN_29_CT29              6
#define AM_HAL_PIN_29_NCE29             7
#define AM_HAL_PIN_29_OBSBUS13          8
#define AM_HAL_PIN_29_RESERVED9         9
#define AM_HAL_PIN_29_RESERVED10        10
#define AM_HAL_PIN_29_FPIO              11
#define AM_HAL_PIN_29_RESERVED12        12
#define AM_HAL_PIN_29_RESERVED13        13
#define AM_HAL_PIN_29_RESERVED14        14
#define AM_HAL_PIN_29_CMLE              15

// PIN 30 functions
#define AM_HAL_PIN_30_TRIG1             0
#define AM_HAL_PIN_30_VCMPO             1
#define AM_HAL_PIN_30_RESERVED2         2
#define AM_HAL_PIN_30_GPIO              3
#define AM_HAL_PIN_30_UART0TX           4
#define AM_HAL_PIN_30_RESERVED5         5
#define AM_HAL_PIN_30_CT30              6
#define AM_HAL_PIN_30_NCE30             7
#define AM_HAL_PIN_30_OBSBUS14          8
#define AM_HAL_PIN_30_RESERVED9         9
#define AM_HAL_PIN_30_RESERVED10        10
#define AM_HAL_PIN_30_FPIO              11
#define AM_HAL_PIN_30_RESERVED12        12
#define AM_HAL_PIN_30_RESERVED13        13
#define AM_HAL_PIN_30_RESERVED14        14
#define AM_HAL_PIN_30_RESERVED15        15

// PIN 31 functions
#define AM_HAL_PIN_31_M3SCL             0
#define AM_HAL_PIN_31_M3SCK             1
#define AM_HAL_PIN_31_I2S0_CLK          2
#define AM_HAL_PIN_31_GPIO              3
#define AM_HAL_PIN_31_UART2TX           4
#define AM_HAL_PIN_31_UART2CTS          5
#define AM_HAL_PIN_31_CT31              6
#define AM_HAL_PIN_31_NCE31             7
#define AM_HAL_PIN_31_OBSBUS15          8
#define AM_HAL_PIN_31_VCMPO             9
#define AM_HAL_PIN_31_RESERVED10        10
#define AM_HAL_PIN_31_FPIO              11
#define AM_HAL_PIN_31_RESERVED12        12
#define AM_HAL_PIN_31_RESERVED13        13
#define AM_HAL_PIN_31_RESERVED14        14
#define AM_HAL_PIN_31_SCANOUT9          15

// PIN 32 functions
#define AM_HAL_PIN_32_M3SDAWIR3         0
#define AM_HAL_PIN_32_M3MOSI            1
#define AM_HAL_PIN_32_I2S0_DATA         2
#define AM_HAL_PIN_32_GPIO              3
#define AM_HAL_PIN_32_UART0RX           4
#define AM_HAL_PIN_32_UART3CTS          5
#define AM_HAL_PIN_32_CT32              6
#define AM_HAL_PIN_32_NCE32             7
#define AM_HAL_PIN_32_OBSBUS0           8
#define AM_HAL_PIN_32_I2S0_SDOUT        9
#define AM_HAL_PIN_32_RESERVED10        10
#define AM_HAL_PIN_32_FPIO              11
#define AM_HAL_PIN_32_RESERVED12        12
#define AM_HAL_PIN_32_RESERVED13        13
#define AM_HAL_PIN_32_RESERVED14        14
#define AM_HAL_PIN_32_LPG_ENABLE        15

// PIN 33 functions
#define AM_HAL_PIN_33_M3MISO            0
#define AM_HAL_PIN_33_CLKOUT            1
#define AM_HAL_PIN_33_I2S0_WS           2
#define AM_HAL_PIN_33_GPIO              3
#define AM_HAL_PIN_33_UART2RX           4
#define AM_HAL_PIN_33_UART2RTS          5
#define AM_HAL_PIN_33_CT33              6
#define AM_HAL_PIN_33_NCE33             7
#define AM_HAL_PIN_33_OBSBUS1           8
#define AM_HAL_PIN_33_DISP_TE           9
#define AM_HAL_PIN_33_MNCE1_0           10
#define AM_HAL_PIN_33_FPIO              11
#define AM_HAL_PIN_33_RESERVED12        12
#define AM_HAL_PIN_33_RESERVED13        13
#define AM_HAL_PIN_33_RESERVED14        14
#define AM_HAL_PIN_33_LPG_LOAD          15

// PIN 34 functions
#define AM_HAL_PIN_34_M4SCL             0
#define AM_HAL_PIN_34_M4SCK             1
#define AM_HAL_PIN_34_SWO               2
#define AM_HAL_PIN_34_GPIO              3
#define AM_HAL_PIN_34_UART0TX           4
#define AM_HAL_PIN_34_UART2RX           5
#define AM_HAL_PIN_34_CT34              6
#define AM_HAL_PIN_34_NCE34             7
#define AM_HAL_PIN_34_OBSBUS2           8
#define AM_HAL_PIN_34_VCMPO             9
#define AM_HAL_PIN_34_I2S1_CLK          10
#define AM_HAL_PIN_34_FPIO              11
#define AM_HAL_PIN_34_RESERVED12        12
#define AM_HAL_PIN_34_RESERVED13        13
#define AM_HAL_PIN_34_RESERVED14        14
#define AM_HAL_PIN_34_RESERVED15        15

// PIN 35 functions
#define AM_HAL_PIN_35_M4SDAWIR3         0
#define AM_HAL_PIN_35_M4MOSI            1
#define AM_HAL_PIN_35_SWO               2
#define AM_HAL_PIN_35_GPIO              3
#define AM_HAL_PIN_35_UART2TX           4
#define AM_HAL_PIN_35_UART3TX           5
#define AM_HAL_PIN_35_CT35              6
#define AM_HAL_PIN_35_NCE35             7
#define AM_HAL_PIN_35_OBSBUS3           8
#define AM_HAL_PIN_35_I2S1_SDOUT        9
#define AM_HAL_PIN_35_I2S1_DATA         10
#define AM_HAL_PIN_35_FPIO              11
#define AM_HAL_PIN_35_RESERVED12        12
#define AM_HAL_PIN_35_RESERVED13        13
#define AM_HAL_PIN_35_RESERVED14        14
#define AM_HAL_PIN_35_SCANOUT8          15

// PIN 36 functions
#define AM_HAL_PIN_36_M4MISO            0
#define AM_HAL_PIN_36_TRIG0             1
#define AM_HAL_PIN_36_MNCE3_0           2
#define AM_HAL_PIN_36_GPIO              3
#define AM_HAL_PIN_36_UART0RX           4
#define AM_HAL_PIN_36_UART1RX           5
#define AM_HAL_PIN_36_CT36              6
#define AM_HAL_PIN_36_NCE36             7
#define AM_HAL_PIN_36_OBSBUS4           8
#define AM_HAL_PIN_36_MNCE1_0           9
#define AM_HAL_PIN_36_I2S1_WS           10
#define AM_HAL_PIN_36_FPIO              11
#define AM_HAL_PIN_36_RESERVED12        12
#define AM_HAL_PIN_36_RESERVED13        13
#define AM_HAL_PIN_36_RESERVED14        14
#define AM_HAL_PIN_36_RESERVED15        15

// PIN 37 functions
#define AM_HAL_PIN_37_MSPI0_10          0
#define AM_HAL_PIN_37_TRIG1             1
#define AM_HAL_PIN_37_32KHzXT           2
#define AM_HAL_PIN_37_GPIO              3
#define AM_HAL_PIN_37_UART2RX           4
#define AM_HAL_PIN_37_UART3RX           5
#define AM_HAL_PIN_37_CT37              6
#define AM_HAL_PIN_37_NCE37             7
#define AM_HAL_PIN_37_OBSBUS5           8
#define AM_HAL_PIN_37_RESERVED9         9
#define AM_HAL_PIN_37_RESERVED10        10
#define AM_HAL_PIN_37_FPIO              11
#define AM_HAL_PIN_37_RESERVED12        12
#define AM_HAL_PIN_37_RESERVED13        13
#define AM_HAL_PIN_37_RESERVED14        14
#define AM_HAL_PIN_37_RESERVED15        15

// PIN 38 functions
#define AM_HAL_PIN_38_MSPI0_11          0
#define AM_HAL_PIN_38_TRIG2             1
#define AM_HAL_PIN_38_SWTRACECLK        2
#define AM_HAL_PIN_38_GPIO              3
#define AM_HAL_PIN_38_UART0RTS          4
#define AM_HAL_PIN_38_UART2RTS          5
#define AM_HAL_PIN_38_CT38              6
#define AM_HAL_PIN_38_NCE38             7
#define AM_HAL_PIN_38_OBSBUS6           8
#define AM_HAL_PIN_38_RESERVED9         9
#define AM_HAL_PIN_38_RESERVED10        10
#define AM_HAL_PIN_38_FPIO              11
#define AM_HAL_PIN_38_RESERVED12        12
#define AM_HAL_PIN_38_RESERVED13        13
#define AM_HAL_PIN_38_RESERVED14        14
#define AM_HAL_PIN_38_RESERVED15        15

// PIN 39 functions
#define AM_HAL_PIN_39_MSPI0_12          0
#define AM_HAL_PIN_39_TRIG3             1
#define AM_HAL_PIN_39_SWTRACE0          2
#define AM_HAL_PIN_39_GPIO              3
#define AM_HAL_PIN_39_UART2RTS          4
#define AM_HAL_PIN_39_UART3RTS          5
#define AM_HAL_PIN_39_CT39              6
#define AM_HAL_PIN_39_NCE39             7
#define AM_HAL_PIN_39_OBSBUS7           8
#define AM_HAL_PIN_39_RESERVED9         9
#define AM_HAL_PIN_39_RESERVED10        10
#define AM_HAL_PIN_39_FPIO              11
#define AM_HAL_PIN_39_RESERVED12        12
#define AM_HAL_PIN_39_RESERVED13        13
#define AM_HAL_PIN_39_RESERVED14        14
#define AM_HAL_PIN_39_RESERVED15        15

// PIN 40 functions
#define AM_HAL_PIN_40_MSPI0_13          0
#define AM_HAL_PIN_40_TRIG1             1
#define AM_HAL_PIN_40_SWTRACE1          2
#define AM_HAL_PIN_40_GPIO              3
#define AM_HAL_PIN_40_UART0CTS          4
#define AM_HAL_PIN_40_UART1CTS          5
#define AM_HAL_PIN_40_CT40              6
#define AM_HAL_PIN_40_NCE40             7
#define AM_HAL_PIN_40_OBSBUS8           8
#define AM_HAL_PIN_40_RESERVED9         9
#define AM_HAL_PIN_40_RESERVED10        10
#define AM_HAL_PIN_40_FPIO              11
#define AM_HAL_PIN_40_RESERVED12        12
#define AM_HAL_PIN_40_RESERVED13        13
#define AM_HAL_PIN_40_RESERVED14        14
#define AM_HAL_PIN_40_RESERVED15        15

// PIN 41 functions
#define AM_HAL_PIN_41_MSPI0_14          0
#define AM_HAL_PIN_41_TRIG0             1
#define AM_HAL_PIN_41_SWTRACE2          2
#define AM_HAL_PIN_41_GPIO              3
#define AM_HAL_PIN_41_UART0TX           4
#define AM_HAL_PIN_41_UART1TX           5
#define AM_HAL_PIN_41_CT41              6
#define AM_HAL_PIN_41_NCE41             7
#define AM_HAL_PIN_41_OBSBUS9           8
#define AM_HAL_PIN_41_SWO               9
#define AM_HAL_PIN_41_RESERVED10        10
#define AM_HAL_PIN_41_FPIO              11
#define AM_HAL_PIN_41_RESERVED12        12
#define AM_HAL_PIN_41_RESERVED13        13
#define AM_HAL_PIN_41_RESERVED14        14
#define AM_HAL_PIN_41_RESERVED15        15

// PIN 42 functions
#define AM_HAL_PIN_42_MSPI0_15          0
#define AM_HAL_PIN_42_TRIG2             1
#define AM_HAL_PIN_42_SWTRACE3          2
#define AM_HAL_PIN_42_GPIO              3
#define AM_HAL_PIN_42_UART2TX           4
#define AM_HAL_PIN_42_UART3TX           5
#define AM_HAL_PIN_42_CT42              6
#define AM_HAL_PIN_42_NCE42             7
#define AM_HAL_PIN_42_OBSBUS10          8
#define AM_HAL_PIN_42_RESERVED9         9
#define AM_HAL_PIN_42_RESERVED10        10
#define AM_HAL_PIN_42_FPIO              11
#define AM_HAL_PIN_42_RESERVED12        12
#define AM_HAL_PIN_42_RESERVED13        13
#define AM_HAL_PIN_42_RESERVED14        14
#define AM_HAL_PIN_42_RESERVED15        15

// PIN 43 functions
#define AM_HAL_PIN_43_MSPI0_16          0
#define AM_HAL_PIN_43_TRIG3             1
#define AM_HAL_PIN_43_SWTRACECTL        2
#define AM_HAL_PIN_43_GPIO              3
#define AM_HAL_PIN_43_UART0RX           4
#define AM_HAL_PIN_43_UART1RX           5
#define AM_HAL_PIN_43_CT43              6
#define AM_HAL_PIN_43_NCE43             7
#define AM_HAL_PIN_43_OBSBUS11          8
#define AM_HAL_PIN_43_RESERVED9         9
#define AM_HAL_PIN_43_RESERVED10        10
#define AM_HAL_PIN_43_FPIO              11
#define AM_HAL_PIN_43_RESERVED12        12
#define AM_HAL_PIN_43_RESERVED13        13
#define AM_HAL_PIN_43_RESERVED14        14
#define AM_HAL_PIN_43_RESERVED15        15

// PIN 44 functions
#define AM_HAL_PIN_44_MSPI0_17          0
#define AM_HAL_PIN_44_TRIG1             1
#define AM_HAL_PIN_44_SWO               2
#define AM_HAL_PIN_44_GPIO              3
#define AM_HAL_PIN_44_UART2RX           4
#define AM_HAL_PIN_44_UART3RX           5
#define AM_HAL_PIN_44_CT44              6
#define AM_HAL_PIN_44_NCE44             7
#define AM_HAL_PIN_44_OBSBUS12          8
#define AM_HAL_PIN_44_VCMPO             9
#define AM_HAL_PIN_44_RESERVED10        10
#define AM_HAL_PIN_44_FPIO              11
#define AM_HAL_PIN_44_RESERVED12        12
#define AM_HAL_PIN_44_RESERVED13        13
#define AM_HAL_PIN_44_RESERVED14        14
#define AM_HAL_PIN_44_RESERVED15        15

// PIN 45 functions
#define AM_HAL_PIN_45_MSPI0_18          0
#define AM_HAL_PIN_45_TRIG2             1
#define AM_HAL_PIN_45_32KHzXT           2
#define AM_HAL_PIN_45_GPIO              3
#define AM_HAL_PIN_45_UART0TX           4
#define AM_HAL_PIN_45_UART1TX           5
#define AM_HAL_PIN_45_CT45              6
#define AM_HAL_PIN_45_NCE45             7
#define AM_HAL_PIN_45_OBSBUS13          8
#define AM_HAL_PIN_45_RESERVED9         9
#define AM_HAL_PIN_45_RESERVED10        10
#define AM_HAL_PIN_45_FPIO              11
#define AM_HAL_PIN_45_RESERVED12        12
#define AM_HAL_PIN_45_RESERVED13        13
#define AM_HAL_PIN_45_RESERVED14        14
#define AM_HAL_PIN_45_RESERVED15        15

// PIN 46 functions
#define AM_HAL_PIN_46_RESERVED0         0
#define AM_HAL_PIN_46_TRIG3             1
#define AM_HAL_PIN_46_CLKOUT_32M        2
#define AM_HAL_PIN_46_GPIO              3
#define AM_HAL_PIN_46_UART2TX           4
#define AM_HAL_PIN_46_UART3TX           5
#define AM_HAL_PIN_46_CT46              6
#define AM_HAL_PIN_46_NCE46             7
#define AM_HAL_PIN_46_OBSBUS14          8
#define AM_HAL_PIN_46_RESERVED9         9
#define AM_HAL_PIN_46_RESERVED10        10
#define AM_HAL_PIN_46_FPIO              11
#define AM_HAL_PIN_46_RESERVED12        12
#define AM_HAL_PIN_46_RESERVED13        13
#define AM_HAL_PIN_46_RESERVED14        14
#define AM_HAL_PIN_46_RESERVED15        15

// PIN 47 functions
#define AM_HAL_PIN_47_M5SCL             0
#define AM_HAL_PIN_47_M5SCK             1
#define AM_HAL_PIN_47_RESERVED2         2
#define AM_HAL_PIN_47_GPIO              3
#define AM_HAL_PIN_47_UART0RX           4
#define AM_HAL_PIN_47_UART1RX           5
#define AM_HAL_PIN_47_CT47              6
#define AM_HAL_PIN_47_NCE47             7
#define AM_HAL_PIN_47_OBSBUS15          8
#define AM_HAL_PIN_47_RESERVED9         9
#define AM_HAL_PIN_47_I2S0_CLK          10
#define AM_HAL_PIN_47_FPIO              11
#define AM_HAL_PIN_47_RESERVED12        12
#define AM_HAL_PIN_47_RESERVED13        13
#define AM_HAL_PIN_47_RESERVED14        14
#define AM_HAL_PIN_47_RESERVED15        15

// PIN 48 functions
#define AM_HAL_PIN_48_M5SDAWIR3         0
#define AM_HAL_PIN_48_M5MOSI            1
#define AM_HAL_PIN_48_RESERVED2         2
#define AM_HAL_PIN_48_GPIO              3
#define AM_HAL_PIN_48_UART2RX           4
#define AM_HAL_PIN_48_UART3RX           5
#define AM_HAL_PIN_48_CT48              6
#define AM_HAL_PIN_48_NCE48             7
#define AM_HAL_PIN_48_OBSBUS0           8
#define AM_HAL_PIN_48_RESERVED9         9
#define AM_HAL_PIN_48_I2S0_WS           10
#define AM_HAL_PIN_48_FPIO              11
#define AM_HAL_PIN_48_RESERVED12        12
#define AM_HAL_PIN_48_RESERVED13        13
#define AM_HAL_PIN_48_RESERVED14        14
#define AM_HAL_PIN_48_RESERVED15        15

// PIN 49 functions
#define AM_HAL_PIN_49_M5MISO            0
#define AM_HAL_PIN_49_TRIG0             1
#define AM_HAL_PIN_49_MNCE1_0           2
#define AM_HAL_PIN_49_GPIO              3
#define AM_HAL_PIN_49_UART0RTS          4
#define AM_HAL_PIN_49_UART1RTS          5
#define AM_HAL_PIN_49_CT49              6
#define AM_HAL_PIN_49_NCE49             7
#define AM_HAL_PIN_49_OBSBUS1           8
#define AM_HAL_PIN_49_I2S0_DATA         9
#define AM_HAL_PIN_49_I2S0_SDOUT        10
#define AM_HAL_PIN_49_FPIO              11
#define AM_HAL_PIN_49_MNCE1_1           12
#define AM_HAL_PIN_49_RESERVED13        13
#define AM_HAL_PIN_49_RESERVED14        14
#define AM_HAL_PIN_49_RESERVED15        15

// PIN 50 functions
#define AM_HAL_PIN_50_PDM0_CLK          0
#define AM_HAL_PIN_50_TRIG0             1
#define AM_HAL_PIN_50_SWTRACECLK        2
#define AM_HAL_PIN_50_GPIO              3
#define AM_HAL_PIN_50_UART2RTS          4
#define AM_HAL_PIN_50_UART3RTS          5
#define AM_HAL_PIN_50_CT50              6
#define AM_HAL_PIN_50_NCE50             7
#define AM_HAL_PIN_50_OBSBUS2           8
#define AM_HAL_PIN_50_DISP_TE           9
#define AM_HAL_PIN_50_RESERVED10        10
#define AM_HAL_PIN_50_FPIO              11
#define AM_HAL_PIN_50_RESERVED12        12
#define AM_HAL_PIN_50_RESERVED13        13
#define AM_HAL_PIN_50_RESERVED14        14
#define AM_HAL_PIN_50_EN96MHz           15

// PIN 51 functions
#define AM_HAL_PIN_51_PDM0_DATA         0
#define AM_HAL_PIN_51_TRIG1             1
#define AM_HAL_PIN_51_SWTRACE0          2
#define AM_HAL_PIN_51_GPIO              3
#define AM_HAL_PIN_51_UART0CTS          4
#define AM_HAL_PIN_51_UART1CTS          5
#define AM_HAL_PIN_51_CT51              6
#define AM_HAL_PIN_51_NCE51             7
#define AM_HAL_PIN_51_OBSBUS3           8
#define AM_HAL_PIN_51_RESERVED9         9
#define AM_HAL_PIN_51_RESERVED10        10
#define AM_HAL_PIN_51_FPIO              11
#define AM_HAL_PIN_51_RESERVED12        12
#define AM_HAL_PIN_51_RESERVED13        13
#define AM_HAL_PIN_51_RESERVED14        14
#define AM_HAL_PIN_51_CLK96MHz          15

// PIN 52 functions
#define AM_HAL_PIN_52_MNCE3_0           0
#define AM_HAL_PIN_52_TRIG2             1
#define AM_HAL_PIN_52_SWTRACE1          2
#define AM_HAL_PIN_52_GPIO              3
#define AM_HAL_PIN_52_UART2CTS          4
#define AM_HAL_PIN_52_UART3CTS          5
#define AM_HAL_PIN_52_CT52              6
#define AM_HAL_PIN_52_NCE52             7
#define AM_HAL_PIN_52_OBSBUS4           8
#define AM_HAL_PIN_52_VCMPO             9
#define AM_HAL_PIN_52_I2S0_CLK          10
#define AM_HAL_PIN_52_FPIO              11
#define AM_HAL_PIN_52_SLSDAWIR3         12
#define AM_HAL_PIN_52_SLMOSI            13
#define AM_HAL_PIN_52_RESERVED14        14
#define AM_HAL_PIN_52_RESERVED15        15

// PIN 53 functions
#define AM_HAL_PIN_53_MNCE1_0           0
#define AM_HAL_PIN_53_TRIG3             1
#define AM_HAL_PIN_53_SWTRACE2          2
#define AM_HAL_PIN_53_GPIO              3
#define AM_HAL_PIN_53_UART0TX           4
#define AM_HAL_PIN_53_UART1TX           5
#define AM_HAL_PIN_53_CT53              6
#define AM_HAL_PIN_53_NCE53             7
#define AM_HAL_PIN_53_OBSBUS5           8
#define AM_HAL_PIN_53_RESERVED9         9
#define AM_HAL_PIN_53_I2S0_WS           10
#define AM_HAL_PIN_53_FPIO              11
#define AM_HAL_PIN_53_RESERVED12        12
#define AM_HAL_PIN_53_RESERVED13        13
#define AM_HAL_PIN_53_RESERVED14        14
#define AM_HAL_PIN_53_RESERVED15        15

// PIN 54 functions
#define AM_HAL_PIN_54_MNCE1_1           0
#define AM_HAL_PIN_54_TRIG0             1
#define AM_HAL_PIN_54_SWTRACE3          2
#define AM_HAL_PIN_54_GPIO              3
#define AM_HAL_PIN_54_UART2TX           4
#define AM_HAL_PIN_54_UART3TX           5
#define AM_HAL_PIN_54_CT54              6
#define AM_HAL_PIN_54_NCE54             7
#define AM_HAL_PIN_54_OBSBUS6           8
#define AM_HAL_PIN_54_I2S0_DATA         9
#define AM_HAL_PIN_54_I2S0_SDOUT        10
#define AM_HAL_PIN_54_FPIO              11
#define AM_HAL_PIN_54_RESERVED12        12
#define AM_HAL_PIN_54_RESERVED13        13
#define AM_HAL_PIN_54_RESERVED14        14
#define AM_HAL_PIN_54_RESERVED15        15

// PIN 55 functions
#define AM_HAL_PIN_55_MNCE3_1           0
#define AM_HAL_PIN_55_TRIG1             1
#define AM_HAL_PIN_55_SWTRACECTL        2
#define AM_HAL_PIN_55_GPIO              3
#define AM_HAL_PIN_55_UART0RX           4
#define AM_HAL_PIN_55_UART1RX           5
#define AM_HAL_PIN_55_CT55              6
#define AM_HAL_PIN_55_NCE55             7
#define AM_HAL_PIN_55_OBSBUS7           8
#define AM_HAL_PIN_55_RESERVED9         9
#define AM_HAL_PIN_55_I2S1_CLK          10
#define AM_HAL_PIN_55_FPIO              11
#define AM_HAL_PIN_55_RESERVED12        12
#define AM_HAL_PIN_55_RESERVED13        13
#define AM_HAL_PIN_55_RESERVED14        14
#define AM_HAL_PIN_55_RESERVED15        15

// PIN 56 functions
#define AM_HAL_PIN_56_MNCE0_0           0
#define AM_HAL_PIN_56_TRIG2             1
#define AM_HAL_PIN_56_SWO               2
#define AM_HAL_PIN_56_GPIO              3
#define AM_HAL_PIN_56_UART2RX           4
#define AM_HAL_PIN_56_UART3RX           5
#define AM_HAL_PIN_56_CT56              6
#define AM_HAL_PIN_56_NCE56             7
#define AM_HAL_PIN_56_OBSBUS8           8
#define AM_HAL_PIN_56_VCMPO             9
#define AM_HAL_PIN_56_I2S1_WS           10
#define AM_HAL_PIN_56_FPIO              11
#define AM_HAL_PIN_56_RESERVED12        12
#define AM_HAL_PIN_56_RESERVED13        13
#define AM_HAL_PIN_56_RESERVED14        14
#define AM_HAL_PIN_56_RESERVED15        15

// PIN 57 functions
#define AM_HAL_PIN_57_MNCE0_1           0
#define AM_HAL_PIN_57_TRIG3             1
#define AM_HAL_PIN_57_SWO               2
#define AM_HAL_PIN_57_GPIO              3
#define AM_HAL_PIN_57_UART0RTS          4
#define AM_HAL_PIN_57_UART1RTS          5
#define AM_HAL_PIN_57_CT57              6
#define AM_HAL_PIN_57_NCE57             7
#define AM_HAL_PIN_57_OBSBUS9           8
#define AM_HAL_PIN_57_I2S1_DATA         9
#define AM_HAL_PIN_57_I2S1_SDOUT        10
#define AM_HAL_PIN_57_FPIO              11
#define AM_HAL_PIN_57_RESERVED12        12
#define AM_HAL_PIN_57_RESERVED13        13
#define AM_HAL_PIN_57_RESERVED14        14
#define AM_HAL_PIN_57_RESERVED15        15

// PIN 58 functions
#define AM_HAL_PIN_58_RESERVED0         0
#define AM_HAL_PIN_58_RESERVED1         1
#define AM_HAL_PIN_58_RESERVED2         2
#define AM_HAL_PIN_58_GPIO              3
#define AM_HAL_PIN_58_UART0RTS          4
#define AM_HAL_PIN_58_UART3RTS          5
#define AM_HAL_PIN_58_CT58              6
#define AM_HAL_PIN_58_NCE58             7
#define AM_HAL_PIN_58_OBSBUS10          8
#define AM_HAL_PIN_58_RESERVED9         9
#define AM_HAL_PIN_58_RESERVED10        10
#define AM_HAL_PIN_58_FPIO              11
#define AM_HAL_PIN_58_RESERVED12        12
#define AM_HAL_PIN_58_RESERVED13        13
#define AM_HAL_PIN_58_RESERVED14        14
#define AM_HAL_PIN_58_RESERVED15        15

// PIN 59 functions
#define AM_HAL_PIN_59_RESERVED0         0
#define AM_HAL_PIN_59_TRIG0             1
#define AM_HAL_PIN_59_RESERVED2         2
#define AM_HAL_PIN_59_GPIO              3
#define AM_HAL_PIN_59_UART0CTS          4
#define AM_HAL_PIN_59_UART1CTS          5
#define AM_HAL_PIN_59_CT59              6
#define AM_HAL_PIN_59_NCE59             7
#define AM_HAL_PIN_59_OBSBUS11          8
#define AM_HAL_PIN_59_RESERVED9         9
#define AM_HAL_PIN_59_RESERVED10        10
#define AM_HAL_PIN_59_FPIO              11
#define AM_HAL_PIN_59_RESERVED12        12
#define AM_HAL_PIN_59_RESERVED13        13
#define AM_HAL_PIN_59_RESERVED14        14
#define AM_HAL_PIN_59_RESERVED15        15

// PIN 60 functions
#define AM_HAL_PIN_60_RESERVED0         0
#define AM_HAL_PIN_60_TRIG1             1
#define AM_HAL_PIN_60_RESERVED2         2
#define AM_HAL_PIN_60_GPIO              3
#define AM_HAL_PIN_60_UART0TX           4
#define AM_HAL_PIN_60_UART3CTS          5
#define AM_HAL_PIN_60_CT60              6
#define AM_HAL_PIN_60_NCE60             7
#define AM_HAL_PIN_60_OBSBUS12          8
#define AM_HAL_PIN_60_RESERVED9         9
#define AM_HAL_PIN_60_RESERVED10        10
#define AM_HAL_PIN_60_FPIO              11
#define AM_HAL_PIN_60_RESERVED12        12
#define AM_HAL_PIN_60_RESERVED13        13
#define AM_HAL_PIN_60_RESERVED14        14
#define AM_HAL_PIN_60_RESERVED15        15

// PIN 61 functions
#define AM_HAL_PIN_61_M6SCL             0
#define AM_HAL_PIN_61_M6SCK             1
#define AM_HAL_PIN_61_I2S1_CLK          2
#define AM_HAL_PIN_61_GPIO              3
#define AM_HAL_PIN_61_UART2TX           4
#define AM_HAL_PIN_61_UART3TX           5
#define AM_HAL_PIN_61_CT61              6
#define AM_HAL_PIN_61_NCE61             7
#define AM_HAL_PIN_61_OBSBUS13          8
#define AM_HAL_PIN_61_RESERVED9         9
#define AM_HAL_PIN_61_RESERVED10        10
#define AM_HAL_PIN_61_FPIO              11
#define AM_HAL_PIN_61_RESERVED12        12
#define AM_HAL_PIN_61_RESERVED13        13
#define AM_HAL_PIN_61_RESERVED14        14
#define AM_HAL_PIN_61_RESERVED15        15

// PIN 62 functions
#define AM_HAL_PIN_62_M6SDAWIR3         0
#define AM_HAL_PIN_62_M6MOSI            1
#define AM_HAL_PIN_62_I2S1_DATA         2
#define AM_HAL_PIN_62_GPIO              3
#define AM_HAL_PIN_62_UART0RX           4
#define AM_HAL_PIN_62_UART1RX           5
#define AM_HAL_PIN_62_CT62              6
#define AM_HAL_PIN_62_NCE62             7
#define AM_HAL_PIN_62_OBSBUS14          8
#define AM_HAL_PIN_62_I2S1_SDOUT        9
#define AM_HAL_PIN_62_RESERVED10        10
#define AM_HAL_PIN_62_FPIO              11
#define AM_HAL_PIN_62_RESERVED12        12
#define AM_HAL_PIN_62_RESERVED13        13
#define AM_HAL_PIN_62_RESERVED14        14
#define AM_HAL_PIN_62_RESERVED15        15

// PIN 63 functions
#define AM_HAL_PIN_63_M6MISO            0
#define AM_HAL_PIN_63_CLKOUT            1
#define AM_HAL_PIN_63_I2S1_WS           2
#define AM_HAL_PIN_63_GPIO              3
#define AM_HAL_PIN_63_UART2RX           4
#define AM_HAL_PIN_63_UART3RX           5
#define AM_HAL_PIN_63_CT63              6
#define AM_HAL_PIN_63_NCE63             7
#define AM_HAL_PIN_63_OBSBUS15          8
#define AM_HAL_PIN_63_DISP_TE           9
#define AM_HAL_PIN_63_MNCE2_0           10
#define AM_HAL_PIN_63_FPIO              11
#define AM_HAL_PIN_63_MNCE2_1           12
#define AM_HAL_PIN_63_RESERVED13        13
#define AM_HAL_PIN_63_RESERVED14        14
#define AM_HAL_PIN_63_RESERVED15        15

// PIN 64 functions
#define AM_HAL_PIN_64_MSPI0_0           0
#define AM_HAL_PIN_64_32KHzXT           1
#define AM_HAL_PIN_64_SWO               2
#define AM_HAL_PIN_64_GPIO              3
#define AM_HAL_PIN_64_UART0RTS          4
#define AM_HAL_PIN_64_UART2CTS          5
#define AM_HAL_PIN_64_CT64              6
#define AM_HAL_PIN_64_NCE64             7
#define AM_HAL_PIN_64_OBSBUS0           8
#define AM_HAL_PIN_64_I2S1_SDIN         9
#define AM_HAL_PIN_64_RESERVED10        10
#define AM_HAL_PIN_64_FPIO              11
#define AM_HAL_PIN_64_RESERVED12        12
#define AM_HAL_PIN_64_RESERVED13        13
#define AM_HAL_PIN_64_RESERVED14        14
#define AM_HAL_PIN_64_RESERVED15        15

// PIN 65 functions
#define AM_HAL_PIN_65_MSPI0_1           0
#define AM_HAL_PIN_65_32KHzXT           1
#define AM_HAL_PIN_65_SWO               2
#define AM_HAL_PIN_65_GPIO              3
#define AM_HAL_PIN_65_UART0CTS          4
#define AM_HAL_PIN_65_UART1CTS          5
#define AM_HAL_PIN_65_CT65              6
#define AM_HAL_PIN_65_NCE65             7
#define AM_HAL_PIN_65_OBSBUS1           8
#define AM_HAL_PIN_65_RESERVED9         9
#define AM_HAL_PIN_65_RESERVED10        10
#define AM_HAL_PIN_65_FPIO              11
#define AM_HAL_PIN_65_RESERVED12        12
#define AM_HAL_PIN_65_RESERVED13        13
#define AM_HAL_PIN_65_RESERVED14        14
#define AM_HAL_PIN_65_RESERVED15        15

// PIN 66 functions
#define AM_HAL_PIN_66_MSPI0_2           0
#define AM_HAL_PIN_66_CLKOUT            1
#define AM_HAL_PIN_66_SWO               2
#define AM_HAL_PIN_66_GPIO              3
#define AM_HAL_PIN_66_UART0TX           4
#define AM_HAL_PIN_66_UART1TX           5
#define AM_HAL_PIN_66_CT66              6
#define AM_HAL_PIN_66_NCE66             7
#define AM_HAL_PIN_66_OBSBUS2           8
#define AM_HAL_PIN_66_RESERVED9         9
#define AM_HAL_PIN_66_RESERVED10        10
#define AM_HAL_PIN_66_FPIO              11
#define AM_HAL_PIN_66_RESERVED12        12
#define AM_HAL_PIN_66_RESERVED13        13
#define AM_HAL_PIN_66_RESERVED14        14
#define AM_HAL_PIN_66_RESERVED15        15

// PIN 67 functions
#define AM_HAL_PIN_67_MSPI0_3           0
#define AM_HAL_PIN_67_CLKOUT            1
#define AM_HAL_PIN_67_SWO               2
#define AM_HAL_PIN_67_GPIO              3
#define AM_HAL_PIN_67_UART2TX           4
#define AM_HAL_PIN_67_UART3TX           5
#define AM_HAL_PIN_67_CT67              6
#define AM_HAL_PIN_67_NCE67             7
#define AM_HAL_PIN_67_OBSBUS3           8
#define AM_HAL_PIN_67_RESERVED9         9
#define AM_HAL_PIN_67_RESERVED10        10
#define AM_HAL_PIN_67_FPIO              11
#define AM_HAL_PIN_67_RESERVED12        12
#define AM_HAL_PIN_67_RESERVED13        13
#define AM_HAL_PIN_67_RESERVED14        14
#define AM_HAL_PIN_67_RESERVED15        15

// PIN 68 functions
#define AM_HAL_PIN_68_MSPI0_4           0
#define AM_HAL_PIN_68_SWO               1
#define AM_HAL_PIN_68_RESERVED2         2
#define AM_HAL_PIN_68_GPIO              3
#define AM_HAL_PIN_68_UART0RX           4
#define AM_HAL_PIN_68_UART1RX           5
#define AM_HAL_PIN_68_CT68              6
#define AM_HAL_PIN_68_NCE68             7
#define AM_HAL_PIN_68_OBSBUS4           8
#define AM_HAL_PIN_68_RESERVED9         9
#define AM_HAL_PIN_68_RESERVED10        10
#define AM_HAL_PIN_68_FPIO              11
#define AM_HAL_PIN_68_RESERVED12        12
#define AM_HAL_PIN_68_RESERVED13        13
#define AM_HAL_PIN_68_RESERVED14        14
#define AM_HAL_PIN_68_RESERVED15        15

// PIN 69 functions
#define AM_HAL_PIN_69_MSPI0_5           0
#define AM_HAL_PIN_69_32KHzXT           1
#define AM_HAL_PIN_69_SWO               2
#define AM_HAL_PIN_69_GPIO              3
#define AM_HAL_PIN_69_UART2RX           4
#define AM_HAL_PIN_69_UART3RX           5
#define AM_HAL_PIN_69_CT69              6
#define AM_HAL_PIN_69_NCE69             7
#define AM_HAL_PIN_69_OBSBUS5           8
#define AM_HAL_PIN_69_RESERVED9         9
#define AM_HAL_PIN_69_RESERVED10        10
#define AM_HAL_PIN_69_FPIO              11
#define AM_HAL_PIN_69_RESERVED12        12
#define AM_HAL_PIN_69_RESERVED13        13
#define AM_HAL_PIN_69_RESERVED14        14
#define AM_HAL_PIN_69_RESERVED15        15

// PIN 70 functions
#define AM_HAL_PIN_70_MSPI0_6           0
#define AM_HAL_PIN_70_32KHzXT           1
#define AM_HAL_PIN_70_SWTRACE0          2
#define AM_HAL_PIN_70_GPIO              3
#define AM_HAL_PIN_70_UART0RTS          4
#define AM_HAL_PIN_70_UART1RTS          5
#define AM_HAL_PIN_70_CT70              6
#define AM_HAL_PIN_70_NCE70             7
#define AM_HAL_PIN_70_OBSBUS6           8
#define AM_HAL_PIN_70_RESERVED9         9
#define AM_HAL_PIN_70_RESERVED10        10
#define AM_HAL_PIN_70_FPIO              11
#define AM_HAL_PIN_70_RESERVED12        12
#define AM_HAL_PIN_70_RESERVED13        13
#define AM_HAL_PIN_70_RESERVED14        14
#define AM_HAL_PIN_70_RESERVED15        15

// PIN 71 functions
#define AM_HAL_PIN_71_MSPI0_7           0
#define AM_HAL_PIN_71_CLKOUT            1
#define AM_HAL_PIN_71_SWTRACE1          2
#define AM_HAL_PIN_71_GPIO              3
#define AM_HAL_PIN_71_UART0CTS          4
#define AM_HAL_PIN_71_UART3RTS          5
#define AM_HAL_PIN_71_CT71              6
#define AM_HAL_PIN_71_NCE71             7
#define AM_HAL_PIN_71_OBSBUS7           8
#define AM_HAL_PIN_71_RESERVED9         9
#define AM_HAL_PIN_71_RESERVED10        10
#define AM_HAL_PIN_71_FPIO              11
#define AM_HAL_PIN_71_RESERVED12        12
#define AM_HAL_PIN_71_RESERVED13        13
#define AM_HAL_PIN_71_RESERVED14        14
#define AM_HAL_PIN_71_RESERVED15        15

// PIN 72 functions
#define AM_HAL_PIN_72_MSPI0_8           0
#define AM_HAL_PIN_72_CLKOUT            1
#define AM_HAL_PIN_72_SWTRACE2          2
#define AM_HAL_PIN_72_GPIO              3
#define AM_HAL_PIN_72_UART0TX           4
#define AM_HAL_PIN_72_UART1TX           5
#define AM_HAL_PIN_72_CT72              6
#define AM_HAL_PIN_72_NCE72             7
#define AM_HAL_PIN_72_OBSBUS8           8
#define AM_HAL_PIN_72_VCMPO             9
#define AM_HAL_PIN_72_RESERVED10        10
#define AM_HAL_PIN_72_FPIO              11
#define AM_HAL_PIN_72_RESERVED12        12
#define AM_HAL_PIN_72_RESERVED13        13
#define AM_HAL_PIN_72_RESERVED14        14
#define AM_HAL_PIN_72_RESERVED15        15

// PIN 73 functions
#define AM_HAL_PIN_73_MSPI0_9           0
#define AM_HAL_PIN_73_RESERVED1         1
#define AM_HAL_PIN_73_SWTRACE3          2
#define AM_HAL_PIN_73_GPIO              3
#define AM_HAL_PIN_73_UART2TX           4
#define AM_HAL_PIN_73_UART3TX           5
#define AM_HAL_PIN_73_CT73              6
#define AM_HAL_PIN_73_NCE73             7
#define AM_HAL_PIN_73_OBSBUS9           8
#define AM_HAL_PIN_73_RESERVED9         9
#define AM_HAL_PIN_73_RESERVED10        10
#define AM_HAL_PIN_73_FPIO              11
#define AM_HAL_PIN_73_RESERVED12        12
#define AM_HAL_PIN_73_RESERVED13        13
#define AM_HAL_PIN_73_RESERVED14        14
#define AM_HAL_PIN_73_RESERVED15        15

// PIN 74 functions
#define AM_HAL_PIN_74_MSPI2_0           0
#define AM_HAL_PIN_74_RESERVED1         1
#define AM_HAL_PIN_74_RESERVED2         2
#define AM_HAL_PIN_74_GPIO              3
#define AM_HAL_PIN_74_UART0RX           4
#define AM_HAL_PIN_74_UART3CTS          5
#define AM_HAL_PIN_74_CT74              6
#define AM_HAL_PIN_74_NCE74             7
#define AM_HAL_PIN_74_OBSBUS10          8
#define AM_HAL_PIN_74_RESERVED9         9
#define AM_HAL_PIN_74_RESERVED10        10
#define AM_HAL_PIN_74_FPIO              11
#define AM_HAL_PIN_74_RESERVED12        12
#define AM_HAL_PIN_74_RESERVED13        13
#define AM_HAL_PIN_74_RESERVED14        14
#define AM_HAL_PIN_74_RESERVED15        15

// PIN 75 functions
#define AM_HAL_PIN_75_MSPI2_1           0
#define AM_HAL_PIN_75_32KHzXT           1
#define AM_HAL_PIN_75_RESERVED2         2
#define AM_HAL_PIN_75_GPIO              3
#define AM_HAL_PIN_75_UART2RX           4
#define AM_HAL_PIN_75_UART3RX           5
#define AM_HAL_PIN_75_CT75              6
#define AM_HAL_PIN_75_NCE75             7
#define AM_HAL_PIN_75_OBSBUS11          8
#define AM_HAL_PIN_75_RESERVED9         9
#define AM_HAL_PIN_75_RESERVED10        10
#define AM_HAL_PIN_75_FPIO              11
#define AM_HAL_PIN_75_RESERVED12        12
#define AM_HAL_PIN_75_RESERVED13        13
#define AM_HAL_PIN_75_RESERVED14        14
#define AM_HAL_PIN_75_RESERVED15        15

// PIN 76 functions
#define AM_HAL_PIN_76_MSPI2_2           0
#define AM_HAL_PIN_76_32KHzXT           1
#define AM_HAL_PIN_76_RESERVED2         2
#define AM_HAL_PIN_76_GPIO              3
#define AM_HAL_PIN_76_UART0RTS          4
#define AM_HAL_PIN_76_UART1RTS          5
#define AM_HAL_PIN_76_CT76              6
#define AM_HAL_PIN_76_NCE76             7
#define AM_HAL_PIN_76_OBSBUS12          8
#define AM_HAL_PIN_76_RESERVED9         9
#define AM_HAL_PIN_76_RESERVED10        10
#define AM_HAL_PIN_76_FPIO              11
#define AM_HAL_PIN_76_RESERVED12        12
#define AM_HAL_PIN_76_RESERVED13        13
#define AM_HAL_PIN_76_RESERVED14        14
#define AM_HAL_PIN_76_RESERVED15        15

// PIN 77 functions
#define AM_HAL_PIN_77_MSPI2_3           0
#define AM_HAL_PIN_77_RESERVED1         1
#define AM_HAL_PIN_77_RESERVED2         2
#define AM_HAL_PIN_77_GPIO              3
#define AM_HAL_PIN_77_UART0CTS          4
#define AM_HAL_PIN_77_UART1CTS          5
#define AM_HAL_PIN_77_CT77              6
#define AM_HAL_PIN_77_NCE77             7
#define AM_HAL_PIN_77_OBSBUS13          8
#define AM_HAL_PIN_77_RESERVED9         9
#define AM_HAL_PIN_77_RESERVED10        10
#define AM_HAL_PIN_77_FPIO              11
#define AM_HAL_PIN_77_RESERVED12        12
#define AM_HAL_PIN_77_RESERVED13        13
#define AM_HAL_PIN_77_RESERVED14        14
#define AM_HAL_PIN_77_RESERVED15        15

// PIN 78 functions
#define AM_HAL_PIN_78_MSPI2_4           0
#define AM_HAL_PIN_78_RESERVED1         1
#define AM_HAL_PIN_78_RESERVED2         2
#define AM_HAL_PIN_78_GPIO              3
#define AM_HAL_PIN_78_UART0TX           4
#define AM_HAL_PIN_78_UART1TX           5
#define AM_HAL_PIN_78_CT78              6
#define AM_HAL_PIN_78_NCE78             7
#define AM_HAL_PIN_78_OBSBUS14          8
#define AM_HAL_PIN_78_RESERVED9         9
#define AM_HAL_PIN_78_RESERVED10        10
#define AM_HAL_PIN_78_FPIO              11
#define AM_HAL_PIN_78_RESERVED12        12
#define AM_HAL_PIN_78_RESERVED13        13
#define AM_HAL_PIN_78_RESERVED14        14
#define AM_HAL_PIN_78_RESERVED15        15

// PIN 79 functions
#define AM_HAL_PIN_79_MSPI2_5           0
#define AM_HAL_PIN_79_DISP_QSPI_D0_OUT  1
#define AM_HAL_PIN_79_DISP_QSPI_D0      2
#define AM_HAL_PIN_79_GPIO              3
#define AM_HAL_PIN_79_SWO               4
#define AM_HAL_PIN_79_UART1RTS          5
#define AM_HAL_PIN_79_CT79              6
#define AM_HAL_PIN_79_NCE79             7
#define AM_HAL_PIN_79_OBSBUS15          8
#define AM_HAL_PIN_79_DISP_SPI_SD       9
#define AM_HAL_PIN_79_DISP_SPI_SDO      10
#define AM_HAL_PIN_79_FPIO              11
#define AM_HAL_PIN_79_RESERVED12        12
#define AM_HAL_PIN_79_RESERVED13        13
#define AM_HAL_PIN_79_RESERVED14        14
#define AM_HAL_PIN_79_RESERVED15        15

// PIN 80 functions
#define AM_HAL_PIN_80_MSPI2_6           0
#define AM_HAL_PIN_80_CLKOUT            1
#define AM_HAL_PIN_80_DISP_QSPI_D1      2
#define AM_HAL_PIN_80_GPIO              3
#define AM_HAL_PIN_80_SWTRACE0          4
#define AM_HAL_PIN_80_UART2CTS          5
#define AM_HAL_PIN_80_CT80              6
#define AM_HAL_PIN_80_NCE80             7
#define AM_HAL_PIN_80_OBSBUS0           8
#define AM_HAL_PIN_80_DISP_SPI_DCX      9
#define AM_HAL_PIN_80_RESERVED10        10
#define AM_HAL_PIN_80_FPIO              11
#define AM_HAL_PIN_80_RESERVED12        12
#define AM_HAL_PIN_80_RESERVED13        13
#define AM_HAL_PIN_80_RESERVED14        14
#define AM_HAL_PIN_80_RESERVED15        15

// PIN 81 functions
#define AM_HAL_PIN_81_MSPI2_7           0
#define AM_HAL_PIN_81_CLKOUT            1
#define AM_HAL_PIN_81_DISP_QSPI_SCK     2
#define AM_HAL_PIN_81_GPIO              3
#define AM_HAL_PIN_81_SWTRACE1          4
#define AM_HAL_PIN_81_UART2RTS          5
#define AM_HAL_PIN_81_CT81              6
#define AM_HAL_PIN_81_NCE81             7
#define AM_HAL_PIN_81_OBSBUS1           8
#define AM_HAL_PIN_81_DISP_SPI_SCK      9
#define AM_HAL_PIN_81_RESERVED10        10
#define AM_HAL_PIN_81_FPIO              11
#define AM_HAL_PIN_81_RESERVED12        12
#define AM_HAL_PIN_81_RESERVED13        13
#define AM_HAL_PIN_81_RESERVED14        14
#define AM_HAL_PIN_81_RESERVED15        15

// PIN 82 functions
#define AM_HAL_PIN_82_MSPI2_8           0
#define AM_HAL_PIN_82_32KHzXT           1
#define AM_HAL_PIN_82_DISP_QSPI_D2      2
#define AM_HAL_PIN_82_GPIO              3
#define AM_HAL_PIN_82_SWTRACE2          4
#define AM_HAL_PIN_82_UART3CTS          5
#define AM_HAL_PIN_82_CT82              6
#define AM_HAL_PIN_82_NCE82             7
#define AM_HAL_PIN_82_OBSBUS2           8
#define AM_HAL_PIN_82_DISP_SPI_SDI      9
#define AM_HAL_PIN_82_RESERVED10        10
#define AM_HAL_PIN_82_FPIO              11
#define AM_HAL_PIN_82_RESERVED12        12
#define AM_HAL_PIN_82_RESERVED13        13
#define AM_HAL_PIN_82_RESERVED14        14
#define AM_HAL_PIN_82_RESERVED15        15

// PIN 83 functions
#define AM_HAL_PIN_83_MSPI2_9           0
#define AM_HAL_PIN_83_32KHzXT           1
#define AM_HAL_PIN_83_DISP_QSPI_D3      2
#define AM_HAL_PIN_83_GPIO              3
#define AM_HAL_PIN_83_SWTRACE3          4
#define AM_HAL_PIN_83_UART3RTS          5
#define AM_HAL_PIN_83_CT83              6
#define AM_HAL_PIN_83_NCE83             7
#define AM_HAL_PIN_83_OBSBUS3           8
#define AM_HAL_PIN_83_DISP_SPI_RST      9
#define AM_HAL_PIN_83_SLMISO            10
#define AM_HAL_PIN_83_FPIO              11
#define AM_HAL_PIN_83_RESERVED12        12
#define AM_HAL_PIN_83_RESERVED13        13
#define AM_HAL_PIN_83_RESERVED14        14
#define AM_HAL_PIN_83_RESERVED15        15

// PIN 84 functions
#define AM_HAL_PIN_84_RESERVED0         0
#define AM_HAL_PIN_84_RESERVED1         1
#define AM_HAL_PIN_84_SDIF0_DAT0        2
#define AM_HAL_PIN_84_GPIO              3
#define AM_HAL_PIN_84_RESERVED4         4
#define AM_HAL_PIN_84_RESERVED5         5
#define AM_HAL_PIN_84_CT84              6
#define AM_HAL_PIN_84_NCE84             7
#define AM_HAL_PIN_84_OBSBUS4           8
#define AM_HAL_PIN_84_RESERVED9         9
#define AM_HAL_PIN_84_RESERVED10        10
#define AM_HAL_PIN_84_FPIO              11
#define AM_HAL_PIN_84_RESERVED12        12
#define AM_HAL_PIN_84_RESERVED13        13
#define AM_HAL_PIN_84_RESERVED14        14
#define AM_HAL_PIN_84_RESERVED15        15

// PIN 85 functions
#define AM_HAL_PIN_85_RESERVED0         0
#define AM_HAL_PIN_85_RESERVED1         1
#define AM_HAL_PIN_85_SDIF0_DAT1        2
#define AM_HAL_PIN_85_GPIO              3
#define AM_HAL_PIN_85_RESERVED4         4
#define AM_HAL_PIN_85_RESERVED5         5
#define AM_HAL_PIN_85_CT85              6
#define AM_HAL_PIN_85_NCE85             7
#define AM_HAL_PIN_85_OBSBUS5           8
#define AM_HAL_PIN_85_RESERVED9         9
#define AM_HAL_PIN_85_RESERVED10        10
#define AM_HAL_PIN_85_FPIO              11
#define AM_HAL_PIN_85_RESERVED12        12
#define AM_HAL_PIN_85_RESERVED13        13
#define AM_HAL_PIN_85_RESERVED14        14
#define AM_HAL_PIN_85_RESERVED15        15

// PIN 86 functions
#define AM_HAL_PIN_86_RESERVED0         0
#define AM_HAL_PIN_86_RESERVED1         1
#define AM_HAL_PIN_86_SDIF0_DAT2        2
#define AM_HAL_PIN_86_GPIO              3
#define AM_HAL_PIN_86_RESERVED4         4
#define AM_HAL_PIN_86_RESERVED5         5
#define AM_HAL_PIN_86_CT86              6
#define AM_HAL_PIN_86_NCE86             7
#define AM_HAL_PIN_86_OBSBUS6           8
#define AM_HAL_PIN_86_RESERVED9         9
#define AM_HAL_PIN_86_RESERVED10        10
#define AM_HAL_PIN_86_FPIO              11
#define AM_HAL_PIN_86_RESERVED12        12
#define AM_HAL_PIN_86_RESERVED13        13
#define AM_HAL_PIN_86_RESERVED14        14
#define AM_HAL_PIN_86_RESERVED15        15

// PIN 87 functions
#define AM_HAL_PIN_87_RESERVED0         0
#define AM_HAL_PIN_87_RESERVED1         1
#define AM_HAL_PIN_87_SDIF0_DAT3        2
#define AM_HAL_PIN_87_GPIO              3
#define AM_HAL_PIN_87_RESERVED4         4
#define AM_HAL_PIN_87_RESERVED5         5
#define AM_HAL_PIN_87_CT87              6
#define AM_HAL_PIN_87_NCE87             7
#define AM_HAL_PIN_87_OBSBUS7           8
#define AM_HAL_PIN_87_DISP_TE           9
#define AM_HAL_PIN_87_RESERVED10        10
#define AM_HAL_PIN_87_FPIO              11
#define AM_HAL_PIN_87_RESERVED12        12
#define AM_HAL_PIN_87_RESERVED13        13
#define AM_HAL_PIN_87_RESERVED14        14
#define AM_HAL_PIN_87_RESERVED15        15

// PIN 88 functions
#define AM_HAL_PIN_88_RESERVED0         0
#define AM_HAL_PIN_88_RESERVED1         1
#define AM_HAL_PIN_88_SDIF0_CLKOUT      2
#define AM_HAL_PIN_88_GPIO              3
#define AM_HAL_PIN_88_RESERVED4         4
#define AM_HAL_PIN_88_RESERVED5         5
#define AM_HAL_PIN_88_CT88              6
#define AM_HAL_PIN_88_NCE88             7
#define AM_HAL_PIN_88_OBSBUS8           8
#define AM_HAL_PIN_88_RESERVED9         9
#define AM_HAL_PIN_88_RESERVED10        10
#define AM_HAL_PIN_88_FPIO              11
#define AM_HAL_PIN_88_RESERVED12        12
#define AM_HAL_PIN_88_RESERVED13        13
#define AM_HAL_PIN_88_RESERVED14        14
#define AM_HAL_PIN_88_RESERVED15        15

// PIN 89 functions
#define AM_HAL_PIN_89_RESERVED0         0
#define AM_HAL_PIN_89_RESERVED1         1
#define AM_HAL_PIN_89_RESERVED2         2
#define AM_HAL_PIN_89_GPIO              3
#define AM_HAL_PIN_89_RESERVED4         4
#define AM_HAL_PIN_89_RESERVED5         5
#define AM_HAL_PIN_89_CT89              6
#define AM_HAL_PIN_89_NCE89             7
#define AM_HAL_PIN_89_OBSBUS9           8
#define AM_HAL_PIN_89_DISP_SPI_RST      9
#define AM_HAL_PIN_89_RESERVED10        10
#define AM_HAL_PIN_89_FPIO              11
#define AM_HAL_PIN_89_RESERVED12        12
#define AM_HAL_PIN_89_RESERVED13        13
#define AM_HAL_PIN_89_RESERVED14        14
#define AM_HAL_PIN_89_RESERVED15        15

// PIN 90 functions
#define AM_HAL_PIN_90_RESERVED0         0
#define AM_HAL_PIN_90_RESERVED1         1
#define AM_HAL_PIN_90_RESERVED2         2
#define AM_HAL_PIN_90_GPIO              3
#define AM_HAL_PIN_90_RESERVED4         4
#define AM_HAL_PIN_90_RESERVED5         5
#define AM_HAL_PIN_90_CT90              6
#define AM_HAL_PIN_90_NCE90             7
#define AM_HAL_PIN_90_OBSBUS10          8
#define AM_HAL_PIN_90_VCMPO             9
#define AM_HAL_PIN_90_RESERVED10        10
#define AM_HAL_PIN_90_FPIO              11
#define AM_HAL_PIN_90_RESERVED12        12
#define AM_HAL_PIN_90_RESERVED13        13
#define AM_HAL_PIN_90_RESERVED14        14
#define AM_HAL_PIN_90_RESERVED15        15

// PIN 91 functions
#define AM_HAL_PIN_91_RESERVED0         0
#define AM_HAL_PIN_91_RESERVED1         1
#define AM_HAL_PIN_91_RESERVED2         2
#define AM_HAL_PIN_91_GPIO              3
#define AM_HAL_PIN_91_RESERVED4         4
#define AM_HAL_PIN_91_RESERVED5         5
#define AM_HAL_PIN_91_CT91              6
#define AM_HAL_PIN_91_NCE91             7
#define AM_HAL_PIN_91_OBSBUS11          8
#define AM_HAL_PIN_91_VCMPO             9
#define AM_HAL_PIN_91_RESERVED10        10
#define AM_HAL_PIN_91_FPIO              11
#define AM_HAL_PIN_91_RESERVED12        12
#define AM_HAL_PIN_91_RESERVED13        13
#define AM_HAL_PIN_91_RESERVED14        14
#define AM_HAL_PIN_91_RESERVED15        15

// PIN 92 functions
#define AM_HAL_PIN_92_RESERVED0         0
#define AM_HAL_PIN_92_RESERVED1         1
#define AM_HAL_PIN_92_RESERVED2         2
#define AM_HAL_PIN_92_GPIO              3
#define AM_HAL_PIN_92_RESERVED4         4
#define AM_HAL_PIN_92_RESERVED5         5
#define AM_HAL_PIN_92_CT92              6
#define AM_HAL_PIN_92_NCE92             7
#define AM_HAL_PIN_92_OBSBUS12          8
#define AM_HAL_PIN_92_VCMPO             9
#define AM_HAL_PIN_92_RESERVED10        10
#define AM_HAL_PIN_92_FPIO              11
#define AM_HAL_PIN_92_RESERVED12        12
#define AM_HAL_PIN_92_RESERVED13        13
#define AM_HAL_PIN_92_RESERVED14        14
#define AM_HAL_PIN_92_RESERVED15        15

// PIN 93 functions
#define AM_HAL_PIN_93_MNCE1_0           0
#define AM_HAL_PIN_93_RESERVED1         1
#define AM_HAL_PIN_93_RESERVED2         2
#define AM_HAL_PIN_93_GPIO              3
#define AM_HAL_PIN_93_RESERVED4         4
#define AM_HAL_PIN_93_RESERVED5         5
#define AM_HAL_PIN_93_CT93              6
#define AM_HAL_PIN_93_NCE93             7
#define AM_HAL_PIN_93_OBSBUS13          8
#define AM_HAL_PIN_93_VCMPO             9
#define AM_HAL_PIN_93_RESERVED10        10
#define AM_HAL_PIN_93_FPIO              11
#define AM_HAL_PIN_93_RESERVED12        12
#define AM_HAL_PIN_93_RESERVED13        13
#define AM_HAL_PIN_93_RESERVED14        14
#define AM_HAL_PIN_93_RESERVED15        15

// PIN 94 functions
#define AM_HAL_PIN_94_RESERVED0         0
#define AM_HAL_PIN_94_RESERVED1         1
#define AM_HAL_PIN_94_RESERVED2         2
#define AM_HAL_PIN_94_GPIO              3
#define AM_HAL_PIN_94_RESERVED4         4
#define AM_HAL_PIN_94_RESERVED5         5
#define AM_HAL_PIN_94_CT94              6
#define AM_HAL_PIN_94_NCE94             7
#define AM_HAL_PIN_94_OBSBUS14          8
#define AM_HAL_PIN_94_VCMPO             9
#define AM_HAL_PIN_94_RESERVED10        10
#define AM_HAL_PIN_94_FPIO              11
#define AM_HAL_PIN_94_RESERVED12        12
#define AM_HAL_PIN_94_RESERVED13        13
#define AM_HAL_PIN_94_RESERVED14        14
#define AM_HAL_PIN_94_RESERVED15        15

// PIN 95 functions
#define AM_HAL_PIN_95_MSPI1_0           0
#define AM_HAL_PIN_95_RESERVED1         1
#define AM_HAL_PIN_95_RESERVED2         2
#define AM_HAL_PIN_95_GPIO              3
#define AM_HAL_PIN_95_RESERVED4         4
#define AM_HAL_PIN_95_RESERVED5         5
#define AM_HAL_PIN_95_CT95              6
#define AM_HAL_PIN_95_NCE95             7
#define AM_HAL_PIN_95_OBSBUS15          8
#define AM_HAL_PIN_95_I2S0_SDIN         9
#define AM_HAL_PIN_95_RESERVED10        10
#define AM_HAL_PIN_95_FPIO              11
#define AM_HAL_PIN_95_RESERVED12        12
#define AM_HAL_PIN_95_RESERVED13        13
#define AM_HAL_PIN_95_RESERVED14        14
#define AM_HAL_PIN_95_RESERVED15        15

// PIN 96 functions
#define AM_HAL_PIN_96_MSPI1_1           0
#define AM_HAL_PIN_96_RESERVED1         1
#define AM_HAL_PIN_96_RESERVED2         2
#define AM_HAL_PIN_96_GPIO              3
#define AM_HAL_PIN_96_RESERVED4         4
#define AM_HAL_PIN_96_RESERVED5         5
#define AM_HAL_PIN_96_CT96              6
#define AM_HAL_PIN_96_NCE96             7
#define AM_HAL_PIN_96_OBSBUS0           8
#define AM_HAL_PIN_96_RESERVED9         9
#define AM_HAL_PIN_96_RESERVED10        10
#define AM_HAL_PIN_96_FPIO              11
#define AM_HAL_PIN_96_RESERVED12        12
#define AM_HAL_PIN_96_RESERVED13        13
#define AM_HAL_PIN_96_RESERVED14        14
#define AM_HAL_PIN_96_RESERVED15        15

// PIN 97 functions
#define AM_HAL_PIN_97_MSPI1_2           0
#define AM_HAL_PIN_97_RESERVED1         1
#define AM_HAL_PIN_97_RESERVED2         2
#define AM_HAL_PIN_97_GPIO              3
#define AM_HAL_PIN_97_RESERVED4         4
#define AM_HAL_PIN_97_RESERVED5         5
#define AM_HAL_PIN_97_CT97              6
#define AM_HAL_PIN_97_NCE97             7
#define AM_HAL_PIN_97_OBSBUS1           8
#define AM_HAL_PIN_97_RESERVED9         9
#define AM_HAL_PIN_97_RESERVED10        10
#define AM_HAL_PIN_97_FPIO              11
#define AM_HAL_PIN_97_RESERVED12        12
#define AM_HAL_PIN_97_RESERVED13        13
#define AM_HAL_PIN_97_RESERVED14        14
#define AM_HAL_PIN_97_RESERVED15        15

// PIN 98 functions
#define AM_HAL_PIN_98_MSPI1_3           0
#define AM_HAL_PIN_98_RESERVED1         1
#define AM_HAL_PIN_98_RESERVED2         2
#define AM_HAL_PIN_98_GPIO              3
#define AM_HAL_PIN_98_RESERVED4         4
#define AM_HAL_PIN_98_RESERVED5         5
#define AM_HAL_PIN_98_CT98              6
#define AM_HAL_PIN_98_NCE98             7
#define AM_HAL_PIN_98_OBSBUS2           8
#define AM_HAL_PIN_98_RESERVED9         9
#define AM_HAL_PIN_98_RESERVED10        10
#define AM_HAL_PIN_98_FPIO              11
#define AM_HAL_PIN_98_RESERVED12        12
#define AM_HAL_PIN_98_RESERVED13        13
#define AM_HAL_PIN_98_RESERVED14        14
#define AM_HAL_PIN_98_RESERVED15        15

// PIN 99 functions
#define AM_HAL_PIN_99_MSPI1_4           0
#define AM_HAL_PIN_99_RESERVED1         1
#define AM_HAL_PIN_99_RESERVED2         2
#define AM_HAL_PIN_99_GPIO              3
#define AM_HAL_PIN_99_RESERVED4         4
#define AM_HAL_PIN_99_RESERVED5         5
#define AM_HAL_PIN_99_CT99              6
#define AM_HAL_PIN_99_NCE99             7
#define AM_HAL_PIN_99_OBSBUS3           8
#define AM_HAL_PIN_99_RESERVED9         9
#define AM_HAL_PIN_99_RESERVED10        10
#define AM_HAL_PIN_99_FPIO              11
#define AM_HAL_PIN_99_RESERVED12        12
#define AM_HAL_PIN_99_RESERVED13        13
#define AM_HAL_PIN_99_RESERVED14        14
#define AM_HAL_PIN_99_RESERVED15        15

// PIN 100 functions
#define AM_HAL_PIN_100_MSPI1_5          0
#define AM_HAL_PIN_100_DISP_QSPI_D0_OUT  1
#define AM_HAL_PIN_100_DISP_QSPI_D0     2
#define AM_HAL_PIN_100_GPIO             3
#define AM_HAL_PIN_100_DISP_SPI_SD      4
#define AM_HAL_PIN_100_DISP_SPI_SDO     5
#define AM_HAL_PIN_100_CT100            6
#define AM_HAL_PIN_100_NCE100           7
#define AM_HAL_PIN_100_OBSBUS4          8
#define AM_HAL_PIN_100_I2S0_CLK         9
#define AM_HAL_PIN_100_RESERVED10       10
#define AM_HAL_PIN_100_FPIO             11
#define AM_HAL_PIN_100_RESERVED12       12
#define AM_HAL_PIN_100_RESERVED13       13
#define AM_HAL_PIN_100_RESERVED14       14
#define AM_HAL_PIN_100_RESERVED15       15

// PIN 101 functions
#define AM_HAL_PIN_101_MSPI1_6          0
#define AM_HAL_PIN_101_RESERVED1        1
#define AM_HAL_PIN_101_DISP_QSPI_D1     2
#define AM_HAL_PIN_101_GPIO             3
#define AM_HAL_PIN_101_DISP_SPI_DCX     4
#define AM_HAL_PIN_101_RESERVED5        5
#define AM_HAL_PIN_101_CT101            6
#define AM_HAL_PIN_101_NCE101           7
#define AM_HAL_PIN_101_OBSBUS5          8
#define AM_HAL_PIN_101_I2S0_DATA        9
#define AM_HAL_PIN_101_I2S0_SDOUT       10
#define AM_HAL_PIN_101_FPIO             11
#define AM_HAL_PIN_101_RESERVED12       12
#define AM_HAL_PIN_101_RESERVED13       13
#define AM_HAL_PIN_101_RESERVED14       14
#define AM_HAL_PIN_101_RESERVED15       15

// PIN 102 functions
#define AM_HAL_PIN_102_MSPI1_7          0
#define AM_HAL_PIN_102_RESERVED1        1
#define AM_HAL_PIN_102_DISP_QSPI_SCK    2
#define AM_HAL_PIN_102_GPIO             3
#define AM_HAL_PIN_102_DISP_SPI_SCK     4
#define AM_HAL_PIN_102_RESERVED5        5
#define AM_HAL_PIN_102_CT102            6
#define AM_HAL_PIN_102_NCE102           7
#define AM_HAL_PIN_102_OBSBUS6          8
#define AM_HAL_PIN_102_I2S0_WS          9
#define AM_HAL_PIN_102_RESERVED10       10
#define AM_HAL_PIN_102_FPIO             11
#define AM_HAL_PIN_102_RESERVED12       12
#define AM_HAL_PIN_102_RESERVED13       13
#define AM_HAL_PIN_102_RESERVED14       14
#define AM_HAL_PIN_102_RESERVED15       15

// PIN 103 functions
#define AM_HAL_PIN_103_MSPI1_8          0
#define AM_HAL_PIN_103_RESERVED1        1
#define AM_HAL_PIN_103_DISP_QSPI_D2     2
#define AM_HAL_PIN_103_GPIO             3
#define AM_HAL_PIN_103_DISP_SPI_SDI     4
#define AM_HAL_PIN_103_RESERVED5        5
#define AM_HAL_PIN_103_CT103            6
#define AM_HAL_PIN_103_NCE103           7
#define AM_HAL_PIN_103_OBSBUS7          8
#define AM_HAL_PIN_103_RESERVED9        9
#define AM_HAL_PIN_103_RESERVED10       10
#define AM_HAL_PIN_103_FPIO             11
#define AM_HAL_PIN_103_RESERVED12       12
#define AM_HAL_PIN_103_RESERVED13       13
#define AM_HAL_PIN_103_RESERVED14       14
#define AM_HAL_PIN_103_RESERVED15       15

// PIN 104 functions
#define AM_HAL_PIN_104_MSPI1_9          0
#define AM_HAL_PIN_104_RESERVED1        1
#define AM_HAL_PIN_104_DISP_QSPI_D3     2
#define AM_HAL_PIN_104_GPIO             3
#define AM_HAL_PIN_104_DISP_SPI_RST     4
#define AM_HAL_PIN_104_RESERVED5        5
#define AM_HAL_PIN_104_CT104            6
#define AM_HAL_PIN_104_NCE104           7
#define AM_HAL_PIN_104_OBSBUS8          8
#define AM_HAL_PIN_104_RESERVED9        9
#define AM_HAL_PIN_104_RESERVED10       10
#define AM_HAL_PIN_104_FPIO             11
#define AM_HAL_PIN_104_RESERVED12       12
#define AM_HAL_PIN_104_RESERVED13       13
#define AM_HAL_PIN_104_RESERVED14       14
#define AM_HAL_PIN_104_RESERVED15       15

// PIN 105 functions
#define AM_HAL_PIN_105_MSPI3_10         0
#define AM_HAL_PIN_105_RESERVED1        1
#define AM_HAL_PIN_105_RESERVED2        2
#define AM_HAL_PIN_105_GPIO             3
#define AM_HAL_PIN_105_RESERVED4        4
#define AM_HAL_PIN_105_RESERVED5        5
#define AM_HAL_PIN_105_CT105            6
#define AM_HAL_PIN_105_NCE105           7
#define AM_HAL_PIN_105_RESERVED8        8
#define AM_HAL_PIN_105_RESERVED9        9
#define AM_HAL_PIN_105_RESERVED10       10
#define AM_HAL_PIN_105_FPIO             11
#define AM_HAL_PIN_105_RESERVED12       12
#define AM_HAL_PIN_105_RESERVED13       13
#define AM_HAL_PIN_105_RESERVED14       14
#define AM_HAL_PIN_105_RESERVED15       15

// PIN 106 functions
#define AM_HAL_PIN_106_MSPI3_11         0
#define AM_HAL_PIN_106_RESERVED1        1
#define AM_HAL_PIN_106_RESERVED2        2
#define AM_HAL_PIN_106_GPIO             3
#define AM_HAL_PIN_106_RESERVED4        4
#define AM_HAL_PIN_106_RESERVED5        5
#define AM_HAL_PIN_106_CT106            6
#define AM_HAL_PIN_106_NCE106           7
#define AM_HAL_PIN_106_RESERVED8        8
#define AM_HAL_PIN_106_RESERVED9        9
#define AM_HAL_PIN_106_RESERVED10       10
#define AM_HAL_PIN_106_FPIO             11
#define AM_HAL_PIN_106_RESERVED12       12
#define AM_HAL_PIN_106_RESERVED13       13
#define AM_HAL_PIN_106_RESERVED14       14
#define AM_HAL_PIN_106_RESERVED15       15

// PIN 107 functions
#define AM_HAL_PIN_107_MSPI3_12         0
#define AM_HAL_PIN_107_RESERVED1        1
#define AM_HAL_PIN_107_RESERVED2        2
#define AM_HAL_PIN_107_GPIO             3
#define AM_HAL_PIN_107_RESERVED4        4
#define AM_HAL_PIN_107_RESERVED5        5
#define AM_HAL_PIN_107_CT107            6
#define AM_HAL_PIN_107_NCE107           7
#define AM_HAL_PIN_107_RESERVED8        8
#define AM_HAL_PIN_107_RESERVED9        9
#define AM_HAL_PIN_107_RESERVED10       10
#define AM_HAL_PIN_107_FPIO             11
#define AM_HAL_PIN_107_RESERVED12       12
#define AM_HAL_PIN_107_RESERVED13       13
#define AM_HAL_PIN_107_RESERVED14       14
#define AM_HAL_PIN_107_RESERVED15       15

// PIN 108 functions
#define AM_HAL_PIN_108_MSPI3_13         0
#define AM_HAL_PIN_108_RESERVED1        1
#define AM_HAL_PIN_108_RESERVED2        2
#define AM_HAL_PIN_108_GPIO             3
#define AM_HAL_PIN_108_RESERVED4        4
#define AM_HAL_PIN_108_RESERVED5        5
#define AM_HAL_PIN_108_CT108            6
#define AM_HAL_PIN_108_NCE108           7
#define AM_HAL_PIN_108_RESERVED8        8
#define AM_HAL_PIN_108_RESERVED9        9
#define AM_HAL_PIN_108_RESERVED10       10
#define AM_HAL_PIN_108_FPIO             11
#define AM_HAL_PIN_108_RESERVED12       12
#define AM_HAL_PIN_108_RESERVED13       13
#define AM_HAL_PIN_108_RESERVED14       14
#define AM_HAL_PIN_108_RESERVED15       15

// PIN 109 functions
#define AM_HAL_PIN_109_MSPI3_14         0
#define AM_HAL_PIN_109_RESERVED1        1
#define AM_HAL_PIN_109_RESERVED2        2
#define AM_HAL_PIN_109_GPIO             3
#define AM_HAL_PIN_109_RESERVED4        4
#define AM_HAL_PIN_109_RESERVED5        5
#define AM_HAL_PIN_109_CT109            6
#define AM_HAL_PIN_109_NCE109           7
#define AM_HAL_PIN_109_RESERVED8        8
#define AM_HAL_PIN_109_RESERVED9        9
#define AM_HAL_PIN_109_RESERVED10       10
#define AM_HAL_PIN_109_FPIO             11
#define AM_HAL_PIN_109_RESERVED12       12
#define AM_HAL_PIN_109_RESERVED13       13
#define AM_HAL_PIN_109_RESERVED14       14
#define AM_HAL_PIN_109_RESERVED15       15

// PIN 110 functions
#define AM_HAL_PIN_110_MSPI3_15         0
#define AM_HAL_PIN_110_RESERVED1        1
#define AM_HAL_PIN_110_RESERVED2        2
#define AM_HAL_PIN_110_GPIO             3
#define AM_HAL_PIN_110_RESERVED4        4
#define AM_HAL_PIN_110_RESERVED5        5
#define AM_HAL_PIN_110_CT110            6
#define AM_HAL_PIN_110_NCE110           7
#define AM_HAL_PIN_110_RESERVED8        8
#define AM_HAL_PIN_110_RESERVED9        9
#define AM_HAL_PIN_110_RESERVED10       10
#define AM_HAL_PIN_110_FPIO             11
#define AM_HAL_PIN_110_RESERVED12       12
#define AM_HAL_PIN_110_RESERVED13       13
#define AM_HAL_PIN_110_RESERVED14       14
#define AM_HAL_PIN_110_RESERVED15       15

// PIN 111 functions
#define AM_HAL_PIN_111_MSPI3_16         0
#define AM_HAL_PIN_111_RESERVED1        1
#define AM_HAL_PIN_111_RESERVED2        2
#define AM_HAL_PIN_111_GPIO             3
#define AM_HAL_PIN_111_RESERVED4        4
#define AM_HAL_PIN_111_RESERVED5        5
#define AM_HAL_PIN_111_CT111            6
#define AM_HAL_PIN_111_NCE111           7
#define AM_HAL_PIN_111_RESERVED8        8
#define AM_HAL_PIN_111_RESERVED9        9
#define AM_HAL_PIN_111_RESERVED10       10
#define AM_HAL_PIN_111_FPIO             11
#define AM_HAL_PIN_111_RESERVED12       12
#define AM_HAL_PIN_111_RESERVED13       13
#define AM_HAL_PIN_111_RESERVED14       14
#define AM_HAL_PIN_111_RESERVED15       15

// PIN 112 functions
#define AM_HAL_PIN_112_MSPI3_17         0
#define AM_HAL_PIN_112_RESERVED1        1
#define AM_HAL_PIN_112_RESERVED2        2
#define AM_HAL_PIN_112_GPIO             3
#define AM_HAL_PIN_112_RESERVED4        4
#define AM_HAL_PIN_112_RESERVED5        5
#define AM_HAL_PIN_112_CT112            6
#define AM_HAL_PIN_112_NCE112           7
#define AM_HAL_PIN_112_RESERVED8        8
#define AM_HAL_PIN_112_RESERVED9        9
#define AM_HAL_PIN_112_RESERVED10       10
#define AM_HAL_PIN_112_FPIO             11
#define AM_HAL_PIN_112_RESERVED12       12
#define AM_HAL_PIN_112_RESERVED13       13
#define AM_HAL_PIN_112_RESERVED14       14
#define AM_HAL_PIN_112_RESERVED15       15

// PIN 113 functions
#define AM_HAL_PIN_113_MSPI3_18         0
#define AM_HAL_PIN_113_RESERVED1        1
#define AM_HAL_PIN_113_RESERVED2        2
#define AM_HAL_PIN_113_GPIO             3
#define AM_HAL_PIN_113_RESERVED4        4
#define AM_HAL_PIN_113_RESERVED5        5
#define AM_HAL_PIN_113_CT113            6
#define AM_HAL_PIN_113_NCE113           7
#define AM_HAL_PIN_113_RESERVED8        8
#define AM_HAL_PIN_113_RESERVED9        9
#define AM_HAL_PIN_113_RESERVED10       10
#define AM_HAL_PIN_113_FPIO             11
#define AM_HAL_PIN_113_RESERVED12       12
#define AM_HAL_PIN_113_RESERVED13       13
#define AM_HAL_PIN_113_RESERVED14       14
#define AM_HAL_PIN_113_RESERVED15       15

// PIN 114 functions
#define AM_HAL_PIN_114_RESERVED0        0
#define AM_HAL_PIN_114_MNCE3_0          1
#define AM_HAL_PIN_114_RESERVED2        2
#define AM_HAL_PIN_114_GPIO             3
#define AM_HAL_PIN_114_RESERVED4        4
#define AM_HAL_PIN_114_RESERVED5        5
#define AM_HAL_PIN_114_CT114            6
#define AM_HAL_PIN_114_NCE114           7
#define AM_HAL_PIN_114_RESERVED8        8
#define AM_HAL_PIN_114_RESERVED9        9
#define AM_HAL_PIN_114_RESERVED10       10
#define AM_HAL_PIN_114_FPIO             11
#define AM_HAL_PIN_114_RESERVED12       12
#define AM_HAL_PIN_114_RESERVED13       13
#define AM_HAL_PIN_114_RESERVED14       14
#define AM_HAL_PIN_114_RESERVED15       15

// PIN 115 functions
#define AM_HAL_PIN_115_MSPI3_0          0
#define AM_HAL_PIN_115_RESERVED1        1
#define AM_HAL_PIN_115_RESERVED2        2
#define AM_HAL_PIN_115_GPIO             3
#define AM_HAL_PIN_115_RESERVED4        4
#define AM_HAL_PIN_115_RESERVED5        5
#define AM_HAL_PIN_115_CT115            6
#define AM_HAL_PIN_115_NCE115           7
#define AM_HAL_PIN_115_RESERVED8        8
#define AM_HAL_PIN_115_RESERVED9        9
#define AM_HAL_PIN_115_RESERVED10       10
#define AM_HAL_PIN_115_FPIO             11
#define AM_HAL_PIN_115_RESERVED12       12
#define AM_HAL_PIN_115_RESERVED13       13
#define AM_HAL_PIN_115_RESERVED14       14
#define AM_HAL_PIN_115_RESERVED15       15

// PIN 116 functions
#define AM_HAL_PIN_116_MSPI3_1          0
#define AM_HAL_PIN_116_RESERVED1        1
#define AM_HAL_PIN_116_RESERVED2        2
#define AM_HAL_PIN_116_GPIO             3
#define AM_HAL_PIN_116_RESERVED4        4
#define AM_HAL_PIN_116_RESERVED5        5
#define AM_HAL_PIN_116_CT116            6
#define AM_HAL_PIN_116_NCE116           7
#define AM_HAL_PIN_116_RESERVED8        8
#define AM_HAL_PIN_116_RESERVED9        9
#define AM_HAL_PIN_116_RESERVED10       10
#define AM_HAL_PIN_116_FPIO             11
#define AM_HAL_PIN_116_RESERVED12       12
#define AM_HAL_PIN_116_RESERVED13       13
#define AM_HAL_PIN_116_RESERVED14       14
#define AM_HAL_PIN_116_RESERVED15       15

// PIN 117 functions
#define AM_HAL_PIN_117_MSPI3_2          0
#define AM_HAL_PIN_117_RESERVED1        1
#define AM_HAL_PIN_117_RESERVED2        2
#define AM_HAL_PIN_117_GPIO             3
#define AM_HAL_PIN_117_RESERVED4        4
#define AM_HAL_PIN_117_RESERVED5        5
#define AM_HAL_PIN_117_CT117            6
#define AM_HAL_PIN_117_NCE117           7
#define AM_HAL_PIN_117_RESERVED8        8
#define AM_HAL_PIN_117_RESERVED9        9
#define AM_HAL_PIN_117_RESERVED10       10
#define AM_HAL_PIN_117_FPIO             11
#define AM_HAL_PIN_117_RESERVED12       12
#define AM_HAL_PIN_117_RESERVED13       13
#define AM_HAL_PIN_117_RESERVED14       14
#define AM_HAL_PIN_117_RESERVED15       15

// PIN 118 functions
#define AM_HAL_PIN_118_MSPI3_3          0
#define AM_HAL_PIN_118_RESERVED1        1
#define AM_HAL_PIN_118_RESERVED2        2
#define AM_HAL_PIN_118_GPIO             3
#define AM_HAL_PIN_118_RESERVED4        4
#define AM_HAL_PIN_118_RESERVED5        5
#define AM_HAL_PIN_118_CT118            6
#define AM_HAL_PIN_118_NCE118           7
#define AM_HAL_PIN_118_RESERVED8        8
#define AM_HAL_PIN_118_RESERVED9        9
#define AM_HAL_PIN_118_RESERVED10       10
#define AM_HAL_PIN_118_FPIO             11
#define AM_HAL_PIN_118_RESERVED12       12
#define AM_HAL_PIN_118_RESERVED13       13
#define AM_HAL_PIN_118_RESERVED14       14
#define AM_HAL_PIN_118_RESERVED15       15

// PIN 119 functions
#define AM_HAL_PIN_119_MSPI3_4          0
#define AM_HAL_PIN_119_RESERVED1        1
#define AM_HAL_PIN_119_RESERVED2        2
#define AM_HAL_PIN_119_GPIO             3
#define AM_HAL_PIN_119_RESERVED4        4
#define AM_HAL_PIN_119_RESERVED5        5
#define AM_HAL_PIN_119_CT119            6
#define AM_HAL_PIN_119_NCE119           7
#define AM_HAL_PIN_119_RESERVED8        8
#define AM_HAL_PIN_119_RESERVED9        9
#define AM_HAL_PIN_119_RESERVED10       10
#define AM_HAL_PIN_119_FPIO             11
#define AM_HAL_PIN_119_RESERVED12       12
#define AM_HAL_PIN_119_RESERVED13       13
#define AM_HAL_PIN_119_RESERVED14       14
#define AM_HAL_PIN_119_RESERVED15       15

// PIN 120 functions
#define AM_HAL_PIN_120_MSPI3_5          0
#define AM_HAL_PIN_120_RESERVED1        1
#define AM_HAL_PIN_120_RESERVED2        2
#define AM_HAL_PIN_120_GPIO             3
#define AM_HAL_PIN_120_RESERVED4        4
#define AM_HAL_PIN_120_RESERVED5        5
#define AM_HAL_PIN_120_CT120            6
#define AM_HAL_PIN_120_NCE120           7
#define AM_HAL_PIN_120_RESERVED8        8
#define AM_HAL_PIN_120_RESERVED9        9
#define AM_HAL_PIN_120_RESERVED10       10
#define AM_HAL_PIN_120_FPIO             11
#define AM_HAL_PIN_120_RESERVED12       12
#define AM_HAL_PIN_120_RESERVED13       13
#define AM_HAL_PIN_120_RESERVED14       14
#define AM_HAL_PIN_120_RESERVED15       15

// PIN 121 functions
#define AM_HAL_PIN_121_MSPI3_6          0
#define AM_HAL_PIN_121_RESERVED1        1
#define AM_HAL_PIN_121_RESERVED2        2
#define AM_HAL_PIN_121_GPIO             3
#define AM_HAL_PIN_121_RESERVED4        4
#define AM_HAL_PIN_121_RESERVED5        5
#define AM_HAL_PIN_121_CT121            6
#define AM_HAL_PIN_121_NCE121           7
#define AM_HAL_PIN_121_RESERVED8        8
#define AM_HAL_PIN_121_RESERVED9        9
#define AM_HAL_PIN_121_RESERVED10       10
#define AM_HAL_PIN_121_FPIO             11
#define AM_HAL_PIN_121_RESERVED12       12
#define AM_HAL_PIN_121_RESERVED13       13
#define AM_HAL_PIN_121_RESERVED14       14
#define AM_HAL_PIN_121_RESERVED15       15

// PIN 122 functions
#define AM_HAL_PIN_122_MSPI3_7          0
#define AM_HAL_PIN_122_RESERVED1        1
#define AM_HAL_PIN_122_RESERVED2        2
#define AM_HAL_PIN_122_GPIO             3
#define AM_HAL_PIN_122_RESERVED4        4
#define AM_HAL_PIN_122_RESERVED5        5
#define AM_HAL_PIN_122_CT122            6
#define AM_HAL_PIN_122_NCE122           7
#define AM_HAL_PIN_122_RESERVED8        8
#define AM_HAL_PIN_122_RESERVED9        9
#define AM_HAL_PIN_122_RESERVED10       10
#define AM_HAL_PIN_122_FPIO             11
#define AM_HAL_PIN_122_RESERVED12       12
#define AM_HAL_PIN_122_RESERVED13       13
#define AM_HAL_PIN_122_RESERVED14       14
#define AM_HAL_PIN_122_RESERVED15       15

// PIN 123 functions
#define AM_HAL_PIN_123_MSPI3_8          0
#define AM_HAL_PIN_123_RESERVED1        1
#define AM_HAL_PIN_123_RESERVED2        2
#define AM_HAL_PIN_123_GPIO             3
#define AM_HAL_PIN_123_RESERVED4        4
#define AM_HAL_PIN_123_RESERVED5        5
#define AM_HAL_PIN_123_CT123            6
#define AM_HAL_PIN_123_NCE123           7
#define AM_HAL_PIN_123_RESERVED8        8
#define AM_HAL_PIN_123_RESERVED9        9
#define AM_HAL_PIN_123_RESERVED10       10
#define AM_HAL_PIN_123_FPIO             11
#define AM_HAL_PIN_123_RESERVED12       12
#define AM_HAL_PIN_123_RESERVED13       13
#define AM_HAL_PIN_123_RESERVED14       14
#define AM_HAL_PIN_123_RESERVED15       15

// PIN 124 functions
#define AM_HAL_PIN_124_MSPI3_9          0
#define AM_HAL_PIN_124_RESERVED1        1
#define AM_HAL_PIN_124_RESERVED2        2
#define AM_HAL_PIN_124_GPIO             3
#define AM_HAL_PIN_124_RESERVED4        4
#define AM_HAL_PIN_124_RESERVED5        5
#define AM_HAL_PIN_124_CT124            6
#define AM_HAL_PIN_124_NCE124           7
#define AM_HAL_PIN_124_RESERVED8        8
#define AM_HAL_PIN_124_RESERVED9        9
#define AM_HAL_PIN_124_RESERVED10       10
#define AM_HAL_PIN_124_FPIO             11
#define AM_HAL_PIN_124_RESERVED12       12
#define AM_HAL_PIN_124_RESERVED13       13
#define AM_HAL_PIN_124_RESERVED14       14
#define AM_HAL_PIN_124_RESERVED15       15

// PIN 125 functions
#define AM_HAL_PIN_125_SDIF1_DAT0       0
#define AM_HAL_PIN_125_RESERVED1        1
#define AM_HAL_PIN_125_RESERVED2        2
#define AM_HAL_PIN_125_GPIO             3
#define AM_HAL_PIN_125_RESERVED4        4
#define AM_HAL_PIN_125_RESERVED5        5
#define AM_HAL_PIN_125_CT125            6
#define AM_HAL_PIN_125_NCE125           7
#define AM_HAL_PIN_125_RESERVED8        8
#define AM_HAL_PIN_125_RESERVED9        9
#define AM_HAL_PIN_125_RESERVED10       10
#define AM_HAL_PIN_125_FPIO             11
#define AM_HAL_PIN_125_RESERVED12       12
#define AM_HAL_PIN_125_RESERVED13       13
#define AM_HAL_PIN_125_RESERVED14       14
#define AM_HAL_PIN_125_RESERVED15       15

// PIN 126 functions
#define AM_HAL_PIN_126_SDIF1_DAT1       0
#define AM_HAL_PIN_126_RESERVED1        1
#define AM_HAL_PIN_126_RESERVED2        2
#define AM_HAL_PIN_126_GPIO             3
#define AM_HAL_PIN_126_RESERVED4        4
#define AM_HAL_PIN_126_RESERVED5        5
#define AM_HAL_PIN_126_CT126            6
#define AM_HAL_PIN_126_NCE126           7
#define AM_HAL_PIN_126_RESERVED8        8
#define AM_HAL_PIN_126_RESERVED9        9
#define AM_HAL_PIN_126_RESERVED10       10
#define AM_HAL_PIN_126_FPIO             11
#define AM_HAL_PIN_126_RESERVED12       12
#define AM_HAL_PIN_126_RESERVED13       13
#define AM_HAL_PIN_126_RESERVED14       14
#define AM_HAL_PIN_126_RESERVED15       15

// PIN 127 functions
#define AM_HAL_PIN_127_SDIF1_DAT2       0
#define AM_HAL_PIN_127_RESERVED1        1
#define AM_HAL_PIN_127_RESERVED2        2
#define AM_HAL_PIN_127_GPIO             3
#define AM_HAL_PIN_127_RESERVED4        4
#define AM_HAL_PIN_127_RESERVED5        5
#define AM_HAL_PIN_127_CT127            6
#define AM_HAL_PIN_127_NCE127           7
#define AM_HAL_PIN_127_RESERVED8        8
#define AM_HAL_PIN_127_RESERVED9        9
#define AM_HAL_PIN_127_RESERVED10       10
#define AM_HAL_PIN_127_FPIO             11
#define AM_HAL_PIN_127_RESERVED12       12
#define AM_HAL_PIN_127_RESERVED13       13
#define AM_HAL_PIN_127_RESERVED14       14
#define AM_HAL_PIN_127_RESERVED15       15

// PIN 128 functions
#define AM_HAL_PIN_128_SDIF1_DAT3       0
#define AM_HAL_PIN_128_RESERVED1        1
#define AM_HAL_PIN_128_RESERVED2        2
#define AM_HAL_PIN_128_GPIO             3
#define AM_HAL_PIN_128_RESERVED4        4
#define AM_HAL_PIN_128_RESERVED5        5
#define AM_HAL_PIN_128_CT128            6
#define AM_HAL_PIN_128_NCE128           7
#define AM_HAL_PIN_128_RESERVED8        8
#define AM_HAL_PIN_128_RESERVED9        9
#define AM_HAL_PIN_128_RESERVED10       10
#define AM_HAL_PIN_128_FPIO             11
#define AM_HAL_PIN_128_RESERVED12       12
#define AM_HAL_PIN_128_RESERVED13       13
#define AM_HAL_PIN_128_RESERVED14       14
#define AM_HAL_PIN_128_RESERVED15       15

// PIN 129 functions
#define AM_HAL_PIN_129_SDIF1_CLKOUT     0
#define AM_HAL_PIN_129_RESERVED1        1
#define AM_HAL_PIN_129_RESERVED2        2
#define AM_HAL_PIN_129_GPIO             3
#define AM_HAL_PIN_129_RESERVED4        4
#define AM_HAL_PIN_129_RESERVED5        5
#define AM_HAL_PIN_129_CT129            6
#define AM_HAL_PIN_129_NCE129           7
#define AM_HAL_PIN_129_RESERVED8        8
#define AM_HAL_PIN_129_RESERVED9        9
#define AM_HAL_PIN_129_RESERVED10       10
#define AM_HAL_PIN_129_FPIO             11
#define AM_HAL_PIN_129_RESERVED12       12
#define AM_HAL_PIN_129_RESERVED13       13
#define AM_HAL_PIN_129_RESERVED14       14
#define AM_HAL_PIN_129_RESERVED15       15

// PIN 130 functions
#define AM_HAL_PIN_130_SDIF1_DAT4       0
#define AM_HAL_PIN_130_MNCE0_0          1
#define AM_HAL_PIN_130_RESERVED2        2
#define AM_HAL_PIN_130_GPIO             3
#define AM_HAL_PIN_130_RESERVED4        4
#define AM_HAL_PIN_130_RESERVED5        5
#define AM_HAL_PIN_130_CT130            6
#define AM_HAL_PIN_130_NCE130           7
#define AM_HAL_PIN_130_RESERVED8        8
#define AM_HAL_PIN_130_RESERVED9        9
#define AM_HAL_PIN_130_I2S1_CLK         10
#define AM_HAL_PIN_130_FPIO             11
#define AM_HAL_PIN_130_RESERVED12       12
#define AM_HAL_PIN_130_RESERVED13       13
#define AM_HAL_PIN_130_RESERVED14       14
#define AM_HAL_PIN_130_RESERVED15       15

// PIN 131 functions
#define AM_HAL_PIN_131_SDIF1_DAT5       0
#define AM_HAL_PIN_131_MNCE3_0          1
#define AM_HAL_PIN_131_RESERVED2        2
#define AM_HAL_PIN_131_GPIO             3
#define AM_HAL_PIN_131_RESERVED4        4
#define AM_HAL_PIN_131_RESERVED5        5
#define AM_HAL_PIN_131_CT131            6
#define AM_HAL_PIN_131_NCE131           7
#define AM_HAL_PIN_131_RESERVED8        8
#define AM_HAL_PIN_131_I2S1_DATA        9
#define AM_HAL_PIN_131_I2S1_SDOUT       10
#define AM_HAL_PIN_131_FPIO             11
#define AM_HAL_PIN_131_RESERVED12       12
#define AM_HAL_PIN_131_RESERVED13       13
#define AM_HAL_PIN_131_RESERVED14       14
#define AM_HAL_PIN_131_RESERVED15       15

// PIN 132 functions
#define AM_HAL_PIN_132_SDIF1_DAT6       0
#define AM_HAL_PIN_132_RESERVED1        1
#define AM_HAL_PIN_132_RESERVED2        2
#define AM_HAL_PIN_132_GPIO             3
#define AM_HAL_PIN_132_RESERVED4        4
#define AM_HAL_PIN_132_RESERVED5        5
#define AM_HAL_PIN_132_CT132            6
#define AM_HAL_PIN_132_NCE132           7
#define AM_HAL_PIN_132_RESERVED8        8
#define AM_HAL_PIN_132_RESERVED9        9
#define AM_HAL_PIN_132_I2S1_WS          10
#define AM_HAL_PIN_132_FPIO             11
#define AM_HAL_PIN_132_RESERVED12       12
#define AM_HAL_PIN_132_RESERVED13       13
#define AM_HAL_PIN_132_RESERVED14       14
#define AM_HAL_PIN_132_RESERVED15       15

// PIN 133 functions
#define AM_HAL_PIN_133_SDIF1_DAT7       0
#define AM_HAL_PIN_133_RESERVED1        1
#define AM_HAL_PIN_133_RESERVED2        2
#define AM_HAL_PIN_133_GPIO             3
#define AM_HAL_PIN_133_RESERVED4        4
#define AM_HAL_PIN_133_RESERVED5        5
#define AM_HAL_PIN_133_CT133            6
#define AM_HAL_PIN_133_NCE133           7
#define AM_HAL_PIN_133_RESERVED8        8
#define AM_HAL_PIN_133_RESERVED9        9
#define AM_HAL_PIN_133_I2S1_SDIN        10
#define AM_HAL_PIN_133_FPIO             11
#define AM_HAL_PIN_133_RESERVED12       12
#define AM_HAL_PIN_133_RESERVED13       13
#define AM_HAL_PIN_133_RESERVED14       14
#define AM_HAL_PIN_133_RESERVED15       15

// PIN 134 functions
#define AM_HAL_PIN_134_SDIF1_CMD        0
#define AM_HAL_PIN_134_RESERVED1        1
#define AM_HAL_PIN_134_RESERVED2        2
#define AM_HAL_PIN_134_GPIO             3
#define AM_HAL_PIN_134_RESERVED4        4
#define AM_HAL_PIN_134_RESERVED5        5
#define AM_HAL_PIN_134_CT134            6
#define AM_HAL_PIN_134_NCE134           7
#define AM_HAL_PIN_134_RESERVED8        8
#define AM_HAL_PIN_134_RESERVED9        9
#define AM_HAL_PIN_134_RESERVED10       10
#define AM_HAL_PIN_134_FPIO             11
#define AM_HAL_PIN_134_RESERVED12       12
#define AM_HAL_PIN_134_RESERVED13       13
#define AM_HAL_PIN_134_RESERVED14       14
#define AM_HAL_PIN_134_RESERVED15       15

// PIN 135 functions
#define AM_HAL_PIN_135_RESERVED0        0
#define AM_HAL_PIN_135_RESERVED1        1
#define AM_HAL_PIN_135_RESERVED2        2
#define AM_HAL_PIN_135_RESERVED3        3
#define AM_HAL_PIN_135_RESERVED4        4
#define AM_HAL_PIN_135_RESERVED5        5
#define AM_HAL_PIN_135_RESERVED6        6
#define AM_HAL_PIN_135_RESERVED7        7
#define AM_HAL_PIN_135_RESERVED8        8
#define AM_HAL_PIN_135_RESERVED9        9
#define AM_HAL_PIN_135_RESERVED10       10
#define AM_HAL_PIN_135_RESERVED11       11
#define AM_HAL_PIN_135_RESERVED12       12
#define AM_HAL_PIN_135_RESERVED13       13
#define AM_HAL_PIN_135_RESERVED14       14
#define AM_HAL_PIN_135_RESERVED15       15

// PIN 136 functions
#define AM_HAL_PIN_136_RESERVED0        0
#define AM_HAL_PIN_136_DISP_D15         1
#define AM_HAL_PIN_136_RESERVED2        2
#define AM_HAL_PIN_136_GPIO             3
#define AM_HAL_PIN_136_RESERVED4        4
#define AM_HAL_PIN_136_RESERVED5        5
#define AM_HAL_PIN_136_CT136            6
#define AM_HAL_PIN_136_NCE136           7
#define AM_HAL_PIN_136_RESERVED8        8
#define AM_HAL_PIN_136_RESERVED9        9
#define AM_HAL_PIN_136_RESERVED10       10
#define AM_HAL_PIN_136_FPIO             11
#define AM_HAL_PIN_136_RESERVED12       12
#define AM_HAL_PIN_136_RESERVED13       13
#define AM_HAL_PIN_136_RESERVED14       14
#define AM_HAL_PIN_136_RESERVED15       15

// PIN 137 functions
#define AM_HAL_PIN_137_RESERVED0        0
#define AM_HAL_PIN_137_RESERVED1        1
#define AM_HAL_PIN_137_RESERVED2        2
#define AM_HAL_PIN_137_RESERVED3        3
#define AM_HAL_PIN_137_RESERVED4        4
#define AM_HAL_PIN_137_RESERVED5        5
#define AM_HAL_PIN_137_RESERVED6        6
#define AM_HAL_PIN_137_RESERVED7        7
#define AM_HAL_PIN_137_RESERVED8        8
#define AM_HAL_PIN_137_RESERVED9        9
#define AM_HAL_PIN_137_RESERVED10       10
#define AM_HAL_PIN_137_RESERVED11       11
#define AM_HAL_PIN_137_RESERVED12       12
#define AM_HAL_PIN_137_RESERVED13       13
#define AM_HAL_PIN_137_RESERVED14       14
#define AM_HAL_PIN_137_RESERVED15       15

// PIN 138 functions
#define AM_HAL_PIN_138_MNCE1_1          0
#define AM_HAL_PIN_138_32KHzXT          1
#define AM_HAL_PIN_138_RESERVED2        2
#define AM_HAL_PIN_138_GPIO             3
#define AM_HAL_PIN_138_RESERVED4        4
#define AM_HAL_PIN_138_RESERVED5        5
#define AM_HAL_PIN_138_CT138            6
#define AM_HAL_PIN_138_NCE138           7
#define AM_HAL_PIN_138_RESERVED8        8
#define AM_HAL_PIN_138_I2S0_SDIN        9
#define AM_HAL_PIN_138_RESERVED10       10
#define AM_HAL_PIN_138_FPIO             11
#define AM_HAL_PIN_138_RESERVED12       12
#define AM_HAL_PIN_138_RESERVED13       13
#define AM_HAL_PIN_138_RESERVED14       14
#define AM_HAL_PIN_138_RESERVED15       15

// PIN 139 functions
#define AM_HAL_PIN_139_RESERVED0        0
#define AM_HAL_PIN_139_RESERVED1        1
#define AM_HAL_PIN_139_RESERVED2        2
#define AM_HAL_PIN_139_GPIO             3
#define AM_HAL_PIN_139_RESERVED4        4
#define AM_HAL_PIN_139_RESERVED5        5
#define AM_HAL_PIN_139_CT139            6
#define AM_HAL_PIN_139_NCE139           7
#define AM_HAL_PIN_139_RESERVED8        8
#define AM_HAL_PIN_139_RESERVED9        9
#define AM_HAL_PIN_139_I2S0_SDIN        10
#define AM_HAL_PIN_139_FPIO             11
#define AM_HAL_PIN_139_RESERVED12       12
#define AM_HAL_PIN_139_RESERVED13       13
#define AM_HAL_PIN_139_RESERVED14       14
#define AM_HAL_PIN_139_RESERVED15       15

// PIN 140 functions
#define AM_HAL_PIN_140_RESERVED0        0
#define AM_HAL_PIN_140_DISP_D16         1
#define AM_HAL_PIN_140_RESERVED2        2
#define AM_HAL_PIN_140_GPIO             3
#define AM_HAL_PIN_140_RESERVED4        4
#define AM_HAL_PIN_140_RESERVED5        5
#define AM_HAL_PIN_140_CT140            6
#define AM_HAL_PIN_140_NCE140           7
#define AM_HAL_PIN_140_RESERVED8        8
#define AM_HAL_PIN_140_RESERVED9        9
#define AM_HAL_PIN_140_RESERVED10       10
#define AM_HAL_PIN_140_FPIO             11
#define AM_HAL_PIN_140_RESERVED12       12
#define AM_HAL_PIN_140_RESERVED13       13
#define AM_HAL_PIN_140_RESERVED14       14
#define AM_HAL_PIN_140_RESERVED15       15

// PIN 141 functions
#define AM_HAL_PIN_141_RESERVED0        0
#define AM_HAL_PIN_141_DISP_D17         1
#define AM_HAL_PIN_141_RESERVED2        2
#define AM_HAL_PIN_141_GPIO             3
#define AM_HAL_PIN_141_RESERVED4        4
#define AM_HAL_PIN_141_RESERVED5        5
#define AM_HAL_PIN_141_CT141            6
#define AM_HAL_PIN_141_NCE141           7
#define AM_HAL_PIN_141_RESERVED8        8
#define AM_HAL_PIN_141_RESERVED9        9
#define AM_HAL_PIN_141_RESERVED10       10
#define AM_HAL_PIN_141_FPIO             11
#define AM_HAL_PIN_141_RESERVED12       12
#define AM_HAL_PIN_141_RESERVED13       13
#define AM_HAL_PIN_141_RESERVED14       14
#define AM_HAL_PIN_141_RESERVED15       15

// PIN 142 functions
#define AM_HAL_PIN_142_SWTRACECLK       0
#define AM_HAL_PIN_142_RESERVED1        1
#define AM_HAL_PIN_142_RESERVED2        2
#define AM_HAL_PIN_142_GPIO             3
#define AM_HAL_PIN_142_RESERVED4        4
#define AM_HAL_PIN_142_RESERVED5        5
#define AM_HAL_PIN_142_CT142            6
#define AM_HAL_PIN_142_NCE142           7
#define AM_HAL_PIN_142_RESERVED8        8
#define AM_HAL_PIN_142_RESERVED9        9
#define AM_HAL_PIN_142_RESERVED10       10
#define AM_HAL_PIN_142_FPIO             11
#define AM_HAL_PIN_142_RESERVED12       12
#define AM_HAL_PIN_142_RESERVED13       13
#define AM_HAL_PIN_142_RESERVED14       14
#define AM_HAL_PIN_142_RESERVED15       15

// PIN 143 functions
#define AM_HAL_PIN_143_SWTRACE0         0
#define AM_HAL_PIN_143_RESERVED1        1
#define AM_HAL_PIN_143_RESERVED2        2
#define AM_HAL_PIN_143_GPIO             3
#define AM_HAL_PIN_143_RESERVED4        4
#define AM_HAL_PIN_143_RESERVED5        5
#define AM_HAL_PIN_143_CT143            6
#define AM_HAL_PIN_143_NCE143           7
#define AM_HAL_PIN_143_RESERVED8        8
#define AM_HAL_PIN_143_RESERVED9        9
#define AM_HAL_PIN_143_RESERVED10       10
#define AM_HAL_PIN_143_FPIO             11
#define AM_HAL_PIN_143_RESERVED12       12
#define AM_HAL_PIN_143_RESERVED13       13
#define AM_HAL_PIN_143_RESERVED14       14
#define AM_HAL_PIN_143_RESERVED15       15

// PIN 144 functions
#define AM_HAL_PIN_144_SWTRACE1         0
#define AM_HAL_PIN_144_RESERVED1        1
#define AM_HAL_PIN_144_RESERVED2        2
#define AM_HAL_PIN_144_GPIO             3
#define AM_HAL_PIN_144_RESERVED4        4
#define AM_HAL_PIN_144_RESERVED5        5
#define AM_HAL_PIN_144_CT144            6
#define AM_HAL_PIN_144_NCE144           7
#define AM_HAL_PIN_144_RESERVED8        8
#define AM_HAL_PIN_144_RESERVED9        9
#define AM_HAL_PIN_144_RESERVED10       10
#define AM_HAL_PIN_144_FPIO             11
#define AM_HAL_PIN_144_RESERVED12       12
#define AM_HAL_PIN_144_RESERVED13       13
#define AM_HAL_PIN_144_RESERVED14       14
#define AM_HAL_PIN_144_RESERVED15       15

// PIN 145 functions
#define AM_HAL_PIN_145_SWTRACE2         0
#define AM_HAL_PIN_145_RESERVED1        1
#define AM_HAL_PIN_145_RESERVED2        2
#define AM_HAL_PIN_145_GPIO             3
#define AM_HAL_PIN_145_RESERVED4        4
#define AM_HAL_PIN_145_RESERVED5        5
#define AM_HAL_PIN_145_CT145            6
#define AM_HAL_PIN_145_NCE145           7
#define AM_HAL_PIN_145_RESERVED8        8
#define AM_HAL_PIN_145_RESERVED9        9
#define AM_HAL_PIN_145_RESERVED10       10
#define AM_HAL_PIN_145_FPIO             11
#define AM_HAL_PIN_145_RESERVED12       12
#define AM_HAL_PIN_145_RESERVED13       13
#define AM_HAL_PIN_145_RESERVED14       14
#define AM_HAL_PIN_145_RESERVED15       15

// PIN 146 functions
#define AM_HAL_PIN_146_SWTRACE3         0
#define AM_HAL_PIN_146_RESERVED1        1
#define AM_HAL_PIN_146_RESERVED2        2
#define AM_HAL_PIN_146_GPIO             3
#define AM_HAL_PIN_146_RESERVED4        4
#define AM_HAL_PIN_146_RESERVED5        5
#define AM_HAL_PIN_146_CT146            6
#define AM_HAL_PIN_146_NCE146           7
#define AM_HAL_PIN_146_RESERVED8        8
#define AM_HAL_PIN_146_RESERVED9        9
#define AM_HAL_PIN_146_RESERVED10       10
#define AM_HAL_PIN_146_FPIO             11
#define AM_HAL_PIN_146_RESERVED12       12
#define AM_HAL_PIN_146_RESERVED13       13
#define AM_HAL_PIN_146_RESERVED14       14
#define AM_HAL_PIN_146_RESERVED15       15

// PIN 147 functions
#define AM_HAL_PIN_147_SWTRACECTL       0
#define AM_HAL_PIN_147_RESERVED1        1
#define AM_HAL_PIN_147_RESERVED2        2
#define AM_HAL_PIN_147_GPIO             3
#define AM_HAL_PIN_147_RESERVED4        4
#define AM_HAL_PIN_147_RESERVED5        5
#define AM_HAL_PIN_147_CT147            6
#define AM_HAL_PIN_147_NCE147           7
#define AM_HAL_PIN_147_RESERVED8        8
#define AM_HAL_PIN_147_MNCE0_0          9
#define AM_HAL_PIN_147_MNCE3_0          10
#define AM_HAL_PIN_147_FPIO             11
#define AM_HAL_PIN_147_RESERVED12       12
#define AM_HAL_PIN_147_RESERVED13       13
#define AM_HAL_PIN_147_RESERVED14       14
#define AM_HAL_PIN_147_RESERVED15       15

// PIN 148 functions
#define AM_HAL_PIN_148_RESERVED0        0
#define AM_HAL_PIN_148_DISP_D5          1
#define AM_HAL_PIN_148_DBIB_D8          2
#define AM_HAL_PIN_148_GPIO             3
#define AM_HAL_PIN_148_RESERVED4        4
#define AM_HAL_PIN_148_RESERVED5        5
#define AM_HAL_PIN_148_CT148            6
#define AM_HAL_PIN_148_NCE148           7
#define AM_HAL_PIN_148_RESERVED8        8
#define AM_HAL_PIN_148_RESERVED9        9
#define AM_HAL_PIN_148_RESERVED10       10
#define AM_HAL_PIN_148_FPIO             11
#define AM_HAL_PIN_148_RESERVED12       12
#define AM_HAL_PIN_148_RESERVED13       13
#define AM_HAL_PIN_148_RESERVED14       14
#define AM_HAL_PIN_148_RESERVED15       15

// PIN 149 functions
#define AM_HAL_PIN_149_RESERVED0        0
#define AM_HAL_PIN_149_DISP_D6          1
#define AM_HAL_PIN_149_DBIB_D9          2
#define AM_HAL_PIN_149_GPIO             3
#define AM_HAL_PIN_149_RESERVED4        4
#define AM_HAL_PIN_149_RESERVED5        5
#define AM_HAL_PIN_149_CT149            6
#define AM_HAL_PIN_149_NCE149           7
#define AM_HAL_PIN_149_RESERVED8        8
#define AM_HAL_PIN_149_RESERVED9        9
#define AM_HAL_PIN_149_RESERVED10       10
#define AM_HAL_PIN_149_FPIO             11
#define AM_HAL_PIN_149_RESERVED12       12
#define AM_HAL_PIN_149_RESERVED13       13
#define AM_HAL_PIN_149_RESERVED14       14
#define AM_HAL_PIN_149_RESERVED15       15

// PIN 150 functions
#define AM_HAL_PIN_150_RESERVED0        0
#define AM_HAL_PIN_150_DISP_D7          1
#define AM_HAL_PIN_150_DBIB_D10         2
#define AM_HAL_PIN_150_GPIO             3
#define AM_HAL_PIN_150_RESERVED4        4
#define AM_HAL_PIN_150_RESERVED5        5
#define AM_HAL_PIN_150_CT150            6
#define AM_HAL_PIN_150_NCE150           7
#define AM_HAL_PIN_150_RESERVED8        8
#define AM_HAL_PIN_150_RESERVED9        9
#define AM_HAL_PIN_150_RESERVED10       10
#define AM_HAL_PIN_150_FPIO             11
#define AM_HAL_PIN_150_RESERVED12       12
#define AM_HAL_PIN_150_RESERVED13       13
#define AM_HAL_PIN_150_RESERVED14       14
#define AM_HAL_PIN_150_RESERVED15       15

// PIN 151 functions
#define AM_HAL_PIN_151_RESERVED0        0
#define AM_HAL_PIN_151_DISP_D8          1
#define AM_HAL_PIN_151_DBIB_D11         2
#define AM_HAL_PIN_151_GPIO             3
#define AM_HAL_PIN_151_RESERVED4        4
#define AM_HAL_PIN_151_RESERVED5        5
#define AM_HAL_PIN_151_CT151            6
#define AM_HAL_PIN_151_NCE151           7
#define AM_HAL_PIN_151_RESERVED8        8
#define AM_HAL_PIN_151_RESERVED9        9
#define AM_HAL_PIN_151_RESERVED10       10
#define AM_HAL_PIN_151_FPIO             11
#define AM_HAL_PIN_151_RESERVED12       12
#define AM_HAL_PIN_151_RESERVED13       13
#define AM_HAL_PIN_151_RESERVED14       14
#define AM_HAL_PIN_151_RESERVED15       15

// PIN 152 functions
#define AM_HAL_PIN_152_RESERVED0        0
#define AM_HAL_PIN_152_DISP_D9          1
#define AM_HAL_PIN_152_DBIB_D12         2
#define AM_HAL_PIN_152_GPIO             3
#define AM_HAL_PIN_152_RESERVED4        4
#define AM_HAL_PIN_152_RESERVED5        5
#define AM_HAL_PIN_152_CT152            6
#define AM_HAL_PIN_152_NCE152           7
#define AM_HAL_PIN_152_RESERVED8        8
#define AM_HAL_PIN_152_RESERVED9        9
#define AM_HAL_PIN_152_RESERVED10       10
#define AM_HAL_PIN_152_FPIO             11
#define AM_HAL_PIN_152_RESERVED12       12
#define AM_HAL_PIN_152_RESERVED13       13
#define AM_HAL_PIN_152_RESERVED14       14
#define AM_HAL_PIN_152_RESERVED15       15

// PIN 153 functions
#define AM_HAL_PIN_153_RESERVED0        0
#define AM_HAL_PIN_153_DISP_D10         1
#define AM_HAL_PIN_153_DBIB_D13         2
#define AM_HAL_PIN_153_GPIO             3
#define AM_HAL_PIN_153_RESERVED4        4
#define AM_HAL_PIN_153_RESERVED5        5
#define AM_HAL_PIN_153_CT153            6
#define AM_HAL_PIN_153_NCE153           7
#define AM_HAL_PIN_153_RESERVED8        8
#define AM_HAL_PIN_153_RESERVED9        9
#define AM_HAL_PIN_153_RESERVED10       10
#define AM_HAL_PIN_153_FPIO             11
#define AM_HAL_PIN_153_RESERVED12       12
#define AM_HAL_PIN_153_RESERVED13       13
#define AM_HAL_PIN_153_RESERVED14       14
#define AM_HAL_PIN_153_RESERVED15       15

// PIN 154 functions
#define AM_HAL_PIN_154_RESERVED0        0
#define AM_HAL_PIN_154_DISP_D11         1
#define AM_HAL_PIN_154_DBIB_D14         2
#define AM_HAL_PIN_154_GPIO             3
#define AM_HAL_PIN_154_RESERVED4        4
#define AM_HAL_PIN_154_RESERVED5        5
#define AM_HAL_PIN_154_CT154            6
#define AM_HAL_PIN_154_NCE154           7
#define AM_HAL_PIN_154_RESERVED8        8
#define AM_HAL_PIN_154_RESERVED9        9
#define AM_HAL_PIN_154_RESERVED10       10
#define AM_HAL_PIN_154_FPIO             11
#define AM_HAL_PIN_154_RESERVED12       12
#define AM_HAL_PIN_154_RESERVED13       13
#define AM_HAL_PIN_154_RESERVED14       14
#define AM_HAL_PIN_154_RESERVED15       15

// PIN 155 functions
#define AM_HAL_PIN_155_RESERVED0        0
#define AM_HAL_PIN_155_DISP_D12         1
#define AM_HAL_PIN_155_DBIB_D15         2
#define AM_HAL_PIN_155_GPIO             3
#define AM_HAL_PIN_155_RESERVED4        4
#define AM_HAL_PIN_155_RESERVED5        5
#define AM_HAL_PIN_155_CT155            6
#define AM_HAL_PIN_155_NCE155           7
#define AM_HAL_PIN_155_RESERVED8        8
#define AM_HAL_PIN_155_RESERVED9        9
#define AM_HAL_PIN_155_RESERVED10       10
#define AM_HAL_PIN_155_FPIO             11
#define AM_HAL_PIN_155_RESERVED12       12
#define AM_HAL_PIN_155_RESERVED13       13
#define AM_HAL_PIN_155_RESERVED14       14
#define AM_HAL_PIN_155_RESERVED15       15

// PIN 156 functions
#define AM_HAL_PIN_156_SDIF0_DAT4       0
#define AM_HAL_PIN_156_MNCE2_1          1
#define AM_HAL_PIN_156_RESERVED2        2
#define AM_HAL_PIN_156_GPIO             3
#define AM_HAL_PIN_156_RESERVED4        4
#define AM_HAL_PIN_156_RESERVED5        5
#define AM_HAL_PIN_156_CT156            6
#define AM_HAL_PIN_156_NCE156           7
#define AM_HAL_PIN_156_RESERVED8        8
#define AM_HAL_PIN_156_RESERVED9        9
#define AM_HAL_PIN_156_RESERVED10       10
#define AM_HAL_PIN_156_FPIO             11
#define AM_HAL_PIN_156_RESERVED12       12
#define AM_HAL_PIN_156_RESERVED13       13
#define AM_HAL_PIN_156_RESERVED14       14
#define AM_HAL_PIN_156_RESERVED15       15

// PIN 157 functions
#define AM_HAL_PIN_157_SDIF0_DAT5       0
#define AM_HAL_PIN_157_MNCE2_0          1
#define AM_HAL_PIN_157_RESERVED2        2
#define AM_HAL_PIN_157_GPIO             3
#define AM_HAL_PIN_157_RESERVED4        4
#define AM_HAL_PIN_157_RESERVED5        5
#define AM_HAL_PIN_157_CT157            6
#define AM_HAL_PIN_157_NCE157           7
#define AM_HAL_PIN_157_RESERVED8        8
#define AM_HAL_PIN_157_RESERVED9        9
#define AM_HAL_PIN_157_RESERVED10       10
#define AM_HAL_PIN_157_FPIO             11
#define AM_HAL_PIN_157_RESERVED12       12
#define AM_HAL_PIN_157_RESERVED13       13
#define AM_HAL_PIN_157_RESERVED14       14
#define AM_HAL_PIN_157_RESERVED15       15

// PIN 158 functions
#define AM_HAL_PIN_158_SDIF0_DAT6       0
#define AM_HAL_PIN_158_RESERVED1        1
#define AM_HAL_PIN_158_RESERVED2        2
#define AM_HAL_PIN_158_GPIO             3
#define AM_HAL_PIN_158_RESERVED4        4
#define AM_HAL_PIN_158_RESERVED5        5
#define AM_HAL_PIN_158_CT158            6
#define AM_HAL_PIN_158_NCE158           7
#define AM_HAL_PIN_158_RESERVED8        8
#define AM_HAL_PIN_158_RESERVED9        9
#define AM_HAL_PIN_158_RESERVED10       10
#define AM_HAL_PIN_158_FPIO             11
#define AM_HAL_PIN_158_RESERVED12       12
#define AM_HAL_PIN_158_RESERVED13       13
#define AM_HAL_PIN_158_RESERVED14       14
#define AM_HAL_PIN_158_RESERVED15       15

// PIN 159 functions
#define AM_HAL_PIN_159_SDIF0_DAT7       0
#define AM_HAL_PIN_159_RESERVED1        1
#define AM_HAL_PIN_159_RESERVED2        2
#define AM_HAL_PIN_159_GPIO             3
#define AM_HAL_PIN_159_RESERVED4        4
#define AM_HAL_PIN_159_RESERVED5        5
#define AM_HAL_PIN_159_CT159            6
#define AM_HAL_PIN_159_NCE159           7
#define AM_HAL_PIN_159_RESERVED8        8
#define AM_HAL_PIN_159_RESERVED9        9
#define AM_HAL_PIN_159_RESERVED10       10
#define AM_HAL_PIN_159_FPIO             11
#define AM_HAL_PIN_159_RESERVED12       12
#define AM_HAL_PIN_159_RESERVED13       13
#define AM_HAL_PIN_159_RESERVED14       14
#define AM_HAL_PIN_159_RESERVED15       15

// PIN 160 functions
#define AM_HAL_PIN_160_SDIF0_CMD        0
#define AM_HAL_PIN_160_RESERVED1        1
#define AM_HAL_PIN_160_RESERVED2        2
#define AM_HAL_PIN_160_GPIO             3
#define AM_HAL_PIN_160_RESERVED4        4
#define AM_HAL_PIN_160_RESERVED5        5
#define AM_HAL_PIN_160_CT160            6
#define AM_HAL_PIN_160_NCE160           7
#define AM_HAL_PIN_160_RESERVED8        8
#define AM_HAL_PIN_160_RESERVED9        9
#define AM_HAL_PIN_160_RESERVED10       10
#define AM_HAL_PIN_160_FPIO             11
#define AM_HAL_PIN_160_RESERVED12       12
#define AM_HAL_PIN_160_RESERVED13       13
#define AM_HAL_PIN_160_RESERVED14       14
#define AM_HAL_PIN_160_RESERVED15       15

// PIN 161 functions
#define AM_HAL_PIN_161_RESERVED0        0
#define AM_HAL_PIN_161_RESERVED1        1
#define AM_HAL_PIN_161_RESERVED2        2
#define AM_HAL_PIN_161_GPIO             3
#define AM_HAL_PIN_161_RESERVED4        4
#define AM_HAL_PIN_161_RESERVED5        5
#define AM_HAL_PIN_161_CT161            6
#define AM_HAL_PIN_161_NCE161           7
#define AM_HAL_PIN_161_RESERVED8        8
#define AM_HAL_PIN_161_I2S1_SDIN        9
#define AM_HAL_PIN_161_RESERVED10       10
#define AM_HAL_PIN_161_FPIO             11
#define AM_HAL_PIN_161_RESERVED12       12
#define AM_HAL_PIN_161_RESERVED13       13
#define AM_HAL_PIN_161_RESERVED14       14
#define AM_HAL_PIN_161_RESERVED15       15

// PIN 162 functions
#define AM_HAL_PIN_162_SLINT            0
#define AM_HAL_PIN_162_RESERVED1        1
#define AM_HAL_PIN_162_RESERVED2        2
#define AM_HAL_PIN_162_GPIO             3
#define AM_HAL_PIN_162_RESERVED4        4
#define AM_HAL_PIN_162_RESERVED5        5
#define AM_HAL_PIN_162_CT162            6
#define AM_HAL_PIN_162_NCE162           7
#define AM_HAL_PIN_162_RESERVED8        8
#define AM_HAL_PIN_162_RESERVED9        9
#define AM_HAL_PIN_162_RESERVED10       10
#define AM_HAL_PIN_162_FPIO             11
#define AM_HAL_PIN_162_RESERVED12       12
#define AM_HAL_PIN_162_RESERVED13       13
#define AM_HAL_PIN_162_RESERVED14       14
#define AM_HAL_PIN_162_RESERVED15       15

// PIN 163 functions
#define AM_HAL_PIN_163_RESERVED0        0
#define AM_HAL_PIN_163_RESERVED1        1
#define AM_HAL_PIN_163_RESERVED2        2
#define AM_HAL_PIN_163_RESERVED3        3
#define AM_HAL_PIN_163_RESERVED4        4
#define AM_HAL_PIN_163_RESERVED5        5
#define AM_HAL_PIN_163_RESERVED6        6
#define AM_HAL_PIN_163_RESERVED7        7
#define AM_HAL_PIN_163_RESERVED8        8
#define AM_HAL_PIN_163_RESERVED9        9
#define AM_HAL_PIN_163_RESERVED10       10
#define AM_HAL_PIN_163_RESERVED11       11
#define AM_HAL_PIN_163_RESERVED12       12
#define AM_HAL_PIN_163_RESERVED13       13
#define AM_HAL_PIN_163_RESERVED14       14
#define AM_HAL_PIN_163_RESERVED15       15

// PIN 164 functions
#define AM_HAL_PIN_164_RESERVED0        0
#define AM_HAL_PIN_164_RESERVED1        1
#define AM_HAL_PIN_164_RESERVED2        2
#define AM_HAL_PIN_164_RESERVED3        3
#define AM_HAL_PIN_164_RESERVED4        4
#define AM_HAL_PIN_164_RESERVED5        5
#define AM_HAL_PIN_164_RESERVED6        6
#define AM_HAL_PIN_164_RESERVED7        7
#define AM_HAL_PIN_164_RESERVED8        8
#define AM_HAL_PIN_164_RESERVED9        9
#define AM_HAL_PIN_164_RESERVED10       10
#define AM_HAL_PIN_164_RESERVED11       11
#define AM_HAL_PIN_164_RESERVED12       12
#define AM_HAL_PIN_164_RESERVED13       13
#define AM_HAL_PIN_164_RESERVED14       14
#define AM_HAL_PIN_164_RESERVED15       15

// PIN 165 functions
#define AM_HAL_PIN_165_RESERVED0        0
#define AM_HAL_PIN_165_RESERVED1        1
#define AM_HAL_PIN_165_RESERVED2        2
#define AM_HAL_PIN_165_GPIO             3
#define AM_HAL_PIN_165_RESERVED4        4
#define AM_HAL_PIN_165_RESERVED5        5
#define AM_HAL_PIN_165_CT165            6
#define AM_HAL_PIN_165_NCE165           7
#define AM_HAL_PIN_165_RESERVED8        8
#define AM_HAL_PIN_165_RESERVED9        9
#define AM_HAL_PIN_165_RESERVED10       10
#define AM_HAL_PIN_165_FPIO             11
#define AM_HAL_PIN_165_RESERVED12       12
#define AM_HAL_PIN_165_RESERVED13       13
#define AM_HAL_PIN_165_RESERVED14       14
#define AM_HAL_PIN_165_RESERVED15       15

// PIN 166 functions
#define AM_HAL_PIN_166_RESERVED0        0
#define AM_HAL_PIN_166_RESERVED1        1
#define AM_HAL_PIN_166_RESERVED2        2
#define AM_HAL_PIN_166_RESERVED3        3
#define AM_HAL_PIN_166_RESERVED4        4
#define AM_HAL_PIN_166_RESERVED5        5
#define AM_HAL_PIN_166_RESERVED6        6
#define AM_HAL_PIN_166_RESERVED7        7
#define AM_HAL_PIN_166_RESERVED8        8
#define AM_HAL_PIN_166_RESERVED9        9
#define AM_HAL_PIN_166_RESERVED10       10
#define AM_HAL_PIN_166_RESERVED11       11
#define AM_HAL_PIN_166_RESERVED12       12
#define AM_HAL_PIN_166_RESERVED13       13
#define AM_HAL_PIN_166_RESERVED14       14
#define AM_HAL_PIN_166_RESERVED15       15

// PIN 167 functions
#define AM_HAL_PIN_167_RESERVED0        0
#define AM_HAL_PIN_167_RESERVED1        1
#define AM_HAL_PIN_167_RESERVED2        2
#define AM_HAL_PIN_167_RESERVED3        3
#define AM_HAL_PIN_167_RESERVED4        4
#define AM_HAL_PIN_167_RESERVED5        5
#define AM_HAL_PIN_167_RESERVED6        6
#define AM_HAL_PIN_167_RESERVED7        7
#define AM_HAL_PIN_167_RESERVED8        8
#define AM_HAL_PIN_167_RESERVED9        9
#define AM_HAL_PIN_167_RESERVED10       10
#define AM_HAL_PIN_167_RESERVED11       11
#define AM_HAL_PIN_167_RESERVED12       12
#define AM_HAL_PIN_167_RESERVED13       13
#define AM_HAL_PIN_167_RESERVED14       14
#define AM_HAL_PIN_167_RESERVED15       15

// PIN 168 functions
#define AM_HAL_PIN_168_RESERVED0        0
#define AM_HAL_PIN_168_RESERVED1        1
#define AM_HAL_PIN_168_RESERVED2        2
#define AM_HAL_PIN_168_RESERVED3        3
#define AM_HAL_PIN_168_RESERVED4        4
#define AM_HAL_PIN_168_RESERVED5        5
#define AM_HAL_PIN_168_RESERVED6        6
#define AM_HAL_PIN_168_RESERVED7        7
#define AM_HAL_PIN_168_RESERVED8        8
#define AM_HAL_PIN_168_RESERVED9        9
#define AM_HAL_PIN_168_RESERVED10       10
#define AM_HAL_PIN_168_RESERVED11       11
#define AM_HAL_PIN_168_RESERVED12       12
#define AM_HAL_PIN_168_RESERVED13       13
#define AM_HAL_PIN_168_RESERVED14       14
#define AM_HAL_PIN_168_RESERVED15       15

// PIN 169 functions
#define AM_HAL_PIN_169_RESERVED0        0
#define AM_HAL_PIN_169_RESERVED1        1
#define AM_HAL_PIN_169_RESERVED2        2
#define AM_HAL_PIN_169_RESERVED3        3
#define AM_HAL_PIN_169_RESERVED4        4
#define AM_HAL_PIN_169_RESERVED5        5
#define AM_HAL_PIN_169_RESERVED6        6
#define AM_HAL_PIN_169_RESERVED7        7
#define AM_HAL_PIN_169_RESERVED8        8
#define AM_HAL_PIN_169_RESERVED9        9
#define AM_HAL_PIN_169_RESERVED10       10
#define AM_HAL_PIN_169_RESERVED11       11
#define AM_HAL_PIN_169_RESERVED12       12
#define AM_HAL_PIN_169_RESERVED13       13
#define AM_HAL_PIN_169_RESERVED14       14
#define AM_HAL_PIN_169_RESERVED15       15

// PIN 170 functions
#define AM_HAL_PIN_170_RESERVED0        0
#define AM_HAL_PIN_170_RESERVED1        1
#define AM_HAL_PIN_170_RESERVED2        2
#define AM_HAL_PIN_170_RESERVED3        3
#define AM_HAL_PIN_170_RESERVED4        4
#define AM_HAL_PIN_170_RESERVED5        5
#define AM_HAL_PIN_170_RESERVED6        6
#define AM_HAL_PIN_170_RESERVED7        7
#define AM_HAL_PIN_170_RESERVED8        8
#define AM_HAL_PIN_170_RESERVED9        9
#define AM_HAL_PIN_170_RESERVED10       10
#define AM_HAL_PIN_170_RESERVED11       11
#define AM_HAL_PIN_170_RESERVED12       12
#define AM_HAL_PIN_170_RESERVED13       13
#define AM_HAL_PIN_170_RESERVED14       14
#define AM_HAL_PIN_170_RESERVED15       15

// PIN 171 functions
#define AM_HAL_PIN_171_RESERVED0        0
#define AM_HAL_PIN_171_RESERVED1        1
#define AM_HAL_PIN_171_RESERVED2        2
#define AM_HAL_PIN_171_RESERVED3        3
#define AM_HAL_PIN_171_RESERVED4        4
#define AM_HAL_PIN_171_RESERVED5        5
#define AM_HAL_PIN_171_RESERVED6        6
#define AM_HAL_PIN_171_RESERVED7        7
#define AM_HAL_PIN_171_RESERVED8        8
#define AM_HAL_PIN_171_RESERVED9        9
#define AM_HAL_PIN_171_RESERVED10       10
#define AM_HAL_PIN_171_RESERVED11       11
#define AM_HAL_PIN_171_RESERVED12       12
#define AM_HAL_PIN_171_RESERVED13       13
#define AM_HAL_PIN_171_RESERVED14       14
#define AM_HAL_PIN_171_RESERVED15       15

// PIN 172 functions
#define AM_HAL_PIN_172_RESERVED0        0
#define AM_HAL_PIN_172_RESERVED1        1
#define AM_HAL_PIN_172_RESERVED2        2
#define AM_HAL_PIN_172_RESERVED3        3
#define AM_HAL_PIN_172_RESERVED4        4
#define AM_HAL_PIN_172_RESERVED5        5
#define AM_HAL_PIN_172_RESERVED6        6
#define AM_HAL_PIN_172_RESERVED7        7
#define AM_HAL_PIN_172_RESERVED8        8
#define AM_HAL_PIN_172_RESERVED9        9
#define AM_HAL_PIN_172_RESERVED10       10
#define AM_HAL_PIN_172_RESERVED11       11
#define AM_HAL_PIN_172_RESERVED12       12
#define AM_HAL_PIN_172_RESERVED13       13
#define AM_HAL_PIN_172_RESERVED14       14
#define AM_HAL_PIN_172_RESERVED15       15

// PIN 173 functions
#define AM_HAL_PIN_173_SLFDINT          0
#define AM_HAL_PIN_173_SLFD1INT         1
#define AM_HAL_PIN_173_RESERVED2        2
#define AM_HAL_PIN_173_GPIO             3
#define AM_HAL_PIN_173_RESERVED4        4
#define AM_HAL_PIN_173_RESERVED5        5
#define AM_HAL_PIN_173_CT173            6
#define AM_HAL_PIN_173_NCE173           7
#define AM_HAL_PIN_173_RESERVED8        8
#define AM_HAL_PIN_173_RESERVED9        9
#define AM_HAL_PIN_173_RESERVED10       10
#define AM_HAL_PIN_173_FPIO             11
#define AM_HAL_PIN_173_RESERVED12       12
#define AM_HAL_PIN_173_RESERVED13       13
#define AM_HAL_PIN_173_RESERVED14       14
#define AM_HAL_PIN_173_RESERVED15       15

// PIN 174 functions
#define AM_HAL_PIN_174_RESERVED0        0
#define AM_HAL_PIN_174_RESERVED1        1
#define AM_HAL_PIN_174_RESERVED2        2
#define AM_HAL_PIN_174_RESERVED3        3
#define AM_HAL_PIN_174_RESERVED4        4
#define AM_HAL_PIN_174_RESERVED5        5
#define AM_HAL_PIN_174_RESERVED6        6
#define AM_HAL_PIN_174_RESERVED7        7
#define AM_HAL_PIN_174_RESERVED8        8
#define AM_HAL_PIN_174_RESERVED9        9
#define AM_HAL_PIN_174_RESERVED10       10
#define AM_HAL_PIN_174_RESERVED11       11
#define AM_HAL_PIN_174_RESERVED12       12
#define AM_HAL_PIN_174_RESERVED13       13
#define AM_HAL_PIN_174_RESERVED14       14
#define AM_HAL_PIN_174_RESERVED15       15

// PIN 175 functions
#define AM_HAL_PIN_175_RESERVED0        0
#define AM_HAL_PIN_175_RESERVED1        1
#define AM_HAL_PIN_175_RESERVED2        2
#define AM_HAL_PIN_175_GPIO             3
#define AM_HAL_PIN_175_RESERVED4        4
#define AM_HAL_PIN_175_RESERVED5        5
#define AM_HAL_PIN_175_CT175            6
#define AM_HAL_PIN_175_NCE175           7
#define AM_HAL_PIN_175_RESERVED8        8
#define AM_HAL_PIN_175_RESERVED9        9
#define AM_HAL_PIN_175_RESERVED10       10
#define AM_HAL_PIN_175_FPIO             11
#define AM_HAL_PIN_175_RESERVED12       12
#define AM_HAL_PIN_175_RESERVED13       13
#define AM_HAL_PIN_175_RESERVED14       14
#define AM_HAL_PIN_175_RESERVED15       15

// PIN 176 functions
#define AM_HAL_PIN_176_RESERVED0        0
#define AM_HAL_PIN_176_RESERVED1        1
#define AM_HAL_PIN_176_RESERVED2        2
#define AM_HAL_PIN_176_RESERVED3        3
#define AM_HAL_PIN_176_RESERVED4        4
#define AM_HAL_PIN_176_RESERVED5        5
#define AM_HAL_PIN_176_RESERVED6        6
#define AM_HAL_PIN_176_RESERVED7        7
#define AM_HAL_PIN_176_RESERVED8        8
#define AM_HAL_PIN_176_RESERVED9        9
#define AM_HAL_PIN_176_RESERVED10       10
#define AM_HAL_PIN_176_RESERVED11       11
#define AM_HAL_PIN_176_RESERVED12       12
#define AM_HAL_PIN_176_RESERVED13       13
#define AM_HAL_PIN_176_RESERVED14       14
#define AM_HAL_PIN_176_RESERVED15       15

// PIN 177 functions
#define AM_HAL_PIN_177_RESERVED0        0
#define AM_HAL_PIN_177_RESERVED1        1
#define AM_HAL_PIN_177_RESERVED2        2
#define AM_HAL_PIN_177_RESERVED3        3
#define AM_HAL_PIN_177_RESERVED4        4
#define AM_HAL_PIN_177_RESERVED5        5
#define AM_HAL_PIN_177_RESERVED6        6
#define AM_HAL_PIN_177_RESERVED7        7
#define AM_HAL_PIN_177_RESERVED8        8
#define AM_HAL_PIN_177_RESERVED9        9
#define AM_HAL_PIN_177_RESERVED10       10
#define AM_HAL_PIN_177_RESERVED11       11
#define AM_HAL_PIN_177_RESERVED12       12
#define AM_HAL_PIN_177_RESERVED13       13
#define AM_HAL_PIN_177_RESERVED14       14
#define AM_HAL_PIN_177_RESERVED15       15

// PIN 178 functions
#define AM_HAL_PIN_178_RESERVED0        0
#define AM_HAL_PIN_178_RESERVED1        1
#define AM_HAL_PIN_178_RESERVED2        2
#define AM_HAL_PIN_178_RESERVED3        3
#define AM_HAL_PIN_178_RESERVED4        4
#define AM_HAL_PIN_178_RESERVED5        5
#define AM_HAL_PIN_178_RESERVED6        6
#define AM_HAL_PIN_178_RESERVED7        7
#define AM_HAL_PIN_178_RESERVED8        8
#define AM_HAL_PIN_178_RESERVED9        9
#define AM_HAL_PIN_178_RESERVED10       10
#define AM_HAL_PIN_178_RESERVED11       11
#define AM_HAL_PIN_178_RESERVED12       12
#define AM_HAL_PIN_178_RESERVED13       13
#define AM_HAL_PIN_178_RESERVED14       14
#define AM_HAL_PIN_178_RESERVED15       15

// PIN 179 functions
#define AM_HAL_PIN_179_RESERVED0        0
#define AM_HAL_PIN_179_RESERVED1        1
#define AM_HAL_PIN_179_RESERVED2        2
#define AM_HAL_PIN_179_RESERVED3        3
#define AM_HAL_PIN_179_RESERVED4        4
#define AM_HAL_PIN_179_RESERVED5        5
#define AM_HAL_PIN_179_RESERVED6        6
#define AM_HAL_PIN_179_RESERVED7        7
#define AM_HAL_PIN_179_RESERVED8        8
#define AM_HAL_PIN_179_RESERVED9        9
#define AM_HAL_PIN_179_RESERVED10       10
#define AM_HAL_PIN_179_RESERVED11       11
#define AM_HAL_PIN_179_RESERVED12       12
#define AM_HAL_PIN_179_RESERVED13       13
#define AM_HAL_PIN_179_RESERVED14       14
#define AM_HAL_PIN_179_RESERVED15       15

// PIN 180 functions
#define AM_HAL_PIN_180_RESERVED0        0
#define AM_HAL_PIN_180_RESERVED1        1
#define AM_HAL_PIN_180_RESERVED2        2
#define AM_HAL_PIN_180_RESERVED3        3
#define AM_HAL_PIN_180_RESERVED4        4
#define AM_HAL_PIN_180_RESERVED5        5
#define AM_HAL_PIN_180_RESERVED6        6
#define AM_HAL_PIN_180_RESERVED7        7
#define AM_HAL_PIN_180_RESERVED8        8
#define AM_HAL_PIN_180_RESERVED9        9
#define AM_HAL_PIN_180_RESERVED10       10
#define AM_HAL_PIN_180_RESERVED11       11
#define AM_HAL_PIN_180_RESERVED12       12
#define AM_HAL_PIN_180_RESERVED13       13
#define AM_HAL_PIN_180_RESERVED14       14
#define AM_HAL_PIN_180_RESERVED15       15

// PIN 181 functions
#define AM_HAL_PIN_181_RESERVED0        0
#define AM_HAL_PIN_181_RESERVED1        1
#define AM_HAL_PIN_181_RESERVED2        2
#define AM_HAL_PIN_181_RESERVED3        3
#define AM_HAL_PIN_181_RESERVED4        4
#define AM_HAL_PIN_181_RESERVED5        5
#define AM_HAL_PIN_181_RESERVED6        6
#define AM_HAL_PIN_181_RESERVED7        7
#define AM_HAL_PIN_181_RESERVED8        8
#define AM_HAL_PIN_181_RESERVED9        9
#define AM_HAL_PIN_181_RESERVED10       10
#define AM_HAL_PIN_181_RESERVED11       11
#define AM_HAL_PIN_181_RESERVED12       12
#define AM_HAL_PIN_181_RESERVED13       13
#define AM_HAL_PIN_181_RESERVED14       14
#define AM_HAL_PIN_181_RESERVED15       15

// PIN 182 functions
#define AM_HAL_PIN_182_RESERVED0        0
#define AM_HAL_PIN_182_RESERVED1        1
#define AM_HAL_PIN_182_RESERVED2        2
#define AM_HAL_PIN_182_RESERVED3        3
#define AM_HAL_PIN_182_RESERVED4        4
#define AM_HAL_PIN_182_RESERVED5        5
#define AM_HAL_PIN_182_RESERVED6        6
#define AM_HAL_PIN_182_RESERVED7        7
#define AM_HAL_PIN_182_RESERVED8        8
#define AM_HAL_PIN_182_RESERVED9        9
#define AM_HAL_PIN_182_RESERVED10       10
#define AM_HAL_PIN_182_RESERVED11       11
#define AM_HAL_PIN_182_RESERVED12       12
#define AM_HAL_PIN_182_RESERVED13       13
#define AM_HAL_PIN_182_RESERVED14       14
#define AM_HAL_PIN_182_RESERVED15       15

// PIN 183 functions
#define AM_HAL_PIN_183_RESERVED0        0
#define AM_HAL_PIN_183_RESERVED1        1
#define AM_HAL_PIN_183_RESERVED2        2
#define AM_HAL_PIN_183_RESERVED3        3
#define AM_HAL_PIN_183_RESERVED4        4
#define AM_HAL_PIN_183_RESERVED5        5
#define AM_HAL_PIN_183_RESERVED6        6
#define AM_HAL_PIN_183_RESERVED7        7
#define AM_HAL_PIN_183_RESERVED8        8
#define AM_HAL_PIN_183_RESERVED9        9
#define AM_HAL_PIN_183_RESERVED10       10
#define AM_HAL_PIN_183_RESERVED11       11
#define AM_HAL_PIN_183_RESERVED12       12
#define AM_HAL_PIN_183_RESERVED13       13
#define AM_HAL_PIN_183_RESERVED14       14
#define AM_HAL_PIN_183_RESERVED15       15

// PIN 184 functions
#define AM_HAL_PIN_184_RESERVED0        0
#define AM_HAL_PIN_184_RESERVED1        1
#define AM_HAL_PIN_184_RESERVED2        2
#define AM_HAL_PIN_184_RESERVED3        3
#define AM_HAL_PIN_184_RESERVED4        4
#define AM_HAL_PIN_184_RESERVED5        5
#define AM_HAL_PIN_184_RESERVED6        6
#define AM_HAL_PIN_184_RESERVED7        7
#define AM_HAL_PIN_184_RESERVED8        8
#define AM_HAL_PIN_184_RESERVED9        9
#define AM_HAL_PIN_184_RESERVED10       10
#define AM_HAL_PIN_184_RESERVED11       11
#define AM_HAL_PIN_184_RESERVED12       12
#define AM_HAL_PIN_184_RESERVED13       13
#define AM_HAL_PIN_184_RESERVED14       14
#define AM_HAL_PIN_184_RESERVED15       15

// PIN 185 functions
#define AM_HAL_PIN_185_RESERVED0        0
#define AM_HAL_PIN_185_RESERVED1        1
#define AM_HAL_PIN_185_RESERVED2        2
#define AM_HAL_PIN_185_RESERVED3        3
#define AM_HAL_PIN_185_RESERVED4        4
#define AM_HAL_PIN_185_RESERVED5        5
#define AM_HAL_PIN_185_RESERVED6        6
#define AM_HAL_PIN_185_RESERVED7        7
#define AM_HAL_PIN_185_RESERVED8        8
#define AM_HAL_PIN_185_RESERVED9        9
#define AM_HAL_PIN_185_RESERVED10       10
#define AM_HAL_PIN_185_RESERVED11       11
#define AM_HAL_PIN_185_RESERVED12       12
#define AM_HAL_PIN_185_RESERVED13       13
#define AM_HAL_PIN_185_RESERVED14       14
#define AM_HAL_PIN_185_RESERVED15       15

// PIN 186 functions
#define AM_HAL_PIN_186_RESERVED0        0
#define AM_HAL_PIN_186_DISP_SD          1
#define AM_HAL_PIN_186_RESERVED2        2
#define AM_HAL_PIN_186_GPIO             3
#define AM_HAL_PIN_186_RESERVED4        4
#define AM_HAL_PIN_186_RESERVED5        5
#define AM_HAL_PIN_186_CT186            6
#define AM_HAL_PIN_186_NCE186           7
#define AM_HAL_PIN_186_RESERVED8        8
#define AM_HAL_PIN_186_RESERVED9        9
#define AM_HAL_PIN_186_RESERVED10       10
#define AM_HAL_PIN_186_FPIO             11
#define AM_HAL_PIN_186_RESERVED12       12
#define AM_HAL_PIN_186_RESERVED13       13
#define AM_HAL_PIN_186_RESERVED14       14
#define AM_HAL_PIN_186_RESERVED15       15

// PIN 187 functions
#define AM_HAL_PIN_187_RESERVED0        0
#define AM_HAL_PIN_187_DISP_DE          1
#define AM_HAL_PIN_187_RESERVED2        2
#define AM_HAL_PIN_187_GPIO             3
#define AM_HAL_PIN_187_RESERVED4        4
#define AM_HAL_PIN_187_RESERVED5        5
#define AM_HAL_PIN_187_CT187            6
#define AM_HAL_PIN_187_NCE187           7
#define AM_HAL_PIN_187_RESERVED8        8
#define AM_HAL_PIN_187_RESERVED9        9
#define AM_HAL_PIN_187_RESERVED10       10
#define AM_HAL_PIN_187_FPIO             11
#define AM_HAL_PIN_187_RESERVED12       12
#define AM_HAL_PIN_187_RESERVED13       13
#define AM_HAL_PIN_187_RESERVED14       14
#define AM_HAL_PIN_187_RESERVED15       15

// PIN 188 functions
#define AM_HAL_PIN_188_RESERVED0        0
#define AM_HAL_PIN_188_DISP_CM          1
#define AM_HAL_PIN_188_RESERVED2        2
#define AM_HAL_PIN_188_GPIO             3
#define AM_HAL_PIN_188_RESERVED4        4
#define AM_HAL_PIN_188_RESERVED5        5
#define AM_HAL_PIN_188_CT188            6
#define AM_HAL_PIN_188_NCE188           7
#define AM_HAL_PIN_188_RESERVED8        8
#define AM_HAL_PIN_188_RESERVED9        9
#define AM_HAL_PIN_188_RESERVED10       10
#define AM_HAL_PIN_188_FPIO             11
#define AM_HAL_PIN_188_RESERVED12       12
#define AM_HAL_PIN_188_RESERVED13       13
#define AM_HAL_PIN_188_RESERVED14       14
#define AM_HAL_PIN_188_RESERVED15       15

// PIN 189 functions
#define AM_HAL_PIN_189_RESERVED0        0
#define AM_HAL_PIN_189_DISP_PCLK        1
#define AM_HAL_PIN_189_RESERVED2        2
#define AM_HAL_PIN_189_GPIO             3
#define AM_HAL_PIN_189_RESERVED4        4
#define AM_HAL_PIN_189_RESERVED5        5
#define AM_HAL_PIN_189_CT189            6
#define AM_HAL_PIN_189_NCE189           7
#define AM_HAL_PIN_189_RESERVED8        8
#define AM_HAL_PIN_189_RESERVED9        9
#define AM_HAL_PIN_189_RESERVED10       10
#define AM_HAL_PIN_189_FPIO             11
#define AM_HAL_PIN_189_RESERVED12       12
#define AM_HAL_PIN_189_RESERVED13       13
#define AM_HAL_PIN_189_RESERVED14       14
#define AM_HAL_PIN_189_RESERVED15       15

// PIN 190 functions
#define AM_HAL_PIN_190_RESERVED0        0
#define AM_HAL_PIN_190_RESERVED1        1
#define AM_HAL_PIN_190_RESERVED2        2
#define AM_HAL_PIN_190_RESERVED3        3
#define AM_HAL_PIN_190_RESERVED4        4
#define AM_HAL_PIN_190_RESERVED5        5
#define AM_HAL_PIN_190_RESERVED6        6
#define AM_HAL_PIN_190_RESERVED7        7
#define AM_HAL_PIN_190_RESERVED8        8
#define AM_HAL_PIN_190_RESERVED9        9
#define AM_HAL_PIN_190_RESERVED10       10
#define AM_HAL_PIN_190_RESERVED11       11
#define AM_HAL_PIN_190_RESERVED12       12
#define AM_HAL_PIN_190_RESERVED13       13
#define AM_HAL_PIN_190_RESERVED14       14
#define AM_HAL_PIN_190_RESERVED15       15

// PIN 191 functions
#define AM_HAL_PIN_191_RESERVED0        0
#define AM_HAL_PIN_191_RESERVED1        1
#define AM_HAL_PIN_191_RESERVED2        2
#define AM_HAL_PIN_191_GPIO             3
#define AM_HAL_PIN_191_RESERVED4        4
#define AM_HAL_PIN_191_RESERVED5        5
#define AM_HAL_PIN_191_CT191            6
#define AM_HAL_PIN_191_NCE191           7
#define AM_HAL_PIN_191_RESERVED8        8
#define AM_HAL_PIN_191_RESERVED9        9
#define AM_HAL_PIN_191_RESERVED10       10
#define AM_HAL_PIN_191_FPIO             11
#define AM_HAL_PIN_191_RESERVED12       12
#define AM_HAL_PIN_191_RESERVED13       13
#define AM_HAL_PIN_191_RESERVED14       14
#define AM_HAL_PIN_191_RESERVED15       15

// PIN 192 functions
#define AM_HAL_PIN_192_RESERVED0        0
#define AM_HAL_PIN_192_MNCE2_0          1
#define AM_HAL_PIN_192_RESERVED2        2
#define AM_HAL_PIN_192_GPIO             3
#define AM_HAL_PIN_192_RESERVED4        4
#define AM_HAL_PIN_192_RESERVED5        5
#define AM_HAL_PIN_192_CT192            6
#define AM_HAL_PIN_192_NCE192           7
#define AM_HAL_PIN_192_RESERVED8        8
#define AM_HAL_PIN_192_RESERVED9        9
#define AM_HAL_PIN_192_RESERVED10       10
#define AM_HAL_PIN_192_FPIO             11
#define AM_HAL_PIN_192_RESERVED12       12
#define AM_HAL_PIN_192_RESERVED13       13
#define AM_HAL_PIN_192_RESERVED14       14
#define AM_HAL_PIN_192_RESERVED15       15

// PIN 193 functions
#define AM_HAL_PIN_193_RESERVED0        0
#define AM_HAL_PIN_193_RESERVED1        1
#define AM_HAL_PIN_193_RESERVED2        2
#define AM_HAL_PIN_193_RESERVED3        3
#define AM_HAL_PIN_193_RESERVED4        4
#define AM_HAL_PIN_193_RESERVED5        5
#define AM_HAL_PIN_193_RESERVED6        6
#define AM_HAL_PIN_193_RESERVED7        7
#define AM_HAL_PIN_193_RESERVED8        8
#define AM_HAL_PIN_193_RESERVED9        9
#define AM_HAL_PIN_193_RESERVED10       10
#define AM_HAL_PIN_193_RESERVED11       11
#define AM_HAL_PIN_193_RESERVED12       12
#define AM_HAL_PIN_193_RESERVED13       13
#define AM_HAL_PIN_193_RESERVED14       14
#define AM_HAL_PIN_193_RESERVED15       15

// PIN 194 functions
#define AM_HAL_PIN_194_RESERVED0        0
#define AM_HAL_PIN_194_RESERVED1        1
#define AM_HAL_PIN_194_RESERVED2        2
#define AM_HAL_PIN_194_RESERVED3        3
#define AM_HAL_PIN_194_RESERVED4        4
#define AM_HAL_PIN_194_RESERVED5        5
#define AM_HAL_PIN_194_RESERVED6        6
#define AM_HAL_PIN_194_RESERVED7        7
#define AM_HAL_PIN_194_RESERVED8        8
#define AM_HAL_PIN_194_RESERVED9        9
#define AM_HAL_PIN_194_RESERVED10       10
#define AM_HAL_PIN_194_RESERVED11       11
#define AM_HAL_PIN_194_RESERVED12       12
#define AM_HAL_PIN_194_RESERVED13       13
#define AM_HAL_PIN_194_RESERVED14       14
#define AM_HAL_PIN_194_RESERVED15       15

// PIN 195 functions
#define AM_HAL_PIN_195_RESERVED0        0
#define AM_HAL_PIN_195_MNCE3_1          1
#define AM_HAL_PIN_195_MNCE3_0          2
#define AM_HAL_PIN_195_GPIO             3
#define AM_HAL_PIN_195_RESERVED4        4
#define AM_HAL_PIN_195_RESERVED5        5
#define AM_HAL_PIN_195_CT195            6
#define AM_HAL_PIN_195_NCE195           7
#define AM_HAL_PIN_195_RESERVED8        8
#define AM_HAL_PIN_195_RESERVED9        9
#define AM_HAL_PIN_195_RESERVED10       10
#define AM_HAL_PIN_195_FPIO             11
#define AM_HAL_PIN_195_RESERVED12       12
#define AM_HAL_PIN_195_RESERVED13       13
#define AM_HAL_PIN_195_RESERVED14       14
#define AM_HAL_PIN_195_RESERVED15       15

// PIN 196 functions
#define AM_HAL_PIN_196_RESERVED0        0
#define AM_HAL_PIN_196_RESERVED1        1
#define AM_HAL_PIN_196_RESERVED2        2
#define AM_HAL_PIN_196_RESERVED3        3
#define AM_HAL_PIN_196_RESERVED4        4
#define AM_HAL_PIN_196_RESERVED5        5
#define AM_HAL_PIN_196_RESERVED6        6
#define AM_HAL_PIN_196_RESERVED7        7
#define AM_HAL_PIN_196_RESERVED8        8
#define AM_HAL_PIN_196_RESERVED9        9
#define AM_HAL_PIN_196_RESERVED10       10
#define AM_HAL_PIN_196_RESERVED11       11
#define AM_HAL_PIN_196_RESERVED12       12
#define AM_HAL_PIN_196_RESERVED13       13
#define AM_HAL_PIN_196_RESERVED14       14
#define AM_HAL_PIN_196_RESERVED15       15

// PIN 197 functions
#define AM_HAL_PIN_197_RESERVED0        0
#define AM_HAL_PIN_197_RESERVED1        1
#define AM_HAL_PIN_197_RESERVED2        2
#define AM_HAL_PIN_197_RESERVED3        3
#define AM_HAL_PIN_197_RESERVED4        4
#define AM_HAL_PIN_197_RESERVED5        5
#define AM_HAL_PIN_197_RESERVED6        6
#define AM_HAL_PIN_197_RESERVED7        7
#define AM_HAL_PIN_197_RESERVED8        8
#define AM_HAL_PIN_197_RESERVED9        9
#define AM_HAL_PIN_197_RESERVED10       10
#define AM_HAL_PIN_197_RESERVED11       11
#define AM_HAL_PIN_197_RESERVED12       12
#define AM_HAL_PIN_197_RESERVED13       13
#define AM_HAL_PIN_197_RESERVED14       14
#define AM_HAL_PIN_197_RESERVED15       15

// PIN 198 functions
#define AM_HAL_PIN_198_RESERVED0        0
#define AM_HAL_PIN_198_RESERVED1        1
#define AM_HAL_PIN_198_RESERVED2        2
#define AM_HAL_PIN_198_RESERVED3        3
#define AM_HAL_PIN_198_RESERVED4        4
#define AM_HAL_PIN_198_RESERVED5        5
#define AM_HAL_PIN_198_RESERVED6        6
#define AM_HAL_PIN_198_RESERVED7        7
#define AM_HAL_PIN_198_RESERVED8        8
#define AM_HAL_PIN_198_RESERVED9        9
#define AM_HAL_PIN_198_RESERVED10       10
#define AM_HAL_PIN_198_RESERVED11       11
#define AM_HAL_PIN_198_RESERVED12       12
#define AM_HAL_PIN_198_RESERVED13       13
#define AM_HAL_PIN_198_RESERVED14       14
#define AM_HAL_PIN_198_RESERVED15       15

// PIN 199 functions
#define AM_HAL_PIN_199_RESERVED0        0
#define AM_HAL_PIN_199_MNCE0_0          1
#define AM_HAL_PIN_199_RESERVED2        2
#define AM_HAL_PIN_199_GPIO             3
#define AM_HAL_PIN_199_RESERVED4        4
#define AM_HAL_PIN_199_RESERVED5        5
#define AM_HAL_PIN_199_CT199            6
#define AM_HAL_PIN_199_NCE199           7
#define AM_HAL_PIN_199_RESERVED8        8
#define AM_HAL_PIN_199_RESERVED9        9
#define AM_HAL_PIN_199_RESERVED10       10
#define AM_HAL_PIN_199_FPIO             11
#define AM_HAL_PIN_199_RESERVED12       12
#define AM_HAL_PIN_199_RESERVED13       13
#define AM_HAL_PIN_199_RESERVED14       14
#define AM_HAL_PIN_199_RESERVED15       15

// PIN 200 functions
#define AM_HAL_PIN_200_RESERVED0        0
#define AM_HAL_PIN_200_MNCE0_1          1
#define AM_HAL_PIN_200_MNCE0_0          2
#define AM_HAL_PIN_200_GPIO             3
#define AM_HAL_PIN_200_RESERVED4        4
#define AM_HAL_PIN_200_RESERVED5        5
#define AM_HAL_PIN_200_CT200            6
#define AM_HAL_PIN_200_NCE200           7
#define AM_HAL_PIN_200_RESERVED8        8
#define AM_HAL_PIN_200_RESERVED9        9
#define AM_HAL_PIN_200_RESERVED10       10
#define AM_HAL_PIN_200_FPIO             11
#define AM_HAL_PIN_200_RESERVED12       12
#define AM_HAL_PIN_200_RESERVED13       13
#define AM_HAL_PIN_200_RESERVED14       14
#define AM_HAL_PIN_200_RESERVED15       15

// PIN 201 functions
#define AM_HAL_PIN_201_RESERVED0        0
#define AM_HAL_PIN_201_RESERVED1        1
#define AM_HAL_PIN_201_RESERVED2        2
#define AM_HAL_PIN_201_GPIO             3
#define AM_HAL_PIN_201_RESERVED4        4
#define AM_HAL_PIN_201_RESERVED5        5
#define AM_HAL_PIN_201_CT201            6
#define AM_HAL_PIN_201_NCE201           7
#define AM_HAL_PIN_201_RESERVED8        8
#define AM_HAL_PIN_201_RESERVED9        9
#define AM_HAL_PIN_201_RESERVED10       10
#define AM_HAL_PIN_201_FPIO             11
#define AM_HAL_PIN_201_RESERVED12       12
#define AM_HAL_PIN_201_RESERVED13       13
#define AM_HAL_PIN_201_RESERVED14       14
#define AM_HAL_PIN_201_RESERVED15       15

// PIN 202 functions
#define AM_HAL_PIN_202_RESERVED0        0
#define AM_HAL_PIN_202_RESERVED1        1
#define AM_HAL_PIN_202_RESERVED2        2
#define AM_HAL_PIN_202_RESERVED3        3
#define AM_HAL_PIN_202_RESERVED4        4
#define AM_HAL_PIN_202_RESERVED5        5
#define AM_HAL_PIN_202_RESERVED6        6
#define AM_HAL_PIN_202_RESERVED7        7
#define AM_HAL_PIN_202_RESERVED8        8
#define AM_HAL_PIN_202_RESERVED9        9
#define AM_HAL_PIN_202_RESERVED10       10
#define AM_HAL_PIN_202_RESERVED11       11
#define AM_HAL_PIN_202_RESERVED12       12
#define AM_HAL_PIN_202_RESERVED13       13
#define AM_HAL_PIN_202_RESERVED14       14
#define AM_HAL_PIN_202_RESERVED15       15

// PIN 203 functions
#define AM_HAL_PIN_203_RESERVED0        0
#define AM_HAL_PIN_203_RESERVED1        1
#define AM_HAL_PIN_203_RESERVED2        2
#define AM_HAL_PIN_203_RESERVED3        3
#define AM_HAL_PIN_203_RESERVED4        4
#define AM_HAL_PIN_203_RESERVED5        5
#define AM_HAL_PIN_203_RESERVED6        6
#define AM_HAL_PIN_203_RESERVED7        7
#define AM_HAL_PIN_203_RESERVED8        8
#define AM_HAL_PIN_203_RESERVED9        9
#define AM_HAL_PIN_203_RESERVED10       10
#define AM_HAL_PIN_203_RESERVED11       11
#define AM_HAL_PIN_203_RESERVED12       12
#define AM_HAL_PIN_203_RESERVED13       13
#define AM_HAL_PIN_203_RESERVED14       14
#define AM_HAL_PIN_203_RESERVED15       15

// PIN 204 functions
#define AM_HAL_PIN_204_RESERVED0        0
#define AM_HAL_PIN_204_RESERVED1        1
#define AM_HAL_PIN_204_RESERVED2        2
#define AM_HAL_PIN_204_RESERVED3        3
#define AM_HAL_PIN_204_RESERVED4        4
#define AM_HAL_PIN_204_RESERVED5        5
#define AM_HAL_PIN_204_RESERVED6        6
#define AM_HAL_PIN_204_RESERVED7        7
#define AM_HAL_PIN_204_RESERVED8        8
#define AM_HAL_PIN_204_RESERVED9        9
#define AM_HAL_PIN_204_RESERVED10       10
#define AM_HAL_PIN_204_RESERVED11       11
#define AM_HAL_PIN_204_RESERVED12       12
#define AM_HAL_PIN_204_RESERVED13       13
#define AM_HAL_PIN_204_RESERVED14       14
#define AM_HAL_PIN_204_RESERVED15       15

// PIN 205 functions
#define AM_HAL_PIN_205_RESERVED0        0
#define AM_HAL_PIN_205_RESERVED1        1
#define AM_HAL_PIN_205_RESERVED2        2
#define AM_HAL_PIN_205_RESERVED3        3
#define AM_HAL_PIN_205_RESERVED4        4
#define AM_HAL_PIN_205_RESERVED5        5
#define AM_HAL_PIN_205_RESERVED6        6
#define AM_HAL_PIN_205_RESERVED7        7
#define AM_HAL_PIN_205_RESERVED8        8
#define AM_HAL_PIN_205_RESERVED9        9
#define AM_HAL_PIN_205_RESERVED10       10
#define AM_HAL_PIN_205_RESERVED11       11
#define AM_HAL_PIN_205_RESERVED12       12
#define AM_HAL_PIN_205_RESERVED13       13
#define AM_HAL_PIN_205_RESERVED14       14
#define AM_HAL_PIN_205_RESERVED15       15

// PIN 206 functions
#define AM_HAL_PIN_206_RESERVED0        0
#define AM_HAL_PIN_206_RESERVED1        1
#define AM_HAL_PIN_206_RESERVED2        2
#define AM_HAL_PIN_206_RESERVED3        3
#define AM_HAL_PIN_206_RESERVED4        4
#define AM_HAL_PIN_206_RESERVED5        5
#define AM_HAL_PIN_206_RESERVED6        6
#define AM_HAL_PIN_206_RESERVED7        7
#define AM_HAL_PIN_206_RESERVED8        8
#define AM_HAL_PIN_206_RESERVED9        9
#define AM_HAL_PIN_206_RESERVED10       10
#define AM_HAL_PIN_206_RESERVED11       11
#define AM_HAL_PIN_206_RESERVED12       12
#define AM_HAL_PIN_206_RESERVED13       13
#define AM_HAL_PIN_206_RESERVED14       14
#define AM_HAL_PIN_206_RESERVED15       15

// PIN 207 functions
#define AM_HAL_PIN_207_RESERVED0        0
#define AM_HAL_PIN_207_RESERVED1        1
#define AM_HAL_PIN_207_RESERVED2        2
#define AM_HAL_PIN_207_RESERVED3        3
#define AM_HAL_PIN_207_RESERVED4        4
#define AM_HAL_PIN_207_RESERVED5        5
#define AM_HAL_PIN_207_RESERVED6        6
#define AM_HAL_PIN_207_RESERVED7        7
#define AM_HAL_PIN_207_RESERVED8        8
#define AM_HAL_PIN_207_RESERVED9        9
#define AM_HAL_PIN_207_RESERVED10       10
#define AM_HAL_PIN_207_RESERVED11       11
#define AM_HAL_PIN_207_RESERVED12       12
#define AM_HAL_PIN_207_RESERVED13       13
#define AM_HAL_PIN_207_RESERVED14       14
#define AM_HAL_PIN_207_RESERVED15       15

// PIN 208 functions
#define AM_HAL_PIN_208_RESERVED0        0
#define AM_HAL_PIN_208_32KHzXT          1
#define AM_HAL_PIN_208_RESERVED2        2
#define AM_HAL_PIN_208_GPIO             3
#define AM_HAL_PIN_208_RESERVED4        4
#define AM_HAL_PIN_208_RESERVED5        5
#define AM_HAL_PIN_208_CT208            6
#define AM_HAL_PIN_208_NCE208           7
#define AM_HAL_PIN_208_RESERVED8        8
#define AM_HAL_PIN_208_RESERVED9        9
#define AM_HAL_PIN_208_RESERVED10       10
#define AM_HAL_PIN_208_FPIO             11
#define AM_HAL_PIN_208_RESERVED12       12
#define AM_HAL_PIN_208_RESERVED13       13
#define AM_HAL_PIN_208_RESERVED14       14
#define AM_HAL_PIN_208_RESERVED15       15

// PIN 209 functions
#define AM_HAL_PIN_209_DISP_ENB         0
#define AM_HAL_PIN_209_DISP_D1          1
#define AM_HAL_PIN_209_DBIB_CSX         2
#define AM_HAL_PIN_209_GPIO             3
#define AM_HAL_PIN_209_RESERVED4        4
#define AM_HAL_PIN_209_RESERVED5        5
#define AM_HAL_PIN_209_CT209            6
#define AM_HAL_PIN_209_NCE209           7
#define AM_HAL_PIN_209_RESERVED8        8
#define AM_HAL_PIN_209_RESERVED9        9
#define AM_HAL_PIN_209_RESERVED10       10
#define AM_HAL_PIN_209_FPIO             11
#define AM_HAL_PIN_209_RESERVED12       12
#define AM_HAL_PIN_209_RESERVED13       13
#define AM_HAL_PIN_209_RESERVED14       14
#define AM_HAL_PIN_209_RESERVED15       15

// PIN 210 functions
#define AM_HAL_PIN_210_DISP_XRST        0
#define AM_HAL_PIN_210_DISP_D4          1
#define AM_HAL_PIN_210_DBIB_DCX         2
#define AM_HAL_PIN_210_GPIO             3
#define AM_HAL_PIN_210_RESERVED4        4
#define AM_HAL_PIN_210_RESERVED5        5
#define AM_HAL_PIN_210_CT210            6
#define AM_HAL_PIN_210_NCE210           7
#define AM_HAL_PIN_210_RESERVED8        8
#define AM_HAL_PIN_210_RESERVED9        9
#define AM_HAL_PIN_210_RESERVED10       10
#define AM_HAL_PIN_210_FPIO             11
#define AM_HAL_PIN_210_RESERVED12       12
#define AM_HAL_PIN_210_RESERVED13       13
#define AM_HAL_PIN_210_RESERVED14       14
#define AM_HAL_PIN_210_RESERVED15       15

// PIN 211 functions
#define AM_HAL_PIN_211_DISP_R1          0
#define AM_HAL_PIN_211_DISP_D23         1
#define AM_HAL_PIN_211_DBIB_WRX         2
#define AM_HAL_PIN_211_GPIO             3
#define AM_HAL_PIN_211_RESERVED4        4
#define AM_HAL_PIN_211_RESERVED5        5
#define AM_HAL_PIN_211_CT211            6
#define AM_HAL_PIN_211_NCE211           7
#define AM_HAL_PIN_211_RESERVED8        8
#define AM_HAL_PIN_211_RESERVED9        9
#define AM_HAL_PIN_211_RESERVED10       10
#define AM_HAL_PIN_211_FPIO             11
#define AM_HAL_PIN_211_RESERVED12       12
#define AM_HAL_PIN_211_RESERVED13       13
#define AM_HAL_PIN_211_RESERVED14       14
#define AM_HAL_PIN_211_RESERVED15       15

// PIN 212 functions
#define AM_HAL_PIN_212_DISP_R2          0
#define AM_HAL_PIN_212_DISP_D22         1
#define AM_HAL_PIN_212_DBIB_RDX         2
#define AM_HAL_PIN_212_GPIO             3
#define AM_HAL_PIN_212_RESERVED4        4
#define AM_HAL_PIN_212_RESERVED5        5
#define AM_HAL_PIN_212_CT212            6
#define AM_HAL_PIN_212_NCE212           7
#define AM_HAL_PIN_212_RESERVED8        8
#define AM_HAL_PIN_212_RESERVED9        9
#define AM_HAL_PIN_212_RESERVED10       10
#define AM_HAL_PIN_212_FPIO             11
#define AM_HAL_PIN_212_RESERVED12       12
#define AM_HAL_PIN_212_RESERVED13       13
#define AM_HAL_PIN_212_RESERVED14       14
#define AM_HAL_PIN_212_RESERVED15       15

// PIN 213 functions
#define AM_HAL_PIN_213_DISP_G1          0
#define AM_HAL_PIN_213_DISP_D21         1
#define AM_HAL_PIN_213_DBIB_D0          2
#define AM_HAL_PIN_213_GPIO             3
#define AM_HAL_PIN_213_RESERVED4        4
#define AM_HAL_PIN_213_RESERVED5        5
#define AM_HAL_PIN_213_CT213            6
#define AM_HAL_PIN_213_NCE213           7
#define AM_HAL_PIN_213_RESERVED8        8
#define AM_HAL_PIN_213_RESERVED9        9
#define AM_HAL_PIN_213_RESERVED10       10
#define AM_HAL_PIN_213_FPIO             11
#define AM_HAL_PIN_213_RESERVED12       12
#define AM_HAL_PIN_213_RESERVED13       13
#define AM_HAL_PIN_213_RESERVED14       14
#define AM_HAL_PIN_213_RESERVED15       15

// PIN 214 functions
#define AM_HAL_PIN_214_DISP_G2          0
#define AM_HAL_PIN_214_DISP_D20         1
#define AM_HAL_PIN_214_DBIB_D1          2
#define AM_HAL_PIN_214_GPIO             3
#define AM_HAL_PIN_214_RESERVED4        4
#define AM_HAL_PIN_214_RESERVED5        5
#define AM_HAL_PIN_214_CT214            6
#define AM_HAL_PIN_214_NCE214           7
#define AM_HAL_PIN_214_RESERVED8        8
#define AM_HAL_PIN_214_RESERVED9        9
#define AM_HAL_PIN_214_RESERVED10       10
#define AM_HAL_PIN_214_FPIO             11
#define AM_HAL_PIN_214_RESERVED12       12
#define AM_HAL_PIN_214_RESERVED13       13
#define AM_HAL_PIN_214_RESERVED14       14
#define AM_HAL_PIN_214_RESERVED15       15

// PIN 215 functions
#define AM_HAL_PIN_215_DISP_B1          0
#define AM_HAL_PIN_215_DISP_D19         1
#define AM_HAL_PIN_215_DBIB_D2          2
#define AM_HAL_PIN_215_GPIO             3
#define AM_HAL_PIN_215_RESERVED4        4
#define AM_HAL_PIN_215_RESERVED5        5
#define AM_HAL_PIN_215_CT215            6
#define AM_HAL_PIN_215_NCE215           7
#define AM_HAL_PIN_215_RESERVED8        8
#define AM_HAL_PIN_215_RESERVED9        9
#define AM_HAL_PIN_215_RESERVED10       10
#define AM_HAL_PIN_215_FPIO             11
#define AM_HAL_PIN_215_RESERVED12       12
#define AM_HAL_PIN_215_RESERVED13       13
#define AM_HAL_PIN_215_RESERVED14       14
#define AM_HAL_PIN_215_RESERVED15       15

// PIN 216 functions
#define AM_HAL_PIN_216_DISP_B2          0
#define AM_HAL_PIN_216_DISP_D18         1
#define AM_HAL_PIN_216_DBIB_D3          2
#define AM_HAL_PIN_216_GPIO             3
#define AM_HAL_PIN_216_RESERVED4        4
#define AM_HAL_PIN_216_RESERVED5        5
#define AM_HAL_PIN_216_CT216            6
#define AM_HAL_PIN_216_NCE216           7
#define AM_HAL_PIN_216_RESERVED8        8
#define AM_HAL_PIN_216_RESERVED9        9
#define AM_HAL_PIN_216_RESERVED10       10
#define AM_HAL_PIN_216_FPIO             11
#define AM_HAL_PIN_216_RESERVED12       12
#define AM_HAL_PIN_216_RESERVED13       13
#define AM_HAL_PIN_216_RESERVED14       14
#define AM_HAL_PIN_216_RESERVED15       15

// PIN 217 functions
#define AM_HAL_PIN_217_DISP_HST         0
#define AM_HAL_PIN_217_DISP_HS          1
#define AM_HAL_PIN_217_DBIB_D4          2
#define AM_HAL_PIN_217_GPIO             3
#define AM_HAL_PIN_217_RESERVED4        4
#define AM_HAL_PIN_217_RESERVED5        5
#define AM_HAL_PIN_217_CT217            6
#define AM_HAL_PIN_217_NCE217           7
#define AM_HAL_PIN_217_RESERVED8        8
#define AM_HAL_PIN_217_RESERVED9        9
#define AM_HAL_PIN_217_RESERVED10       10
#define AM_HAL_PIN_217_FPIO             11
#define AM_HAL_PIN_217_RESERVED12       12
#define AM_HAL_PIN_217_RESERVED13       13
#define AM_HAL_PIN_217_RESERVED14       14
#define AM_HAL_PIN_217_RESERVED15       15

// PIN 218 functions
#define AM_HAL_PIN_218_DISP_VST         0
#define AM_HAL_PIN_218_DISP_VS          1
#define AM_HAL_PIN_218_DBIB_D5          2
#define AM_HAL_PIN_218_GPIO             3
#define AM_HAL_PIN_218_RESERVED4        4
#define AM_HAL_PIN_218_RESERVED5        5
#define AM_HAL_PIN_218_CT218            6
#define AM_HAL_PIN_218_NCE218           7
#define AM_HAL_PIN_218_RESERVED8        8
#define AM_HAL_PIN_218_RESERVED9        9
#define AM_HAL_PIN_218_RESERVED10       10
#define AM_HAL_PIN_218_FPIO             11
#define AM_HAL_PIN_218_RESERVED12       12
#define AM_HAL_PIN_218_RESERVED13       13
#define AM_HAL_PIN_218_RESERVED14       14
#define AM_HAL_PIN_218_RESERVED15       15

// PIN 219 functions
#define AM_HAL_PIN_219_DISP_HCK         0
#define AM_HAL_PIN_219_DISP_D3          1
#define AM_HAL_PIN_219_DBIB_D6          2
#define AM_HAL_PIN_219_GPIO             3
#define AM_HAL_PIN_219_RESERVED4        4
#define AM_HAL_PIN_219_RESERVED5        5
#define AM_HAL_PIN_219_CT219            6
#define AM_HAL_PIN_219_NCE219           7
#define AM_HAL_PIN_219_RESERVED8        8
#define AM_HAL_PIN_219_RESERVED9        9
#define AM_HAL_PIN_219_RESERVED10       10
#define AM_HAL_PIN_219_FPIO             11
#define AM_HAL_PIN_219_RESERVED12       12
#define AM_HAL_PIN_219_RESERVED13       13
#define AM_HAL_PIN_219_RESERVED14       14
#define AM_HAL_PIN_219_RESERVED15       15

// PIN 220 functions
#define AM_HAL_PIN_220_DISP_VCK         0
#define AM_HAL_PIN_220_DISP_D0          1
#define AM_HAL_PIN_220_DBIB_D7          2
#define AM_HAL_PIN_220_GPIO             3
#define AM_HAL_PIN_220_RESERVED4        4
#define AM_HAL_PIN_220_RESERVED5        5
#define AM_HAL_PIN_220_CT220            6
#define AM_HAL_PIN_220_NCE220           7
#define AM_HAL_PIN_220_RESERVED8        8
#define AM_HAL_PIN_220_RESERVED9        9
#define AM_HAL_PIN_220_RESERVED10       10
#define AM_HAL_PIN_220_FPIO             11
#define AM_HAL_PIN_220_RESERVED12       12
#define AM_HAL_PIN_220_RESERVED13       13
#define AM_HAL_PIN_220_RESERVED14       14
#define AM_HAL_PIN_220_RESERVED15       15

// PIN 221 functions
#define AM_HAL_PIN_221_RESERVED0        0
#define AM_HAL_PIN_221_DISP_D2          1
#define AM_HAL_PIN_221_RESERVED2        2
#define AM_HAL_PIN_221_GPIO             3
#define AM_HAL_PIN_221_RESERVED4        4
#define AM_HAL_PIN_221_RESERVED5        5
#define AM_HAL_PIN_221_CT221            6
#define AM_HAL_PIN_221_NCE221           7
#define AM_HAL_PIN_221_RESERVED8        8
#define AM_HAL_PIN_221_RESERVED9        9
#define AM_HAL_PIN_221_RESERVED10       10
#define AM_HAL_PIN_221_FPIO             11
#define AM_HAL_PIN_221_RESERVED12       12
#define AM_HAL_PIN_221_RESERVED13       13
#define AM_HAL_PIN_221_RESERVED14       14
#define AM_HAL_PIN_221_RESERVED15       15

// PIN 222 functions
#define AM_HAL_PIN_222_RESERVED0        0
#define AM_HAL_PIN_222_DISP_D13         1
#define AM_HAL_PIN_222_RESERVED2        2
#define AM_HAL_PIN_222_GPIO             3
#define AM_HAL_PIN_222_RESERVED4        4
#define AM_HAL_PIN_222_RESERVED5        5
#define AM_HAL_PIN_222_CT222            6
#define AM_HAL_PIN_222_NCE222           7
#define AM_HAL_PIN_222_RESERVED8        8
#define AM_HAL_PIN_222_MNCE0_0          9
#define AM_HAL_PIN_222_RESERVED10       10
#define AM_HAL_PIN_222_FPIO             11
#define AM_HAL_PIN_222_RESERVED12       12
#define AM_HAL_PIN_222_RESERVED13       13
#define AM_HAL_PIN_222_RESERVED14       14
#define AM_HAL_PIN_222_RESERVED15       15

// PIN 223 functions
#define AM_HAL_PIN_223_RESERVED0        0
#define AM_HAL_PIN_223_DISP_D14         1
#define AM_HAL_PIN_223_RESERVED2        2
#define AM_HAL_PIN_223_GPIO             3
#define AM_HAL_PIN_223_RESERVED4        4
#define AM_HAL_PIN_223_RESERVED5        5
#define AM_HAL_PIN_223_CT223            6
#define AM_HAL_PIN_223_NCE223           7
#define AM_HAL_PIN_223_RESERVED8        8
#define AM_HAL_PIN_223_MNCE3_0          9
#define AM_HAL_PIN_223_RESERVED10       10
#define AM_HAL_PIN_223_FPIO             11
#define AM_HAL_PIN_223_RESERVED12       12
#define AM_HAL_PIN_223_RESERVED13       13
#define AM_HAL_PIN_223_RESERVED14       14
#define AM_HAL_PIN_223_RESERVED15       15

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
    AM_HAL_PIN_FN_CLK96MHz,
    AM_HAL_PIN_FN_CLK192MHz,
    AM_HAL_PIN_FN_CLKOUT,
    AM_HAL_PIN_FN_CLKOUT_32M,
    AM_HAL_PIN_FN_CME,
    AM_HAL_PIN_FN_CMLE,
    AM_HAL_PIN_FN_CMPIN0,
    AM_HAL_PIN_FN_CMPIN1,
    AM_HAL_PIN_FN_CMPRF0,
    AM_HAL_PIN_FN_CMPRF1,
    AM_HAL_PIN_FN_CMPRF2,
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
    AM_HAL_PIN_FN_CT120,
    AM_HAL_PIN_FN_CT121,
    AM_HAL_PIN_FN_CT122,
    AM_HAL_PIN_FN_CT123,
    AM_HAL_PIN_FN_CT124,
    AM_HAL_PIN_FN_CT125,
    AM_HAL_PIN_FN_CT126,
    AM_HAL_PIN_FN_CT127,
    AM_HAL_PIN_FN_CT128,
    AM_HAL_PIN_FN_CT129,
    AM_HAL_PIN_FN_CT130,
    AM_HAL_PIN_FN_CT131,
    AM_HAL_PIN_FN_CT132,
    AM_HAL_PIN_FN_CT133,
    AM_HAL_PIN_FN_CT134,
    AM_HAL_PIN_FN_CT136,
    AM_HAL_PIN_FN_CT138,
    AM_HAL_PIN_FN_CT139,
    AM_HAL_PIN_FN_CT140,
    AM_HAL_PIN_FN_CT141,
    AM_HAL_PIN_FN_CT142,
    AM_HAL_PIN_FN_CT143,
    AM_HAL_PIN_FN_CT144,
    AM_HAL_PIN_FN_CT145,
    AM_HAL_PIN_FN_CT146,
    AM_HAL_PIN_FN_CT147,
    AM_HAL_PIN_FN_CT148,
    AM_HAL_PIN_FN_CT149,
    AM_HAL_PIN_FN_CT150,
    AM_HAL_PIN_FN_CT151,
    AM_HAL_PIN_FN_CT152,
    AM_HAL_PIN_FN_CT153,
    AM_HAL_PIN_FN_CT154,
    AM_HAL_PIN_FN_CT155,
    AM_HAL_PIN_FN_CT156,
    AM_HAL_PIN_FN_CT157,
    AM_HAL_PIN_FN_CT158,
    AM_HAL_PIN_FN_CT159,
    AM_HAL_PIN_FN_CT160,
    AM_HAL_PIN_FN_CT161,
    AM_HAL_PIN_FN_CT162,
    AM_HAL_PIN_FN_CT165,
    AM_HAL_PIN_FN_CT173,
    AM_HAL_PIN_FN_CT175,
    AM_HAL_PIN_FN_CT186,
    AM_HAL_PIN_FN_CT187,
    AM_HAL_PIN_FN_CT188,
    AM_HAL_PIN_FN_CT189,
    AM_HAL_PIN_FN_CT191,
    AM_HAL_PIN_FN_CT192,
    AM_HAL_PIN_FN_CT195,
    AM_HAL_PIN_FN_CT199,
    AM_HAL_PIN_FN_CT200,
    AM_HAL_PIN_FN_CT201,
    AM_HAL_PIN_FN_CT208,
    AM_HAL_PIN_FN_CT209,
    AM_HAL_PIN_FN_CT210,
    AM_HAL_PIN_FN_CT211,
    AM_HAL_PIN_FN_CT212,
    AM_HAL_PIN_FN_CT213,
    AM_HAL_PIN_FN_CT214,
    AM_HAL_PIN_FN_CT215,
    AM_HAL_PIN_FN_CT216,
    AM_HAL_PIN_FN_CT217,
    AM_HAL_PIN_FN_CT218,
    AM_HAL_PIN_FN_CT219,
    AM_HAL_PIN_FN_CT220,
    AM_HAL_PIN_FN_CT221,
    AM_HAL_PIN_FN_CT222,
    AM_HAL_PIN_FN_CT223,
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
    AM_HAL_PIN_FN_DISP_CM,
    AM_HAL_PIN_FN_DISP_D0,
    AM_HAL_PIN_FN_DISP_D1,
    AM_HAL_PIN_FN_DISP_D2,
    AM_HAL_PIN_FN_DISP_D3,
    AM_HAL_PIN_FN_DISP_D4,
    AM_HAL_PIN_FN_DISP_D5,
    AM_HAL_PIN_FN_DISP_D6,
    AM_HAL_PIN_FN_DISP_D7,
    AM_HAL_PIN_FN_DISP_D8,
    AM_HAL_PIN_FN_DISP_D9,
    AM_HAL_PIN_FN_DISP_D10,
    AM_HAL_PIN_FN_DISP_D11,
    AM_HAL_PIN_FN_DISP_D12,
    AM_HAL_PIN_FN_DISP_D13,
    AM_HAL_PIN_FN_DISP_D14,
    AM_HAL_PIN_FN_DISP_D15,
    AM_HAL_PIN_FN_DISP_D16,
    AM_HAL_PIN_FN_DISP_D17,
    AM_HAL_PIN_FN_DISP_D18,
    AM_HAL_PIN_FN_DISP_D19,
    AM_HAL_PIN_FN_DISP_D20,
    AM_HAL_PIN_FN_DISP_D21,
    AM_HAL_PIN_FN_DISP_D22,
    AM_HAL_PIN_FN_DISP_D23,
    AM_HAL_PIN_FN_DISP_DE,
    AM_HAL_PIN_FN_DISP_ENB,
    AM_HAL_PIN_FN_DISP_G1,
    AM_HAL_PIN_FN_DISP_G2,
    AM_HAL_PIN_FN_DISP_HCK,
    AM_HAL_PIN_FN_DISP_HS,
    AM_HAL_PIN_FN_DISP_HST,
    AM_HAL_PIN_FN_DISP_PCLK,
    AM_HAL_PIN_FN_DISP_QSPI_D0,
    AM_HAL_PIN_FN_DISP_QSPI_D1,
    AM_HAL_PIN_FN_DISP_QSPI_D2,
    AM_HAL_PIN_FN_DISP_QSPI_D3,
    AM_HAL_PIN_FN_DISP_QSPI_D0_OUT,
    AM_HAL_PIN_FN_DISP_QSPI_SCK,
    AM_HAL_PIN_FN_DISP_R1,
    AM_HAL_PIN_FN_DISP_R2,
    AM_HAL_PIN_FN_DISP_SD,
    AM_HAL_PIN_FN_DISP_SPI_DCX,
    AM_HAL_PIN_FN_DISP_SPI_RST,
    AM_HAL_PIN_FN_DISP_SPI_SCK,
    AM_HAL_PIN_FN_DISP_SPI_SD,
    AM_HAL_PIN_FN_DISP_SPI_SDI,
    AM_HAL_PIN_FN_DISP_SPI_SDO,
    AM_HAL_PIN_FN_DISP_TE,
    AM_HAL_PIN_FN_DISP_VCK,
    AM_HAL_PIN_FN_DISP_VS,
    AM_HAL_PIN_FN_DISP_VST,
    AM_HAL_PIN_FN_DISP_XRST,
    AM_HAL_PIN_FN_EN96MHz,
    AM_HAL_PIN_FN_EN192MHz,
    AM_HAL_PIN_FN_FLB_FCLK,
    AM_HAL_PIN_FN_FLB_TCLK,
    AM_HAL_PIN_FN_FLB_TCLK_1,
    AM_HAL_PIN_FN_FLB_TDI,
    AM_HAL_PIN_FN_FLB_TDI_1,
    AM_HAL_PIN_FN_FLB_TDO,
    AM_HAL_PIN_FN_FLB_TDO_1,
    AM_HAL_PIN_FN_FLB_TMS,
    AM_HAL_PIN_FN_FLB_TMS_1,
    AM_HAL_PIN_FN_FLB_TRSTN,
    AM_HAL_PIN_FN_FLLOAD_ADDR,
    AM_HAL_PIN_FN_FLLOAD_DATA,
    AM_HAL_PIN_FN_FLLOAD_DIR,
    AM_HAL_PIN_FN_FLLOAD_STRB,
    AM_HAL_PIN_FN_FPIO,
    AM_HAL_PIN_FN_GPIO,
    AM_HAL_PIN_FN_HFRC_EXT,
    AM_HAL_PIN_FN_I2S0_CLK,
    AM_HAL_PIN_FN_I2S1_CLK,
    AM_HAL_PIN_FN_I2S0_DATA,
    AM_HAL_PIN_FN_I2S1_DATA,
    AM_HAL_PIN_FN_I2S0_SDIN,
    AM_HAL_PIN_FN_I2S1_SDIN,
    AM_HAL_PIN_FN_I2S0_SDOUT,
    AM_HAL_PIN_FN_I2S1_SDOUT,
    AM_HAL_PIN_FN_I2S0_WS,
    AM_HAL_PIN_FN_I2S1_WS,
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
    AM_HAL_PIN_FN_M6MISO,
    AM_HAL_PIN_FN_M7MISO,
    AM_HAL_PIN_FN_M0MOSI,
    AM_HAL_PIN_FN_M1MOSI,
    AM_HAL_PIN_FN_M2MOSI,
    AM_HAL_PIN_FN_M3MOSI,
    AM_HAL_PIN_FN_M4MOSI,
    AM_HAL_PIN_FN_M5MOSI,
    AM_HAL_PIN_FN_M6MOSI,
    AM_HAL_PIN_FN_M7MOSI,
    AM_HAL_PIN_FN_MNCE0_0,
    AM_HAL_PIN_FN_MNCE0_1,
    AM_HAL_PIN_FN_MNCE1_0,
    AM_HAL_PIN_FN_MNCE1_1,
    AM_HAL_PIN_FN_MNCE2_0,
    AM_HAL_PIN_FN_MNCE2_1,
    AM_HAL_PIN_FN_MNCE3_0,
    AM_HAL_PIN_FN_MNCE3_1,
    AM_HAL_PIN_FN_M0SCK,
    AM_HAL_PIN_FN_M1SCK,
    AM_HAL_PIN_FN_M2SCK,
    AM_HAL_PIN_FN_M3SCK,
    AM_HAL_PIN_FN_M4SCK,
    AM_HAL_PIN_FN_M5SCK,
    AM_HAL_PIN_FN_M6SCK,
    AM_HAL_PIN_FN_M7SCK,
    AM_HAL_PIN_FN_M0SCL,
    AM_HAL_PIN_FN_M1SCL,
    AM_HAL_PIN_FN_M2SCL,
    AM_HAL_PIN_FN_M3SCL,
    AM_HAL_PIN_FN_M4SCL,
    AM_HAL_PIN_FN_M5SCL,
    AM_HAL_PIN_FN_M6SCL,
    AM_HAL_PIN_FN_M7SCL,
    AM_HAL_PIN_FN_M0SDAWIR3,
    AM_HAL_PIN_FN_M1SDAWIR3,
    AM_HAL_PIN_FN_M2SDAWIR3,
    AM_HAL_PIN_FN_M3SDAWIR3,
    AM_HAL_PIN_FN_M4SDAWIR3,
    AM_HAL_PIN_FN_M5SDAWIR3,
    AM_HAL_PIN_FN_M6SDAWIR3,
    AM_HAL_PIN_FN_M7SDAWIR3,
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
    AM_HAL_PIN_FN_MSPI0_10,
    AM_HAL_PIN_FN_MSPI1_0,
    AM_HAL_PIN_FN_MSPI0_11,
    AM_HAL_PIN_FN_MSPI1_1,
    AM_HAL_PIN_FN_MSPI0_12,
    AM_HAL_PIN_FN_MSPI1_2,
    AM_HAL_PIN_FN_MSPI0_13,
    AM_HAL_PIN_FN_MSPI1_3,
    AM_HAL_PIN_FN_MSPI0_14,
    AM_HAL_PIN_FN_MSPI1_4,
    AM_HAL_PIN_FN_MSPI0_15,
    AM_HAL_PIN_FN_MSPI1_5,
    AM_HAL_PIN_FN_MSPI0_16,
    AM_HAL_PIN_FN_MSPI1_6,
    AM_HAL_PIN_FN_MSPI0_17,
    AM_HAL_PIN_FN_MSPI1_7,
    AM_HAL_PIN_FN_MSPI0_18,
    AM_HAL_PIN_FN_MSPI1_8,
    AM_HAL_PIN_FN_MSPI1_9,
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
    AM_HAL_PIN_FN_MSPI3_0,
    AM_HAL_PIN_FN_MSPI3_1,
    AM_HAL_PIN_FN_MSPI3_2,
    AM_HAL_PIN_FN_MSPI3_3,
    AM_HAL_PIN_FN_MSPI3_4,
    AM_HAL_PIN_FN_MSPI3_5,
    AM_HAL_PIN_FN_MSPI3_6,
    AM_HAL_PIN_FN_MSPI3_7,
    AM_HAL_PIN_FN_MSPI3_8,
    AM_HAL_PIN_FN_MSPI3_9,
    AM_HAL_PIN_FN_MSPI3_10,
    AM_HAL_PIN_FN_MSPI3_11,
    AM_HAL_PIN_FN_MSPI3_12,
    AM_HAL_PIN_FN_MSPI3_13,
    AM_HAL_PIN_FN_MSPI3_14,
    AM_HAL_PIN_FN_MSPI3_15,
    AM_HAL_PIN_FN_MSPI3_16,
    AM_HAL_PIN_FN_MSPI3_17,
    AM_HAL_PIN_FN_MSPI3_18,
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
    AM_HAL_PIN_FN_NCE120,
    AM_HAL_PIN_FN_NCE121,
    AM_HAL_PIN_FN_NCE122,
    AM_HAL_PIN_FN_NCE123,
    AM_HAL_PIN_FN_NCE124,
    AM_HAL_PIN_FN_NCE125,
    AM_HAL_PIN_FN_NCE126,
    AM_HAL_PIN_FN_NCE127,
    AM_HAL_PIN_FN_NCE128,
    AM_HAL_PIN_FN_NCE129,
    AM_HAL_PIN_FN_NCE130,
    AM_HAL_PIN_FN_NCE131,
    AM_HAL_PIN_FN_NCE132,
    AM_HAL_PIN_FN_NCE133,
    AM_HAL_PIN_FN_NCE134,
    AM_HAL_PIN_FN_NCE136,
    AM_HAL_PIN_FN_NCE138,
    AM_HAL_PIN_FN_NCE139,
    AM_HAL_PIN_FN_NCE140,
    AM_HAL_PIN_FN_NCE141,
    AM_HAL_PIN_FN_NCE142,
    AM_HAL_PIN_FN_NCE143,
    AM_HAL_PIN_FN_NCE144,
    AM_HAL_PIN_FN_NCE145,
    AM_HAL_PIN_FN_NCE146,
    AM_HAL_PIN_FN_NCE147,
    AM_HAL_PIN_FN_NCE148,
    AM_HAL_PIN_FN_NCE149,
    AM_HAL_PIN_FN_NCE150,
    AM_HAL_PIN_FN_NCE151,
    AM_HAL_PIN_FN_NCE152,
    AM_HAL_PIN_FN_NCE153,
    AM_HAL_PIN_FN_NCE154,
    AM_HAL_PIN_FN_NCE155,
    AM_HAL_PIN_FN_NCE156,
    AM_HAL_PIN_FN_NCE157,
    AM_HAL_PIN_FN_NCE158,
    AM_HAL_PIN_FN_NCE159,
    AM_HAL_PIN_FN_NCE160,
    AM_HAL_PIN_FN_NCE161,
    AM_HAL_PIN_FN_NCE162,
    AM_HAL_PIN_FN_NCE165,
    AM_HAL_PIN_FN_NCE173,
    AM_HAL_PIN_FN_NCE175,
    AM_HAL_PIN_FN_NCE186,
    AM_HAL_PIN_FN_NCE187,
    AM_HAL_PIN_FN_NCE188,
    AM_HAL_PIN_FN_NCE189,
    AM_HAL_PIN_FN_NCE191,
    AM_HAL_PIN_FN_NCE192,
    AM_HAL_PIN_FN_NCE195,
    AM_HAL_PIN_FN_NCE199,
    AM_HAL_PIN_FN_NCE200,
    AM_HAL_PIN_FN_NCE201,
    AM_HAL_PIN_FN_NCE208,
    AM_HAL_PIN_FN_NCE209,
    AM_HAL_PIN_FN_NCE210,
    AM_HAL_PIN_FN_NCE211,
    AM_HAL_PIN_FN_NCE212,
    AM_HAL_PIN_FN_NCE213,
    AM_HAL_PIN_FN_NCE214,
    AM_HAL_PIN_FN_NCE215,
    AM_HAL_PIN_FN_NCE216,
    AM_HAL_PIN_FN_NCE217,
    AM_HAL_PIN_FN_NCE218,
    AM_HAL_PIN_FN_NCE219,
    AM_HAL_PIN_FN_NCE220,
    AM_HAL_PIN_FN_NCE221,
    AM_HAL_PIN_FN_NCE222,
    AM_HAL_PIN_FN_NCE223,
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
    AM_HAL_PIN_FN_PDM0_CLK,
    AM_HAL_PIN_FN_PDM0_DATA,
    AM_HAL_PIN_FN_REFCLK_EXT,
    AM_HAL_PIN_FN_RESERVED0,
    AM_HAL_PIN_FN_RESERVED1,
    AM_HAL_PIN_FN_RESERVED2,
    AM_HAL_PIN_FN_RESERVED3,
    AM_HAL_PIN_FN_RESERVED4,
    AM_HAL_PIN_FN_RESERVED5,
    AM_HAL_PIN_FN_RESERVED6,
    AM_HAL_PIN_FN_RESERVED7,
    AM_HAL_PIN_FN_RESERVED8,
    AM_HAL_PIN_FN_RESERVED9,
    AM_HAL_PIN_FN_RESERVED10,
    AM_HAL_PIN_FN_RESERVED11,
    AM_HAL_PIN_FN_RESERVED12,
    AM_HAL_PIN_FN_RESERVED13,
    AM_HAL_PIN_FN_RESERVED14,
    AM_HAL_PIN_FN_RESERVED15,
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
    AM_HAL_PIN_FN_SLFD1INT,
    AM_HAL_PIN_FN_SLFDMISO,
    AM_HAL_PIN_FN_SLFD1MISO,
    AM_HAL_PIN_FN_SLFDMOSI,
    AM_HAL_PIN_FN_SLFDSCK,
    AM_HAL_PIN_FN_SLFDWIR3,
    AM_HAL_PIN_FN_SLFD1WIR3,
    AM_HAL_PIN_FN_SLFDnCE,
    AM_HAL_PIN_FN_SLINT,
    AM_HAL_PIN_FN_SLMISO,
    AM_HAL_PIN_FN_SLMOSI,
    AM_HAL_PIN_FN_SLSCK,
    AM_HAL_PIN_FN_SLSCL,
    AM_HAL_PIN_FN_SLSDAWIR3,
    AM_HAL_PIN_FN_SLnCE,
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
    AM_HAL_PIN_FN_UART2CTS,
    AM_HAL_PIN_FN_UART3CTS,
    AM_HAL_PIN_FN_UART0RTS,
    AM_HAL_PIN_FN_UART1RTS,
    AM_HAL_PIN_FN_UART2RTS,
    AM_HAL_PIN_FN_UART3RTS,
    AM_HAL_PIN_FN_UART0RX,
    AM_HAL_PIN_FN_UART1RX,
    AM_HAL_PIN_FN_UART2RX,
    AM_HAL_PIN_FN_UART3RX,
    AM_HAL_PIN_FN_UART0TX,
    AM_HAL_PIN_FN_UART1TX,
    AM_HAL_PIN_FN_UART2TX,
    AM_HAL_PIN_FN_UART3TX,
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
