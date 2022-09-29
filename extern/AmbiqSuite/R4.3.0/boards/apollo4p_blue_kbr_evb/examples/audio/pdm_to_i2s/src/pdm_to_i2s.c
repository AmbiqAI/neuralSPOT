//*****************************************************************************
//
//! @file pdm_to_i2s.c
//!
//! @brief An example to show PDM to I2S(slave) operation.
//!
//! Purpose: This example enables the PDM and I2S interface to collect audio signals from
//!          an external microphone, I2S module using pingpong buffer to interact with PDM,
//!          and start transaction when mclk is supplied(from external I2S master).
//!          Notice: external mclk should be supplied first at this example.
//! The required pin connections are:
//!
//! Printing takes place over the ITM at 1M Baud.
//!
//! GPIO 50 - PDM0 CLK
//! GPIO 51 - PDM0 DATA
//!
//! GPIO 52 - PDM1 CLK
//! GPIO 53 - PDM1 DATA
//!
//! GPIO 54 - PDM2 CLK
//! GPIO 55 - PDM2 DATA
//!
//! GPIO 56 - PDM3 CLK
//! GPIO 57 - PDM3 DATA
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
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "SEGGER_RTT.h"

//*****************************************************************************
//
// Insert compiler version at compile time.
//
//*****************************************************************************

//*****************************************************************************
//
// Example parameters.
//
//*****************************************************************************
#define  PDM_MODULE             0

//*****************************************************************************
//
// PDM pins
//
//*****************************************************************************
#if PDM_MODULE == 0
#define PDM_DATA_GPIO_FUNC  AM_HAL_PIN_51_PDM0_DATA
#define PDM_DATA_GPIO_PIN   51
#define PDM_CLK_GPIO_FUNC   AM_HAL_PIN_50_PDM0_CLK
#define PDM_CLK_GPIO_PIN    50
#elif PDM_MODULE == 1
#define PDM_DATA_GPIO_FUNC  AM_HAL_PIN_53_PDM1_DATA
#define PDM_DATA_GPIO_PIN   53
#define PDM_CLK_GPIO_FUNC   AM_HAL_PIN_52_PDM1_CLK
#define PDM_CLK_GPIO_PIN    52
#elif PDM_MODULE == 2
#define PDM_DATA_GPIO_FUNC  AM_HAL_PIN_55_PDM2_DATA
#define PDM_DATA_GPIO_PIN   55
#define PDM_CLK_GPIO_FUNC   AM_HAL_PIN_54_PDM2_CLK
#define PDM_CLK_GPIO_PIN    54
#elif PDM_MODULE == 3
#define PDM_DATA_GPIO_FUNC  AM_HAL_PIN_57_PDM3_DATA
#define PDM_DATA_GPIO_PIN   57
#define PDM_CLK_GPIO_FUNC   AM_HAL_PIN_56_PDM3_CLK
#define PDM_CLK_GPIO_PIN    56
#endif

// should be padded to 32 samples.
#define PDM_STREAM_SIZE                320 // 20ms
#define DMA_SIZE                       PDM_STREAM_SIZE
#define PDM_STREAM_BYTES               (PDM_STREAM_SIZE * 4)

//! PDM interrupts.
static const IRQn_Type pdm_interrupts[] =
{
    PDM0_IRQn,
    PDM1_IRQn,
    PDM2_IRQn,
    PDM3_IRQn
};

//
// Take over the interrupt handler for whichever PDM we're using.
//
#define example_pdm_isr     am_pdm_isr1(PDM_MODULE)
#define am_pdm_isr1(n)      am_pdm_isr(n)
#define am_pdm_isr(n)       am_pdm ## n ## _isr

#define FIFO_THRESHOLD_CNT      16
#define DMA_BYTES               PDM_FFT_BYTES

#define DATA_VERIFY             0
#define PDM_ISR_TEST_PAD        2
#define I2S_ISR_TEST_PAD        3
//*****************************************************************************
//
// PDM configuration information.
//
//*****************************************************************************
void *PDMHandle;

