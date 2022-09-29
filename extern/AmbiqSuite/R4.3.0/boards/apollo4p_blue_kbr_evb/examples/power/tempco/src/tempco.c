//*****************************************************************************
//
//! @file tempco.c
//!
//! @brief A brief demonstration of the Temperature Compensation feature.
//!
//! This example initializes and invokes the TempCo feature.
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

//*****************************************************************************
//
// Globals
//
//*****************************************************************************
uint32_t g_ui32TempcoIsrRet;
uint32_t g_ui32TempcoADCslot;
am_hal_adc_sample_t g_sSamples[AM_HAL_TEMPCO_NUMSAMPLES];

//*****************************************************************************
//
// Timer globals
//
//*****************************************************************************
//
// Temperature check interval timer configuration.
//
const static
am_hal_timer_config_t g_TimerCfg_TemperatureUpdateInterval =
{
    .eInputClock        = AM_HAL_TIMER_CLOCK_LFRC,
    .eFunction          = AM_HAL_TIMER_FN_UPCOUNT,
    .bInvertOutput0     = false,
    .bInvertOutput1     = false,
    .eTriggerType       = AM_HAL_TIMER_TRIGGER_DIS,
    .eTriggerSource     = AM_HAL_TIMER_TRIGGER_TMR0_OUT1,
    .bLowJitter         = 0,
    .ui32PatternLimit   = 0,                            // Results in infinite repeats for upcount mode
    .ui32Compare0       = 10 * 1024,                    // Set 10s interval. Timer counter resets to 0 on CMP0
    .ui32Compare1       = 10 * 1024 + 1
};

//*****************************************************************************
//
// ADC globals
//
//*****************************************************************************
#define TEMPCO_ADC_TEMPERATURE_SLOT     5

//
// ADC Device Handle.
//
static void *g_ADCHandle;

//
// ADC Configuration
//
const static am_hal_adc_config_t g_sADC_Cfg =
{
    .eClock         = AM_HAL_ADC_CLKSEL_HFRC_24MHZ,     // Select the ADC Clock source
    .ePolarity      = AM_HAL_ADC_TRIGPOL_RISING,        // Polarity
    .eTrigger       = AM_HAL_ADC_TRIGSEL_SOFTWARE,      // ADC trigger source
    .eClockMode     = AM_HAL_ADC_CLKMODE_LOW_POWER,     // Clock mode
    .ePowerMode     = AM_HAL_ADC_LPMODE1,               // Power mode for idle state
    .eRepeat        = AM_HAL_ADC_SINGLE_SCAN,
    .eRepeatTrigger = AM_HAL_ADC_RPTTRIGSEL_TMR
};

