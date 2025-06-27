

# File ns\_power\_profile.c



[**FileList**](files.md) **>** [**apollo4**](dir_f4b6e016fb89d71f6ddac0c53471985d.md) **>** [**ns\_power\_profile.c**](apollo4_2ns__power__profile_8c.md)

[Go to the source code of this file](apollo4_2ns__power__profile_8c_source.md)

_Collection of power profiling utilities._ [More...](#detailed-description)

* `#include "ns_power_profile.h"`
* `#include "ns_ambiqsuite_harness.h"`
* `#include <stdarg.h>`
* `#include <stdio.h>`
* `#include <stdlib.h>`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  [**am\_bsp\_pp\_b1\_t**](structam__bsp__pp__b1__t.md) | [**am\_bsp\_pp1**](#variable-am_bsp_pp1)  <br> |
|  [**am\_bsp\_pp\_b2\_t**](structam__bsp__pp__b2__t.md) | [**am\_bsp\_pp2**](#variable-am_bsp_pp2)  <br> |
|  [**am\_bsp\_pp\_b3\_t**](structam__bsp__pp__b3__t.md) | [**am\_bsp\_pp3**](#variable-am_bsp_pp3)  <br> |
|  [**am\_bsp\_pp\_b4\_t**](structam__bsp__pp__b4__t.md) | [**am\_bsp\_pp4**](#variable-am_bsp_pp4)  <br> |
|  void \* | [**g\_pp\_AUDADCHandle**](#variable-g_pp_audadchandle)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_pp\_ap4\_snapshot**](#function-ns_pp_ap4_snapshot) (bool bSingleShot, uint32\_t uNumber) <br> |
|  void | [**ns\_print\_JSON**](#function-ns_print_json) (void) <br>_Prints the filled up JSON to serial port._  |




























## Detailed Description




**Author:**

Ambiq 




**Version:**

0.1 




**Date:**

2022-10-10




**Copyright:**

Copyright (c) 2022 





    
## Public Attributes Documentation




### variable am\_bsp\_pp1 

```C++
am_bsp_pp_b1_t am_bsp_pp1;
```




<hr>



### variable am\_bsp\_pp2 

```C++
am_bsp_pp_b2_t am_bsp_pp2;
```




<hr>



### variable am\_bsp\_pp3 

```C++
am_bsp_pp_b3_t am_bsp_pp3;
```




<hr>



### variable am\_bsp\_pp4 

```C++
am_bsp_pp_b4_t am_bsp_pp4;
```




<hr>



### variable g\_pp\_AUDADCHandle 

```C++
void* g_pp_AUDADCHandle;
```




<hr>
## Public Functions Documentation




### function ns\_pp\_ap4\_snapshot 

```C++
void ns_pp_ap4_snapshot (
    bool bSingleShot,
    uint32_t uNumber
) 
```




<hr>



### function ns\_print\_JSON 

_Prints the filled up JSON to serial port._ 
```C++
void ns_print_JSON (
    void
) 
```





**Parameters:**


* `void` 

This function will print filled JSON fields to serial port.




**Note:**

- before use this function, make sure the UART or SWO is enabled




**Returns:**

None 





        

<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-utils/src/apollo4/ns_power_profile.c`

