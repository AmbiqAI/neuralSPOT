

# File ns\_camera.h



[**FileList**](files.md) **>** [**includes-api**](dir_01be6a440053b8b57cd3acfc114f4ae9.md) **>** [**ns\_camera.h**](ns__camera_8h.md)

[Go to the source code of this file](ns__camera_8h_source.md)



* `#include "ns_core.h"`
* `#include "ns_spi.h"`
* `#include "arm_math.h"`
* `#include <stdint.h>`















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**ns\_camera\_cfg**](structns__camera__cfg.md) <br> |


## Public Types

| Type | Name |
| ---: | :--- |
| typedef int8\_t | [**img\_t**](#typedef-img_t)  <br> |
| typedef struct [**ns\_camera\_cfg**](structns__camera__cfg.md) | [**ns\_camera\_config\_t**](#typedef-ns_camera_config_t)  <br> |
| typedef void(\* | [**ns\_camera\_dma\_cb**](#typedef-ns_camera_dma_cb)  <br> |
| enum  | [**ns\_camera\_hw\_e**](#enum-ns_camera_hw_e)  <br> |
| typedef void(\* | [**ns\_camera\_picture\_cb**](#typedef-ns_camera_picture_cb)  <br> |
| enum  | [**ns\_image\_mode\_e**](#enum-ns_image_mode_e)  <br> |
| enum  | [**ns\_image\_pix\_fmt\_e**](#enum-ns_image_pix_fmt_e)  <br> |




## Public Attributes

| Type | Name |
| ---: | :--- |
|  const ns\_core\_api\_t | [**ns\_camera\_V1\_0\_0**](#variable-ns_camera_v1_0_0)  <br> |
|  const ns\_core\_api\_t | [**ns\_camera\_current\_version**](#variable-ns_camera_current_version)  <br> |
|  const ns\_core\_api\_t | [**ns\_camera\_oldest\_supported\_version**](#variable-ns_camera_oldest_supported_version)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**arducam\_delay\_ms**](#function-arducam_delay_ms) (uint16\_t delay) <br> |
|  void | [**arducam\_delay\_us**](#function-arducam_delay_us) (uint16\_t delay) <br> |
|  int | [**arducam\_spi\_read**](#function-arducam_spi_read) (const void \* buf, uint32\_t bufLen, uint64\_t reg, uint32\_t regLen, uint32\_t csPin) <br> |
|  int | [**arducam\_spi\_write**](#function-arducam_spi_write) (const void \* buf, uint32\_t bufLen, uint64\_t reg, uint32\_t regLen, uint32\_t csPin) <br> |
|  int | [**camera\_decode\_image**](#function-camera_decode_image) (uint8\_t \* camBuf, uint32\_t camLen, uint8\_t \* imgBuf, uint32\_t imgWidth, uint32\_t imgHeight, uint32\_t scaleFactor) <br>_Converts a JPG to an RGB565 image._  |
|  void | [**ns\_camera\_adjust\_settings**](#function-ns_camera_adjust_settings) (int8\_t contrast, int8\_t brightness, int8\_t ev) <br>_Adjust camera settings._  |
|  uint32\_t | [**ns\_camera\_init**](#function-ns_camera_init) ([**ns\_camera\_config\_t**](structns__camera__cfg.md) \* cfg) <br>_Initialize the camera._  |
|  uint32\_t | [**ns\_chop\_off\_trailing\_zeros**](#function-ns_chop_off_trailing_zeros) (uint8\_t \* buff, uint32\_t length) <br>_Chop off trailing zeros from a buffer._  |
|  int | [**ns\_is\_camera\_capturing**](#function-ns_is_camera_capturing) () <br>_Check if camera is still capturing This is a helper function, it should typically only be used by ns\_camera._  |
|  uint32\_t | [**ns\_press\_shutter\_button**](#function-ns_press_shutter_button) ([**ns\_camera\_config\_t**](structns__camera__cfg.md) \* cfg) <br>_Press the shutter button Start the capture process, turns on a timer to poll the camera for completion._  |
|  uint32\_t | [**ns\_start\_camera**](#function-ns_start_camera) ([**ns\_camera\_config\_t**](structns__camera__cfg.md) \* cfg) <br>_Start the camera (take out of low power mode)_  |
|  uint32\_t | [**ns\_start\_dma\_read**](#function-ns_start_dma_read) ([**ns\_camera\_config\_t**](structns__camera__cfg.md) \* cfg, uint8\_t \* camBuf, uint32\_t \* buffer\_offset, uint32\_t bufLen) <br>_Start a DMA read of the camera buffer._  |
|  uint32\_t | [**ns\_stop\_camera**](#function-ns_stop_camera) ([**ns\_camera\_config\_t**](structns__camera__cfg.md) \* cfg) <br>_Stop the camera (put into low power mode)_  |
|  uint32\_t | [**ns\_take\_picture**](#function-ns_take_picture) ([**ns\_camera\_config\_t**](structns__camera__cfg.md) \* cfg) <br>_Take a picture in mode specified by cfg Once picture is taken, it can be transferred using ns\_transfer\_picture._  |
|  uint32\_t | [**ns\_transfer\_picture**](#function-ns_transfer_picture) ([**ns\_camera\_config\_t**](structns__camera__cfg.md) \* cfg, uint8\_t \* camBuf, uint32\_t \* buffer\_offset, uint32\_t bufLen) <br>_Transfer captured frame over SPI to local buffer NOTE: This routine is blocking and will wait for inflight capture._  |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**NS\_CAMERA\_API\_ID**](ns__camera_8h.md#define-ns_camera_api_id)  `0xCA000A`<br> |
| define  | [**NS\_CAMERA\_CURRENT\_VERSION**](ns__camera_8h.md#define-ns_camera_current_version)  `NS\_CAMERA\_V1\_0\_0`<br> |
| define  | [**NS\_CAMERA\_OLDEST\_SUPPORTED\_VERSION**](ns__camera_8h.md#define-ns_camera_oldest_supported_version)  `NS\_CAMERA\_V1\_0\_0`<br> |
| define  | [**NS\_CAMERA\_V1\_0\_0**](ns__camera_8h.md#define-ns_camera_v1_0_0)  `{ .major = 1, .minor = 0, .revision = 0 }`<br> |

## Public Types Documentation




### typedef img\_t 

```C++
typedef int8_t img_t;
```




<hr>



### typedef ns\_camera\_config\_t 

```C++
typedef struct ns_camera_cfg ns_camera_config_t;
```




<hr>



### typedef ns\_camera\_dma\_cb 

```C++
typedef void(* ns_camera_dma_cb) (struct ns_camera_cfg *cfg);
```




<hr>



### enum ns\_camera\_hw\_e 

```C++
enum ns_camera_hw_e {
    NS_ARDUCAM = 0
};
```




<hr>



### typedef ns\_camera\_picture\_cb 

```C++
typedef void(* ns_camera_picture_cb) (struct ns_camera_cfg *cfg);
```




<hr>



### enum ns\_image\_mode\_e 

```C++
enum ns_image_mode_e {
    NS_CAM_IMAGE_MODE_QQVGA = 0x00,
    NS_CAM_IMAGE_MODE_QVGA = 0x01,
    NS_CAM_IMAGE_MODE_VGA = 0x02,
    NS_CAM_IMAGE_MODE_SVGA = 0x03,
    NS_CAM_IMAGE_MODE_HD = 0x04,
    NS_CAM_IMAGE_MODE_SXGAM = 0x05,
    NS_CAM_IMAGE_MODE_UXGA = 0x06,
    NS_CAM_IMAGE_MODE_FHD = 0x07,
    NS_CAM_IMAGE_MODE_QXGA = 0x08,
    NS_CAM_IMAGE_MODE_WQXGA2 = 0x09,
    NS_CAM_IMAGE_MODE_96X96 = 0x0a,
    NS_CAM_IMAGE_MODE_128X128 = 0x0b,
    NS_CAM_IMAGE_MODE_320X320 = 0x0c
};
```




<hr>



### enum ns\_image\_pix\_fmt\_e 

```C++
enum ns_image_pix_fmt_e {
    NS_CAM_IMAGE_PIX_FMT_RGB565 = 0x02,
    NS_CAM_IMAGE_PIX_FMT_JPEG = 0x01
};
```




<hr>
## Public Attributes Documentation




### variable ns\_camera\_V1\_0\_0 

```C++
const ns_core_api_t ns_camera_V1_0_0;
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

_Converts a JPG to an RGB565 image._ 
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





**Parameters:**


* `camBuf` Buffer containing JPG image 
* `camLen` Length of JPG image in bytes 
* `imgBuf` Buffer to store RGB565 image 
* `imgWidth` Width of RGB565 image 
* `imgHeight` Height of RGB565 image 
* `scaleFactor` Scale factor to reduce image size 



**Returns:**

int


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
## Macro Definition Documentation





### define NS\_CAMERA\_API\_ID 

```C++
#define NS_CAMERA_API_ID `0xCA000A`
```




<hr>



### define NS\_CAMERA\_CURRENT\_VERSION 

```C++
#define NS_CAMERA_CURRENT_VERSION `NS_CAMERA_V1_0_0`
```




<hr>



### define NS\_CAMERA\_OLDEST\_SUPPORTED\_VERSION 

```C++
#define NS_CAMERA_OLDEST_SUPPORTED_VERSION `NS_CAMERA_V1_0_0`
```




<hr>



### define NS\_CAMERA\_V1\_0\_0 

```C++
#define NS_CAMERA_V1_0_0 `{ .major = 1, .minor = 0, .revision = 0 }`
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-camera/includes-api/ns_camera.h`

