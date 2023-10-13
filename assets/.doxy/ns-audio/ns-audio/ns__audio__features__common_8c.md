

# File ns\_audio\_features\_common.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-audio**](dir_45211a8475460839574f71aa108f4957.md) **>** [**src**](dir_e70eef2d5115541d1d6cb7ad27f30382.md) **>** [**ns\_audio\_features\_common.c**](ns__audio__features__common_8c.md)

[Go to the source code of this file](ns__audio__features__common_8c_source.md)



* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "float.h"`
* `#include "ns_audio_features_common.h"`
* `#include <string.h>`





































## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**create\_mel\_fbank**](#function-create_mel_fbank) ([**ns\_fbanks\_cfg\_t**](structns__fbanks__cfg__t.md) \* cfg) <br> |
|  void | [**ns\_fbanks\_init**](#function-ns_fbanks_init) ([**ns\_fbanks\_cfg\_t**](structns__fbanks__cfg__t.md) \* c) <br> |


## Public Static Functions

| Type | Name |
| ---: | :--- |
|  float | [**ns\_audio\_InverseMelScale**](#function-ns_audio_inversemelscale) (float mel\_freq) <br> |
|  float | [**ns\_audio\_MelScale**](#function-ns_audio_melscale) (float freq) <br> |
|  void | [**ns\_fbanks\_map\_arena**](#function-ns_fbanks_map_arena) ([**ns\_fbanks\_cfg\_t**](structns__fbanks__cfg__t.md) \* cfg) <br> |


























## Public Functions Documentation




### function create\_mel\_fbank 

```C++
void create_mel_fbank (
    ns_fbanks_cfg_t * cfg
) 
```






### function ns\_fbanks\_init 

```C++
void ns_fbanks_init (
    ns_fbanks_cfg_t * c
) 
```



## Public Static Functions Documentation




### function ns\_audio\_InverseMelScale 

```C++
static inline float ns_audio_InverseMelScale (
    float mel_freq
) 
```






### function ns\_audio\_MelScale 

```C++
static inline float ns_audio_MelScale (
    float freq
) 
```






### function ns\_fbanks\_map\_arena 

```C++
static void ns_fbanks_map_arena (
    ns_fbanks_cfg_t * cfg
) 
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-audio/src/ns_audio_features_common.c`

