/**
 * @file rpc_server_example.cc
 * @author Carlos Morales
 * @brief Generic Data Operations RPC Example
 * @version 0.1
 * @date 2022-08-26
 *
 * This example showcases the NS Generic Data RPC interface, which
 * is useful for sending data back and forth across the EVB's second
 * USB interface via remote procedure calls.
 *
 * Both remote procedure call directions (EVB to PC and PC to EVB) are
 * demonstrated, using audio as an example data source.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <cstdlib>
#include <cstring>

#include "ns_ambiqsuite_harness.h"
#include "ns_core.h"
#include "ns_malloc.h"
#include "ns_peripherals_button.h"
#include "ns_peripherals_power.h"
#include "ns_rpc_generic_data.h"
#include "ns_usb.h"

// GenericDataOperations implements 3 function calls that service
// remote calls from a PC. They must be instantiated to enable them.
// Datatypes, function prototypes, etc, are defined in the RPC's include files
// These functions are passed to ns_rpc init as callbacks.

// Handler for sendBlockToEVB, invoked by PC
status
example_sendBlockToEVB(const dataBlock *block) {
    ns_lp_printf("LOCAL Received call to sendBlockToEVB\n");
    // Grab block and do something with it
    // ...
    ns_rpc_genericDataOperations_printDatablock(block);
    return ns_rpc_data_success;
}

// Handler for fetchBlockFromEVB, invoked by PC
status
example_fetchBlockFromEVB(dataBlock *block) {
    ns_lp_printf("LOCAL Received call to fetchBlockFromEVB\n");

    // For strings (binary->description) and binary structs (block->buffer)
    // ERPC will attempt to free() the memory - this is kind
    // of an ERPC bug IMHO. Nevertheless, strings & structs must be
    // malloc'd using ns_malloc.

    uint32_t len = 4;
    uint16_t *retBuffer = (uint16_t *)ns_malloc(len * sizeof(uint16_t));
    char *msg_store = (char *)ns_malloc(sizeof(char) * 30); // arbitrary size

    uint16_t db[] = {0xCA, 0x01, 0x73, 0x50};
    memcpy(retBuffer, db, len * sizeof(uint16_t));

    char msg[] = "DisplayThis\0";
    memcpy(msg_store, msg, sizeof(msg));

    binary_t binaryBlock = {.data = (uint8_t *)retBuffer, .dataLength = len * sizeof(uint16_t)};

    // Populate the block to be sent to PC
    block->length = len * sizeof(uint16_t);
    block->dType = uint16_e;
    block->description = msg_store;
    block->cmd = generic_cmd;
    block->buffer = binaryBlock;
    ns_rpc_genericDataOperations_printDatablock(block);

    return ns_rpc_data_success;
}

// Handler for computeOnEVB, invoked by PC
status
example_computeOnEVB(const dataBlock *in, dataBlock *res) {
    uint32_t i;

    ns_lp_printf("LOCAL Received call to computeOnEVB\n");

    // Compute res block based on in block
    ns_lp_printf("Incoming Data Block:\n");
    ns_rpc_genericDataOperations_printDatablock(in);
    uint32_t len = in->buffer.dataLength;
    uint8_t *resultBuffer = (uint8_t *)ns_malloc(
        len * sizeof(uint8_t)); // see above for explanation of why we need malloc
    char *msg_store = (char *)ns_malloc(sizeof(char) * 30);
    char msg[] = "Multiplied by 2\0";
    memcpy(msg_store, msg, sizeof(msg));

    // Multiply the incoming block * 2
    for (i = 0; i < len; i++) {
        resultBuffer[i] = in->buffer.data[i] * 2;
    }

    binary_t binaryBlock = {.data = (uint8_t *)resultBuffer, .dataLength = len * sizeof(uint8_t)};

    // Build the block sent to PC
    res->length = len * sizeof(uint8_t);
    res->dType = uint8_e;
    res->description = msg_store;
    res->cmd = generic_cmd;
    res->buffer = binaryBlock;

    ns_lp_printf("Resulting Data Block To Be Sent:\n");
    ns_rpc_genericDataOperations_printDatablock(res);

    return ns_rpc_data_success;
}

#define MY_RX_BUFSIZE 512
#define MY_TX_BUFSIZE 512
uint8_t my_cdc_rx_ff_buf[MY_RX_BUFSIZE];
uint8_t my_cdc_tx_ff_buf[MY_TX_BUFSIZE];

int
main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};

    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\b");
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed\n");
    ns_itm_printf_enable();

    // ----- Non-RPC Init ------
    // -- These are needed for the demo, not directly related to RPC
    ns_interrupt_master_enable();

    // -- Init the button handler, used in the example, not needed by RPC
    volatile int g_intButtonPressed = 0;
    ns_button_config_t button_config = {.api = &ns_button_V1_0_0,
                                        .button_0_enable = true,
                                        .button_1_enable = false,
                                        .button_0_flag = &g_intButtonPressed,
                                        .button_1_flag = NULL};
    NS_TRY(ns_peripheral_button_init(&button_config), "Button init failed\n");

    // Add callbacks to handle incoming requests
    ns_rpc_config_t rpcConfig = {.api = &ns_rpc_gdo_V1_0_0,
                                 .mode = NS_RPC_GENERICDATA_SERVER, // Puts EVB in RPC server mode
                                 .rx_buf = my_cdc_rx_ff_buf,
                                 .rx_bufLength = MY_RX_BUFSIZE,
                                 .tx_buf = my_cdc_tx_ff_buf,
                                 .tx_bufLength = MY_TX_BUFSIZE,
                                 .sendBlockToEVB_cb = example_sendBlockToEVB,
                                 .fetchBlockFromEVB_cb = example_fetchBlockFromEVB,
                                 .computeOnEVB_cb = example_computeOnEVB};

    NS_TRY(ns_rpc_genericDataOperations_init(&rpcConfig), "RPC Init Failed\n");

    ns_lp_printf("Ready to receive RPC Calls\n");

    // In the app loop we service USB and the RPC server
    // Any incoming RPC calls will result in calls to the
    // RPC handler functions defined above.

    while (1) {
        ns_rpc_genericDataOperations_pollServer(&rpcConfig);
        ns_deep_sleep();
    }
}
