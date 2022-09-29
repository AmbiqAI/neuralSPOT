//*****************************************************************************
//!
//! @file adc_measure.c
//!
//! @brief Example of ADC sampling the voltage applied to a particular pin.
//!
//! Purpose: This example initializes the ADC and periodically samples the
//! voltage as applied to the ADC input configured on pin 18.
//!
//! On interrupt, the samples are pulled from the FIFO, the example then
//! applies Vref to the sample and displays the measured voltage to SWO.
//!
//! It should be noted that he pin number used for input is designated in the
//! example source code via the define ADC_INPUT_PIN.
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
// Local defines
//
//*****************************************************************************
#define ADC_INPUT_PIN           18

//*****************************************************************************
//
// Global Variables
//
//*****************************************************************************
//
// ADC Device Handle.
//
static void *g_ADCHandle;

//
// Sample Count semaphore from ADC ISR to base level.
//
uint32_t g_ui32SampleCount = 0;

//
// Pin configuration
//
#if ADC_INPUT_PIN == 18
const am_hal_gpio_pincfg_t g_ADC_PIN_CFG =
{
    .GP.cfg_b.uFuncSel       = AM_HAL_PIN_18_ADCSE1,
};
#else
#error Must set up a pin config for the pin that is to be used for voltage input
#endif

//
// Set up an array to use for retrieving the correction trim values.
//
float g_fTrims[4];

//*****************************************************************************
//
// ADC Interrupt Service Routine (ISR)
//
//*****************************************************************************
void
am_adc_isr(void)
{
    uint32_t ui32IntStatus;

    //
    // Clear the ADC interrupt.
    //
    am_hal_adc_interrupt_status(g_ADCHandle, &ui32IntStatus, true);
    am_hal_adc_interrupt_clear(g_ADCHandle, ui32IntStatus);

    //
    // Keep grabbing samples from the ADC FIFO until it goes empty.
    //
    uint32_t ui32NumSamples = 1;
    am_hal_adc_sample_t sSample;

    //
    // Get samples until the FIFO is emptied.
    //
    while ( AM_HAL_ADC_FIFO_COUNT(ADC->FIFO) )
    {
        ui32NumSamples = 1;

        //
        // Invalidate DAXI to make sure CPU sees the new data when loaded.
        //
        am_hal_daxi_control(AM_HAL_DAXI_CONTROL_INVALIDATE, NULL);
        am_hal_adc_samples_read(g_ADCHandle, false, NULL, &ui32NumSamples, &sSample);
        am_util_delay_ms(200);

        am_util_stdio_printf("ADC sample read=%d, measured voltage=%.5f mV\n",
                             sSample.ui32Sample, (float)(sSample.ui32Sample  * AM_HAL_ADC_VREFMV / 0x1000) );
        am_util_delay_ms(200);
    }

    //
    // Signal interrupt arrival to base level.
    //
    g_ui32SampleCount++;
}

//*****************************************************************************
//
// ADC INIT Function
//
//*****************************************************************************
void
adc_init(void)
{
    am_hal_adc_config_t           ADCConfig;
    am_hal_adc_slot_config_t      ADCSlotConfig;

    //
    // Initialize the ADC and get the handle.
    //
    if ( am_hal_adc_initialize(0, &g_ADCHandle) != AM_HAL_STATUS_SUCCESS )
    {
        am_util_stdio_printf("Error - reservation of the ADC instance failed.\n");
    }

    //
    // Get the ADC correction offset and gain for this DUT.
    // Note that g_fTrims[3] must contain the value -123.456f before calling
    // the function.
    // On return g_fTrims[0] contains the offset, g_fTrims[1] the gain.
    //
    g_fTrims[0] = g_fTrims[1] = g_fTrims[2] = 0.0F;
    g_fTrims[3] = -123.456f;
    am_hal_adc_control(g_ADCHandle, AM_HAL_ADC_REQ_CORRECTION_TRIMS_GET, g_fTrims);
    am_util_stdio_printf(" ADC correction offset = %.6f\n", g_fTrims[0]);
    am_util_stdio_printf(" ADC correction gain   = %.6f\n", g_fTrims[1]);

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
    if ( am_hal_adc_configure(g_ADCHandle, &ADCConfig) != AM_HAL_STATUS_SUCCESS )
    {
        am_util_stdio_printf("Error - configuring ADC failed.\n");
    }

    //
    // Set up an ADC slot
    //

    //! Set additional input sampling ADC clock cycles
    ADCSlotConfig.eMeasToAvg      = AM_HAL_ADC_SLOT_AVG_128;
    ADCSlotConfig.ui32TrkCyc      = AM_HAL_ADC_MIN_TRKCYC;
    ADCSlotConfig.ePrecisionMode  = AM_HAL_ADC_SLOT_12BIT;
    ADCSlotConfig.eChannel        = AM_HAL_ADC_SLOT_CHSEL_SE1;
    ADCSlotConfig.bWindowCompare  = false;
    ADCSlotConfig.bEnabled        = true;
    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_configure_slot(g_ADCHandle, 0, &ADCSlotConfig))
    {
        am_util_stdio_printf("Error - configuring ADC Slot 0 failed.\n");
    }

    //
    // For this example, the samples will be coming in slowly. This means we
    // can afford to wake up for every conversion.
    //
    am_hal_adc_interrupt_enable(g_ADCHandle, AM_HAL_ADC_INT_FIFOOVR1 | AM_HAL_ADC_INT_DERR | AM_HAL_ADC_INT_DCMP | AM_HAL_ADC_INT_CNVCMP | AM_HAL_ADC_INT_SCNCMP );

    //
    // Enable the ADC.
    //
    if ( am_hal_adc_enable(g_ADCHandle) != AM_HAL_STATUS_SUCCESS )
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

    //
    // Configure the board for low power operation.
    //
    am_bsp_low_power_init();

    //
    // Initialize the printf interface for ITM output
    //
    am_bsp_itm_printf_enable();

    //
    // Clear the terminal screen, and print a quick message to show that we're
    // alive.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("ADC voltage measure example.\n");
    am_util_stdio_printf(" Use pin %d for input.\n", ADC_INPUT_PIN);
    am_util_stdio_printf(" The applied voltage should be between 0 and 1.1v.\n");

    //
    // Enable floating point.
    //
    am_hal_sysctrl_fpu_enable();
    am_hal_sysctrl_fpu_stacking_enable(true);

    //
    // Set a pin to act as our ADC input
    //
    am_hal_gpio_pinconfig(ADC_INPUT_PIN, g_ADC_PIN_CFG);

    //
    // Initialize the ADC.
    //
    adc_init();

    //
    // Enable interrupts.
    //
    NVIC_SetPriority(ADC_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(ADC_IRQn);
    am_hal_interrupt_master_enable();

    //
    // Print out ADC initial register state.
    //
    am_util_stdio_printf("\n");
    am_util_stdio_printf("ADC REGISTERS:\n");
    am_util_stdio_printf("ADC CFG   = 0x%08X\n", ADC->CFG);
    am_util_stdio_printf("ADC SLOT0 = 0x%08X\n", ADC->SL0CFG);

    //
    // Reset the sample count which will be incremented by the ISR.
    //
    g_ui32SampleCount = 0;

    //
    // Kick Start Repeat with an ADC software trigger in REPEAT mode.
    //
    am_hal_adc_sw_trigger(g_ADCHandle);

    //
    // Wait here for the ISR to grab a buffer of samples.
    //
    while (1)
    {
    }
}
