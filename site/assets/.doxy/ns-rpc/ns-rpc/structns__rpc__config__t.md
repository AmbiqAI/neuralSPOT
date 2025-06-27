

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
|  ns\_rpc\_transport\_e | [**transport**](#variable-transport)  <br>_Transport type USB or UART._  |
|  uint8\_t \* | [**tx\_buf**](#variable-tx_buf)  <br>_Pointer to allocated buffer which USB CDC will use for tx transfers._  |
|  uint32\_t | [**tx\_bufLength**](#variable-tx_buflength)  <br>_Length of tx buffer._  |
|  ns\_uart\_handle\_t | [**uartHandle**](#variable-uarthandle)  <br>_UART handle._  |












































## Public Attributes Documentation




### variable api 

_API prefix._ 
```C++
const ns_core_api_t* ns_rpc_config_t::api;
```




<hr>



### variable computeOnEVB\_cb 

_Callback for computeOnEVB._ 
```C++
ns_rpc_data_computeOnEVB_cb ns_rpc_config_t::computeOnEVB_cb;
```




<hr>



### variable fetchBlockFromEVB\_cb 

_Callback for fetchBlockFromEVB._ 
```C++
ns_rpc_data_fetchBlockFromEVB_cb ns_rpc_config_t::fetchBlockFromEVB_cb;
```




<hr>



### variable mode 

_Client or Server mode._ 
```C++
rpcGenericDataMode_e ns_rpc_config_t::mode;
```




<hr>



### variable rx\_buf 

_Pointer to allocated buffer which USB CDC will use for rx transfers._ 
```C++
uint8_t* ns_rpc_config_t::rx_buf;
```




<hr>



### variable rx\_bufLength 

_Length of rx buffer._ 
```C++
uint32_t ns_rpc_config_t::rx_bufLength;
```




<hr>



### variable sendBlockToEVB\_cb 

_Callback for sendBlockToEVB._ 
```C++
ns_rpc_data_sendBlockToEVB_cb ns_rpc_config_t::sendBlockToEVB_cb;
```




<hr>



### variable transport 

_Transport type USB or UART._ 
```C++
ns_rpc_transport_e ns_rpc_config_t::transport;
```




<hr>



### variable tx\_buf 

_Pointer to allocated buffer which USB CDC will use for tx transfers._ 
```C++
uint8_t* ns_rpc_config_t::tx_buf;
```




<hr>



### variable tx\_bufLength 

_Length of tx buffer._ 
```C++
uint32_t ns_rpc_config_t::tx_bufLength;
```




<hr>



### variable uartHandle 

_UART handle._ 
```C++
ns_uart_handle_t ns_rpc_config_t::uartHandle;
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-rpc/includes-api/ns_rpc_generic_data.h`

