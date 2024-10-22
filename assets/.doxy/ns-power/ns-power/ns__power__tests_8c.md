

# File ns\_power\_tests.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-peripherals**](dir_62cbd78784261bb0d09981988628a167.md) **>** [**tests**](dir_212d9119d40fadcef4edfdca97975f2d.md) **>** [**ns\_power\_tests.c**](ns__power__tests_8c.md)

[Go to the source code of this file](ns__power__tests_8c_source.md)



* `#include "unity/unity.h"`
* `#include "ns_peripherals_power.h"`
* `#include "ns_core.h"`























## Public Static Attributes

| Type | Name |
| ---: | :--- |
|  ns\_core\_config\_t | [**core\_cfg**](#variable-core_cfg)   = = {
        .api = &ns\_core\_V1\_0\_0,
    }<br> |
|  [**ns\_power\_config\_t**](structns__power__config__t.md) | [**custom\_cfg**](#variable-custom_cfg)  <br> |














## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_power\_config\_all\_false\_test**](#function-ns_power_config_all_false_test) () <br> |
|  void | [**ns\_power\_config\_all\_true\_test**](#function-ns_power_config_all_true_test) () <br> |
|  void | [**ns\_power\_config\_invalid\_api\_test**](#function-ns_power_config_invalid_api_test) () <br> |
|  void | [**ns\_power\_config\_null\_test**](#function-ns_power_config_null_test) () <br> |
|  void | [**ns\_power\_config\_power\_mode\_test**](#function-ns_power_config_power_mode_test) () <br> |
|  void | [**ns\_power\_config\_test**](#function-ns_power_config_test) () <br> |
|  void | [**ns\_power\_tests\_post\_test\_hook**](#function-ns_power_tests_post_test_hook) () <br> |
|  void | [**ns\_power\_tests\_pre\_test\_hook**](#function-ns_power_tests_pre_test_hook) () <br> |


## Public Static Functions

| Type | Name |
| ---: | :--- |
|  void | [**reset\_custom\_cfg**](#function-reset_custom_cfg) () <br> |


























## Public Static Attributes Documentation




### variable core\_cfg 

```C++
ns_core_config_t core_cfg;
```






### variable custom\_cfg 

```C++
ns_power_config_t custom_cfg;
```



## Public Functions Documentation




### function ns\_power\_config\_all\_false\_test 

```C++
void ns_power_config_all_false_test () 
```






### function ns\_power\_config\_all\_true\_test 

```C++
void ns_power_config_all_true_test () 
```






### function ns\_power\_config\_invalid\_api\_test 

```C++
void ns_power_config_invalid_api_test () 
```






### function ns\_power\_config\_null\_test 

```C++
void ns_power_config_null_test () 
```






### function ns\_power\_config\_power\_mode\_test 

```C++
void ns_power_config_power_mode_test () 
```






### function ns\_power\_config\_test 

```C++
void ns_power_config_test () 
```






### function ns\_power\_tests\_post\_test\_hook 

```C++
void ns_power_tests_post_test_hook () 
```






### function ns\_power\_tests\_pre\_test\_hook 

```C++
void ns_power_tests_pre_test_hook () 
```



## Public Static Functions Documentation




### function reset\_custom\_cfg 

```C++
static void reset_custom_cfg () 
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-peripherals/tests/ns_power_tests.c`

