

# File ns\_tempco.c

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-peripherals**](dir_62cbd78784261bb0d09981988628a167.md) **>** [**src**](dir_cf9eff0d6bf97258df730d615fa0f132.md) **>** [**ns\_tempco.c**](ns__tempco_8c.md)

[Go to the documentation of this file](ns__tempco_8c.md)

```C++


#if defined(NS_AMBIQSUITE_VERSION_R4_1_0) || defined(AM_PART_APOLLO4L)
// TEMPCO not supported in this version
#else

    #include "ns_tempco.h"
    #include "am_bsp.h"
    #include "am_mcu_apollo.h"
    #include "am_util.h"
    #include "ns_ambiqsuite_harness.h"
    #include "ns_core.h"
    #include "ns_timer.h"

uint32_t g_ns_tempco_ui32TempcoIsrRet;
uint32_t g_ns_tempco_ui32TempcoADCslot;
am_hal_adc_sample_t g_ns_tempco_sSamples[AM_HAL_TEMPCO_NUMSAMPLES];
void *g_ns_tempco_ADCHandle;

static uint32_t
adc_temperature_samples_get(uint32_t ui32NumSamples, am_hal_adc_sample_t sSamples[]);
static void adc_trigger_wait(void);
static void ns_tempco_callback(ns_timer_config_t *c);

ns_timer_config_t g_ns_tempcoTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_TEMPCO,
    .enableInterrupt = true,
    .periodInMicroseconds = 10 * 1024, // 10 seconds
    .callback = ns_tempco_callback};

//*****************************************************************************
//
// ADC and TempCo initialization
//
// For use in TempCo, the ADC must be initialized, powered, configured.
// Following the call to am_hal_pwrctrl_tempco_init(), then it must be
// enabled via a call to am_hal_adc_enable().
//
//*****************************************************************************
uint32_t ns_tempco_init(void) {
    uint32_t ui32Retval;
    am_hal_adc_config_t sADC_Cfg = {
        .eClock = AM_HAL_ADC_CLKSEL_HFRC_24MHZ,     // Select the ADC Clock source
        .ePolarity = AM_HAL_ADC_TRIGPOL_RISING,     // Polarity
        .eTrigger = AM_HAL_ADC_TRIGSEL_SOFTWARE,    // ADC trigger source
        .eClockMode = AM_HAL_ADC_CLKMODE_LOW_POWER, // Clock mode
        .ePowerMode = AM_HAL_ADC_LPMODE1,           // Power mode for idle state
        .eRepeat = AM_HAL_ADC_SINGLE_SCAN,
        .eRepeatTrigger = AM_HAL_ADC_RPTTRIGSEL_TMR};

    // Set the slot to use for temperature
    g_ns_tempco_ui32TempcoADCslot = TEMPCO_ADC_TEMPERATURE_SLOT;

    // Initialize the ADC and get the handle.
    if (am_hal_adc_initialize(0, &g_ns_tempco_ADCHandle) != AM_HAL_STATUS_SUCCESS) {
        ns_printf("tempco_init() Error - reservation of the ADC instance failed.\n");
        return 1;
    }

    // Power on the ADC.
    if (am_hal_adc_power_control(g_ns_tempco_ADCHandle, AM_HAL_SYSCTRL_WAKE, false) !=
        AM_HAL_STATUS_SUCCESS) {
        ns_printf("tempco_init() Error - ADC power on failed.\n");
        return 2;
    }

    // Configure the ADC.
    if (am_hal_adc_configure(g_ns_tempco_ADCHandle, (am_hal_adc_config_t *)&sADC_Cfg) !=
        AM_HAL_STATUS_SUCCESS) {
        ns_printf("tempco_init() Error - configuring ADC failed.\n");
        return 3;
    }

    // Configure and init the timer
    ns_timer_init(&g_ns_tempcoTimer);

    // Initialize the TempCo low power
    ui32Retval = am_hal_pwrctrl_tempco_init(g_ns_tempco_ADCHandle, g_ns_tempco_ui32TempcoADCslot);
    if (ui32Retval != AM_HAL_STATUS_SUCCESS) {
        ns_printf("ERROR am_hal_pwrctrl_tempco_init() returned %d.\n", ui32Retval);
        return 4;
    }

    // Enable the ADC.
    am_hal_adc_enable(g_ns_tempco_ADCHandle);

    g_ns_state.tempcoCurrentlyEnabled = true;
    g_ns_state.tempcoWantsToBeEnabled = true;

    return 0;
}

// timer ISR callback
static void ns_tempco_callback(ns_timer_config_t *c) {
    // Power up, configure, and enable the ADC.
    am_hal_adc_power_control(g_ns_tempco_ADCHandle, AM_HAL_SYSCTRL_WAKE, true);

    // Enable the ADC.
    am_hal_adc_enable(g_ns_tempco_ADCHandle);

    // Call the TempCo ISR
    // Get temperature samples to send to the ISR
    adc_temperature_samples_get(AM_HAL_TEMPCO_NUMSAMPLES, g_ns_tempco_sSamples);

    g_ns_tempco_ui32TempcoIsrRet =
        am_hal_pwrctrl_tempco_sample_handler(AM_HAL_TEMPCO_NUMSAMPLES, g_ns_tempco_sSamples);
    if (g_ns_tempco_ui32TempcoIsrRet != AM_HAL_STATUS_SUCCESS) {
        //
        // One of the primary reasons the handler might return non-success would
        // be if the device is not enabled for TempCo. A fail condition should be
        // handled appropriately here.
        //
    }

    // Disable and power down the ADC.
    am_hal_adc_power_control(g_ns_tempco_ADCHandle, AM_HAL_SYSCTRL_DEEPSLEEP, true);
}

// ****************************************************************************
// Function to trigger the ADC and wait for a value in the FIFO.
// ****************************************************************************
static void adc_trigger_wait(void) {
    // Trigger and wait for something to show up in the FIFO.
    uint32_t ui32Cnt0;

    ui32Cnt0 = _FLD2VAL(ADC_FIFO_COUNT, ADC->FIFO);
    while (_FLD2VAL(ADC_FIFO_COUNT, ADC->FIFO) == ui32Cnt0) {
        am_hal_adc_sw_trigger(g_ns_tempco_ADCHandle);
        am_hal_delay_us(1);
    }
} // adc_trigger_wait()

// ****************************************************************************
// Get samples from temperature sensor.
// ****************************************************************************
static uint32_t
adc_temperature_samples_get(uint32_t ui32NumSamples, am_hal_adc_sample_t sSamples[]) {
    uint32_t ux, ui32OneSample;

    ui32OneSample = 1;

    sSamples[0].ui32Sample = sSamples[1].ui32Sample = sSamples[2].ui32Sample =
        sSamples[3].ui32Sample = sSamples[4].ui32Sample = 0;
    ux = 0;
    while (ux < ui32NumSamples) {
        adc_trigger_wait();

        // Invalidate DAXI to make sure CPU sees the new data when loaded.
        am_hal_daxi_control(AM_HAL_DAXI_CONTROL_INVALIDATE, NULL);
        am_hal_adc_samples_read(g_ns_tempco_ADCHandle, true, NULL, &ui32OneSample, &sSamples[ux]);

        if (sSamples[ux].ui32Slot == g_ns_tempco_ui32TempcoADCslot) {
            // This is a temperature sample. Get the next sample.
            ux++;
        } else {
            // This sample is something other than a temperature sample.
            // It should be handled appropriately.
        }
    }

    return AM_HAL_STATUS_SUCCESS;

} // adc_temperature_samples_get()

#endif

```

