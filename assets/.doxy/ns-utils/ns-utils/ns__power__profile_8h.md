

# File ns\_power\_profile.h



[**FileList**](files.md) **>** [**includes-api**](dir_0f796f8be3b51b94a477512418b4fa0e.md) **>** [**ns\_power\_profile.h**](ns__power__profile_8h.md)

[Go to the source code of this file](ns__power__profile_8h_source.md)

_Functions to aid power profiling and debugging._ 

* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include <stdbool.h>`
* `#include <stdint.h>`















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**am\_bsp\_pp\_b1\_t**](structam__bsp__pp__b1__t.md) <br> |
| struct | [**am\_bsp\_pp\_b2\_t**](structam__bsp__pp__b2__t.md) <br> |
| struct | [**am\_bsp\_pp\_b3\_t**](structam__bsp__pp__b3__t.md) <br> |
| struct | [**am\_bsp\_pp\_b4\_t**](structam__bsp__pp__b4__t.md) <br> |






















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_pp\_snapshot**](#function-ns_pp_snapshot) (bool bDebug, uint32\_t uNumber) <br> |
|  void | [**ns\_print\_JSON**](#function-ns_print_json) () <br>_Prints the filled up JSON to serial port._  |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**AI0**](ns__power__profile_8h.md#define-ai0)  (\*(volatile uint32\_t \*)0x400401FC)<br> |
| define  | [**AI1**](ns__power__profile_8h.md#define-ai1)  (\*(volatile uint32\_t \*)0x400400F8)<br> |
| define  | [**AI2**](ns__power__profile_8h.md#define-ai2)  (\*(volatile uint32\_t \*)0x40040144)<br> |
| define  | [**AI3**](ns__power__profile_8h.md#define-ai3)  (\*(volatile uint32\_t \*)0x40040278)<br> |

## Public Functions Documentation




### function ns\_pp\_snapshot 

```C++
void ns_pp_snapshot (
    bool bDebug,
    uint32_t uNumber
) 
```






### function ns\_print\_JSON 

_Prints the filled up JSON to serial port._ 
```C++
void ns_print_JSON () 
```





**Parameters:**


* `void` 

This function will print filled JSON fields to serial port.




**Note:**

- before use this function, make sure the UART or SWO is enabled




**Returns:**

None 





        
## Macro Definition Documentation





### define AI0 

```C++
#define AI0 (*(volatile uint32_t *)0x400401FC)
```






### define AI1 

```C++
#define AI1 (*(volatile uint32_t *)0x400400F8)
```






### define AI2 

```C++
#define AI2 (*(volatile uint32_t *)0x40040144)
```






### define AI3 

```C++
#define AI3 (*(volatile uint32_t *)0x40040278)
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-utils/includes-api/ns_power_profile.h`

