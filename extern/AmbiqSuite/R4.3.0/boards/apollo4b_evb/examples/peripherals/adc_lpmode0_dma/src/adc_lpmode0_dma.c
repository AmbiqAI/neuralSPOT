//*****************************************************************************
//
//! @file adc_lpmode0_dma.c
//!
//! @brief This example takes samples with the ADC at high-speed using DMA.
//!
//! Purpose: This example shows the CTIMER-A3 triggering repeated samples of an external
//! input at 1.2Msps in LPMODE0.  The example uses the CTIMER-A3 to trigger
//! ADC sampling.  Each data point is 128 sample average and is transferred
//! from the ADC FIFO into an SRAM buffer using DMA.
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

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

//*****************************************************************************
//
// Define a circular buffer to hold the ADC samples
//
//*****************************************************************************
#define ADC_EXAMPLE_DEBUG   1

//
// ADC Sample buffer.
//
#define ADC_SAMPLE_BUF_SIZE 128

#if (defined (__ARMCC_VERSION)) && (__ARMCC_VERSION < 6000000)
__align(16) uint32_t g_ui32ADCSampleBuffer[ADC_SAMPLE_BUF_SIZE];
#elif (defined (__ARMCC_VERSION)) && (__ARMCC_VERSION >= 6000000)
uint32_t g_ui32ADCSampleBuffer[ADC_SAMPLE_BUF_SIZE]  __attribute__((aligned(16)));

#elif defined(__GNUC_STDC_INLINE__)
uint32_t g_ui32ADCSampleBuffer[ADC_SAMPLE_BUF_SIZE]  __attribute__ ((aligned (16)));
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment = 16
uint32_t g_ui32ADCSampleBuffer[ADC_SAMPLE_BUF_SIZE];
#else
#error Unknown compiler.
#endif

am_hal_adc_sample_t SampleBuffer[ADC_SAMPLE_BUF_SIZE];

//
// ADC Device Handle.
//
static void *g_ADCHandle;

//
// ADC DMA complete flag.
//
volatile bool                   g_bADCDMAComplete;

//
// ADC DMA error flag.
//
volatile bool                   g_bADCDMAError;

volatile uint32_t g_ui32AdcFifoCount = 0;

#if defined(AM_PART_APOLLO4B)
// AXI Scratch buffer
// Need to allocate 20 Words even though we only need 16, to ensure we have 16 Byte alignment
AM_SHARED_RW uint32_t axiScratchBuf[20];
#endif

//
// Define the ADC SE0 pin to be used.
//
const am_hal_gpio_pincfg_t g_AM_PIN_19_ADCSE0 =
{
    .GP.cfg_b.uFuncSel       = AM_HAL_PIN_19_ADCSE0,
};

//*****************************************************************************
//
// Interrupt handler for the ADC.
//
//*****************************************************************************
void
am_adc_isr(void)
{
    uint32_t ui32IntMask;
    //
    // Read the interrupt status.
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_interrupt_status(g_ADCHandle, &ui32IntMask, false))
    {
        am_util_stdio_printf("Error reading ADC interrupt status\n");
    }

    //
    // Clear the ADC interrupt.
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_interrupt_clear(g_ADCHandle, ui32IntMask))
    {
        am_util_stdio_printf("Error clearing ADC interrupt status\n");
    }

    //
    // If we got a DMA complete, set the flag.
    //
    if (ui32IntMask & AM_HAL_ADC_INT_FIFOOVR1)
    {
        if ( ADCn(0)->DMASTAT_b.DMACPL )
        {
            g_bADCDMAComplete = true;
        }
    }

    //
    // If we got a DMA error, set the flag.
    //
    if ( ui32IntMask & AM_HAL_ADC_INT_DERR )
    {
        g_bADCDMAError = true;
    }
}

//*****************************************************************************
//
// Set up the core for sleeping, and then go to sleep.
//
//*****************************************************************************
void
sleep(void)
{
    //
    // Disable things that can't run in sleep mode.
    //
#if (0 == ADC_EXAMPLE_DEBUG)
    am_bsp_debug_printf_disable();
#endif

    //
    // Go to Deep Sleep.
    //
    am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);

    //
    // Re-enable peripherals for run mode.
    //