am_hal_pdm_config_t g_sPdmConfig =
{

#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P)
    //
    // Example setting:
    //  1.5MHz PDM CLK OUT:
    //      AM_HAL_PDM_CLK_HFRC2ADJ_24_576MHZ, AM_HAL_PDM_MCLKDIV_1, AM_HAL_PDM_PDMA_CLKO_DIV7
    //  16.00KHz 24bit Sampling:
    //      DecimationRate = 48
    //
    .ePDMClkSpeed = AM_HAL_PDM_CLK_HFRC2ADJ_24_576MHZ,
    .eClkDivider = AM_HAL_PDM_MCLKDIV_1,
    .ePDMAClkOutDivder = AM_HAL_PDM_PDMA_CLKO_DIV7,
    .ui32DecimationRate = 48,

#else
    //
    // Example setting:
    //  1.5MHz PDM CLK OUT:
    //      AM_HAL_PDM_CLK_24MHZ, AM_HAL_PDM_MCLKDIV_1, AM_HAL_PDM_PDMA_CLKO_DIV7
    //  15.625KHz 24bit Sampling:
    //      DecimationRate = 48
    //
    .ePDMClkSpeed = AM_HAL_PDM_CLK_24MHZ,
    .eClkDivider = AM_HAL_PDM_MCLKDIV_1,
    .ePDMAClkOutDivder = AM_HAL_PDM_PDMA_CLKO_DIV7,
    .ui32DecimationRate = 48,
#endif

    .eLeftGain = AM_HAL_PDM_GAIN_P105DB,
    .eRightGain = AM_HAL_PDM_GAIN_P105DB,
    .eStepSize = AM_HAL_PDM_GAIN_STEP_0_13DB,

    .bHighPassEnable = AM_HAL_PDM_HIGH_PASS_ENABLE,
    .ui32HighPassCutoff = 0x3,
    .bDataPacking = 1,
    .ePCMChannels = AM_HAL_PDM_CHANNEL_STEREO,

    .bPDMSampleDelay = AM_HAL_PDM_CLKOUT_PHSDLY_NONE,
    .ui32GainChangeDelay = AM_HAL_PDM_CLKOUT_DELAY_NONE,

    .bSoftMute = 0,
    .bLRSwap = 0,
};

//*****************************************************************************
//
// Global variables.
//
//*****************************************************************************
uint32_t g_ui32FifoOVFCount = 0;
volatile bool g_bPDMDataReady = false;

//*****************************************************************************
//
// Global variables.
//
//*****************************************************************************

// AXI Scratch buffer
// Need to allocate 20 Words even though we only need 16, to ensure we have 16 Byte alignment
AM_SHARED_RW uint32_t axiScratchBuf[20];

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//*****************************************************************************
//
// I2S
//
//*****************************************************************************
#define     I2S_MODULE          0   // I2S0
#define     USE_DMA             1

#if USE_DMA
//#define BUFFER_SIZE_BYTES               PDM_FFT_BYTES
//! RX size = TX size * output sample rate /internal sample rate
//! Notice .eClock from g_sI2SConfig and g_sI2SConfig_slave.
//#define BUFFER_SIZE_ASRC_RX_BYTES       PDM_FFT_BYTES
#endif

//! I2S interrupts.
static const IRQn_Type i2s_interrupts[] =
{
    I2S0_IRQn,
    I2S1_IRQn
};

