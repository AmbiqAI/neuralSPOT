

# File ns\_rpc\_generic\_data.c

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-rpc**](dir_e7349e64d3511848a99e2d9321b8e9b2.md) **>** [**src**](dir_cd98b33a533efa56f36cb87393b63939.md) **>** [**ns\_rpc\_generic\_data.c**](ns__rpc__generic__data_8c.md)

[Go to the documentation of this file](ns__rpc__generic__data_8c.md)

```C++


#include "ns_rpc_generic_data.h"
#include "erpc_client_setup.h"
#include "erpc_server_setup.h"
#include "ns_usb.h"

// Common interface header files
#include "GenericDataOperations_EvbToPc.h"
#include "GenericDataOperations_PcToEvb.h"
#include "GenericDataOperations_PcToEvb_server.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_core.h"
#include "ns_malloc.h"

#define NS_RPC_GENERIC_DATA
#ifdef NS_RPC_GENERIC_DATA

const ns_core_api_t ns_rpc_gdo_V0_0_1 = {.apiId = NS_RPC_GDO_API_ID, .version = NS_RPC_GDO_V0_0_1};

const ns_core_api_t ns_rpc_gdo_V1_0_0 = {.apiId = NS_RPC_GDO_API_ID, .version = NS_RPC_GDO_V1_0_0};

const ns_core_api_t ns_rpc_gdo_oldest_supported_version = {.apiId = NS_RPC_GDO_API_ID,
                                                           .version = NS_RPC_GDO_V0_0_1};

const ns_core_api_t ns_rpc_gdo_current_version = {.apiId = NS_RPC_GDO_API_ID,
                                                  .version = NS_RPC_GDO_V1_0_0};

ns_usb_config_t g_RpcGenericUSBHandle = {.api = &ns_usb_V1_0_0,
                                         .deviceType = NS_USB_CDC_DEVICE,
                                         .rx_buffer = NULL,
                                         .rx_bufferLength = 0,
                                         .tx_buffer = NULL,
                                         .tx_bufferLength = 0,
                                         .rx_cb = NULL,
                                         .tx_cb = NULL,
                                         .service_cb = NULL};

ns_rpc_config_t g_RpcGenericDataConfig = {.api = &ns_rpc_gdo_current_version,
                                          .mode = NS_RPC_GENERICDATA_CLIENT,
                                          .rx_buf = NULL,
                                          .rx_bufLength = 0,
                                          .tx_buf = NULL,
                                          .tx_bufLength = 0,
                                          .usbHandle = NULL,
                                          .sendBlockToEVB_cb = NULL,
                                          .fetchBlockFromEVB_cb = NULL,
                                          .computeOnEVB_cb = NULL};

// GenericDataOperations implements 3 function calls that service
// remote calls from a PC. They must be instantiated to enable them.
// Datatypes, function prototypes, etc, are defined in the RPC's include files
status
ns_rpc_data_sendBlockToEVB(const dataBlock *block) {
    // ns_lp_printf("Received call to sendBlockToEVB\n");

    if (g_RpcGenericDataConfig.sendBlockToEVB_cb != NULL) {
        return g_RpcGenericDataConfig.sendBlockToEVB_cb(block);
    } else {
        return ns_rpc_data_success;
    }
}

status
ns_rpc_data_fetchBlockFromEVB(dataBlock *block) {
    // ns_lp_printf("Received call to fetchBlockFromEVB\n");

    if (g_RpcGenericDataConfig.fetchBlockFromEVB_cb != NULL) {
        return g_RpcGenericDataConfig.fetchBlockFromEVB_cb(block);
    } else {
        return ns_rpc_data_success;
    }
}

status
ns_rpc_data_computeOnEVB(const dataBlock *in_block, dataBlock *result_block) {
    // ns_lp_printf("Received call to computeOnEVB\n");

    if (g_RpcGenericDataConfig.computeOnEVB_cb != NULL) {
        return g_RpcGenericDataConfig.computeOnEVB_cb(in_block, result_block);
    } else {
        return ns_rpc_data_success;
    }
}

// void ns_rpc_data_serverService(uint8_t haveUsbData) {
//     if ((g_RpcGenericDataConfig.serviceServer == true) && (haveUsbData == 1)) {
//         erpc_server_poll(); // service RPC server
//     }
// }

uint16_t
ns_rpc_genericDataOperations_init(ns_rpc_config_t *cfg) {
    #ifndef NS_DISABLE_API_VALIDATION
    if (cfg == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }

    if (ns_core_check_api(cfg->api, &ns_rpc_gdo_oldest_supported_version,
                          &ns_rpc_gdo_current_version)) {
        return NS_STATUS_INVALID_VERSION;
    }
    #endif
    // usb_handle_t usb_handle = ns_usb_init(&g_RpcGenericUSBHandle);
    usb_handle_t usb_handle = NULL;

    // For server mode, add a service callback to USB
    // if (cfg->mode == NS_RPC_GENERICDATA_SERVER) {
    //     g_RpcGenericUSBHandle.service_cb = &ns_rpc_data_serverService;
    // }
    g_RpcGenericUSBHandle.rx_buffer = cfg->rx_buf;
    g_RpcGenericUSBHandle.rx_bufferLength = cfg->rx_bufLength;
    g_RpcGenericUSBHandle.tx_buffer = cfg->tx_buf;
    g_RpcGenericUSBHandle.tx_bufferLength = cfg->tx_bufLength;

    NS_TRY(ns_usb_init(&g_RpcGenericUSBHandle, &usb_handle), "USB Init Failed\n");

    g_RpcGenericDataConfig.mode = cfg->mode;
    g_RpcGenericDataConfig.sendBlockToEVB_cb = cfg->sendBlockToEVB_cb;
    g_RpcGenericDataConfig.fetchBlockFromEVB_cb = cfg->fetchBlockFromEVB_cb;
    g_RpcGenericDataConfig.computeOnEVB_cb = cfg->computeOnEVB_cb;
    // g_RpcGenericDataConfig.serviceServer = cfg->serviceServer;
    g_RpcGenericDataConfig.rx_buf = cfg->rx_buf;
    g_RpcGenericDataConfig.rx_bufLength = cfg->rx_bufLength;
    g_RpcGenericDataConfig.tx_buf = cfg->tx_buf;
    g_RpcGenericDataConfig.tx_bufLength = cfg->tx_bufLength;

    g_RpcGenericDataConfig.usbHandle = usb_handle;

    // Common ERPC init
    /* USB transport layer initialization */
    erpc_transport_t transport = erpc_transport_usb_cdc_init(usb_handle);

    /* MessageBufferFactory initialization */
    erpc_mbf_t message_buffer_factory = erpc_mbf_static_init();

    if (cfg->mode == NS_RPC_GENERICDATA_CLIENT) {
        /* Init eRPC client environment */
        erpc_client_init(transport, message_buffer_factory);
    } else {
        // Initialize the server and service
        erpc_server_init(transport, message_buffer_factory);
        erpc_service_t service = create_pc_to_evb_service();
        erpc_add_service_to_server(service);
    }

    return NS_STATUS_SUCCESS;
}

// void
// ns_rpc_genericDataOperations_enableServerPoll(ns_rpc_config_t *cfg) {
//     g_RpcGenericDataConfig.serviceServer = true;
// }

// void
// ns_rpc_genericDataOperations_disableServerPoll(ns_rpc_config_t *cfg) {
//     g_RpcGenericDataConfig.serviceServer = false;
// }

void
ns_rpc_genericDataOperations_pollServer(ns_rpc_config_t *cfg) {
    if (ns_usb_data_available(cfg->usbHandle)) {
        erpc_server_poll(); // service RPC server
    }
}

uint16_t
ns_rpc_genericDataOperationsClient_reset(ns_rpc_config_t *cfg) {
    erpc_client_deinit();
    return ns_rpc_genericDataOperations_init(cfg);
}

void
ns_rpc_genericDataOperations_printDatablock(const dataBlock *block) {
    uint32_t i = 0;
    ns_lp_printf("Descriptor: %s\n", block->description);
    ns_lp_printf("Length: %d\n", block->length);
    ns_lp_printf("buffer.dataLength: %d\n", block->buffer.dataLength);
    ns_lp_printf("Contents:\n");
    for (i = 0; i < block->buffer.dataLength; i++) {
        ns_lp_printf("0x%x, ", block->buffer.data[i]);
    }
    ns_lp_printf("\n");
}

void
ns_rpc_data_clientDoneWithBlockFromPC(const dataBlock *block) {
    ns_free(block->description);
    ns_free(block->buffer.data);
}

#else
uint16_t
ns_rpc_genericDataOperations_init() {
    return 1;
}
#endif

```
