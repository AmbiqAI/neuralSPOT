//*****************************************************************************
//
//! @file i2s_loopback.c
//!
//! @brief An example to show basic I2S operation.
//!
//! Purpose: This example enables the I2S interfaces to loop back data from
//!          each other. Either I2S0 or I2S1 can be selected as the master.
//! NOTE: 1.Use embedded pingpong machine
//!         step 1: prepare 2 blocks of buffer
//!              //sTransfer0.ui32RxTargetAddr = addr1;
//!              //sTransfer0.ui32RxTargetAddrReverse = addr2;
//!              //am_hal_i2s_dma_configure(pI2S0Handle, &g_sI2S0Config, &sTransfer0);
//!         step 2: call am_hal_i2s_interrupt_service to restart DMA operation, the handler help automatically switch to reverse buffer
//!              //am_dspi2s0_isr()
//!              //{
//!              //  am_hal_i2s_interrupt_service(pI2S0Handle, ui32Status, &g_sI2S0Config);
//!              //}
//!         step 3: fetch the ready data
//!              //am_hal_i2s_dma_get_buffer
//!       2.Use new DMA-able buffer
//!         step 1: prepare 1 block of buffer
//!              //sTransfer0.ui32RxTargetAddr = addr1;
//!              //am_hal_i2s_dma_configure(pI2S0Handle, &g_sI2S0Config, &sTransfer0);
//!         step 2: call am_hal_i2s_dma_transfer_continue to restart DMA operation with new allocated buffer
//!              //am_dspi2s0_isr()
//!              //{
//!              //  am_hal_i2s_dma_transfer_continue
//!              //}
//!
//! The required pin connections are as follows.
//! GPIO16 I2S1CLK  to GPIO11 I2S0CLK
//! GPIO18 I2S1WS   to GPIO13 I2S0WS
//! GPIO17 I2S1DOUT to GPIO14 I2S0DIN
//! GPIO19 I2S1DIN  to GPIO12 I2S0DOUT
//!
//! Printing takes place over the ITM at 1M Baud.
//!
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
// This is part of revision release_sdk_4_3_0-0ca7d78a2b of the AmbiqSuite Development Package.
//
//*****************************************************************************
#include <arm_math.h>

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

//*****************************************************************************
//
// Select master
//
//*****************************************************************************
#define I2S_MODULE_0                0
#define I2S_MODULE_1                1
#define USE_I2S_MASTER              I2S_MODULE_1 // 0: master = I2S0; 1: master = I2S1

#define SIZE_SAMPLES                256          // padded to 32 samples, align with DMA Threshold.

#define TEST_I2S_DCMP_INT           1

static uint32_t g_ui32TestLoop    = 0;
//*****************************************************************************
//
// Non-selectable definitions
//
//*****************************************************************************
#define I2S0_DATA_IN_GPIO_FUNC          AM_HAL_PIN_14_I2S0_SDIN
#define I2S0_DATA_IN_GPIO_PIN           14
#define I2S0_DATA_OUT_GPIO_FUNC         AM_HAL_PIN_12_I2S0_SDOUT
#define I2S0_DATA_OUT_GPIO_PIN          12
#define I2S0_CLK_GPIO_FUNC              AM_HAL_PIN_11_I2S0_CLK
#define I2S0_CLK_GPIO_PIN               11
#define I2S0_WS_GPIO_FUNC               AM_HAL_PIN_13_I2S0_WS
#define I2S0_WS_GPIO_PIN                13

#define I2S1_DATA_IN_GPIO_FUNC          AM_HAL_PIN_19_I2S1_SDIN
#define I2S1_DATA_IN_GPIO_PIN           19
#define I2S1_DATA_OUT_GPIO_FUNC         AM_HAL_PIN_17_I2S1_SDOUT
#define I2S1_DATA_OUT_GPIO_PIN          17
#define I2S1_CLK_GPIO_FUNC              AM_HAL_PIN_16_I2S1_CLK
#define I2S1_CLK_GPIO_PIN               16
#define I2S1_WS_GPIO_FUNC               AM_HAL_PIN_18_I2S1_WS
#define I2S1_WS_GPIO_PIN                18

//*****************************************************************************
//
// I2S interrupt configuration.
//
//*****************************************************************************
//
//! I2S interrupts.
//
static const IRQn_Type i2s_interrupts[] =
{
    I2S0_IRQn,
    I2S1_IRQn
};

