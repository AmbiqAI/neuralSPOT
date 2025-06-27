

# File ns\_mfcc.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-audio**](dir_45211a8475460839574f71aa108f4957.md) **>** [**src**](dir_e70eef2d5115541d1d6cb7ad27f30382.md) **>** [**ns\_mfcc.c**](ns__mfcc_8c.md)

[Go to the source code of this file](ns__mfcc_8c_source.md)

_Based on original Arm version, see below._ [More...](#detailed-description)

* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "float.h"`
* `#include "ns_audio_features_common.h"`
* `#include "ns_audio_mfcc.h"`
* `#include "ns_core.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  arm\_rfft\_fast\_instance\_f32 | [**g\_mfccRfft**](#variable-g_mfccrfft)  <br> |
|  const ns\_core\_api\_t | [**ns\_mfcc\_V0\_0\_1**](#variable-ns_mfcc_v0_0_1)  <br> |
|  const ns\_core\_api\_t | [**ns\_mfcc\_V1\_0\_0**](#variable-ns_mfcc_v1_0_0)  <br> |
|  const ns\_core\_api\_t | [**ns\_mfcc\_current\_version**](#variable-ns_mfcc_current_version)  <br> |
|  const ns\_core\_api\_t | [**ns\_mfcc\_oldest\_supported\_version**](#variable-ns_mfcc_oldest_supported_version)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**ns\_mfcc\_compute**](#function-ns_mfcc_compute) ([**ns\_mfcc\_cfg\_t**](structns__mfcc__cfg__t.md) \* c, const int16\_t \* audio\_data, float \* mfcc\_out) <br>_Computes MFCC based on audio data._  |
|  uint32\_t | [**ns\_mfcc\_init**](#function-ns_mfcc_init) ([**ns\_mfcc\_cfg\_t**](structns__mfcc__cfg__t.md) \* c) <br>_Initializes the MFCC calculator based on desired configuration._  |


## Public Static Functions

| Type | Name |
| ---: | :--- |
|  void | [**create\_dct\_matrix**](#function-create_dct_matrix) ([**ns\_mfcc\_cfg\_t**](structns__mfcc__cfg__t.md) \* cfg, int32\_t input\_length, int32\_t coefficient\_count) <br> |
|  void | [**ns\_mfcc\_map\_arena**](#function-ns_mfcc_map_arena) ([**ns\_mfcc\_cfg\_t**](structns__mfcc__cfg__t.md) \* cfg) <br> |


























## Detailed Description




**Author:**

Carlos Morales 




**Version:**

0.1 




**Date:**

2022-08-09




**Copyright:**

Copyright (c) 2022 





    
## Public Attributes Documentation




### variable g\_mfccRfft 

```C++
arm_rfft_fast_instance_f32 g_mfccRfft;
```




<hr>



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
## Public Static Functions Documentation




### function create\_dct\_matrix 

```C++
static void create_dct_matrix (
    ns_mfcc_cfg_t * cfg,
    int32_t input_length,
    int32_t coefficient_count
) 
```




<hr>



### function ns\_mfcc\_map\_arena 

```C++
static void ns_mfcc_map_arena (
    ns_mfcc_cfg_t * cfg
) 
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-audio/src/ns_mfcc.c`

