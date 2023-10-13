

# File ns-ambiqsuite-harness.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-harness**](dir_e0d7b3aff6df2cba2f05a768a095730e.md) **>** [**src**](dir_8df671c8e5b7eec7f2ec532421bc80bd.md) **>** [**ns-ambiqsuite-harness.c**](ns-ambiqsuite-harness_8c.md)

[Go to the source code of this file](ns-ambiqsuite-harness_8c_source.md)

_Wrappers for common AmbiqSuite operations._ 

* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "ns_ambiqsuite_harness.h"`





































## Public Functions

| Type | Name |
| ---: | :--- |
|  int32\_t | [**ns\_cryptoless\_itm\_printf\_disable**](#function-ns_cryptoless_itm_printf_disable) (void) <br>_Disable prints enabled by_ [_**ns\_itm\_printf\_enable()**_](ns__ambiqsuite__harness_8h.md#function-ns_itm_printf_enable) __ |
|  int32\_t | [**ns\_cryptoless\_itm\_printf\_enable**](#function-ns_cryptoless_itm_printf_enable) (void) <br>_Special enable that doesn't check crypto/dcu._  |
|  void | [**ns\_itm\_printf\_enable**](#function-ns_itm_printf_enable) (void) <br>_Enable ITM prints without needed Crypto to be enabled._  |
|  void | [**ns\_lp\_printf**](#function-ns_lp_printf) (const char \* format, ...) <br>_Low power print that enables only the needed hardware only while needed._  |
|  void | [**ns\_uart\_printf\_enable**](#function-ns_uart_printf_enable) (void) <br>_Enable UART prints in power-sensitive way._  |




























## Public Functions Documentation




### function ns\_cryptoless\_itm\_printf\_disable 

_Disable prints enabled by_ [_**ns\_itm\_printf\_enable()**_](ns__ambiqsuite__harness_8h.md#function-ns_itm_printf_enable) __
```C++
int32_t ns_cryptoless_itm_printf_disable (
    void
) 
```





**Returns:**

int32\_t


Disable prints enabled by [**ns\_itm\_printf\_enable()**](ns__ambiqsuite__harness_8h.md#function-ns_itm_printf_enable)




**Returns:**

0 on success. 





        



### function ns\_cryptoless\_itm\_printf\_enable 

_Special enable that doesn't check crypto/dcu._ 
```C++
int32_t ns_cryptoless_itm_printf_enable (
    void
) 
```





**Returns:**

0 on success. 





        



### function ns\_itm\_printf\_enable 

_Enable ITM prints without needed Crypto to be enabled._ 
```C++
void ns_itm_printf_enable (
    void
) 
```



Enable ITM prints without needed Crypto to be enabled. 


        



### function ns\_lp\_printf 

_Low power print that enables only the needed hardware only while needed._ 
```C++
void ns_lp_printf (
    const char * format,
    ...
) 
```





**Parameters:**


* `format` 
* `...` 

Low power print that enables only the needed hardware only while needed.




**Parameters:**


* `format` 
* `...` 




        



### function ns\_uart\_printf\_enable 

```C++
void ns_uart_printf_enable (
    void
) 
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-harness/src/ns-ambiqsuite-harness.c`

