

# File ns\_pmu\_utils.h



[**FileList**](files.md) **>** [**includes-api**](dir_0f796f8be3b51b94a477512418b4fa0e.md) **>** [**ns\_pmu\_utils.h**](ns__pmu__utils_8h.md)

[Go to the source code of this file](ns__pmu__utils_8h_source.md)

_Utilities for setting up and using the Performance Monitoring Unit._ [More...](#detailed-description)
















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**ns\_pmu\_config**](structns__pmu__config.md) <br> |
| struct | [**ns\_pmu\_counter**](structns__pmu__counter.md) <br> |
| struct | [**ns\_pmu\_counters\_t**](structns__pmu__counters__t.md) <br> |
| struct | [**ns\_pmu\_event**](structns__pmu__event.md) <br> |


## Public Types

| Type | Name |
| ---: | :--- |
| typedef int(\* | [**invoke\_fp**](#typedef-invoke_fp)  <br> |
| typedef struct [**ns\_pmu\_config**](structns__pmu__config.md) | [**ns\_pmu\_config\_t**](#typedef-ns_pmu_config_t)  <br> |
| typedef struct [**ns\_pmu\_counter**](structns__pmu__counter.md) | [**ns\_pmu\_counter\_t**](#typedef-ns_pmu_counter_t)  <br> |
| enum  | [**ns\_pmu\_event\_counter\_size\_e**](#enum-ns_pmu_event_counter_size_e)  <br> |
| typedef struct [**ns\_pmu\_event**](structns__pmu__event.md) | [**ns\_pmu\_event\_t**](#typedef-ns_pmu_event_t)  <br> |




## Public Attributes

| Type | Name |
| ---: | :--- |
|  const ns\_core\_api\_t | [**ns\_pmu\_V0\_0\_1**](#variable-ns_pmu_v0_0_1)  <br> |
|  const ns\_core\_api\_t | [**ns\_pmu\_V1\_0\_0**](#variable-ns_pmu_v1_0_0)  <br> |
|  const ns\_core\_api\_t | [**ns\_pmu\_current\_version**](#variable-ns_pmu_current_version)  <br> |
|  const ns\_core\_api\_t | [**ns\_pmu\_oldest\_supported\_version**](#variable-ns_pmu_oldest_supported_version)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_delta\_pmu**](#function-ns_delta_pmu) ([**ns\_pmu\_counters\_t**](structns__pmu__counters__t.md) \* s, [**ns\_pmu\_counters\_t**](structns__pmu__counters__t.md) \* e, [**ns\_pmu\_counters\_t**](structns__pmu__counters__t.md) \* d) <br> |
|  void | [**ns\_pmu\_characterize\_function**](#function-ns_pmu_characterize_function) (invoke\_fp func, [**ns\_pmu\_config\_t**](structns__pmu__config.md) \* cfg) <br> |
|  void | [**ns\_pmu\_event\_create**](#function-ns_pmu_event_create) ([**ns\_pmu\_event\_t**](structns__pmu__event.md) \* event, uint32\_t eventId, ns\_pmu\_event\_counter\_size\_e counterSize) <br> |
|  uint32\_t | [**ns\_pmu\_get\_counters**](#function-ns_pmu_get_counters) ([**ns\_pmu\_config\_t**](structns__pmu__config.md) \* cfg) <br>_Get PMU counter values._  |
|  void | [**ns\_pmu\_get\_name**](#function-ns_pmu_get_name) ([**ns\_pmu\_config\_t**](structns__pmu__config.md) \* cfg, uint32\_t i, char \* name) <br> |
|  uint32\_t | [**ns\_pmu\_init**](#function-ns_pmu_init) ([**ns\_pmu\_config\_t**](structns__pmu__config.md) \* cfg) <br>_Initialize and start the PMU profiling syste._  |
|  uint32\_t | [**ns\_pmu\_print\_counters**](#function-ns_pmu_print_counters) ([**ns\_pmu\_config\_t**](structns__pmu__config.md) \* cfg) <br> |
|  void | [**ns\_pmu\_reset\_config**](#function-ns_pmu_reset_config) ([**ns\_pmu\_config\_t**](structns__pmu__config.md) \* cfg) <br> |
|  void | [**ns\_pmu\_reset\_counters**](#function-ns_pmu_reset_counters) () <br> |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**NS\_PMU\_API\_ID**](ns__pmu__utils_8h.md#define-ns_pmu_api_id)  `0xCA000B`<br> |
| define  | [**NS\_PMU\_CURRENT\_VERSION**](ns__pmu__utils_8h.md#define-ns_pmu_current_version)  `NS\_PMU\_V1\_0\_0`<br> |
| define  | [**NS\_PMU\_OLDEST\_SUPPORTED\_VERSION**](ns__pmu__utils_8h.md#define-ns_pmu_oldest_supported_version)  `NS\_PMU\_V0\_0\_1`<br> |
| define  | [**NS\_PMU\_V0\_0\_1**](ns__pmu__utils_8h.md#define-ns_pmu_v0_0_1)  `{ .major = 0, .minor = 0, .revision = 1 }`<br> |
| define  | [**NS\_PMU\_V1\_0\_0**](ns__pmu__utils_8h.md#define-ns_pmu_v1_0_0)  `{ .major = 1, .minor = 0, .revision = 0 }`<br> |

## Detailed Description




**Author:**

Ambiq




**Version:**

0.1 




**Date:**

2024-08-16




**Copyright:**

Copyright (c) 2024 





    
## Public Types Documentation




### typedef invoke\_fp 

```C++
typedef int(* invoke_fp) ();
```




<hr>



### typedef ns\_pmu\_config\_t 

```C++
typedef struct ns_pmu_config ns_pmu_config_t;
```




<hr>



### typedef ns\_pmu\_counter\_t 

```C++
typedef struct ns_pmu_counter ns_pmu_counter_t;
```




<hr>



### enum ns\_pmu\_event\_counter\_size\_e 

```C++
enum ns_pmu_event_counter_size_e {
    NS_PMU_EVENT_COUNTER_SIZE_16 = 0,
    NS_PMU_EVENT_COUNTER_SIZE_32 = 1
};
```




<hr>



### typedef ns\_pmu\_event\_t 

```C++
typedef struct ns_pmu_event ns_pmu_event_t;
```




<hr>
## Public Attributes Documentation




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



### variable ns\_pmu\_oldest\_supported\_version 

```C++
const ns_core_api_t ns_pmu_oldest_supported_version;
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
## Macro Definition Documentation





### define NS\_PMU\_API\_ID 

```C++
#define NS_PMU_API_ID `0xCA000B`
```




<hr>



### define NS\_PMU\_CURRENT\_VERSION 

```C++
#define NS_PMU_CURRENT_VERSION `NS_PMU_V1_0_0`
```




<hr>



### define NS\_PMU\_OLDEST\_SUPPORTED\_VERSION 

```C++
#define NS_PMU_OLDEST_SUPPORTED_VERSION `NS_PMU_V0_0_1`
```




<hr>



### define NS\_PMU\_V0\_0\_1 

```C++
#define NS_PMU_V0_0_1 `{ .major = 0, .minor = 0, .revision = 1 }`
```




<hr>



### define NS\_PMU\_V1\_0\_0 

```C++
#define NS_PMU_V1_0_0 `{ .major = 1, .minor = 0, .revision = 0 }`
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-utils/includes-api/ns_pmu_utils.h`

