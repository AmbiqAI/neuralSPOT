

# File startup\_gcc.c



[**FileList**](files.md) **>** [**apollo5a**](dir_a6fe3be1a7876c624c59ed050417822e.md) **>** [**gcc**](dir_0d388e8cc3cd473f17b84236fe6e888d.md) **>** [**startup\_gcc.c**](apollo5a_2gcc_2startup__gcc_8c.md)

[Go to the source code of this file](apollo5a_2gcc_2startup__gcc_8c_source.md)

_Definitions for interrupt handlers, the vector table, and the stack._ 

* `#include "apollo5a.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**\_ebss**](#variable-_ebss)  <br> |
|  uint32\_t | [**\_edata**](#variable-_edata)  <br> |
|  uint32\_t | [**\_etext**](#variable-_etext)  <br> |
|  uint32\_t | [**\_sbss**](#variable-_sbss)  <br> |
|  uint32\_t | [**\_sdata**](#variable-_sdata)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**HardFault\_Handler**](#function-hardfault_handler) (void) <br> |
|  void | [**NMI\_Handler**](#function-nmi_handler) (void) <br> |
|  void | [**Reset\_Handler**](#function-reset_handler) (void) <br> |
|   | [**\_\_attribute\_\_**](#function-__attribute__) ((section(".patch"))) <br> |
|   | [**\_\_attribute\_\_**](#function-__attribute__) ((weak, noreturn)) <br> |
|  void | [**am\_default\_isr**](#function-am_default_isr) (void) <br> |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**AM\_HEAP\_SIZE**](apollo5a_2gcc_2startup__gcc_8c.md#define-am_heap_size)  `(sizeof(g\_pui32Heap))`<br> |
| define  | [**AM\_STACK\_LIMIT**](apollo5a_2gcc_2startup__gcc_8c.md#define-am_stack_limit)  `((uint32\_t)&g\_pui32Stack)`<br> |
| define  | [**AM\_STACK\_SIZE**](apollo5a_2gcc_2startup__gcc_8c.md#define-am_stack_size)  `(sizeof(g\_pui32Stack))`<br> |

## Public Attributes Documentation




### variable \_ebss 

```C++
uint32_t _ebss;
```




<hr>



### variable \_edata 

```C++
uint32_t _edata;
```




<hr>



### variable \_etext 

```C++
uint32_t _etext;
```




<hr>



### variable \_sbss 

```C++
uint32_t _sbss;
```




<hr>



### variable \_sdata 

```C++
uint32_t _sdata;
```




<hr>
## Public Functions Documentation




### function HardFault\_Handler 

```C++
void HardFault_Handler (
    void
) 
```




<hr>



### function NMI\_Handler 

```C++
void NMI_Handler (
    void
) 
```




<hr>



### function Reset\_Handler 

```C++
void Reset_Handler (
    void
) 
```




<hr>



### function \_\_attribute\_\_ 

```C++
__attribute__ (
    (section(".patch"))
) 
```




<hr>



### function \_\_attribute\_\_ 

```C++
__attribute__ (
    (weak, noreturn)
) 
```




<hr>



### function am\_default\_isr 

```C++
void am_default_isr (
    void
) 
```




<hr>
## Macro Definition Documentation





### define AM\_HEAP\_SIZE 

```C++
#define AM_HEAP_SIZE `(sizeof(g_pui32Heap))`
```




<hr>



### define AM\_STACK\_LIMIT 

```C++
#define AM_STACK_LIMIT `((uint32_t)&g_pui32Stack)`
```




<hr>



### define AM\_STACK\_SIZE 

```C++
#define AM_STACK_SIZE `(sizeof(g_pui32Stack))`
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-core/src/apollo5a/gcc/startup_gcc.c`

