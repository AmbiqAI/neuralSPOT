//*****************************************************************************
//
//! @file uart_stream_example_config.h
//!
//! @brief Common code and definitions for uart_stream examples
//!
//! @addtogroup uart_stream_echo_rx_dma
//! @{
//! @defgroup uart_stream_example_rx_dma_config UART RX DMA Streaming Example
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

#ifndef EXAMPLE_CONFIG_H
#define EXAMPLE_CONFIG_H

#define TEST_BUFFER_SIZE  (1024*12)

#define MAX_TX_DATA_BUFFER_SIZE         TEST_BUFFER_SIZE
#define UART_RX_TIMEOUT_MS              (5)

#define UART_DMA_MODE  AM_HAL_UART_DMA_RX_DOUBLE
//#define UART_DMA_MODE  AM_HAL_UART_DMA_TX_DOUBLE_BUFFER
//#define UART_DMA_MODE  AM_HAL_UART_DMA_NONE

#define USE_HIGH_SPEED_BAUD

#ifdef USE_HIGH_SPEED_BAUD
#define UART_BAUDRATE 3000000
#else
#define UART_BAUDRATE 115200
#endif

#define DATABUFF_SIZE (1024*8)

#define TIMER_PERIOD_MS 10    //!< call the timer function at 100Hz for this test
#define USE_RX_BUFFER_DTCM 0  //!< don't use DTCM, use sram for RX DMA

#define RX_BUFFER_DTCM_SIZE (1024*2)
#define TX_BUFFER_SRAM_USED RX_BUFFER_DTCM_SIZE

//
//! from am_hal_uart_stream_tx_complete_mode_e;
//
#define TX_COMPLETE_MODE eTXCOMPLETE_INTERRUPT

//
//! from am_hal_uart_streaming_dma_mode_e
//
#define STREAMING_DMA_MODE  AM_HAL_UART_DMA_RX_DOUBLE

#define RX_BUFF_THRESHOLD 64   //!< 64 bytes before rx callback

#endif //EXAMPLE_CONFIG_H
//*****************************************************************************
//
// End Doxygen group.
//! @}
//! @}
//
//*****************************************************************************
