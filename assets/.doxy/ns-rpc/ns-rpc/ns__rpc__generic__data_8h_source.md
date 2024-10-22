

# File ns\_rpc\_generic\_data.h

[**File List**](files.md) **>** [**includes-api**](dir_2723405b1f2a31e5bda368efdd9e115c.md) **>** [**ns\_rpc\_generic\_data.h**](ns__rpc__generic__data_8h.md)

[Go to the documentation of this file](ns__rpc__generic__data_8h.md)

```C++


#ifndef NS_RPC_GENERIC_DATA_H
    #define NS_RPC_GENERIC_DATA_H

    #include "GenericDataOperations_EvbToPc.h"
    #include "GenericDataOperations_PcToEvb.h"
    #include "erpc_server_setup.h"
    #ifdef __cplusplus
extern "C" {
    #endif
    #include "ns_core.h"

    #define NS_RPC_GDO_V0_0_1                                                                      \
        { .major = 0, .minor = 0, .revision = 1 }
    #define NS_RPC_GDO_V1_0_0                                                                      \
        { .major = 1, .minor = 0, .revision = 0 }

    #define NS_RPC_GDO_OLDEST_SUPPORTED_VERSION NS_RPC_GDO_V0_0_1
    #define NS_RPC_GDO_CURRENT_VERSION NS_RPC_GDO_V1_0_0
    #define NS_RPC_GDO_API_ID 0xCA0100

    #define NS_RPC_MALLOC_SIZE_IN_K 8
extern const ns_core_api_t ns_rpc_gdo_V0_0_1;
extern const ns_core_api_t ns_rpc_gdo_V1_0_0;
extern const ns_core_api_t ns_rpc_gdo_oldest_supported_version;
extern const ns_core_api_t ns_rpc_gdo_current_version;

typedef status (*ns_rpc_data_sendBlockToEVB_cb)(const dataBlock *block);

typedef status (*ns_rpc_data_fetchBlockFromEVB_cb)(dataBlock *block);

typedef status (*ns_rpc_data_computeOnEVB_cb)(const dataBlock *in_block, dataBlock *result_block);

typedef enum { NS_RPC_GENERICDATA_CLIENT, NS_RPC_GENERICDATA_SERVER } rpcGenericDataMode_e;

typedef struct {
    const ns_core_api_t *api;  
    rpcGenericDataMode_e mode; 
    uint8_t *rx_buf;        
    uint32_t rx_bufLength;  
    uint8_t *tx_buf;        
    uint32_t tx_bufLength;  
    usb_handle_t usbHandle; 
    ns_rpc_data_sendBlockToEVB_cb sendBlockToEVB_cb;       
    ns_rpc_data_fetchBlockFromEVB_cb fetchBlockFromEVB_cb; 
    ns_rpc_data_computeOnEVB_cb computeOnEVB_cb;           
} ns_rpc_config_t;

extern uint16_t ns_rpc_genericDataOperations_init(ns_rpc_config_t *cfg);

extern uint16_t ns_rpc_genericDataOperationsClient_reset(ns_rpc_config_t *cfg);

extern void ns_rpc_genericDataOperations_printDatablock(const dataBlock *block);

extern void ns_rpc_data_clientDoneWithBlockFromPC(const dataBlock *block);

extern void ns_rpc_genericDataOperations_enableServerPoll(ns_rpc_config_t *cfg);

extern void ns_rpc_genericDataOperations_disableServerPoll(ns_rpc_config_t *cfg);

extern void ns_rpc_genericDataOperations_pollServer(ns_rpc_config_t *cfg);

    #ifdef __cplusplus
}
    #endif
#endif    // NS_RPC_GENERIC_DATA_H // end of ns-rpc-generic-data

```

