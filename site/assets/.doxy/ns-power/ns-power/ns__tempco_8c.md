

# File ns\_tempco.c



[**FileList**](files.md) **>** [**apollo4**](dir_1c005e3bbb5b4e96ef4b5df2b5884295.md) **>** [**ns\_tempco.c**](ns__tempco_8c.md)

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
|  ns\_timer\_config\_t | [**g\_ns\_tempcoTimer**](#variable-g_ns_tempcotimer)   = `/* multi line expression */`<br> |
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


























## Detailed Description




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




<hr>



### variable g\_ns\_tempco\_ADCHandle 

```C++
void* g_ns_tempco_ADCHandle;
```




<hr>



### variable g\_ns\_tempco\_sSamples 

```C++
am_hal_adc_sample_t g_ns_tempco_sSamples[AM_HAL_TEMPCO_NUMSAMPLES];
```




<hr>



### variable g\_ns\_tempco\_ui32TempcoADCslot 

```C++
uint32_t g_ns_tempco_ui32TempcoADCslot;
```




<hr>



### variable g\_ns\_tempco\_ui32TempcoIsrRet 

```C++
uint32_t g_ns_tempco_ui32TempcoIsrRet;
```




<hr>
## Public Functions Documentation




### function ns\_tempco\_init 

```C++
uint32_t ns_tempco_init (
    void
) 
```




<hr>
## Public Static Functions Documentation




### function adc\_temperature\_samples\_get 

```C++
static uint32_t adc_temperature_samples_get (
    uint32_t ui32NumSamples,
    am_hal_adc_sample_t sSamples
) 
```




<hr>



### function adc\_trigger\_wait 

```C++
static void adc_trigger_wait (
    void
) 
```




<hr>



### function ns\_tempco\_callback 

```C++
static void ns_tempco_callback (
    ns_timer_config_t * c
) 
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-peripherals/src/apollo4/ns_tempco.c`

