

# Struct ns\_mfcc\_cfg\_t



[**ClassList**](annotated.md) **>** [**ns\_mfcc\_cfg\_t**](structns__mfcc__cfg__t.md)



_Config and state for MFCC calculator._ 

* `#include <ns_audio_mfcc.h>`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  const ns\_core\_api\_t \* | [**api**](#variable-api)  <br>_API prefix._  |
|  uint8\_t \* | [**arena**](#variable-arena)  <br>_Pointer to arena (must be allocated by caller)_  |
|  [**ns\_fbanks\_cfg\_t**](structns__fbanks__cfg__t.md) | [**fbc**](#variable-fbc)  <br>_Filterbank config (set internally)_  |
|  uint32\_t | [**frame\_len**](#variable-frame_len)  <br>_Frame length._  |
|  uint32\_t | [**frame\_len\_ms**](#variable-frame_len_ms)  <br>_Not used._  |
|  uint32\_t | [**frame\_len\_pow2**](#variable-frame_len_pow2)  <br>_Frame length to nearest power of 2._  |
|  uint32\_t | [**frame\_shift\_ms**](#variable-frame_shift_ms)  <br>_Not used._  |
|  uint32\_t | [**high\_freq**](#variable-high_freq)  <br>_High frequency cutoff._  |
|  uint32\_t | [**low\_freq**](#variable-low_freq)  <br>_Low frequency cutoff._  |
|  float \* | [**mfccBuffer**](#variable-mfccbuffer)  <br>_pointer to MFCC buffer (set internally)_  |
|  float \* | [**mfccDCTMatrix**](#variable-mfccdctmatrix)  <br>_pointer to MFCC DCT matrix (set internally)_  |
|  float \* | [**mfccEnergies**](#variable-mfccenergies)  <br>_pointer to MFCC energies (set internally)_  |
|  float \* | [**mfccFrame**](#variable-mfccframe)  <br>_pointer to MFCC frame (set internally)_  |
|  float \* | [**mfccWindowFunction**](#variable-mfccwindowfunction)  <br>_pointer to MFCC window function (set internally)_  |
|  uint32\_t | [**num\_coeffs**](#variable-num_coeffs)  <br>_Number of MFCC coefficients._  |
|  uint32\_t | [**num\_dec\_bits**](#variable-num_dec_bits)  <br>_Number of decimation bits._  |
|  uint32\_t | [**num\_fbank\_bins**](#variable-num_fbank_bins)  <br>_Number of filterbank bins._  |
|  uint32\_t | [**num\_frames**](#variable-num_frames)  <br>_Number of frames._  |
|  uint32\_t | [**sample\_frequency**](#variable-sample_frequency)  <br>_Sample frequency of audio data._  |












































## Public Attributes Documentation




### variable api 

_API prefix._ 
```C++
const ns_core_api_t* ns_mfcc_cfg_t::api;
```




<hr>



### variable arena 

_Pointer to arena (must be allocated by caller)_ 
```C++
uint8_t* ns_mfcc_cfg_t::arena;
```




<hr>



### variable fbc 

_Filterbank config (set internally)_ 
```C++
ns_fbanks_cfg_t ns_mfcc_cfg_t::fbc;
```




<hr>



### variable frame\_len 

_Frame length._ 
```C++
uint32_t ns_mfcc_cfg_t::frame_len;
```




<hr>



### variable frame\_len\_ms 

_Not used._ 
```C++
uint32_t ns_mfcc_cfg_t::frame_len_ms;
```




<hr>



### variable frame\_len\_pow2 

_Frame length to nearest power of 2._ 
```C++
uint32_t ns_mfcc_cfg_t::frame_len_pow2;
```




<hr>



### variable frame\_shift\_ms 

_Not used._ 
```C++
uint32_t ns_mfcc_cfg_t::frame_shift_ms;
```




<hr>



### variable high\_freq 

_High frequency cutoff._ 
```C++
uint32_t ns_mfcc_cfg_t::high_freq;
```




<hr>



### variable low\_freq 

_Low frequency cutoff._ 
```C++
uint32_t ns_mfcc_cfg_t::low_freq;
```




<hr>



### variable mfccBuffer 

_pointer to MFCC buffer (set internally)_ 
```C++
float* ns_mfcc_cfg_t::mfccBuffer;
```




<hr>



### variable mfccDCTMatrix 

_pointer to MFCC DCT matrix (set internally)_ 
```C++
float* ns_mfcc_cfg_t::mfccDCTMatrix;
```




<hr>



### variable mfccEnergies 

_pointer to MFCC energies (set internally)_ 
```C++
float* ns_mfcc_cfg_t::mfccEnergies;
```




<hr>



### variable mfccFrame 

_pointer to MFCC frame (set internally)_ 
```C++
float* ns_mfcc_cfg_t::mfccFrame;
```




<hr>



### variable mfccWindowFunction 

_pointer to MFCC window function (set internally)_ 
```C++
float* ns_mfcc_cfg_t::mfccWindowFunction;
```




<hr>



### variable num\_coeffs 

_Number of MFCC coefficients._ 
```C++
uint32_t ns_mfcc_cfg_t::num_coeffs;
```




<hr>



### variable num\_dec\_bits 

_Number of decimation bits._ 
```C++
uint32_t ns_mfcc_cfg_t::num_dec_bits;
```




<hr>



### variable num\_fbank\_bins 

_Number of filterbank bins._ 
```C++
uint32_t ns_mfcc_cfg_t::num_fbank_bins;
```




<hr>



### variable num\_frames 

_Number of frames._ 
```C++
uint32_t ns_mfcc_cfg_t::num_frames;
```




<hr>



### variable sample\_frequency 

_Sample frequency of audio data._ 
```C++
uint32_t ns_mfcc_cfg_t::sample_frequency;
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-audio/includes-api/ns_audio_mfcc.h`

