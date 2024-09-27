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
#include "ns_model.h"
#include "mut_model_metadata.h"
#include "mut_model_data.h"

#include "tflm_validator.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_core.h"
#include "ns_malloc.h"
#include "ns_peripherals_power.h"
#include "ns_rpc_generic_data.h"
#include "ns_usb.h"

#if (configAPPLICATION_ALLOCATED_HEAP == 1)
size_t ucHeapSize = (NS_RPC_MALLOC_SIZE_IN_K + 4) * 1024;
uint8_t ucHeap[(NS_RPC_MALLOC_SIZE_IN_K + 4) * 1024] __attribute__((aligned(4)));
#endif

// TFLM Config and arena
ns_model_state_t tflm;

// TF Tensor Arena
static constexpr int kTensorArenaSize = 1024 * TFLM_VALIDATOR_ARENA_SIZE;
#ifdef AM_PART_APOLLO3
    // Apollo3 doesn't have AM_SHARED_RW
    alignas(16) static uint8_t tensor_arena[kTensorArenaSize];
#else // not AM_PART_APOLLO3
    #if (TFLM_ARENA_LOCATION == NS_AD_SRAM)
        AM_SHARED_RW alignas(16) static uint8_t tensor_arena[kTensorArenaSize];
    #else
        alignas(16) static uint8_t tensor_arena[kTensorArenaSize];
    #endif
#endif

// Resource Variable Arena
static constexpr int kVarArenaSize =
    4 * (TFLM_VALIDATOR_MAX_RESOURCE_VARIABLES + 1) * sizeof(tflite::MicroResourceVariables);
alignas(16) static uint8_t var_arena[kVarArenaSize];

// Validator Stuff
AM_SHARED_RW ns_incoming_config_t mut_cfg;
AM_SHARED_RW ns_outgoing_stats_t mut_stats;
static uint32_t invokes_so_far = 0;
bool input_tensor_is_chunked = false;
bool output_tensor_is_chunked = false;
bool stats_is_chunked = false;
uint32_t input_tensor_offset = 0;
uint32_t output_tensor_offset = 0;
uint32_t model_chunk_offset = 0;
uint32_t stats_offset = 0;
uint32_t stats_remaining = 0;

ns_incoming_tensor_details_u inputTensorDetails[NS_MAX_INPUT_TENSORS];
ns_incoming_tensor_details_u outputTensorDetails[NS_MAX_OUTPUT_TENSORS];
extern int tflm_validator_model_init(ns_model_state_t *ms);

#ifdef NS_MLPROFILE
// Timer is used for TF profiling
ns_timer_config_t basic_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};
#endif

/**
 * @brief Initializes the model per config struct
 *
 * @param block - serialized tflm_config_struct
 * @return status
 */
