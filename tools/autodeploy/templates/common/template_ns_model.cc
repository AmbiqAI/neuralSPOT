// /**
//  * @file Template for generic_model.h
//  * @author Carlos Morales (carlos.morales@ambiq.com)
//  * @brief Generic TF Model wrapper
//  * @version 0.1
//  * @date 2023-3-08
//  *
//  * @copyright Copyright (c) 2023
//  *
//  */

// // NS includes
// // #include "ns_model.h"
// // #include "ns_ambiqsuite_harness.h"
// // #include "ns_debug_log.h"
// #include "NS_AD_NAME_api.h"
// #include "NS_AD_NAME_model.h"
// #include "ns_ambiqsuite_harness.h"
// #include "ns_core.h"
// #include "NS_AD_NAME_model_data.h"
// #include "ns_model.h"
// // Tensorflow Lite for Microcontroller includes (somewhat boilerplate)
// // #include "tensorflow/lite/micro/all_ops_resolver.h"
// #include "tensorflow/lite/micro/kernels/micro_ops.h"
// #include "tensorflow/lite/micro/micro_interpreter.h"
// #include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
// #include "tensorflow/lite/micro/micro_profiler.h"
// #include "tensorflow/lite/micro/system_setup.h"
// #include "tensorflow/lite/schema/schema_generated.h"

// #ifdef NS_TFSTRUCTURE_RECENT
//     #include "tensorflow/lite/micro/tflite_bridge/micro_error_reporter.h"
// #else
//     #include "tensorflow/lite/micro/micro_error_reporter.h"
// #endif

// #if (NS_AD_NAME_MODEL_LOCATION == NS_AD_SRAM)
//     AM_SHARED_RW alignas(16) static unsigned char NS_AD_NAME_model[NS_AD_NAME_model_for_sram_LEN];
// #endif

// static constexpr int NS_AD_NAME_tensor_arena_size = 1024 * NS_AD_NAME_COMPUTED_ARENA_SIZE;
// #ifdef AM_PART_APOLLO3
//     // Apollo3 doesn't have AM_SHARED_RW
//     alignas(16) static uint8_t NS_AD_NAME_tensor_arena[NS_AD_NAME_tensor_arena_size];
// #else // not AM_PART_APOLLO3
//     #if (NS_AD_NAME_ARENA_LOCATION == NS_AD_SRAM)
//         AM_SHARED_RW alignas(16) static uint8_t NS_AD_NAME_tensor_arena[NS_AD_NAME_tensor_arena_size];
//     #else
//         alignas(16) static uint8_t NS_AD_NAME_tensor_arena[NS_AD_NAME_tensor_arena_size];
//     #endif
// #endif

// // Resource Variable Arena
// static constexpr int NS_AD_NAME_resource_var_arena_size =
//     4 * (NS_AD_RV_COUNT + 1) * sizeof(tflite::MicroResourceVariables);
// alignas(16) static uint8_t NS_AD_NAME_var_arena[NS_AD_NAME_resource_var_arena_size];

// #ifdef NS_MLPROFILE
// // Timer is used for TF profiling
// ns_timer_config_t basic_tickTimer = {
//     .api = &ns_timer_V1_0_0,
//     .timer = NS_TIMER_COUNTER,
//     .enableInterrupt = false,
// };
// uint32_t NS_AD_NAME_mac_estimates[NS_AD_MAC_ESTIMATE_COUNT] = {NS_AD_MAC_ESTIMATE_LIST};
// #endif

// int NS_AD_NAME_init(ns_model_state_t *ms);

// int NS_AD_NAME_minimal_init(ns_model_state_t *ms) {
//     ms->runtime = TFLM;
//     // ms->model_array = NS_AD_NAME_model;
//     ms->arena = NS_AD_NAME_tensor_arena;
//     ms->arena_size = NS_AD_NAME_tensor_arena_size;
//     ms->rv_arena = NS_AD_NAME_var_arena;
//     ms->rv_arena_size = NS_AD_NAME_resource_var_arena_size;
//     ms->rv_count = NS_AD_RV_COUNT;
//     ms->numInputTensors = NS_AD_NUM_INPUT_VECTORS;
//     ms->numOutputTensors = NS_AD_NUM_OUTPUT_VECTORS;

// #if (NS_AD_NAME_MODEL_LOCATION == NS_AD_SRAM)
//     // Copy to SRAM
//     memcpy(NS_AD_NAME_model, NS_AD_NAME_model_for_sram, NS_AD_NAME_model_for_sram_len);
//     ms->model_array = NS_AD_NAME_model;
// #else
//     ms->model_array = NS_AD_NAME_model;
// #endif

