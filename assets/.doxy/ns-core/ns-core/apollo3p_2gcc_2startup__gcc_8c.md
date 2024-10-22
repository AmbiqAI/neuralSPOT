

# File startup\_gcc.c



[**FileList**](files.md) **>** [**apollo3p**](dir_2eb28cd4eb479aaf73d8bfdd9ecf519f.md) **>** [**gcc**](dir_c93f3b8c23cb6878000160df7e1bdfa6.md) **>** [**startup\_gcc.c**](apollo3p_2gcc_2startup__gcc_8c.md)

[Go to the source code of this file](apollo3p_2gcc_2startup__gcc_8c_source.md)

_Definitions for interrupt handlers, the vector table, and the stack._ 

* `#include <stdint.h>`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**\_ebss**](#variable-_ebss)  <br> |
|  uint32\_t | [**\_edata**](#variable-_edata)  <br> |
|  uint32\_t | [**\_etcm**](#variable-_etcm)  <br> |
|  uint32\_t | [**\_etext**](#variable-_etext)  <br> |
|  uint32\_t | [**\_sbss**](#variable-_sbss)  <br> |
|  uint32\_t | [**\_sdata**](#variable-_sdata)  <br> |
|  uint32\_t | [**\_stcm**](#variable-_stcm)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**HardFault\_Handler**](#function-hardfault_handler) (void) <br> |
|  void | [**NMI\_Handler**](#function-nmi_handler) (void) <br> |
|  void | [**Reset\_Handler**](#function-reset_handler) (void) <br> |
|   | [**\_\_attribute\_\_**](#function-__attribute__) ((section(".patch"))) <br> |
|  void | [**am\_default\_isr**](#function-am_default_isr) (void) <br> |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**HEAP\_SIZE**](apollo3p_2gcc_2startup__gcc_8c.md#define-heap_size)  0<br> |

## Public Attributes Documentation




### variable \_ebss 

```C++
uint32_t _ebss;
```






### variable \_edata 

```C++
uint32_t _edata;
```






### variable \_etcm 

```C++
uint32_t _etcm;
```






### variable \_etext 

```C++
uint32_t _etext;
```






### variable \_sbss 

```C++
uint32_t _sbss;
```






### variable \_sdata 

```C++
uint32_t _sdata;
```






### variable \_stcm 

```C++
uint32_t _stcm;
```



## Public Functions Documentation




### function HardFault\_Handler 

```C++
void HardFault_Handler (
    void
) 
```






### function NMI\_Handler 

```C++
void NMI_Handler (
    void
) 
```






### function Reset\_Handler 

```C++
void Reset_Handler (
    void
) 
```






### function \_\_attribute\_\_ 

```C++
__attribute__ (
    (section(".patch"))
) 
```






### function am\_default\_isr 

```C++
void am_default_isr (
    void
) 
```



## Macro Definition Documentation





### define HEAP\_SIZE 

```C++
#define HEAP_SIZE 0
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-core/src/apollo3p/gcc/startup_gcc.c`