status configureModel(const dataBlock *in) {
    ns_lp_printf("[INFO] PC requested model initialization\n");

    // Grab incoming buffer, decode into config struct
    if (in->buffer.dataLength < sizeof(mut_cfg)) { // Sanity check before memcpy
        ns_lp_printf(
            "[ERROR] Incoming config is too small, expected %d, got %d\n", sizeof(mut_cfg),
            in->buffer.dataLength);
        return ns_rpc_data_failure;
    }

    memcpy(&mut_cfg, in->buffer.data, sizeof(mut_cfg));

    if (in->buffer.dataLength != (sizeof(mut_cfg) + 4 * (mut_cfg.config.num_input_tensors +
                                                         mut_cfg.config.num_output_tensors))) {
        ns_lp_printf(
            "[ERROR] Configuration Size mismatch, expected %d, got %d\n",
            sizeof(mut_cfg) +
                4 * (mut_cfg.config.num_input_tensors + mut_cfg.config.num_output_tensors),
            in->buffer.dataLength);
        return ns_rpc_data_failure;
    }

    ns_lp_printf(
        "[INFO] MUT configuration: profile %d, warmup %d, input tensor length %d, output "
        "tensor length %d\n",
        mut_cfg.config.profile_mut, mut_cfg.config.profile_warmup, mut_cfg.config.input_length,
        mut_cfg.config.output_length);

    // buffer.data contains variable length arrays after the preamble
    // uint32_t inputTensorLengths[numInputTensors]
    // uint32_t outputTensorLengths[numPutputTensors]

    tflm.runtime = TFLM;
    tflm.model_array = mut_model;
    tflm.arena = tensor_arena;
    tflm.arena_size = kTensorArenaSize;
    tflm.rv_arena = var_arena;
    tflm.rv_arena_size = kVarArenaSize;
    tflm.rv_count = TFLM_VALIDATOR_MAX_RESOURCE_VARIABLES;
    tflm.numInputTensors = mut_cfg.config.num_input_tensors;
    tflm.numOutputTensors = mut_cfg.config.num_output_tensors;

    memcpy(
        &inputTensorDetails, in->buffer.data + sizeof(mut_cfg),
        4 * mut_cfg.config.num_input_tensors);
    memcpy(
        &outputTensorDetails,
        in->buffer.data + sizeof(mut_cfg) + (4 * mut_cfg.config.num_input_tensors),
        4 * mut_cfg.config.num_output_tensors);

#ifdef NS_MLPROFILE
    ns_perf_mac_count_t basic_mac = {
        .number_of_layers = tflm_validator_number_of_estimates,
        .mac_count_map = (uint32_t *)tflm_validator_mac_estimates};
    tflm.tickTimer = &basic_tickTimer;
    tflm.mac_estimates = &basic_mac;
#else
    tflm.tickTimer = NULL;
#endif
    ns_lp_printf("[INFO] Initializing Model\n");
    int status = tflm_validator_model_init(&tflm);
    ns_lp_printf("[INFO] Model Initialized status = %d\n", status);
    mut_stats.stats.computed_arena_size = tflm.computed_arena_size;
    ns_lp_printf("[INFO] Input Size %d \n", tflm.interpreter->inputs_size());
    ns_lp_printf("[INFO] Output Size %d \n", tflm.interpreter->outputs_size());

    ns_rpc_genericDataOperations_printDatablock(in);
    if (status == 0) {
        return ns_rpc_data_success;
    } else {
        return ns_rpc_data_failure;
    }
}

/**
 * @brief Gets a chunk of tensor via sendBlockToEVB_cb
 *
 * @param in - chunk
 * @return status
 */
status incomingTensorChunk(const dataBlock *in) {
    ns_lp_printf(
        "[INFO] PC Sent Input Tensor Chunk of %d bytes, copied to %d\n", in->buffer.dataLength,
        input_tensor_offset);
    // Get latest chunk, copy into next spot in raw
    // tensor
    memcpy(
        tflm.model_input[0]->data.int8 + input_tensor_offset, in->buffer.data,
        in->buffer.dataLength);
    input_tensor_offset += in->buffer.dataLength;
    input_tensor_is_chunked = true;
    return ns_rpc_data_success;
}

int imc = 0;
/**
 * @brief Gets a chunk of model via sendBlockToEVB_cb
 *
 * @param in - chunk
 * @return status
 */
status incomingModelChunk(const dataBlock *in) {
    // ns_lp_printf(
    //     "[INFO] PC Sent Model Chunk of %d bytes, copied to %d\n", in->buffer.dataLength,
    //     model_chunk_offset);
    ns_lp_printf(".");
    imc++;
    if (imc > 100) {
        ns_lp_printf("\n");
        imc = 0;
    }
    // Get around a compiler error when model is in MRAM (ie. const)
    //
    #if (TFLM_MODEL_LOCATION == NS_AD_MRAM)
        ns_lp_printf("[ERROR] Model in MRAM, cannot write\n");
        return ns_rpc_data_failure;
    #else
        // Get latest chunk, copy into next spot in model array
        memcpy(&mut_model[model_chunk_offset], in->buffer.data, in->buffer.dataLength);
        model_chunk_offset += in->buffer.dataLength;
    #endif
    return ns_rpc_data_success;
}

status decodeIncomingSendblock(const dataBlock *in) {
    if (in->cmd == 0) {
        return configureModel(in);
    } else if (in->cmd == 4) {
        return incomingTensorChunk(in);
    }
    else {
        return incomingModelChunk(in);
    }
}

// Handler for fetchBlockFromEVB, invoked by PC
/**
 * @brief Get the statistics about the last Invoke()
 *
 * @param block - serialized tflm_stats_struct
 * @return status
 */
