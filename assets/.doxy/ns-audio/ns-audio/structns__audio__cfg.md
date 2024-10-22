

# Struct ns\_audio\_cfg



[**ClassList**](annotated.md) **>** [**ns\_audio\_cfg**](structns__audio__cfg.md)



[More...](#detailed-description)

* `#include <ns_audio.h>`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  const ns\_core\_api\_t \* | [**api**](#variable-api)  <br>_API prefix._  |
|  [**ns\_audadc\_cfg\_t**](structns__audadc__cfg__t.md) \* | [**audadc\_config**](#variable-audadc_config)  <br> |
|  void \* | [**audioBuffer**](#variable-audiobuffer)  <br>_Where the audio will be located when callback occurs._  |
|  void \* | [**audioSystemHandle**](#variable-audiosystemhandle)  <br>_Handle, filled by init._  |
|  ns\_ipc\_ring\_buffer\_t \* | [**bufferHandle**](#variable-bufferhandle)  <br>_Filled by init._  |
|  [**ns\_audio\_callback\_cb**](ns__audio_8h.md#typedef-ns_audio_callback_cb) | [**callback**](#variable-callback)  <br>_Invoked when there is audio in buffer._  |
|  [**ns\_audio\_api\_mode\_e**](ns__audio_8h.md#enum-ns_audio_api_mode_e) | [**eAudioApiMode**](#variable-eaudioapimode)  <br> |
|  [**ns\_audio\_source\_e**](ns__audio_8h.md#enum-ns_audio_source_e) | [**eAudioSource**](#variable-eaudiosource)  <br>_Choose audio source such as AUDADC._  |
|  float | [**fLGAdB**](#variable-flgadb)  <br> |
|  uint8\_t | [**numChannels**](#variable-numchannels)  <br>_Number of audio channels, currently 1 or 2._  |
|  uint16\_t | [**numSamples**](#variable-numsamples)  <br>_Samples collected per callback._  |
|  [**ns\_pdm\_cfg\_t**](structns__pdm__cfg__t.md) \* | [**pdm\_config**](#variable-pdm_config)  <br> |
|  am\_hal\_offset\_cal\_coeffs\_array\_t \* | [**sOffsetCalib**](#variable-soffsetcalib)  <br> |
|  am\_hal\_pdm\_transfer\_t | [**sTransfer**](#variable-stransfer)  <br> |
|  uint32\_t \* | [**sampleBuffer**](#variable-samplebuffer)  <br>_Where samples are DMA'd to._  |
|  uint16\_t | [**sampleRate**](#variable-samplerate)  <br>_In Hz._  |
|  void \* | [**workingBuffer**](#variable-workingbuffer)  <br>_Not used for Apollo4 Lite._  |












































# Detailed Description


NeuralSPOT Audio API Configuration Struct


Audio configuration is via this struct, which also serves as a handle after [**ns\_audio\_init()**](ns__audio_8h.md#function-ns_audio_init) has been invoked 


    
## Public Attributes Documentation




### variable api 

```C++
const ns_core_api_t* ns_audio_cfg::api;
```






### variable audadc\_config 


```C++
ns_audadc_cfg_t* ns_audio_cfg::audadc_config;
```



AUDADC Config - only used by audadc driver 


        



### variable audioBuffer 

```C++
void* ns_audio_cfg::audioBuffer;
```






### variable audioSystemHandle 

_Handle, filled by init._ 
```C++
void* ns_audio_cfg::audioSystemHandle;
```



Internals 


        



### variable bufferHandle 

```C++
ns_ipc_ring_buffer_t* ns_audio_cfg::bufferHandle;
```






### variable callback 

_Invoked when there is audio in buffer._ 
```C++
ns_audio_callback_cb ns_audio_cfg::callback;
```



IPC 


        



### variable eAudioApiMode 


```C++
ns_audio_api_mode_e ns_audio_cfg::eAudioApiMode;
```



Defines how the audio system will interact with the applications 


        



### variable eAudioSource 

_Choose audio source such as AUDADC._ 
```C++
ns_audio_source_e ns_audio_cfg::eAudioSource;
```



Audio Config 


        



### variable fLGAdB 

```C++
float ns_audio_cfg::fLGAdB;
```






### variable numChannels 

```C++
uint8_t ns_audio_cfg::numChannels;
```






### variable numSamples 

```C++
uint16_t ns_audio_cfg::numSamples;
```






### variable pdm\_config 


```C++
ns_pdm_cfg_t* ns_audio_cfg::pdm_config;
```



PDM Config - only used by the pdm driver 


        



### variable sOffsetCalib 

```C++
am_hal_offset_cal_coeffs_array_t* ns_audio_cfg::sOffsetCalib;
```






### variable sTransfer 

```C++
am_hal_pdm_transfer_t ns_audio_cfg::sTransfer;
```






### variable sampleBuffer 

```C++
uint32_t* ns_audio_cfg::sampleBuffer;
```






### variable sampleRate 

```C++
uint16_t ns_audio_cfg::sampleRate;
```






### variable workingBuffer 

```C++
void* ns_audio_cfg::workingBuffer;
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-audio/includes-api/ns_audio.h`

