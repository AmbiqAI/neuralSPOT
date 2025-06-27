

# File ns\_audio\_mfcc.h



[**FileList**](files.md) **>** [**includes-api**](dir_b70d46c064802b213244316ef6218d52.md) **>** [**ns\_audio\_mfcc.h**](ns__audio__mfcc_8h.md)

[Go to the source code of this file](ns__audio__mfcc_8h_source.md)

_Confifurable MFCC Calculator._ [More...](#detailed-description)

* `#include "arm_math.h"`
* `#include "ns_audio_features_common.h"`
* `#include "ns_core.h"`
* `#include "string.h"`















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**ns\_mfcc\_cfg\_t**](structns__mfcc__cfg__t.md) <br>_Config and state for MFCC calculator._  |






## Public Attributes

| Type | Name |
| ---: | :--- |
|  const ns\_core\_api\_t | [**ns\_mfcc\_V0\_0\_1**](#variable-ns_mfcc_v0_0_1)  <br> |
|  const ns\_core\_api\_t | [**ns\_mfcc\_V1\_0\_0**](#variable-ns_mfcc_v1_0_0)  <br> |
|  const ns\_core\_api\_t | [**ns\_mfcc\_current\_version**](#variable-ns_mfcc_current_version)  <br> |
|  const ns\_core\_api\_t | [**ns\_mfcc\_oldest\_supported\_version**](#variable-ns_mfcc_oldest_supported_version)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**ns\_mfcc\_compute**](#function-ns_mfcc_compute) ([**ns\_mfcc\_cfg\_t**](structns__mfcc__cfg__t.md) \* c, const int16\_t \* audio\_data, float \* mfcc\_out) <br>_Computes MFCC based on audio data._  |
|  uint32\_t | [**ns\_mfcc\_init**](#function-ns_mfcc_init) ([**ns\_mfcc\_cfg\_t**](structns__mfcc__cfg__t.md) \* c) <br>_Initializes the MFCC calculator based on desired configuration._  |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**M\_2PI**](ns__audio__mfcc_8h.md#define-m_2pi)  `6.283185307179586476925286766559005`<br> |
| define  | [**M\_PI**](ns__audio__mfcc_8h.md#define-m_pi)  `3.14159265358979323846264338328`<br> |
| define  | [**NS\_MFCC\_API\_ID**](ns__audio__mfcc_8h.md#define-ns_mfcc_api_id)  `0xCA0005`<br> |
| define  | [**NS\_MFCC\_CURRENT\_VERSION**](ns__audio__mfcc_8h.md#define-ns_mfcc_current_version)  `NS\_MFCC\_V1\_0\_0`<br> |
| define  | [**NS\_MFCC\_OLDEST\_SUPPORTED\_VERSION**](ns__audio__mfcc_8h.md#define-ns_mfcc_oldest_supported_version)  `NS\_MFCC\_V0\_0\_1`<br> |
| define  | [**NS\_MFCC\_SIZEBINS**](ns__audio__mfcc_8h.md#define-ns_mfcc_sizebins)  `53`<br> |
| define  | [**NS\_MFCC\_V0\_0\_1**](ns__audio__mfcc_8h.md#define-ns_mfcc_v0_0_1)  `{ .major = 0, .minor = 0, .revision = 1 }`<br> |
| define  | [**NS\_MFCC\_V1\_0\_0**](ns__audio__mfcc_8h.md#define-ns_mfcc_v1_0_0)  `{ .major = 1, .minor = 0, .revision = 0 }`<br> |

## Detailed Description




**Author:**

Carlos Morales 




**Version:**

0.1 




**Date:**

2022-08-12


This MFCC utility is adapted from Arm's example C++ implementation. It can be configured via DEFINES, but better config support is planned.




**Copyright:**

Copyright (c) 2022 





    
## Public Attributes Documentation




### variable ns\_mfcc\_V0\_0\_1 

```C++
const ns_core_api_t ns_mfcc_V0_0_1;
```




<hr>



### variable ns\_mfcc\_V1\_0\_0 

```C++
const ns_core_api_t ns_mfcc_V1_0_0;
```




<hr>



### variable ns\_mfcc\_current\_version 

```C++
const ns_core_api_t ns_mfcc_current_version;
```




<hr>



### variable ns\_mfcc\_oldest\_supported\_version 

```C++
const ns_core_api_t ns_mfcc_oldest_supported_version;
```




<hr>
## Public Functions Documentation




### function ns\_mfcc\_compute 

_Computes MFCC based on audio data._ 
```C++
uint32_t ns_mfcc_compute (
    ns_mfcc_cfg_t * c,
    const int16_t * audio_data,
    float * mfcc_out
) 
```





**Parameters:**


* `c` - configuration struct (see [**ns\_mfcc\_cfg\_t**](structns__mfcc__cfg__t.md)) from ns\_mfcc\_init 
* `audio_data` - pointer to audio data (int16\_t) 
* `mfcc_out` - pointer to output buffer (float) 



**Returns:**

uint32\_t 





        

<hr>



### function ns\_mfcc\_init 

_Initializes the MFCC calculator based on desired configuration._ 
```C++
uint32_t ns_mfcc_init (
    ns_mfcc_cfg_t * c
) 
```





**Parameters:**


* `c` configuration struct (see [**ns\_mfcc\_cfg\_t**](structns__mfcc__cfg__t.md)) 



**Returns:**

uint32\_t status 





        

<hr>
## Macro Definition Documentation





### define M\_2PI 

```C++
#define M_2PI `6.283185307179586476925286766559005`
```




<hr>



### define M\_PI 

```C++
#define M_PI `3.14159265358979323846264338328`
```




<hr>



### define NS\_MFCC\_API\_ID 

```C++
#define NS_MFCC_API_ID `0xCA0005`
```




<hr>



### define NS\_MFCC\_CURRENT\_VERSION 

```C++
#define NS_MFCC_CURRENT_VERSION `NS_MFCC_V1_0_0`
```




<hr>



### define NS\_MFCC\_OLDEST\_SUPPORTED\_VERSION 

```C++
#define NS_MFCC_OLDEST_SUPPORTED_VERSION `NS_MFCC_V0_0_1`
```




<hr>



### define NS\_MFCC\_SIZEBINS 

```C++
#define NS_MFCC_SIZEBINS `53`
```




<hr>



### define NS\_MFCC\_V0\_0\_1 

```C++
#define NS_MFCC_V0_0_1 `{ .major = 0, .minor = 0, .revision = 1 }`
```




<hr>



### define NS\_MFCC\_V1\_0\_0 

```C++
#define NS_MFCC_V1_0_0 `{ .major = 1, .minor = 0, .revision = 0 }`
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-audio/includes-api/ns_audio_mfcc.h`

