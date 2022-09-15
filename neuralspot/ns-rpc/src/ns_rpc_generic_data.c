/**
 * @file ns-rpc-audio.c
 * @author Carlos Morales
 * @brief API for Audio RPC
 * @version 0.1
 * @date 2022-08-26
 * 
 * This supports both server and client modes in a single source file,
 * but these modes are currently exclusive in NeuralSPOT - choose one
 * at init time.
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "ns_usb.h"
#include "ns_rpc_generic_data.h"
#include "erpc_client_setup.h"
#include "erpc_server_setup.h"

// Common interface header files
#include "GenericDataOperations_EvbToPc.h"
#include "GenericDataOperations_PcToEvb.h"
#include "GenericDataOperations_PcToEvb_server.h"
#include "ns_ambiqsuite_harness.h"

#define NS_RPC_GENERIC_DATA
#ifdef NS_RPC_GENERIC_DATA

ns_usb_config_t g_RpcGenericUSBHandle = {
        .deviceType = NS_USB_CDC_DEVICE,
        .buffer = NULL, // Not needed by RPC
        .bufferLength = 0,
        .rx_cb = NULL,
        .tx_cb = NULL
};

ns_rpc_config_t g_RpcGenericDataConfig = {
    .mode = NS_RPC_GENERICDATA_CLIENT,
    .sendBlockToEVB_cb = NULL,
    .fetchBlockFromEVB_cb = NULL,
    .computeOnEVB_cb = NULL
};

// GenericDataOperations implements 3 function calls that service
// remote calls from a PC. They must be instantiated to enable them.
// Datatypes, function prototypes, etc, are defined in the RPC's include files
status ns_rpc_data_sendBlockToEVB(const dataBlock * block) {
    ns_printf("Received call to sendBlockToEVB\n");

    if (g_RpcGenericDataConfig.sendBlockToEVB_cb != NULL) {
        return g_RpcGenericDataConfig.sendBlockToEVB_cb(block);
    } else { 
        return ns_rpc_data_success;
    }
}

status ns_rpc_data_fetchBlockFromEVB(dataBlock * block) {
    ns_printf("Received call to fetchBlockFromEVB\n");

    if (g_RpcGenericDataConfig.fetchBlockFromEVB_cb != NULL) {
        return g_RpcGenericDataConfig.fetchBlockFromEVB_cb(block);
    } else { 
        return ns_rpc_data_success;
    }
}

status ns_rpc_data_computeOnEVB(const dataBlock * in_block, dataBlock * result_block) {
    ns_printf("Received call to computeOnEVB\n");

    if (g_RpcGenericDataConfig.computeOnEVB_cb != NULL) {
        return g_RpcGenericDataConfig.computeOnEVB_cb(in_block, result_block);
    } else { 
        return ns_rpc_data_success;
    }
}

uint16_t ns_rpc_genericDataOperations_init(ns_rpc_config_t *cfg) {

    usb_handle_t usb_handle = ns_usb_init(&g_RpcGenericUSBHandle);

    g_RpcGenericDataConfig.mode = cfg->mode;
    g_RpcGenericDataConfig.sendBlockToEVB_cb = cfg->sendBlockToEVB_cb;
    g_RpcGenericDataConfig.fetchBlockFromEVB_cb = cfg->fetchBlockFromEVB_cb;
    g_RpcGenericDataConfig.computeOnEVB_cb = cfg->computeOnEVB_cb;

    // Common ERPC init
    /* USB transport layer initialization */
    erpc_transport_t transport = erpc_transport_usb_cdc_init(usb_handle);

    /* MessageBufferFactory initialization */
    erpc_mbf_t message_buffer_factory = erpc_mbf_dynamic_init();

    if (cfg->mode == NS_RPC_GENERICDATA_CLIENT) {
        /* Init eRPC client environment */
        erpc_client_init(transport, message_buffer_factory);
    } else {
        // Initialize the server and service
        erpc_server_init(transport, message_buffer_factory);
        erpc_service_t service = create_pc_to_evb_service();
        erpc_add_service_to_server(service);
    }

    /* run server (IN MAIN LOOP) */
    //erpc_server_run(); /* or erpc_server_poll(); */
    //erpc_server_run(); /* or erpc_server_poll(); */

    return 1;
}

#else
uint16_t ns_rpc_genericDataOperations_init() {return 1;}
#endif