

# File ArducamCamera.h



[**FileList**](files.md) **>** [**includes-api**](dir_01be6a440053b8b57cd3acfc114f4ae9.md) **>** [**ArducamCamera.h**](_arducam_camera_8h.md)

[Go to the source code of this file](_arducam_camera_8h_source.md)

[More...](#detailed-description)

* `#include <stdint.h>`















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**ArducamCamera**](struct_arducam_camera.md) <br>_Camera drive interface and information._  |
| struct | [**CameraInfo**](struct_camera_info.md) <br>_Basic information of the camera module._  |
| struct | [**ClassCamera**](struct_class_camera.md) <br> |
| struct | [**SdkDate**](struct_sdk_date.md) <br>_SDK version update time._  |
| struct | [**SdkInfo**](union_sdk_info.md) <br>_Basic information of the sdk._  |


## Public Types

| Type | Name |
| ---: | :--- |
| enum  | [**@0**](#enum-@0)  <br> |
| typedef uint8\_t(\* | [**BUFFER\_CALLBACK**](#typedef-buffer_callback)  <br> |
| enum  | [**CAM\_BRIGHTNESS\_LEVEL**](#enum-cam_brightness_level)  <br>_Configure camera brightness level._  |
| enum  | [**CAM\_COLOR\_FX**](#enum-cam_color_fx)  <br>_Configure special effects._  |
| enum  | [**CAM\_CONTRAST\_LEVEL**](#enum-cam_contrast_level)  <br>_Configure camera contrast level._  |
| enum  | [**CAM\_EV\_LEVEL**](#enum-cam_ev_level)  <br>_Configure camera EV level._  |
| enum  | [**CAM\_IMAGE\_MODE**](#enum-cam_image_mode)  <br>_Configure camera resolution._  |
| enum  | [**CAM\_IMAGE\_PIX\_FMT**](#enum-cam_image_pix_fmt)  <br>_Configure image pixel format._  |
| enum  | [**CAM\_SHARPNESS\_LEVEL**](#enum-cam_sharpness_level)  <br>_Configure camera Sharpness level._  |
| enum  | [**CAM\_STAURATION\_LEVEL**](#enum-cam_stauration_level)  <br>_Configure camera stauration level._  |
| enum  | [**CAM\_VIDEO\_MODE**](#enum-cam_video_mode)  <br>_Configure resolution in video streaming mode._  |
| enum  | [**CAM\_WHITE\_BALANCE**](#enum-cam_white_balance)  <br>_Configure white balance mode._  |
| enum  | [**CamStatus**](#enum-camstatus)  <br>_Camera status._  |
| enum  | [**CameraStatus**](#enum-camerastatus)  <br> |
| enum  | [**IMAGE\_QUALITY**](#enum-image_quality)  <br> |
| typedef void(\* | [**STOP\_HANDLE**](#typedef-stop_handle)  <br> |




## Public Attributes

| Type | Name |
| ---: | :--- |
|  union [**SdkInfo**](union_sdk_info.md) | [**\_\_attribute\_\_**](#variable-__attribute__)  <br> |
|  uint8\_t | [**day**](#variable-day)  <br> |
|  uint8\_t | [**month**](#variable-month)  <br> |
|  uint16\_t | [**version**](#variable-version)  <br> |
|  uint8\_t | [**year**](#variable-year)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  struct [**SdkDate**](struct_sdk_date.md) | [**\_\_attribute\_\_**](#function-__attribute__) ((packed)) <br> |
|  [**CamStatus**](_arducam_camera_8h.md#enum-camstatus) | [**begin**](#function-begin) ([**ArducamCamera**](struct_arducam_camera.md) \* camera) <br>_Initialize the configuration of the camera module._  |
|  uint8\_t | [**cameraHeartBeat**](#function-cameraheartbeat) ([**ArducamCamera**](struct_arducam_camera.md) \* camera) <br>_Inform the other party of camera status._  |
|  uint32\_t | [**cameraImageAvailable**](#function-cameraimageavailable) ([**ArducamCamera**](struct_arducam_camera.md) \* camera) <br> |
|  uint32\_t | [**cameraReadFifoLength**](#function-camerareadfifolength) ([**ArducamCamera**](struct_arducam_camera.md) \* camera) <br> |
|  uint8\_t | [**cameraReadReg**](#function-camerareadreg) ([**ArducamCamera**](struct_arducam_camera.md) \* camera, uint8\_t addr) <br> |
|  void | [**captureThread**](#function-capturethread) ([**ArducamCamera**](struct_arducam_camera.md) \* camera) <br> |
|  [**ArducamCamera**](struct_arducam_camera.md) | [**createArducamCamera**](#function-createarducamcamera) (int cs) <br>_Create a camera instance._  |
|  void | [**debugWriteRegister**](#function-debugwriteregister) ([**ArducamCamera**](struct_arducam_camera.md) \* camera, uint8\_t \* buff) <br>_Debug mode._  |
|  uint8\_t | [**getAutoFocusSta**](#function-getautofocussta) ([**ArducamCamera**](struct_arducam_camera.md) \* camera) <br>_Get auto focus status._  |
|  void | [**lowPowerOff**](#function-lowpoweroff) ([**ArducamCamera**](struct_arducam_camera.md) \* camera) <br>_Turn off low power mode._  |
|  void | [**lowPowerOn**](#function-lowpoweron) ([**ArducamCamera**](struct_arducam_camera.md) \* camera) <br>_Turn on low power mode._  |
|  uint32\_t | [**readBuff**](#function-readbuff) ([**ArducamCamera**](struct_arducam_camera.md) \* camera, uint8\_t \* buff, uint32\_t length) <br>_Read image data with specified length to buffer._  |
|  uint8\_t | [**readByte**](#function-readbyte) ([**ArducamCamera**](struct_arducam_camera.md) \* camera) <br>_Read a byte from FIFO._  |
|  void | [**registerCallback**](#function-registercallback) ([**ArducamCamera**](struct_arducam_camera.md) \* camera, [**BUFFER\_CALLBACK**](_arducam_camera_8h.md#typedef-buffer_callback) function, uint8\_t blockSize, [**STOP\_HANDLE**](_arducam_camera_8h.md#typedef-stop_handle) handle) <br>_Create callback function._  |
|  [**CamStatus**](_arducam_camera_8h.md#enum-camstatus) | [**reset**](#function-reset) ([**ArducamCamera**](struct_arducam_camera.md) \* camera) <br>_reset cpld and camera_  |
|  [**CamStatus**](_arducam_camera_8h.md#enum-camstatus) | [**setAbsoluteExposure**](#function-setabsoluteexposure) ([**ArducamCamera**](struct_arducam_camera.md) \* camera, uint32\_t val) <br>_Set the exposure time Manually._  |
|  [**CamStatus**](_arducam_camera_8h.md#enum-camstatus) | [**setAutoExposure**](#function-setautoexposure) ([**ArducamCamera**](struct_arducam_camera.md) \* camera, uint8\_t val) <br>_Set the exposure mode._  |
|  [**CamStatus**](_arducam_camera_8h.md#enum-camstatus) | [**setAutoFocus**](#function-setautofocus) ([**ArducamCamera**](struct_arducam_camera.md) \* camera, uint8\_t val) <br>_Set auto focus mode._  |
|  [**CamStatus**](_arducam_camera_8h.md#enum-camstatus) | [**setAutoISOSensitive**](#function-setautoisosensitive) ([**ArducamCamera**](struct_arducam_camera.md) \* camera, uint8\_t val) <br>_Set the gain mode._  |
|  [**CamStatus**](_arducam_camera_8h.md#enum-camstatus) | [**setAutoWhiteBalance**](#function-setautowhitebalance) ([**ArducamCamera**](struct_arducam_camera.md) \* camera, uint8\_t val) <br>_Set white balance mode._  |
|  [**CamStatus**](_arducam_camera_8h.md#enum-camstatus) | [**setAutoWhiteBalanceMode**](#function-setautowhitebalancemode) ([**ArducamCamera**](struct_arducam_camera.md) \* camera, [**CAM\_WHITE\_BALANCE**](_arducam_camera_8h.md#enum-cam_white_balance) mode) <br>_Set the white balance mode Manually._  |
|  [**CamStatus**](_arducam_camera_8h.md#enum-camstatus) | [**setBrightness**](#function-setbrightness) ([**ArducamCamera**](struct_arducam_camera.md) \* camera, [**CAM\_BRIGHTNESS\_LEVEL**](_arducam_camera_8h.md#enum-cam_brightness_level) level) <br>_Set Brightness level._  |
|  [**CamStatus**](_arducam_camera_8h.md#enum-camstatus) | [**setColorEffect**](#function-setcoloreffect) ([**ArducamCamera**](struct_arducam_camera.md) \* camera, [**CAM\_COLOR\_FX**](_arducam_camera_8h.md#enum-cam_color_fx) effect) <br>_Set special effects._  |
|  [**CamStatus**](_arducam_camera_8h.md#enum-camstatus) | [**setContrast**](#function-setcontrast) ([**ArducamCamera**](struct_arducam_camera.md) \* camera, [**CAM\_CONTRAST\_LEVEL**](_arducam_camera_8h.md#enum-cam_contrast_level) level) <br>_Set Contrast level._  |
|  [**CamStatus**](_arducam_camera_8h.md#enum-camstatus) | [**setEV**](#function-setev) ([**ArducamCamera**](struct_arducam_camera.md) \* camera, [**CAM\_EV\_LEVEL**](_arducam_camera_8h.md#enum-cam_ev_level) level) <br>_Set EV level._  |
|  [**CamStatus**](_arducam_camera_8h.md#enum-camstatus) | [**setISOSensitivity**](#function-setisosensitivity) ([**ArducamCamera**](struct_arducam_camera.md) \* camera, int iso\_sense) <br>_Set the exposure time Manually._  |
|  [**CamStatus**](_arducam_camera_8h.md#enum-camstatus) | [**setImageQuality**](#function-setimagequality) ([**ArducamCamera**](struct_arducam_camera.md) \* camera, IMAGE\_QUALITY qualtiy) <br>_Set jpeg image quality._  |
|  [**CamStatus**](_arducam_camera_8h.md#enum-camstatus) | [**setManualFocus**](#function-setmanualfocus) ([**ArducamCamera**](struct_arducam_camera.md) \* camera, uint16\_t val) <br>_Set manual focus mode._  |
|  [**CamStatus**](_arducam_camera_8h.md#enum-camstatus) | [**setSaturation**](#function-setsaturation) ([**ArducamCamera**](struct_arducam_camera.md) \* camera, [**CAM\_STAURATION\_LEVEL**](_arducam_camera_8h.md#enum-cam_stauration_level) level) <br>_Set saturation level._  |
|  [**CamStatus**](_arducam_camera_8h.md#enum-camstatus) | [**setSharpness**](#function-setsharpness) ([**ArducamCamera**](struct_arducam_camera.md) \* camera, [**CAM\_SHARPNESS\_LEVEL**](_arducam_camera_8h.md#enum-cam_sharpness_level) level) <br>_Set Sharpness level._  |
|  [**CamStatus**](_arducam_camera_8h.md#enum-camstatus) | [**startPreview**](#function-startpreview) ([**ArducamCamera**](struct_arducam_camera.md) \* camera, [**CAM\_VIDEO\_MODE**](_arducam_camera_8h.md#enum-cam_video_mode) mode) <br>_Start preview with specified resolution mode._  |
|  [**CamStatus**](_arducam_camera_8h.md#enum-camstatus) | [**stopPreview**](#function-stoppreview) ([**ArducamCamera**](struct_arducam_camera.md) \* camera) <br>_Stop preview._  |
|  [**CamStatus**](_arducam_camera_8h.md#enum-camstatus) | [**takeMultiPictures**](#function-takemultipictures) ([**ArducamCamera**](struct_arducam_camera.md) \* camera, [**CAM\_IMAGE\_MODE**](_arducam_camera_8h.md#enum-cam_image_mode) mode, [**CAM\_IMAGE\_PIX\_FMT**](_arducam_camera_8h.md#enum-cam_image_pix_fmt) pixel\_format, uint8\_t number) <br>_Start multi capture with specified number of image._  |
|  [**CamStatus**](_arducam_camera_8h.md#enum-camstatus) | [**takePicture**](#function-takepicture) ([**ArducamCamera**](struct_arducam_camera.md) \* camera, [**CAM\_IMAGE\_MODE**](_arducam_camera_8h.md#enum-cam_image_mode) mode, [**CAM\_IMAGE\_PIX\_FMT**](_arducam_camera_8h.md#enum-cam_image_pix_fmt) pixel\_format) <br>_Start a snapshot with specified resolution and pixel format._  |




























# Detailed Description




**Author:**

Arducam 




**Date:**

2023/3/6 




**Version:**

V2.0.1 




**Copyright:**

Arducam 





    
## Public Types Documentation




### enum @0 

```C++
enum @0 {
    SENSOR_5MP_1 = 0x81,
    SENSOR_3MP_1 = 0x82,
    SENSOR_5MP_2 = 0x83,
    SENSOR_3MP_2 = 0x84
};
```






### typedef BUFFER\_CALLBACK 


```C++
typedef uint8_t(* BUFFER_CALLBACK) (uint8_t *buffer, uint8_t lenght);
```



Callback function prototype 
 


        



### enum CAM\_BRIGHTNESS\_LEVEL 

```C++
enum CAM_BRIGHTNESS_LEVEL {
    CAM_BRIGHTNESS_LEVEL_MINUS_4 = 8,
    CAM_BRIGHTNESS_LEVEL_MINUS_3 = 6,
    CAM_BRIGHTNESS_LEVEL_MINUS_2 = 4,
    CAM_BRIGHTNESS_LEVEL_MINUS_1 = 2,
    CAM_BRIGHTNESS_LEVEL_DEFAULT = 0,
    CAM_BRIGHTNESS_LEVEL_1 = 1,
    CAM_BRIGHTNESS_LEVEL_2 = 3,
    CAM_BRIGHTNESS_LEVEL_3 = 5,
    CAM_BRIGHTNESS_LEVEL_4 = 7
};
```






### enum CAM\_COLOR\_FX 

```C++
enum CAM_COLOR_FX {
    CAM_COLOR_FX_NONE = 0,
    CAM_COLOR_FX_BLUEISH,
    CAM_COLOR_FX_REDISH,
    CAM_COLOR_FX_BW,
    CAM_COLOR_FX_SEPIA,
    CAM_COLOR_FX_NEGATIVE,
    CAM_COLOR_FX_GRASS_GREEN,
    CAM_COLOR_FX_OVER_EXPOSURE,
    CAM_COLOR_FX_SOLARIZE
};
```






### enum CAM\_CONTRAST\_LEVEL 

```C++
enum CAM_CONTRAST_LEVEL {
    CAM_CONTRAST_LEVEL_MINUS_3 = 6,
    CAM_CONTRAST_LEVEL_MINUS_2 = 4,
    CAM_CONTRAST_LEVEL_MINUS_1 = 2,
    CAM_CONTRAST_LEVEL_DEFAULT = 0,
    CAM_CONTRAST_LEVEL_1 = 1,
    CAM_CONTRAST_LEVEL_2 = 3,
    CAM_CONTRAST_LEVEL_3 = 5
};
```






### enum CAM\_EV\_LEVEL 

```C++
enum CAM_EV_LEVEL {
    CAM_EV_LEVEL_MINUS_3 = 6,
    CAM_EV_LEVEL_MINUS_2 = 4,
    CAM_EV_LEVEL_MINUS_1 = 2,
    CAM_EV_LEVEL_DEFAULT = 0,
    CAM_EV_LEVEL_1 = 1,
    CAM_EV_LEVEL_2 = 3,
    CAM_EV_LEVEL_3 = 5
};
```






### enum CAM\_IMAGE\_MODE 

```C++
enum CAM_IMAGE_MODE {
    CAM_IMAGE_MODE_QQVGA = 0x00,
    CAM_IMAGE_MODE_QVGA = 0x01,
    CAM_IMAGE_MODE_VGA = 0x02,
    CAM_IMAGE_MODE_SVGA = 0x03,
    CAM_IMAGE_MODE_HD = 0x04,
    CAM_IMAGE_MODE_SXGAM = 0x05,
    CAM_IMAGE_MODE_UXGA = 0x06,
    CAM_IMAGE_MODE_FHD = 0x07,
    CAM_IMAGE_MODE_QXGA = 0x08,
    CAM_IMAGE_MODE_WQXGA2 = 0x09,
    CAM_IMAGE_MODE_96X96 = 0x0a,
    CAM_IMAGE_MODE_128X128 = 0x0b,
    CAM_IMAGE_MODE_320X320 = 0x0c
};
```






### enum CAM\_IMAGE\_PIX\_FMT 

```C++
enum CAM_IMAGE_PIX_FMT {
    CAM_IMAGE_PIX_FMT_RGB565 = 0x02,
    CAM_IMAGE_PIX_FMT_JPG = 0x01,
    CAM_IMAGE_PIX_FMT_YUV = 0x03,
    CAM_IMAGE_PIX_FMT_NONE
};
```






### enum CAM\_SHARPNESS\_LEVEL 

```C++
enum CAM_SHARPNESS_LEVEL {
    CAM_SHARPNESS_LEVEL_AUTO = 0,
    CAM_SHARPNESS_LEVEL_1,
    CAM_SHARPNESS_LEVEL_2,
    CAM_SHARPNESS_LEVEL_3,
    CAM_SHARPNESS_LEVEL_4,
    CAM_SHARPNESS_LEVEL_5,
    CAM_SHARPNESS_LEVEL_6,
    CAM_SHARPNESS_LEVEL_7,
    CAM_SHARPNESS_LEVEL_8
};
```






### enum CAM\_STAURATION\_LEVEL 

```C++
enum CAM_STAURATION_LEVEL {
    CAM_STAURATION_LEVEL_MINUS_3 = 6,
    CAM_STAURATION_LEVEL_MINUS_2 = 4,
    CAM_STAURATION_LEVEL_MINUS_1 = 2,
    CAM_STAURATION_LEVEL_DEFAULT = 0,
    CAM_STAURATION_LEVEL_1 = 1,
    CAM_STAURATION_LEVEL_2 = 3,
    CAM_STAURATION_LEVEL_3 = 5
};
```






### enum CAM\_VIDEO\_MODE 

```C++
enum CAM_VIDEO_MODE {
    CAM_VIDEO_MODE_0 = 1,
    CAM_VIDEO_MODE_1 = 2
};
```






### enum CAM\_WHITE\_BALANCE 

```C++
enum CAM_WHITE_BALANCE {
    CAM_WHITE_BALANCE_MODE_DEFAULT = 0,
    CAM_WHITE_BALANCE_MODE_SUNNY,
    CAM_WHITE_BALANCE_MODE_OFFICE,
    CAM_WHITE_BALANCE_MODE_CLOUDY,
    CAM_WHITE_BALANCE_MODE_HOME
};
```






### enum CamStatus 

```C++
enum CamStatus {
    CAM_ERR_SUCCESS = 0,
    CAM_ERR_NO_CALLBACK = -1
};
```






### enum CameraStatus 

```C++
enum CameraStatus {
    Camera_uninit = 0,
    Camera_init,
    Camera_open,
    Camera_close
};
```






### enum IMAGE\_QUALITY 

```C++
enum IMAGE_QUALITY {
    HIGH_QUALITY = 0,
    DEFAULT_QUALITY = 1,
    LOW_QUALITY = 2
};
```






### typedef STOP\_HANDLE 


```C++
typedef void(* STOP_HANDLE) (void);
```



Callback function prototype 
 


        
## Public Attributes Documentation




### variable \_\_attribute\_\_ 

```C++
union SdkInfo __attribute__;
```






### variable day 

```C++
uint8_t day;
```






### variable month 

```C++
uint8_t month;
```






### variable version 

```C++
uint16_t version;
```






### variable year 

```C++
uint8_t year;
```



## Public Functions Documentation




### function \_\_attribute\_\_ 

```C++
struct SdkDate __attribute__ (
    (packed)
) 
```






### function begin 

_Initialize the configuration of the camera module._ 
```C++
CamStatus begin (
    ArducamCamera * camera
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance 



**Returns:**

Return operation status 





        



### function cameraHeartBeat 

_Inform the other party of camera status._ 
```C++
uint8_t cameraHeartBeat (
    ArducamCamera * camera
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance



**Returns:**

Returns camera status 





        



### function cameraImageAvailable 

```C++
uint32_t cameraImageAvailable (
    ArducamCamera * camera
) 
```






### function cameraReadFifoLength 

```C++
uint32_t cameraReadFifoLength (
    ArducamCamera * camera
) 
```






### function cameraReadReg 

```C++
uint8_t cameraReadReg (
    ArducamCamera * camera,
    uint8_t addr
) 
```






### function captureThread 

```C++
void captureThread (
    ArducamCamera * camera
) 
```






### function createArducamCamera 

_Create a camera instance._ 
```C++
ArducamCamera createArducamCamera (
    int cs
) 
```





**Parameters:**


* `cs` Chip select signal for SPI communication



**Returns:**

Return a [**ArducamCamera**](struct_arducam_camera.md) instance 





        



### function debugWriteRegister 

_Debug mode._ 
```C++
void debugWriteRegister (
    ArducamCamera * camera,
    uint8_t * buff
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance 
* `buff` There are four bytes of buff Byte 1 indicates the device address, Byte 2 indicates the high octet of the register, Byte 3 indicates the low octet of the register, and Byte 4 indicates the value written to the register 




        



### function getAutoFocusSta 

_Get auto focus status._ 
```C++
uint8_t getAutoFocusSta (
    ArducamCamera * camera
) 
```





**Returns:**

Return 0x10：focus is finished




**Note:**

Only `5MP` cameras support auto focus control 





        



### function lowPowerOff 

_Turn off low power mode._ 
```C++
void lowPowerOff (
    ArducamCamera * camera
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance 




        



### function lowPowerOn 

_Turn on low power mode._ 
```C++
void lowPowerOn (
    ArducamCamera * camera
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance 




        



### function readBuff 

_Read image data with specified length to buffer._ 
```C++
uint32_t readBuff (
    ArducamCamera * camera,
    uint8_t * buff,
    uint32_t length
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance 
* `buff` Buffer for storing camera data 
* `length` The length of the available data to be read



**Returns:**

Returns the length actually read




**Note:**

Transmission length should be less than `255` 





        



### function readByte 

_Read a byte from FIFO._ 
```C++
uint8_t readByte (
    ArducamCamera * camera
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance



**Returns:**

Returns Camera data




**Note:**

Before calling this function, make sure that the data is available in the buffer 





        



### function registerCallback 

_Create callback function._ 
```C++
void registerCallback (
    ArducamCamera * camera,
    BUFFER_CALLBACK function,
    uint8_t blockSize,
    STOP_HANDLE handle
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance 
* `function` Callback function name 
* `blockSize` The length of the data transmitted by the callback function at one time 
* `handle` stop function Callback function name



**Note:**

Transmission length should be less than `255` 





        



### function reset 

_reset cpld and camera_ 
```C++
CamStatus reset (
    ArducamCamera * camera
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance 



**Returns:**

Return operation status 





        



### function setAbsoluteExposure 

_Set the exposure time Manually._ 
```C++
CamStatus setAbsoluteExposure (
    ArducamCamera * camera,
    uint32_t val
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance 
* `val` Value of exposure line



**Returns:**

Return operation status




**Note:**

Before calling this function, you need to use the [**setAutoExposure()**](_arducam_camera_8h.md#function-setautoexposure) function to turn off the auto exposure function 





        



### function setAutoExposure 

_Set the exposure mode._ 
```C++
CamStatus setAutoExposure (
    ArducamCamera * camera,
    uint8_t val
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance 
* `val` `1` Turn on automatic exposure `0` Turn off automatic exposure



**Returns:**

Return operation status 





        



### function setAutoFocus 

_Set auto focus mode._ 
```C++
CamStatus setAutoFocus (
    ArducamCamera * camera,
    uint8_t val
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance 
* `val` mode of autofocus



**Returns:**

Return operation status




**Note:**

Only `5MP` cameras support auto focus control 





        



### function setAutoISOSensitive 

_Set the gain mode._ 
```C++
CamStatus setAutoISOSensitive (
    ArducamCamera * camera,
    uint8_t val
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance 
* `val` `1` turn on automatic gain `0` turn off automatic gain



**Returns:**

Return operation status 





        



### function setAutoWhiteBalance 

_Set white balance mode._ 
```C++
CamStatus setAutoWhiteBalance (
    ArducamCamera * camera,
    uint8_t val
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance 
* `val` `1` turn on automatic white balance `0` turn off automatic white balance



**Returns:**

Return operation status 





        



### function setAutoWhiteBalanceMode 

_Set the white balance mode Manually._ 
```C++
CamStatus setAutoWhiteBalanceMode (
    ArducamCamera * camera,
    CAM_WHITE_BALANCE mode
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance 
* `mode` White balance mode



**Returns:**

Return operation status 





        



### function setBrightness 

_Set Brightness level._ 
```C++
CamStatus setBrightness (
    ArducamCamera * camera,
    CAM_BRIGHTNESS_LEVEL level
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance 
* `level` Brightness level



**Returns:**

Return operation status 





        



### function setColorEffect 

_Set special effects._ 
```C++
CamStatus setColorEffect (
    ArducamCamera * camera,
    CAM_COLOR_FX effect
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance 
* `effect` Special effects mode



**Returns:**

Return operation status 





        



### function setContrast 

_Set Contrast level._ 
```C++
CamStatus setContrast (
    ArducamCamera * camera,
    CAM_CONTRAST_LEVEL level
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance 
* `level` Contrast level



**Returns:**

Return operation status 





        



### function setEV 

_Set EV level._ 
```C++
CamStatus setEV (
    ArducamCamera * camera,
    CAM_EV_LEVEL level
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance 
* `level` EV level



**Returns:**

Return operation status 





        



### function setISOSensitivity 

_Set the exposure time Manually._ 
```C++
CamStatus setISOSensitivity (
    ArducamCamera * camera,
    int iso_sense
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance 
* `iso_sense` Value of gain



**Returns:**

Return operation status




**Note:**

Before calling this function, you need to use the [**setAutoISOSensitive()**](_arducam_camera_8h.md#function-setautoisosensitive) function to turn off the auto gain function 





        



### function setImageQuality 

_Set jpeg image quality._ 
```C++
CamStatus setImageQuality (
    ArducamCamera * camera,
    IMAGE_QUALITY qualtiy
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance 
* `qualtiy` Image Quality



**Returns:**

Return operation status




**Note:**

Only `3MP` cameras support sharpness control 





        



### function setManualFocus 

_Set manual focus mode._ 
```C++
CamStatus setManualFocus (
    ArducamCamera * camera,
    uint16_t val
) 
```





**Parameters:**


* `value` of VCM code



**Returns:**

Return operation status




**Note:**

Only `5MP` cameras support maunal focus control 





        



### function setSaturation 

_Set saturation level._ 
```C++
CamStatus setSaturation (
    ArducamCamera * camera,
    CAM_STAURATION_LEVEL level
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance 
* `level` Saturation level



**Returns:**

Return operation status 





        



### function setSharpness 

_Set Sharpness level._ 
```C++
CamStatus setSharpness (
    ArducamCamera * camera,
    CAM_SHARPNESS_LEVEL level
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance 
* `level` Sharpness level



**Returns:**

Return operation status




**Note:**

Only `3MP` cameras support sharpness control 





        



### function startPreview 

_Start preview with specified resolution mode._ 
```C++
CamStatus startPreview (
    ArducamCamera * camera,
    CAM_VIDEO_MODE mode
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance 
* `mode` Resolution of the camera module



**Returns:**

Return operation status




**Note:**

Before calling this function, you need to register the callback function.The default image pixel format is JPEG 





        



### function stopPreview 

_Stop preview._ 
```C++
CamStatus stopPreview (
    ArducamCamera * camera
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance



**Returns:**

Return operation status 





        



### function takeMultiPictures 

_Start multi capture with specified number of image._ 
```C++
CamStatus takeMultiPictures (
    ArducamCamera * camera,
    CAM_IMAGE_MODE mode,
    CAM_IMAGE_PIX_FMT pixel_format,
    uint8_t number
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance 
* `mode` Resolution of the camera module 
* `pixel_format` Output image pixel format,which supports JPEG, RGB, YUV 
* `number` Number of pictures taken



**Returns:**

Return operation status




**Note:**

The mode parameter must be the resolution which the current camera supported 





        



### function takePicture 

_Start a snapshot with specified resolution and pixel format._ 
```C++
CamStatus takePicture (
    ArducamCamera * camera,
    CAM_IMAGE_MODE mode,
    CAM_IMAGE_PIX_FMT pixel_format
) 
```





**Parameters:**


* `camera` [**ArducamCamera**](struct_arducam_camera.md) instance 
* `mode` Resolution of the camera module 
* `pixel_format` Output image pixel format,which supports JPEG, RGB, YUV



**Returns:**

Return operation status




**Note:**

The mode parameter must be the resolution wh 





        

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-camera/includes-api/ArducamCamera.h`