void *pI2S0Handle;
void *pI2S1Handle;

//*****************************************************************************
//
// Global variables.
//
//*****************************************************************************
// AXI Scratch buffer
// Need to allocate 20 Words even though we only need 16, to ensure we have 16 Byte alignment
AM_SHARED_RW uint32_t axiScratchBuf[20];

// Used as pingpong buffer internally.
// Buffers are to be padded at 16B alignment
AM_SHARED_RW uint32_t g_ui32I2S0RxDataBuffer[2 * SIZE_SAMPLES + 3];
AM_SHARED_RW uint32_t g_ui32I2S0TxDataBuffer[2 * SIZE_SAMPLES + 3];
AM_SHARED_RW uint32_t g_ui32I2S1RxDataBuffer[2 * SIZE_SAMPLES + 3];
AM_SHARED_RW uint32_t g_ui32I2S1TxDataBuffer[2 * SIZE_SAMPLES + 3];

volatile uint32_t g_ui32I2SDmaCpl[8] =
{
    0, //I2S0 TX.
    0, //I2S0 RX.
    0, //I2S1 Tx.
    0, //I2S1 Rx.
    0  //Success or Fail.
};

//
// Programmer Reference setting.
//
static am_hal_i2s_io_signal_t g_sI2SIOConfig =
{
    .eFyncCpol = AM_HAL_I2S_IO_FSYNC_CPOL_HIGH,
    .eTxCpol = AM_HAL_I2S_IO_TX_CPOL_FALLING,
    .eRxCpol = AM_HAL_I2S_IO_RX_CPOL_RISING
};

static am_hal_i2s_data_format_t g_sI2SDataConfig =
{
    .ePhase = AM_HAL_I2S_DATA_PHASE_SINGLE,
    .eDataDelay = 0x1,
    .ui32ChannelNumbersPhase1 = 2,
    .ui32ChannelNumbersPhase2 = 2,
    .eDataJust = AM_HAL_I2S_DATA_JUSTIFIED_LEFT,
    .eChannelLenPhase1 = AM_HAL_I2S_FRAME_WDLEN_32BITS,
    .eChannelLenPhase2 = AM_HAL_I2S_FRAME_WDLEN_32BITS,
    .eSampleLenPhase1 = AM_HAL_I2S_SAMPLE_LENGTH_24BITS,
    .eSampleLenPhase2 = AM_HAL_I2S_SAMPLE_LENGTH_24BITS
};

static am_hal_i2s_config_t g_sI2S0Config =
{
    .eClock               = eAM_HAL_I2S_CLKSEL_HFRC_6MHz,
    .eDiv3                = 0,
#if (USE_I2S_MASTER == I2S_MODULE_0)
    .eMode                = AM_HAL_I2S_IO_MODE_MASTER,
#else
    .eMode                = AM_HAL_I2S_IO_MODE_SLAVE,
#endif
    .eXfer                = AM_HAL_I2S_XFER_RXTX,
    .eData                = &g_sI2SDataConfig,
    .eIO                  = &g_sI2SIOConfig
};

static am_hal_i2s_config_t g_sI2S1Config =
{
    .eClock               = eAM_HAL_I2S_CLKSEL_HFRC_6MHz,
    .eDiv3                = 0,
    .eASRC                = 0,
#if (USE_I2S_MASTER == I2S_MODULE_0)
    .eMode                = AM_HAL_I2S_IO_MODE_SLAVE,
#else
    .eMode                = AM_HAL_I2S_IO_MODE_MASTER,
#endif
    .eXfer                = AM_HAL_I2S_XFER_RXTX,
    .eData                = &g_sI2SDataConfig,
    .eIO                  = &g_sI2SIOConfig
};

//
// Transfer buffer settings.
// Pingpong buffer used in hal.
//
static am_hal_i2s_transfer_t sTransfer0 =
{
    .ui32RxTotalCount         = SIZE_SAMPLES,
    .ui32RxTargetAddr         = 0x0,
    .ui32RxTargetAddrReverse  = 0x0,
    .ui32TxTotalCount         = SIZE_SAMPLES,
    .ui32TxTargetAddr         = 0x0,
    .ui32TxTargetAddrReverse  = 0x0,
};

