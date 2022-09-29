//*****************************************************************************
//
//! @file audadc_rtt_stream.c
//!
//! @brief This example uses AUDADC to capture and send audio data to PC via SEGGER RTT.
//!
//! Purpose: This example uses AUDADC INTTRIGTIMER to capture audio samples at 16 kHz
//!          LPMODE1 is used for power efficiency
//!          DMA is used to transfer samples from the AUDADC FIFO into an SRAM buffer
//!
//! Additional Information:
//! Please set AUDADC_EXAMPLE_DEBUG to 1 when running in debug (rtt) mode.
//!
//! @RTT Streaming: Run rtt_logger.py to capture pcm raw data via PC.
//!                 Should modify -RTTAddress in rtt_logger.py to _SEGGER_RTT address in audadc_rtt_stream.map
//!                 The data saved as stereo(L:low gain/R: high gain)
//!
//! @pcm_to_wav.py:  Convert pcm raw data to wav file.
//!                  Save mono left channel to destination file.
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

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "SEGGER_RTT.h"

//*****************************************************************************
//
// Insert compiler version at compile time.
//
//*****************************************************************************
#define STRINGIZE_VAL(n)                    STRINGIZE_VAL2(n)
#define STRINGIZE_VAL2(n)                   #n

#ifdef __GNUC__
#define COMPILER_VERSION                    ("GCC " __VERSION__)
#elif defined(__ARMCC_VERSION)
#define COMPILER_VERSION                    ("ARMCC " STRINGIZE_VAL(__ARMCC_VERSION))
#elif defined(__KEIL__)
#define COMPILER_VERSION                    "KEIL_CARM " STRINGIZE_VAL(__CA__)
#elif defined(__IAR_SYSTEMS_ICC__)
#define COMPILER_VERSION                    __VERSION__
#else
#define COMPILER_VERSION                    "Compiler unknown"
#endif

//
// Select AUDADC clock source
// When switching clock source from XTHS to HFRC/HFRC2,
// please power cycle MCU, then run the bin with new clock source.
//
#define XTHS  0
#define HFRC  1
#define HFRC2 2
#define HFRC2_ADJ 3
#define CLK_SRC HFRC2_ADJ

#define AUDADC_EXAMPLE_DEBUG 0 // Please set this to 1 when running in debug (rtt) mode.

//
// 0 - DMA buffer read, 1 - FIFO read
//
#define FIFO_READ            0
#if defined(AM_PART_APOLLO4P)
    #define DC_OFFSET_CAL
#endif

//*****************************************************************************
//
// AUDADC Sample buffer.
//
//*****************************************************************************
#define AUDADC_SAMPLE_BUF_SIZE      (2400)    // Should be padded to 12 samples follow current DMA/FIFO mechanism:
                                              // DMA trigger on FIFO 75% full
#define AUDADC_DATA_BUFFER_SIZE     (2*AUDADC_SAMPLE_BUF_SIZE)

AM_SHARED_RW uint32_t g_ui32AUDADCSampleBuffer[AUDADC_SAMPLE_BUF_SIZE + AUDADC_SAMPLE_BUF_SIZE + 3];

AM_SHARED_RW int16_t g_in16AudioDataBuffer[AUDADC_DATA_BUFFER_SIZE];

am_hal_audadc_sample_t sLGSampleBuffer[AUDADC_SAMPLE_BUF_SIZE];
am_hal_audadc_sample_t sHGSampleBuffer[AUDADC_SAMPLE_BUF_SIZE];
//
// RTT streaming buffer
//
#if AUDADC_EXAMPLE_DEBUG
#define TIMEOUT                     400000      // RTT streaming timeout loop count
#define RTT_BUFFER_LENGTH           (256*1024)
#define AUDIO_SAMPLE_TO_RTT         (256*1024)
uint8_t g_rttRecorderBuffer[RTT_BUFFER_LENGTH];
AM_SHARED_RW int16_t g_in16SampleToRTT[AUDIO_SAMPLE_TO_RTT];
uint32_t g_ui32SampleToRTT = 0;
#endif

