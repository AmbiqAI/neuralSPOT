

# File ns\_tempco.h

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-peripherals**](dir_62cbd78784261bb0d09981988628a167.md) **>** [**src**](dir_cf9eff0d6bf97258df730d615fa0f132.md) **>** [**ns\_tempco.h**](ns__tempco_8h.md)

[Go to the documentation of this file](ns__tempco_8h.md)

```C++


#if defined(NS_AMBIQSUITE_VERSION_R4_1_0) || defined(AM_PART_APOLLO4L)
// TEMPCO not supported in this version
#else

    #ifndef NS_TEMPCO
        #define NS_TEMPCO

        #ifdef __cplusplus
extern "C" {
        #endif

        #include "am_bsp.h"
        #include "am_mcu_apollo.h"
        #include "am_util.h"
        #include "ns_ambiqsuite_harness.h"

//*****************************************************************************
//
// Globals
//
//*****************************************************************************
extern uint32_t g_ns_tempco_ui32TempcoIsrRet;
extern uint32_t g_ns_tempco_ui32TempcoADCslot;
extern am_hal_adc_sample_t g_ns_tempco_sSamples[AM_HAL_TEMPCO_NUMSAMPLES];
extern void *g_ns_tempco_ADCHandle;
        #define TEMPCO_ADC_TEMPERATURE_SLOT 5

extern uint32_t ns_tempco_init(void);

        #ifdef __cplusplus
}
        #endif
    #endif
#endif

```

