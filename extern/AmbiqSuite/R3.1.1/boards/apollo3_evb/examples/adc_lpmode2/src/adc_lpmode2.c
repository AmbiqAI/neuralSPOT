//*****************************************************************************
//
//! @file adc_lpmode2.c
//!
//! @brief This example takes samples with the ADC at 1Hz in lowest power mode.
//!
//! Purpose: To demonstrate the lowest possible power usage of the ADC.  The
//! example powers off the ADC between samples.  CTIMER-A1 is used to drive the
//! process.  The CTIMER ISR reconfigures the ADC from scratch and triggers each
//! sample.  The ADC ISR stores the sample and shuts down the ADC.
//!
//! Additional Information:
//! The ADC_EXAMPLE_DEBUG flag is used to display information in the example to
//! show that it is operating.  This should be set to 0 for true low power
//! operation.
//!
//! Printing takes place over the ITM at 1M Baud.
//!
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2023, Ambiq Micro, Inc.
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
// This is part of revision release_sdk_3_1_1-10cda4b5e0 of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

#define ADC_EXAMPLE_DEBUG 1

//
// ADC Device Handle.
//
static void *g_ADCHandle;

//
// Define the ADC SE0 pin to be used.
//
const am_hal_gpio_pincfg_t g_AM_PIN_16_ADCSE0 =
{
    .uFuncSel       = AM_HAL_PIN_16_ADCSE0,
};

//*****************************************************************************
//
// Forward function declarations.
//
//*****************************************************************************
void adc_config(void);
void adc_deconfig(void);

//*****************************************************************************
//
// Interrupt handler for the CTIMER
//
//*****************************************************************************
void
am_ctimer_isr(void)
{
  //
  // Clear TimerA0 Interrupt.
  //
  am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA0);

  //
  // Re-configure the ADC. We lose configuration data in the power-down, so
  // we'll reconfigure the ADC here. If you don't shut down the ADC, this
  // step is unnecessary.
  //
  adc_config();

  //
  // Trigger the ADC
  //
  am_hal_adc_sw_trigger(g_ADCHandle);
}

//*****************************************************************************
//
// Interrupt handler for the ADC.
//
//*****************************************************************************
void
am_adc_isr(void)
{
  uint32_t            ui32IntMask;
  am_hal_adc_sample_t Sample;

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
  // If we got a conversion completion interrupt (which should be our only
  // ADC interrupt), go ahead and read the data.
  //
  if (ui32IntMask & AM_HAL_ADC_INT_CNVCMP)
  {
    uint32_t    ui32NumSamples = 1;
    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_samples_read(g_ADCHandle, false,
                                                         NULL,
                                                         &ui32NumSamples,
                                                         &Sample))
    {
      am_util_stdio_printf("Error - ADC sample read from FIFO failed.\n");
    }

#if (1 == ADC_EXAMPLE_DEBUG)
    am_util_stdio_printf("ADC Slot =  %d\n", Sample.ui32Slot);
    am_util_stdio_printf("ADC Value = %8.8X\n", Sample.ui32Sample);
#endif
  }

  adc_deconfig();

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
  // Set up the ADC configuration parameters. These settings are reasonable
  // for accurate measurements at a low sample rate.
  //
  ADCConfig.eClock             = AM_HAL_ADC_CLKSEL_HFRC;
  ADCConfig.ePolarity          = AM_HAL_ADC_TRIGPOL_RISING;
  ADCConfig.eTrigger           = AM_HAL_ADC_TRIGSEL_SOFTWARE;
  ADCConfig.eReference         = AM_HAL_ADC_REFSEL_INT_2P0;
  ADCConfig.eClockMode         = AM_HAL_ADC_CLKMODE_LOW_POWER;
  ADCConfig.ePowerMode         = AM_HAL_ADC_LPMODE0;
  ADCConfig.eRepeat            = AM_HAL_ADC_REPEATING_SCAN;
  if (AM_HAL_STATUS_SUCCESS != am_hal_adc_configure(g_ADCHandle, &ADCConfig))
  {
    am_util_stdio_printf("Error - configuring ADC failed.\n");
  }

  //
  // Set up an ADC slot
  //
  ADCSlotConfig.eMeasToAvg      = AM_HAL_ADC_SLOT_AVG_1;
  ADCSlotConfig.ePrecisionMode  = AM_HAL_ADC_SLOT_14BIT;
  ADCSlotConfig.eChannel        = AM_HAL_ADC_SLOT_CHSEL_SE0;
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
  am_hal_adc_interrupt_enable(g_ADCHandle, AM_HAL_ADC_INT_CNVCMP );

  //
  // Enable the ADC.
  //
  if (AM_HAL_STATUS_SUCCESS != am_hal_adc_enable(g_ADCHandle))
  {
    am_util_stdio_printf("Error - enabling ADC failed.\n");
  }
}

