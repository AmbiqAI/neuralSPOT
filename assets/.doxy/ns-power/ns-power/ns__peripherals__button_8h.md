

# File ns\_peripherals\_button.h



[**FileList**](files.md) **>** [**includes-api**](dir_fe04d02a8bfca0247bd216a75646089c.md) **>** [**ns\_peripherals\_button.h**](ns__peripherals__button_8h.md)

[Go to the source code of this file](ns__peripherals__button_8h_source.md)



* `#include "ns_core.h"`















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**ns\_button\_config\_t**](structns__button__config__t.md) <br> |






## Public Attributes

| Type | Name |
| ---: | :--- |
|  const ns\_core\_api\_t | [**ns\_button\_V0\_0\_1**](#variable-ns_button_v0_0_1)  <br> |
|  const ns\_core\_api\_t | [**ns\_button\_V1\_0\_0**](#variable-ns_button_v1_0_0)  <br> |
|  const ns\_core\_api\_t | [**ns\_button\_current\_version**](#variable-ns_button_current_version)  <br> |
|  const ns\_core\_api\_t | [**ns\_button\_oldest\_supported\_version**](#variable-ns_button_oldest_supported_version)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**ns\_peripheral\_button\_init**](#function-ns_peripheral_button_init) ([**ns\_button\_config\_t**](structns__button__config__t.md) \* cfg) <br>_Initialize button monitoring per the configuration struct._  |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**NS\_BUTTON\_API\_ID**](ns__peripherals__button_8h.md#define-ns_button_api_id)  0xCA0003<br> |
| define  | [**NS\_BUTTON\_CURRENT\_VERSION**](ns__peripherals__button_8h.md#define-ns_button_current_version)  NS\_BUTTON\_V1\_0\_0<br> |
| define  | [**NS\_BUTTON\_OLDEST\_SUPPORTED\_VERSION**](ns__peripherals__button_8h.md#define-ns_button_oldest_supported_version)  NS\_BUTTON\_V0\_0\_1<br> |
| define  | [**NS\_BUTTON\_V0\_0\_1**](ns__peripherals__button_8h.md#define-ns_button_v0_0_1)      { .major = 0, .minor = 0, .revision = 1 }<br> |
| define  | [**NS\_BUTTON\_V1\_0\_0**](ns__peripherals__button_8h.md#define-ns_button_v1_0_0)      { .major = 1, .minor = 0, .revision = 0 }<br> |

## Public Attributes Documentation




### variable ns\_button\_V0\_0\_1 

```C++
const ns_core_api_t ns_button_V0_0_1;
```






### variable ns\_button\_V1\_0\_0 

```C++
const ns_core_api_t ns_button_V1_0_0;
```






### variable ns\_button\_current\_version 

```C++
const ns_core_api_t ns_button_current_version;
```






### variable ns\_button\_oldest\_supported\_version 

```C++
const ns_core_api_t ns_button_oldest_supported_version;
```



## Public Functions Documentation




### function ns\_peripheral\_button\_init 

_Initialize button monitoring per the configuration struct._ 
```C++
uint32_t ns_peripheral_button_init (
    ns_button_config_t * cfg
) 
```





**Parameters:**


* `ns_button_config_t*` : button configuration struct



**Returns:**

uint32\_t 





        
## Macro Definition Documentation





### define NS\_BUTTON\_API\_ID 

```C++
#define NS_BUTTON_API_ID 0xCA0003
```






### define NS\_BUTTON\_CURRENT\_VERSION 

```C++
#define NS_BUTTON_CURRENT_VERSION NS_BUTTON_V1_0_0
```






### define NS\_BUTTON\_OLDEST\_SUPPORTED\_VERSION 

```C++
#define NS_BUTTON_OLDEST_SUPPORTED_VERSION NS_BUTTON_V0_0_1
```






### define NS\_BUTTON\_V0\_0\_1 

```C++
#define NS_BUTTON_V0_0_1 { .major = 0, .minor = 0, .revision = 1 }
```






### define NS\_BUTTON\_V1\_0\_0 

```C++
#define NS_BUTTON_V1_0_0 { .major = 1, .minor = 0, .revision = 0 }
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-peripherals/includes-api/ns_peripherals_button.h`

