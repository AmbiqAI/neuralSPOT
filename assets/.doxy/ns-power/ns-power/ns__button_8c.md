

# File ns\_button.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-peripherals**](dir_62cbd78784261bb0d09981988628a167.md) **>** [**src**](dir_cf9eff0d6bf97258df730d615fa0f132.md) **>** [**ns\_button.c**](ns__button_8c.md)

[Go to the source code of this file](ns__button_8c_source.md)



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
|  const ns\_core\_api\_t | [**ns\_button\_V0\_0\_1**](#variable-ns_button_v0_0_1)   = = {.apiId = NS\_BUTTON\_API\_ID, .version = NS\_BUTTON\_V0\_0\_1}<br> |
|  const ns\_core\_api\_t | [**ns\_button\_V1\_0\_0**](#variable-ns_button_v1_0_0)   = = {.apiId = NS\_BUTTON\_API\_ID, .version = NS\_BUTTON\_V1\_0\_0}<br> |
|  const ns\_core\_api\_t | [**ns\_button\_current\_version**](#variable-ns_button_current_version)   = = {
    .apiId = NS\_BUTTON\_API\_ID, .version = NS\_BUTTON\_V1\_0\_0}<br> |
|  const ns\_core\_api\_t | [**ns\_button\_oldest\_supported\_version**](#variable-ns_button_oldest_supported_version)   = = {
    .apiId = NS\_BUTTON\_API\_ID, .version = NS\_BUTTON\_V0\_0\_1}<br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**am\_gpio0\_001f\_isr**](#function-am_gpio0_001f_isr) (void) <br>_GPIO Button0 ISR handler._  |
|  void | [**ns\_button\_0\_handler**](#function-ns_button_0_handler) (void \* pArg) <br> |
|  void | [**ns\_button\_1\_handler**](#function-ns_button_1_handler) (void \* pArg) <br> |
|  void | [**ns\_joulescope\_trigger\_handler**](#function-ns_joulescope_trigger_handler) (void \* pArg) <br> |
|  uint32\_t | [**ns\_peripheral\_button\_init**](#function-ns_peripheral_button_init) ([**ns\_button\_config\_t**](structns__button__config__t.md) \* cfg) <br>_Initialize button monitoring per the configuration struct._  |




























## Public Attributes Documentation




### variable g\_ns\_peripheral\_button0 

```C++
int volatile* g_ns_peripheral_button0;
```






### variable g\_ns\_peripheral\_button1 

```C++
int volatile* g_ns_peripheral_button1;
```






### variable g\_ns\_peripheral\_joulescope\_trigger 

```C++
int volatile* g_ns_peripheral_joulescope_trigger;
```






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




### function am\_gpio0\_001f\_isr 

```C++
void am_gpio0_001f_isr (
    void
) 
```






### function ns\_button\_0\_handler 

```C++
void ns_button_0_handler (
    void * pArg
) 
```






### function ns\_button\_1\_handler 

```C++
void ns_button_1_handler (
    void * pArg
) 
```






### function ns\_joulescope\_trigger\_handler 

```C++
void ns_joulescope_trigger_handler (
    void * pArg
) 
```






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





        

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-peripherals/src/ns_button.c`

