

# Group ns-MFCC



[**Modules**](modules.md) **>** [**ns-MFCC**](group__ns-_m_f_c_c.md)




















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**ns\_mfcc\_cfg\_t**](structns__mfcc__cfg__t.md) <br>_Config and state for MFCC calculator._  |






## Public Attributes

| Type | Name |
| ---: | :--- |
|  const ns\_core\_api\_t | [**ns\_mfcc\_V0\_0\_1**](#variable-ns_mfcc_v0_0_1)  <br> |
|  const ns\_core\_api\_t | [**ns\_mfcc\_V1\_0\_0**](#variable-ns_mfcc_v1_0_0)  <br> |
|  const ns\_core\_api\_t | [**ns\_mfcc\_current\_version**](#variable-ns_mfcc_current_version)  <br> |
|  const ns\_core\_api\_t | [**ns\_mfcc\_oldest\_supported\_version**](#variable-ns_mfcc_oldest_supported_version)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**ns\_mfcc\_compute**](#function-ns_mfcc_compute) ([**ns\_mfcc\_cfg\_t**](structns__mfcc__cfg__t.md) \* c, const int16\_t \* audio\_data, float \* mfcc\_out) <br>_Computes MFCC based on audio data._  |
|  uint32\_t | [**ns\_mfcc\_init**](#function-ns_mfcc_init) ([**ns\_mfcc\_cfg\_t**](structns__mfcc__cfg__t.md) \* c) <br>_Initializes the MFCC calculator based on desired configuration._  |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**M\_2PI**](ns__audio__mfcc_8h.md#define-m_2pi)  6.283185307179586476925286766559005<br> |
| define  | [**M\_PI**](ns__audio__mfcc_8h.md#define-m_pi)  3.14159265358979323846264338328<br> |
| define  | [**NS\_MFCC\_API\_ID**](ns__audio__mfcc_8h.md#define-ns_mfcc_api_id)  0xCA0005<br> |
| define  | [**NS\_MFCC\_CURRENT\_VERSION**](ns__audio__mfcc_8h.md#define-ns_mfcc_current_version)  NS\_MFCC\_V1\_0\_0<br> |
| define  | [**NS\_MFCC\_OLDEST\_SUPPORTED\_VERSION**](ns__audio__mfcc_8h.md#define-ns_mfcc_oldest_supported_version)  NS\_MFCC\_V0\_0\_1<br> |
| define  | [**NS\_MFCC\_SIZEBINS**](ns__audio__mfcc_8h.md#define-ns_mfcc_sizebins)  53<br> |
| define  | [**NS\_MFCC\_V0\_0\_1**](ns__audio__mfcc_8h.md#define-ns_mfcc_v0_0_1)          { .major = 0, .minor = 0, .revision = 1 }<br> |
| define  | [**NS\_MFCC\_V1\_0\_0**](ns__audio__mfcc_8h.md#define-ns_mfcc_v1_0_0)          { .major = 1, .minor = 0, .revision = 0 }<br> |

## Public Attributes Documentation




### variable ns\_mfcc\_V0\_0\_1 

```C++
const ns_core_api_t ns_mfcc_V0_0_1;
```






### variable ns\_mfcc\_V1\_0\_0 

```C++
const ns_core_api_t ns_mfcc_V1_0_0;
```






### variable ns\_mfcc\_current\_version 

```C++
const ns_core_api_t ns_mfcc_current_version;
```






### variable ns\_mfcc\_oldest\_supported\_version 

```C++
const ns_core_api_t ns_mfcc_oldest_supported_version;
```



## Public Functions Documentation




### function ns\_mfcc\_compute 

_Computes MFCC based on audio data._ 
```C++
uint32_t ns_mfcc_compute (
    ns_mfcc_cfg_t * c,
    const int16_t * audio_data,
    float * mfcc_out
) 
```





**Parameters:**


* `c` - configuration struct (see [**ns\_mfcc\_cfg\_t**](structns__mfcc__cfg__t.md)) from ns\_mfcc\_init 
* `audio_data` - pointer to audio data (int16\_t) 
* `mfcc_out` - pointer to output buffer (float) 



**Returns:**

uint32\_t 





        



### function ns\_mfcc\_init 

_Initializes the MFCC calculator based on desired configuration._ 
```C++
uint32_t ns_mfcc_init (
    ns_mfcc_cfg_t * c
) 
```





**Parameters:**


* `c` configuration struct (see [**ns\_mfcc\_cfg\_t**](structns__mfcc__cfg__t.md)) 



**Returns:**

uint32\_t status 





        
## Macro Definition Documentation





### define M\_2PI 

```C++
#define M_2PI 6.283185307179586476925286766559005
```






### define M\_PI 

```C++
#define M_PI 3.14159265358979323846264338328
```






### define NS\_MFCC\_API\_ID 

```C++
#define NS_MFCC_API_ID 0xCA0005
```






### define NS\_MFCC\_CURRENT\_VERSION 

```C++
#define NS_MFCC_CURRENT_VERSION NS_MFCC_V1_0_0
```






### define NS\_MFCC\_OLDEST\_SUPPORTED\_VERSION 

```C++
#define NS_MFCC_OLDEST_SUPPORTED_VERSION NS_MFCC_V0_0_1
```






### define NS\_MFCC\_SIZEBINS 

```C++
#define NS_MFCC_SIZEBINS 53
```






### define NS\_MFCC\_V0\_0\_1 

```C++
#define NS_MFCC_V0_0_1 { .major = 0, .minor = 0, .revision = 1 }
```






### define NS\_MFCC\_V1\_0\_0 

```C++
#define NS_MFCC_V1_0_0 { .major = 1, .minor = 0, .revision = 0 }
```




------------------------------

