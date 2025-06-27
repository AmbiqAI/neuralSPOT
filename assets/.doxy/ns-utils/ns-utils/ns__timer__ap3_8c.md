

# File ns\_timer\_ap3.c



[**FileList**](files.md) **>** [**apollo3**](dir_707bd67c77e0cdcc384b3fcaae8a053a.md) **>** [**ns\_timer\_ap3.c**](ns__timer__ap3_8c.md)

[Go to the source code of this file](ns__timer__ap3_8c_source.md)



* `#include "ns_timer.h"`
* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "ns_core.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  [**ns\_timer\_config\_t**](ns__timer_8h.md#typedef-ns_timer_config_t) \* | [**ns\_timer\_config**](#variable-ns_timer_config)  <br> |


## Public Static Attributes

| Type | Name |
| ---: | :--- |
|  am\_hal\_ctimer\_config\_t | [**g\_sTimer**](#variable-g_stimer)   = `/* multi line expression */`<br> |














## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**am\_ctimer\_isr**](#function-am_ctimer_isr) (void) <br> |
|  uint32\_t | [**ns\_timer\_clear**](#function-ns_timer_clear) ([**ns\_timer\_config\_t**](ns__timer_8h.md#typedef-ns_timer_config_t) \* cfg) <br>_Clear timer._  |
|  uint32\_t | [**ns\_timer\_platform\_init**](#function-ns_timer_platform_init) ([**ns\_timer\_config\_t**](ns__timer_8h.md#typedef-ns_timer_config_t) \* cfg) <br> |
|  uint32\_t | [**ns\_us\_ticker\_read**](#function-ns_us_ticker_read) ([**ns\_timer\_config\_t**](ns__timer_8h.md#typedef-ns_timer_config_t) \* cfg) <br>_Read current value of timer._  |
|  void | [**timer0\_handler**](#function-timer0_handler) (void) <br> |
|  void | [**timer1\_handler**](#function-timer1_handler) (void) <br> |
|  void | [**timer2\_handler**](#function-timer2_handler) (void) <br> |
|  void | [**timer3\_handler**](#function-timer3_handler) (void) <br> |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**AP3\_CTIMER\_FREQ\_IN\_MHZ**](ns__timer__ap3_8c.md#define-ap3_ctimer_freq_in_mhz)  `12`<br> |

## Public Attributes Documentation




### variable ns\_timer\_config 

```C++
ns_timer_config_t* ns_timer_config[NS_TIMER_TEMPCO+1];
```




<hr>
## Public Static Attributes Documentation




### variable g\_sTimer 

```C++
am_hal_ctimer_config_t g_sTimer;
```




<hr>
## Public Functions Documentation




### function am\_ctimer\_isr 

```C++
void am_ctimer_isr (
    void
) 
```




<hr>



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





        

<hr>



### function ns\_timer\_platform\_init 

```C++
uint32_t ns_timer_platform_init (
    ns_timer_config_t * cfg
) 
```




<hr>



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





        

<hr>



### function timer0\_handler 

```C++
void timer0_handler (
    void
) 
```




<hr>



### function timer1\_handler 

```C++
void timer1_handler (
    void
) 
```




<hr>



### function timer2\_handler 

```C++
void timer2_handler (
    void
) 
```




<hr>



### function timer3\_handler 

```C++
void timer3_handler (
    void
) 
```




<hr>
## Macro Definition Documentation





### define AP3\_CTIMER\_FREQ\_IN\_MHZ 

```C++
#define AP3_CTIMER_FREQ_IN_MHZ `12`
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-utils/src/apollo3/ns_timer_ap3.c`

