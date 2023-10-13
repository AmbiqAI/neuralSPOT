

# File GenericDataOperations\_EvbToPc.h



[**FileList**](files.md) **>** [**includes-api**](dir_2723405b1f2a31e5bda368efdd9e115c.md) **>** [**GenericDataOperations\_EvbToPc.h**](_generic_data_operations___evb_to_pc_8h.md)

[Go to the source code of this file](_generic_data_operations___evb_to_pc_8h_source.md)



* `#include "erpc_version.h"`
* `#include <stdbool.h>`
* `#include <stddef.h>`
* `#include <stdint.h>`















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**binary\_t**](structbinary__t.md) <br> |
| struct | [**dataBlock**](structdata_block.md) <br> |


## Public Types

| Type | Name |
| ---: | :--- |
| enum  | [**\_evb\_to\_pc\_ids**](#enum-_evb_to_pc_ids)  <br>_evb\_to\_pc identifiers_  |
| typedef struct [**binary\_t**](structbinary__t.md) | [**binary\_t**](#typedef-binary_t)  <br> |
| enum  | [**command**](#enum-command)  <br> |
| typedef enum command | [**command**](#typedef-command)  <br> |
| typedef struct [**dataBlock**](structdata_block.md) | [**dataBlock**](#typedef-datablock)  <br> |
| enum  | [**dataType**](#enum-datatype)  <br> |
| typedef enum dataType | [**dataType**](#typedef-datatype)  <br> |
| enum  | [**status**](#enum-status)  <br> |
| typedef enum status | [**status**](#typedef-status)  <br> |




















## Public Functions

| Type | Name |
| ---: | :--- |
|  status | [**ns\_rpc\_data\_computeOnPC**](#function-ns_rpc_data_computeonpc) (const [**dataBlock**](structdata_block.md) \* in\_block, [**dataBlock**](structdata_block.md) \* result\_block) <br> |
|  status | [**ns\_rpc\_data\_fetchBlockFromPC**](#function-ns_rpc_data_fetchblockfrompc) ([**dataBlock**](structdata_block.md) \* block) <br> |
|  status | [**ns\_rpc\_data\_remotePrintOnPC**](#function-ns_rpc_data_remoteprintonpc) (const char \* msg) <br> |
|  status | [**ns\_rpc\_data\_sendBlockToPC**](#function-ns_rpc_data_sendblocktopc) (const [**dataBlock**](structdata_block.md) \* block) <br> |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**ERPC\_TYPE\_DEFINITIONS**](_generic_data_operations___evb_to_pc_8h.md#define-erpc_type_definitions)  <br> |

## Public Types Documentation




### enum \_evb\_to\_pc\_ids 

```C++
enum _evb_to_pc_ids {
    kevb_to_pc_service_id = 1,
    kevb_to_pc_ns_rpc_data_sendBlockToPC_id = 1,
    kevb_to_pc_ns_rpc_data_fetchBlockFromPC_id = 2,
    kevb_to_pc_ns_rpc_data_computeOnPC_id = 3,
    kevb_to_pc_ns_rpc_data_remotePrintOnPC_id = 4
};
```






### typedef binary\_t 

```C++
typedef struct binary_t binary_t;
```






### enum command 

```C++
enum command {
    generic_cmd = 0,
    visualize_cmd = 1,
    infer_cmd = 2,
    extract_cmd = 3,
    write_cmd = 4,
    read = 5,
    generic_cmd = 0,
    visualize_cmd = 1,
    infer_cmd = 2,
    extract_cmd = 3,
    write_cmd = 4,
    read = 5
};
```






### typedef command 

```C++
typedef enum command command;
```






### typedef dataBlock 

```C++
typedef struct dataBlock dataBlock;
```






### enum dataType 

```C++
enum dataType {
    uint8_e = 0,
    uint16_e = 1,
    uint32_e = 2,
    int8_e = 3,
    int16_e = 4,
    int32_e = 5,
    float32_e = 6,
    float64_e = 7,
    uint8_e = 0,
    uint16_e = 1,
    uint32_e = 2,
    int8_e = 3,
    int16_e = 4,
    int32_e = 5,
    float32_e = 6,
    float64_e = 7
};
```






### typedef dataType 

```C++
typedef enum dataType dataType;
```






### enum status 

```C++
enum status {
    ns_rpc_data_success = 0,
    ns_rpc_data_failure = 1,
    ns_rpc_data_blockTooLarge = 2,
    ns_rpc_data_success = 0,
    ns_rpc_data_failure = 1,
    ns_rpc_data_blockTooLarge = 2
};
```






### typedef status 

```C++
typedef enum status status;
```



## Public Functions Documentation




### function ns\_rpc\_data\_computeOnPC 

```C++
status ns_rpc_data_computeOnPC (
    const dataBlock * in_block,
    dataBlock * result_block
) 
```






### function ns\_rpc\_data\_fetchBlockFromPC 

```C++
status ns_rpc_data_fetchBlockFromPC (
    dataBlock * block
) 
```






### function ns\_rpc\_data\_remotePrintOnPC 

```C++
status ns_rpc_data_remotePrintOnPC (
    const char * msg
) 
```






### function ns\_rpc\_data\_sendBlockToPC 

```C++
status ns_rpc_data_sendBlockToPC (
    const dataBlock * block
) 
```



## Macro Definition Documentation





### define ERPC\_TYPE\_DEFINITIONS 

```C++
#define ERPC_TYPE_DEFINITIONS 
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-rpc/includes-api/GenericDataOperations_EvbToPc.h`

