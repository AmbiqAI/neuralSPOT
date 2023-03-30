/**
 * @file generic_model.h
 * @author Carlos Morales (carlos.morales@ambiq.com)
 * @brief Generic TF Model wrapper
 * @version 0.1
 * @date 2023-3-08
 *
 * @copyright Copyright (c) 2023
 *
 */

// NS includes
#include "ns_model.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_debug_log.h"

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

/**
 * @brief Initialize TF with model
 *
 * This code is fairly common across most TF-based models.
 * The major differences relate to input and output tensor
 * handling.
 *
 */
int
ns_model_init(ns_model_state_t *ms) {
    ms->state = NOT_READY;

    tflite::MicroErrorReporter micro_error_reporter;
    ms->error_reporter = &micro_error_reporter;

#ifdef NS_MLPROFILE
    // Need a timer for the profiler to collect latencies
    NS_TRY(ns_timer_init(ms->tickTimer), "Timer init failed.\n");
    static tflite::MicroProfiler micro_profiler;
    ms->profiler = &micro_profiler;

    #ifdef NS_MODEL_ANALYSIS
    ns_TFDebugLogInit(ms->tickTimer, ms->mac_estimates);
    #else
    ns_TFDebugLogInit(ms->tickTimer, NULL);
    #endif
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
        TF_LITE_REPORT_ERROR(ms->error_reporter,
                             "Model provided is schema version %d not equal "
                             "to supported version %d.",
                             ms->model->version(), TFLITE_SCHEMA_VERSION);
        return NS_STATUS_FAILURE;
    }

    static tflite::AllOpsResolver resolver;

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
        ms->computed_arena_size = 0xDEADBEEF;
        return NS_STATUS_FAILURE;
    }

    ms->computed_arena_size = ms->interpreter->arena_used_bytes(); // prep to send back to PC

    // Obtain pointers to the model's input and output tensors.
    ms->model_input = ms->interpreter->input(0);
    ms->model_output = ms->interpreter->output(0);

    ms->state = READY;
    return NS_STATUS_SUCCESS;
}

uint32_t
ns_tf_get_num_input_tensors(ns_model_state_t *ms) {
    return ms->interpreter->inputs_size();
}

uint32_t
ns_tf_get_num_output_tensors(ns_model_state_t *ms) {
    return ms->interpreter->inputs_size();
}

TfLiteTensor *
ns_tf_get_input_tensor(ns_model_state_t *ms, uint32_t index) {
    return ms->interpreter->input(index);
}

TfLiteTensor *
ns_tf_get_output_tensor(ns_model_state_t *ms, uint32_t index) {
    return ms->interpreter->output(index);
}