#if (0 == ADC_EXAMPLE_DEBUG)
    am_bsp_debug_printf_enable();
#endif
}

//*****************************************************************************
//
// Configure the ADC.
//
//*****************************************************************************
void
adc_config_dma(void)
{
    am_hal_adc_dma_config_t       ADCDMAConfig;

    //
    // Configure the ADC to use DMA for the sample transfer.
    //
    ADCDMAConfig.bDynamicPriority = true;
    ADCDMAConfig.ePriority = AM_HAL_ADC_PRIOR_SERVICE_IMMED;
    ADCDMAConfig.bDMAEnable = true;
    ADCDMAConfig.ui32SampleCount = ADC_SAMPLE_BUF_SIZE;
    ADCDMAConfig.ui32TargetAddress = (uint32_t)g_ui32ADCSampleBuffer;
    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_configure_dma(g_ADCHandle, &ADCDMAConfig))
    {
        am_util_stdio_printf("Error - configuring ADC DMA failed.\n");
    }

    //
    // Reset the ADC DMA flags.
    //
    g_bADCDMAComplete = false;
    g_bADCDMAError = false;
}

//*****************************************************************************
//
// Configure the ADC.
//
//*****************************************************************************
void
adc_config(void)
{
    am_hal_adc_config_t           ADCConfig;
    am_hal_adc_slot_config_t      ADCSlotConfig;

    //
    // Initialize the ADC and get the handle.
    //
    if ( AM_HAL_STATUS_SUCCESS != am_hal_adc_initialize(0, &g_ADCHandle) )
    {
        am_util_stdio_printf("Error - reservation of the ADC instance failed.\n");
    }

    //
    // Power on the ADC.
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_power_control(g_ADCHandle,
                                                          AM_HAL_SYSCTRL_WAKE,
                                                          false) )
    {
        am_util_stdio_printf("Error - ADC power on failed.\n");
    }

    //
    // Set up internal repeat trigger timer
    //
    am_hal_adc_irtt_config_t      ADCIrttConfig =
    {
        .bIrttEnable        = true,
        .eClkDiv            = AM_HAL_ADC_RPTT_CLK_DIV16,
        .ui32IrttCountMax   = 30,
    };

    am_hal_adc_configure_irtt(g_ADCHandle, &ADCIrttConfig);

    //
    // Set up the ADC configuration parameters. These settings are reasonable
    // for accurate measurements at a low sample rate.
    //
    ADCConfig.eClock             = AM_HAL_ADC_CLKSEL_HFRC_24MHZ;
    ADCConfig.ePolarity          = AM_HAL_ADC_TRIGPOL_RISING;
    ADCConfig.eTrigger           = AM_HAL_ADC_TRIGSEL_SOFTWARE;
    ADCConfig.eClockMode         = AM_HAL_ADC_CLKMODE_LOW_LATENCY;
    ADCConfig.ePowerMode         = AM_HAL_ADC_LPMODE0;
    ADCConfig.eRepeat            = AM_HAL_ADC_REPEATING_SCAN;
    ADCConfig.eRepeatTrigger     = AM_HAL_ADC_RPTTRIGSEL_INT;
    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_configure(g_ADCHandle, &ADCConfig))
    {
        am_util_stdio_printf("Error - configuring ADC failed.\n");
    }

    //
    // Set up an ADC slot
    //
    ADCSlotConfig.eMeasToAvg      = AM_HAL_ADC_SLOT_AVG_128;
    ADCSlotConfig.ui32TrkCyc      = AM_HAL_ADC_MIN_TRKCYC;
    ADCSlotConfig.ePrecisionMode  = AM_HAL_ADC_SLOT_12BIT;
    ADCSlotConfig.eChannel        = AM_HAL_ADC_SLOT_CHSEL_SE0;
    ADCSlotConfig.bWindowCompare  = false;
    ADCSlotConfig.bEnabled        = true;
    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_configure_slot(g_ADCHandle, 0, &ADCSlotConfig))
    {
        am_util_stdio_printf("Error - configuring ADC Slot 0 failed.\n");
    }

    //
    // Configure the ADC to use DMA for the sample transfer.
    //
    adc_config_dma();

    //
    // For this example, the samples will be coming in slowly. This means we
    // can afford to wake up for every conversion.
    //
    am_hal_adc_interrupt_enable(g_ADCHandle, AM_HAL_ADC_INT_FIFOOVR1 | AM_HAL_ADC_INT_DERR | AM_HAL_ADC_INT_DCMP);

    //
    // Enable the ADC.
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_enable(g_ADCHandle))
    {
        am_util_stdio_printf("Error - enabling ADC failed.\n");
    }

    //
    // Enable internal repeat trigger timer
    //
    am_hal_adc_irtt_enable(g_ADCHandle);
}