#if I2S_MODULE == 0
#define I2S_DATA_IN_GPIO_FUNC          AM_HAL_PIN_14_I2S0_SDIN
#define I2S_DATA_IN_GPIO_PIN           14
#define I2S_DATA_OUT_GPIO_FUNC         AM_HAL_PIN_12_I2S0_SDOUT
#define I2S_DATA_OUT_GPIO_PIN          12
#define I2S_CLK_GPIO_FUNC              AM_HAL_PIN_11_I2S0_CLK
#define I2S_CLK_GPIO_PIN               11
#define I2S_WS_GPIO_FUNC               AM_HAL_PIN_13_I2S0_WS
#define I2S_WS_GPIO_PIN                13
#elif I2S_MODULE == 1
#define I2S_DATA_IN_GPIO_FUNC          AM_HAL_PIN_19_I2S1_SDIN
#define I2S_DATA_IN_GPIO_PIN           19
#define I2S_DATA_OUT_GPIO_FUNC         AM_HAL_PIN_17_I2S1_SDOUT
#define I2S_DATA_OUT_GPIO_PIN          17
#define I2S_CLK_GPIO_FUNC              AM_HAL_PIN_16_I2S1_CLK
#define I2S_CLK_GPIO_PIN               16
#define I2S_WS_GPIO_FUNC               AM_HAL_PIN_18_I2S1_WS
#define I2S_WS_GPIO_PIN                18
#endif

void *I2SHandle;

// Programmer Reference setting.
static am_hal_i2s_io_signal_t g_sI2SIOConfig =
{
  .eFyncCpol = AM_HAL_I2S_IO_FSYNC_CPOL_HIGH,

  .eTxCpol = AM_HAL_I2S_IO_TX_CPOL_FALLING,
  .eRxCpol = AM_HAL_I2S_IO_RX_CPOL_RISING,
};

static am_hal_i2s_data_format_t g_sI2SDataConfig =
{
  .ePhase = AM_HAL_I2S_DATA_PHASE_SINGLE,
  .ui32ChannelNumbersPhase1 = 2,
  .ui32ChannelNumbersPhase2 = 0,
  .eDataDelay = 0x1,
  .eDataJust = AM_HAL_I2S_DATA_JUSTIFIED_LEFT,

  .eChannelLenPhase1 = AM_HAL_I2S_FRAME_WDLEN_32BITS, //32bits
  .eChannelLenPhase2 = AM_HAL_I2S_FRAME_WDLEN_32BITS, //32bits
  .eSampleLenPhase1 = AM_HAL_I2S_SAMPLE_LENGTH_24BITS,
  .eSampleLenPhase2 = AM_HAL_I2S_SAMPLE_LENGTH_24BITS
};

//*****************************************************************************
//
// I2S configuration information.
//
//*****************************************************************************
static am_hal_i2s_config_t g_sI2SConfig =
{
  .eClock               = eAM_HAL_I2S_CLKSEL_HFRC2_3MHz, //eAM_HAL_I2S_CLKSEL_HFRC_6MHz,
  .eDiv3                = 1,
#if DATA_VERIFY
  .eASRC                = 0,
#else
  .eASRC                = 0,
#endif
  .eMode                = AM_HAL_I2S_IO_MODE_MASTER,
  .eXfer                = AM_HAL_I2S_XFER_TX,
  .eData                = &g_sI2SDataConfig,
  .eIO                  = &g_sI2SIOConfig
};

// Transfer setting.
static am_hal_i2s_transfer_t g_sTransferI2S =
{
    .ui32TxTotalCount         = DMA_SIZE,

    .ui32TxTargetAddr         = 0x0,
    .ui32TxTargetAddrReverse  = 0x0,

};

am_hal_pdm_transfer_t g_sTransferPDM =
{
    .ui32TargetAddr        = 0x0,
    .ui32TargetAddrReverse = 0x0,
    .ui32TotalCount        = PDM_STREAM_BYTES,
};

//pingpong buffer size: PMD_DMATOTCOUNT(x2)
AM_SHARED_RW uint32_t g_ui32PingPongBuffer[DMA_SIZE + DMA_SIZE + 3 ];

//*****************************************************************************
//
// PDM initialization.
//
//*****************************************************************************
void
pdm_init(void)
{
    //
    // Initialize, power-up, and configure the PDM.
    //
    am_hal_pdm_initialize(PDM_MODULE, &PDMHandle);
    am_hal_pdm_power_control(PDMHandle, AM_HAL_PDM_POWER_ON, false);

#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P)
    // use external XTHS, not reference clock
    am_hal_mcuctrl_control(AM_HAL_MCUCTRL_CONTROL_EXTCLK32M_KICK_START, false);

    // Enable HFRC2
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_HFRC2_START, false);
    am_util_delay_us(500);      // wait for FLL to lock

    // set HF2ADJ for 24.576MHz output
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_HF2ADJ_ENABLE, false);
    am_util_delay_us(500);      // wait for adj to apply
