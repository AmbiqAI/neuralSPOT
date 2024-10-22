

# Group harness



[**Modules**](modules.md) **>** [**harness**](group__harness.md)










































## Public Functions

| Type | Name |
| ---: | :--- |
|  int32\_t | [**ns\_cryptoless\_itm\_printf\_disable**](#function-ns_cryptoless_itm_printf_disable) (void) <br>_Disable prints enabled by_ [_**ns\_itm\_printf\_enable()**_](ns__ambiqsuite__harness_8h.md#function-ns_itm_printf_enable) __ |
|  void | [**ns\_itm\_printf\_enable**](#function-ns_itm_printf_enable) (void) <br>_Enable ITM prints without needed Crypto to be enabled._  |
|  void | [**ns\_lp\_printf**](#function-ns_lp_printf) (const char \* format, ...) <br>_Low power print that enables only the needed hardware only while needed._  |
|  void | [**ns\_uart\_printf\_enable**](#function-ns_uart_printf_enable) (void) <br>_Enable UART prints in power-sensitive way._  |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**NS\_PUT\_IN\_TCM**](ns__ambiqsuite__harness_8h.md#define-ns_put_in_tcm)  <br> |
| define  | [**ns\_debug\_printf\_disable**](ns__ambiqsuite__harness_8h.md#define-ns_debug_printf_disable)  am\_bsp\_debug\_printf\_disable<br> |
| define  | [**ns\_debug\_printf\_enable**](ns__ambiqsuite__harness_8h.md#define-ns_debug_printf_enable)  am\_bsp\_debug\_printf\_enable<br> |
| define  | [**ns\_delay\_us**](ns__ambiqsuite__harness_8h.md#define-ns_delay_us)  am\_hal\_delay\_us<br> |
| define  | [**ns\_interrupt\_master\_disable**](ns__ambiqsuite__harness_8h.md#define-ns_interrupt_master_disable)  am\_hal\_interrupt\_master\_disable<br> |
| define  | [**ns\_interrupt\_master\_enable**](ns__ambiqsuite__harness_8h.md#define-ns_interrupt_master_enable)  am\_hal\_interrupt\_master\_enable<br> |
| define  | [**ns\_itm\_printf\_disable**](ns__ambiqsuite__harness_8h.md#define-ns_itm_printf_disable)  <br> |
| define  | [**ns\_printf**](ns__ambiqsuite__harness_8h.md#define-ns_printf)  [**ns\_lp\_printf**](ns__ambiqsuite__harness_8h.md#function-ns_lp_printf)<br> |
| define  | [**ns\_uart\_printf\_disable**](ns__ambiqsuite__harness_8h.md#define-ns_uart_printf_disable)  am\_bsp\_uart\_printf\_disable<br> |

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



## Macro Definition Documentation





### define NS\_PUT\_IN\_TCM 

```C++
#define NS_PUT_IN_TCM 
```






### define ns\_debug\_printf\_disable 

```C++
#define ns_debug_printf_disable am_bsp_debug_printf_disable
```






### define ns\_debug\_printf\_enable 

```C++
#define ns_debug_printf_enable am_bsp_debug_printf_enable
```






### define ns\_delay\_us 

```C++
#define ns_delay_us am_hal_delay_us
```






### define ns\_interrupt\_master\_disable 

```C++
#define ns_interrupt_master_disable am_hal_interrupt_master_disable
```






### define ns\_interrupt\_master\_enable 

```C++
#define ns_interrupt_master_enable am_hal_interrupt_master_enable
```






### define ns\_itm\_printf\_disable 

```C++
#define ns_itm_printf_disable g_ns_state.itmPrintCurrentlyEnabled = false;                                               \
        am_bsp_itm_printf_disable
```






### define ns\_printf 

```C++
#define ns_printf ns_lp_printf
```






### define ns\_uart\_printf\_disable 

```C++
#define ns_uart_printf_disable am_bsp_uart_printf_disable
```




------------------------------