//*****************************************************************************
//
// Main function.
//
//*****************************************************************************
int
main(void)
{
    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

#if defined(AM_PART_APOLLO4B)
    //
    // Set up scratch AXI buf (needs 64B - aligned to 16 Bytes)
    //
    am_hal_daxi_control(AM_HAL_DAXI_CONTROL_AXIMEM, (uint8_t *)((uint32_t)(axiScratchBuf + 3) & ~0xF));
#else
    am_hal_daxi_control(AM_HAL_DAXI_CONTROL_ENABLE, NULL);
#endif

    //
    // Configure the board for low power operation.
    //
    am_bsp_low_power_init();

    //
    // Start the ITM interface.
    //
    am_bsp_itm_printf_enable();

    //
    // Set a pin to act as our ADC input
    //
    am_hal_gpio_pinconfig(19, g_AM_PIN_19_ADCSE0);

    //
    // Configure the ADC
    //
    adc_config();

    //
    // Enable interrupts.
    //
    NVIC_SetPriority(ADC_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(ADC_IRQn);
    am_hal_interrupt_master_enable();

    //
    // Trigger the ADC sampling for the first time manually.
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_sw_trigger(g_ADCHandle))
    {
        am_util_stdio_printf("Error - triggering the ADC failed.\n");
    }

    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("ADC Example with 2.4Msps and LPMODE=0\n");

    //
    // Allow time for all printing to finish.
    //
    am_util_delay_ms(10);

    //
    // We are done printing. Disable debug printf messages on ITM.
    //
#if (0 == ADC_EXAMPLE_DEBUG)
    am_bsp_debug_printf_disable();
#endif

    //
    // Loop forever.
    //
    while(1)
    {
        //
        // Go to Deep Sleep.
        //
        if (!g_bADCDMAComplete)
        {
            sleep();
        }

        //
        // Check for DMA errors.
        //
        if (g_bADCDMAError)
        {
            am_util_stdio_printf("DMA Error occured\n");
            while(1);
        }

        //
        // Check if the ADC DMA completion interrupt occurred.
        //
        if (g_bADCDMAComplete)
        {
            // Invalidate DAXI to make sure CPU sees the new data when loaded.
            am_hal_daxi_control(AM_HAL_DAXI_CONTROL_INVALIDATE, NULL);
#if ADC_EXAMPLE_DEBUG
            {
                uint32_t        ui32SampleCount;
                am_util_stdio_printf("DMA Complete\n");
                ui32SampleCount = ADC_SAMPLE_BUF_SIZE;
                if (AM_HAL_STATUS_SUCCESS != am_hal_adc_samples_read(g_ADCHandle, false,
                                                                     g_ui32ADCSampleBuffer,
                                                                     &ui32SampleCount,
                                                                     SampleBuffer))
                {
                    am_util_stdio_printf("Error - failed to process samples.\n");
                }
            }
#endif
            //
            // Reset the DMA completion and error flags.
            //
            g_bADCDMAComplete = false;

            //
            // Re-configure the ADC DMA.
            //
            adc_config_dma();

            //
            // Clear the ADC interrupts.
            //
            if (AM_HAL_STATUS_SUCCESS != am_hal_adc_interrupt_clear(g_ADCHandle, 0xFFFFFFFF))
            {
                am_util_stdio_printf("Error - clearing the ADC interrupts failed.\n");
            }
        }
    }
}
