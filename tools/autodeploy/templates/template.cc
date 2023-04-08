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
#include "NS_AD_NAME_model_data.h"
#include "ns_model.h"

// Tensorflow Lite for Microcontroller includes (somewhat boilerplate)
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_profiler.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

#ifdef NS_TF_VERSION_fecdd5d
    #include "tensorflow/lite/micro/tflite_bridge/micro_error_reporter.h"
#else
    #include "tensorflow/lite/micro/micro_error_reporter.h"
#endif

static constexpr int NS_AD_NAME_tensor_arena_size = 1024 * NS_AD_NAME_COMPUTED_ARENA_SIZE;
alignas(16) static uint8_t NS_AD_NAME_tensor_arena[NS_AD_NAME_tensor_arena_size];

// Resource Variable Arena
static constexpr int NS_AD_NAME_resource_var_arena_size =
    4 * (NS_AD_RV_COUNT + 1) * sizeof(tflite::MicroResourceVariables);
alignas(16) static uint8_t NS_AD_NAME_var_arena[NS_AD_NAME_resource_var_arena_size];

int
NS_AD_NAME_init(ns_model_state_t *ms);

int
NS_AD_NAME_minimal_init(ns_model_state_t *ms) {
    ms->runtime = TFLM;
    ms->model_array = NS_AD_NAME_model;
    ms->arena = NS_AD_NAME_tensor_arena;
    ms->arena_size = NS_AD_NAME_tensor_arena_size;
    ms->rv_arena = NS_AD_NAME_var_arena;
    ms->rv_arena_size = NS_AD_NAME_resource_var_arena_size;
    ms->rv_count = NS_AD_RV_COUNT;
    ms->numInputTensors = NS_AD_NUM_INPUT_VECTORS;
    ms->numOutputTensors = NS_AD_NUM_OUTPUT_VECTORS;

    ms->tickTimer = NULL;
    ms->mac_estimate = NULL;

    int status = NS_AD_NAME_init(ms);
    return status;
}

int
NS_AD_NAME_init(ns_model_state_t *ms) {
    ms->state = NOT_READY;

    tflite::MicroErrorReporter micro_error_reporter;
    ms->error_reporter = &micro_error_reporter;

    tflite::InitializeTarget();

    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    ms->model = tflite::GetModel(ms->model_array);
    if (ms->model->version() != TFLITE_SCHEMA_VERSION) {
        TF_LITE_REPORT_ERROR(ms->error_reporter,
                             "Model provided is schema version %d not equal "
                             "to supported version %d.",
                             ms->model->version(), TFLITE_SCHEMA_VERSION);
        return NS_AD_NAME_STATUS_FAILURE;
    }

#ifdef NS_TF_VERSION_fecdd5d
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
#ifdef NS_TF_VERSION_fecdd5d
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

    // Obtain pointers to the model's input and output tensors.
    for (uint32_t t = 0; t <= ms->numInputTensors; t++) {
        ms->model_input[t] = ms->interpreter->input(t);
    }

    for (uint32_t t = 0; t <= ms->numOutputTensors; t++) {
        ms->model_output[t] = ms->interpreter->output(t);
    }

    ms->state = READY;
    return NS_AD_NAME_STATUS_SUCCESS;
}
