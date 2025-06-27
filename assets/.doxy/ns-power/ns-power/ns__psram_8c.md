

# File ns\_psram.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-peripherals**](dir_62cbd78784261bb0d09981988628a167.md) **>** [**src**](dir_cf9eff0d6bf97258df730d615fa0f132.md) **>** [**ns\_psram.c**](ns__psram_8c.md)

[Go to the source code of this file](ns__psram_8c_source.md)

_Initialize PSRAM per the configuration struct._ [More...](#detailed-description)

* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "ns_core.h"`
* `#include "ns_peripherals_psram.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  const ns\_core\_api\_t | [**ns\_psram\_V0\_0\_1**](#variable-ns_psram_v0_0_1)   = `{.apiId = NS\_PSRAM\_API\_ID, .version = NS\_PSRAM\_V0\_0\_1}`<br> |
|  const ns\_core\_api\_t | [**ns\_psram\_current\_version**](#variable-ns_psram_current_version)   = `/* multi line expression */`<br> |
|  const ns\_core\_api\_t | [**ns\_psram\_oldest\_supported\_version**](#variable-ns_psram_oldest_supported_version)   = `/* multi line expression */`<br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**ns\_psram\_init**](#function-ns_psram_init) ([**ns\_psram\_config\_t**](structns__psram__config__t.md) \* cfg) <br>_Initialize PSRAM per the configuration struct._  |
|  uint32\_t | [**ns\_psram\_platform\_init**](#function-ns_psram_platform_init) ([**ns\_psram\_config\_t**](structns__psram__config__t.md) \* cfg) <br> |




























## Detailed Description




**Author:**

Ambiq 




**Version:**

0.1 




**Date:**

2024-04-19




**Copyright:**

Copyright (c) 2024 





    
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



### function ns\_psram\_platform\_init 

```C++
uint32_t ns_psram_platform_init (
    ns_psram_config_t * cfg
) 
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-peripherals/src/ns_psram.c`