status getStatistics(dataBlock *res) {

    uint32_t statSize = sizeof(mut_stats.bytes) * sizeof(uint8_t);
    uint32_t bufSize = (statSize <= (TFLM_VALIDATOR_RX_BUFSIZE - 1000))
                           ? statSize
                           : (TFLM_VALIDATOR_RX_BUFSIZE - 1000);

    ns_lp_printf(
        "[INFO] Server asked for statistics (%d bytes), send back %d bytes\n", statSize, bufSize);

    uint8_t *resultBuffer = (uint8_t *)ns_malloc(bufSize);
    char *msg_store = (char *)ns_malloc(sizeof(char) * 30);
    res->length = bufSize;
    res->dType = uint8_e;
    res->description = msg_store;
    res->cmd = generic_cmd;
    binary_t binaryBlock = {.data = (uint8_t *)resultBuffer, .dataLength = bufSize};
    res->buffer = binaryBlock;
    mut_stats.stats.computed_stat_buffer_size = sizeof(mut_stats.bytes);
    mut_stats.stats.computed_stat_per_event_size = sizeof(ns_profiler_event_stats_t);
#ifdef NS_TFLM_VALIDATOR
    mut_stats.stats.captured_events = ns_microProfilerSidecar.captured_event_num;
    memcpy(
        mut_stats.stats.stat_buffer, ns_profiler_events_stats, sizeof(mut_stats.stats.stat_buffer));
#else
    mut_stats.stats.captured_events = 0;
#endif

    memcpy(resultBuffer, mut_stats.bytes, bufSize);

    if (bufSize == statSize) {
        stats_offset = 0;
        stats_is_chunked = false;
        stats_remaining = 0;
        char msg[] = "FullStats\0";
        memcpy(msg_store, msg, sizeof(msg));
    } else {
        // We have to chunk the response
        stats_offset = bufSize;
        stats_remaining = statSize - stats_offset;
        stats_is_chunked = true;
        char msg[] = "PartStats\0";
        memcpy(msg_store, msg, sizeof(msg));
    }
    ns_lp_printf(
        "[INFO] Sending stats chunk, sr %d, cs %d, so %d\n", stats_remaining, bufSize,
        stats_offset);
    ns_lp_printf(
        "[INFO] size of rx buffer %d, and tx buffer is %d\n", TFLM_VALIDATOR_RX_BUFSIZE,
        TFLM_VALIDATOR_TX_BUFSIZE);
    return ns_rpc_data_success;
}

status getStatChunk(dataBlock *res) {
    // Return a chunk of stats

    uint32_t chunkSize = (stats_remaining <= (TFLM_VALIDATOR_RX_BUFSIZE - 1000))
                             ? stats_remaining
                             : (TFLM_VALIDATOR_RX_BUFSIZE - 1000);

    ns_lp_printf(
        "[INFO] Server asked for stats chunk, stats remaining %d, offset %d, sending %d\n",
        stats_remaining, stats_offset, chunkSize);

    uint8_t *resultBuffer = (uint8_t *)ns_malloc(chunkSize);
    char *msg_store = (char *)ns_malloc(sizeof(char) * 30);
    res->length = sizeof(mut_stats.bytes) * sizeof(uint8_t);
    res->dType = uint8_e;
    res->description = msg_store;
    res->cmd = write_cmd;
    binary_t binaryBlock = {.data = (uint8_t *)resultBuffer, .dataLength = chunkSize};
    res->buffer = binaryBlock;

    memcpy(resultBuffer, mut_stats.bytes + stats_offset, chunkSize);
    stats_remaining = stats_remaining - chunkSize;
    if (stats_remaining == 0) {
        stats_is_chunked = false;
        stats_offset = 0;
        char msg[] = "LastStats\0";
        ns_lp_printf("[INFO] Last stats chunk\n");
        memcpy(msg_store, msg, sizeof(msg));
    } else {
        stats_offset = stats_offset + chunkSize;
        char msg[] = "PartStats\0";
        ns_lp_printf(
            "[INFO] Part stats chunk sr %d, cs %d, so %d\n", stats_remaining, chunkSize,
            stats_offset);
        memcpy(msg_store, msg, sizeof(msg));
    }

    return ns_rpc_data_success;
}

status decodeIncomingFetchblock(dataBlock *ret) {
    if (stats_is_chunked == false) {
        return getStatistics(ret);
    } else {
        return getStatChunk(ret);
    }
}

// Handler for computeOnEVB, invoked by PC
/**
 * @brief Calls TFLM's invoke()
 *
 * @param in  - input tensor
 * @param res - output tensor
 * @return status - fail if not configured or if invoke fails
 */
uint32_t output_tensor_chunk_offset = 0;
uint32_t totalSize = 0;
uint32_t remaining = 0;
AM_SHARED_RW uint8_t output_tensor_buffer[200000];
binary_t binaryBlock;