//*****************************************************************************
//
// ADC and TempCo initialization
//
// For use in TempCo, the ADC must be initialized, powered, configured.
// Following the call to am_hal_pwrctrl_tempco_init(), then it must be
// enabled via a call to am_hal_adc_enable().
//
//*****************************************************************************
uint32_t
tempco_init(void)
{
    uint32_t ui32Retval;
    uint32_t ui32TimerNum = 13;

    //
    // Set the slot to use for temperature
    //
    g_ui32TempcoADCslot = TEMPCO_ADC_TEMPERATURE_SLOT;

    //
    // Initialize the ADC and get the handle.
    //
    if ( am_hal_adc_initialize(0, &g_ADCHandle) != AM_HAL_STATUS_SUCCESS )
    {
        am_util_stdio_printf("tempco_init() Error - reservation of the ADC instance failed.\n");
        return 1;
    }

    //
    // Power on the ADC.
    //
    if (am_hal_adc_power_control(g_ADCHandle, AM_HAL_SYSCTRL_WAKE, false) != AM_HAL_STATUS_SUCCESS )
    {
        am_util_stdio_printf("tempco_init() Error - ADC power on failed.\n");
        return 2;
    }

    //
    // Configure the ADC.
    //
    if ( am_hal_adc_configure(g_ADCHandle, (am_hal_adc_config_t*)&g_sADC_Cfg) != AM_HAL_STATUS_SUCCESS )
    {
        am_util_stdio_printf("tempco_init() Error - configuring ADC failed.\n");
        return 3;
    }

    //
    // Configure and init the timer
    //
    ui32Retval = am_hal_timer_config(ui32TimerNum, (am_hal_timer_config_t*)&g_TimerCfg_TemperatureUpdateInterval);
    if ( ui32Retval != AM_HAL_STATUS_SUCCESS )
    {
       return ui32Retval;
    }

    //
    // Enable the timer interrupt
    //
    am_hal_timer_interrupt_enable(TIMER_INTEN_TMR00INT_Msk << (ui32TimerNum * 2));

    NVIC_SetPriority((IRQn_Type)((uint32_t)TIMER0_IRQn + ui32TimerNum), AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ((IRQn_Type)((uint32_t)TIMER0_IRQn + ui32TimerNum));

    //
    // Start the timer
    //
    am_hal_timer_start(ui32TimerNum);

    //
    // Initialize the TempCo low power
    //
    ui32Retval = am_hal_pwrctrl_tempco_init(g_ADCHandle, g_ui32TempcoADCslot);
    if ( ui32Retval != AM_HAL_STATUS_SUCCESS )
    {
        am_util_stdio_printf("ERROR am_hal_pwrctrl_tempco_init() returned %d.\n", ui32Retval);
        return 4;
    }

    //
    // Enable the ADC.
    //
    am_hal_adc_enable(g_ADCHandle);

    return 0;

} // tempco_init()

// ****************************************************************************
// Function to trigger the ADC and wait for a value in the FIFO.
// ****************************************************************************
static void
adc_trigger_wait(void)
{
    //
    // Trigger and wait for something to show up in the FIFO.
    //
    uint32_t ui32Cnt0;

    ui32Cnt0 = _FLD2VAL(ADC_FIFO_COUNT, ADC->FIFO);
    while ( _FLD2VAL(ADC_FIFO_COUNT, ADC->FIFO) == ui32Cnt0 )
    {
        am_hal_adc_sw_trigger(g_ADCHandle);
        am_hal_delay_us(1);
    }
} // adc_trigger_wait()

// ****************************************************************************
// Get samples from temperature sensor.
// ****************************************************************************
static uint32_t
adc_temperature_samples_get(uint32_t ui32NumSamples, am_hal_adc_sample_t sSamples[])
{
    uint32_t ux, ui32OneSample;

    ui32OneSample = 1;

    sSamples[0].ui32Sample = sSamples[1].ui32Sample = sSamples[2].ui32Sample = sSamples[3].ui32Sample = sSamples[4].ui32Sample = 0;
    ux = 0;
    while ( ux < ui32NumSamples )
    {
        adc_trigger_wait();

        //
        // Invalidate DAXI to make sure CPU sees the new data when loaded.
        //
        am_hal_daxi_control(AM_HAL_DAXI_CONTROL_INVALIDATE, NULL);
        am_hal_adc_samples_read(g_ADCHandle, true, NULL, &ui32OneSample, &sSamples[ux]);

        if ( sSamples[ux].ui32Slot == g_ui32TempcoADCslot )
        {
            //
            // This is a temperature sample. Get the next sample.
            //
            ux++;
        }
        else
        {
            //
            // This sample is something other than a temperature sample.
            // It should be handled appropriately.
            //
        }
    }

    return AM_HAL_STATUS_SUCCESS;

} // adc_temperature_samples_get()

//*****************************************************************************
//
// Timer13 ISR
//
//*****************************************************************************
void
am_timer13_isr(void)
{
    //
    // Clear the timer interrupt.
    //
    am_hal_timer_interrupt_clear(TIMER_INTCLR_TMR130INT_Msk | TIMER_INTCLR_TMR131INT_Msk);

    //
    // Power up, configure, and enable the ADC.
    //
    // If the ADC is used for uses in addtion to just temperature measurement,
    // the application would likely provide other steps here such as to check
    // to see if it is already powered up and enabled before going through
    // these steps again.
    // Also in this case, the ADC might not need to be powered down here.
    //
    am_hal_adc_power_control(g_ADCHandle, AM_HAL_SYSCTRL_WAKE, true);

    //
    // Enable the ADC.
    //
    am_hal_adc_enable(g_ADCHandle);

    //
    // Call the TempCo ISR
    // Get temperature samples to send to the ISR
    //
    adc_temperature_samples_get(AM_HAL_TEMPCO_NUMSAMPLES, g_sSamples);

    g_ui32TempcoIsrRet = am_hal_pwrctrl_tempco_sample_handler(AM_HAL_TEMPCO_NUMSAMPLES, g_sSamples);
    if ( g_ui32TempcoIsrRet != AM_HAL_STATUS_SUCCESS )
    {
        //
        // One of the primary reasons the handler might return non-success would
        // be if the device is not enabled for TempCo. A fail condition should be
        // handled appropriately here.
        //
    }

    //
    // Disable and power down the ADC.
    //
    am_hal_adc_power_control(g_ADCHandle, AM_HAL_SYSCTRL_DEEPSLEEP, true);

} // am_timer13_isr()

//*****************************************************************************
//
// Main
//
//*****************************************************************************
int
main(void)
{
    uint32_t ui32Ret;
    float fTempMeasured;

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
    if (am_bsp_debug_printf_enable())
    {
        // Cannot print - so no point proceeding
        while(1);
    }

    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("TempCo example!\n\n");

    am_util_stdio_printf("Begin TempCo initialization.\n");
    am_util_stdio_printf("App Compiler:    %s\n", COMPILER_VERSION);

    //
    // Initialize for TempCo
    //
    ui32Ret = tempco_init();
    if ( ui32Ret )
    {
        am_util_stdio_printf("tempco_init failed, exiting the example.\n");
        while (1);
    }

    //
    // Disable and power down the ADC before deepsleep.
    //
    am_hal_adc_power_control(g_ADCHandle, AM_HAL_SYSCTRL_DEEPSLEEP, true);

    //
    // Global interrupt enable
    //
    am_hal_interrupt_master_enable();

    am_util_stdio_printf("TempCo setup completed.\n");
    am_util_stdio_printf("Sleep until timer wakes for TempCo updates.\n\n");
    am_bsp_debug_printf_disable();

    //
    // Deepsleep loop.
    //
    while (1)
    {
        //
        // Go to deepsleep and wait for the timer to wake for a TempCo adjustment.
        //
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);

        am_bsp_debug_printf_enable();
        am_util_stdio_printf(" Wake for TempCo update. am_hal_pwrctrl_tempco_sample_handler() returned ");
        if ( g_ui32TempcoIsrRet == AM_HAL_STATUS_SUCCESS )
        {
            am_util_stdio_printf("Success!\n");
            am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_TEMPCO_GETMEASTEMP, &fTempMeasured);
            am_util_stdio_printf("  Measured temperature=%.2F C.\n", fTempMeasured);
        }
        else
        {
            am_util_stdio_printf("Error %d\n", g_ui32TempcoIsrRet);
        }

        am_bsp_debug_printf_disable();
    }
} // main()
