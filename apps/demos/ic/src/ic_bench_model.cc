/**
 * @file <ic_bench>_model.c (generated from template.cc)
 * @author autodeploy.py
 * @brief Generated automatically from template code by NS autodeploy script
 * @version 0.1
 * @date 2023-03-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "ic_bench_api.h"
#include "ic_bench_model.h"
#include "ic_bench_model_data.h"
#include "ns_model.h"
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

static constexpr int ic_bench_tensor_arena_size = 1024 * ic_bench_COMPUTED_ARENA_SIZE;
alignas(16) static uint8_t ic_bench_tensor_arena[ic_bench_tensor_arena_size];

// Resource Variable Arena
static constexpr int ic_bench_resource_var_arena_size =
    4 * (0 + 1) * sizeof(tflite::MicroResourceVariables);
alignas(16) static uint8_t ic_bench_var_arena[ic_bench_resource_var_arena_size];

int ic_bench_init(ns_model_state_t *ms);

int ic_bench_minimal_init(ns_model_state_t *ms) {
    ms->runtime = TFLM;
    ms->model_array = ic_bench_model;
    ms->arena = ic_bench_tensor_arena;
    ms->arena_size = ic_bench_tensor_arena_size;
    ms->rv_arena = ic_bench_var_arena;
    ms->rv_arena_size = ic_bench_resource_var_arena_size;
    ms->rv_count = 0;
    ms->numInputTensors = 1;
    ms->numOutputTensors = 1;

    ms->tickTimer = NULL;
    ms->mac_estimate = NULL;

    int status = ic_bench_init(ms);
    return status;
}

int ic_bench_init(ns_model_state_t *ms) {
    ms->state = NOT_READY;

    tflite::MicroErrorReporter micro_error_reporter;
    ms->error_reporter = &micro_error_reporter;

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
        return ic_bench_STATUS_FAILURE;
    }

#ifdef NS_TFSTRUCTURE_RECENT
    static tflite::MicroMutableOpResolver<8> resolver;
#else
    static tflite::MicroMutableOpResolver<8> resolver(error_reporter);
#endif
    resolver.AddConv2D();
    resolver.AddAdd();
    resolver.AddAveragePool2D();
    resolver.AddReshape();
    resolver.AddFullyConnected();
    resolver.AddSoftmax();
    resolver.AddQuantize();
    resolver.AddDequantize();

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
        ns_lp_printf("AllocateTensors() failed\n");
        return ic_bench_STATUS_FAILURE;
    }

    // Obtain pointers to the model's input and output tensors.
    for (uint32_t t = 0; t <= ms->numInputTensors; t++) {
        ms->model_input[t] = ms->interpreter->input(t);
    }

    for (uint32_t t = 0; t <= ms->numOutputTensors; t++) {
        ms->model_output[t] = ms->interpreter->output(t);
    }

    ms->state = READY;
    return ic_bench_STATUS_SUCCESS;
}
