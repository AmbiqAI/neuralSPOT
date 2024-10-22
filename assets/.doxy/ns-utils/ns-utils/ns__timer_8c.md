

# File ns\_timer.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-utils**](dir_8caed56d1b8d43fb57ec0577c38aa59e.md) **>** [**src**](dir_5922fa0bec7bd191dd0e3ff5da447491.md) **>** [**ns\_timer.c**](ns__timer_8c.md)

[Go to the source code of this file](ns__timer_8c_source.md)

_Simple timer facility._ [More...](#detailed-description)

* `#include "ns_timer.h"`
* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "ns_core.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  const ns\_core\_api\_t | [**ns\_timer\_V0\_0\_1**](#variable-ns_timer_v0_0_1)  <br> |
|  const ns\_core\_api\_t | [**ns\_timer\_V1\_0\_0**](#variable-ns_timer_v1_0_0)  <br> |
|  [**ns\_timer\_config\_t**](ns__timer_8h.md#typedef-ns_timer_config_t) \* | [**ns\_timer\_config**](#variable-ns_timer_config)  <br> |
|  const ns\_core\_api\_t | [**ns\_timer\_current\_version**](#variable-ns_timer_current_version)  <br> |
|  const ns\_core\_api\_t | [**ns\_timer\_oldest\_supported\_version**](#variable-ns_timer_oldest_supported_version)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  bool | [**ns\_core\_initialized**](#function-ns_core_initialized) (void) <br> |
|  uint32\_t | [**ns\_timer\_init**](#function-ns_timer_init) ([**ns\_timer\_config\_t**](ns__timer_8h.md#typedef-ns_timer_config_t) \* cfg) <br>_Initialize one of 3 timers supported by NeuralSPOT._  |
|  uint32\_t | [**ns\_timer\_platform\_init**](#function-ns_timer_platform_init) ([**ns\_timer\_config\_t**](ns__timer_8h.md#typedef-ns_timer_config_t) \* cfg) <br> |




























# Detailed Description




**Author:**

Carlos Morales 




**Version:**

0.1 




**Date:**

2022-10-11




**Copyright:**

Copyright (c) 2022 





    
## Public Attributes Documentation




### variable ns\_timer\_V0\_0\_1 

```C++
const ns_core_api_t ns_timer_V0_0_1;
```






### variable ns\_timer\_V1\_0\_0 

```C++
const ns_core_api_t ns_timer_V1_0_0;
```






### variable ns\_timer\_config 

```C++
ns_timer_config_t* ns_timer_config[NS_TIMER_TEMPCO+1];
```






### variable ns\_timer\_current\_version 

```C++
const ns_core_api_t ns_timer_current_version;
```






### variable ns\_timer\_oldest\_supported\_version 

```C++
const ns_core_api_t ns_timer_oldest_supported_version;
```



## Public Functions Documentation




### function ns\_core\_initialized 

```C++
bool ns_core_initialized (
    void
) 
```






### function ns\_timer\_init 

_Initialize one of 3 timers supported by NeuralSPOT._ 
```C++
uint32_t ns_timer_init (
    ns_timer_config_t * cfg
) 
```



NS\_TIMER\_COUNTER Intended use is reading timerticks NS\_TIMER\_INTERRUPT Calls a callback periodically NS\_TIMER\_USB Used by ns\_usb to periodically service USB NS\_TIMER\_TEMPCO Used by ns\_tempco to periodically collect temps




**Parameters:**


* `cfg` 



**Returns:**

uint32\_t status 





        



### function ns\_timer\_platform\_init 

```C++
uint32_t ns_timer_platform_init (
    ns_timer_config_t * cfg
) 
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-utils/src/ns_timer.c`

