

# File system\_apollo4p.c



[**FileList**](files.md) **>** [**armclang**](dir_2202e181cb244b86d6f52412849515e1.md) **>** [**system\_apollo4p.c**](system__apollo4p_8c.md)

[Go to the source code of this file](system__apollo4p_8c_source.md)

_Ambiq Micro Apollo4 Plus MCU specific functions._ 

* `#include <stdint.h>`
* `#include "system_apollo4p.h"`
* `#include "apollo4p.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**SystemCoreClock**](#variable-systemcoreclock)   = = \_\_SYSTEM\_CLOCK<br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**SystemCoreClockUpdate**](#function-systemcoreclockupdate) (void) <br>_Set the global clock frequncy._  |
|  void | [**SystemInit**](#function-systeminit) (void) <br>_Initialize the system._  |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**\_\_HSI**](system__apollo4p_8c.md#define-__hsi)  (6000000UL)<br> |
| define  | [**\_\_SYSTEM\_CLOCK**](system__apollo4p_8c.md#define-__system_clock)  (1 \* \_\_SYS\_OSC\_CLK)<br> |
| define  | [**\_\_SYS\_OSC\_CLK**](system__apollo4p_8c.md#define-__sys_osc_clk)  (96000000)<br> |
| define  | [**\_\_XTAL**](system__apollo4p_8c.md#define-__xtal)  (32768UL)<br> |

## Public Attributes Documentation




### variable SystemCoreClock 

```C++
uint32_t SystemCoreClock;
```



## Public Functions Documentation




### function SystemCoreClockUpdate 

_Set the global clock frequncy._ 
```C++
void SystemCoreClockUpdate (
    void
) 
```



This function sets the global clock frequency.




**Returns:**

None. 





        



### function SystemInit 

_Initialize the system._ 
```C++
void SystemInit (
    void
) 
```



This function sets up the microcontroller system.




**Returns:**

None. 





        
## Macro Definition Documentation





### define \_\_HSI 

```C++
#define __HSI (6000000UL)
```






### define \_\_SYSTEM\_CLOCK 

```C++
#define __SYSTEM_CLOCK (1 * __SYS_OSC_CLK)
```






### define \_\_SYS\_OSC\_CLK 

```C++
#define __SYS_OSC_CLK (96000000)
```






### define \_\_XTAL 

```C++
#define __XTAL (32768UL)
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-core/src/armclang/system_apollo4p.c`