static am_hal_i2s_transfer_t sTransfer1 =
{
    .ui32RxTotalCount         = SIZE_SAMPLES,
    .ui32RxTargetAddr         = 0x0,
    .ui32RxTargetAddrReverse  = 0x0,
    .ui32TxTotalCount         = SIZE_SAMPLES,
    .ui32TxTargetAddr         = 0x0,
    .ui32TxTargetAddrReverse  = 0x0,
};

//*****************************************************************************
//
// I2S helper function.
//
//*****************************************************************************
static bool check_i2s_data(uint32_t rxtx_size, uint32_t* rx_databuf,          \
                           uint32_t* tx_databuf)
{
    int i, index_0 = 0;

    //
    // Find the first element of Tx buffer in Rx buffer, and return the index.
    // Rx will delay N samples in full duplex mode.
    //
    for ( i = 0; i < rxtx_size; i++ )
    {
        if ( rx_databuf[i] == tx_databuf[0] )
        {
            index_0 = i;
            break;
        }
    }
    for ( i = 0; i < rxtx_size-index_0; i++ )
    {
        if ( rx_databuf[i + index_0] != tx_databuf[i] )
        {
            return false;
        }
    }
    return true;
}

//*****************************************************************************
//
// I2S0 interrupt handler.
//
//*****************************************************************************
void
am_dspi2s0_isr()
{
    uint32_t ui32Status;

    am_hal_i2s_interrupt_status_get(pI2S0Handle, &ui32Status, true);
    am_hal_i2s_interrupt_clear(pI2S0Handle, ui32Status);

    // I2S interrupt service
    am_hal_i2s_interrupt_service(pI2S0Handle, ui32Status, &g_sI2S0Config);

#if TEST_I2S_DCMP_INT
    if (ui32Status & AM_HAL_I2S_INT_TXDMACPL)
    {
      g_ui32I2SDmaCpl[0] = 1;
    }
    if (ui32Status & AM_HAL_I2S_INT_RXDMACPL)
    {
      g_ui32I2SDmaCpl[1] = 1;
    }
#endif
}

//*****************************************************************************
//
// I2S1 interrupt handler.
//
//*****************************************************************************
void
am_dspi2s1_isr()
{
    uint32_t ui32Status;

    am_hal_i2s_interrupt_status_get(pI2S1Handle, &ui32Status, true);
    am_hal_i2s_interrupt_clear(pI2S1Handle, ui32Status);

    am_hal_i2s_interrupt_service(pI2S1Handle, ui32Status, &g_sI2S1Config);

#if TEST_I2S_DCMP_INT
    if (ui32Status & AM_HAL_I2S_INT_TXDMACPL)
    {
        g_ui32I2SDmaCpl[2] = 1;
    }

    if (ui32Status & AM_HAL_I2S_INT_RXDMACPL)
    {
        g_ui32I2SDmaCpl[3] = 1;
    }
#endif
}

