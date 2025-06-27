

# File GenericDataOperations\_PcToEvb.h



[**FileList**](files.md) **>** [**includes-api**](dir_2723405b1f2a31e5bda368efdd9e115c.md) **>** [**GenericDataOperations\_PcToEvb.h**](_generic_data_operations___pc_to_evb_8h.md)

[Go to the source code of this file](_generic_data_operations___pc_to_evb_8h_source.md)



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
| enum  | [**\_pc\_to\_evb\_ids**](#enum-_pc_to_evb_ids)  <br>_pc\_to\_evb identifiers_  |
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
|  status | [**ns\_rpc\_data\_computeOnEVB**](#function-ns_rpc_data_computeonevb) (const [**dataBlock**](structdata_block.md) \* in\_block, [**dataBlock**](structdata_block.md) \* result\_block) <br> |
|  status | [**ns\_rpc\_data\_fetchBlockFromEVB**](#function-ns_rpc_data_fetchblockfromevb) ([**dataBlock**](structdata_block.md) \* block) <br> |
|  status | [**ns\_rpc\_data\_sendBlockToEVB**](#function-ns_rpc_data_sendblocktoevb) (const [**dataBlock**](structdata_block.md) \* block) <br> |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**ERPC\_TYPE\_DEFINITIONS**](_generic_data_operations___pc_to_evb_8h.md#define-erpc_type_definitions)  <br> |

## Public Types Documentation




### enum \_pc\_to\_evb\_ids 

_pc\_to\_evb identifiers_ 
```C++
enum _pc_to_evb_ids {
    kpc_to_evb_service_id = 2,
    kpc_to_evb_ns_rpc_data_sendBlockToEVB_id = 1,
    kpc_to_evb_ns_rpc_data_fetchBlockFromEVB_id = 2,
    kpc_to_evb_ns_rpc_data_computeOnEVB_id = 3
};
```




<hr>



### typedef binary\_t 

```C++
typedef struct binary_t binary_t;
```




<hr>



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




<hr>



### typedef command 

```C++
typedef enum command command;
```




<hr>



### typedef dataBlock 

```C++
typedef struct dataBlock dataBlock;
```




<hr>



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




<hr>



### typedef dataType 

```C++
typedef enum dataType dataType;
```




<hr>



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




<hr>



### typedef status 

```C++
typedef enum status status;
```




<hr>
## Public Functions Documentation




### function ns\_rpc\_data\_computeOnEVB 

```C++
status ns_rpc_data_computeOnEVB (
    const dataBlock * in_block,
    dataBlock * result_block
) 
```




<hr>



### function ns\_rpc\_data\_fetchBlockFromEVB 

```C++
status ns_rpc_data_fetchBlockFromEVB (
    dataBlock * block
) 
```




<hr>



### function ns\_rpc\_data\_sendBlockToEVB 

```C++
status ns_rpc_data_sendBlockToEVB (
    const dataBlock * block
) 
```




<hr>
## Macro Definition Documentation





### define ERPC\_TYPE\_DEFINITIONS 

```C++
#define ERPC_TYPE_DEFINITIONS 
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-rpc/includes-api/GenericDataOperations_PcToEvb.h`

