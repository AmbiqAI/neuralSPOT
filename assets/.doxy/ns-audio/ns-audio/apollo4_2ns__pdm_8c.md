

# File ns\_pdm.c



[**FileList**](files.md) **>** [**apollo4**](dir_9e4df1ce7893b775eb3c7fcb555505ab.md) **>** [**ns\_pdm.c**](apollo4_2ns__pdm_8c.md)

[Go to the source code of this file](apollo4_2ns__pdm_8c_source.md)

_Driver for Ambiq PDM Microphone interface._ [More...](#detailed-description)

* `#include "../ns_pdm.h"`
* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "ns_audio.h"`
* `#include "ns_core.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  [**ns\_pdm\_cfg\_t**](structns__pdm__cfg__t.md) | [**ns\_pdm\_default**](#variable-ns_pdm_default)   = = {
    .clock = NS\_CLKSEL\_HFRC,
    .clock\_freq = NS\_AUDIO\_PDM\_CLK\_750KHZ,
    .mic = NS\_AUDIO\_PDM\_MICBOARD\_0,
    .numBytes = NS\_AUDIO\_PDM\_SAMPLE\_16BIT,
}<br> |


## Public Static Attributes

| Type | Name |
| ---: | :--- |
|  const IRQn\_Type | [**g\_ePdmInterrupts**](#variable-g_epdminterrupts)   = = {PDM0\_IRQn, PDM1\_IRQn, PDM2\_IRQn, PDM3\_IRQn}<br> |
|  void \* | [**pvPDMHandle**](#variable-pvpdmhandle)  <br> |














## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**am\_pdm0\_isr**](#function-am_pdm0_isr) (void) <br> |
|  void | [**am\_pdm1\_isr**](#function-am_pdm1_isr) (void) <br> |
|  void | [**am\_pdm2\_isr**](#function-am_pdm2_isr) (void) <br> |
|  void | [**am\_pdm\_isr\_common**](#function-am_pdm_isr_common) (uint8\_t pdmNumber) <br> |
|  void | [**pdm\_deinit**](#function-pdm_deinit) ([**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) \* config) <br> |
|  uint32\_t | [**pdm\_init**](#function-pdm_init) ([**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) \* config) <br>_AUDADC subsystem init - should only be invoked by ns\_audio, not directly._  |
|  void | [**pdm\_trigger\_dma**](#function-pdm_trigger_dma) ([**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) \* config) <br> |




























# Detailed Description




**Author:**

Ambiq 




**Version:**

0.1 




**Date:**

2023-05-17




**Copyright:**

Copyright (c) 2023 





    
## Public Attributes Documentation




### variable ns\_pdm\_default 

```C++
ns_pdm_cfg_t ns_pdm_default;
```



## Public Static Attributes Documentation




### variable g\_ePdmInterrupts 

```C++
const IRQn_Type g_ePdmInterrupts[];
```






### variable pvPDMHandle 

```C++
void* pvPDMHandle;
```



## Public Functions Documentation




### function am\_pdm0\_isr 

```C++
void am_pdm0_isr (
    void
) 
```






### function am\_pdm1\_isr 

```C++
void am_pdm1_isr (
    void
) 
```






### function am\_pdm2\_isr 

```C++
void am_pdm2_isr (
    void
) 
```






### function am\_pdm\_isr\_common 

```C++
void am_pdm_isr_common (
    uint8_t pdmNumber
) 
```






### function pdm\_deinit 

```C++
void pdm_deinit (
    ns_audio_config_t * config
) 
```






### function pdm\_init 

```C++
uint32_t pdm_init (
    ns_audio_config_t * config
) 
```






### function pdm\_trigger\_dma 

```C++
void pdm_trigger_dma (
    ns_audio_config_t * config
) 
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-audio/src/apollo4/ns_pdm.c`

