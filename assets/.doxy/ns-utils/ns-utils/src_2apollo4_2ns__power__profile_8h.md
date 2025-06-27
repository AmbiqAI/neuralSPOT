

# File ns\_power\_profile.h



[**FileList**](files.md) **>** [**apollo4**](dir_f4b6e016fb89d71f6ddac0c53471985d.md) **>** [**ns\_power\_profile.h**](src_2apollo4_2ns__power__profile_8h.md)

[Go to the source code of this file](src_2apollo4_2ns__power__profile_8h_source.md)

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

















































## Macros

| Type | Name |
| ---: | :--- |
| define  | [**AI0**](src_2apollo4_2ns__power__profile_8h.md#define-ai0)  `(\*(volatile uint32\_t \*)0x400401FC)`<br> |
| define  | [**AI1**](src_2apollo4_2ns__power__profile_8h.md#define-ai1)  `(\*(volatile uint32\_t \*)0x400400F8)`<br> |
| define  | [**AI2**](src_2apollo4_2ns__power__profile_8h.md#define-ai2)  `(\*(volatile uint32\_t \*)0x40040144)`<br> |
| define  | [**AI3**](src_2apollo4_2ns__power__profile_8h.md#define-ai3)  `(\*(volatile uint32\_t \*)0x40040278)`<br> |

## Macro Definition Documentation





### define AI0 

```C++
#define AI0 `(*(volatile uint32_t *)0x400401FC)`
```




<hr>



### define AI1 

```C++
#define AI1 `(*(volatile uint32_t *)0x400400F8)`
```




<hr>



### define AI2 

```C++
#define AI2 `(*(volatile uint32_t *)0x40040144)`
```




<hr>



### define AI3 

```C++
#define AI3 `(*(volatile uint32_t *)0x40040278)`
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-utils/src/apollo4/ns_power_profile.h`

