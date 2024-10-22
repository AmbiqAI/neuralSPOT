

# File ns\_usb\_overrides.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-usb**](dir_450d7ce7daa9d29b9b3b5cb7d00f16f9.md) **>** [**src**](dir_f255d6a194767e484966de6b2584c9a6.md) **>** [**overrides**](dir_9c7a8f6708b262eb1b0dd6b85c35f66f.md) **>** [**ns\_usb\_overrides.c**](ns__usb__overrides_8c.md)

[Go to the source code of this file](ns__usb__overrides_8c_source.md)



* `#include "ns_usb.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  volatile uint8\_t | [**gGotUSBRx**](#variable-ggotusbrx)  <br> |
|  [**ns\_usb\_config\_t**](structns__usb__config__t.md) | [**usb\_config**](#variable-usb_config)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**tud\_cdc\_rx\_cb**](#function-tud_cdc_rx_cb) (uint8\_t itf) <br> |
|  void | [**tud\_cdc\_tx\_complete\_cb**](#function-tud_cdc_tx_complete_cb) (uint8\_t itf) <br> |




























## Public Attributes Documentation




### variable gGotUSBRx 

```C++
volatile uint8_t gGotUSBRx;
```






### variable usb\_config 

```C++
ns_usb_config_t usb_config;
```



## Public Functions Documentation




### function tud\_cdc\_rx\_cb 

```C++
void tud_cdc_rx_cb (
    uint8_t itf
) 
```






### function tud\_cdc\_tx\_complete\_cb 

```C++
void tud_cdc_tx_complete_cb (
    uint8_t itf
) 
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-usb/src/overrides/ns_usb_overrides.c`

