//*****************************************************************************
//
//! @file adc_vbatt.c
//!
//! @brief Example of ADC sampling VBATT voltage divider, BATT load, and temperature.
//!
//! Purpose: This example initializes the ADC, and a timer. Two times per second it
//! reads the VBATT voltage divider and temperature sensor and prints them.
//! It monitors button 0 and if pressed, it turns on the BATT LOAD resistor.
//! One should monitor MCU current to see when the load is on or off.
//!
//! Printing takes place over the ITM at 1M Baud.
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
uint32_t g_ui32SampleCount;

//
// ADC code for voltage divider from ADC ISR to base level.
//
uint16_t g_ui16ADCVDD_code;

//
// ADC code for temperature sensor from ADC ISR to base level.
//
uint16_t g_ui16ADCTEMP_code;

//*****************************************************************************
//
// ADC Configuration
//
//*****************************************************************************
const static am_hal_adc_config_t g_sADC_Cfg =
{
    //
    // Select the ADC Clock source.
    //
    .eClock = AM_HAL_ADC_CLKSEL_HFRC_DIV2,

    //
    // Polarity
    //
    .ePolarity = AM_HAL_ADC_TRIGPOL_RISING,

    //
    // Select the ADC trigger source using a trigger source macro.
    //
    .eTrigger = AM_HAL_ADC_TRIGSEL_SOFTWARE,

    //
    // Select the ADC reference voltage.
    //
    .eReference = AM_HAL_ADC_REFSEL_INT_1P5,
    .eClockMode = AM_HAL_ADC_CLKMODE_LOW_POWER,

    //
    // Choose the power mode for the ADC's idle state.
    //
    .ePowerMode = AM_HAL_ADC_LPMODE1,

    //
    // Enable repeating samples using Timer3A.
    //
    .eRepeat = AM_HAL_ADC_REPEATING_SCAN
};

//*****************************************************************************
//
// Timer configurations.
//
//*****************************************************************************
am_hal_ctimer_config_t g_sTimer3 =
{
    // do not link A and B together to make a long 32-bit counter.
    0,

    // Set up timer 3A to drive the ADC
    (AM_HAL_CTIMER_FN_PWM_REPEAT |
     AM_HAL_CTIMER_LFRC_32HZ),

    // Timer 3B is not used in this example.
    0,
};

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
    // Clear timer 3 interrupt.
    //
    am_hal_adc_interrupt_status(g_ADCHandle, &ui32IntStatus, true);
    am_hal_adc_interrupt_clear(g_ADCHandle, ui32IntStatus);

    //
    // Toggle LED 3.
    //
    am_devices_led_toggle(am_bsp_psLEDs, 3);

    //
    // Keep grabbing samples from the ADC FIFO until it goes empty.
    //
#if 0
//! @param pHandle              - handle for the module instance.
//! @param ui32SlotNumber       - desired slot number to filter samples on.
//!                               If set to AM_HAL_ADC_MAX_SLOTS then all
//!                               values will be provided.
//! @param ui32BufferSize       - number of entries in the sample buffer.
//!                               If 0 then samples will be read directly
//!                               from the FIFO.
//! @param pui32SampleBuffer    - pointer to the input sample buffer.
//! @param pui32NumberSamples   - returns the number of samples found.
//! @param pui32Samples         - pointer to a sample buffer to process.
//!                               If NULL then samples will be read directly
//!                               from the FIFO.

uint32_t am_hal_adc_samples_read(void *pHandle,
                                 uint32_t *pui32InSampleBuffer,
                                 uint32_t *pui32InOutNumberSamples,
                                 am_hal_adc_sample_t *pui32OutBuffer)

typedef struct
{
  uint32_t      ui32Sample;
  uint32_t      ui32Slot;
} am_hal_adc_sample_t;

#endif

    uint32_t ui32NumSamples = 1;
    am_hal_adc_sample_t sSample;

    //
    // Go get the sample.
    //