#endif

    am_hal_pdm_configure(PDMHandle, &g_sPdmConfig);

    //
    // Configure the necessary pins.
    //
    am_hal_gpio_pincfg_t sPinCfg = {0};

    sPinCfg.GP.cfg_b.uFuncSel = PDM_DATA_GPIO_FUNC;
    am_hal_gpio_pinconfig(PDM_DATA_GPIO_PIN, sPinCfg);
    sPinCfg.GP.cfg_b.uFuncSel = PDM_CLK_GPIO_FUNC;
    am_hal_gpio_pinconfig(PDM_CLK_GPIO_PIN, sPinCfg);

    am_hal_pdm_fifo_threshold_setup(PDMHandle, FIFO_THRESHOLD_CNT);

    //
    // Configure and enable PDM interrupts (set up to trigger on DMA
    // completion).
    //
    am_hal_pdm_interrupt_enable(PDMHandle, (AM_HAL_PDM_INT_DERR
                                            | AM_HAL_PDM_INT_DCMP
                                            | AM_HAL_PDM_INT_UNDFL
                                            | AM_HAL_PDM_INT_OVF));

    NVIC_SetPriority(pdm_interrupts[PDM_MODULE], AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(pdm_interrupts[PDM_MODULE]);
}

//*****************************************************************************
//
// PDM interrupt handler.
//
//*****************************************************************************
void
example_pdm_isr(void)
{
    uint32_t ui32Status;

#if DATA_VERIFY
    am_hal_gpio_output_set(PDM_ISR_TEST_PAD);
#endif

    //
    // Read the interrupt status.
    //
    am_hal_pdm_interrupt_status_get(PDMHandle, &ui32Status, true);
    am_hal_pdm_interrupt_clear(PDMHandle, ui32Status);

#if DATA_VERIFY
    am_hal_pdm_state_t *pState = (am_hal_pdm_state_t *) PDMHandle;
    static uint32_t ui32Switch = 0;
    if ( ui32Switch )
    {
        ui32Switch = 0;
        for (int i = 0; i < DMA_SIZE; i++)
        {
            ((uint32_t*)pState->ui32BufferPtr)[i] = (i & 0xFF) | 0xAB0000;
        }
    }
    else
    {
        ui32Switch = 1;
        for (int i = 0; i < DMA_SIZE; i++)
        {
            ((uint32_t*)pState->ui32BufferPtr)[i] = (i & 0xFF) | 0xCD0000;
        }
    }

    //am_util_stdio_printf("pdm isr addr = %x.\n", pState->ui32BufferPtr);
#endif

    //
    // interrupt service routine.
    //
    am_hal_pdm_interrupt_service(PDMHandle, ui32Status, &g_sTransferPDM);

    if (ui32Status & AM_HAL_PDM_INT_DCMP)
    {

        //am_util_stdio_printf("pdm isr.\n");
        g_bPDMDataReady = true;
    }

     if (ui32Status & AM_HAL_PDM_INT_OVF)
     {
        am_hal_pdm_fifo_count_get(PDMHandle);
        am_hal_pdm_fifo_flush(PDMHandle);

        g_ui32FifoOVFCount++;
     }

#if DATA_VERIFY
    am_hal_gpio_output_clear(PDM_ISR_TEST_PAD);
#endif
}

