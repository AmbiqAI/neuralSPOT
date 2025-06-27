

# Struct ArducamCamera



[**ClassList**](annotated.md) **>** [**ArducamCamera**](struct_arducam_camera.md)



_Camera drive interface and information._ 

* `#include <ArducamCamera.h>`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  const struct CameraOperations \* | [**arducamCameraOp**](#variable-arducamcameraop)  <br> |
|  uint8\_t | [**blockSize**](#variable-blocksize)  <br> |
|  uint8\_t | [**burstFirstFlag**](#variable-burstfirstflag)  <br> |
|  [**BUFFER\_CALLBACK**](_arducam_camera_8h.md#typedef-buffer_callback) | [**callBackFunction**](#variable-callbackfunction)  <br> |
|  uint8\_t | [**cameraId**](#variable-cameraid)  <br> |
|  int | [**csPin**](#variable-cspin)  <br> |
|  uint8\_t | [**currentPictureMode**](#variable-currentpicturemode)  <br> |
|  uint8\_t | [**currentPixelFormat**](#variable-currentpixelformat)  <br> |
|  union [**SdkInfo**](union_sdk_info.md) \* | [**currentSDK**](#variable-currentsdk)  <br> |
|  [**STOP\_HANDLE**](_arducam_camera_8h.md#typedef-stop_handle) | [**handle**](#variable-handle)  <br> |
|  struct [**CameraInfo**](struct_camera_info.md) | [**myCameraInfo**](#variable-mycamerainfo)  <br> |
|  uint8\_t | [**previewMode**](#variable-previewmode)  <br> |
|  uint32\_t | [**receivedLength**](#variable-receivedlength)  <br> |
|  uint32\_t | [**totalLength**](#variable-totallength)  <br> |
|  uint8\_t | [**verDateAndNumber**](#variable-verdateandnumber)  <br> |












































## Public Attributes Documentation




### variable arducamCameraOp 

```C++
const struct CameraOperations* ArducamCamera::arducamCameraOp;
```



Camera function interface 


        

<hr>



### variable blockSize 

```C++
uint8_t ArducamCamera::blockSize;
```



The length of the callback function transmission 


        

<hr>



### variable burstFirstFlag 

```C++
uint8_t ArducamCamera::burstFirstFlag;
```



Flag bit for reading data for the first time in burst mode 


        

<hr>



### variable callBackFunction 

```C++
BUFFER_CALLBACK ArducamCamera::callBackFunction;
```



Camera callback function 


        

<hr>



### variable cameraId 

```C++
uint8_t ArducamCamera::cameraId;
```



Model of camera module 


        

<hr>



### variable csPin 

```C++
int ArducamCamera::csPin;
```



CS pin 


        

<hr>



### variable currentPictureMode 

```C++
uint8_t ArducamCamera::currentPictureMode;
```



Currently set resolution 


        

<hr>



### variable currentPixelFormat 

```C++
uint8_t ArducamCamera::currentPixelFormat;
```



The currently set image pixel format 


        

<hr>



### variable currentSDK 

```C++
union SdkInfo* ArducamCamera::currentSDK;
```



Current SDK version 


        

<hr>



### variable handle 

```C++
STOP_HANDLE ArducamCamera::handle;
```




<hr>



### variable myCameraInfo 

```C++
struct CameraInfo ArducamCamera::myCameraInfo;
```



Basic information of the current camera 


        

<hr>



### variable previewMode 

```C++
uint8_t ArducamCamera::previewMode;
```



Stream mode flag 


        

<hr>



### variable receivedLength 

```C++
uint32_t ArducamCamera::receivedLength;
```



The remaining length of the picture 


        

<hr>



### variable totalLength 

```C++
uint32_t ArducamCamera::totalLength;
```



The total length of the picture 


        

<hr>



### variable verDateAndNumber 

```C++
uint8_t ArducamCamera::verDateAndNumber[4];
```



Camera firmware version 


        

<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-camera/includes-api/ArducamCamera.h`