#if 0   // Need to get the FULL sample.  The HAL call doesn't currently return the full.
    uint32_t ui32fifodata = ADC->FIFOPR;

    //
    // Select which one of the two enabled slots is here right now.
    //
    if ( AM_HAL_ADC_FIFO_SLOT(ui32fifodata) == 5)
    {
        //
        // Just grab the ADC code for the battery voltage divider.
        //
        g_ui16ADCVDD_code = AM_HAL_ADC_FIFO_FULL_SAMPLE(ui32fifodata);
    }
    else
    {
        //
        // Just grab the ADC code for the temperature sensor.
        // We need the integer part in the low 16-bits.
        //
        g_ui16ADCTEMP_code = AM_HAL_ADC_FIFO_FULL_SAMPLE(ui32fifodata) & 0xFFC0;
    }

#elif 1
    //
    // Empty the FIFO, we'll just look at the last one read.
    //
    while ( AM_HAL_ADC_FIFO_COUNT(ADC->FIFO) )
    {
      ui32NumSamples = 1;
      am_hal_adc_samples_read(g_ADCHandle, true, NULL, &ui32NumSamples, &sSample);

      //
      // Determine which slot it came from?
      //
      if (sSample.ui32Slot == 5 )
      {
        //
        // The returned ADC sample is for the battery voltage divider.
        //
        g_ui16ADCVDD_code = AM_HAL_ADC_FIFO_SAMPLE(sSample.ui32Sample);

      }
      else
      {
        //
        // The returned ADC sample is for the temperature sensor.
        // We need the integer part in the low 16-bits.
        //
        g_ui16ADCTEMP_code = sSample.ui32Sample & 0xFFC0;
      }
    }
#else
#endif


#if 0
    uint32_t ui32fifodata, ui32IntStatus;
    while ( AM_HAL_ADC_FIFO_COUNT(am_hal_adc_fifo_peek()) )
    {
        ui32fifodata = am_hal_adc_fifo_pop();

        //
        // Select which one of the two enabled slots is here right now.
        //
        if ( AM_HAL_ADC_FIFO_SLOT(ui32fifodata) == 5)
        {
            //
            // Just grab the ADC code for the battery voltage divider.
            //
            g_ui16ADCVDD_code = AM_HAL_ADC_FIFO_FULL_SAMPLE(ui32fifodata);
        }
        else
        {
            //
            // Just grab the ADC code for the temperature sensor.
            // We need the integer part in the low 16-bits.
            //
            g_ui16ADCTEMP_code = AM_HAL_ADC_FIFO_FULL_SAMPLE(ui32fifodata) & 0xFFC0;
        }
    }
#endif

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
    am_hal_adc_slot_config_t sSlotCfg;

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
    // Configure the ADC.
    //
    if ( am_hal_adc_configure(g_ADCHandle, (am_hal_adc_config_t*)&g_sADC_Cfg) != AM_HAL_STATUS_SUCCESS )
    {
        am_util_stdio_printf("Error - configuring ADC failed.\n");
    }

    sSlotCfg.bEnabled       = false;
    sSlotCfg.bWindowCompare = false;
    sSlotCfg.eChannel       = AM_HAL_ADC_SLOT_CHSEL_SE0;    // 0
    sSlotCfg.eMeasToAvg     = AM_HAL_ADC_SLOT_AVG_1;        // 0
    sSlotCfg.ePrecisionMode = AM_HAL_ADC_SLOT_14BIT;        // 0

    am_hal_adc_configure_slot(g_ADCHandle, 0, &sSlotCfg);   // Unused slot
    am_hal_adc_configure_slot(g_ADCHandle, 1, &sSlotCfg);   // Unused slot
    am_hal_adc_configure_slot(g_ADCHandle, 2, &sSlotCfg);   // Unused slot
    am_hal_adc_configure_slot(g_ADCHandle, 3, &sSlotCfg);   // Unused slot
    am_hal_adc_configure_slot(g_ADCHandle, 4, &sSlotCfg);   // Unused slot
    am_hal_adc_configure_slot(g_ADCHandle, 6, &sSlotCfg);   // Unused slot

    sSlotCfg.bEnabled       = true;
    sSlotCfg.bWindowCompare = true;
    sSlotCfg.eChannel       = AM_HAL_ADC_SLOT_CHSEL_BATT;
    sSlotCfg.eMeasToAvg     = AM_HAL_ADC_SLOT_AVG_1;
    sSlotCfg.ePrecisionMode = AM_HAL_ADC_SLOT_14BIT;
    am_hal_adc_configure_slot(g_ADCHandle, 5, &sSlotCfg);   // BATT

    sSlotCfg.bEnabled       = true;
    sSlotCfg.bWindowCompare = true;
    sSlotCfg.eChannel       = AM_HAL_ADC_SLOT_CHSEL_TEMP;
    sSlotCfg.eMeasToAvg     = AM_HAL_ADC_SLOT_AVG_1;
    sSlotCfg.ePrecisionMode = AM_HAL_ADC_SLOT_10BIT;
    am_hal_adc_configure_slot(g_ADCHandle, 7, &sSlotCfg);   // TEMP

    //
    // Enable the ADC.
    //
    am_hal_adc_enable(g_ADCHandle);
}

