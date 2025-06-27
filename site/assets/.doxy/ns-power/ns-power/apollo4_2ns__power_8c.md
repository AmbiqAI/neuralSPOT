

# File ns\_power.c



[**FileList**](files.md) **>** [**apollo4**](dir_1c005e3bbb5b4e96ef4b5df2b5884295.md) **>** [**ns\_power.c**](apollo4_2ns__power_8c.md)

[Go to the source code of this file](apollo4_2ns__power_8c_source.md)



* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "ns_core.h"`
* `#include "ns_peripherals_power.h"`
* `#include "ns_tempco.h"`





































## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_platform\_deep\_sleep**](#function-ns_platform_deep_sleep) (void) <br>_Wraps am\_hal\_sysctrl\_sleep to enable and disable systems as needed._  |
|  void | [**ns\_power\_down\_peripherals**](#function-ns_power_down_peripherals) (const [**ns\_power\_config\_t**](structns__power__config__t.md) \* pCfg) <br> |
|  uint32\_t | [**ns\_power\_platform\_config**](#function-ns_power_platform_config) (const [**ns\_power\_config\_t**](structns__power__config__t.md) \* pCfg) <br> |
|  uint32\_t | [**ns\_set\_performance\_mode**](#function-ns_set_performance_mode) ([**ns\_power\_mode\_e**](ns__peripherals__power_8h.md#enum-ns_power_mode_e) eAIPowerMode) <br>_Sets CPU frequency to one of the ns\_power\_modes._  |




























## Public Functions Documentation




### function ns\_platform\_deep\_sleep 

_Wraps am\_hal\_sysctrl\_sleep to enable and disable systems as needed._ 
```C++
void ns_platform_deep_sleep (
    void
) 
```




<hr>



### function ns\_power\_down\_peripherals 

```C++
void ns_power_down_peripherals (
    const ns_power_config_t * pCfg
) 
```




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
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-peripherals/src/apollo4/ns_power.c`

