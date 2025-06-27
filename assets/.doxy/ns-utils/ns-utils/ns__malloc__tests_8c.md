

# File ns\_malloc\_tests.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-utils**](dir_8caed56d1b8d43fb57ec0577c38aa59e.md) **>** [**tests**](dir_62cfc4ab5fe382f357338287112ab49a.md) **>** [**ns\_malloc\_tests.c**](ns__malloc__tests_8c.md)

[Go to the source code of this file](ns__malloc__tests_8c_source.md)



* `#include "ns_malloc.h"`
* `#include "unity/unity.h"`
* `#include "ns_core.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  size\_t const | [**ucHeapSize**](#variable-ucheapsize)   = `configTOTAL\_HEAP\_SIZE`<br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  uint8\_t ucHeap[NS\_MALLOC\_HEAP\_SIZE\_IN\_K \*1024] | [**\_\_attribute\_\_**](#function-__attribute__) ((aligned(4))) <br> |
|  void | [**ns\_malloc\_test\_allocate\_zero**](#function-ns_malloc_test_allocate_zero) () <br> |
|  void | [**ns\_malloc\_test\_basic\_allocation**](#function-ns_malloc_test_basic_allocation) () <br> |
|  void | [**ns\_malloc\_test\_no\_overlap\_in\_heap\_allocations**](#function-ns_malloc_test_no_overlap_in_heap_allocations) () <br> |
|  void | [**ns\_malloc\_test\_past\_max\_size**](#function-ns_malloc_test_past_max_size) () <br> |
|  void | [**ns\_malloc\_tests\_post\_test\_hook**](#function-ns_malloc_tests_post_test_hook) () <br> |
|  void | [**ns\_malloc\_tests\_pre\_test\_hook**](#function-ns_malloc_tests_pre_test_hook) () <br> |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**configTOTAL\_HEAP\_SIZE**](ns__malloc__tests_8c.md#define-configtotal_heap_size)  `NS\_MALLOC\_HEAP\_SIZE\_IN\_K \* 1024`<br> |

## Public Attributes Documentation




### variable ucHeapSize 

```C++
size_t const ucHeapSize;
```




<hr>
## Public Functions Documentation




### function \_\_attribute\_\_ 

```C++
uint8_t ucHeap[NS_MALLOC_HEAP_SIZE_IN_K *1024] __attribute__ (
    (aligned(4))
) 
```




<hr>



### function ns\_malloc\_test\_allocate\_zero 

```C++
void ns_malloc_test_allocate_zero () 
```




<hr>



### function ns\_malloc\_test\_basic\_allocation 

```C++
void ns_malloc_test_basic_allocation () 
```




<hr>



### function ns\_malloc\_test\_no\_overlap\_in\_heap\_allocations 

```C++
void ns_malloc_test_no_overlap_in_heap_allocations () 
```




<hr>



### function ns\_malloc\_test\_past\_max\_size 

```C++
void ns_malloc_test_past_max_size () 
```




<hr>



### function ns\_malloc\_tests\_post\_test\_hook 

```C++
void ns_malloc_tests_post_test_hook () 
```




<hr>



### function ns\_malloc\_tests\_pre\_test\_hook 

```C++
void ns_malloc_tests_pre_test_hook () 
```




<hr>
## Macro Definition Documentation





### define configTOTAL\_HEAP\_SIZE 

```C++
#define configTOTAL_HEAP_SIZE `NS_MALLOC_HEAP_SIZE_IN_K * 1024`
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-utils/tests/ns_malloc_tests.c`

