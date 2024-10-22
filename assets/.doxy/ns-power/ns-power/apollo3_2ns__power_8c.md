

# File ns\_power.c



[**FileList**](files.md) **>** [**apollo3**](dir_96b1dd14f5e69a1b588911fee16f56b3.md) **>** [**ns\_power.c**](apollo3_2ns__power_8c.md)

[Go to the source code of this file](apollo3_2ns__power_8c_source.md)



* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "ns_core.h"`
* `#include "ns_peripherals_power.h"`





































## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_platform\_deep\_sleep**](#function-ns_platform_deep_sleep) (void) <br>_Wraps am\_hal\_sysctrl\_sleep to enable and disable systems as needed._  |
|  void | [**ns\_power\_down\_peripherals**](#function-ns_power_down_peripherals) (const [**ns\_power\_config\_t**](structns__power__config__t.md) \* pCfg) <br> |
|  uint32\_t | [**ns\_power\_platform\_config**](#function-ns_power_platform_config) (const [**ns\_power\_config\_t**](structns__power__config__t.md) \* pCfg) <br> |
|  uint32\_t | [**ns\_set\_performance\_mode**](#function-ns_set_performance_mode) ([**ns\_power\_mode\_e**](ns__peripherals__power_8h.md#enum-ns_power_mode_e) eAIPowerMode) <br>_Sets CPU frequency to one of the ns\_power\_modes._  |




























## Public Functions Documentation




### function ns\_platform\_deep\_sleep 

```C++
void ns_platform_deep_sleep (
    void
) 
```






### function ns\_power\_down\_peripherals 

```C++
void ns_power_down_peripherals (
    const ns_power_config_t * pCfg
) 
```






### function ns\_power\_platform\_config 

```C++
uint32_t ns_power_platform_config (
    const ns_power_config_t * pCfg
) 
```






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


        

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-peripherals/src/apollo3/ns_power.c`

