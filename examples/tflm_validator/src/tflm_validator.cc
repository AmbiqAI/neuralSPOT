/**
 * @file tflm_validator.cc
 * @author Carlos Morales
 * @brief Shell application for instantiating a TFLM model and allowing a RPC
 * client to set input tensors, invoke() it, and collect output tensors
 * @version 0.1
 * @date 2023-02-28
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <cstdlib>
#include <cstring>

#include "generic_model.h"

#include "ns_ambiqsuite_harness.h"
#include "ns_core.h"
#include "ns_malloc.h"
#include "ns_peripherals_button.h"
#include "ns_peripherals_power.h"
#include "ns_rpc_generic_data.h"
#include "ns_usb.h"

typedef struct {
    uint32_t profile_mut;
    uint32_t input_length;  // in bytes
    uint32_t output_length; // in bytes
} ns_mut_config_t;

typedef union {
    ns_mut_config_t config;
    char bytes[sizeof(ns_mut_config_t)];
} ns_incoming_config_t;

ns_incoming_config_t mut_cfg;

// GenericDataOperations implements 3 function calls that service
// remote calls from a PC. They must be instantiated to enable them.
// Datatypes, function prototypes, etc, are defined in the RPC's include files
// These functions are passed to ns_rpc init as callbacks.

// Handler for sendBlockToEVB, invoked by PC
/**
 * @brief Initializes the model per config struct
 *
 * @param block - serialized tflm_config_struct
 * @return status
 */
status
configureModel(const dataBlock *in) {
    ns_lp_printf("Server requested model initialization\n");
    ns_lp_printf("Uint Size %d, mut.profile size %d\n", sizeof(uint8_t),
                 sizeof(mut_cfg.config.profile_mut));
    // Grab incoming buffer, decode into config struct
    if (in->buffer.dataLength != sizeof(mut_cfg)) {
        ns_lp_printf("Error - size mismatch, expected %d, got %d", sizeof(mut_cfg),
                     in->buffer.dataLength);
        return ns_rpc_data_failure;
    }
    memcpy(&mut_cfg, in->buffer.data, sizeof(mut_cfg));
    model_init();
    ns_rpc_genericDataOperations_printDatablock(in);
    return ns_rpc_data_success;
}

// Handler for fetchBlockFromEVB, invoked by PC
/**
 * @brief Get the statistics about the last Invoke()
 *
 * @param block - serialized tflm_stats_struct
 * @return status
 */
status
getStatistics(dataBlock *block) {
    ns_printf("Server asked for invoke() statistics\n");

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
/**
 * @brief Calls TFLM's invoke()
 *
 * @param in  - input tensor
 * @param res - output tensor
 * @return status - fail if not configured or if invoke fails
 */
status
infer_on_tflm(const dataBlock *in, dataBlock *res) {
    // Prep the return block, needs to happen whether errors occur or not
    ns_printf("Server requested invoke()\n");

    uint8_t *resultBuffer =
        (uint8_t *)ns_malloc(mut_cfg.config.output_length *
                             sizeof(uint8_t)); // see above for explanation of why we need malloc
    char *msg_store = (char *)ns_malloc(sizeof(char) * 30);
    res->length = mut_cfg.config.output_length * sizeof(uint8_t);
    res->dType = uint8_e;
    res->description = msg_store;
    res->cmd = generic_cmd;
    binary_t binaryBlock = {.data = (uint8_t *)resultBuffer,
                            .dataLength = mut_cfg.config.output_length * sizeof(uint8_t)};
    res->buffer = binaryBlock;

    // 'in' contains the input tensors, treat as homogeneous block
    ns_printf("Incoming Data Block:\n");
    ns_rpc_genericDataOperations_printDatablock(in);
    memcpy(model_input->data.int8, in->buffer.data, in->buffer.dataLength);

    TfLiteStatus invoke_status = interpreter->Invoke();

    if (invoke_status != kTfLiteOk) {
        ns_lp_printf("Invoke failed\n");
        char error_msg[] = "Invoke failed\0";
        memcpy(msg_store, error_msg, sizeof(error_msg));
        return ns_rpc_data_failure;
    }

    // Prep the return block with output tensor
    memcpy(resultBuffer, model_output->data.int8, mut_cfg.config.output_length);

    ns_printf("Resulting Data Block To Be Sent:\n");
    ns_rpc_genericDataOperations_printDatablock(res);

    char res_msg[] = "Invoke Successful!\0";
    memcpy(msg_store, res_msg, sizeof(res_msg));
    return ns_rpc_data_success;
}

void
ns_preAction(void) {
    ns_lp_printf("Starting action\n");
}

void
ns_postAction(void) {
    ns_lp_printf("Stopping action\n");
}

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
                                 .sendBlockToEVB_cb = configureModel,
                                 .fetchBlockFromEVB_cb = getStatistics,
                                 .computeOnEVB_cb = infer_on_tflm};
    NS_TRY(ns_rpc_genericDataOperations_init(&rpcConfig), "RPC Init Failed\n");

    // Add some pre/post callbacks
    erpc_server_add_pre_cb_action(&ns_preAction);
    erpc_server_add_post_cb_action(&ns_postAction);

    // ns_printf("Start the PC-side client, then press Button 0 to get started\n");
    // while (g_intButtonPressed == 0) {
    //     ns_delay_us(1000);
    // }

    ns_printf("Ready to receive RPC Calls\n");

    // In the app loop we service USB and the RPC server
    // Any incoming RPC calls will result in calls to the
    // RPC handler functions defined above.
    //
    while (1) {
        erpc_server_poll(); // service RPC server
        ns_delay_us(1);
    }
}
