

# File ns\_rpc\_generic\_data.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-rpc**](dir_e7349e64d3511848a99e2d9321b8e9b2.md) **>** [**src**](dir_cd98b33a533efa56f36cb87393b63939.md) **>** [**ns\_rpc\_generic\_data.c**](ns__rpc__generic__data_8c.md)

[Go to the source code of this file](ns__rpc__generic__data_8c_source.md)



* `#include "ns_rpc_generic_data.h"`
* `#include "erpc_client_setup.h"`
* `#include "erpc_server_setup.h"`
* `#include "ns_usb.h"`
* `#include "GenericDataOperations_EvbToPc.h"`
* `#include "GenericDataOperations_PcToEvb.h"`
* `#include "GenericDataOperations_PcToEvb_server.h"`
* `#include "ns_ambiqsuite_harness.h"`
* `#include "ns_core.h"`
* `#include "ns_malloc.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  [**ns\_rpc\_config\_t**](structns__rpc__config__t.md) | [**g\_RpcGenericDataConfig**](#variable-g_rpcgenericdataconfig)   = = {.api = &ns\_rpc\_gdo\_current\_version,
                                          .mode = NS\_RPC\_GENERICDATA\_CLIENT,
                                          .rx\_buf = NULL,
                                          .rx\_bufLength = 0,
                                          .tx\_buf = NULL,
                                          .tx\_bufLength = 0,
                                          .usbHandle = NULL,
                                          .sendBlockToEVB\_cb = NULL,
                                          .fetchBlockFromEVB\_cb = NULL,
                                          .computeOnEVB\_cb = NULL}<br> |
|  ns\_usb\_config\_t | [**g\_RpcGenericUSBHandle**](#variable-g_rpcgenericusbhandle)   = = {.api = &ns\_usb\_V1\_0\_0,
                                         .deviceType = NS\_USB\_CDC\_DEVICE,
                                         .rx\_buffer = NULL,
                                         .rx\_bufferLength = 0,
                                         .tx\_buffer = NULL,
                                         .tx\_bufferLength = 0,
                                         .rx\_cb = NULL,
                                         .tx\_cb = NULL,
                                         .service\_cb = NULL}<br> |
|  const ns\_core\_api\_t | [**ns\_rpc\_gdo\_V0\_0\_1**](#variable-ns_rpc_gdo_v0_0_1)  <br> |
|  const ns\_core\_api\_t | [**ns\_rpc\_gdo\_V1\_0\_0**](#variable-ns_rpc_gdo_v1_0_0)  <br> |
|  const ns\_core\_api\_t | [**ns\_rpc\_gdo\_current\_version**](#variable-ns_rpc_gdo_current_version)  <br> |
|  const ns\_core\_api\_t | [**ns\_rpc\_gdo\_oldest\_supported\_version**](#variable-ns_rpc_gdo_oldest_supported_version)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_rpc\_data\_clientDoneWithBlockFromPC**](#function-ns_rpc_data_clientdonewithblockfrompc) (const [**dataBlock**](structdata_block.md) \* block) <br>_Helper function to be called after processing the block from ns\_rpc\_data\_fetchBlockFromPC or result block from ns\_rpc\_data\_computeOnPC. This will free() the description and buffer.data block struct members, so only do it after you're done with those!_  |
|  status | [**ns\_rpc\_data\_computeOnEVB**](#function-ns_rpc_data_computeonevb) (const [**dataBlock**](structdata_block.md) \* in\_block, [**dataBlock**](structdata_block.md) \* result\_block) <br> |
|  status | [**ns\_rpc\_data\_fetchBlockFromEVB**](#function-ns_rpc_data_fetchblockfromevb) ([**dataBlock**](structdata_block.md) \* block) <br> |
|  status | [**ns\_rpc\_data\_sendBlockToEVB**](#function-ns_rpc_data_sendblocktoevb) (const [**dataBlock**](structdata_block.md) \* block) <br> |
|  uint16\_t | [**ns\_rpc\_genericDataOperationsClient\_reset**](#function-ns_rpc_genericdataoperationsclient_reset) ([**ns\_rpc\_config\_t**](structns__rpc__config__t.md) \* cfg) <br> |
|  uint16\_t | [**ns\_rpc\_genericDataOperations\_init**](#function-ns_rpc_genericdataoperations_init) ([**ns\_rpc\_config\_t**](structns__rpc__config__t.md) \* cfg) <br>_Initialize the GenericDataOperations RPC system._  |
|  void | [**ns\_rpc\_genericDataOperations\_pollServer**](#function-ns_rpc_genericdataoperations_pollserver) ([**ns\_rpc\_config\_t**](structns__rpc__config__t.md) \* cfg) <br>_Polls the server for incoming RPC calls. Must be called to handle incoming RPC calls (for example, when a USB or timer event occurs)_  |
|  void | [**ns\_rpc\_genericDataOperations\_printDatablock**](#function-ns_rpc_genericdataoperations_printdatablock) (const [**dataBlock**](structdata_block.md) \* block) <br>_Helper function for printing a block's contents._  |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**NS\_RPC\_GENERIC\_DATA**](ns__rpc__generic__data_8c.md#define-ns_rpc_generic_data)  <br> |

## Public Attributes Documentation




### variable g\_RpcGenericDataConfig 

```C++
ns_rpc_config_t g_RpcGenericDataConfig;
```






### variable g\_RpcGenericUSBHandle 

```C++
ns_usb_config_t g_RpcGenericUSBHandle;
```






### variable ns\_rpc\_gdo\_V0\_0\_1 

```C++
const ns_core_api_t ns_rpc_gdo_V0_0_1;
```






### variable ns\_rpc\_gdo\_V1\_0\_0 

```C++
const ns_core_api_t ns_rpc_gdo_V1_0_0;
```






### variable ns\_rpc\_gdo\_current\_version 

```C++
const ns_core_api_t ns_rpc_gdo_current_version;
```






### variable ns\_rpc\_gdo\_oldest\_supported\_version 

```C++
const ns_core_api_t ns_rpc_gdo_oldest_supported_version;
```



## Public Functions Documentation




### function ns\_rpc\_data\_clientDoneWithBlockFromPC 

_Helper function to be called after processing the block from ns\_rpc\_data\_fetchBlockFromPC or result block from ns\_rpc\_data\_computeOnPC. This will free() the description and buffer.data block struct members, so only do it after you're done with those!_ 
```C++
void ns_rpc_data_clientDoneWithBlockFromPC (
    const dataBlock * block
) 
```





**Parameters:**


* `block` Block to be freed 




        



### function ns\_rpc\_data\_computeOnEVB 

```C++
status ns_rpc_data_computeOnEVB (
    const dataBlock * in_block,
    dataBlock * result_block
) 
```






### function ns\_rpc\_data\_fetchBlockFromEVB 

```C++
status ns_rpc_data_fetchBlockFromEVB (
    dataBlock * block
) 
```






### function ns\_rpc\_data\_sendBlockToEVB 

```C++
status ns_rpc_data_sendBlockToEVB (
    const dataBlock * block
) 
```






### function ns\_rpc\_genericDataOperationsClient\_reset 

```C++
uint16_t ns_rpc_genericDataOperationsClient_reset (
    ns_rpc_config_t * cfg
) 
```






### function ns\_rpc\_genericDataOperations\_init 

_Initialize the GenericDataOperations RPC system._ 
```C++
uint16_t ns_rpc_genericDataOperations_init (
    ns_rpc_config_t * cfg
) 
```





**Parameters:**


* `cfg` 



**Returns:**

uint16\_t Status 





        



### function ns\_rpc\_genericDataOperations\_pollServer 

_Polls the server for incoming RPC calls. Must be called to handle incoming RPC calls (for example, when a USB or timer event occurs)_ 
```C++
void ns_rpc_genericDataOperations_pollServer (
    ns_rpc_config_t * cfg
) 
```





**Parameters:**


* `cfg` 




        



### function ns\_rpc\_genericDataOperations\_printDatablock 

_Helper function for printing a block's contents._ 
```C++
void ns_rpc_genericDataOperations_printDatablock (
    const dataBlock * block
) 
```





**Parameters:**


* `block` to be printed 




        
## Macro Definition Documentation





### define NS\_RPC\_GENERIC\_DATA 

```C++
#define NS_RPC_GENERIC_DATA 
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-rpc/src/ns_rpc_generic_data.c`