//*****************************************************************************
//
// i2s_init
//
//*****************************************************************************
void
i2s_init(void)
{
    //
    // When changing the I2S clock source, no matter whether HFRC2 is being used or not,
    // HFRC2 has to be enabled first to make clock source changing effective.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_HFRC2_START, false);
    am_util_delay_us(500);

    if ( (eAM_HAL_I2S_CLKSEL_XTHS_EXTREF_CLK <= g_sI2SConfig.eClock  && g_sI2SConfig.eClock <= eAM_HAL_I2S_CLKSEL_XTHS_500KHz) ) //enable EXTCLK32M
    {
        am_hal_mcuctrl_control(AM_HAL_MCUCTRL_CONTROL_EXTCLK32M_NORMAL, 0);
        am_util_delay_ms(200);
    }

    //
    // Configure the necessary pins.
    //
    am_hal_gpio_pincfg_t sPinCfg =
    {
      .GP.cfg_b.eGPOutCfg = 1,
      .GP.cfg_b.ePullup   = 0
    };

    sPinCfg.GP.cfg_b.uFuncSel = I2S_DATA_OUT_GPIO_FUNC;
    am_hal_gpio_pinconfig(I2S_DATA_OUT_GPIO_PIN, sPinCfg);
    sPinCfg.GP.cfg_b.uFuncSel = I2S_DATA_IN_GPIO_FUNC;
    am_hal_gpio_pinconfig(I2S_DATA_IN_GPIO_PIN, sPinCfg);

    sPinCfg.GP.cfg_b.uFuncSel = I2S_CLK_GPIO_FUNC;
    am_hal_gpio_pinconfig(I2S_CLK_GPIO_PIN, sPinCfg);
    sPinCfg.GP.cfg_b.uFuncSel = I2S_WS_GPIO_FUNC;
    am_hal_gpio_pinconfig(I2S_WS_GPIO_PIN, sPinCfg);

    am_hal_i2s_initialize(I2S_MODULE, &I2SHandle);
    am_hal_i2s_power_control(I2SHandle, AM_HAL_I2S_POWER_ON, false);
    am_hal_i2s_configure(I2SHandle, &g_sI2SConfig);
    am_hal_i2s_enable(I2SHandle);
}

//*****************************************************************************
//
// am_dspi2s0_isr
//
//*****************************************************************************
void
am_dspi2s0_isr()
{
    uint32_t ui32Status;
#if DATA_VERIFY
    am_hal_gpio_output_set(I2S_ISR_TEST_PAD);
#endif
    am_hal_i2s_interrupt_status_get(I2SHandle, &ui32Status, true);
    am_hal_i2s_interrupt_clear(I2SHandle, ui32Status);

    // I2S interrupt service
    am_hal_i2s_interrupt_service(I2SHandle, ui32Status, &g_sI2SConfig);

#if DATA_VERIFY
    am_hal_gpio_output_clear(I2S_ISR_TEST_PAD);
#endif
}

//*****************************************************************************
//
// pdm_deinit
//
//*****************************************************************************
void pdm_deinit(void *pHandle)
{
    am_hal_pdm_interrupt_clear(pHandle, (AM_HAL_PDM_INT_DERR
                                            | AM_HAL_PDM_INT_DCMP
                                            | AM_HAL_PDM_INT_UNDFL
                                            | AM_HAL_PDM_INT_OVF));

    am_hal_pdm_interrupt_disable(pHandle, (AM_HAL_PDM_INT_DERR
                                            | AM_HAL_PDM_INT_DCMP
                                            | AM_HAL_PDM_INT_UNDFL
                                            | AM_HAL_PDM_INT_OVF));

    NVIC_DisableIRQ(pdm_interrupts[PDM_MODULE]);

    am_hal_gpio_pinconfig(PDM_CLK_GPIO_PIN, am_hal_gpio_pincfg_disabled);
    am_hal_gpio_pinconfig(PDM_DATA_GPIO_PIN, am_hal_gpio_pincfg_disabled);

    am_hal_pdm_disable(pHandle);
    am_hal_pdm_power_control(pHandle, AM_HAL_PDM_POWER_OFF, false);

    am_hal_pdm_deinitialize(pHandle);
}

