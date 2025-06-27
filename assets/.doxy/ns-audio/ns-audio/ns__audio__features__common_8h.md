

# File ns\_audio\_features\_common.h



[**FileList**](files.md) **>** [**includes-api**](dir_b70d46c064802b213244316ef6218d52.md) **>** [**ns\_audio\_features\_common.h**](ns__audio__features__common_8h.md)

[Go to the source code of this file](ns__audio__features__common_8h_source.md)



* `#include "arm_math.h"`
* `#include "string.h"`















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**ns\_fbanks\_cfg\_t**](structns__fbanks__cfg__t.md) <br> |


## Public Types

| Type | Name |
| ---: | :--- |
| typedef float | [**ns\_fbank\_t**](#typedef-ns_fbank_t)  <br> |




















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**create\_mel\_fbank**](#function-create_mel_fbank) ([**ns\_fbanks\_cfg\_t**](structns__fbanks__cfg__t.md) \* cfg) <br> |
|  void | [**ns\_fbanks\_init**](#function-ns_fbanks_init) ([**ns\_fbanks\_cfg\_t**](structns__fbanks__cfg__t.md) \* c) <br> |




























## Public Types Documentation




### typedef ns\_fbank\_t 

```C++
typedef float ns_fbank_t[][50];
```




<hr>
## Public Functions Documentation




### function create\_mel\_fbank 

```C++
void create_mel_fbank (
    ns_fbanks_cfg_t * cfg
) 
```




<hr>



### function ns\_fbanks\_init 

```C++
void ns_fbanks_init (
    ns_fbanks_cfg_t * c
) 
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-audio/includes-api/ns_audio_features_common.h`