//
// AUDADC Device Handle.
//
static void *g_AUDADCHandle;

//
// AUDADC DMA complete flag.
//
volatile bool g_bAUDADCDMAComplete;

//
// AUDADC DMA error flag.
//
volatile bool g_bAUDADCDMAError;
#ifndef AM_PART_APOLLO4P
// AXI Scratch buffer
// Need to allocate 20 Words even though we only need 16, to ensure we have 16 Byte alignment
AM_SHARED_RW uint32_t axiScratchBuf[20];
#endif
//*****************************************************************************
//
// AUDADC gain configuration information.
//
//*****************************************************************************
#define CH_A0_EN  1
#define CH_A1_EN  1
#define CH_B0_EN  1
#define CH_B1_EN  1

#define PREAMP_FULL_GAIN    12    // Enable op amps for full gain range
#define CH_A0_GAIN_DB       12
#define CH_A1_GAIN_DB       12
#define CH_B0_GAIN_DB       12
#define CH_B1_GAIN_DB       12

am_hal_audadc_gain_config_t g_sAudadcGainConfig =
{
    .ui32LGA        = 0,        // 0 code
    .ui32HGADELTA   = 0,        // delta from the LGA field
    .ui32LGB        = 0,        // 0 code
    .ui32HGBDELTA   = 0,        // delta from the LGB field

    .eUpdateMode    = AM_HAL_AUDADC_GAIN_UPDATE_IMME,
};

//
// Configure the AUDADC to use DMA for the sample transfer.
//
am_hal_audadc_dma_config_t g_sAUDADCDMAConfig =
{
    .bDynamicPriority  = true,
    .ePriority         = AM_HAL_AUDADC_PRIOR_SERVICE_IMMED,
    .bDMAEnable        = true,
    .ui32SampleCount   = AUDADC_SAMPLE_BUF_SIZE,
    .ui32TargetAddress = 0x0,
    .ui32TargetAddressReverse = 0x0,
};

//
// helper function.
//
#if AUDADC_EXAMPLE_DEBUG
void pcm_rtt_record_slow(void* pBuffer, uint32_t NumBytes)
{
    uint32_t timeout = TIMEOUT;
    uint32_t bytesEveryTime = 512;
    uint32_t bytes_stored;
    int32_t u32Bytes = NumBytes;
    void* data = pBuffer;

    for (; u32Bytes >= 0; u32Bytes -= bytesEveryTime )
    {
        while (timeout--);
        timeout = TIMEOUT;

        bytes_stored = SEGGER_RTT_Write(1, data, bytesEveryTime);
        while ((bytes_stored != bytesEveryTime));

        data = (void*) ((uint32_t)data + bytesEveryTime);
    }
}
#endif

//*****************************************************************************
//
// Set up the core for sleeping, and then go to sleep.
//
//*****************************************************************************
void
sleep(void)
{
    //
    // Go to Deep Sleep.
    //
    am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);

}