//*****************************************************************************
//
// i2s_deinit
//
//*****************************************************************************
void i2s_deinit(void *pHandle)
{
    am_hal_i2s_dma_transfer_complete(pHandle);

    am_hal_i2s_interrupt_clear(pHandle, (AM_HAL_I2S_INT_RXDMACPL | AM_HAL_I2S_INT_TXDMACPL));
    am_hal_i2s_interrupt_disable(pHandle, (AM_HAL_I2S_INT_RXDMACPL | AM_HAL_I2S_INT_TXDMACPL));

    NVIC_DisableIRQ(i2s_interrupts[I2S_MODULE]);

    am_hal_gpio_pinconfig(I2S_WS_GPIO_PIN, am_hal_gpio_pincfg_disabled);
    am_hal_gpio_pinconfig(I2S_CLK_GPIO_PIN, am_hal_gpio_pincfg_disabled);
    am_hal_gpio_pinconfig(I2S_DATA_OUT_GPIO_PIN, am_hal_gpio_pincfg_disabled);
    am_hal_gpio_pinconfig(I2S_DATA_IN_GPIO_PIN, am_hal_gpio_pincfg_disabled);

    am_hal_i2s_disable(pHandle);
    am_hal_i2s_power_control(pHandle, AM_HAL_I2S_POWER_OFF, false);

    am_hal_i2s_deinitialize(pHandle);
}

//*****************************************************************************
//
// Main
//
//*****************************************************************************
int
main(void)
{
    am_bsp_itm_printf_enable();

    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("==============================================\n");
    am_util_stdio_printf("PDM_I2S streaming example.\n\n");

    am_bsp_low_power_init();

#if DATA_VERIFY
    am_hal_gpio_pinconfig(PDM_ISR_TEST_PAD, am_hal_gpio_pincfg_output);
    am_hal_gpio_pinconfig(I2S_ISR_TEST_PAD, am_hal_gpio_pincfg_output);
    am_hal_gpio_output_clear(PDM_ISR_TEST_PAD);
    am_hal_gpio_output_clear(I2S_ISR_TEST_PAD);
#endif
#ifndef AM_PART_APOLLO4P
    //
    // Set up scratch AXI buf (needs 64B - aligned to 16 Bytes)
    //
    am_hal_daxi_control(AM_HAL_DAXI_CONTROL_AXIMEM, (uint8_t *)((uint32_t)(axiScratchBuf + 3) & ~0xF));
#endif
    //
    // PDM DMA data config:
    //   DMA buffers padded at 16B alignment.
    //
    uint32_t ui32PDMDataPtr = (uint32_t)((uint32_t)(g_ui32PingPongBuffer + 3) & ~0xF);
    g_sTransferPDM.ui32TargetAddr = ui32PDMDataPtr;
    g_sTransferPDM.ui32TargetAddrReverse = g_sTransferPDM.ui32TargetAddr + g_sTransferPDM.ui32TotalCount;

    //
    // Initialize PDM-to-PCM module
    //
    pdm_init();
    am_hal_pdm_enable(PDMHandle);

    //
    // Initialize I2S.
    //
    i2s_init();
    NVIC_SetPriority(i2s_interrupts[I2S_MODULE], AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(i2s_interrupts[I2S_MODULE]);
    am_hal_interrupt_master_enable();

    //
    // Start PDM streaming.
    //
    am_hal_pdm_dma_start(PDMHandle, &g_sTransferPDM);

    //Avoid interrupt coming simultaneously.
    am_util_delay_ms(5);

    // use the reverser buffer of PDM
    g_sTransferI2S.ui32TxTargetAddr = am_hal_pdm_dma_get_buffer(PDMHandle);
    g_sTransferI2S.ui32TxTargetAddrReverse = (g_sTransferI2S.ui32TxTargetAddr == g_sTransferPDM.ui32TargetAddr)? g_sTransferPDM.ui32TargetAddrReverse: g_sTransferPDM.ui32TargetAddr;
    //Start I2S data transaction.
    am_hal_i2s_dma_configure(I2SHandle, &g_sI2SConfig, &g_sTransferI2S);
    am_hal_i2s_dma_transfer_start(I2SHandle, &g_sI2SConfig);

    //
    // Loop forever while sleeping.
    //
    while (1)
    {
        //
        // Go to Deep Sleep.
        //
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
    }
}

