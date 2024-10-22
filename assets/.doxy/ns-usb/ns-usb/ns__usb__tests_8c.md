

# File ns\_usb\_tests.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-usb**](dir_450d7ce7daa9d29b9b3b5cb7d00f16f9.md) **>** [**tests**](dir_c776a9f5367fd77ebb6bd7a429b2b06e.md) **>** [**ns\_usb\_tests.c**](ns__usb__tests_8c.md)

[Go to the source code of this file](ns__usb__tests_8c_source.md)



* `#include "ns_usb.h"`
* `#include "unity/unity.h"`
* `#include "ns_core.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  [**ns\_usb\_config\_t**](structns__usb__config__t.md) | [**cfg**](#variable-cfg)  <br> |
|  usb\_handle\_t | [**handle**](#variable-handle)   = = NULL<br> |
|  uint8\_t | [**rx\_buf**](#variable-rx_buf)  <br> |
|  uint8\_t | [**tx\_buf**](#variable-tx_buf)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_usb\_test\_init**](#function-ns_usb_test_init) () <br> |
|  void | [**ns\_usb\_test\_init\_invalid\_api**](#function-ns_usb_test_init_invalid_api) () <br> |
|  void | [**ns\_usb\_test\_init\_null\_buffers**](#function-ns_usb_test_init_null_buffers) () <br> |
|  void | [**ns\_usb\_test\_init\_null\_config**](#function-ns_usb_test_init_null_config) () <br> |
|  void | [**ns\_usb\_tests\_post\_test\_hook**](#function-ns_usb_tests_post_test_hook) () <br> |
|  void | [**ns\_usb\_tests\_pre\_test\_hook**](#function-ns_usb_tests_pre_test_hook) () <br> |
|  void | [**reset\_usb\_cfg**](#function-reset_usb_cfg) () <br> |




























## Public Attributes Documentation




### variable cfg 

```C++
ns_usb_config_t cfg;
```






### variable handle 

```C++
usb_handle_t handle;
```






### variable rx\_buf 

```C++
uint8_t rx_buf[4096];
```






### variable tx\_buf 

```C++
uint8_t tx_buf[4096];
```



## Public Functions Documentation




### function ns\_usb\_test\_init 

```C++
void ns_usb_test_init () 
```






### function ns\_usb\_test\_init\_invalid\_api 

```C++
void ns_usb_test_init_invalid_api () 
```






### function ns\_usb\_test\_init\_null\_buffers 

```C++
void ns_usb_test_init_null_buffers () 
```






### function ns\_usb\_test\_init\_null\_config 

```C++
void ns_usb_test_init_null_config () 
```






### function ns\_usb\_tests\_post\_test\_hook 

```C++
void ns_usb_tests_post_test_hook () 
```






### function ns\_usb\_tests\_pre\_test\_hook 

```C++
void ns_usb_tests_pre_test_hook () 
```






### function reset\_usb\_cfg 

```C++
void reset_usb_cfg () 
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-usb/tests/ns_usb_tests.c`