//*****************************************************************************
//
// I2S initialization.
//
//*****************************************************************************
void
i2s_init(void)
{
    //
    // Configure the necessary pins.
    //
    am_hal_gpio_pincfg_t sPinCfg =
    {
      .GP.cfg_b.eGPOutCfg = 1,
      .GP.cfg_b.ePullup   = 0
    };

    // I2S0
    sPinCfg.GP.cfg_b.uFuncSel = I2S0_DATA_OUT_GPIO_FUNC;
    am_hal_gpio_pinconfig(I2S0_DATA_OUT_GPIO_PIN, sPinCfg);
    sPinCfg.GP.cfg_b.uFuncSel = I2S0_DATA_IN_GPIO_FUNC;
    am_hal_gpio_pinconfig(I2S0_DATA_IN_GPIO_PIN, sPinCfg);
    sPinCfg.GP.cfg_b.uFuncSel = I2S0_CLK_GPIO_FUNC;
    am_hal_gpio_pinconfig(I2S0_CLK_GPIO_PIN, sPinCfg);
    sPinCfg.GP.cfg_b.uFuncSel = I2S0_WS_GPIO_FUNC;
    am_hal_gpio_pinconfig(I2S0_WS_GPIO_PIN, sPinCfg);
    am_hal_i2s_initialize(I2S_MODULE_0, &pI2S0Handle);
    am_hal_i2s_power_control(pI2S0Handle, AM_HAL_I2S_POWER_ON, false);
    am_hal_i2s_configure(pI2S0Handle, &g_sI2S0Config);
    am_hal_i2s_enable(pI2S0Handle);

    // I2S1
    sPinCfg.GP.cfg_b.uFuncSel = I2S1_DATA_OUT_GPIO_FUNC;
    am_hal_gpio_pinconfig(I2S1_DATA_OUT_GPIO_PIN, sPinCfg);
    sPinCfg.GP.cfg_b.uFuncSel = I2S1_DATA_IN_GPIO_FUNC;
    am_hal_gpio_pinconfig(I2S1_DATA_IN_GPIO_PIN, sPinCfg);
    sPinCfg.GP.cfg_b.uFuncSel = I2S1_CLK_GPIO_FUNC;
    am_hal_gpio_pinconfig(I2S1_CLK_GPIO_PIN, sPinCfg);
    sPinCfg.GP.cfg_b.uFuncSel = I2S1_WS_GPIO_FUNC;
    am_hal_gpio_pinconfig(I2S1_WS_GPIO_PIN, sPinCfg);
    am_hal_i2s_initialize(I2S_MODULE_1, &pI2S1Handle);
    am_hal_i2s_power_control(pI2S1Handle, AM_HAL_I2S_POWER_ON, false);
    am_hal_i2s_configure(pI2S1Handle, &g_sI2S1Config);
    am_hal_i2s_enable(pI2S1Handle);

    //
    // Enable hfrc2.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_HFRC2_START, false);
    am_util_delay_us(500);      // wait for FLL to lock

    //
    // Enable EXT CLK32M.
    //
    if ( (eAM_HAL_I2S_CLKSEL_XTHS_EXTREF_CLK <= g_sI2S0Config.eClock  &&       \
          g_sI2S0Config.eClock <= eAM_HAL_I2S_CLKSEL_XTHS_500KHz) ||           \
         (eAM_HAL_I2S_CLKSEL_XTHS_EXTREF_CLK <= g_sI2S1Config.eClock  && \
          g_sI2S1Config.eClock <= eAM_HAL_I2S_CLKSEL_XTHS_500KHz) )
    {
        am_hal_mcuctrl_control(AM_HAL_MCUCTRL_CONTROL_EXTCLK32M_NORMAL, 0);
        am_util_delay_ms(200);
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
    //
    // Initialize the printf interface for ITM output.
    //
    am_bsp_itm_printf_enable();

    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();

#if (USE_I2S_MASTER == I2S_MODULE_0)
    am_util_stdio_printf("I2S Full Duplex Loopback Test: Master = I2S0, Slave = I2S1.\n\n");
#else
    am_util_stdio_printf("I2S Full Duplex Loopback Test: Master = I2S1, Slave = I2S0.\n\n");
#endif
#ifndef AM_PART_APOLLO4P
    //
    // Set up scratch AXI buf (needs 64B - aligned to 16 Bytes)
    //
    am_hal_daxi_control(AM_HAL_DAXI_CONTROL_AXIMEM, (uint8_t *)((uint32_t)(axiScratchBuf + 3) & ~0xF));
#endif
    //
    // I2S DMA data config:
    //   DMA buffers padded at 16B alignment.
    //
    uint32_t ui32I2S0RxDataPtr = (uint32_t)((uint32_t)(g_ui32I2S0RxDataBuffer + 3) & ~0xF);
    uint32_t ui32I2S0TxDataPtr = (uint32_t)((uint32_t)(g_ui32I2S0TxDataBuffer + 3) & ~0xF);
    uint32_t ui32I2S1RxDataPtr = (uint32_t)((uint32_t)(g_ui32I2S1RxDataBuffer + 3) & ~0xF);
    uint32_t ui32I2S1TxDataPtr = (uint32_t)((uint32_t)(g_ui32I2S1TxDataBuffer + 3) & ~0xF);
    // Initialize data.
    for (int i = 0; i < SIZE_SAMPLES; i++)
    {
        ((uint32_t*)ui32I2S0TxDataPtr)[i] = (i & 0xFF) | 0xAB0000;
        ((uint32_t*)ui32I2S1TxDataPtr)[i] = (i & 0xFF) | 0xAB0000;

        ((uint32_t*)(ui32I2S0TxDataPtr + sizeof(uint32_t)*SIZE_SAMPLES ))[i] = (i & 0xFF) | 0xCD0000;
        ((uint32_t*)(ui32I2S1TxDataPtr + sizeof(uint32_t)*SIZE_SAMPLES ))[i] = (i & 0xFF) | 0xCD0000;
    }
    sTransfer0.ui32RxTargetAddr = ui32I2S0RxDataPtr;
    sTransfer0.ui32RxTargetAddrReverse = sTransfer0.ui32RxTargetAddr + sizeof(uint32_t)*sTransfer0.ui32RxTotalCount;
    sTransfer0.ui32TxTargetAddr = ui32I2S0TxDataPtr;
    sTransfer0.ui32TxTargetAddrReverse = sTransfer0.ui32TxTargetAddr + sizeof(uint32_t)*sTransfer0.ui32TxTotalCount;

    sTransfer1.ui32RxTargetAddr = ui32I2S1RxDataPtr;
    sTransfer1.ui32RxTargetAddrReverse = sTransfer1.ui32RxTargetAddr + sizeof(uint32_t)*sTransfer1.ui32RxTotalCount;
    sTransfer1.ui32TxTargetAddr = ui32I2S1TxDataPtr;
    sTransfer1.ui32TxTargetAddrReverse = sTransfer1.ui32TxTargetAddr + sizeof(uint32_t)*sTransfer1.ui32TxTotalCount;

    i2s_init();
    am_hal_i2s_dma_configure(pI2S0Handle, &g_sI2S0Config, &sTransfer0);
    am_hal_i2s_dma_configure(pI2S1Handle, &g_sI2S1Config, &sTransfer1);
    NVIC_EnableIRQ(i2s_interrupts[I2S_MODULE_0]);
    NVIC_EnableIRQ(i2s_interrupts[I2S_MODULE_1]);
    am_hal_interrupt_master_enable();

    // Start DMA transaction.
#if (USE_I2S_MASTER == I2S_MODULE_0)
    am_hal_i2s_dma_transfer_start(pI2S1Handle, &g_sI2S1Config);
    am_hal_i2s_dma_transfer_start(pI2S0Handle, &g_sI2S0Config);
#else
    am_hal_i2s_dma_transfer_start(pI2S0Handle, &g_sI2S0Config);
    am_hal_i2s_dma_transfer_start(pI2S1Handle, &g_sI2S1Config);
#endif

    //
    // Loop forever
    //
    while (1)
    {
        if ( g_ui32I2SDmaCpl[0] && g_ui32I2SDmaCpl[1] &&    \
             g_ui32I2SDmaCpl[2] && g_ui32I2SDmaCpl[3] )
        {
            g_ui32TestLoop++;
            g_ui32I2SDmaCpl[0] = g_ui32I2SDmaCpl[1] = g_ui32I2SDmaCpl[2] =    \
                                 g_ui32I2SDmaCpl[3] = 0;

            uint32_t i2s0_rx_buffer = am_hal_i2s_dma_get_buffer(pI2S0Handle, AM_HAL_I2S_XFER_RX);
            uint32_t i2s1_rx_buffer = am_hal_i2s_dma_get_buffer(pI2S1Handle, AM_HAL_I2S_XFER_RX);

            uint32_t i2s0_tx_buffer = am_hal_i2s_dma_get_buffer(pI2S0Handle, AM_HAL_I2S_XFER_TX);
            uint32_t i2s1_tx_buffer = am_hal_i2s_dma_get_buffer(pI2S1Handle, AM_HAL_I2S_XFER_TX);

            if ( check_i2s_data(SIZE_SAMPLES, (uint32_t*)i2s1_rx_buffer, (uint32_t*)i2s0_tx_buffer)          \
                 && check_i2s_data(SIZE_SAMPLES, (uint32_t*)i2s0_rx_buffer, (uint32_t*)i2s1_tx_buffer) )
            {
                g_ui32I2SDmaCpl[4] = 1;
                am_util_stdio_printf("I2S Loopback Iteration %d PASSED!\n", g_ui32TestLoop);
            }
            else
            {
                am_util_stdio_printf("I2S Loopback Iteration %d FAILED!\n", g_ui32TestLoop);
            }
        }

        //
        // I2S DMA will stop once run into Deep Sleep
        //
        //am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
    }
}
