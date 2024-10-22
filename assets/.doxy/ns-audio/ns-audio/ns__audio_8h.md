

# File ns\_audio.h



[**FileList**](files.md) **>** [**includes-api**](dir_b70d46c064802b213244316ef6218d52.md) **>** [**ns\_audio.h**](ns__audio_8h.md)

[Go to the source code of this file](ns__audio_8h_source.md)

_API Definition for NeuralSPOT Audio library._ [More...](#detailed-description)

* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "ns_core.h"`
* `#include "ns_ipc_ring_buffer.h"`















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**ns\_audadc\_cfg\_t**](structns__audadc__cfg__t.md) <br>_AUDADC Configuration._  |
| struct | [**ns\_audio\_cfg**](structns__audio__cfg.md) <br> |
| struct | [**ns\_pdm\_cfg\_t**](structns__pdm__cfg__t.md) <br>_PDM Configuration._  |


## Public Types

| Type | Name |
| ---: | :--- |
| enum  | [**ns\_audio\_api\_mode\_e**](#enum-ns_audio_api_mode_e)  <br>_Audio IPC Modes._  |
| typedef void(\* | [**ns\_audio\_callback\_cb**](#typedef-ns_audio_callback_cb)  <br>_Invoked by IRQ when audio buffer is ready._  |
| enum  | [**ns\_audio\_clksel\_e**](#enum-ns_audio_clksel_e)  <br>_Audio Clock Source._  |
| typedef struct [**ns\_audio\_cfg**](structns__audio__cfg.md) | [**ns\_audio\_config\_t**](#typedef-ns_audio_config_t)  <br> |
| enum  | [**ns\_audio\_pdm\_clock\_e**](#enum-ns_audio_pdm_clock_e)  <br>_PDM Clock Frequency._  |
| enum  | [**ns\_audio\_pdm\_micsel\_e**](#enum-ns_audio_pdm_micsel_e)  <br>_PDM Microphone Selection._  |
| enum  | [**ns\_audio\_source\_e**](#enum-ns_audio_source_e)  <br>_Audio Source (current only AUDADC and PDM are supported)_  |




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
















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_audio\_getPCM**](#function-ns_audio_getpcm) (int16\_t \* pcm, uint32\_t \* raw, int16\_t len) <br>_Extract int16 PCM from data collected by ADC._  |
|  void | [**ns\_audio\_getPCM\_v2**](#function-ns_audio_getpcm_v2) ([**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) \* config, void \* pcm) <br>_Extract int16 PCM from AUDADC or PDM sources._  |
|  uint32\_t | [**ns\_audio\_init**](#function-ns_audio_init) ([**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) \* cfg) <br>_Initialize NeuralSPOT audio capture library._  |
|  uint32\_t | [**ns\_end\_audio**](#function-ns_end_audio) ([**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) \* cfg) <br>_Stop audio capture._  |
|  uint32\_t | [**ns\_start\_audio**](#function-ns_start_audio) ([**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) \* cfg) <br>_Start audio capture, must be called after ns\_audio\_init._  |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**NS\_AUDIO\_API\_ID**](ns__audio_8h.md#define-ns_audio_api_id)  0xCA0001<br> |
| define  | [**NS\_AUDIO\_CURRENT\_VERSION**](ns__audio_8h.md#define-ns_audio_current_version)  NS\_AUDIO\_V2\_1\_0<br> |
| define  | [**NS\_AUDIO\_DMA\_BUFFER\_SIZE**](ns__audio_8h.md#define-ns_audio_dma_buffer_size)  480<br> |
| define  | [**NS\_AUDIO\_OLDEST\_SUPPORTED\_VERSION**](ns__audio_8h.md#define-ns_audio_oldest_supported_version)  NS\_AUDIO\_V0\_0\_1<br> |
| define  | [**NS\_AUDIO\_V0\_0\_1**](ns__audio_8h.md#define-ns_audio_v0_0_1)          { .major = 0, .minor = 0, .revision = 1 }<br> |
| define  | [**NS\_AUDIO\_V1\_0\_0**](ns__audio_8h.md#define-ns_audio_v1_0_0)          { .major = 1, .minor = 0, .revision = 0 }<br> |
| define  | [**NS\_AUDIO\_V2\_0\_0**](ns__audio_8h.md#define-ns_audio_v2_0_0)          { .major = 2, .minor = 0, .revision = 0 }<br> |
| define  | [**NS\_AUDIO\_V2\_1\_0**](ns__audio_8h.md#define-ns_audio_v2_1_0)          { .major = 2, .minor = 1, .revision = 0 }<br> |

# Detailed Description




**Author:**

Carlos Morales 




**Version:**

0.1 




**Date:**

2022-08-12




**Copyright:**

Copyright (c) 2022 





    
## Public Types Documentation




### enum ns\_audio\_api\_mode\_e 

```C++
enum ns_audio_api_mode_e {
    NS_AUDIO_API_CALLBACK,
    NS_AUDIO_API_RINGBUFFER,
    NS_AUDIO_API_TASK
};
```






### typedef ns\_audio\_callback\_cb 

```C++
typedef void(* ns_audio_callback_cb) (struct ns_audio_cfg *, uint16_t);
```






### enum ns\_audio\_clksel\_e 

```C++
enum ns_audio_clksel_e {
    NS_CLKSEL_XTHS,
    NS_CLKSEL_HFXTAL,
    NS_CLKSEL_HFRC,
    NS_CLKSEL_HFRC2,
    NS_CLKSEL_HFRC2_ADJ
};
```






### typedef ns\_audio\_config\_t 


```C++
typedef struct ns_audio_cfg ns_audio_config_t;
```



NeuralSPOT Audio API Configuration Struct


Audio configuration is via this struct, which also serves as a handle after [**ns\_audio\_init()**](ns__audio_8h.md#function-ns_audio_init) has been invoked 


        



### enum ns\_audio\_pdm\_clock\_e 

```C++
enum ns_audio_pdm_clock_e {
    NS_AUDIO_PDM_CLK_750KHZ,
    NS_AUDIO_PDM_CLK_1_5MHZ
};
```






### enum ns\_audio\_pdm\_micsel\_e 

```C++
enum ns_audio_pdm_micsel_e {
    NS_AUDIO_PDM_MICBOARD_0 = 0,
    NS_AUDIO_PDM_MICBOARD_1 = 2,
    NS_AUDIO_PDM_MICBOARD_2 = 1
};
```






### enum ns\_audio\_source\_e 

```C++
enum ns_audio_source_e {
    NS_AUDIO_SOURCE_AUDADC,
    NS_AUDIO_SOURCE_PDM
};
```



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






### variable ns\_audio\_V2\_1\_0 

```C++
const ns_core_api_t ns_audio_V2_1_0;
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




        



### function ns\_end\_audio 

_Stop audio capture._ 
```C++
uint32_t ns_end_audio (
    ns_audio_config_t * cfg
) 
```





**Parameters:**


* `cfg` : desired configuration 




        



### function ns\_start\_audio 

_Start audio capture, must be called after ns\_audio\_init._ 
```C++
uint32_t ns_start_audio (
    ns_audio_config_t * cfg
) 
```





**Parameters:**


* `cfg` : desired configuration 




        
## Macro Definition Documentation





### define NS\_AUDIO\_API\_ID 

```C++
#define NS_AUDIO_API_ID 0xCA0001
```






### define NS\_AUDIO\_CURRENT\_VERSION 

```C++
#define NS_AUDIO_CURRENT_VERSION NS_AUDIO_V2_1_0
```






### define NS\_AUDIO\_DMA\_BUFFER\_SIZE 

```C++
#define NS_AUDIO_DMA_BUFFER_SIZE 480
```






### define NS\_AUDIO\_OLDEST\_SUPPORTED\_VERSION 

```C++
#define NS_AUDIO_OLDEST_SUPPORTED_VERSION NS_AUDIO_V0_0_1
```






### define NS\_AUDIO\_V0\_0\_1 

```C++
#define NS_AUDIO_V0_0_1 { .major = 0, .minor = 0, .revision = 1 }
```






### define NS\_AUDIO\_V1\_0\_0 

```C++
#define NS_AUDIO_V1_0_0 { .major = 1, .minor = 0, .revision = 0 }
```






### define NS\_AUDIO\_V2\_0\_0 

```C++
#define NS_AUDIO_V2_0_0 { .major = 2, .minor = 0, .revision = 0 }
```






### define NS\_AUDIO\_V2\_1\_0 

```C++
#define NS_AUDIO_V2_1_0 { .major = 2, .minor = 1, .revision = 0 }
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-audio/includes-api/ns_audio.h`

