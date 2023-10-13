

# File ns\_core.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-core**](dir_7a01d249276e526cbac17daf32597066.md) **>** [**src**](dir_2b540daedd13b1486e7bf344eba1e313.md) **>** [**ns\_core.c**](ns__core_8c.md)

[Go to the source code of this file](ns__core_8c_source.md)

_Code common to all neuralSPOT._ [More...](#detailed-description)

* `#include "ns_core.h"`





















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


## Public Static Functions

| Type | Name |
| ---: | :--- |
|  int | [**semver\_compare**](#function-semver_compare) (const [**ns\_semver\_t**](structns__semver__t.md) \* c, const [**ns\_semver\_t**](structns__semver__t.md) \* n) <br> |

























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**NS\_CORE\_API\_ID**](ns__core_8c.md#define-ns_core_api_id)  0xCA0000<br> |

# Detailed Description




**Author:**

Carlos Morales 




**Version:**

0.1 




**Date:**

2022-11-03




**Copyright:**

Copyright (c) 2022 





    
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





        
## Public Static Functions Documentation




### function semver\_compare 


```C++
static int semver_compare (
    const ns_semver_t * c,
    const ns_semver_t * n
) 
```



Compare semantic versions


param c - 'current' version param n - 'target' version return int - 0 if equal, -1 is c is less than n, 1 if c is more than n 


        
## Macro Definition Documentation





### define NS\_CORE\_API\_ID 

```C++
#define NS_CORE_API_ID 0xCA0000
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-core/src/ns_core.c`

