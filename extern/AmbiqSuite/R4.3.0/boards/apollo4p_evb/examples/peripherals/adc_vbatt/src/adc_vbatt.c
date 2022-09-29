//*****************************************************************************
//
//! @file adc_vbatt.c
//!
//! @brief Example of ADC sampling VBATT voltage divider, BATT load, and temperature.
//!
//! Purpose: This example initializes the ADC. About two times per second it reads
//! the VBATT voltage divider and temperature sensor and prints the results.
//!
//! It also monitors button 0 and if pressed, toggles between enabling and
//! disabling the BATTLOAD resistor.
//! One would want to monitor MCU current to see when the load is on or off.
//!
//! The expected VBATT voltage as measured for the Apollo4 EVB is ~0.65V.
//! In general the measured voltage should be VDD / 3.
//!
//! Printing takes place over the SWO at 1M Baud.
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

//
// Modify ISR_SAMPLE_COUNT define to adjust the number of prints per second.
//

#define SLOT_BATT   5
#define SLOT_TEMP   7

//
// Define LED numbers
//
#define LED_DEEPSLEEP       0
#define LED_ADC_ISR         1
#define LED_VBATT_LOAD      2

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

// AXI Scratch buffer
// Need to allocate 20 Words even though we only need 16, to ensure we have 16 Byte alignment
AM_SHARED_RW uint32_t axiScratchBuf[20];

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
    .eClock = AM_HAL_ADC_CLKSEL_HFRC_24MHZ,

    //
    // Polarity
    //
    .ePolarity = AM_HAL_ADC_TRIGPOL_RISING,

    //
    // Select the ADC trigger source using a trigger source macro.
    //
    .eTrigger = AM_HAL_ADC_TRIGSEL_SOFTWARE,

    //
    // Apollo4 uses a 1.2v reference.
    // Select the clock mode.
    //
    .eClockMode = AM_HAL_ADC_CLKMODE_LOW_POWER,

    //
    // Choose the power mode for the ADC's idle state.
    //
    .ePowerMode = AM_HAL_ADC_LPMODE1,

    //
    // Enable repeating samples
    //
    .eRepeat = AM_HAL_ADC_REPEATING_SCAN,
    .eRepeatTrigger = AM_HAL_ADC_RPTTRIGSEL_INT
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
    // Clear the ADC interrupt.
    //
    am_hal_adc_interrupt_status(g_ADCHandle, &ui32IntStatus, true);
    am_hal_adc_interrupt_clear(g_ADCHandle, ui32IntStatus);

#ifdef AM_BSP_NUM_LEDS
    //
    // Toggle LED.
    //
    am_devices_led_toggle(am_bsp_psLEDs, LED_ADC_ISR);
