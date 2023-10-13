

# File ns\_audio.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-audio**](dir_45211a8475460839574f71aa108f4957.md) **>** [**src**](dir_e70eef2d5115541d1d6cb7ad27f30382.md) **>** [**ns\_audio.c**](ns__audio_8c.md)

[Go to the source code of this file](ns__audio_8c_source.md)

_Implementation of the NeuralSPOT ns-audio API._ [More...](#detailed-description)

* `#include "ns_audio.h"`
* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "ns_audadc.h"`
* `#include "ns_ipc_ring_buffer.h"`
* `#include "ns_pdm.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  [**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) \* | [**g\_ns\_audio\_config**](#variable-g_ns_audio_config)  <br>_Audio Configuration and State._  |
|  const ns\_core\_api\_t | [**ns\_audio\_V0\_0\_1**](#variable-ns_audio_v0_0_1)  <br> |
|  const ns\_core\_api\_t | [**ns\_audio\_V1\_0\_0**](#variable-ns_audio_v1_0_0)  <br> |
|  const ns\_core\_api\_t | [**ns\_audio\_V2\_0\_0**](#variable-ns_audio_v2_0_0)  <br> |
|  const ns\_core\_api\_t | [**ns\_audio\_current\_version**](#variable-ns_audio_current_version)  <br> |
|  const ns\_core\_api\_t | [**ns\_audio\_oldest\_supported\_version**](#variable-ns_audio_oldest_supported_version)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_audio\_getPCM**](#function-ns_audio_getpcm) (int16\_t \* pcm, uint32\_t \* raw, int16\_t len) <br>_Extract int16 PCM from data collected by ADC._  |
|  void | [**ns\_audio\_getPCM\_v2**](#function-ns_audio_getpcm_v2) ([**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) \* config, void \* pcm) <br>_Extract int16 PCM from AUDADC or PDM sources._  |
|  uint32\_t | [**ns\_audio\_init**](#function-ns_audio_init) ([**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) \* cfg) <br>_Initialize NeuralSPOT audio capture library._  |




























# Detailed Description


Purpose: A single point of entry for capturing Ambiq Audio 


    
## Public Attributes Documentation




### variable g\_ns\_audio\_config 

```C++
ns_audio_config_t* g_ns_audio_config;
```






### variable ns\_audio\_V0\_0\_1 

```C++
const ns_core_api_t ns_audio_V0_0_1;
```






### variable ns\_audio\_V1\_0\_0 

```C++
const ns_core_api_t ns_audio_V1_0_0;
```






### variable ns\_audio\_V2\_0\_0 

```C++
const ns_core_api_t ns_audio_V2_0_0;
```






### variable ns\_audio\_current\_version 

```C++
const ns_core_api_t ns_audio_current_version;
```






### variable ns\_audio\_oldest\_supported\_version 

```C++
const ns_core_api_t ns_audio_oldest_supported_version;
```



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




        



### function ns\_audio\_init 

_Initialize NeuralSPOT audio capture library._ 
```C++
uint32_t ns_audio_init (
    ns_audio_config_t * cfg
) 
```





**Parameters:**


* `cfg` : desired configuration 




        

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-audio/src/ns_audio.c`