//*****************************************************************************
//
// Configure the ADC.
//
//*****************************************************************************
void
adc_deconfig(void)
{
  //
  // Disable the ADC.
  //
  if (AM_HAL_STATUS_SUCCESS != am_hal_adc_disable(g_ADCHandle))
  {
    am_util_stdio_printf("Error - disable ADC failed.\n");
  }

  //
  // Enable the ADC power domain.
  //
  if (AM_HAL_STATUS_SUCCESS != am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_ADC))
  {
    am_util_stdio_printf("Error - disabling the ADC power domain failed.\n");
  }

  //
  // Initialize the ADC and get the handle.
  //
  if (AM_HAL_STATUS_SUCCESS != am_hal_adc_deinitialize(g_ADCHandle))
  {
    am_util_stdio_printf("Error - return of the ADC instance failed.\n");
  }

}

//*****************************************************************************
//
// Start sampling
//
//*****************************************************************************
void
init_timerA1_for_ADC(void)
{
  //
  // Start a timer to trigger the ADC periodically. This timer won't actually
  // be connected to the ADC (as can be done with Timer 3). Instead, we'll
  // generate interrupts to the CPU, and then use the CPU to trigger the ADC
  // in the CTIMER interrupt handler.
  //
  am_hal_ctimer_config_single(0, AM_HAL_CTIMER_TIMERA,
                              AM_HAL_CTIMER_LFRC_512HZ |
                                AM_HAL_CTIMER_FN_REPEAT |
                                  AM_HAL_CTIMER_INT_ENABLE);

  am_hal_ctimer_int_enable(AM_HAL_CTIMER_INT_TIMERA0);

  am_hal_ctimer_period_set(0, AM_HAL_CTIMER_TIMERA, 511, 0);


  //
  // Start the timer.
  //
  am_hal_ctimer_start(0, AM_HAL_CTIMER_TIMERA);
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
  // Set the clock frequency.
  //
  if (AM_HAL_STATUS_SUCCESS != am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0))
  {
    am_util_stdio_printf("Error - configuring the system clock failed.\n");
  }


  //
  // Set the default cache configuration and enable it.
  //
  if (AM_HAL_STATUS_SUCCESS != am_hal_cachectrl_config(&am_hal_cachectrl_defaults))
  {
    am_util_stdio_printf("Error - configuring the system cache failed.\n");
  }
  if (AM_HAL_STATUS_SUCCESS != am_hal_cachectrl_enable())
  {
    am_util_stdio_printf("Error - enabling the system cache failed.\n");
  }

  //
  // Configure the board for low power operation.
  //
  am_bsp_low_power_init();

  //
  // Enable only the first 512KB bank of Flash (0).  Disable Flash(1)
  //
  if (AM_HAL_STATUS_SUCCESS != am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEM_FLASH_MIN))
  {
    am_util_stdio_printf("Error - configuring the flash memory failed.\n");
  }

#if defined(AM_PART_APOLLO3)
    //
    // Enable the first 32K of TCM SRAM.
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEM_SRAM_32K_DTCM))
    {
        am_util_stdio_printf("Error - configuring the SRAM failed.\n");
    }
#else
    //
    // Enable the first 128K of SRAM.
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEM_SRAM_128K))
    {
        am_util_stdio_printf("Error - configuring the SRAM failed.\n");
    }
#endif

  //
  // Start the ITM interface.
  //
  am_bsp_itm_printf_enable();

  //
  // Set a pin to act as our ADC input
  //
  am_hal_gpio_pinconfig(16, g_AM_PIN_16_ADCSE0);

  //
  // Start the timer-based ADC measurements.
  //
  init_timerA1_for_ADC();

  //
  // Print the banner.
  //
  am_util_stdio_terminal_clear();
  am_util_stdio_printf("ADC Example at 1Hz with ADC disabled between samples\n");

  //
  // Allow time for all printing to finish.
  //
  am_util_delay_ms(10);

  //
  // Enable interrupts.
  //
  NVIC_EnableIRQ(ADC_IRQn);
  NVIC_EnableIRQ(CTIMER_IRQn);
  am_hal_interrupt_master_enable();

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
    // Disable interrupts
    //
    am_hal_interrupt_master_disable();

    //
    // Put the core to sleep.
    //
    sleep();

    //
    // Enable interrupts.
    //
    am_hal_interrupt_master_enable();
  }
}
