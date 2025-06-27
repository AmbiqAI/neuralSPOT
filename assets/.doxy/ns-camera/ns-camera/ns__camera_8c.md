

# File ns\_camera.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-camera**](dir_34c4a2041c04f1165fd5b331589bb26b.md) **>** [**src**](dir_368bfd9171cce0e4a9a7d7b2cf674b9a.md) **>** [**ns\_camera.c**](ns__camera_8c.md)

[Go to the source code of this file](ns__camera_8c_source.md)



* `#include <stdlib.h>`
* `#include "ns_camera.h"`
* `#include "ArducamCamera.h"`
* `#include "arm_math.h"`
* `#include "jpeg-decoder/jpeg_decoder.h"`
* `#include "jpeg-decoder/picojpeg.h"`
* `#include "ns_spi.h"`
* `#include "ns_ambiqsuite_harness.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  [**ArducamCamera**](struct_arducam_camera.md) | [**camera**](#variable-camera)  <br> |
|  [**jpeg\_decoder\_context\_t**](structjpeg__decoder__context__t.md) | [**jpegCtx**](#variable-jpegctx)   = `{}`<br> |
|  bool | [**nsCameraPictureBeingTaken**](#variable-nscamerapicturebeingtaken)   = `false`<br> |
|  const ns\_core\_api\_t | [**ns\_camera\_V1\_0\_0**](#variable-ns_camera_v1_0_0)   = `{.apiId = NS\_CAMERA\_API\_ID, .version = NS\_CAMERA\_V1\_0\_0}`<br> |
|  [**ns\_camera\_config\_t**](structns__camera__cfg.md) | [**ns\_camera\_config**](#variable-ns_camera_config)  <br> |
|  const ns\_core\_api\_t | [**ns\_camera\_current\_version**](#variable-ns_camera_current_version)   = `/* multi line expression */`<br> |
|  const ns\_core\_api\_t | [**ns\_camera\_oldest\_supported\_version**](#variable-ns_camera_oldest_supported_version)   = `/* multi line expression */`<br> |
|  bool | [**ns\_read\_done**](#variable-ns_read_done)   = `false`<br> |
|  ns\_spi\_config\_t \* | [**spiHandle**](#variable-spihandle)   = `NULL`<br> |
|  ns\_timer\_config\_t | [**timerCfg**](#variable-timercfg)   = `/* multi line expression */`<br> |


## Public Static Attributes

| Type | Name |
| ---: | :--- |
|  uint8\_t \* | [**dma\_cambuf**](#variable-dma_cambuf)  <br> |
|  uint32\_t | [**dma\_current\_chunk\_length**](#variable-dma_current_chunk_length)  <br> |
|  uint32\_t | [**dma\_offset**](#variable-dma_offset)  <br> |
|  uint32\_t | [**dma\_total\_requested\_length**](#variable-dma_total_requested_length)  <br> |














## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**arducam\_delay\_ms**](#function-arducam_delay_ms) (uint16\_t delay) <br> |
|  void | [**arducam\_delay\_us**](#function-arducam_delay_us) (uint16\_t delay) <br> |
|  int | [**arducam\_spi\_read**](#function-arducam_spi_read) (const void \* buf, uint32\_t bufLen, uint64\_t reg, uint32\_t regLen, uint32\_t csPin) <br> |
|  int | [**arducam\_spi\_write**](#function-arducam_spi_write) (const void \* buf, uint32\_t bufLen, uint64\_t reg, uint32\_t regLen, uint32\_t csPin) <br> |
|  int | [**camera\_decode\_image**](#function-camera_decode_image) (uint8\_t \* camBuf, uint32\_t camLen, uint8\_t \* imgBuf, uint32\_t imgWidth, uint32\_t imgHeight, uint32\_t scaleFactor) <br>_Decode a JPEG image from the camera buffer to RGB565 buffer._  |
|  void | [**ns\_camera\_adjust\_settings**](#function-ns_camera_adjust_settings) (int8\_t contrast, int8\_t brightness, int8\_t ev) <br>_Adjust camera settings._  |
|  uint32\_t | [**ns\_camera\_capture**](#function-ns_camera_capture) ([**ns\_camera\_config\_t**](structns__camera__cfg.md) \* cfg, uint8\_t \* camBuf, uint32\_t bufLen) <br>_Convenience routine to capture and transfer next frame._  |
|  void | [**ns\_camera\_check\_picture\_completion**](#function-ns_camera_check_picture_completion) (ns\_timer\_config\_t \* timer) <br> |
|  uint32\_t | [**ns\_camera\_init**](#function-ns_camera_init) ([**ns\_camera\_config\_t**](structns__camera__cfg.md) \* cfg) <br>_Initialize the camera._  |
|  uint32\_t | [**ns\_chop\_off\_trailing\_zeros**](#function-ns_chop_off_trailing_zeros) (uint8\_t \* buff, uint32\_t length) <br>_Chop off trailing zeros from a buffer._  |
|  int | [**ns\_is\_camera\_capturing**](#function-ns_is_camera_capturing) () <br>_Check if camera is still capturing This is a helper function, it should typically only be used by ns\_camera._  |
|  uint32\_t | [**ns\_press\_shutter\_button**](#function-ns_press_shutter_button) ([**ns\_camera\_config\_t**](structns__camera__cfg.md) \* cfg) <br>_Press the shutter button Start the capture process, turns on a timer to poll the camera for completion._  |
|  uint32\_t | [**ns\_read\_buff**](#function-ns_read_buff) ([**ArducamCamera**](struct_arducam_camera.md) \* camera, uint8\_t \* buff, uint32\_t length) <br> |
|  void | [**ns\_rgb565\_to\_rgb888**](#function-ns_rgb565_to_rgb888) (uint16\_t rgb565Pixel, uint8\_t \* r, uint8\_t \* g, uint8\_t \* b) <br> |
|  uint32\_t | [**ns\_start\_camera**](#function-ns_start_camera) ([**ns\_camera\_config\_t**](structns__camera__cfg.md) \* cfg) <br>_Start the camera (take out of low power mode)_  |
|  uint32\_t | [**ns\_start\_dma\_read**](#function-ns_start_dma_read) ([**ns\_camera\_config\_t**](structns__camera__cfg.md) \* cfg, uint8\_t \* camBuf, uint32\_t \* buffer\_offset, uint32\_t bufLen) <br>_Start a DMA read of the camera buffer._  |
|  uint32\_t | [**ns\_stop\_camera**](#function-ns_stop_camera) ([**ns\_camera\_config\_t**](structns__camera__cfg.md) \* cfg) <br>_Stop the camera (put into low power mode)_  |
|  uint32\_t | [**ns\_take\_picture**](#function-ns_take_picture) ([**ns\_camera\_config\_t**](structns__camera__cfg.md) \* cfg) <br>_Take a picture in mode specified by cfg Once picture is taken, it can be transferred using ns\_transfer\_picture._  |
|  uint32\_t | [**ns\_transfer\_picture**](#function-ns_transfer_picture) ([**ns\_camera\_config\_t**](structns__camera__cfg.md) \* cfg, uint8\_t \* camBuf, uint32\_t \* buffer\_offset, uint32\_t bufLen) <br>_Transfer captured frame over SPI to local buffer NOTE: This routine is blocking and will wait for inflight capture._  |


## Public Static Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_camera\_buff\_read\_done**](#function-ns_camera_buff_read_done) (ns\_spi\_config\_t \* cfg) <br> |
|  uint8\_t | [**ns\_mapCameraValuesToArducamScale**](#function-ns_mapcameravaluestoarducamscale) (int8\_t in) <br> |

























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**ARDU\_BURST\_FIFO\_READ**](ns__camera_8c.md#define-ardu_burst_fifo_read)  `0x3C`<br> |
| define  | [**MAX\_SPI\_BUF\_LEN**](ns__camera_8c.md#define-max_spi_buf_len)  `64`<br> |
| define  | [**MAX\_SPI\_DMA\_LEN**](ns__camera_8c.md#define-max_spi_dma_len)  `4095`<br> |
| define  | [**NS\_CAMERA\_ARDUCHIP\_TRIG**](ns__camera_8c.md#define-ns_camera_arduchip_trig)  `0x44`<br> |
| define  | [**NS\_CAMERA\_CAP\_DONE\_MASK**](ns__camera_8c.md#define-ns_camera_cap_done_mask)  `0x04`<br> |
| define  | [**NS\_CAMERA\_SHUTTER\_MASK**](ns__camera_8c.md#define-ns_camera_shutter_mask)  `0x02`<br> |
| define  | [**NS\_CAMERA\_VSYNC\_MASK**](ns__camera_8c.md#define-ns_camera_vsync_mask)  `0x01`<br> |
| define  | [**NS\_CAM\_REG\_CAPTURE\_RESOLUTION**](ns__camera_8c.md#define-ns_cam_reg_capture_resolution)  `0X21`<br> |
| define  | [**NS\_CAM\_REG\_FORMAT**](ns__camera_8c.md#define-ns_cam_reg_format)  `0X20`<br> |
| define  | [**NS\_CAM\_SET\_CAPTURE\_MODE**](ns__camera_8c.md#define-ns_cam_set_capture_mode)  `(0 &lt;&lt; 7)`<br> |

## Public Attributes Documentation




### variable camera 

```C++
ArducamCamera camera;
```




<hr>



### variable jpegCtx 

```C++
jpeg_decoder_context_t jpegCtx;
```




<hr>



### variable nsCameraPictureBeingTaken 

```C++
bool nsCameraPictureBeingTaken;
```




<hr>



### variable ns\_camera\_V1\_0\_0 

```C++
const ns_core_api_t ns_camera_V1_0_0;
```




<hr>



### variable ns\_camera\_config 

```C++
ns_camera_config_t ns_camera_config;
```




<hr>



### variable ns\_camera\_current\_version 

```C++
const ns_core_api_t ns_camera_current_version;
```




<hr>



### variable ns\_camera\_oldest\_supported\_version 

```C++
const ns_core_api_t ns_camera_oldest_supported_version;
```




<hr>



### variable ns\_read\_done 

```C++
bool ns_read_done;
```




<hr>



### variable spiHandle 

```C++
ns_spi_config_t* spiHandle;
```




<hr>



### variable timerCfg 

```C++
ns_timer_config_t timerCfg;
```




<hr>
## Public Static Attributes Documentation




### variable dma\_cambuf 

```C++
uint8_t* dma_cambuf;
```




<hr>



### variable dma\_current\_chunk\_length 

```C++
uint32_t dma_current_chunk_length;
```




<hr>



### variable dma\_offset 

```C++
uint32_t dma_offset;
```




<hr>



### variable dma\_total\_requested\_length 

```C++
uint32_t dma_total_requested_length;
```




<hr>
## Public Functions Documentation




### function arducam\_delay\_ms 

```C++
void arducam_delay_ms (
    uint16_t delay
) 
```




<hr>



### function arducam\_delay\_us 

```C++
void arducam_delay_us (
    uint16_t delay
) 
```




<hr>



### function arducam\_spi\_read 

```C++
int arducam_spi_read (
    const void * buf,
    uint32_t bufLen,
    uint64_t reg,
    uint32_t regLen,
    uint32_t csPin
) 
```




<hr>



### function arducam\_spi\_write 

```C++
int arducam_spi_write (
    const void * buf,
    uint32_t bufLen,
    uint64_t reg,
    uint32_t regLen,
    uint32_t csPin
) 
```




<hr>



### function camera\_decode\_image 

_Decode a JPEG image from the camera buffer to RGB565 buffer._ 
```C++
int camera_decode_image (
    uint8_t * camBuf,
    uint32_t camLen,
    uint8_t * imgBuf,
    uint32_t imgWidth,
    uint32_t imgHeight,
    uint32_t scaleFactor
) 
```



Converts a JPG to an RGB565 image.




**Parameters:**


* `camBuf` Camera buffer to decode 
* `camLen` Length of camera buffer 
* `imgBuf` Image buffer to store decoded image 
* `imgWidth` Width of image 
* `imgHeight` Height of image 
* `scaleFactor` Scale factor for image 



**Returns:**

int 





        

<hr>



### function ns\_camera\_adjust\_settings 

_Adjust camera settings._ 
```C++
void ns_camera_adjust_settings (
    int8_t contrast,
    int8_t brightness,
    int8_t ev
) 
```





**Parameters:**


* `contrast` 
* `brightness` 
* `ev` 




        

<hr>



### function ns\_camera\_capture 

_Convenience routine to capture and transfer next frame._ 
```C++
uint32_t ns_camera_capture (
    ns_camera_config_t * cfg,
    uint8_t * camBuf,
    uint32_t bufLen
) 
```





**Parameters:**


* `camBuf` Camera buffer to store frame 
* `bufLen` Buffer size 



**Returns:**

uint32\_t 





        

<hr>



### function ns\_camera\_check\_picture\_completion 

```C++
void ns_camera_check_picture_completion (
    ns_timer_config_t * timer
) 
```




<hr>



### function ns\_camera\_init 

_Initialize the camera._ 
```C++
uint32_t ns_camera_init (
    ns_camera_config_t * cfg
) 
```





**Parameters:**


* `cfg` 



**Returns:**

uint32\_t 





        

<hr>



### function ns\_chop\_off\_trailing\_zeros 

_Chop off trailing zeros from a buffer._ 
```C++
uint32_t ns_chop_off_trailing_zeros (
    uint8_t * buff,
    uint32_t length
) 
```





**Parameters:**


* `buff` JPG buffer 
* `length` Adjusted length after trailing zeros are removed 



**Returns:**

uint32\_t 





        

<hr>



### function ns\_is\_camera\_capturing 

_Check if camera is still capturing This is a helper function, it should typically only be used by ns\_camera._ 
```C++
int ns_is_camera_capturing () 
```





**Returns:**

int 





        

<hr>



### function ns\_press\_shutter\_button 

_Press the shutter button Start the capture process, turns on a timer to poll the camera for completion._ 
```C++
uint32_t ns_press_shutter_button (
    ns_camera_config_t * cfg
) 
```





**Parameters:**


* `cfg` 



**Returns:**

uint32\_t 





        

<hr>



### function ns\_read\_buff 

```C++
uint32_t ns_read_buff (
    ArducamCamera * camera,
    uint8_t * buff,
    uint32_t length
) 
```




<hr>



### function ns\_rgb565\_to\_rgb888 

```C++
void ns_rgb565_to_rgb888 (
    uint16_t rgb565Pixel,
    uint8_t * r,
    uint8_t * g,
    uint8_t * b
) 
```




<hr>



### function ns\_start\_camera 

_Start the camera (take out of low power mode)_ 
```C++
uint32_t ns_start_camera (
    ns_camera_config_t * cfg
) 
```





**Parameters:**


* `cfg` 



**Returns:**

uint32\_t 





        

<hr>



### function ns\_start\_dma\_read 

_Start a DMA read of the camera buffer._ 
```C++
uint32_t ns_start_dma_read (
    ns_camera_config_t * cfg,
    uint8_t * camBuf,
    uint32_t * buffer_offset,
    uint32_t bufLen
) 
```





**Parameters:**


* `cfg` 
* `camBuf` Buffer to store image 
* `buffer_offset` Returned value of buffer offset 
* `bufLen` Length of buffer 



**Returns:**

uint32\_t Total size of image in bytes 





        

<hr>



### function ns\_stop\_camera 

_Stop the camera (put into low power mode)_ 
```C++
uint32_t ns_stop_camera (
    ns_camera_config_t * cfg
) 
```





**Parameters:**


* `cfg` 



**Returns:**

uint32\_t 





        

<hr>



### function ns\_take\_picture 

_Take a picture in mode specified by cfg Once picture is taken, it can be transferred using ns\_transfer\_picture._ 
```C++
uint32_t ns_take_picture (
    ns_camera_config_t * cfg
) 
```





**Parameters:**


* `cfg` 



**Returns:**

uint32\_t 





        

<hr>



### function ns\_transfer\_picture 

_Transfer captured frame over SPI to local buffer NOTE: This routine is blocking and will wait for inflight capture._ 
```C++
uint32_t ns_transfer_picture (
    ns_camera_config_t * cfg,
    uint8_t * camBuf,
    uint32_t * buffer_offset,
    uint32_t bufLen
) 
```





**Parameters:**


* `cfg` 
* `camBuf` Camera buffer to store frame 
* `buffer_offset` JPG images are stored 1 byte offset from start of buffer, so use this to find it 
* `bufLen` Buffer size 



**Returns:**

uint32\_t 





        

<hr>
## Public Static Functions Documentation




### function ns\_camera\_buff\_read\_done 

```C++
static void ns_camera_buff_read_done (
    ns_spi_config_t * cfg
) 
```




<hr>



### function ns\_mapCameraValuesToArducamScale 

```C++
static uint8_t ns_mapCameraValuesToArducamScale (
    int8_t in
) 
```




<hr>
## Macro Definition Documentation





### define ARDU\_BURST\_FIFO\_READ 

```C++
#define ARDU_BURST_FIFO_READ `0x3C`
```




<hr>



### define MAX\_SPI\_BUF\_LEN 

```C++
#define MAX_SPI_BUF_LEN `64`
```




<hr>



### define MAX\_SPI\_DMA\_LEN 

```C++
#define MAX_SPI_DMA_LEN `4095`
```




<hr>



### define NS\_CAMERA\_ARDUCHIP\_TRIG 

```C++
#define NS_CAMERA_ARDUCHIP_TRIG `0x44`
```




<hr>



### define NS\_CAMERA\_CAP\_DONE\_MASK 

```C++
#define NS_CAMERA_CAP_DONE_MASK `0x04`
```




<hr>



### define NS\_CAMERA\_SHUTTER\_MASK 

```C++
#define NS_CAMERA_SHUTTER_MASK `0x02`
```




<hr>



### define NS\_CAMERA\_VSYNC\_MASK 

```C++
#define NS_CAMERA_VSYNC_MASK `0x01`
```




<hr>



### define NS\_CAM\_REG\_CAPTURE\_RESOLUTION 

```C++
#define NS_CAM_REG_CAPTURE_RESOLUTION `0X21`
```




<hr>



### define NS\_CAM\_REG\_FORMAT 

```C++
#define NS_CAM_REG_FORMAT `0X20`
```




<hr>



### define NS\_CAM\_SET\_CAPTURE\_MODE 

```C++
#define NS_CAM_SET_CAPTURE_MODE `(0 << 7)`
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-camera/src/ns_camera.c`

