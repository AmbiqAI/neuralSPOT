

# File ns\_melspec.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-audio**](dir_45211a8475460839574f71aa108f4957.md) **>** [**src**](dir_e70eef2d5115541d1d6cb7ad27f30382.md) **>** [**ns\_melspec.c**](ns__melspec_8c.md)

[Go to the source code of this file](ns__melspec_8c_source.md)

_Set of mel-scaled spectrogram related utilities._ [More...](#detailed-description)

* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "float.h"`
* `#include "ns_ambiqsuite_harness.h"`
* `#include "ns_audio_melspec.h"`
* `#include <string.h>`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  arm\_cfft\_instance\_f32 | [**g\_melspecRfft**](#variable-g_melspecrfft)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_melspec\_audio\_to\_stft**](#function-ns_melspec_audio_to_stft) ([**ns\_melspec\_cfg\_t**](structns__melspec__cfg__t.md) \* cfg, const int16\_t \* audio\_in, float32\_t \* stft\_out) <br>_convert the audio samples into a short-time fourier transform_  |
|  void | [**ns\_melspec\_init**](#function-ns_melspec_init) ([**ns\_melspec\_cfg\_t**](structns__melspec__cfg__t.md) \* cfg) <br>_initialize data structures used for mel spectrogram related functions_  |
|  void | [**ns\_melspec\_melspec\_to\_stft**](#function-ns_melspec_melspec_to_stft) ([**ns\_melspec\_cfg\_t**](structns__melspec__cfg__t.md) \* cfg, const float32\_t \* melspec\_in, float32\_t \* stft\_out) <br>_Inverts a mel spectrogram back into linear short-time fourier transform._  |
|  void | [**ns\_melspec\_stft\_to\_audio**](#function-ns_melspec_stft_to_audio) ([**ns\_melspec\_cfg\_t**](structns__melspec__cfg__t.md) \* cfg, float32\_t \* stft\_in, int16\_t \* audio\_out) <br>_Inverts short-time fourier transform into a reconstructed audio sample._  |
|  void | [**ns\_melspec\_stft\_to\_compressed\_melspec**](#function-ns_melspec_stft_to_compressed_melspec) ([**ns\_melspec\_cfg\_t**](structns__melspec__cfg__t.md) \* cfg, const float32\_t \* stft\_in, float32\_t \* melspec\_out) <br>_Converts complex valued short-time fourier transform into real-valued, power compressed mel scaled space._  |




























## Detailed Description




**Author:**

Scott Leishman 




**Version:**

0.1 




**Date:**

2022-10-10




**Copyright:**

Copyright (c) 2022 





    
## Public Attributes Documentation




### variable g\_melspecRfft 

```C++
arm_cfft_instance_f32 g_melspecRfft;
```




<hr>
## Public Functions Documentation




### function ns\_melspec\_audio\_to\_stft 

_convert the audio samples into a short-time fourier transform_ 
```C++
void ns_melspec_audio_to_stft (
    ns_melspec_cfg_t * cfg,
    const int16_t * audio_in,
    float32_t * stft_out
) 
```



Result is complex valued (containing magnitude and phase info in alternating 32-bit values, so stft\_out should be 2\*MELSPEC\_FRAME\_LEN 


        

<hr>



### function ns\_melspec\_init 

_initialize data structures used for mel spectrogram related functions_ 
```C++
void ns_melspec_init (
    ns_melspec_cfg_t * cfg
) 
```




<hr>



### function ns\_melspec\_melspec\_to\_stft 

_Inverts a mel spectrogram back into linear short-time fourier transform._ 
```C++
void ns_melspec_melspec_to_stft (
    ns_melspec_cfg_t * cfg,
    const float32_t * melspec_in,
    float32_t * stft_out
) 
```




<hr>



### function ns\_melspec\_stft\_to\_audio 

_Inverts short-time fourier transform into a reconstructed audio sample._ 
```C++
void ns_melspec_stft_to_audio (
    ns_melspec_cfg_t * cfg,
    float32_t * stft_in,
    int16_t * audio_out
) 
```



We assume the stft input is complex valued (containing magnitude and phase info in alternating 32-bit values).


Note that this operation currently modifies the input buffer! 


        

<hr>



### function ns\_melspec\_stft\_to\_compressed\_melspec 

_Converts complex valued short-time fourier transform into real-valued, power compressed mel scaled space._ 
```C++
void ns_melspec_stft_to_compressed_melspec (
    ns_melspec_cfg_t * cfg,
    const float32_t * stft_in,
    float32_t * melspec_out
) 
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-audio/src/ns_melspec.c`

