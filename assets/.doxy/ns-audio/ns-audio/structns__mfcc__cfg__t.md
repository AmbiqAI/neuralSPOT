

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

```C++
const ns_core_api_t* ns_mfcc_cfg_t::api;
```






### variable arena 

```C++
uint8_t* ns_mfcc_cfg_t::arena;
```






### variable fbc 

```C++
ns_fbanks_cfg_t ns_mfcc_cfg_t::fbc;
```






### variable frame\_len 

```C++
uint32_t ns_mfcc_cfg_t::frame_len;
```






### variable frame\_len\_ms 

```C++
uint32_t ns_mfcc_cfg_t::frame_len_ms;
```






### variable frame\_len\_pow2 

```C++
uint32_t ns_mfcc_cfg_t::frame_len_pow2;
```






### variable frame\_shift\_ms 

```C++
uint32_t ns_mfcc_cfg_t::frame_shift_ms;
```






### variable high\_freq 

```C++
uint32_t ns_mfcc_cfg_t::high_freq;
```






### variable low\_freq 

```C++
uint32_t ns_mfcc_cfg_t::low_freq;
```






### variable mfccBuffer 

```C++
float* ns_mfcc_cfg_t::mfccBuffer;
```






### variable mfccDCTMatrix 

```C++
float* ns_mfcc_cfg_t::mfccDCTMatrix;
```






### variable mfccEnergies 

```C++
float* ns_mfcc_cfg_t::mfccEnergies;
```






### variable mfccFrame 

```C++
float* ns_mfcc_cfg_t::mfccFrame;
```






### variable mfccWindowFunction 

```C++
float* ns_mfcc_cfg_t::mfccWindowFunction;
```






### variable num\_coeffs 

```C++
uint32_t ns_mfcc_cfg_t::num_coeffs;
```






### variable num\_dec\_bits 

```C++
uint32_t ns_mfcc_cfg_t::num_dec_bits;
```






### variable num\_fbank\_bins 

```C++
uint32_t ns_mfcc_cfg_t::num_fbank_bins;
```






### variable num\_frames 

```C++
uint32_t ns_mfcc_cfg_t::num_frames;
```






### variable sample\_frequency 

```C++
uint32_t ns_mfcc_cfg_t::sample_frequency;
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-audio/includes-api/ns_audio_mfcc.h`