// #ifdef NS_MLPROFILE
//     ns_perf_mac_count_t basic_mac = {
//         .number_of_layers = NS_AD_MAC_ESTIMATE_COUNT, .mac_count_map = NS_AD_NAME_mac_estimates};
//     ms->tickTimer = &basic_tickTimer;
//     ms->mac_estimates = &basic_mac;
// #else
//     ms->tickTimer = NULL;
//     ms->mac_estimates = NULL;
// #endif

//     int status = NS_AD_NAME_init(ms);
//     return status;
// }
// /**
//  * @brief Initialize TF with model
//  *
//  * This code is fairly common across most TF-based models.
//  * The major differences relate to input and output tensor
//  * handling.
//  *
//  */
// int NS_AD_NAME_model_init(ns_model_state_t *ms) {
//     ms->state = NOT_READY;

//     tflite::MicroErrorReporter micro_error_reporter;
//     ms->error_reporter = &micro_error_reporter;

// #ifdef NS_MLPROFILE
//     // Need a timer for the profiler to collect latencies
//     NS_TRY(ns_timer_init(ms->tickTimer), "Timer init failed.\n");
//     static tflite::MicroProfiler micro_profiler;
//     ms->profiler = &micro_profiler;

//     ns_TFDebugLogInit(ms->tickTimer, ms->mac_estimates);

// #else
//     #ifdef NS_MLDEBUG
//     ns_TFDebugLogInit(NULL, NULL);
//     #endif
// #endif
//     ns_lp_printf("Initializing model...\n");
//     tflite::InitializeTarget();
//     ns_lp_printf("Target initialized.\n");
//     // Map the model into a usable data structure. This doesn't involve any
//     // copying or parsing, it's a very lightweight operation.
//     ms->model = tflite::GetModel(ms->model_array);
//     if (ms->model->version() != TFLITE_SCHEMA_VERSION) {
//         TF_LITE_REPORT_ERROR(
//             ms->error_reporter,
//             "Model provided is schema version %d not equal "
//             "to supported version %d.",
//             ms->model->version(), TFLITE_SCHEMA_VERSION);
//         return NS_STATUS_FAILURE;
//     }
//     ns_lp_printf("Model mapped.\n");
// #ifdef NS_TFSTRUCTURE_RECENT
//     static tflite::MicroMutableOpResolver<NS_AD_NUM_OPS> resolver;
// #else
//     static tflite::MicroMutableOpResolver<NS_AD_NUM_OPS> resolver(ms->error_reporter);
// #endif
//     // NS_AD_RESOLVER_ADDS

//     // Allocate ResourceVariable stuff if needed
//     tflite::MicroResourceVariables *resource_variables;
//     tflite::MicroAllocator *var_allocator;
//     ns_lp_printf("Allocating resource variables...\n");
//     if (ms->rv_count != 0) {
//         var_allocator = tflite::MicroAllocator::Create(ms->rv_arena, ms->rv_arena_size, nullptr);
//         resource_variables = tflite::MicroResourceVariables::Create(var_allocator, ms->rv_count);
//     } else {
//         resource_variables = nullptr;
//     }
//     ns_lp_printf("Resource variables allocated.\n");
//     // Build an interpreter to run the model with.
// #ifdef NS_TFSTRUCTURE_RECENT
//     static tflite::MicroInterpreter static_interpreter(
//         ms->model, resolver, ms->arena, ms->arena_size, resource_variables, ms->profiler);
// #else
//     static tflite::MicroInterpreter static_interpreter(
//         ms->model, resolver, ms->arena, ms->arena_size, ms->error_reporter, resource_variables,
//         ms->profiler);
// #endif
//     ms->interpreter = &static_interpreter;
//     ns_lp_printf("Interpreter built.\n");
//     // Allocate memory from the tensor_arena for the model's tensors.
//     TfLiteStatus allocate_status = ms->interpreter->AllocateTensors();
//     ns_lp_printf("Tensors allocated.\n");
//     if (allocate_status != kTfLiteOk) {
//         TF_LITE_REPORT_ERROR(ms->error_reporter, "AllocateTensors() failed");
//         ms->computed_arena_size = 0xDEADBEEF;
//         return NS_STATUS_FAILURE;
//     }

//     ms->computed_arena_size = ms->interpreter->arena_used_bytes(); // prep to send back to PC
//     ns_lp_printf("Arena size computed., %d\n", ms->computed_arena_size);
//     // Obtain pointers to the model's input and output tensors.
//     for (uint32_t t = 0; t < ms->numInputTensors; t++) {
//         ms->model_input[t] = ms->interpreter->input(t);
//     }

