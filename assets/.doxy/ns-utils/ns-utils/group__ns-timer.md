

# Group ns-timer



[**Modules**](modules.md) **>** [**ns-timer**](group__ns-timer.md)




















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**ns\_timer\_config**](structns__timer__config.md) <br>_ns-timer configuration struct_  |


## Public Types

| Type | Name |
| ---: | :--- |
| typedef void(\* | [**ns\_timer\_callback\_cb**](#typedef-ns_timer_callback_cb)  <br> |
| typedef struct [**ns\_timer\_config**](structns__timer__config.md) | [**ns\_timer\_config\_t**](#typedef-ns_timer_config_t)  <br>_ns-timer configuration struct_  |
| enum  | [**ns\_timers\_e**](#enum-ns_timers_e)  <br>_Supported Timers._  |




## Public Attributes

| Type | Name |
| ---: | :--- |
|  const ns\_core\_api\_t | [**ns\_timer\_V0\_0\_1**](#variable-ns_timer_v0_0_1)  <br> |
|  const ns\_core\_api\_t | [**ns\_timer\_V1\_0\_0**](#variable-ns_timer_v1_0_0)  <br> |
|  const ns\_core\_api\_t | [**ns\_timer\_current\_version**](#variable-ns_timer_current_version)  <br> |
|  const ns\_core\_api\_t | [**ns\_timer\_oldest\_supported\_version**](#variable-ns_timer_oldest_supported_version)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**ns\_timer\_clear**](#function-ns_timer_clear) ([**ns\_timer\_config\_t**](ns__timer_8h.md#typedef-ns_timer_config_t) \* cfg) <br>_Clear timer._  |
|  uint32\_t | [**ns\_timer\_init**](#function-ns_timer_init) ([**ns\_timer\_config\_t**](ns__timer_8h.md#typedef-ns_timer_config_t) \* cfg) <br>_Initialize one of 3 timers supported by NeuralSPOT._  |
|  uint32\_t | [**ns\_us\_ticker\_read**](#function-ns_us_ticker_read) ([**ns\_timer\_config\_t**](ns__timer_8h.md#typedef-ns_timer_config_t) \* cfg) <br>_Read current value of timer._  |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**NS\_TIMER\_API\_ID**](ns__timer_8h.md#define-ns_timer_api_id)  `0xCA0002`<br> |
| define  | [**NS\_TIMER\_CURRENT\_VERSION**](ns__timer_8h.md#define-ns_timer_current_version)  `NS\_TIMER\_V1\_0\_0`<br> |
| define  | [**NS\_TIMER\_OLDEST\_SUPPORTED\_VERSION**](ns__timer_8h.md#define-ns_timer_oldest_supported_version)  `NS\_TIMER\_V0\_0\_1`<br> |
| define  | [**NS\_TIMER\_V0\_0\_1**](ns__timer_8h.md#define-ns_timer_v0_0_1)  `{ .major = 0, .minor = 0, .revision = 1 }`<br> |
| define  | [**NS\_TIMER\_V1\_0\_0**](ns__timer_8h.md#define-ns_timer_v1_0_0)  `{ .major = 1, .minor = 0, .revision = 0 }`<br> |

## Public Types Documentation




### typedef ns\_timer\_callback\_cb 

```C++
typedef void(* ns_timer_callback_cb) (struct ns_timer_config *);
```




<hr>



### typedef ns\_timer\_config\_t 

_ns-timer configuration struct_ 
```C++
typedef struct ns_timer_config ns_timer_config_t;
```




<hr>



### enum ns\_timers\_e 

_Supported Timers._ 
```C++
enum ns_timers_e {
    NS_TIMER_COUNTER = 0,
    NS_TIMER_INTERRUPT = 1,
    NS_TIMER_USB = 3,
    NS_TIMER_TEMPCO = 4
};
```




<hr>
## Public Attributes Documentation




### variable ns\_timer\_V0\_0\_1 

```C++
const ns_core_api_t ns_timer_V0_0_1;
```




<hr>



### variable ns\_timer\_V1\_0\_0 

```C++
const ns_core_api_t ns_timer_V1_0_0;
```




<hr>



### variable ns\_timer\_current\_version 

```C++
const ns_core_api_t ns_timer_current_version;
```




<hr>



### variable ns\_timer\_oldest\_supported\_version 

```C++
const ns_core_api_t ns_timer_oldest_supported_version;
```




<hr>
## Public Functions Documentation




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



### function ns\_timer\_init 

_Initialize one of 3 timers supported by NeuralSPOT._ 
```C++
uint32_t ns_timer_init (
    ns_timer_config_t * cfg
) 
```



NS\_TIMER\_COUNTER Intended use is reading timerticks NS\_TIMER\_INTERRUPT Calls a callback periodically NS\_TIMER\_USB Used by ns\_usb to periodically service USB NS\_TIMER\_TEMPCO Used by ns\_tempco to periodically collect temps




**Parameters:**


* `cfg` 



**Returns:**

uint32\_t status 





        

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
## Macro Definition Documentation





### define NS\_TIMER\_API\_ID 

```C++
#define NS_TIMER_API_ID `0xCA0002`
```




<hr>



### define NS\_TIMER\_CURRENT\_VERSION 

```C++
#define NS_TIMER_CURRENT_VERSION `NS_TIMER_V1_0_0`
```




<hr>



### define NS\_TIMER\_OLDEST\_SUPPORTED\_VERSION 

```C++
#define NS_TIMER_OLDEST_SUPPORTED_VERSION `NS_TIMER_V0_0_1`
```




<hr>



### define NS\_TIMER\_V0\_0\_1 

```C++
#define NS_TIMER_V0_0_1 `{ .major = 0, .minor = 0, .revision = 1 }`
```




<hr>



### define NS\_TIMER\_V1\_0\_0 

```C++
#define NS_TIMER_V1_0_0 `{ .major = 1, .minor = 0, .revision = 0 }`
```




<hr>

------------------------------


