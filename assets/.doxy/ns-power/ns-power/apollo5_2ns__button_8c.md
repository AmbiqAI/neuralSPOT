

# File ns\_button.c



[**FileList**](files.md) **>** [**apollo5**](dir_d179b330ef1d2a1dad64ceb5f2d1a96e.md) **>** [**ns\_button.c**](apollo5_2ns__button_8c.md)

[Go to the source code of this file](apollo5_2ns__button_8c_source.md)



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
|  void | [**am\_gpio0\_001f\_isr**](#function-am_gpio0_001f_isr) (void) <br>_GPIO ISR handler._  |
|  void | [**am\_gpio0\_405f\_isr**](#function-am_gpio0_405f_isr) (void) <br> |
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




### function am\_gpio0\_001f\_isr 

_GPIO ISR handler._ 
```C++
void am_gpio0_001f_isr (
    void
) 
```




<hr>



### function am\_gpio0\_405f\_isr 

```C++
void am_gpio0_405f_isr (
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
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-peripherals/src/apollo5/ns_button.c`