//*****************************************************************************
//
// Configure the AUDADC SLOT.
//
//*****************************************************************************
void
audadc_slot_config(void)
{
    am_hal_audadc_slot_config_t      AUDADCSlotConfig;

    //
    // Set up an AUDADC slot
    //
    AUDADCSlotConfig.eMeasToAvg      = AM_HAL_AUDADC_SLOT_AVG_1;
    AUDADCSlotConfig.ePrecisionMode  = AM_HAL_AUDADC_SLOT_12BIT;
#if (CLK_SRC == XTHS)
    AUDADCSlotConfig.ui32TrkCyc      = 24;
#else
    AUDADCSlotConfig.ui32TrkCyc      = 34;
#endif
    AUDADCSlotConfig.eChannel        = AM_HAL_AUDADC_SLOT_CHSEL_SE0;
    AUDADCSlotConfig.bWindowCompare  = false;
    AUDADCSlotConfig.bEnabled        = true;

#if CH_A0_EN
    if (AM_HAL_STATUS_SUCCESS != am_hal_audadc_configure_slot(g_AUDADCHandle, 0, &AUDADCSlotConfig))
    {
        am_util_stdio_printf("Error - configuring AUDADC Slot 0 failed.\n");
    }
#endif
#if CH_A1_EN
    AUDADCSlotConfig.eChannel        = AM_HAL_AUDADC_SLOT_CHSEL_SE1;
    if (AM_HAL_STATUS_SUCCESS != am_hal_audadc_configure_slot(g_AUDADCHandle, 1, &AUDADCSlotConfig))
    {
        am_util_stdio_printf("Error - configuring AUDADC Slot 1 failed.\n");
    }
#endif
#if CH_B0_EN
    AUDADCSlotConfig.eChannel        = AM_HAL_AUDADC_SLOT_CHSEL_SE2;
    if (AM_HAL_STATUS_SUCCESS != am_hal_audadc_configure_slot(g_AUDADCHandle, 2, &AUDADCSlotConfig))
    {
        am_util_stdio_printf("Error - configuring AUDADC Slot 2 failed.\n");
    }
#endif
#if CH_B1_EN
    AUDADCSlotConfig.eChannel        = AM_HAL_AUDADC_SLOT_CHSEL_SE3;
    if (AM_HAL_STATUS_SUCCESS != am_hal_audadc_configure_slot(g_AUDADCHandle, 3, &AUDADCSlotConfig))
    {
        am_util_stdio_printf("Error - configuring AUDADC Slot 3 failed.\n");
    }
#endif
}

//*****************************************************************************
//
// Configure the AUDADC.
//
//*****************************************************************************
void
audadc_config(void)
{
    //
    // Set up the AUDADC configuration parameters. These settings are reasonable
    // for accurate measurements at a low sample rate.
    //
    am_hal_audadc_config_t           AUDADCConfig =
    {
#if (CLK_SRC == XTHS)
        .eClock             = AM_HAL_AUDADC_CLKSEL_XTHS_24MHz,
#elif (CLK_SRC == HFRC)
        .eClock             = AM_HAL_AUDADC_CLKSEL_HFRC_48MHz,
#elif (CLK_SRC == HFRC2) || (CLK_SRC == HFRC2_ADJ)
        .eClock             = AM_HAL_AUDADC_CLKSEL_HFRC2_48MHz,
#else
        #warning Invalid clock source.
#endif
        .ePolarity          = AM_HAL_AUDADC_TRIGPOL_RISING,
        .eTrigger           = AM_HAL_AUDADC_TRIGSEL_SOFTWARE,
        .eClockMode         = AM_HAL_AUDADC_CLKMODE_LOW_LATENCY,
        .ePowerMode         = AM_HAL_AUDADC_LPMODE1,
        .eRepeat            = AM_HAL_AUDADC_REPEATING_SCAN,
        .eRepeatTrigger     = AM_HAL_AUDADC_RPTTRIGSEL_INT,
        .eSampMode          = AM_HAL_AUDADC_SAMPMODE_MED,       //AM_HAL_AUDADC_SAMPMODE_LP,
    };

    //
    // Set up internal repeat trigger timer
    //
    am_hal_audadc_irtt_config_t      AUDADCIrttConfig =
    {
        .bIrttEnable        = true,
        .eClkDiv            = AM_HAL_AUDADC_RPTT_CLK_DIV32,
        //
        // Adjust sample rate to around 16K.
        // sample rate = eClock/eClkDiv/(ui32IrttCountMax+1)
        //
#if (CLK_SRC == XTHS) // On Apollo4P_EB, XTHS is 32 MHZ.
        .ui32IrttCountMax   = 62,
#elif (CLK_SRC == HFRC)
        .ui32IrttCountMax   = 93,
#elif (CLK_SRC == HFRC2)
        .ui32IrttCountMax   = 93,
#elif (CLK_SRC == HFRC2_ADJ)
        .ui32IrttCountMax   = 47,
#else
        #warning Invalid clock source.
#endif
    };

    //
    // Initialize the AUDADC and get the handle.
    //
    if ( AM_HAL_STATUS_SUCCESS != am_hal_audadc_initialize(0, &g_AUDADCHandle) )
    {
        am_util_stdio_printf("Error - reservation of the AUDADC instance failed.\n");
    }
    //
    // Power on the AUDADC.
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_audadc_power_control(g_AUDADCHandle,
                                                          AM_HAL_SYSCTRL_WAKE,
                                                          false) )
    {
        am_util_stdio_printf("Error - AUDADC power on failed.\n");
    }
