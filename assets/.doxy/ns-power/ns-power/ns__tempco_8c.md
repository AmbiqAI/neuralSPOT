

# File ns\_tempco.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-peripherals**](dir_62cbd78784261bb0d09981988628a167.md) **>** [**src**](dir_cf9eff0d6bf97258df730d615fa0f132.md) **>** [**ns\_tempco.c**](ns__tempco_8c.md)

[Go to the source code of this file](ns__tempco_8c_source.md)

_Temperature Compensation._ [More...](#detailed-description)

* `#include "ns_tempco.h"`
* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "ns_ambiqsuite_harness.h"`
* `#include "ns_core.h"`
* `#include "ns_timer.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  ns\_timer\_config\_t | [**g\_ns\_tempcoTimer**](#variable-g_ns_tempcotimer)   = = {
    .api = &ns\_timer\_V1\_0\_0,
    .timer = NS\_TIMER\_TEMPCO,
    .enableInterrupt = true,
    .periodInMicroseconds = 10 \* 1024, 
    .callback = ns\_tempco\_callback}<br> |
|  void \* | [**g\_ns\_tempco\_ADCHandle**](#variable-g_ns_tempco_adchandle)  <br> |
|  am\_hal\_adc\_sample\_t | [**g\_ns\_tempco\_sSamples**](#variable-g_ns_tempco_ssamples)  <br> |
|  uint32\_t | [**g\_ns\_tempco\_ui32TempcoADCslot**](#variable-g_ns_tempco_ui32tempcoadcslot)  <br> |
|  uint32\_t | [**g\_ns\_tempco\_ui32TempcoIsrRet**](#variable-g_ns_tempco_ui32tempcoisrret)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**ns\_tempco\_init**](#function-ns_tempco_init) (void) <br> |


## Public Static Functions

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**adc\_temperature\_samples\_get**](#function-adc_temperature_samples_get) (uint32\_t ui32NumSamples, am\_hal\_adc\_sample\_t sSamples) <br> |
|  void | [**adc\_trigger\_wait**](#function-adc_trigger_wait) (void) <br> |
|  void | [**ns\_tempco\_callback**](#function-ns_tempco_callback) (ns\_timer\_config\_t \* c) <br> |


























# Detailed Description




**Author:**

Carlos Morales 




**Version:**

0.1 




**Date:**

2022-11-03




**Copyright:**

Copyright (c) 2022 





    
## Public Attributes Documentation




### variable g\_ns\_tempcoTimer 

```C++
ns_timer_config_t g_ns_tempcoTimer;
```






### variable g\_ns\_tempco\_ADCHandle 

```C++
void* g_ns_tempco_ADCHandle;
```






### variable g\_ns\_tempco\_sSamples 

```C++
am_hal_adc_sample_t g_ns_tempco_sSamples[AM_HAL_TEMPCO_NUMSAMPLES];
```






### variable g\_ns\_tempco\_ui32TempcoADCslot 

```C++
uint32_t g_ns_tempco_ui32TempcoADCslot;
```






### variable g\_ns\_tempco\_ui32TempcoIsrRet 

```C++
uint32_t g_ns_tempco_ui32TempcoIsrRet;
```



## Public Functions Documentation




### function ns\_tempco\_init 

```C++
uint32_t ns_tempco_init (
    void
) 
```



## Public Static Functions Documentation




### function adc\_temperature\_samples\_get 

```C++
static uint32_t adc_temperature_samples_get (
    uint32_t ui32NumSamples,
    am_hal_adc_sample_t sSamples
) 
```






### function adc\_trigger\_wait 

```C++
static void adc_trigger_wait (
    void
) 
```






### function ns\_tempco\_callback 

```C++
static void ns_tempco_callback (
    ns_timer_config_t * c
) 
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-peripherals/src/ns_tempco.c`