#endif // AM_BSP_NUM_LEDS

    //
    // Keep grabbing samples from the ADC FIFO until it goes empty.
    //
    uint32_t ui32NumSamples = 1;
    am_hal_adc_sample_t sSample;

    //
    // Go get the sample.
    // Empty the FIFO, we'll just look at the last one read.
    //
    while ( AM_HAL_ADC_FIFO_COUNT(ADC->FIFO) )
    {
        ui32NumSamples = 1;

        // Invalidate DAXI to make sure CPU sees the new data when loaded.
        am_hal_daxi_control(AM_HAL_DAXI_CONTROL_INVALIDATE, NULL);

        am_hal_adc_samples_read(g_ADCHandle, true, NULL, &ui32NumSamples, &sSample);

        //
        // Determine which slot it came from?
        //
        if (sSample.ui32Slot == SLOT_BATT )
        {
            //
            // The returned ADC sample is for the battery voltage divider.
            // We need the integer part of the 20 bit fifo sample( >> 6)
            //
            g_ui16ADCVDD_code = AM_HAL_ADC_FIFO_SAMPLE(sSample.ui32Sample);
        }
        else if (sSample.ui32Slot == SLOT_TEMP )
        {
            //
            // The returned ADC sample is for the temperature sensor.
            // We need the integer part in the low 16-bits.
            //
            g_ui16ADCTEMP_code = AM_HAL_ADC_FIFO_SAMPLE(sSample.ui32Sample);
        }
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
    am_hal_adc_slot_config_t sSlotCfg;

    //
    // Initialize the ADC and get the handle.
    //
    if ( am_hal_adc_initialize(0, &g_ADCHandle) != AM_HAL_STATUS_SUCCESS )
    {
        am_util_stdio_printf("adc_init() Error - reservation of the ADC instance failed.\n");
    }

    //
    // Power on the ADC.
    //
    if (am_hal_adc_power_control(g_ADCHandle, AM_HAL_SYSCTRL_WAKE, false) != AM_HAL_STATUS_SUCCESS )
    {
        am_util_stdio_printf("adc_init() Error - ADC power on failed.\n");
    }

    //
    // Configure the ADC.
    //
    if ( am_hal_adc_configure(g_ADCHandle, (am_hal_adc_config_t*)&g_sADC_Cfg) != AM_HAL_STATUS_SUCCESS )
    {
        am_util_stdio_printf("adc_init() Error - configuring ADC failed.\n");
    }

    //
    // Set up internal repeat trigger timer
    //
    am_hal_adc_irtt_config_t      ADCIrttConfig =
    {
        .bIrttEnable        = true,
        .eClkDiv            = AM_HAL_ADC_RPTT_CLK_DIV16, // 24/16 = 1.5MHz
        .ui32IrttCountMax   = 30,
    };

    am_hal_adc_configure_irtt(g_ADCHandle, &ADCIrttConfig);

    //
    // For this example, the samples will be coming in slowly. This means we
    // can afford to wake up for every conversion.
    //
    am_hal_adc_interrupt_enable(g_ADCHandle, AM_HAL_ADC_INT_FIFOOVR1 | AM_HAL_ADC_INT_DERR | AM_HAL_ADC_INT_DCMP | AM_HAL_ADC_INT_CNVCMP | AM_HAL_ADC_INT_SCNCMP );

    sSlotCfg.bEnabled       = false;
    sSlotCfg.bWindowCompare = false;
    sSlotCfg.eChannel       = AM_HAL_ADC_SLOT_CHSEL_SE0;    // 0
    sSlotCfg.eMeasToAvg     = AM_HAL_ADC_SLOT_AVG_1;        // 0
    sSlotCfg.ePrecisionMode = AM_HAL_ADC_SLOT_12BIT;        // 0
    sSlotCfg.ui32TrkCyc     = AM_HAL_ADC_MIN_TRKCYC;

    am_hal_adc_configure_slot(g_ADCHandle, 0, &sSlotCfg);   // Unused slot
    am_hal_adc_configure_slot(g_ADCHandle, 1, &sSlotCfg);   // Unused slot
    am_hal_adc_configure_slot(g_ADCHandle, 2, &sSlotCfg);   // Unused slot
    am_hal_adc_configure_slot(g_ADCHandle, 3, &sSlotCfg);   // Unused slot
    am_hal_adc_configure_slot(g_ADCHandle, 4, &sSlotCfg);   // Unused slot
    am_hal_adc_configure_slot(g_ADCHandle, 6, &sSlotCfg);   // Unused slot

    sSlotCfg.bEnabled       = true;
    sSlotCfg.bWindowCompare = false;
    sSlotCfg.eChannel       = AM_HAL_ADC_SLOT_CHSEL_BATT;
    sSlotCfg.eMeasToAvg     = AM_HAL_ADC_SLOT_AVG_1;
    sSlotCfg.ePrecisionMode = AM_HAL_ADC_SLOT_12BIT;
    sSlotCfg.ui32TrkCyc     = AM_HAL_ADC_MIN_TRKCYC;
    am_hal_adc_configure_slot(g_ADCHandle, SLOT_BATT, &sSlotCfg);   // BATT

    sSlotCfg.bEnabled       = true;
    sSlotCfg.bWindowCompare = false;
    sSlotCfg.eChannel       = AM_HAL_ADC_SLOT_CHSEL_TEMP;
    sSlotCfg.eMeasToAvg     = AM_HAL_ADC_SLOT_AVG_1;
    sSlotCfg.ePrecisionMode = AM_HAL_ADC_SLOT_10BIT;
    sSlotCfg.ui32TrkCyc     = AM_HAL_ADC_MIN_TRKCYC;
    am_hal_adc_configure_slot(g_ADCHandle, SLOT_TEMP, &sSlotCfg);   // TEMP

    //
    // Enable the ADC.
    //
    if ( am_hal_adc_enable(g_ADCHandle) != AM_HAL_STATUS_SUCCESS)
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
    bool  bMeasured;
    float fTempF;
    int32_t i32BaseLevelCount;
    const float fReferenceVoltage = AM_HAL_ADC_VREF;
    float fVBATT;
    float fADCTempVolts;
    float fADCTempDegreesC;
    float fTrims[4];
    uint32_t ui32Retval;
#ifdef AM_BSP_GPIO_BUTTON0
    am_hal_mcuctrl_status_t sMcuctrlStatus;
    uint32_t ui32ReadState = 1;
#endif // AM_BSP_GPIO_BUTTON0


    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    //
    // Configure the board for low power operation.
    //
    am_bsp_low_power_init();

#ifdef AM_BSP_NUM_LEDS
    //
    // Initialize device drivers for the LEDs on the board.
    //
    am_devices_led_array_init(am_bsp_psLEDs, AM_BSP_NUM_LEDS);
#endif // AM_BSP_NUM_LEDS

#ifdef AM_BSP_GPIO_BUTTON0
    //
    // Configure the button pin.
    //
    am_hal_gpio_pinconfig(AM_BSP_GPIO_BUTTON0, g_AM_BSP_GPIO_BUTTON0);
#endif // AM_BSP_GPIO_BUTTON0

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
    // Enable interrupts.
    //
    NVIC_SetPriority(ADC_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(ADC_IRQn);
    am_hal_interrupt_master_enable();

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
    bMeasured = *(uint32_t*)&fTrims[3] ? true : false;
    am_util_stdio_printf("\n");
    am_util_stdio_printf("TRIMMED TEMP    = %.3f K (%.3f C)\n", fTrims[0], fTrims[0] - 273.15F );
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

    am_hal_adc_interrupt_enable(g_ADCHandle, AM_HAL_ADC_INT_FIFOOVR2    |
                                             AM_HAL_ADC_INT_FIFOOVR2);

    //
    // Reset the sample count which will be incremented by the ISR.
    //
    g_ui32SampleCount = 0;

    //
    // Kick Start Repeat with an ADC software trigger in REPEAT used.
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
#ifdef AM_BSP_GPIO_BUTTON0
        //
        // Read button status for later.
        //
        if ( ui32ReadState )
        {
            am_hal_gpio_state_read(AM_BSP_GPIO_BUTTON0, AM_HAL_GPIO_INPUT_READ, &ui32ReadState);
        }
#endif // AM_BSP_GPIO_BUTTON0

        //
        // Print the battery voltage and temperature for each interrupt
        // Note: At 24MHZ, ISR_SAMPLE_COUNT=3000 results in ~2 prints per second.
        //
        #define ISR_SAMPLE_COUNT   3000
        if (g_ui32SampleCount > (i32BaseLevelCount + ISR_SAMPLE_COUNT) )
        {
            i32BaseLevelCount = g_ui32SampleCount;

            //
            // Compute the voltage divider output
            //
            fVBATT = ((float)g_ui16ADCVDD_code) * fReferenceVoltage / (4096.0f); // 12-bit sample

            //
            // Print the voltage divider output.
            //
            am_util_stdio_printf("VBATT (0x%04X) = %.3f V",
                                 g_ui16ADCVDD_code, fVBATT);

            //
            // Convert and scale the temperature.
            // First get the ADC voltage corresponding to temperature.
            //
            fADCTempVolts = (float)g_ui16ADCTEMP_code * fReferenceVoltage / 1024.0f;    // 10-bit sample

            //
            // Now call the HAL routine to convert volts to degrees Celsius.
            //
            float fVT[3];
            fVT[0] = fADCTempVolts;
            fVT[1] = 0.0f;
            fVT[2] = -123.456;
            ui32Retval = am_hal_adc_control(g_ADCHandle, AM_HAL_ADC_REQ_TEMP_CELSIUS_GET, fVT);
            if ( ui32Retval == AM_HAL_STATUS_SUCCESS )
            {
                fADCTempDegreesC = fVT[1];  // Get the temperature

                //
                // Print the temperature sample code, then the temperature
                // in both Celsius and Fahrenheit.
                //
                fTempF = (fADCTempDegreesC * (180.0f / 100.0f)) + 32.0f;
                am_util_stdio_printf(",  TEMP (0x%04X) = %.2f C, %.2f F",
                                     g_ui16ADCTEMP_code, fADCTempDegreesC, fTempF);
            }
            else
            {
                am_util_stdio_printf("Error: am_hal_adc_control returned %d\n", ui32Retval);
            }

            am_util_stdio_printf("\n");

            //
            // Use button 0 to turn on or off the battery load resistor.
            //
#ifdef AM_BSP_GPIO_BUTTON0
            if ( ui32ReadState == 0 )
            {
                am_hal_mcuctrl_status_get(&sMcuctrlStatus);

                if ( sMcuctrlStatus.bBattLoadEnabled )
                {
                    am_util_stdio_printf(" *** Battery Load Resistor was on. Turning OFF.\n");
                    MCUCTRL->ADCBATTLOAD_b.BATTLOAD = MCUCTRL_ADCBATTLOAD_BATTLOAD_DIS;
                    am_devices_led_on(am_bsp_psLEDs, LED_VBATT_LOAD);
                }
                else
                {
                    am_util_stdio_printf(" *** Battery Load Resistor was off. Turning ON.\n");
                    MCUCTRL->ADCBATTLOAD_b.BATTLOAD = MCUCTRL_ADCBATTLOAD_BATTLOAD_EN;
                    am_devices_led_off(am_bsp_psLEDs, LED_VBATT_LOAD);
                }
            }
#endif // AM_BSP_GPIO_BUTTON0

#ifdef AM_BSP_GPIO_BUTTON0
            //
            // Wait here until user releases the button
            //
            while ( ui32ReadState == 0 )
            {
                am_hal_gpio_state_read(AM_BSP_GPIO_BUTTON0, AM_HAL_GPIO_INPUT_READ, &ui32ReadState);
            }
#endif // AM_BSP_GPIO_BUTTON0
        }

        //
        // Sleep here until the next ADC interrupt comes along.
        //
#ifdef AM_BSP_NUM_LEDS
        am_devices_led_off(am_bsp_psLEDs, LED_DEEPSLEEP);
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
        am_devices_led_on(am_bsp_psLEDs, LED_DEEPSLEEP);
#endif // AM_BSP_NUM_LEDS

        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);

    }
}