#if defined(AM_PART_APOLLO4) || defined(AM_PART_APOLLO4B)
    #if (CLK_SRC == HFRC2) || (CLK_SRC == HFRC2_ADJ)
        //
        //Enable hfrc2.
        //
        am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_HFRC2_START, false);
        am_util_delay_us(200);
    #endif
#endif
#if (CLK_SRC == HFRC2_ADJ)
        //
        // hfrc2 adj.
        //
        am_hal_mcuctrl_control(AM_HAL_MCUCTRL_CONTROL_EXTCLK32M_KICK_START, false);
        am_util_delay_us(1500);
        //
        // set HF2ADJ for 24.576MHz output
        //
        am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_HF2ADJ_ENABLE, false);

        am_util_delay_us(500);      // wait for adj to apply
#endif

    if ( AUDADCConfig.eClock == AM_HAL_AUDADC_CLKSEL_XTHS_24MHz )
    {
        am_hal_mcuctrl_control(AM_HAL_MCUCTRL_CONTROL_EXTCLK32M_NORMAL, 0);
        am_util_delay_us(1500);
    }

    if (AM_HAL_STATUS_SUCCESS != am_hal_audadc_configure(g_AUDADCHandle, &AUDADCConfig))
    {
        am_util_stdio_printf("Error - configuring AUDADC failed.\n");
    }

    //
    // Set up internal repeat trigger timer
    //
    am_hal_audadc_configure_irtt(g_AUDADCHandle, &AUDADCIrttConfig);

    //
    // Enable the AUDADC.
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_audadc_enable(g_AUDADCHandle))
    {
        am_util_stdio_printf("Error - enabling AUDADC failed.\n");
    }

    //
    // Enable internal repeat trigger timer
    //
    am_hal_audadc_irtt_enable(g_AUDADCHandle);

    //
    // Configure the AUDADC to use DMA for the sample transfer.
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_audadc_configure_dma(g_AUDADCHandle, &g_sAUDADCDMAConfig))
    {
        am_util_stdio_printf("Error - configuring AUDADC DMA failed.\n");
    }

    //
    // For this example, the samples will be coming in slowly. This means we
    // can afford to wake up for every conversion.
    //
    am_hal_audadc_interrupt_enable(g_AUDADCHandle, AM_HAL_AUDADC_INT_FIFOOVR1 | AM_HAL_AUDADC_INT_FIFOOVR2 | AM_HAL_AUDADC_INT_DERR | AM_HAL_AUDADC_INT_DCMP ); //| AM_HAL_AUDADC_INT_CNVCMP | AM_HAL_AUDADC_INT_SCNCMP);
}

//*****************************************************************************
//
// Interrupt handler for the AUDADC.
//
//*****************************************************************************
void
am_audadc0_isr(void)
{
    uint32_t ui32IntMask;
    //
    // Read the interrupt status.
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_audadc_interrupt_status(g_AUDADCHandle, &ui32IntMask, false))
    {
        am_util_stdio_printf("Error reading AUDADC interrupt status\n");
    }

    //
    // Clear the AUDADC interrupt.
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_audadc_interrupt_clear(g_AUDADCHandle, ui32IntMask))
    {
        am_util_stdio_printf("Error clearing AUDADC interrupt status\n");
    }

    //
    // If we got a DMA complete, set the flag.
    //
    if (ui32IntMask & AM_HAL_AUDADC_INT_FIFOOVR1)
    {
        if ( AUDADCn(0)->DMASTAT_b.DMACPL )
        {
            g_bAUDADCDMAError = false;
            am_hal_audadc_interrupt_service(g_AUDADCHandle, &g_sAUDADCDMAConfig);

            g_bAUDADCDMAComplete = true;
        }
    }

    //
    // If we got a DMA error, set the flag.
    //
    if ( ui32IntMask & AM_HAL_AUDADC_INT_DERR )
    {
        g_bAUDADCDMAError = true;
    }
}

