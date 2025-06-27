

# File startup\_keil6.c



[**FileList**](files.md) **>** [**apollo510**](dir_4836c8f18adf8fe661489f81269964c0.md) **>** [**armclang**](dir_07427f3653b1c93a64ed19d955d493a3.md) **>** [**startup\_keil6.c**](apollo510_2armclang_2startup__keil6_8c.md)

[Go to the source code of this file](apollo510_2armclang_2startup__keil6_8c_source.md)

_Definitions for the Apollo5 vector table, interrupt handlers, and stack._ 

* `#include "apollo510.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**\_\_INITIAL\_SP**](#variable-__initial_sp)  <br> |
|  uint32\_t | [**\_\_STACK\_LIMIT**](#variable-__stack_limit)  <br> |
|  const uint32\_t \*const | [**\_\_pPatchable**](#variable-__ppatchable)   = `(uint32\_t \*) \_\_VECTOR\_TABLE + AM\_PATCHABLE\_OFFSET`<br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**HardFault\_Handler**](#function-hardfault_handler) (void) <br> |
|  void | [**Reset\_Handler**](#function-reset_handler) (void) <br> |
|  \_\_NO\_RETURN void | [**\_\_PROGRAM\_START**](#function-__program_start) (void) <br> |
|  void | [**am\_default\_isr**](#function-am_default_isr) (void) <br> |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**AM\_PATCHABLE\_OFFSET**](apollo510_2armclang_2startup__keil6_8c.md#define-am_patchable_offset)  `(134 + 16 + 1)`<br> |

## Public Attributes Documentation




### variable \_\_INITIAL\_SP 

```C++
uint32_t __INITIAL_SP;
```




<hr>



### variable \_\_STACK\_LIMIT 

```C++
uint32_t __STACK_LIMIT;
```




<hr>



### variable \_\_pPatchable 

```C++
const uint32_t* const __pPatchable;
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



### function Reset\_Handler 

```C++
void Reset_Handler (
    void
) 
```




<hr>



### function \_\_PROGRAM\_START 

```C++
__NO_RETURN void __PROGRAM_START (
    void
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





### define AM\_PATCHABLE\_OFFSET 

```C++
#define AM_PATCHABLE_OFFSET `(134 + 16 + 1)`
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-core/src/apollo510/armclang/startup_keil6.c`

