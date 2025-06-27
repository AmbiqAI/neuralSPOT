

# File ns\_power.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-peripherals**](dir_62cbd78784261bb0d09981988628a167.md) **>** [**src**](dir_cf9eff0d6bf97258df730d615fa0f132.md) **>** [**ns\_power.c**](ns__power_8c.md)

[Go to the source code of this file](ns__power_8c_source.md)



* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "ns_core.h"`
* `#include "ns_peripherals_power.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  const [**ns\_power\_config\_t**](structns__power__config__t.md) | [**ns\_audio\_default**](#variable-ns_audio_default)  <br>_Good for AI that uses audio peripherals._  |
|  const [**ns\_power\_config\_t**](structns__power__config__t.md) | [**ns\_debug\_default**](#variable-ns_debug_default)  <br>_Enables all things._  |
|  const [**ns\_power\_config\_t**](structns__power__config__t.md) | [**ns\_development\_default**](#variable-ns_development_default)  <br>_Enables most things._  |
|  const [**ns\_power\_config\_t**](structns__power__config__t.md) | [**ns\_good\_default**](#variable-ns_good_default)  <br>_Reasonable settings for more applications._  |
|  const [**ns\_power\_config\_t**](structns__power__config__t.md) | [**ns\_mlperf\_mode1**](#variable-ns_mlperf_mode1)  <br>_Good power/perf setting._  |
|  const [**ns\_power\_config\_t**](structns__power__config__t.md) | [**ns\_mlperf\_mode2**](#variable-ns_mlperf_mode2)  <br>_Good power/perf setting._  |
|  const [**ns\_power\_config\_t**](structns__power__config__t.md) | [**ns\_mlperf\_mode3**](#variable-ns_mlperf_mode3)  <br>_Good power/perf setting._  |
|  const ns\_core\_api\_t | [**ns\_power\_V0\_0\_1**](#variable-ns_power_v0_0_1)  <br>_Power Control Utilities._  |
|  const ns\_core\_api\_t | [**ns\_power\_V1\_0\_0**](#variable-ns_power_v1_0_0)  <br> |
|  const ns\_core\_api\_t | [**ns\_power\_current\_version**](#variable-ns_power_current_version)  <br> |
|  const ns\_core\_api\_t | [**ns\_power\_oldest\_supported\_version**](#variable-ns_power_oldest_supported_version)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_deep\_sleep**](#function-ns_deep_sleep) (void) <br>_neuralSPOT-aware deep\_sleep - turns off certain systems off before sleeping and turns them back upon waking._  |
|  void | [**ns\_platform\_deep\_sleep**](#function-ns_platform_deep_sleep) (void) <br>_Wraps am\_hal\_sysctrl\_sleep to enable and disable systems as needed._  |
|  uint32\_t | [**ns\_power\_config**](#function-ns_power_config) (const [**ns\_power\_config\_t**](structns__power__config__t.md) \* pCfg) <br>_Set SOC Power Mode._  |
|  uint32\_t | [**ns\_power\_platform\_config**](#function-ns_power_platform_config) (const [**ns\_power\_config\_t**](structns__power__config__t.md) \* pCfg) <br> |
|  uint32\_t | [**ns\_set\_performance\_mode**](#function-ns_set_performance_mode) ([**ns\_power\_mode\_e**](ns__peripherals__power_8h.md#enum-ns_power_mode_e) eAIPowerMode) <br>_Sets CPU frequency to one of the ns\_power\_modes._  |




























## Public Attributes Documentation




### variable ns\_audio\_default 

_Good for AI that uses audio peripherals._ 
```C++
const ns_power_config_t ns_audio_default;
```




<hr>



### variable ns\_debug\_default 

_Enables all things._ 
```C++
const ns_power_config_t ns_debug_default;
```




<hr>



### variable ns\_development\_default 

_Enables most things._ 
```C++
const ns_power_config_t ns_development_default;
```




<hr>



### variable ns\_good\_default 

_Reasonable settings for more applications._ 
```C++
const ns_power_config_t ns_good_default;
```




<hr>



### variable ns\_mlperf\_mode1 

_Good power/perf setting._ 
```C++
const ns_power_config_t ns_mlperf_mode1;
```




<hr>



### variable ns\_mlperf\_mode2 

_Good power/perf setting._ 
```C++
const ns_power_config_t ns_mlperf_mode2;
```




<hr>



### variable ns\_mlperf\_mode3 

_Good power/perf setting._ 
```C++
const ns_power_config_t ns_mlperf_mode3;
```




<hr>



### variable ns\_power\_V0\_0\_1 

_Power Control Utilities._ 
```C++
const ns_core_api_t ns_power_V0_0_1;
```




<hr>



### variable ns\_power\_V1\_0\_0 

```C++
const ns_core_api_t ns_power_V1_0_0;
```




<hr>



### variable ns\_power\_current\_version 

```C++
const ns_core_api_t ns_power_current_version;
```




<hr>



### variable ns\_power\_oldest\_supported\_version 

```C++
const ns_core_api_t ns_power_oldest_supported_version;
```




<hr>
## Public Functions Documentation




### function ns\_deep\_sleep 

_neuralSPOT-aware deep\_sleep - turns off certain systems off before sleeping and turns them back upon waking._ 
```C++
void ns_deep_sleep (
    void
) 
```




<hr>



### function ns\_platform\_deep\_sleep 

_Wraps am\_hal\_sysctrl\_sleep to enable and disable systems as needed._ 
```C++
void ns_platform_deep_sleep (
    void
) 
```




<hr>



### function ns\_power\_config 

_Set SOC Power Mode._ 
```C++
uint32_t ns_power_config (
    const ns_power_config_t * pCfg
) 
```





**Parameters:**


* [**ns\_power\_config\_t**](structns__power__config__t.md) Desired power mode 



**Returns:**

uint32\_t success/failure 





        

<hr>



### function ns\_power\_platform\_config 

```C++
uint32_t ns_power_platform_config (
    const ns_power_config_t * pCfg
) 
```




<hr>



### function ns\_set\_performance\_mode 

_Sets CPU frequency to one of the ns\_power\_modes._ 
```C++
uint32_t ns_set_performance_mode (
    ns_power_mode_e eAIPowerMode
) 
```





**Parameters:**


* `eAIPowerMode` 



**Returns:**

uint32\_t status


Sets CPU frequency to one of the ns\_power\_modes. 


        

<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-peripherals/src/ns_power.c`

