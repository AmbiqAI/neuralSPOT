

# File ns\_pdm.h



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-audio**](dir_45211a8475460839574f71aa108f4957.md) **>** [**src**](dir_e70eef2d5115541d1d6cb7ad27f30382.md) **>** [**ns\_pdm.h**](ns__pdm_8h.md)

[Go to the source code of this file](ns__pdm_8h_source.md)

[More...](#detailed-description)

* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "ns_audio.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  [**ns\_pdm\_cfg\_t**](structns__pdm__cfg__t.md) | [**ns\_pdm\_default**](#variable-ns_pdm_default)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**pdm\_deinit**](#function-pdm_deinit) ([**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) \* config) <br> |
|  uint32\_t | [**pdm\_init**](#function-pdm_init) ([**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) \* config) <br>_AUDADC subsystem init - should only be invoked by ns\_audio, not directly._  |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**NS\_AUDIO\_PDM\_SAMPLE\_16BIT**](ns__pdm_8h.md#define-ns_audio_pdm_sample_16bit)  2<br> |

# Detailed Description




**Author:**

your name ([you@domain.com](mailto:you@domain.com)) 




**Version:**

0.1 




**Date:**

2023-05-22




**Copyright:**

Copyright (c) 2023 





    
## Public Attributes Documentation




### variable ns\_pdm\_default 

```C++
ns_pdm_cfg_t ns_pdm_default;
```



## Public Functions Documentation




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



## Macro Definition Documentation





### define NS\_AUDIO\_PDM\_SAMPLE\_16BIT 

```C++
#define NS_AUDIO_PDM_SAMPLE_16BIT 2
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-audio/src/ns_pdm.h`

