

# File ns\_usb.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-usb**](dir_450d7ce7daa9d29b9b3b5cb7d00f16f9.md) **>** [**src**](dir_f255d6a194767e484966de6b2584c9a6.md) **>** [**ns\_usb.c**](ns__usb_8c.md)

[Go to the source code of this file](ns__usb_8c_source.md)



* `#include "ns_usb.h"`
* `#include "ns_ambiqsuite_harness.h"`
* `#include "ns_core.h"`
* `#include "ns_timer.h"`
* `#include "tusb.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  volatile uint8\_t | [**gGotUSBRx**](#variable-ggotusbrx)   = = 0<br> |
|  ns\_timer\_config\_t | [**g\_ns\_usbTimer**](#variable-g_ns_usbtimer)   = = {
    .api = &ns\_timer\_V1\_0\_0,
    .timer = NS\_TIMER\_USB,
    .enableInterrupt = true,
    .periodInMicroseconds = 1000,
    .callback = ns\_usb\_service\_callback}<br> |
|  const ns\_core\_api\_t | [**ns\_usb\_V0\_0\_1**](#variable-ns_usb_v0_0_1)  <br> |
|  const ns\_core\_api\_t | [**ns\_usb\_V1\_0\_0**](#variable-ns_usb_v1_0_0)  <br> |
|  const ns\_core\_api\_t | [**ns\_usb\_current\_version**](#variable-ns_usb_current_version)  <br> |
|  const ns\_core\_api\_t | [**ns\_usb\_oldest\_supported\_version**](#variable-ns_usb_oldest_supported_version)  <br> |
|  [**ns\_usb\_config\_t**](structns__usb__config__t.md) | [**usb\_config**](#variable-usb_config)   = = {
    .api = &ns\_usb\_V1\_0\_0,
    .deviceType = NS\_USB\_CDC\_DEVICE,
    .rx\_buffer = NULL,
    .rx\_bufferLength = 0,
    .tx\_buffer = NULL,
    .tx\_bufferLength = 0,
    .rx\_cb = NULL,
    .tx\_cb = NULL,
    .service\_cb = NULL,
    .desc\_url = NULL}<br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**ns\_get\_cdc\_rx\_bufferLength**](#function-ns_get_cdc_rx_bufferlength) () <br> |
|  uint32\_t | [**ns\_get\_cdc\_tx\_bufferLength**](#function-ns_get_cdc_tx_bufferlength) () <br> |
|  [**ns\_tusb\_desc\_webusb\_url\_t**](ns__usb_8h.md#typedef-ns_tusb_desc_webusb_url_t) \* | [**ns\_get\_desc\_url**](#function-ns_get_desc_url) () <br> |
|  bool | [**ns\_usb\_data\_available**](#function-ns_usb_data_available) (usb\_handle\_t handle) <br> |
|  uint8\_t \* | [**ns\_usb\_get\_rx\_buffer**](#function-ns_usb_get_rx_buffer) () <br> |
|  uint8\_t \* | [**ns\_usb\_get\_tx\_buffer**](#function-ns_usb_get_tx_buffer) () <br> |
|  void | [**ns\_usb\_handle\_read\_error**](#function-ns_usb_handle_read_error) (usb\_handle\_t h) <br>_Flushes the USB RX fifo after a delay, resets ns\_usb rx state._  |
|  uint32\_t | [**ns\_usb\_init**](#function-ns_usb_init) ([**ns\_usb\_config\_t**](structns__usb__config__t.md) \* cfg, usb\_handle\_t \* h) <br>_Initialize the USB system._  |
|  uint32\_t | [**ns\_usb\_recieve\_data**](#function-ns_usb_recieve_data) (usb\_handle\_t handle, void \* buffer, uint32\_t bufsize) <br>_Blocking USB Receive Data._  |
|  void | [**ns\_usb\_register\_callbacks**](#function-ns_usb_register_callbacks) (usb\_handle\_t handle, ns\_usb\_rx\_cb rxcb, ns\_usb\_tx\_cb txcb) <br>_Register callbacks for USB events, should be called after ns\_usb\_init._  |
|  uint32\_t | [**ns\_usb\_send\_data**](#function-ns_usb_send_data) (usb\_handle\_t handle, void \* buffer, uint32\_t bufsize) <br>_Blocking USB Send Data._  |


## Public Static Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_usb\_service\_callback**](#function-ns_usb_service_callback) (ns\_timer\_config\_t \* c) <br> |


























## Public Attributes Documentation




### variable gGotUSBRx 

```C++
volatile uint8_t gGotUSBRx;
```






### variable g\_ns\_usbTimer 

```C++
ns_timer_config_t g_ns_usbTimer;
```






### variable ns\_usb\_V0\_0\_1 

```C++
const ns_core_api_t ns_usb_V0_0_1;
```






### variable ns\_usb\_V1\_0\_0 

```C++
const ns_core_api_t ns_usb_V1_0_0;
```






### variable ns\_usb\_current\_version 

```C++
const ns_core_api_t ns_usb_current_version;
```






### variable ns\_usb\_oldest\_supported\_version 

```C++
const ns_core_api_t ns_usb_oldest_supported_version;
```






### variable usb\_config 

```C++
ns_usb_config_t usb_config;
```



## Public Functions Documentation




### function ns\_get\_cdc\_rx\_bufferLength 

```C++
uint32_t ns_get_cdc_rx_bufferLength () 
```






### function ns\_get\_cdc\_tx\_bufferLength 

```C++
uint32_t ns_get_cdc_tx_bufferLength () 
```






### function ns\_get\_desc\_url 

```C++
ns_tusb_desc_webusb_url_t * ns_get_desc_url () 
```






### function ns\_usb\_data\_available 

```C++
bool ns_usb_data_available (
    usb_handle_t handle
) 
```






### function ns\_usb\_get\_rx\_buffer 

```C++
uint8_t * ns_usb_get_rx_buffer () 
```






### function ns\_usb\_get\_tx\_buffer 

```C++
uint8_t * ns_usb_get_tx_buffer () 
```






### function ns\_usb\_handle\_read\_error 

_Flushes the USB RX fifo after a delay, resets ns\_usb rx state._ 
```C++
void ns_usb_handle_read_error (
    usb_handle_t h
) 
```





**Parameters:**


* `h` handle 




        



### function ns\_usb\_init 

_Initialize the USB system._ 
```C++
uint32_t ns_usb_init (
    ns_usb_config_t * cfg,
    usb_handle_t * h
) 
```





**Parameters:**


* `cfg` 
* `handle` 



**Returns:**

uint32\_t Status 





        



### function ns\_usb\_recieve\_data 

_Blocking USB Receive Data._ 
```C++
uint32_t ns_usb_recieve_data (
    usb_handle_t handle,
    void * buffer,
    uint32_t bufsize
) 
```





**Parameters:**


* `handle` USB handle 
* `buffer` Pointer to buffer where data will be placed 
* `bufsize` Requested number of bytes 



**Returns:**

uint32\_t 





        



### function ns\_usb\_register\_callbacks 

_Register callbacks for USB events, should be called after ns\_usb\_init._ 
```C++
void ns_usb_register_callbacks (
    usb_handle_t handle,
    ns_usb_rx_cb rxcb,
    ns_usb_tx_cb txcb
) 
```





**Parameters:**


* `handle` 
* `rx_cb` 
* `tx_cb` 




        



### function ns\_usb\_send\_data 

_Blocking USB Send Data._ 
```C++
uint32_t ns_usb_send_data (
    usb_handle_t handle,
    void * buffer,
    uint32_t bufsize
) 
```





**Parameters:**


* `handle` USB handle 
* `buffer` Pointer to buffer with data to be sent 
* `bufsize` Requested number of bytes 



**Returns:**

uint32\_t 





        
## Public Static Functions Documentation




### function ns\_usb\_service\_callback 

```C++
static void ns_usb_service_callback (
    ns_timer_config_t * c
) 
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-usb/src/ns_usb.c`

