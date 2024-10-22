

# File ns\_free\_tests.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-utils**](dir_8caed56d1b8d43fb57ec0577c38aa59e.md) **>** [**tests**](dir_62cfc4ab5fe382f357338287112ab49a.md) **>** [**ns\_free\_tests.c**](ns__free__tests_8c.md)

[Go to the source code of this file](ns__free__tests_8c_source.md)



* `#include "ns_malloc.h"`
* `#include "unity/unity.h"`
* `#include "ns_core.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  size\_t const | [**ucHeapSize**](#variable-ucheapsize)   = = configTOTAL\_HEAP\_SIZE<br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  uint8\_t ucHeap[NS\_MALLOC\_HEAP\_SIZE\_IN\_K \*1024] | [**\_\_attribute\_\_**](#function-__attribute__) ((aligned(4))) <br> |
|  void | [**ns\_free\_test\_basic**](#function-ns_free_test_basic) () <br> |
|  void | [**ns\_free\_test\_memory\_fragmentation**](#function-ns_free_test_memory_fragmentation) () <br> |
|  void | [**ns\_free\_test\_non\_malloced\_pointer**](#function-ns_free_test_non_malloced_pointer) () <br> |
|  void | [**ns\_free\_test\_null\_pointer**](#function-ns_free_test_null_pointer) () <br> |
|  void | [**ns\_free\_test\_twice**](#function-ns_free_test_twice) () <br> |
|  void | [**ns\_free\_tests\_post\_test\_hook**](#function-ns_free_tests_post_test_hook) () <br> |
|  void | [**ns\_free\_tests\_pre\_test\_hook**](#function-ns_free_tests_pre_test_hook) () <br> |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**configTOTAL\_HEAP\_SIZE**](ns__free__tests_8c.md#define-configtotal_heap_size)  NS\_MALLOC\_HEAP\_SIZE\_IN\_K \* 1024<br> |

## Public Attributes Documentation




### variable ucHeapSize 

```C++
size_t const ucHeapSize;
```



## Public Functions Documentation




### function \_\_attribute\_\_ 

```C++
uint8_t ucHeap[NS_MALLOC_HEAP_SIZE_IN_K *1024] __attribute__ (
    (aligned(4))
) 
```






### function ns\_free\_test\_basic 

```C++
void ns_free_test_basic () 
```






### function ns\_free\_test\_memory\_fragmentation 

```C++
void ns_free_test_memory_fragmentation () 
```






### function ns\_free\_test\_non\_malloced\_pointer 

```C++
void ns_free_test_non_malloced_pointer () 
```






### function ns\_free\_test\_null\_pointer 

```C++
void ns_free_test_null_pointer () 
```






### function ns\_free\_test\_twice 

```C++
void ns_free_test_twice () 
```






### function ns\_free\_tests\_post\_test\_hook 

```C++
void ns_free_tests_post_test_hook () 
```






### function ns\_free\_tests\_pre\_test\_hook 

```C++
void ns_free_tests_pre_test_hook () 
```



## Macro Definition Documentation





### define configTOTAL\_HEAP\_SIZE 

```C++
#define configTOTAL_HEAP_SIZE NS_MALLOC_HEAP_SIZE_IN_K * 1024
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-utils/tests/ns_free_tests.c`

