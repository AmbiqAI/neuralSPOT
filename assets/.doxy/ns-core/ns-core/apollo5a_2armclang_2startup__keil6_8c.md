

# File startup\_keil6.c



[**FileList**](files.md) **>** [**apollo5a**](dir_a6fe3be1a7876c624c59ed050417822e.md) **>** [**armclang**](dir_1aa96d0d08bb9c7ebc3aa9fc44295b9d.md) **>** [**startup\_keil6.c**](apollo5a_2armclang_2startup__keil6_8c.md)

[Go to the source code of this file](apollo5a_2armclang_2startup__keil6_8c_source.md)

_Definitions for the Apollo5 vector table, interrupt handlers, and stack._ 

* `#include "apollo5a.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**\_\_INITIAL\_SP**](#variable-__initial_sp)  <br> |
|  uint32\_t | [**\_\_STACK\_LIMIT**](#variable-__stack_limit)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**HardFault\_Handler**](#function-hardfault_handler) (void) <br> |
|  void | [**Reset\_Handler**](#function-reset_handler) (void) <br> |
|  \_\_NO\_RETURN void | [**\_\_PROGRAM\_START**](#function-__program_start) (void) <br> |
|   | [**\_\_attribute\_\_**](#function-__attribute__) ((section(".patch"))) <br> |
|  void | [**am\_default\_isr**](#function-am_default_isr) (void) <br> |




























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



### function \_\_attribute\_\_ 

```C++
__attribute__ (
    (section(".patch"))
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

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-core/src/apollo5a/armclang/startup_keil6.c`

