

# File ns\_power\_profile.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-utils**](dir_8caed56d1b8d43fb57ec0577c38aa59e.md) **>** [**src**](dir_5922fa0bec7bd191dd0e3ff5da447491.md) **>** [**ns\_power\_profile.c**](ns__power__profile_8c.md)

[Go to the source code of this file](ns__power__profile_8c_source.md)

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
|  void | [**ns\_pp\_snapshot**](#function-ns_pp_snapshot) (bool bSingleShot, uint32\_t uNumber) <br> |
|  void | [**ns\_print\_JSON**](#function-ns_print_json) (void) <br>_Prints the filled up JSON to serial port._  |




























# Detailed Description




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






### variable am\_bsp\_pp2 

```C++
am_bsp_pp_b2_t am_bsp_pp2;
```






### variable am\_bsp\_pp3 

```C++
am_bsp_pp_b3_t am_bsp_pp3;
```






### variable am\_bsp\_pp4 

```C++
am_bsp_pp_b4_t am_bsp_pp4;
```






### variable g\_pp\_AUDADCHandle 

```C++
void* g_pp_AUDADCHandle;
```



## Public Functions Documentation




### function ns\_pp\_snapshot 

```C++
void ns_pp_snapshot (
    bool bSingleShot,
    uint32_t uNumber
) 
```






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





        

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-utils/src/ns_power_profile.c`

