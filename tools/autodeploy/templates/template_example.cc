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

// #include <cstdlib>
// #include <cstring>

#include "NS_AD_NAME_api.h"
#include "ns_model.h"

// TFLM Config and arena
static ns_model_state_t model;

int
main(void) {
    // ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    // NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\b");

    int status = NS_AD_NAME_init(&model);

    // 'in' contains the input tensors, treat as homogeneous block
    // memcpy(tflm.model_input->data.int8, in->buffer.data, in->buffer.dataLength);

    TfLiteStatus invoke_status = model.interpreter->Invoke();

    if (invoke_status != kTfLiteOk) {
        while (1)
            ;
        // ns_lp_printf("Invoke failed\n");
    }

    //  memcpy(resultBuffer, tflm.model_output->data.int8, mut_cfg.config.output_length);

    // Add some pre/post callbacks
    // erpc_server_add_pre_cb_action(&ns_preAction);
    // erpc_server_add_post_cb_action(&ns_postAction);
    //

    while (1) {
        // ns_rpc_genericDataOperations_pollServer(&rpcConfig);
        // ns_delay_us(1000);
        // ns_deep_sleep();
    }
}
