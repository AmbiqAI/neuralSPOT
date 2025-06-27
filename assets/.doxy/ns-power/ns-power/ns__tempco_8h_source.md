

# File ns\_tempco.h

[**File List**](files.md) **>** [**apollo4**](dir_1c005e3bbb5b4e96ef4b5df2b5884295.md) **>** [**ns\_tempco.h**](ns__tempco_8h.md)

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


