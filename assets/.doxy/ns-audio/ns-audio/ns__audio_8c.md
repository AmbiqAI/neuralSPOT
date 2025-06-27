

# File ns\_audio.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-audio**](dir_45211a8475460839574f71aa108f4957.md) **>** [**src**](dir_e70eef2d5115541d1d6cb7ad27f30382.md) **>** [**ns\_audio.c**](ns__audio_8c.md)

[Go to the source code of this file](ns__audio_8c_source.md)

_Implementation of the NeuralSPOT ns-audio API._ [More...](#detailed-description)

* `#include "ns_audio.h"`
* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "ns_ipc_ring_buffer.h"`
* `#include "ns_pdm.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  [**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) \* | [**g\_ns\_audio\_config**](#variable-g_ns_audio_config)  <br>_Audio Configuration and State._  |
|  const ns\_core\_api\_t | [**ns\_audio\_V0\_0\_1**](#variable-ns_audio_v0_0_1)  <br> |
|  const ns\_core\_api\_t | [**ns\_audio\_V1\_0\_0**](#variable-ns_audio_v1_0_0)  <br> |
|  const ns\_core\_api\_t | [**ns\_audio\_V2\_0\_0**](#variable-ns_audio_v2_0_0)  <br> |
|  const ns\_core\_api\_t | [**ns\_audio\_V2\_1\_0**](#variable-ns_audio_v2_1_0)  <br> |
|  const ns\_core\_api\_t | [**ns\_audio\_current\_version**](#variable-ns_audio_current_version)  <br> |
|  const ns\_core\_api\_t | [**ns\_audio\_oldest\_supported\_version**](#variable-ns_audio_oldest_supported_version)  <br> |


## Public Static Attributes

| Type | Name |
| ---: | :--- |
|  bool | [**audio\_initialized**](#variable-audio_initialized)   = `false`<br> |
|  bool | [**audio\_started**](#variable-audio_started)   = `false`<br> |














## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_audio\_getPCM**](#function-ns_audio_getpcm) (int16\_t \* pcm, uint32\_t \* raw, int16\_t len) <br>_Extract int16 PCM from data collected by ADC._  |
|  void | [**ns\_audio\_getPCM\_v2**](#function-ns_audio_getpcm_v2) ([**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) \* config, void \* pcm) <br>_Extract int16 PCM from AUDADC or PDM sources._  |
|  uint32\_t | [**ns\_audio\_init**](#function-ns_audio_init) ([**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) \* cfg) <br>_Initialize NeuralSPOT audio capture library._  |
|  uint32\_t | [**ns\_audio\_set\_gain**](#function-ns_audio_set_gain) (int left\_gain, int right\_gain) <br>_Set gain of audio source._  |
|  uint32\_t | [**ns\_end\_audio**](#function-ns_end_audio) ([**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) \* cfg) <br>_Stop audio capture._  |
|  uint32\_t | [**ns\_start\_audio**](#function-ns_start_audio) ([**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) \* cfg) <br>_Start audio capture, must be called after ns\_audio\_init._  |




























## Detailed Description


Purpose: A single point of entry for capturing Ambiq Audio 


    
## Public Attributes Documentation




### variable g\_ns\_audio\_config 

_Audio Configuration and State._ 
```C++
ns_audio_config_t* g_ns_audio_config;
```




<hr>



### variable ns\_audio\_V0\_0\_1 

```C++
const ns_core_api_t ns_audio_V0_0_1;
```




<hr>



### variable ns\_audio\_V1\_0\_0 

```C++
const ns_core_api_t ns_audio_V1_0_0;
```




<hr>



### variable ns\_audio\_V2\_0\_0 

```C++
const ns_core_api_t ns_audio_V2_0_0;
```




<hr>



### variable ns\_audio\_V2\_1\_0 

```C++
const ns_core_api_t ns_audio_V2_1_0;
```




<hr>



### variable ns\_audio\_current\_version 

```C++
const ns_core_api_t ns_audio_current_version;
```




<hr>



### variable ns\_audio\_oldest\_supported\_version 

```C++
const ns_core_api_t ns_audio_oldest_supported_version;
```




<hr>
## Public Static Attributes Documentation




### variable audio\_initialized 

```C++
bool audio_initialized;
```




<hr>



### variable audio\_started 

```C++
bool audio_started;
```




<hr>
## Public Functions Documentation




### function ns\_audio\_getPCM 

_Extract int16 PCM from data collected by ADC._ 
```C++
void ns_audio_getPCM (
    int16_t * pcm,
    uint32_t * raw,
    int16_t len
) 
```





**Parameters:**


* `pcm` - resulting PCM data 
* `raw` - incoming data from ADC engine 
* `len` - number of sample words to convert 




        

<hr>



### function ns\_audio\_getPCM\_v2 

_Extract int16 PCM from AUDADC or PDM sources._ 
```C++
void ns_audio_getPCM_v2 (
    ns_audio_config_t * config,
    void * pcm
) 
```





**Parameters:**


* `config` - ns audio config 
* `pcm` - resulting PCM data 




        

<hr>



### function ns\_audio\_init 

_Initialize NeuralSPOT audio capture library._ 
```C++
uint32_t ns_audio_init (
    ns_audio_config_t * cfg
) 
```





**Parameters:**


* `cfg` : desired configuration 




        

<hr>



### function ns\_audio\_set\_gain 

_Set gain of audio source._ 
```C++
uint32_t ns_audio_set_gain (
    int left_gain,
    int right_gain
) 
```





**Parameters:**


* `left_gain` - left channel gain 
* `right_gain` - right channel gain 




        

<hr>



### function ns\_end\_audio 

_Stop audio capture._ 
```C++
uint32_t ns_end_audio (
    ns_audio_config_t * cfg
) 
```





**Parameters:**


* `cfg` : desired configuration 




        

<hr>



### function ns\_start\_audio 

_Start audio capture, must be called after ns\_audio\_init._ 
```C++
uint32_t ns_start_audio (
    ns_audio_config_t * cfg
) 
```





**Parameters:**


* `cfg` : desired configuration 




        

<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-audio/src/ns_audio.c`

