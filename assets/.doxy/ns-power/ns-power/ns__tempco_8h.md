

# File ns\_tempco.h



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-peripherals**](dir_62cbd78784261bb0d09981988628a167.md) **>** [**src**](dir_cf9eff0d6bf97258df730d615fa0f132.md) **>** [**ns\_tempco.h**](ns__tempco_8h.md)

[Go to the source code of this file](ns__tempco_8h_source.md)

_Temperature Compensation._ [More...](#detailed-description)

* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "ns_ambiqsuite_harness.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  void \* | [**g\_ns\_tempco\_ADCHandle**](#variable-g_ns_tempco_adchandle)  <br> |
|  am\_hal\_adc\_sample\_t | [**g\_ns\_tempco\_sSamples**](#variable-g_ns_tempco_ssamples)  <br> |
|  uint32\_t | [**g\_ns\_tempco\_ui32TempcoADCslot**](#variable-g_ns_tempco_ui32tempcoadcslot)  <br> |
|  uint32\_t | [**g\_ns\_tempco\_ui32TempcoIsrRet**](#variable-g_ns_tempco_ui32tempcoisrret)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**ns\_tempco\_init**](#function-ns_tempco_init) (void) <br> |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**TEMPCO\_ADC\_TEMPERATURE\_SLOT**](ns__tempco_8h.md#define-tempco_adc_temperature_slot)  5<br> |

# Detailed Description




**Author:**

your name ([you@domain.com](mailto:you@domain.com)) 




**Version:**

0.1 




**Date:**

2022-11-03




**Copyright:**

Copyright (c) 2022 





    
## Public Attributes Documentation




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



## Macro Definition Documentation





### define TEMPCO\_ADC\_TEMPERATURE\_SLOT 

```C++
#define TEMPCO_ADC_TEMPERATURE_SLOT 5
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-peripherals/src/ns_tempco.h`

