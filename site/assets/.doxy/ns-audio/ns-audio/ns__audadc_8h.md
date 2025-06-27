

# File ns\_audadc.h



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-audio**](dir_45211a8475460839574f71aa108f4957.md) **>** [**src**](dir_e70eef2d5115541d1d6cb7ad27f30382.md) **>** [**ns\_audadc.h**](ns__audadc_8h.md)

[Go to the source code of this file](ns__audadc_8h_source.md)



* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "ns_audio.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  [**ns\_audadc\_cfg\_t**](structns__audadc__cfg__t.md) | [**ns\_audadc\_default**](#variable-ns_audadc_default)  <br> |
|  [**ns\_audadc\_cfg\_t**](structns__audadc__cfg__t.md) | [**ns\_audadc\_vos\_default**](#variable-ns_audadc_vos_default)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**audadc\_deinit**](#function-audadc_deinit) ([**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) \* cfg) <br> |
|  uint32\_t | [**audadc\_init**](#function-audadc_init) ([**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) \* cfg) <br> |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**AUDADC\_MAX\_SAMPLE\_BUF\_SIZE**](ns__audadc_8h.md#define-audadc_max_sample_buf_size)  `(NS\_AUDIO\_DMA\_BUFFER\_SIZE)`<br> |

## Public Attributes Documentation




### variable ns\_audadc\_default 

```C++
ns_audadc_cfg_t ns_audadc_default;
```




<hr>



### variable ns\_audadc\_vos\_default 

```C++
ns_audadc_cfg_t ns_audadc_vos_default;
```




<hr>
## Public Functions Documentation




### function audadc\_deinit 

```C++
void audadc_deinit (
    ns_audio_config_t * cfg
) 
```




<hr>



### function audadc\_init 

```C++
uint32_t audadc_init (
    ns_audio_config_t * cfg
) 
```




<hr>
## Macro Definition Documentation





### define AUDADC\_MAX\_SAMPLE\_BUF\_SIZE 

```C++
#define AUDADC_MAX_SAMPLE_BUF_SIZE `(NS_AUDIO_DMA_BUFFER_SIZE)`
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-audio/src/ns_audadc.h`

