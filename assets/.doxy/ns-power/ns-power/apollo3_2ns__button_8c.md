

# File ns\_button.c



[**FileList**](files.md) **>** [**apollo3**](dir_96b1dd14f5e69a1b588911fee16f56b3.md) **>** [**ns\_button.c**](apollo3_2ns__button_8c.md)

[Go to the source code of this file](apollo3_2ns__button_8c_source.md)



* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "ns_core.h"`
* `#include "ns_peripherals_button.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  int volatile \* | [**g\_ns\_peripheral\_button0**](#variable-g_ns_peripheral_button0)  <br> |
|  int volatile \* | [**g\_ns\_peripheral\_button1**](#variable-g_ns_peripheral_button1)  <br> |
|  int volatile \* | [**g\_ns\_peripheral\_joulescope\_trigger**](#variable-g_ns_peripheral_joulescope_trigger)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**am\_gpio\_isr**](#function-am_gpio_isr) (void) <br>_GPIO Button0 ISR handler._  |
|  void | [**ns\_button\_0\_handler**](#function-ns_button_0_handler) (void \* pArg) <br> |
|  void | [**ns\_button\_1\_handler**](#function-ns_button_1_handler) (void \* pArg) <br> |
|  uint32\_t | [**ns\_button\_platform\_init**](#function-ns_button_platform_init) ([**ns\_button\_config\_t**](structns__button__config__t.md) \* cfg) <br> |
|  void | [**ns\_joulescope\_trigger\_handler**](#function-ns_joulescope_trigger_handler) (void \* pArg) <br> |




























## Public Attributes Documentation




### variable g\_ns\_peripheral\_button0 

```C++
int volatile* g_ns_peripheral_button0;
```




<hr>



### variable g\_ns\_peripheral\_button1 

```C++
int volatile* g_ns_peripheral_button1;
```




<hr>



### variable g\_ns\_peripheral\_joulescope\_trigger 

```C++
int volatile* g_ns_peripheral_joulescope_trigger;
```




<hr>
## Public Functions Documentation




### function am\_gpio\_isr 

_GPIO Button0 ISR handler._ 
```C++
void am_gpio_isr (
    void
) 
```




<hr>



### function ns\_button\_0\_handler 

```C++
void ns_button_0_handler (
    void * pArg
) 
```




<hr>



### function ns\_button\_1\_handler 

```C++
void ns_button_1_handler (
    void * pArg
) 
```




<hr>



### function ns\_button\_platform\_init 

```C++
uint32_t ns_button_platform_init (
    ns_button_config_t * cfg
) 
```




<hr>



### function ns\_joulescope\_trigger\_handler 

```C++
void ns_joulescope_trigger_handler (
    void * pArg
) 
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-peripherals/src/apollo3/ns_button.c`