//*****************************************************************************
//
// Main function.
//
//*****************************************************************************
int
main(void)
{
#if defined(AM_PART_APOLLO4P) && defined(DC_OFFSET_CAL)
    am_hal_offset_cal_coeffs_array_t sOffsetCalib;
    #if AUDADC_EXAMPLE_DEBUG
        int32_t i32OffsetAdj = 0;
        uint16_t ui16AudChannel = 0;
    #endif
#endif
    //
    // Start the ITM interface.
    //
    am_bsp_itm_printf_enable();

    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("==============================================\n");
    am_util_stdio_printf("AUDADC RTT streaming example.\n\n");

    //
    // Initialize RTT
    //
#if AUDADC_EXAMPLE_DEBUG
    SEGGER_RTT_Init();
    SEGGER_RTT_ConfigUpBuffer(1, "DataLogger", g_rttRecorderBuffer, RTT_BUFFER_LENGTH, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
#endif

    //
    // Print compiler version and rtt control block address
    //
    am_util_stdio_printf("App Compiler:    %s\n", COMPILER_VERSION);
#if AUDADC_EXAMPLE_DEBUG
    am_util_stdio_printf("RTT Control Block Address:  0x%08X\n", (uint32_t)&_SEGGER_RTT);
    am_util_stdio_printf("Disable SWO print and starting RTT streaming...\n");
#endif
    am_util_stdio_printf("==============================================\n");
    am_bsp_debug_printf_disable();

    //
    // Configure the board for low power.
    //
    am_bsp_low_power_init();
#ifndef AM_PART_APOLLO4P
    //
    // Set up scratch AXI buf (needs 64B - aligned to 16 Bytes)
    //
    am_hal_daxi_control(AM_HAL_DAXI_CONTROL_AXIMEM, (uint8_t *)((uint32_t)(axiScratchBuf + 3) & ~0xF));
#endif
    //
    // AUDADC DMA data config:
    //   DMA buffers padded at 16B alignment.
    //
    uint32_t ui32AUDADCDataPtr = (uint32_t)((uint32_t)(g_ui32AUDADCSampleBuffer + 3) & ~0xF);
    g_sAUDADCDMAConfig.ui32TargetAddress = ui32AUDADCDataPtr;
    g_sAUDADCDMAConfig.ui32TargetAddressReverse = g_sAUDADCDMAConfig.ui32TargetAddress + sizeof(uint32_t)* g_sAUDADCDMAConfig.ui32SampleCount;

    //
    // Power up PrePGA
    //
    am_hal_audadc_refgen_powerup();

    am_hal_audadc_pga_powerup(0);
    am_hal_audadc_pga_powerup(1);
    am_hal_audadc_pga_powerup(2);
    am_hal_audadc_pga_powerup(3);

    am_hal_audadc_gain_set(0, 2*PREAMP_FULL_GAIN);
    am_hal_audadc_gain_set(1, 2*PREAMP_FULL_GAIN);
    am_hal_audadc_gain_set(2, 2*PREAMP_FULL_GAIN);
    am_hal_audadc_gain_set(3, 2*PREAMP_FULL_GAIN);

    //
    //  Turn on mic bias
    //
    am_hal_audadc_micbias_powerup(24);
    am_util_delay_ms(400);

    //
    // Configure the AUDADC
    //
    audadc_config();

    // Gain setting
    g_sAudadcGainConfig.ui32LGA = (uint32_t)((float)CH_A0_GAIN_DB*2 + 12);
    g_sAudadcGainConfig.ui32HGADELTA = ((uint32_t)((float)CH_A1_GAIN_DB*2 + 12)) - g_sAudadcGainConfig.ui32LGA;
    g_sAudadcGainConfig.ui32LGB = (uint32_t)((float)CH_B0_GAIN_DB*2 + 12);
    g_sAudadcGainConfig.ui32HGBDELTA = ((uint32_t)((float)CH_B1_GAIN_DB*2 + 12)) - g_sAudadcGainConfig.ui32LGB;
    g_sAudadcGainConfig.eUpdateMode = AM_HAL_AUDADC_GAIN_UPDATE_IMME;
    am_hal_audadc_internal_pga_config(g_AUDADCHandle, &g_sAudadcGainConfig);

    // Configure the slot
    audadc_slot_config();
#if defined(AM_PART_APOLLO4P) && defined(DC_OFFSET_CAL)
    //
    // Calculate DC offset calibartion parameter.
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_audadc_slot_dc_offset_calculate(g_AUDADCHandle, 4, &sOffsetCalib))
    {
        am_util_stdio_printf("Error - failed to calculate offset calibartion parameter.\n");
    }
#endif
    NVIC_SetPriority(AUDADC0_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(AUDADC0_IRQn);
    am_hal_interrupt_master_enable();

    //
    // Trigger the AUDADC sampling for the first time manually.
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_audadc_sw_trigger(g_AUDADCHandle))
    {
        am_util_stdio_printf("Error - triggering the AUDADC failed.\n");
    }

    //
    // Loop forever.
    //
    while (1)
    {
        //
        // Go to Deep Sleep.
        //
#if (0 == AUDADC_EXAMPLE_DEBUG)
    //
    // Do not enter deepsleep when using HFRC2_ADJ or XTHS,
    // because XTHS doesn't work in deepsleep mode.
    //
    #if (CLK_SRC != HFRC2_ADJ) && (CLK_SRC != XTHS)
        if (!g_bAUDADCDMAComplete)
        {
            sleep();
        }
    #endif
#endif

        // Check for DMA errors.
        //
        if (g_bAUDADCDMAError)
        {
            am_util_stdio_printf("DMA Error occured\n");
            while (1);
        }

        //
        // Check if the AUDADC DMA completion interrupt occurred.
        //
        if (g_bAUDADCDMAComplete)
        {
            uint32_t* pui32_buffer = (uint32_t* )am_hal_audadc_dma_get_buffer(g_AUDADCHandle);
#if AUDADC_EXAMPLE_DEBUG
            //
            //DMA read.
            //
            uint32_t  ui32SampleCount = AUDADC_SAMPLE_BUF_SIZE;
            for (int i = 0; i < ui32SampleCount; i++)
            {
                g_in16AudioDataBuffer[2 * i]     = (int16_t)(pui32_buffer[i] & 0x0000FFF0);          // Low-gain PGA sample data.
                g_in16AudioDataBuffer[2 * i + 1] = (int16_t)((pui32_buffer[i] >> 16) & 0x0000FFF0);  // High-gain PGA sample data.
                ui16AudChannel = (uint16_t)((pui32_buffer[i] & 0x00080000) >> 19);
    #if defined(AM_PART_APOLLO4P) && defined(DC_OFFSET_CAL)
                //
                // Apply dc offset calibration to debug mode.
                //
                if (sOffsetCalib.sCalibCoeff[ui16AudChannel * 2].bValid)
                {
                    i32OffsetAdj = sOffsetCalib.sCalibCoeff[ui16AudChannel * 2].i32DCOffsetAdj << 4;
                    if ((g_in16AudioDataBuffer[2 * i] >= 0) &&
                        (i32OffsetAdj > (32767 - g_in16AudioDataBuffer[2 * i])))
                    {
                        g_in16AudioDataBuffer[2 * i] = 32767; // Saturation
                    }
                    else if ((g_in16AudioDataBuffer[2 * i] < 0) &&
                             (i32OffsetAdj < (-32768 - g_in16AudioDataBuffer[2 * i])))
                    {
                        g_in16AudioDataBuffer[2 * i] = -32768; // Saturation
                    }
                    else
                    {
                        g_in16AudioDataBuffer[2 * i] += i32OffsetAdj;
                    }
                }
                if (sOffsetCalib.sCalibCoeff[ui16AudChannel * 2 + 1].bValid)
                {
                    i32OffsetAdj = sOffsetCalib.sCalibCoeff[ui16AudChannel * 2 + 1].i32DCOffsetAdj << 4;
                    if ((g_in16AudioDataBuffer[2 * i + 1] >= 0) &&
                        (i32OffsetAdj > (32767 - g_in16AudioDataBuffer[2 * i + 1])))
                    {
                        g_in16AudioDataBuffer[2 * i + 1] = 32767; // Saturation
                    }
                    else if ((g_in16AudioDataBuffer[2 * i + 1] < 0) &&
                             (i32OffsetAdj < (-32768 - g_in16AudioDataBuffer[2 * i + 1])))
                    {
                        g_in16AudioDataBuffer[2 * i + 1] = -32768; // Saturation
                    }
                    else
                    {
                        g_in16AudioDataBuffer[2 * i + 1] += i32OffsetAdj;
                    }
                }
    #endif
            }

            SEGGER_RTT_Write(1, g_in16AudioDataBuffer, AUDADC_SAMPLE_BUF_SIZE*2*sizeof(int16_t));
#endif
#if FIFO_READ // read from FIFO
            uint32_t ui32SampleCount2 = AUDADC_SAMPLE_BUF_SIZE;
    #if defined(AM_PART_APOLLO4P) && defined(DC_OFFSET_CAL)
            am_hal_audadc_samples_read(g_AUDADCHandle,
                                       NULL,
                                       &ui32SampleCount2,
                                       true, &sLGSampleBuffer[0],
                                       true, &sHGSampleBuffer[0],
                                       &sOffsetCalib);
    #elif defined(AM_PART_APOLLO4P)
            am_hal_audadc_samples_read(g_AUDADCHandle,
                                       NULL,
                                       &ui32SampleCount2,
                                       true, &sLGSampleBuffer[0],
                                       true, &sHGSampleBuffer[0],
                                       NULL);
    #else
            am_hal_audadc_samples_read(g_AUDADCHandle,
                                       NULL,
                                       &ui32SampleCount2,
                                       true, &sLGSampleBuffer[0],
                                       true, &sHGSampleBuffer[0]);
    #endif
#else // read from DMA buffer
            uint32_t ui32SampleCount2 = AUDADC_SAMPLE_BUF_SIZE;
    #if defined(AM_PART_APOLLO4P) && defined(DC_OFFSET_CAL)
            am_hal_audadc_samples_read(g_AUDADCHandle,
                                       pui32_buffer,
                                       &ui32SampleCount2,
                                       true, &sLGSampleBuffer[0],
                                       true, &sHGSampleBuffer[0],
                                       &sOffsetCalib);
    #elif defined(AM_PART_APOLLO4P)
            am_hal_audadc_samples_read(g_AUDADCHandle,
                                       pui32_buffer,
                                       &ui32SampleCount2,
                                       true, &sLGSampleBuffer[0],
                                       true, &sHGSampleBuffer[0],
                                       NULL);
    #else
            am_hal_audadc_samples_read(g_AUDADCHandle,
                                       pui32_buffer,
                                       &ui32SampleCount2,
                                       true, &sLGSampleBuffer[0],
                                       true, &sHGSampleBuffer[0]);
    #endif
#endif
            //
            // Reset the DMA completion and error flags.
            //
            g_bAUDADCDMAComplete = false;

            //
            // Clear the AUDADC interrupts.
            //
            if (AM_HAL_STATUS_SUCCESS != am_hal_audadc_interrupt_clear(g_AUDADCHandle, 0xFFFFFFFF))
            {
                am_util_stdio_printf("Error - clearing the AUDADC interrupts failed.\n");
            }

        }
    }
}
