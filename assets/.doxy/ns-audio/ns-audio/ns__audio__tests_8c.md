

# File ns\_audio\_tests.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-audio**](dir_45211a8475460839574f71aa108f4957.md) **>** [**tests**](dir_ebce5d9663c103d379a321152159c42b.md) **>** [**ns\_audio\_tests.c**](ns__audio__tests_8c.md)

[Go to the source code of this file](ns__audio__tests_8c_source.md)



* `#include "unity/unity.h"`
* `#include "ns_audio.h"`
* `#include <stdint.h>`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  [**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) | [**audioConfig**](#variable-audioconfig)  <br> |
|  am\_hal\_offset\_cal\_coeffs\_array\_t | [**sOffsetCalib**](#variable-soffsetcalib)  <br> |


## Public Static Attributes

| Type | Name |
| ---: | :--- |
|  int16\_t | [**audioDataBuffer**](#variable-audiodatabuffer)  <br> |
|  uint32\_t | [**dmaBuffer**](#variable-dmabuffer)  <br> |
|  volatile static bool | [**g\_audioReady**](#variable-g_audioready)   = `false`<br> |
|  volatile static bool | [**g\_audioRecording**](#variable-g_audiorecording)   = `false`<br> |
|  am\_hal\_audadc\_sample\_t | [**sLGSampleBuffer**](#variable-slgsamplebuffer)  <br> |














## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_audio\_api\_test**](#function-ns_audio_api_test) () <br> |
|  void | [**ns\_audio\_audioSource\_test**](#function-ns_audio_audiosource_test) () <br> |
|  void | [**ns\_audio\_init\_test**](#function-ns_audio_init_test) () <br> |
|  void | [**ns\_audio\_negative\_sample\_rate\_test**](#function-ns_audio_negative_sample_rate_test) () <br> |
|  void | [**ns\_audio\_null\_config\_test**](#function-ns_audio_null_config_test) () <br> |
|  void | [**ns\_audio\_null\_handle\_test**](#function-ns_audio_null_handle_test) () <br> |
|  void | [**ns\_audio\_num\_channels\_greater\_than\_2\_test**](#function-ns_audio_num_channels_greater_than_2_test) () <br> |
|  void | [**ns\_audio\_num\_samples\_test**](#function-ns_audio_num_samples_test) () <br> |
|  void | [**ns\_audio\_pdm\_config\_test**](#function-ns_audio_pdm_config_test) () <br> |
|  void | [**ns\_audio\_tests\_post\_test\_hook**](#function-ns_audio_tests_post_test_hook) () <br> |
|  void | [**ns\_audio\_tests\_pre\_test\_hook**](#function-ns_audio_tests_pre_test_hook) () <br> |
|  void | [**ns\_switch\_audio\_test**](#function-ns_switch_audio_test) () <br> |


## Public Static Functions

| Type | Name |
| ---: | :--- |
|  void | [**audio\_frame\_callback**](#function-audio_frame_callback) ([**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) \* config, uint16\_t bytesCollected) <br> |
|  void | [**initialize\_audio\_config**](#function-initialize_audio_config) () <br> |

























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**NUM\_CHANNELS**](ns__audio__tests_8c.md#define-num_channels)  `1`<br> |
| define  | [**NUM\_FRAMES**](ns__audio__tests_8c.md#define-num_frames)  `100`<br> |
| define  | [**SAMPLES\_IN\_FRAME**](ns__audio__tests_8c.md#define-samples_in_frame)  `480`<br> |
| define  | [**SAMPLE\_RATE**](ns__audio__tests_8c.md#define-sample_rate)  `16000`<br> |

## Public Attributes Documentation




### variable audioConfig 

```C++
ns_audio_config_t audioConfig;
```




<hr>



### variable sOffsetCalib 

```C++
am_hal_offset_cal_coeffs_array_t sOffsetCalib;
```




<hr>
## Public Static Attributes Documentation




### variable audioDataBuffer 

```C++
int16_t audioDataBuffer[SAMPLES_IN_FRAME];
```




<hr>



### variable dmaBuffer 

```C++
uint32_t dmaBuffer[SAMPLES_IN_FRAME *NUM_CHANNELS *2];
```




<hr>



### variable g\_audioReady 

```C++
volatile static bool g_audioReady;
```




<hr>



### variable g\_audioRecording 

```C++
volatile static bool g_audioRecording;
```




<hr>



### variable sLGSampleBuffer 

```C++
am_hal_audadc_sample_t sLGSampleBuffer[SAMPLES_IN_FRAME *NUM_CHANNELS];
```




<hr>
## Public Functions Documentation




### function ns\_audio\_api\_test 

```C++
void ns_audio_api_test () 
```




<hr>



### function ns\_audio\_audioSource\_test 

```C++
void ns_audio_audioSource_test () 
```




<hr>



### function ns\_audio\_init\_test 

```C++
void ns_audio_init_test () 
```




<hr>



### function ns\_audio\_negative\_sample\_rate\_test 

```C++
void ns_audio_negative_sample_rate_test () 
```




<hr>



### function ns\_audio\_null\_config\_test 

```C++
void ns_audio_null_config_test () 
```




<hr>



### function ns\_audio\_null\_handle\_test 

```C++
void ns_audio_null_handle_test () 
```




<hr>



### function ns\_audio\_num\_channels\_greater\_than\_2\_test 

```C++
void ns_audio_num_channels_greater_than_2_test () 
```




<hr>



### function ns\_audio\_num\_samples\_test 

```C++
void ns_audio_num_samples_test () 
```




<hr>



### function ns\_audio\_pdm\_config\_test 

```C++
void ns_audio_pdm_config_test () 
```




<hr>



### function ns\_audio\_tests\_post\_test\_hook 

```C++
void ns_audio_tests_post_test_hook () 
```




<hr>



### function ns\_audio\_tests\_pre\_test\_hook 

```C++
void ns_audio_tests_pre_test_hook () 
```




<hr>



### function ns\_switch\_audio\_test 

```C++
void ns_switch_audio_test () 
```




<hr>
## Public Static Functions Documentation




### function audio\_frame\_callback 

```C++
static void audio_frame_callback (
    ns_audio_config_t * config,
    uint16_t bytesCollected
) 
```




<hr>



### function initialize\_audio\_config 

```C++
static void initialize_audio_config () 
```




<hr>
## Macro Definition Documentation





### define NUM\_CHANNELS 

```C++
#define NUM_CHANNELS `1`
```




<hr>



### define NUM\_FRAMES 

```C++
#define NUM_FRAMES `100`
```




<hr>



### define SAMPLES\_IN\_FRAME 

```C++
#define SAMPLES_IN_FRAME `480`
```




<hr>



### define SAMPLE\_RATE 

```C++
#define SAMPLE_RATE `16000`
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-audio/tests/ns_audio_tests.c`

