

# File ns\_malloc.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-utils**](dir_8caed56d1b8d43fb57ec0577c38aa59e.md) **>** [**src**](dir_5922fa0bec7bd191dd0e3ff5da447491.md) **>** [**ns\_malloc.c**](ns__malloc_8c.md)

[Go to the source code of this file](ns__malloc_8c_source.md)



* `#include "ns_malloc.h"`





































## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_free**](#function-ns_free) (void \* ptr) <br>_Simple vPortFree wrapper._  |
|  void \* | [**ns\_malloc**](#function-ns_malloc) (size\_t size) <br>_Simple pvPortMalloc wrapper._  |
|  uint8\_t | [**ns\_malloc\_init**](#function-ns_malloc_init) () <br>_Empty for now, but placeholder in case we need multi-heap support._  |




























## Public Functions Documentation




### function ns\_free 

```C++
void ns_free (
    void * ptr
) 
```






### function ns\_malloc 

```C++
void * ns_malloc (
    size_t size
) 
```






### function ns\_malloc\_init 

```C++
uint8_t ns_malloc_init () 
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-utils/src/ns_malloc.c`