status infer_on_tflm(const dataBlock *in, dataBlock *res) {
    // Prep the return block, needs to happen whether errors occur or not
     uint32_t outputSize;
    char msg_full[] = "FullTensor\0";
    char msg_part[] = "PartTensor\0";
    char msg_last[] = "LastTensor\0";
    char error_msg[] = "Invoke failed\0";

    uint8_t *resultBuffer;
    char *msg_store;

    // If the output tensor is greater than 3000, we need to chunk it
    if (mut_cfg.config.output_length < (TFLM_VALIDATOR_RX_BUFSIZE - 1000)) {
        // ns_lp_printf("[INFO] Output tensor is small enough to fit in one block\n");
        output_tensor_is_chunked = false;
        outputSize = mut_cfg.config.output_length;
    } else {
        // ns_lp_printf("[INFO] Output tensor is too large, will chunk\n");
        output_tensor_is_chunked = true;
        outputSize = TFLM_VALIDATOR_RX_BUFSIZE - 1000;
    }

    // Check the command value and if it is 'write' then send an output tensor chunk
    msg_store = (char *)ns_malloc(sizeof(char) * 30);

    if (in->cmd == write_cmd) {
        if (remaining >= TFLM_VALIDATOR_RX_BUFSIZE - 1000) {
            // Send the maximum chunk size
            outputSize = TFLM_VALIDATOR_RX_BUFSIZE - 1000;
        } else {
            // Send the remaining chunk
            outputSize = remaining;
        }
        remaining = remaining - outputSize;
        // ns_lp_printf("[INFO] PC requested input tensor chunk, trying to malloc %d\n", outputSize);
        resultBuffer = (uint8_t *)ns_malloc(outputSize);
        memcpy(resultBuffer, output_tensor_buffer + output_tensor_chunk_offset, outputSize);
        output_tensor_chunk_offset = output_tensor_chunk_offset + outputSize;
        res->description = msg_store;
        res->length = outputSize;
        res->dType = uint8_e;
        if (remaining > 0) {
            memcpy(msg_store, msg_part, sizeof(msg_part));
        } else {
            memcpy(msg_store, msg_last, sizeof(msg_last));
        }
        res->cmd = write_cmd;

        binaryBlock = {
            .data = (uint8_t *)resultBuffer,
            .dataLength = outputSize};
        res->buffer = binaryBlock;
        memcpy(resultBuffer, in->buffer.data, in->buffer.dataLength);
        return ns_rpc_data_success;
    }
    else {
        if (output_tensor_is_chunked) {
            memcpy(msg_store, msg_part, sizeof(msg_part));
        }
        else {
            memcpy(msg_store, msg_full, sizeof(msg_full));
        }
    }

    // Prep the return block, needs to happen whether errors occur or not
    // ns_lp_printf("[INFO] PC requested inference, trying to malloc %d\n", mut_cfg.config.output_length);
    resultBuffer = (uint8_t *)ns_malloc(outputSize);
    res->length = outputSize;
    res->dType = uint8_e;
    res->description = msg_store;
    res->cmd = generic_cmd;
    binaryBlock = {
        .data = (uint8_t *)resultBuffer,
        .dataLength = outputSize};
    res->buffer = binaryBlock;

    // 'in' contains the input tensors, treat as homogeneous block
    if (input_tensor_is_chunked == false) {
        memcpy(tflm.model_input[0]->data.int8, in->buffer.data, in->buffer.dataLength);
    } // else it is already in the input tensor

    ns_lp_printf("[INFO] Invoking model with %d byte tensor\n", in->buffer.dataLength);

    // Print first 10 mac estimates
    // for (int i = 0; i < 10; i++) {
    //     ns_lp_printf("Before MAC estimate %d: %d %d @ 0x%x\n", i, tflm.mac_estimates->mac_count_map[i], ns_microProfilerSidecar.mac_count_map[i], ns_microProfilerSidecar.mac_count_map);
    // }

    TfLiteStatus invoke_status = tflm.interpreter->Invoke();

    // for (int i = 0; i < 10; i++) {
    //     ns_lp_printf("after invoke MAC estimate %d: %d %d @ 0x%x\n", i, tflm.mac_estimates->mac_count_map[i], ns_microProfilerSidecar.mac_count_map[i], ns_microProfilerSidecar.mac_count_map);
    // }

    if (invoke_status != kTfLiteOk) {
        ns_lp_printf("Invoke failed\n");
        memcpy(msg_store, error_msg, sizeof(error_msg));
        return ns_rpc_data_failure;
    }
    ns_lp_printf("[INFO] Invoke successful\n");

    if ((mut_cfg.config.profile_mut == 1) && (invokes_so_far == mut_cfg.config.profile_warmup)) {
        ns_lp_printf(
            "[INFO] requested warmup %d,  invokes_so_far %d", mut_cfg.config.profile_warmup,
            invokes_so_far);
        tflm.profiler->LogCsv(); // prints and also captures events in a buffer
        ns_stop_perf_profiler();
        // ns_lp_printf("[INFO] after profiler successful\n");
    }
    // Prep the return block with output tensor
    // If the output tensor is chunked, we need to copy it to holding buffer
    // and return the first chunk

    // Calculate the total size
    for (uint32_t t = 0; t < mut_cfg.config.num_output_tensors; t++) {
        totalSize += outputTensorDetails[t].details.tensorSizeBytes;
    }

    // If the output tensor is too big, we need to copy it to holding buffer
    uint8_t *destination = totalSize < (TFLM_VALIDATOR_RX_BUFSIZE - 1000) ? resultBuffer : output_tensor_buffer;

    int offset = 0;
    for (uint32_t t = 0; t < mut_cfg.config.num_output_tensors; t++) {
        memcpy(
            destination + offset, tflm.model_output[t]->data.int8,
            outputTensorDetails[t].details.tensorSizeBytes);
        offset += outputTensorDetails[t].details.tensorSizeBytes;
    }

    // If the output tensor is too big, put the first chunk in the return block
    if (totalSize > (TFLM_VALIDATOR_RX_BUFSIZE - 1000)) {
        memcpy(resultBuffer, destination, outputSize);
        output_tensor_chunk_offset = outputSize;
        remaining = totalSize - outputSize;
    }

    // ns_lp_printf("[INFO] output memcpy successful\n");
    // char res_msg[] = "Invoke Successful!\0";
    ns_lp_printf(".");
    invokes_so_far++;
    input_tensor_offset = 0;
    return ns_rpc_data_success;
}

