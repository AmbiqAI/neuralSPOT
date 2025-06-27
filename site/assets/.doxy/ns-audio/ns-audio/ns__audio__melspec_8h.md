

# File ns\_audio\_melspec.h



[**FileList**](files.md) **>** [**includes-api**](dir_b70d46c064802b213244316ef6218d52.md) **>** [**ns\_audio\_melspec.h**](ns__audio__melspec_8h.md)

[Go to the source code of this file](ns__audio__melspec_8h_source.md)



* `#include "arm_math.h"`
* `#include "ns_audio_features_common.h"`















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**ns\_melspec\_cfg\_t**](structns__melspec__cfg__t.md) <br> |






















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_melspec\_audio\_to\_stft**](#function-ns_melspec_audio_to_stft) ([**ns\_melspec\_cfg\_t**](structns__melspec__cfg__t.md) \* c, const int16\_t \* audio\_data, float32\_t \* stft\_out) <br>_convert the audio samples into a short-time fourier transform_  |
|  void | [**ns\_melspec\_init**](#function-ns_melspec_init) ([**ns\_melspec\_cfg\_t**](structns__melspec__cfg__t.md) \* c) <br>_initialize data structures used for mel spectrogram related functions_  |
|  void | [**ns\_melspec\_melspec\_to\_stft**](#function-ns_melspec_melspec_to_stft) ([**ns\_melspec\_cfg\_t**](structns__melspec__cfg__t.md) \* c, const float32\_t \* melspec\_in, float32\_t \* stft\_out) <br>_Inverts a mel spectrogram back into linear short-time fourier transform._  |
|  void | [**ns\_melspec\_stft\_to\_audio**](#function-ns_melspec_stft_to_audio) ([**ns\_melspec\_cfg\_t**](structns__melspec__cfg__t.md) \* c, float32\_t \* stft\_in, int16\_t \* audio\_out) <br>_Inverts short-time fourier transform into a reconstructed audio sample._  |
|  void | [**ns\_melspec\_stft\_to\_compressed\_melspec**](#function-ns_melspec_stft_to_compressed_melspec) ([**ns\_melspec\_cfg\_t**](structns__melspec__cfg__t.md) \* c, const float32\_t \* stft\_in, float32\_t \* melspec\_out) <br>_Converts complex valued short-time fourier transform into real-valued, power compressed mel scaled space._  |




























## Public Functions Documentation




### function ns\_melspec\_audio\_to\_stft 

_convert the audio samples into a short-time fourier transform_ 
```C++
void ns_melspec_audio_to_stft (
    ns_melspec_cfg_t * c,
    const int16_t * audio_data,
    float32_t * stft_out
) 
```



Result is complex valued (containing magnitude and phase info in alternating 32-bit values, so stft\_out should be 2\*MELSPEC\_FRAME\_LEN 


        

<hr>



### function ns\_melspec\_init 

_initialize data structures used for mel spectrogram related functions_ 
```C++
void ns_melspec_init (
    ns_melspec_cfg_t * c
) 
```




<hr>



### function ns\_melspec\_melspec\_to\_stft 

_Inverts a mel spectrogram back into linear short-time fourier transform._ 
```C++
void ns_melspec_melspec_to_stft (
    ns_melspec_cfg_t * c,
    const float32_t * melspec_in,
    float32_t * stft_out
) 
```




<hr>



### function ns\_melspec\_stft\_to\_audio 

_Inverts short-time fourier transform into a reconstructed audio sample._ 
```C++
void ns_melspec_stft_to_audio (
    ns_melspec_cfg_t * c,
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
    ns_melspec_cfg_t * c,
    const float32_t * stft_in,
    float32_t * melspec_out
) 
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-audio/includes-api/ns_audio_melspec.h`

