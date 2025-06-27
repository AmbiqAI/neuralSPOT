

# File ns\_peripherals\_psram.h



[**FileList**](files.md) **>** [**includes-api**](dir_fe04d02a8bfca0247bd216a75646089c.md) **>** [**ns\_peripherals\_psram.h**](ns__peripherals__psram_8h.md)

[Go to the source code of this file](ns__peripherals__psram_8h_source.md)

_Enable PSRAM._ [More...](#detailed-description)

* `#include "ns_core.h"`















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**ns\_psram\_config\_t**](structns__psram__config__t.md) <br> |


## Public Types

| Type | Name |
| ---: | :--- |
| enum  | [**psram\_type\_e**](#enum-psram_type_e)  <br> |




## Public Attributes

| Type | Name |
| ---: | :--- |
|  const ns\_core\_api\_t | [**ns\_psram\_V0\_0\_1**](#variable-ns_psram_v0_0_1)  <br> |
|  const ns\_core\_api\_t | [**ns\_psram\_current\_version**](#variable-ns_psram_current_version)  <br> |
|  const ns\_core\_api\_t | [**ns\_psram\_oldest\_supported\_version**](#variable-ns_psram_oldest_supported_version)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**ns\_psram\_init**](#function-ns_psram_init) ([**ns\_psram\_config\_t**](structns__psram__config__t.md) \* cfg) <br>_Initialize PSRAM per the configuration struct._  |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**NS\_PSRAM\_API\_ID**](ns__peripherals__psram_8h.md#define-ns_psram_api_id)  `0xCA000A`<br> |
| define  | [**NS\_PSRAM\_CURRENT\_VERSION**](ns__peripherals__psram_8h.md#define-ns_psram_current_version)  `NS\_PSRAM\_V1\_0\_0`<br> |
| define  | [**NS\_PSRAM\_OLDEST\_SUPPORTED\_VERSION**](ns__peripherals__psram_8h.md#define-ns_psram_oldest_supported_version)  `NS\_PSRAM\_V0\_0\_1`<br> |
| define  | [**NS\_PSRAM\_V0\_0\_1**](ns__peripherals__psram_8h.md#define-ns_psram_v0_0_1)  `{ .major = 0, .minor = 0, .revision = 1 }`<br> |
| define  | [**NS\_PSRAM\_V1\_0\_0**](ns__peripherals__psram_8h.md#define-ns_psram_v1_0_0)  `{ .major = 1, .minor = 0, .revision = 0 }`<br> |

## Detailed Description




**Author:**

Ambiq 




**Version:**

0.1 




**Date:**

2024-04-19




**Copyright:**

Copyright (c) 2024 





    
## Public Types Documentation




### enum psram\_type\_e 

```C++
enum psram_type_e {
    PSRAM_TYPE_HEX = 0,
    PSRAM_TYPE_OCT = 1
};
```




<hr>
## Public Attributes Documentation




### variable ns\_psram\_V0\_0\_1 

```C++
const ns_core_api_t ns_psram_V0_0_1;
```




<hr>



### variable ns\_psram\_current\_version 

```C++
const ns_core_api_t ns_psram_current_version;
```




<hr>



### variable ns\_psram\_oldest\_supported\_version 

```C++
const ns_core_api_t ns_psram_oldest_supported_version;
```




<hr>
## Public Functions Documentation




### function ns\_psram\_init 

_Initialize PSRAM per the configuration struct._ 
```C++
uint32_t ns_psram_init (
    ns_psram_config_t * cfg
) 
```





**Parameters:**


* `ns_psram_config_t*` : psram configuration struct



**Returns:**

uint32\_t 





        

<hr>
## Macro Definition Documentation





### define NS\_PSRAM\_API\_ID 

```C++
#define NS_PSRAM_API_ID `0xCA000A`
```




<hr>



### define NS\_PSRAM\_CURRENT\_VERSION 

```C++
#define NS_PSRAM_CURRENT_VERSION `NS_PSRAM_V1_0_0`
```




<hr>



### define NS\_PSRAM\_OLDEST\_SUPPORTED\_VERSION 

```C++
#define NS_PSRAM_OLDEST_SUPPORTED_VERSION `NS_PSRAM_V0_0_1`
```




<hr>



### define NS\_PSRAM\_V0\_0\_1 

```C++
#define NS_PSRAM_V0_0_1 `{ .major = 0, .minor = 0, .revision = 1 }`
```




<hr>



### define NS\_PSRAM\_V1\_0\_0 

```C++
#define NS_PSRAM_V1_0_0 `{ .major = 1, .minor = 0, .revision = 0 }`
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-peripherals/includes-api/ns_peripherals_psram.h`

