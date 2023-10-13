

# Struct ns\_rpc\_config\_t



[**ClassList**](annotated.md) **>** [**ns\_rpc\_config\_t**](structns__rpc__config__t.md)



_RPC Configuration Struct._ 

* `#include <ns_rpc_generic_data.h>`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  const ns\_core\_api\_t \* | [**api**](#variable-api)  <br>_API prefix._  |
|  ns\_rpc\_data\_computeOnEVB\_cb | [**computeOnEVB\_cb**](#variable-computeonevb_cb)  <br>_Callback for computeOnEVB._  |
|  ns\_rpc\_data\_fetchBlockFromEVB\_cb | [**fetchBlockFromEVB\_cb**](#variable-fetchblockfromevb_cb)  <br>_Callback for fetchBlockFromEVB._  |
|  rpcGenericDataMode\_e | [**mode**](#variable-mode)  <br>_Client or Server mode._  |
|  uint8\_t \* | [**rx\_buf**](#variable-rx_buf)  <br>_Pointer to allocated buffer which USB CDC will use for rx transfers._  |
|  uint32\_t | [**rx\_bufLength**](#variable-rx_buflength)  <br>_Length of rx buffer._  |
|  ns\_rpc\_data\_sendBlockToEVB\_cb | [**sendBlockToEVB\_cb**](#variable-sendblocktoevb_cb)  <br>_Callback for sendBlockToEVB._  |
|  uint8\_t \* | [**tx\_buf**](#variable-tx_buf)  <br>_Pointer to allocated buffer which USB CDC will use for tx transfers._  |
|  uint32\_t | [**tx\_bufLength**](#variable-tx_buflength)  <br>_Length of tx buffer._  |
|  usb\_handle\_t | [**usbHandle**](#variable-usbhandle)  <br>_USB handle._  |












































## Public Attributes Documentation




### variable api 

```C++
const ns_core_api_t* ns_rpc_config_t::api;
```






### variable computeOnEVB\_cb 

```C++
ns_rpc_data_computeOnEVB_cb ns_rpc_config_t::computeOnEVB_cb;
```






### variable fetchBlockFromEVB\_cb 

```C++
ns_rpc_data_fetchBlockFromEVB_cb ns_rpc_config_t::fetchBlockFromEVB_cb;
```






### variable mode 

```C++
rpcGenericDataMode_e ns_rpc_config_t::mode;
```






### variable rx\_buf 

```C++
uint8_t* ns_rpc_config_t::rx_buf;
```






### variable rx\_bufLength 

```C++
uint32_t ns_rpc_config_t::rx_bufLength;
```






### variable sendBlockToEVB\_cb 

```C++
ns_rpc_data_sendBlockToEVB_cb ns_rpc_config_t::sendBlockToEVB_cb;
```






### variable tx\_buf 

```C++
uint8_t* ns_rpc_config_t::tx_buf;
```






### variable tx\_bufLength 

```C++
uint32_t ns_rpc_config_t::tx_bufLength;
```






### variable usbHandle 

```C++
usb_handle_t ns_rpc_config_t::usbHandle;
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-rpc/includes-api/ns_rpc_generic_data.h`

