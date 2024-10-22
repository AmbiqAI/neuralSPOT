

# Group ns-rpc-generic-data



[**Modules**](modules.md) **>** [**ns-rpc-generic-data**](group__ns-rpc-generic-data.md)




















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**ns\_rpc\_config\_t**](structns__rpc__config__t.md) <br>_RPC Configuration Struct._  |


## Public Types

| Type | Name |
| ---: | :--- |
| typedef status(\* | [**ns\_rpc\_data\_computeOnEVB\_cb**](#typedef-ns_rpc_data_computeonevb_cb)  <br> |
| typedef status(\* | [**ns\_rpc\_data\_fetchBlockFromEVB\_cb**](#typedef-ns_rpc_data_fetchblockfromevb_cb)  <br> |
| typedef status(\* | [**ns\_rpc\_data\_sendBlockToEVB\_cb**](#typedef-ns_rpc_data_sendblocktoevb_cb)  <br> |
| enum  | [**rpcGenericDataMode\_e**](#enum-rpcgenericdatamode_e)  <br> |




## Public Attributes

| Type | Name |
| ---: | :--- |
|  const ns\_core\_api\_t | [**ns\_rpc\_gdo\_V0\_0\_1**](#variable-ns_rpc_gdo_v0_0_1)  <br> |
|  const ns\_core\_api\_t | [**ns\_rpc\_gdo\_V1\_0\_0**](#variable-ns_rpc_gdo_v1_0_0)  <br> |
|  const ns\_core\_api\_t | [**ns\_rpc\_gdo\_current\_version**](#variable-ns_rpc_gdo_current_version)  <br> |
|  const ns\_core\_api\_t | [**ns\_rpc\_gdo\_oldest\_supported\_version**](#variable-ns_rpc_gdo_oldest_supported_version)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_rpc\_data\_clientDoneWithBlockFromPC**](#function-ns_rpc_data_clientdonewithblockfrompc) (const [**dataBlock**](structdata_block.md) \* block) <br>_Helper function to be called after processing the block from ns\_rpc\_data\_fetchBlockFromPC or result block from ns\_rpc\_data\_computeOnPC. This will free() the description and buffer.data block struct members, so only do it after you're done with those!_  |
|  uint16\_t | [**ns\_rpc\_genericDataOperationsClient\_reset**](#function-ns_rpc_genericdataoperationsclient_reset) ([**ns\_rpc\_config\_t**](structns__rpc__config__t.md) \* cfg) <br> |
|  void | [**ns\_rpc\_genericDataOperations\_disableServerPoll**](#function-ns_rpc_genericdataoperations_disableserverpoll) ([**ns\_rpc\_config\_t**](structns__rpc__config__t.md) \* cfg) <br>_Disable RPC server and stop receiving RPC calls._  |
|  void | [**ns\_rpc\_genericDataOperations\_enableServerPoll**](#function-ns_rpc_genericdataoperations_enableserverpoll) ([**ns\_rpc\_config\_t**](structns__rpc__config__t.md) \* cfg) <br>_Enable RPC server and prepare to receive RPC calls._  |
|  uint16\_t | [**ns\_rpc\_genericDataOperations\_init**](#function-ns_rpc_genericdataoperations_init) ([**ns\_rpc\_config\_t**](structns__rpc__config__t.md) \* cfg) <br>_Initialize the GenericDataOperations RPC system._  |
|  void | [**ns\_rpc\_genericDataOperations\_pollServer**](#function-ns_rpc_genericdataoperations_pollserver) ([**ns\_rpc\_config\_t**](structns__rpc__config__t.md) \* cfg) <br>_Polls the server for incoming RPC calls. Must be called to handle incoming RPC calls (for example, when a USB or timer event occurs)_  |
|  void | [**ns\_rpc\_genericDataOperations\_printDatablock**](#function-ns_rpc_genericdataoperations_printdatablock) (const [**dataBlock**](structdata_block.md) \* block) <br>_Helper function for printing a block's contents._  |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**NS\_RPC\_GDO\_API\_ID**](ns__rpc__generic__data_8h.md#define-ns_rpc_gdo_api_id)  0xCA0100<br> |
| define  | [**NS\_RPC\_GDO\_CURRENT\_VERSION**](ns__rpc__generic__data_8h.md#define-ns_rpc_gdo_current_version)  NS\_RPC\_GDO\_V1\_0\_0<br> |
| define  | [**NS\_RPC\_GDO\_OLDEST\_SUPPORTED\_VERSION**](ns__rpc__generic__data_8h.md#define-ns_rpc_gdo_oldest_supported_version)  NS\_RPC\_GDO\_V0\_0\_1<br> |
| define  | [**NS\_RPC\_GDO\_V0\_0\_1**](ns__rpc__generic__data_8h.md#define-ns_rpc_gdo_v0_0_1)          { .major = 0, .minor = 0, .revision = 1 }<br> |
| define  | [**NS\_RPC\_GDO\_V1\_0\_0**](ns__rpc__generic__data_8h.md#define-ns_rpc_gdo_v1_0_0)          { .major = 1, .minor = 0, .revision = 0 }<br> |
| define  | [**NS\_RPC\_MALLOC\_SIZE\_IN\_K**](ns__rpc__generic__data_8h.md#define-ns_rpc_malloc_size_in_k)  8<br> |

## Public Types Documentation




### typedef ns\_rpc\_data\_computeOnEVB\_cb 

```C++
typedef status(* ns_rpc_data_computeOnEVB_cb) (const dataBlock *in_block, dataBlock *result_block);
```






### typedef ns\_rpc\_data\_fetchBlockFromEVB\_cb 

```C++
typedef status(* ns_rpc_data_fetchBlockFromEVB_cb) (dataBlock *block);
```






### typedef ns\_rpc\_data\_sendBlockToEVB\_cb 

```C++
typedef status(* ns_rpc_data_sendBlockToEVB_cb) (const dataBlock *block);
```






### enum rpcGenericDataMode\_e 

```C++
enum rpcGenericDataMode_e {
    NS_RPC_GENERICDATA_CLIENT,
    NS_RPC_GENERICDATA_SERVER
};
```



## Public Attributes Documentation




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




        



### function ns\_rpc\_genericDataOperationsClient\_reset 

```C++
uint16_t ns_rpc_genericDataOperationsClient_reset (
    ns_rpc_config_t * cfg
) 
```






### function ns\_rpc\_genericDataOperations\_disableServerPoll 

_Disable RPC server and stop receiving RPC calls._ 
```C++
void ns_rpc_genericDataOperations_disableServerPoll (
    ns_rpc_config_t * cfg
) 
```





**Parameters:**


* `cfg` 




        



### function ns\_rpc\_genericDataOperations\_enableServerPoll 

_Enable RPC server and prepare to receive RPC calls._ 
```C++
void ns_rpc_genericDataOperations_enableServerPoll (
    ns_rpc_config_t * cfg
) 
```





**Parameters:**


* `cfg` 




        



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





### define NS\_RPC\_GDO\_API\_ID 

```C++
#define NS_RPC_GDO_API_ID 0xCA0100
```






### define NS\_RPC\_GDO\_CURRENT\_VERSION 

```C++
#define NS_RPC_GDO_CURRENT_VERSION NS_RPC_GDO_V1_0_0
```






### define NS\_RPC\_GDO\_OLDEST\_SUPPORTED\_VERSION 

```C++
#define NS_RPC_GDO_OLDEST_SUPPORTED_VERSION NS_RPC_GDO_V0_0_1
```






### define NS\_RPC\_GDO\_V0\_0\_1 

```C++
#define NS_RPC_GDO_V0_0_1 { .major = 0, .minor = 0, .revision = 1 }
```






### define NS\_RPC\_GDO\_V1\_0\_0 

```C++
#define NS_RPC_GDO_V1_0_0 { .major = 1, .minor = 0, .revision = 0 }
```






### define NS\_RPC\_MALLOC\_SIZE\_IN\_K 

```C++
#define NS_RPC_MALLOC_SIZE_IN_K 8
```




------------------------------