void ns_preAction(void) { ns_lp_printf("Starting action\n"); }

void ns_postAction(void) { ns_lp_printf("Stopping action\n"); }

uint8_t tflm_v_cdc_rx_ff_buf[TFLM_VALIDATOR_RX_BUFSIZE];
uint8_t tlfm_v_cdc_tx_ff_buf[TFLM_VALIDATOR_TX_BUFSIZE];

int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};

    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\b");
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed\n");
#ifdef NS_MLPROFILE
    basic_tickTimer.api = &ns_timer_V1_0_0;
    basic_tickTimer.timer = NS_TIMER_COUNTER;
    basic_tickTimer.enableInterrupt = false;
    NS_TRY(ns_timer_init(&basic_tickTimer), "Timer init failed.\n");
#endif
    ns_itm_printf_enable();

    ns_interrupt_master_enable();

    // Add callbacks to handle incoming requests
    ns_rpc_config_t rpcConfig = {
        .api = &ns_rpc_gdo_V1_0_0,
        .mode = NS_RPC_GENERICDATA_SERVER, // Puts EVB in RPC server mode
        .rx_buf = tflm_v_cdc_rx_ff_buf,
        .rx_bufLength = TFLM_VALIDATOR_RX_BUFSIZE,
        .tx_buf = tlfm_v_cdc_tx_ff_buf,
        .tx_bufLength = TFLM_VALIDATOR_TX_BUFSIZE,
        .sendBlockToEVB_cb = decodeIncomingSendblock,
        .fetchBlockFromEVB_cb = decodeIncomingFetchblock,
        .computeOnEVB_cb = infer_on_tflm};
    NS_TRY(ns_rpc_genericDataOperations_init(&rpcConfig), "RPC Init Failed\n");

    // Add some pre/post callbacks
    // erpc_server_add_pre_cb_action(&ns_preAction);
    // erpc_server_add_post_cb_action(&ns_postAction);

    ns_lp_printf("Ready to receive RPC Calls\n");
    ns_lp_printf(
        "Debug kv %d, va %d, mrv %d\n", kVarArenaSize, sizeof(var_arena),
        sizeof(tflite::MicroResourceVariables));
    while (1) {
        ns_rpc_genericDataOperations_pollServer(&rpcConfig);
        ns_delay_us(1000);
        // ns_deep_sleep();
    }
}
