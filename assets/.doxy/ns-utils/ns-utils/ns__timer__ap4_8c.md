

# File ns\_timer\_ap4.c



[**FileList**](files.md) **>** [**apollo4**](dir_f4b6e016fb89d71f6ddac0c53471985d.md) **>** [**ns\_timer\_ap4.c**](ns__timer__ap4_8c.md)

[Go to the source code of this file](ns__timer__ap4_8c_source.md)

_Apollo4-specific timer implementation._ [More...](#detailed-description)

* `#include "ns_timer.h"`
* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "ns_core.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  [**ns\_timer\_config\_t**](ns__timer_8h.md#typedef-ns_timer_config_t) \* | [**ns\_timer\_config**](#variable-ns_timer_config)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**am\_timer01\_isr**](#function-am_timer01_isr) (void) <br> |
|  void | [**am\_timer02\_isr**](#function-am_timer02_isr) (void) <br> |
|  void | [**am\_timer03\_isr**](#function-am_timer03_isr) (void) <br> |
|  uint32\_t | [**ns\_timer\_clear**](#function-ns_timer_clear) ([**ns\_timer\_config\_t**](ns__timer_8h.md#typedef-ns_timer_config_t) \* cfg) <br>_Clear timer._  |
|  uint32\_t | [**ns\_timer\_platform\_init**](#function-ns_timer_platform_init) ([**ns\_timer\_config\_t**](ns__timer_8h.md#typedef-ns_timer_config_t) \* cfg) <br> |
|  uint32\_t | [**ns\_us\_ticker\_read**](#function-ns_us_ticker_read) ([**ns\_timer\_config\_t**](ns__timer_8h.md#typedef-ns_timer_config_t) \* cfg) <br>_Read current value of timer._  |




























# Detailed Description




**Author:**

Carlos Morales 




**Version:**

0.1 




**Date:**

2022-10-11




**Copyright:**

Copyright (c) 2024 





    
## Public Attributes Documentation




### variable ns\_timer\_config 

```C++
ns_timer_config_t* ns_timer_config[NS_TIMER_TEMPCO+1];
```



## Public Functions Documentation




### function am\_timer01\_isr 

```C++
void am_timer01_isr (
    void
) 
```






### function am\_timer02\_isr 

```C++
void am_timer02_isr (
    void
) 
```






### function am\_timer03\_isr 

```C++
void am_timer03_isr (
    void
) 
```






### function ns\_timer\_clear 

_Clear timer._ 
```C++
uint32_t ns_timer_clear (
    ns_timer_config_t * cfg
) 
```





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






### function ns\_us\_ticker\_read 

_Read current value of timer._ 
```C++
uint32_t ns_us_ticker_read (
    ns_timer_config_t * cfg
) 
```





**Parameters:**


* `cfg` 



**Returns:**

uint32\_t timer if success, 0xDEADBEEF if bad handle 





        

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-utils/src/apollo4/ns_timer_ap4.c`

