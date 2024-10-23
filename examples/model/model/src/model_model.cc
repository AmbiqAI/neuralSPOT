/**
 * @file <model>_model.c (generated from template.cc)
 * @author autodeploy.py
 * @brief Generated automatically from template code by NS autodeploy script
 * @version 0.1
 * @date 2023-03-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "model_api.h"
#include "model_model.h"
#include "model_model_data.h"
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

static constexpr int model_tensor_arena_size = 1024 * model_COMPUTED_ARENA_SIZE;
alignas(16) static uint8_t model_tensor_arena[model_tensor_arena_size];

// Resource Variable Arena
static constexpr int model_resource_var_arena_size =
    4 * (0 + 1) * sizeof(tflite::MicroResourceVariables);
alignas(16) static uint8_t model_var_arena[model_resource_var_arena_size];

#ifdef NS_MLPROFILE
// Timer is used for TF profiling
ns_timer_config_t basic_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};
uint32_t model_mac_estimates[NS_AD_MAC_ESTIMATE_COUNT] = {NS_AD_MAC_ESTIMATE_LIST};

#endif

int model_init(ns_model_state_t *ms);

int model_minimal_init(ns_model_state_t *ms) {
    ms->runtime = TFLM;
    ms->model_array = model_model;
    ms->arena = model_tensor_arena;
    ms->arena_size = model_tensor_arena_size;
    ms->rv_arena = model_var_arena;
    ms->rv_arena_size = model_resource_var_arena_size;
    ms->rv_count = 0;
    ms->numInputTensors = 1;
    ms->numOutputTensors = 1;

#ifdef NS_MLPROFILE
    ns_perf_mac_count_t basic_mac = {
        .number_of_layers = NS_AD_MAC_ESTIMATE_COUNT, .mac_count_map = model_mac_estimates};
    ms->tickTimer = &basic_tickTimer;
    ms->mac_estimates = &basic_mac;
#else
    ms->tickTimer = NULL;
    ms->mac_estimates = NULL;
#endif

    int status = model_init(ms);
    return status;
}

int model_init(ns_model_state_t *ms) {
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
        return model_STATUS_FAILURE;
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
        return model_STATUS_FAILURE;
    }

    // Obtain pointers to the model's input and output tensors.
    for (uint32_t t = 0; t <= ms->numInputTensors; t++) {
        ms->model_input[t] = ms->interpreter->input(t);
    }

    for (uint32_t t = 0; t <= ms->numOutputTensors; t++) {
        ms->model_output[t] = ms->interpreter->output(t);
    }

    ms->state = READY;
    return model_STATUS_SUCCESS;
}