//*****************************************************************************
//
// Enable the ADC INIT TIMER 3A function and set for 0.5 second period.
//
//*****************************************************************************
static void
timer_init(void)
{
//
// Only CTIMER 3 supports the ADC.
//
#define TIMERNUM    3
    uint32_t ui32Period = 2000; // Set for 2 second (2000ms) period

    //
    // LFRC has to be turned on for this example because we are running this
    // timer off of the LFRC.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_LFRC_START, 0);

    //
    // Set up timer 3A so start by clearing it.
    //
    am_hal_ctimer_clear(TIMERNUM, AM_HAL_CTIMER_TIMERA);

    //
    // Configure the timer to count 32Hz LFRC clocks but don't start it yet.
    //
    am_hal_ctimer_config(TIMERNUM, &g_sTimer3);

    //
    // Compute CMPR value needed for desired period based on a 32HZ clock.
    //
    ui32Period = ui32Period * 32 / 1000;
    am_hal_ctimer_period_set(TIMERNUM, AM_HAL_CTIMER_TIMERA,
                             ui32Period, (ui32Period >> 1));

#if 0
    //
//  // Enable the timer output "pin". This refers to the pin as seen from
//  // inside the timer. The actual GPIO pin is neither enabled nor driven.
//  am_hal_ctimer_pin_enable(TIMERNUM, AM_HAL_CTIMER_TIMERA);
#endif
#if 0
    //
    // CTimer A3 is available on the following pads: 5, 22, 31, 43, 48, 37.
    // On the apollo3_evb, only pin 31 is unused, so we'll pick it.
    //
    am_hal_ctimer_output_config(TIMERNUM, AM_HAL_CTIMER_TIMERA, 31,
                                AM_HAL_CTIMER_OUTPUT_FORCE0,
                                AM_HAL_GPIO_PIN_DRIVESTRENGTH_2MA);
#endif

    //
    // Set up timer 3A as the trigger source for the ADC.
    //
    am_hal_ctimer_adc_trigger_enable();

#if 0
    //
    // Clear the timer Interrupt
    //
    am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA0 << (TIMERNUM * 2));
#endif

    //
    // Start timer 3A.
    //
    am_hal_ctimer_start(TIMERNUM, AM_HAL_CTIMER_TIMERA);
} // timer_init()

