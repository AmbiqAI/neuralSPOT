/**
 * @file <model_power>_model.c (generated from template.cc)
 * @author autodeploy.py
 * @brief Generated automatically from template code by NS autodeploy script
 * @version 0.1
 * @date 2023-03-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "ns_model.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_debug_log.h"
#include "model_power_api.h"
#include "model_power_model.h"
#include "model_power_model_data.h"
#include "ns_ambiqsuite_harness.h"
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

// static constexpr int model_power_tensor_arena_size = 1024 * model_power_COMPUTED_ARENA_SIZE;



#if (model_power_MODEL_LOCATION == NS_AD_SRAM)
    AM_SHARED_RW alignas(16) static unsigned char model_power_model[model_power_model_for_sram_LEN];
#endif

static constexpr int model_power_tensor_arena_size = 1024 * model_power_COMPUTED_ARENA_SIZE;
#ifdef AM_PART_APOLLO3
    // Apollo3 doesn't have AM_SHARED_RW
    alignas(16) static uint8_t model_power_tensor_arena[model_power_tensor_arena_size];
#else // not AM_PART_APOLLO3
    #if (model_power_ARENA_LOCATION == NS_AD_SRAM)
        AM_SHARED_RW alignas(16) static uint8_t model_power_tensor_arena[model_power_tensor_arena_size];
    #else
        alignas(16) static uint8_t model_power_tensor_arena[model_power_tensor_arena_size];
    #endif
#endif

// alignas(16) static uint8_t model_power_tensor_arena[model_power_tensor_arena_size];

// Resource Variable Arena
static constexpr int model_power_resource_var_arena_size =
    4 * (0 + 1) * sizeof(tflite::MicroResourceVariables);
alignas(16) static uint8_t model_power_var_arena[model_power_resource_var_arena_size];

#ifdef NS_MLPROFILE
// Timer is used for TF profiling
ns_timer_config_t basic_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};
uint32_t model_power_mac_estimates[13] = {320000, 72000, 512000, 72000, 512000, 72000, 512000, 72000, 512000, 0, 0, 768, 0};

#endif

int model_power_init(ns_model_state_t *ms);

int model_power_minimal_init(ns_model_state_t *ms) {

    ms->runtime = TFLM;
    // ms->model_array = model_power_model;
    ms->arena = model_power_tensor_arena;
    ms->arena_size = model_power_tensor_arena_size;
    ms->rv_arena = model_power_var_arena;
    ms->rv_arena_size = model_power_resource_var_arena_size;
    ms->rv_count = 0;
    ms->numInputTensors = 1;
    ms->numOutputTensors = 1;

#if (model_power_MODEL_LOCATION == NS_AD_SRAM)
    // Copy to SRAM
    memcpy(model_power_model, model_power_model_for_sram, model_power_model_for_sram_len);
    ms->model_array = model_power_model;
#else
    ms->model_array = model_power_model;
#endif

#ifdef NS_MLPROFILE
    ns_perf_mac_count_t basic_mac = {
        .number_of_layers = 13, .mac_count_map = model_power_mac_estimates};
    ms->tickTimer = &basic_tickTimer;
    ms->mac_estimates = &basic_mac;
#else
    ms->tickTimer = NULL;
    ms->mac_estimates = NULL;
#endif
    int status = model_power_init(ms);
    return status;
}

int model_power_init(ns_model_state_t *ms) {
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
        return model_power_STATUS_FAILURE;
    }

#ifdef NS_TFSTRUCTURE_RECENT
    static tflite::MicroMutableOpResolver<6> resolver;
#else
    static tflite::MicroMutableOpResolver<6> resolver(error_reporter);
#endif
    resolver.AddConv2D();
resolver.AddDepthwiseConv2D();
resolver.AddAveragePool2D();
resolver.AddReshape();
resolver.AddFullyConnected();
resolver.AddSoftmax();


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
        return model_power_STATUS_FAILURE;
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
    return model_power_STATUS_SUCCESS;
}
