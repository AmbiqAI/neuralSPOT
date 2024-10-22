

# File heap\_4.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-core**](dir_7a01d249276e526cbac17daf32597066.md) **>** [**src**](dir_2b540daedd13b1486e7bf344eba1e313.md) **>** [**heap\_4.c**](heap__4_8c.md)

[Go to the source code of this file](heap__4_8c_source.md)



* `#include <stdlib.h>`
* `#include "FreeRTOS.h"`
* `#include "task.h"`















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**A\_BLOCK\_LINK**](struct_a___b_l_o_c_k___l_i_n_k.md) <br> |


## Public Types

| Type | Name |
| ---: | :--- |
| typedef struct [**A\_BLOCK\_LINK**](struct_a___b_l_o_c_k___l_i_n_k.md) | [**BlockLink\_t**](#typedef-blocklink_t)  <br> |




## Public Attributes

| Type | Name |
| ---: | :--- |
|  size\_t const | [**ucHeapSize**](#variable-ucheapsize)   = = configTOTAL\_HEAP\_SIZE<br> |


## Public Static Attributes

| Type | Name |
| ---: | :--- |
|  [**BlockLink\_t**](struct_a___b_l_o_c_k___l_i_n_k.md) \* | [**pxEnd**](#variable-pxend)   = = NULL<br> |
|  size\_t | [**xBlockAllocatedBit**](#variable-xblockallocatedbit)   = = 0<br> |
|  size\_t | [**xFreeBytesRemaining**](#variable-xfreebytesremaining)   = = 0U<br> |
|  const size\_t | [**xHeapStructSize**](#variable-xheapstructsize)   = =
    (sizeof([**BlockLink\_t**](struct_a___b_l_o_c_k___l_i_n_k.md)) + ((size\_t)(portBYTE\_ALIGNMENT - 1))) & ~((size\_t)portBYTE\_ALIGNMENT\_MASK)<br> |
|  size\_t | [**xMinimumEverFreeBytesRemaining**](#variable-xminimumeverfreebytesremaining)   = = 0U<br> |
|  [**BlockLink\_t**](struct_a___b_l_o_c_k___l_i_n_k.md) | [**xStart**](#variable-xstart)  <br> |














## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**prvPortFree**](#function-prvportfree) (void \* pv, uint8\_t enableSuspendResume) <br> |
|  void \* | [**prvPortMalloc**](#function-prvportmalloc) (size\_t xWantedSize, uint8\_t enableSuspendResume) <br> |
|  void \* | [**pvPortMalloc**](#function-pvportmalloc) (size\_t xWantedSize) <br> |
|  void \* | [**pvTasklessPortMalloc**](#function-pvtasklessportmalloc) (size\_t xWantedSize) <br> |
|  void | [**vPortFree**](#function-vportfree) (void \* pv) <br> |
|  void | [**vPortInitialiseBlocks**](#function-vportinitialiseblocks) (void) <br> |
|  void | [**vTasklessPortFree**](#function-vtasklessportfree) (void \* pv) <br> |
|  size\_t | [**xPortGetFreeHeapSize**](#function-xportgetfreeheapsize) (void) <br> |
|  size\_t | [**xPortGetMinimumEverFreeHeapSize**](#function-xportgetminimumeverfreeheapsize) (void) <br> |


## Public Static Functions

| Type | Name |
| ---: | :--- |
|  uint8\_t ucHeap[configTOTAL\_HEAP\_SIZE] | [**\_\_attribute\_\_**](#function-__attribute__) ((aligned(4))) <br> |
|  void | [**prvHeapInit**](#function-prvheapinit) (void) <br> |
|  void | [**prvInsertBlockIntoFreeList**](#function-prvinsertblockintofreelist) ([**BlockLink\_t**](struct_a___b_l_o_c_k___l_i_n_k.md) \* pxBlockToInsert) <br> |

























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**MPU\_WRAPPERS\_INCLUDED\_FROM\_API\_FILE**](heap__4_8c.md#define-mpu_wrappers_included_from_api_file)  <br> |
| define  | [**configTOTAL\_HEAP\_SIZE**](heap__4_8c.md#define-configtotal_heap_size)  NS\_MALLOC\_HEAP\_SIZE\_IN\_K \* 1024<br> |
| define  | [**heapBITS\_PER\_BYTE**](heap__4_8c.md#define-heapbits_per_byte)  ((size\_t)8)<br> |
| define  | [**heapMINIMUM\_BLOCK\_SIZE**](heap__4_8c.md#define-heapminimum_block_size)  ((size\_t)(xHeapStructSize &lt;&lt; 1))<br> |

## Public Types Documentation




### typedef BlockLink\_t 

```C++
typedef struct A_BLOCK_LINK BlockLink_t;
```



## Public Attributes Documentation




### variable ucHeapSize 

```C++
size_t const ucHeapSize;
```



## Public Static Attributes Documentation




### variable pxEnd 

```C++
BlockLink_t * pxEnd;
```






### variable xBlockAllocatedBit 

```C++
size_t xBlockAllocatedBit;
```






### variable xFreeBytesRemaining 

```C++
size_t xFreeBytesRemaining;
```






### variable xHeapStructSize 

```C++
const size_t xHeapStructSize;
```






### variable xMinimumEverFreeBytesRemaining 

```C++
size_t xMinimumEverFreeBytesRemaining;
```






### variable xStart 

```C++
BlockLink_t xStart;
```



## Public Functions Documentation




### function prvPortFree 

```C++
void prvPortFree (
    void * pv,
    uint8_t enableSuspendResume
) 
```






### function prvPortMalloc 


```C++
void * prvPortMalloc (
    size_t xWantedSize,
    uint8_t enableSuspendResume
) 
```



pvPortMalloc has a bug where it assumes that the task scheduler is running: The culprit is ResumeAll, which calls enterCritical/exitCritical - these functions, in port.c, assume the task scheduler is running, and the exit is not enabling interrupts because of the nested critical assert.


NeuralSPOT solution is to create a new version of malloc 


        



### function pvPortMalloc 

```C++
void * pvPortMalloc (
    size_t xWantedSize
) 
```






### function pvTasklessPortMalloc 

```C++
void * pvTasklessPortMalloc (
    size_t xWantedSize
) 
```






### function vPortFree 

```C++
void vPortFree (
    void * pv
) 
```






### function vPortInitialiseBlocks 

```C++
void vPortInitialiseBlocks (
    void
) 
```






### function vTasklessPortFree 

```C++
void vTasklessPortFree (
    void * pv
) 
```






### function xPortGetFreeHeapSize 

```C++
size_t xPortGetFreeHeapSize (
    void
) 
```






### function xPortGetMinimumEverFreeHeapSize 

```C++
size_t xPortGetMinimumEverFreeHeapSize (
    void
) 
```



## Public Static Functions Documentation




### function \_\_attribute\_\_ 

```C++
static uint8_t ucHeap[configTOTAL_HEAP_SIZE] __attribute__ (
    (aligned(4))
) 
```






### function prvHeapInit 

```C++
static void prvHeapInit (
    void
) 
```






### function prvInsertBlockIntoFreeList 

```C++
static void prvInsertBlockIntoFreeList (
    BlockLink_t * pxBlockToInsert
) 
```



## Macro Definition Documentation





### define MPU\_WRAPPERS\_INCLUDED\_FROM\_API\_FILE 

```C++
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE 
```






### define configTOTAL\_HEAP\_SIZE 

```C++
#define configTOTAL_HEAP_SIZE NS_MALLOC_HEAP_SIZE_IN_K * 1024
```






### define heapBITS\_PER\_BYTE 

```C++
#define heapBITS_PER_BYTE ((size_t)8)
```






### define heapMINIMUM\_BLOCK\_SIZE 

```C++
#define heapMINIMUM_BLOCK_SIZE ((size_t)(xHeapStructSize << 1))
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-core/src/heap_4.c`

