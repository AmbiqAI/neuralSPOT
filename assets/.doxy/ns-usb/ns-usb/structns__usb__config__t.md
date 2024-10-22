

# Struct ns\_usb\_config\_t



[**ClassList**](annotated.md) **>** [**ns\_usb\_config\_t**](structns__usb__config__t.md)



_USB Configuration Struct._ 

* `#include <ns_usb.h>`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  const ns\_core\_api\_t \* | [**api**](#variable-api)  <br>_API prefix._  |
|  [**ns\_tusb\_desc\_webusb\_url\_t**](ns__usb_8h.md#typedef-ns_tusb_desc_webusb_url_t) \* | [**desc\_url**](#variable-desc_url)  <br>_WebUSB URL descriptor._  |
|  [**ns\_usb\_device\_type\_e**](ns__usb_8h.md#enum-ns_usb_device_type_e) | [**deviceType**](#variable-devicetype)  <br>_Device type, only CDC is currently supported._  |
|  void \* | [**rx\_buffer**](#variable-rx_buffer)  <br>_Pointer to allocated buffer which USB CDC will use for rx transfers._  |
|  uint16\_t | [**rx\_bufferLength**](#variable-rx_bufferlength)  <br>_Length of rx buffer._  |
|  ns\_usb\_rx\_cb | [**rx\_cb**](#variable-rx_cb)  <br>_Callback for rx events._  |
|  ns\_usb\_service\_cb | [**service\_cb**](#variable-service_cb)  <br>_Callback for service events._  |
|  void \* | [**tx\_buffer**](#variable-tx_buffer)  <br>_Pointer to allocated buffer which USB CDC will use for tx transfers._  |
|  uint16\_t | [**tx\_bufferLength**](#variable-tx_bufferlength)  <br>_Length of tx buffer._  |
|  ns\_usb\_tx\_cb | [**tx\_cb**](#variable-tx_cb)  <br>_Callback for tx events._  |












































## Public Attributes Documentation




### variable api 

```C++
const ns_core_api_t* ns_usb_config_t::api;
```






### variable desc\_url 

```C++
ns_tusb_desc_webusb_url_t* ns_usb_config_t::desc_url;
```






### variable deviceType 

```C++
ns_usb_device_type_e ns_usb_config_t::deviceType;
```






### variable rx\_buffer 

```C++
void* ns_usb_config_t::rx_buffer;
```






### variable rx\_bufferLength 

```C++
uint16_t ns_usb_config_t::rx_bufferLength;
```






### variable rx\_cb 

```C++
ns_usb_rx_cb ns_usb_config_t::rx_cb;
```






### variable service\_cb 

```C++
ns_usb_service_cb ns_usb_config_t::service_cb;
```






### variable tx\_buffer 

```C++
void* ns_usb_config_t::tx_buffer;
```






### variable tx\_bufferLength 

```C++
uint16_t ns_usb_config_t::tx_bufferLength;
```






### variable tx\_cb 

```C++
ns_usb_tx_cb ns_usb_config_t::tx_cb;
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-usb/includes-api/ns_usb.h`

