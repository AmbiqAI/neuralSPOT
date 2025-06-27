

# Struct ns\_camera\_cfg



[**ClassList**](annotated.md) **>** [**ns\_camera\_cfg**](structns__camera__cfg.md)


























## Public Attributes

| Type | Name |
| ---: | :--- |
|  const ns\_core\_api\_t \* | [**api**](#variable-api)  <br>_API prefix._  |
|  ns\_camera\_hw\_e | [**cameraHw**](#variable-camerahw)  <br> |
|  uint32\_t | [**dmaBufferLength**](#variable-dmabufferlength)  <br> |
|  uint32\_t | [**dmaBufferOffset**](#variable-dmabufferoffset)  <br> |
|  ns\_camera\_dma\_cb | [**dmaCompleteCb**](#variable-dmacompletecb)  <br>_If using DMA, this will be called when DMA chunk is complete._  |
|  ns\_image\_mode\_e | [**imageMode**](#variable-imagemode)  <br> |
|  ns\_image\_pix\_fmt\_e | [**imagePixFmt**](#variable-imagepixfmt)  <br> |
|  int8\_t | [**iom**](#variable-iom)  <br>_Apollo4 IOM port._  |
|  ns\_camera\_picture\_cb | [**pictureTakenCb**](#variable-picturetakencb)  <br>_If using polling timer, this will be called when pic is ready to xfer._  |
|  ns\_spi\_config\_t | [**spiConfig**](#variable-spiconfig)  <br> |
|  uint32\_t | [**spiSpeed**](#variable-spispeed)  <br>_= CAM\_SPI\_SPEED;_  |












































## Public Attributes Documentation




### variable api 

_API prefix._ 
```C++
const ns_core_api_t* ns_camera_cfg::api;
```




<hr>



### variable cameraHw 

```C++
ns_camera_hw_e ns_camera_cfg::cameraHw;
```




<hr>



### variable dmaBufferLength 

```C++
uint32_t ns_camera_cfg::dmaBufferLength;
```




<hr>



### variable dmaBufferOffset 

```C++
uint32_t ns_camera_cfg::dmaBufferOffset;
```




<hr>



### variable dmaCompleteCb 

_If using DMA, this will be called when DMA chunk is complete._ 
```C++
ns_camera_dma_cb ns_camera_cfg::dmaCompleteCb;
```




<hr>



### variable imageMode 

```C++
ns_image_mode_e ns_camera_cfg::imageMode;
```




<hr>



### variable imagePixFmt 

```C++
ns_image_pix_fmt_e ns_camera_cfg::imagePixFmt;
```




<hr>



### variable iom 

_Apollo4 IOM port._ 
```C++
int8_t ns_camera_cfg::iom;
```




<hr>



### variable pictureTakenCb 

_If using polling timer, this will be called when pic is ready to xfer._ 
```C++
ns_camera_picture_cb ns_camera_cfg::pictureTakenCb;
```




<hr>



### variable spiConfig 

```C++
ns_spi_config_t ns_camera_cfg::spiConfig;
```




<hr>



### variable spiSpeed 

_= CAM\_SPI\_SPEED;_ 
```C++
uint32_t ns_camera_cfg::spiSpeed;
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-camera/includes-api/ns_camera.h`