//     for (uint32_t t = 0; t < ms->numOutputTensors; t++) {
//         ms->model_output[t] = ms->interpreter->output(t);
//     }

//     ms->state = READY;
//     return NS_STATUS_SUCCESS;
// }
/**
 * @file <NS_AD_NAME>_model.c (generated from template.cc)
 * @author autodeploy.py
 * @brief Generated automatically from template code by NS autodeploy script
 * @version 0.1
 * @date 2023-03-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "NS_AD_NAME_api.h"
#include "NS_AD_NAME_model.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_core.h"
#if (NS_AD_NAME_MODEL_LOCATION == NS_AD_PSRAM)
    #include "ns_peripherals_psram.h"
#endif
#include "NS_AD_NAME_model_data.h"
#include "ns_model.h"

// Tensorflow Lite for Microcontroller includes (somewhat boilerplate)
// #include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_profiler.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

#ifdef NS_TFSTRUCTURE_RECENT
    #include "tensorflow/lite/micro/tflite_bridge/micro_error_reporter.h"
#else
    #include "tensorflow/lite/micro/micro_error_reporter.h"
#endif

// static constexpr int NS_AD_NAME_tensor_arena_size = 1024 * NS_AD_NAME_COMPUTED_ARENA_SIZE;


#if (NS_AD_NAME_MODEL_LOCATION == NS_AD_PSRAM)
    unsigned char *NS_AD_NAME_model_psram;
#elif (NS_AD_NAME_MODEL_LOCATION == NS_AD_SRAM)
    AM_SHARED_RW alignas(16) static unsigned char NS_AD_NAME_model[NS_AD_NAME_model_for_sram_LEN];
#endif

#if (NS_AD_NAME_ARENA_LOCATION == NS_AD_PSRAM)
    static uint8_t *NS_AD_NAME_tensor_arena;
    static constexpr int NS_AD_NAME_tensor_arena_size = 1024 * 1024 * 10; // 10MB
#else
    static constexpr int NS_AD_NAME_tensor_arena_size = 1024 * NS_AD_NAME_COMPUTED_ARENA_SIZE;
    #ifdef AM_PART_APOLLO3
        // Apollo3 doesn't have AM_SHARED_RW
        alignas(16) static uint8_t NS_AD_NAME_tensor_arena[NS_AD_NAME_tensor_arena_size];
    #else // not AM_PART_APOLLO3
        #if (NS_AD_NAME_ARENA_LOCATION == NS_AD_SRAM)
            AM_SHARED_RW alignas(16) static uint8_t NS_AD_NAME_tensor_arena[NS_AD_NAME_tensor_arena_size];
        #else
            alignas(16) static uint8_t NS_AD_NAME_tensor_arena[NS_AD_NAME_tensor_arena_size];
        #endif
    #endif
#endif

// alignas(16) static uint8_t NS_AD_NAME_tensor_arena[NS_AD_NAME_tensor_arena_size];

// Resource Variable Arena
static constexpr int NS_AD_NAME_resource_var_arena_size =
    4 * (NS_AD_RV_COUNT + 1) * sizeof(tflite::MicroResourceVariables);
alignas(16) static uint8_t NS_AD_NAME_var_arena[NS_AD_NAME_resource_var_arena_size];

#ifdef NS_MLPROFILE
// Timer is used for TF profiling
ns_timer_config_t basic_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};
uint32_t NS_AD_NAME_mac_estimates[NS_AD_MAC_ESTIMATE_COUNT] = {NS_AD_MAC_ESTIMATE_LIST};

#endif

#if (NS_AD_NAME_MODEL_LOCATION == NS_AD_PSRAM) or (NS_AD_NAME_ARENA_LOCATION == NS_AD_PSRAM)
extern ns_psram_config_t psram_cfg;
#endif

int NS_AD_NAME_init(ns_model_state_t *ms);

int NS_AD_NAME_minimal_init(ns_model_state_t *ms) {

#if (NS_AD_NAME_ARENA_LOCATION == NS_AD_PSRAM)
    NS_AD_NAME_tensor_arena = (uint8_t *)(psram_cfg.psram_base_address + 20*1024*1024);
#endif
    ms->runtime = TFLM;
    // ms->model_array = NS_AD_NAME_model;
    ms->arena = NS_AD_NAME_tensor_arena;
    ms->arena_size = NS_AD_NAME_tensor_arena_size;
    ms->rv_arena = NS_AD_NAME_var_arena;
    ms->rv_arena_size = NS_AD_NAME_resource_var_arena_size;
    ms->rv_count = NS_AD_RV_COUNT;
    ms->numInputTensors = NS_AD_NUM_INPUT_VECTORS;
    ms->numOutputTensors = NS_AD_NUM_OUTPUT_VECTORS;

#if (NS_AD_NAME_MODEL_LOCATION == NS_AD_PSRAM)
    // Copy model to PSRAM
    NS_AD_NAME_model_psram = (unsigned char *)(psram_cfg.psram_base_address);
    memcpy((unsigned char *)(psram_cfg.psram_base_address), NS_AD_NAME_model, NS_AD_NAME_model_len);
    ms->model_array = NS_AD_NAME_model_psram;
#elif (NS_AD_NAME_MODEL_LOCATION == NS_AD_SRAM)
    // Copy to SRAM
    memcpy(NS_AD_NAME_model, NS_AD_NAME_model_for_sram, NS_AD_NAME_model_for_sram_len);
    ms->model_array = NS_AD_NAME_model;
#else
    ms->model_array = NS_AD_NAME_model;
#endif

#ifdef NS_MLPROFILE
    ns_perf_mac_count_t basic_mac = {
        .number_of_layers = NS_AD_MAC_ESTIMATE_COUNT, .mac_count_map = NS_AD_NAME_mac_estimates};
    ms->tickTimer = &basic_tickTimer;
    ms->mac_estimates = &basic_mac;
#else
    ms->tickTimer = NULL;
    ms->mac_estimates = NULL;
#endif

    int status = NS_AD_NAME_init(ms);
    return status;
}

int NS_AD_NAME_init(ns_model_state_t *ms) {
    ms->state = NOT_READY;

    tflite::MicroErrorReporter micro_error_reporter;
    ms->error_reporter = &micro_error_reporter;

#ifdef NS_MLPROFILE
    // Need a timer for the profiler to collect latencies
    NS_TRY(ns_timer_init(ms->tickTimer), "Timer init failed.\n");
    static tflite::MicroProfiler micro_profiler;
    ms->profiler = &micro_profiler;
    ns_TFDebugLogInit(ms->tickTimer, ms->mac_estimates);
#else
    #ifdef NS_MLDEBUG
    ns_TFDebugLogInit(NULL, NULL);
    #endif
#endif

    tflite::InitializeTarget();

    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    ms->model = tflite::GetModel(ms->model_array);
    if (ms->model->version() != TFLITE_SCHEMA_VERSION) {
        TF_LITE_REPORT_ERROR(
            ms->error_reporter,
            "Model provided is schema version %d not equal "
            "to supported version %d.",
            ms->model->version(), TFLITE_SCHEMA_VERSION);
        return NS_AD_NAME_STATUS_FAILURE;
    }

#ifdef NS_TFSTRUCTURE_RECENT
    static tflite::MicroMutableOpResolver<NS_AD_NUM_OPS> resolver;
#else
    static tflite::MicroMutableOpResolver<NS_AD_NUM_OPS> resolver(error_reporter);
#endif
    NS_AD_RESOLVER_ADDS

    // Allocate ResourceVariable stuff if needed
    tflite::MicroResourceVariables *resource_variables;
    tflite::MicroAllocator *var_allocator;

    if (ms->rv_count != 0) {
        var_allocator = tflite::MicroAllocator::Create(ms->rv_arena, ms->rv_arena_size, nullptr);
        resource_variables = tflite::MicroResourceVariables::Create(var_allocator, ms->rv_count);
    } else {
        resource_variables = nullptr;
    }

    // Build an interpreter to run the model with.
#ifdef NS_TFSTRUCTURE_RECENT
    static tflite::MicroInterpreter static_interpreter(
        ms->model, resolver, ms->arena, ms->arena_size, resource_variables, ms->profiler);
#else
    static tflite::MicroInterpreter static_interpreter(
        ms->model, resolver, ms->arena, ms->arena_size, ms->error_reporter, nullptr, ms->profiler);
#endif
    ms->interpreter = &static_interpreter;

    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = ms->interpreter->AllocateTensors();

    if (allocate_status != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(ms->error_reporter, "AllocateTensors() failed");
        return NS_AD_NAME_STATUS_FAILURE;
    }
    ms->computed_arena_size = ms->interpreter->arena_used_bytes(); // prep to send back to PC
    #ifdef NS_MLPROFILE
    ns_lp_printf("Arena size computed., %d\n", ms->computed_arena_size);
    #endif
    // Obtain pointers to the model's input and output tensors.
    for (uint32_t t = 0; t < ms->numInputTensors; t++) {
        ms->model_input[t] = ms->interpreter->input(t);
    }

    for (uint32_t t = 0; t < ms->numOutputTensors; t++) {
        ms->model_output[t] = ms->interpreter->output(t);
    }

    ms->state = READY;
    return NS_AD_NAME_STATUS_SUCCESS;
}