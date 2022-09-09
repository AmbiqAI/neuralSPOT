/**
 * @file ns-rpc-audio.c
 * @author Carlos Morales
 * @brief API for Audio RPC
 * @version 0.1
 * @date 2022-08-26
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

uint16_t ns_rpc_genericDataOperations_init() {

    usb_handle_t usb_handle = ns_usb_init(&g_RpcGenericUSBHandle);

    // Common ERPC init
    /* USB transport layer initialization */
    erpc_transport_t transport = erpc_transport_usb_cdc_init(usb_handle);

    /* MessageBufferFactory initialization */
    erpc_mbf_t message_buffer_factory = erpc_mbf_dynamic_init();

    /* Init eRPC client environment */
    /* eRPC client side initialization */
    erpc_client_init(transport, message_buffer_factory);

    // Now initialize the server
    erpc_server_init(transport, message_buffer_factory);

    /* connect generated service into server, look into erpc_matrix_multiply_server.h */
    erpc_service_t service = create_pc_to_evb_service();
    erpc_add_service_to_server(service);

    /* run server (IN MAIN LOOP) */
    //erpc_server_run(); /* or erpc_server_poll(); */
    //erpc_server_run(); /* or erpc_server_poll(); */

    return 1;
}

#else
uint16_t ns_rpc_genericDataOperations_init() {return 1;}
#endif