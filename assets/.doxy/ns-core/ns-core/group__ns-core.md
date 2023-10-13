

# Group ns-core



[**Modules**](modules.md) **>** [**ns-core**](group__ns-core.md)




















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**ns\_core\_api\_t**](structns__core__api__t.md) <br> |
| struct | [**ns\_core\_config\_t**](structns__core__config__t.md) <br>_Core config struct._  |
| struct | [**ns\_desired\_state\_t**](structns__desired__state__t.md) <br> |
| struct | [**ns\_semver\_t**](structns__semver__t.md) <br> |






## Public Attributes

| Type | Name |
| ---: | :--- |
|  [**ns\_desired\_state\_t**](structns__desired__state__t.md) | [**g\_ns\_state**](#variable-g_ns_state)  <br>_Collection of ns-wide states and desired states._  |
|  const [**ns\_core\_api\_t**](structns__core__api__t.md) | [**ns\_core\_V0\_0\_1**](#variable-ns_core_v0_0_1)  <br> |
|  const [**ns\_core\_api\_t**](structns__core__api__t.md) | [**ns\_core\_V1\_0\_0**](#variable-ns_core_v1_0_0)  <br> |
|  const [**ns\_core\_api\_t**](structns__core__api__t.md) | [**ns\_core\_current\_version**](#variable-ns_core_current_version)  <br> |
|  const [**ns\_core\_api\_t**](structns__core__api__t.md) | [**ns\_core\_oldest\_supported\_version**](#variable-ns_core_oldest_supported_version)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**ns\_core\_check\_api**](#function-ns_core_check_api) (const [**ns\_core\_api\_t**](structns__core__api__t.md) \* submitted, const [**ns\_core\_api\_t**](structns__core__api__t.md) \* oldest, const [**ns\_core\_api\_t**](structns__core__api__t.md) \* newest) <br> |
|  void | [**ns\_core\_fail\_loop**](#function-ns_core_fail_loop) () <br> |
|  uint32\_t | [**ns\_core\_init**](#function-ns_core_init) ([**ns\_core\_config\_t**](structns__core__config__t.md) \* c) <br>_Initialize core state._  |
|  bool | [**ns\_core\_initialized**](#function-ns_core_initialized) () <br> |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**NS\_CORE\_CURRENT\_VERSION**](ns__core_8h.md#define-ns_core_current_version)  NS\_CORE\_V1\_0\_0<br> |
| define  | [**NS\_CORE\_OLDEST\_SUPPORTED\_VERSION**](ns__core_8h.md#define-ns_core_oldest_supported_version)  NS\_CORE\_V0\_0\_1<br> |
| define  | [**NS\_CORE\_V0\_0\_1**](ns__core_8h.md#define-ns_core_v0_0_1)          { .major = 0, .minor = 0, .revision = 1 }<br> |
| define  | [**NS\_CORE\_V1\_0\_0**](ns__core_8h.md#define-ns_core_v1_0_0)          { .major = 1, .minor = 0, .revision = 0 }<br> |
| define  | [**NS\_STATUS\_FAILURE**](ns__core_8h.md#define-ns_status_failure)  -1<br> |
| define  | [**NS\_STATUS\_INIT\_FAILED**](ns__core_8h.md#define-ns_status_init_failed)  4<br> |
| define  | [**NS\_STATUS\_INVALID\_CONFIG**](ns__core_8h.md#define-ns_status_invalid_config)  3<br> |
| define  | [**NS\_STATUS\_INVALID\_HANDLE**](ns__core_8h.md#define-ns_status_invalid_handle)  1<br> |
| define  | [**NS\_STATUS\_INVALID\_VERSION**](ns__core_8h.md#define-ns_status_invalid_version)  2<br> |
| define  | [**NS\_STATUS\_SUCCESS**](ns__core_8h.md#define-ns_status_success)  0<br> |
| define  | [**NS\_TRY**](ns__core_8h.md#define-ns_try) (func, msg) <br> |

## Public Attributes Documentation




### variable g\_ns\_state 

```C++
ns_desired_state_t g_ns_state;
```






### variable ns\_core\_V0\_0\_1 

```C++
const ns_core_api_t ns_core_V0_0_1;
```






### variable ns\_core\_V1\_0\_0 

```C++
const ns_core_api_t ns_core_V1_0_0;
```






### variable ns\_core\_current\_version 

```C++
const ns_core_api_t ns_core_current_version;
```






### variable ns\_core\_oldest\_supported\_version 

```C++
const ns_core_api_t ns_core_oldest_supported_version;
```



## Public Functions Documentation




### function ns\_core\_check\_api 


```C++
uint32_t ns_core_check_api (
    const ns_core_api_t * submitted,
    const ns_core_api_t * oldest,
    const ns_core_api_t * newest
) 
```



brief Checks API and semantic version of desired API vs. allowed APIs


param submitted param oldest param newest return uint32\_t 


        



### function ns\_core\_fail\_loop 

```C++
void ns_core_fail_loop () 
```






### function ns\_core\_init 

_Initialize core state._ 
```C++
uint32_t ns_core_init (
    ns_core_config_t * c
) 
```





**Parameters:**


* `c` [**ns\_core\_config\_t**](structns__core__config__t.md) indicating the API version 



**Returns:**

uint32\_t 





        



### function ns\_core\_initialized 

```C++
bool ns_core_initialized () 
```



## Macro Definition Documentation





### define NS\_CORE\_CURRENT\_VERSION 

```C++
#define NS_CORE_CURRENT_VERSION NS_CORE_V1_0_0
```






### define NS\_CORE\_OLDEST\_SUPPORTED\_VERSION 

```C++
#define NS_CORE_OLDEST_SUPPORTED_VERSION NS_CORE_V0_0_1
```






### define NS\_CORE\_V0\_0\_1 

```C++
#define NS_CORE_V0_0_1 { .major = 0, .minor = 0, .revision = 1 }
```






### define NS\_CORE\_V1\_0\_0 

```C++
#define NS_CORE_V1_0_0 { .major = 1, .minor = 0, .revision = 0 }
```






### define NS\_STATUS\_FAILURE 

```C++
#define NS_STATUS_FAILURE -1
```






### define NS\_STATUS\_INIT\_FAILED 

```C++
#define NS_STATUS_INIT_FAILED 4
```






### define NS\_STATUS\_INVALID\_CONFIG 

```C++
#define NS_STATUS_INVALID_CONFIG 3
```






### define NS\_STATUS\_INVALID\_HANDLE 

```C++
#define NS_STATUS_INVALID_HANDLE 1
```






### define NS\_STATUS\_INVALID\_VERSION 

```C++
#define NS_STATUS_INVALID_VERSION 2
```






### define NS\_STATUS\_SUCCESS 

```C++
#define NS_STATUS_SUCCESS 0
```






### define NS\_TRY 

```C++
#define NS_TRY (
    func,
    msg
) if (func) {                                                                                \
            ns_lp_printf(msg);                                                                     \
            ns_core_fail_loop();                                                                   \
        }
```




------------------------------


