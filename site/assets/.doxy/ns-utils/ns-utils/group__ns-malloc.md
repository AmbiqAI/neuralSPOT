

# Group ns-malloc



[**Modules**](modules.md) **>** [**ns-malloc**](group__ns-malloc.md)










































## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_free**](#function-ns_free) (void \* ptr) <br>_Simple vPortFree wrapper._  |
|  void \* | [**ns\_malloc**](#function-ns_malloc) (size\_t size) <br>_Simple pvPortMalloc wrapper._  |
|  uint8\_t | [**ns\_malloc\_init**](#function-ns_malloc_init) () <br>_Empty for now, but placeholder in case we need multi-heap support._  |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**NS\_MALLOC\_HEAP\_SIZE\_IN\_K**](ns__malloc_8h.md#define-ns_malloc_heap_size_in_k)  `16`<br> |

## Public Functions Documentation




### function ns\_free 

_Simple vPortFree wrapper._ 
```C++
void ns_free (
    void * ptr
) 
```




<hr>



### function ns\_malloc 

_Simple pvPortMalloc wrapper._ 
```C++
void * ns_malloc (
    size_t size
) 
```




<hr>



### function ns\_malloc\_init 

_Empty for now, but placeholder in case we need multi-heap support._ 
```C++
uint8_t ns_malloc_init () 
```




<hr>
## Macro Definition Documentation





### define NS\_MALLOC\_HEAP\_SIZE\_IN\_K 

```C++
#define NS_MALLOC_HEAP_SIZE_IN_K `16`
```




<hr>

------------------------------


