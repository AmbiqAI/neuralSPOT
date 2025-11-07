//*****************************************************************************
//
//! @file ios_fifo_common.h
//!
//! @brief Common utilities for IOS FIFO communication examples.
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

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

#define     USE_SPI             1   // 0 = I2C, 1 = SPI
#define     I2C_ADDR            0x10

#define     XOR_BYTE            0
#define     EMPTY_BYTE          0xEE

#define AM_IOSTEST_IOSTOIOM_DATAAVAIL_INTMASK  1

//*****************************************************************************
//
// Configure GPIOs for communicating with a SPI device
// This should ideally come from BSP to keep the code portable
//
//*****************************************************************************
#define ENABLE_DMA                      1

//
// Apollo510 IOSFD does not support I2C
//
#if defined(AM_PART_APOLLO510) && (USE_SPI == 0)
#undef  TEST_IOS_MODULE
#define TEST_IOS_MODULE                 0
#undef  SLINT_GPIO
#define SLINT_GPIO                      1
#undef  lram_array
#define lram_array                      am_hal_ios_pui8LRAM
#undef  IOS_ACC_IRQ
#define IOS_ACC_IRQ                     IOSLAVEACC_IRQn
#undef  IOS_IRQ
#define IOS_IRQ                         IOSLAVE_IRQn
#undef  ios_isr
#define ios_isr                         am_ioslave_ios_isr
#undef  ios_acc_isr
#define ios_acc_isr                     am_ioslave_acc_isr
#undef  SLINT_FUN_SEL
#define SLINT_FUN_SEL                   AM_HAL_PIN_4_GPIO
#undef  AM_BSP_GPIO_TEST_IOS_SCK
#define AM_BSP_GPIO_TEST_IOS_SCK        AM_BSP_GPIO_IOS_SCL
#undef  AM_BSP_GPIO_TEST_IOS_MOSI
#define AM_BSP_GPIO_TEST_IOS_MOSI       AM_BSP_GPIO_IOS_SDA
#endif

#define HANDSHAKE_PIN                   4

typedef enum
{
    AM_IOSTEST_CMD_START_DATA    = 0xF0,
    AM_IOSTEST_CMD_STOP_DATA     = 0xF1,
    AM_IOSTEST_CMD_ACK_DATA      = 0xF2,
} AM_IOSTEST_CMD_E;

void common_setup(void);

