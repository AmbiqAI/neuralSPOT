

# Struct CameraInfo



[**ClassList**](annotated.md) **>** [**CameraInfo**](struct_camera_info.md)



_Basic information of the camera module._ 

* `#include <ArducamCamera.h>`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  char \* | [**cameraId**](#variable-cameraid)  <br> |
|  unsigned char | [**deviceAddress**](#variable-deviceaddress)  <br> |
|  unsigned long | [**exposureValueMax**](#variable-exposurevaluemax)  <br> |
|  unsigned int | [**exposureValueMin**](#variable-exposurevaluemin)  <br> |
|  unsigned int | [**gainValueMax**](#variable-gainvaluemax)  <br> |
|  unsigned int | [**gainValueMin**](#variable-gainvaluemin)  <br> |
|  unsigned char | [**supportFocus**](#variable-supportfocus)  <br> |
|  int | [**supportResolution**](#variable-supportresolution)  <br> |
|  unsigned char | [**supportSharpness**](#variable-supportsharpness)  <br> |
|  int | [**supportSpecialEffects**](#variable-supportspecialeffects)  <br> |












































## Public Attributes Documentation




### variable cameraId 


```C++
char* CameraInfo::cameraId;
```



Model of camera module 


        



### variable deviceAddress 

```C++
unsigned char CameraInfo::deviceAddress;
```






### variable exposureValueMax 


```C++
unsigned long CameraInfo::exposureValueMax;
```



Maximum exposure time supported by the camera module 


        



### variable exposureValueMin 


```C++
unsigned int CameraInfo::exposureValueMin;
```



Minimum exposure time supported by the camera module 


        



### variable gainValueMax 


```C++
unsigned int CameraInfo::gainValueMax;
```



Maximum gain supported by the camera module 


        



### variable gainValueMin 


```C++
unsigned int CameraInfo::gainValueMin;
```



Minimum gain supported by the camera module 


        



### variable supportFocus 


```C++
unsigned char CameraInfo::supportFocus;
```



Does the camera module support the focus function 


        



### variable supportResolution 


```C++
int CameraInfo::supportResolution;
```



Resolution supported by the camera module 


        



### variable supportSharpness 


```C++
unsigned char CameraInfo::supportSharpness;
```



Does the camera module support the sharpening function 


        



### variable supportSpecialEffects 


```C++
int CameraInfo::supportSpecialEffects;
```



Special effects supported by the camera module 


        

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-camera/includes-api/ArducamCamera.h`

