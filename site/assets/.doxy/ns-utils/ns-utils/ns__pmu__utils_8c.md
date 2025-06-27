

# File ns\_pmu\_utils.c



[**FileList**](files.md) **>** [**apollo5**](dir_4b586f9555feab2a00d1b63aa502548f.md) **>** [**ns\_pmu\_utils.c**](ns__pmu__utils_8c.md)

[Go to the source code of this file](ns__pmu__utils_8c_source.md)

_A collection of functions to collect and analyze performance data._ [More...](#detailed-description)

* `#include "ns_core.h"`
* `#include "ns_pmu_map.h"`
* `#include "ns_pmu_utils.h"`
* `#include <string.h>`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**g\_ns\_pmu\_map\_length**](#variable-g_ns_pmu_map_length)  <br> |
|  const ns\_core\_api\_t | [**ns\_pmu\_V0\_0\_1**](#variable-ns_pmu_v0_0_1)   = `{.apiId = NS\_PMU\_API\_ID, .version = NS\_PMU\_V0\_0\_1}`<br> |
|  const ns\_core\_api\_t | [**ns\_pmu\_V1\_0\_0**](#variable-ns_pmu_v1_0_0)   = `{.apiId = NS\_PMU\_API\_ID, .version = NS\_PMU\_V1\_0\_0}`<br> |
|  const ns\_core\_api\_t | [**ns\_pmu\_current\_version**](#variable-ns_pmu_current_version)   = `/* multi line expression */`<br> |
|  const [**ns\_pmu\_map\_t**](structns__pmu__map__t.md) | [**ns\_pmu\_map**](#variable-ns_pmu_map)  <br> |
|  const ns\_core\_api\_t | [**ns\_pmu\_oldest\_supported\_version**](#variable-ns_pmu_oldest_supported_version)   = `/* multi line expression */`<br> |


## Public Static Attributes

| Type | Name |
| ---: | :--- |
|  am\_util\_pmu\_config\_t | [**ns\_am\_pmu\_config**](#variable-ns_am_pmu_config)  <br> |
|  uint32\_t | [**ns\_pmu\_config\_index**](#variable-ns_pmu_config_index)  <br> |
|  bool | [**ns\_pmu\_initialized**](#variable-ns_pmu_initialized)   = `false`<br> |
|  bool | [**ns\_pmu\_profiling**](#variable-ns_pmu_profiling)   = `false`<br> |














## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_delta\_pmu**](#function-ns_delta_pmu) ([**ns\_pmu\_counters\_t**](structns__pmu__counters__t.md) \* s, [**ns\_pmu\_counters\_t**](structns__pmu__counters__t.md) \* e, [**ns\_pmu\_counters\_t**](structns__pmu__counters__t.md) \* d) <br> |
|  void | [**ns\_pmu\_characterize\_function**](#function-ns_pmu_characterize_function) (invoke\_fp func, [**ns\_pmu\_config\_t**](structns__pmu__config.md) \* cfg) <br> |
|  void | [**ns\_pmu\_event\_create**](#function-ns_pmu_event_create) ([**ns\_pmu\_event\_t**](structns__pmu__event.md) \* event, uint32\_t eventId, ns\_pmu\_event\_counter\_size\_e counterSize) <br> |
|  uint32\_t | [**ns\_pmu\_get\_counters**](#function-ns_pmu_get_counters) ([**ns\_pmu\_config\_t**](structns__pmu__config.md) \* cfg) <br>_Get PMU counter values._  |
|  int | [**ns\_pmu\_get\_map\_index**](#function-ns_pmu_get_map_index) (uint32\_t eventId) <br> |
|  uint32\_t | [**ns\_pmu\_get\_map\_length**](#function-ns_pmu_get_map_length) () <br> |
|  void | [**ns\_pmu\_get\_name**](#function-ns_pmu_get_name) ([**ns\_pmu\_config\_t**](structns__pmu__config.md) \* cfg, uint32\_t i, char \* name) <br> |
|  uint32\_t | [**ns\_pmu\_init**](#function-ns_pmu_init) ([**ns\_pmu\_config\_t**](structns__pmu__config.md) \* cfg) <br>_Initialize and start the PMU profiling syste._  |
|  uint32\_t | [**ns\_pmu\_print\_counters**](#function-ns_pmu_print_counters) ([**ns\_pmu\_config\_t**](structns__pmu__config.md) \* cfg) <br> |
|  void | [**ns\_pmu\_reset\_config**](#function-ns_pmu_reset_config) ([**ns\_pmu\_config\_t**](structns__pmu__config.md) \* cfg) <br> |
|  void | [**ns\_pmu\_reset\_counters**](#function-ns_pmu_reset_counters) () <br> |


## Public Static Functions

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**cntr\_disable**](#function-cntr_disable) (uint32\_t ui32CountersDisable) <br> |
|  uint32\_t | [**cntr\_enable**](#function-cntr_enable) (uint32\_t ui32CountersEnable) <br> |


























## Detailed Description




**Author:**

Ambiq 




**Version:**

0.1 




**Date:**

2024-08-15




**Copyright:**

Copyright (c) 2024 





    
## Public Attributes Documentation




### variable g\_ns\_pmu\_map\_length 

```C++
uint32_t g_ns_pmu_map_length;
```




<hr>



### variable ns\_pmu\_V0\_0\_1 

```C++
const ns_core_api_t ns_pmu_V0_0_1;
```




<hr>



### variable ns\_pmu\_V1\_0\_0 

```C++
const ns_core_api_t ns_pmu_V1_0_0;
```




<hr>



### variable ns\_pmu\_current\_version 

```C++
const ns_core_api_t ns_pmu_current_version;
```




<hr>



### variable ns\_pmu\_map 

```C++
const ns_pmu_map_t ns_pmu_map[];
```




<hr>



### variable ns\_pmu\_oldest\_supported\_version 

```C++
const ns_core_api_t ns_pmu_oldest_supported_version;
```




<hr>
## Public Static Attributes Documentation




### variable ns\_am\_pmu\_config 

```C++
am_util_pmu_config_t ns_am_pmu_config;
```




<hr>



### variable ns\_pmu\_config\_index 

```C++
uint32_t ns_pmu_config_index[8];
```




<hr>



### variable ns\_pmu\_initialized 

```C++
bool ns_pmu_initialized;
```




<hr>



### variable ns\_pmu\_profiling 

```C++
bool ns_pmu_profiling;
```




<hr>
## Public Functions Documentation




### function ns\_delta\_pmu 

```C++
void ns_delta_pmu (
    ns_pmu_counters_t * s,
    ns_pmu_counters_t * e,
    ns_pmu_counters_t * d
) 
```




<hr>



### function ns\_pmu\_characterize\_function 

```C++
void ns_pmu_characterize_function (
    invoke_fp func,
    ns_pmu_config_t * cfg
) 
```




<hr>



### function ns\_pmu\_event\_create 

```C++
void ns_pmu_event_create (
    ns_pmu_event_t * event,
    uint32_t eventId,
    ns_pmu_event_counter_size_e counterSize
) 
```




<hr>



### function ns\_pmu\_get\_counters 

_Get PMU counter values._ 
```C++
uint32_t ns_pmu_get_counters (
    ns_pmu_config_t * cfg
) 
```




<hr>



### function ns\_pmu\_get\_map\_index 

```C++
int ns_pmu_get_map_index (
    uint32_t eventId
) 
```




<hr>



### function ns\_pmu\_get\_map\_length 

```C++
uint32_t ns_pmu_get_map_length () 
```




<hr>



### function ns\_pmu\_get\_name 

```C++
void ns_pmu_get_name (
    ns_pmu_config_t * cfg,
    uint32_t i,
    char * name
) 
```




<hr>



### function ns\_pmu\_init 

_Initialize and start the PMU profiling syste._ 
```C++
uint32_t ns_pmu_init (
    ns_pmu_config_t * cfg
) 
```





**Parameters:**


* `cfg` 



**Returns:**

uint32\_t status 





        

<hr>



### function ns\_pmu\_print\_counters 

```C++
uint32_t ns_pmu_print_counters (
    ns_pmu_config_t * cfg
) 
```




<hr>



### function ns\_pmu\_reset\_config 

```C++
void ns_pmu_reset_config (
    ns_pmu_config_t * cfg
) 
```




<hr>



### function ns\_pmu\_reset\_counters 

```C++
void ns_pmu_reset_counters () 
```




<hr>
## Public Static Functions Documentation




### function cntr\_disable 

```C++
static uint32_t cntr_disable (
    uint32_t ui32CountersDisable
) 
```




<hr>



### function cntr\_enable 

```C++
static uint32_t cntr_enable (
    uint32_t ui32CountersEnable
) 
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-utils/src/apollo5/ns_pmu_utils.c`