//*****************************************************************************
//
// Main function.
//
//*****************************************************************************
int
main(void)
{
    bool  bMeasured;
    float fTempF;
    int32_t i32BaseLevelCount;
    const float fReferenceVoltage = 1.5;
    float fVBATT;
    float fADCTempVolts;
    float fADCTempDegreesC;
//  float fTemp, fVoltage, fOffset;
    float fTrims[4];
    uint32_t ui32Retval;

    //
    // Set the clock frequency.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);

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
    // Initialize device drivers for the LEDs on the board.
    //
    am_devices_led_array_init(am_bsp_psLEDs, AM_BSP_NUM_LEDS);

    //
    // Configure the button pin.
    //
    am_hal_gpio_pinconfig(AM_BSP_GPIO_BUTTON0, g_AM_BSP_GPIO_BUTTON0);

    //
    // Initialize the printf interface for ITM output
    //
    am_bsp_itm_printf_enable();

    //
    // Clear the terminal screen, and print a quick message to show that we're
    // alive.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("ADC VBATT and Temperature Sensing Example.\n");

    //
    // Enable floating point.
    //
    am_hal_sysctrl_fpu_enable();
    am_hal_sysctrl_fpu_stacking_enable(true);

    //
    // Initialize the ADC.
    //
    adc_init();

    //
    // Initialize CTIMER 3A to trigger the ADC every 0.5 seconds.
    //
    timer_init();

    //
    // Print out ctimer initial register state.
    //
    am_util_stdio_printf("\n");
    am_util_stdio_printf("CTIMER3=0x%08X @ 0x%08X\n",
                         AM_REGVAL(CTIMERADDRn(CTIMER, 3, TMR0)),
                         CTIMERADDRn(CTIMER, 3, TMR0));
    am_util_stdio_printf("CTIMER3=0x%08X @ 0x%08X\n",
                         AM_REGVAL(CTIMERADDRn(CTIMER, 3, CMPRA0)),
                         CTIMERADDRn(CTIMER, 3, CMPRA0));
    am_util_stdio_printf("CTIMER3=0x%08X @ 0x%08X\n",
                         AM_REGVAL(CTIMERADDRn(CTIMER, 3, CMPRB0)),
                         CTIMERADDRn(CTIMER, 3, CMPRB0));
    am_util_stdio_printf("CTIMER3=0x%08X @ 0x%08X\n",
                         AM_REGVAL(CTIMERADDRn(CTIMER, 3, CTRL0)),
                         CTIMERADDRn(CTIMER, 3, CTRL0));

    //
    // Print out ADC initial register state.
    //
    am_util_stdio_printf("\n");
    am_util_stdio_printf("ADC REGISTERS @ 0x%08X\n", (uint32_t)REG_ADC_BASEADDR);
    am_util_stdio_printf("ADC CFG   = 0x%08X\n", ADC->CFG);
    am_util_stdio_printf("ADC SLOT0 = 0x%08X\n", ADC->SL0CFG);
    am_util_stdio_printf("ADC SLOT1 = 0x%08X\n", ADC->SL1CFG);
    am_util_stdio_printf("ADC SLOT2 = 0x%08X\n", ADC->SL2CFG);
    am_util_stdio_printf("ADC SLOT3 = 0x%08X\n", ADC->SL3CFG);
    am_util_stdio_printf("ADC SLOT4 = 0x%08X\n", ADC->SL4CFG);
    am_util_stdio_printf("ADC SLOT5 = 0x%08X\n", ADC->SL5CFG);
    am_util_stdio_printf("ADC SLOT6 = 0x%08X\n", ADC->SL6CFG);
    am_util_stdio_printf("ADC SLOT7 = 0x%08X\n", ADC->SL7CFG);

    //
    // Print out the temperature trim values as recorded in OTP.
    //
    fTrims[0] = fTrims[1] = fTrims[2] = 0.0F;
    fTrims[3] = -123.456f;
    am_hal_adc_control(g_ADCHandle, AM_HAL_ADC_REQ_TEMP_TRIMS_GET, fTrims);
    bMeasured = fTrims[3] ? true : false;
    am_util_stdio_printf("\n");
    am_util_stdio_printf("TRIMMED TEMP    = %.3f\n", fTrims[0]);
    am_util_stdio_printf("TRIMMED VOLTAGE = %.3f\n", fTrims[1]);
    am_util_stdio_printf("TRIMMED Offset  = %.3f\n", fTrims[2]);
    am_util_stdio_printf("Note - these trim values are '%s' values.\n",
                         bMeasured ? "calibrated" : "uncalibrated default");
    am_util_stdio_printf("\n");

    //
    // Enable the ADC interrupt in the NVIC.
    //
    NVIC_EnableIRQ(ADC_IRQn);
    am_hal_interrupt_master_enable();

    //
    // Enable the ADC interrupts in the ADC.
    //
    am_hal_adc_interrupt_enable(g_ADCHandle, AM_HAL_ADC_INT_WCINC       |
                                             AM_HAL_ADC_INT_WCEXC       |
                                             AM_HAL_ADC_INT_FIFOOVR2    |
                                             AM_HAL_ADC_INT_FIFOOVR1    |
                                             AM_HAL_ADC_INT_SCNCMP      |
                                             AM_HAL_ADC_INT_CNVCMP);

    //
    // Reset the sample count which will be incremented by the ISR.
    //
    g_ui32SampleCount = 0;

    //
    // Kick Start Timer 3 with an ADC software trigger in REPEAT used.
    //
    am_hal_adc_sw_trigger(g_ADCHandle);

    //
    // Track buffer depth for progress messages.
    //
    i32BaseLevelCount = g_ui32SampleCount;

    //
    // Wait here for the ISR to grab a buffer of samples.
    //
    while (1)
    {
        //
        // Print the battery voltage and temperature for each interrupt
        //
        if (g_ui32SampleCount > i32BaseLevelCount)
        {
            i32BaseLevelCount = g_ui32SampleCount;

            //
            // Compute the voltage divider output.
            //
            fVBATT = ((float)g_ui16ADCVDD_code) * 3.0f * fReferenceVoltage / (1024.0f / 64.0f);

            //
            // Print the voltage divider output.
            //
            am_util_stdio_printf("VBATT = <%.3f> (0x%04X) ",
                                 fVBATT, g_ui16ADCVDD_code);

            //
            // Convert and scale the temperature.
            // Temperatures are in Fahrenheit range -40 to 225 degrees.
            // Voltage range is 0.825V to 1.283V
            // First get the ADC voltage corresponding to temperature.
            //
            fADCTempVolts = ((float)g_ui16ADCTEMP_code) * fReferenceVoltage / (1024.0f * 64.0f);

            //
            // Now call the HAL routine to convert volts to degrees Celsius.
            //
            float fVT[3];
            fVT[0] = fADCTempVolts;
            fVT[1] = 0.0f;
            fVT[2] = -123.456;
//          fADCTempDegreesC = am_hal_adc_volts_to_celsius(fADCTempVolts);
            ui32Retval = am_hal_adc_control(g_ADCHandle, AM_HAL_ADC_REQ_TEMP_CELSIUS_GET, fVT);
            if ( ui32Retval == AM_HAL_STATUS_SUCCESS )
            {
                fADCTempDegreesC = fVT[1];  // Get the temperature

                //
                // print the temperature value in Celsius.
                //
                am_util_stdio_printf("TEMP = %.2f C (0x%04X) ",
                                     fADCTempDegreesC, g_ui16ADCTEMP_code);

                //
                // Print the temperature value in Fahrenheit.
                //
                fTempF = (fADCTempDegreesC * (180.0f / 100.0f)) + 32.0f;
                am_util_stdio_printf(" %.2f F", fTempF);
            }
            else
            {
                am_util_stdio_printf("Error: am_haL_adc_control returned %d\n", ui32Retval);

            }

            //
            // Use button 0 to turn on or off the battery load resistor.
            //
#if AM_PART_APOLLO
            if (!am_hal_gpio_input_bit_read(AM_BSP_GPIO_BUTTON0))
            {
                am_util_stdio_printf("BATTERY LOAD RESISTOR ON\n");
                am_hal_adc_batt_load_enable();
                am_devices_led_on(am_bsp_psLEDs, 2);
            }
            else
            {
                am_util_stdio_printf("\n");
                am_hal_adc_batt_load_disable();
                am_devices_led_off(am_bsp_psLEDs, 2);
            }
#else
            am_util_stdio_printf("\n");
#endif
        }

        //
        // Sleep here until the next ADC interrupt comes along.
        //
        am_devices_led_off(am_bsp_psLEDs, 0);
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
        am_devices_led_on(am_bsp_psLEDs, 0);
    }
}
