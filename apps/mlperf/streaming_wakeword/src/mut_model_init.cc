/**
 * @file Template for generic_model.h
 * @author Carlos Morales (carlos.morales@ambiq.com)
 * @brief Generic TF Model wrapper
 * @version 0.1
 * @date 2023-3-08
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "mut_model_metadata.h"

// NS includes
#include "ns_ambiqsuite_harness.h"
#include "ns_debug_log.h"
#include "tflm_ns_model.h"

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

/**
 * @brief Initialize TF with model
 *
 * This code is fairly common across most TF-based models.
 * The major differences relate to input and output tensor
 * handling.
 *
 */
int tflm_validator_model_init(ns_model_state_t *ms) {
    ms->state = NOT_READY;

    tflite::MicroErrorReporter micro_error_reporter;
    ms->error_reporter = &micro_error_reporter;

#ifdef NS_MLPROFILE
    // Need a timer for the profiler to collect latencies
    NS_TRY(ns_timer_init(ms->tickTimer), "Timer init failed.\n");
    AM_SHARED_RW static tflite::MicroProfiler micro_profiler;
    ms->profiler = &micro_profiler;

    // Create the config struct for the debug log
    ns_debug_log_init_t cfg = {
        .t = ms->tickTimer,
        .m = ms->mac_estimates,
        #ifdef AM_PART_APOLLO5B
        .pmu = ms->pmu,
        #endif
    };
    ns_TFDebugLogInit(&cfg);

#else
    #ifdef NS_MLDEBUG
    ns_TFDebugLogInit(NULL);
    #endif
#endif
    ns_lp_printf("Initializing model...\n");
    tflite::InitializeTarget();
    ns_lp_printf("Target initialized.\n");
    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    ms->model = tflite::GetModel(ms->model_array);
    if (ms->model->version() != TFLITE_SCHEMA_VERSION) {
        TF_LITE_REPORT_ERROR(
            ms->error_reporter,
            "Model provided is schema version %d not equal "
            "to supported version %d.",
            ms->model->version(), TFLITE_SCHEMA_VERSION);
        return NS_STATUS_FAILURE;
    }
    ns_lp_printf("Model mapped.\n");
#ifdef NS_TFSTRUCTURE_RECENT
    static tflite::MicroMutableOpResolver<5> resolver;
#else
    static tflite::MicroMutableOpResolver<5> resolver(ms->error_reporter);
#endif
    resolver.AddDepthwiseConv2D();
resolver.AddConv2D();
resolver.AddReshape();
resolver.AddFullyConnected();
resolver.AddSoftmax();


    // Allocate ResourceVariable stuff if needed
    tflite::MicroResourceVariables *resource_variables;
    tflite::MicroAllocator *var_allocator;
    ns_lp_printf("Allocating %d resource variables...\n", ms->rv_count);
    if (ms->rv_count != 0) {
        var_allocator = tflite::MicroAllocator::Create(ms->rv_arena, ms->rv_arena_size, nullptr);
        resource_variables = tflite::MicroResourceVariables::Create(var_allocator, ms->rv_count);
    } else {
        resource_variables = nullptr;
    }
    ns_lp_printf("Resource variables allocated.\n");
    // Build an interpreter to run the model with.
#ifdef NS_TFSTRUCTURE_RECENT
    static tflite::MicroInterpreter static_interpreter(
        ms->model, resolver, ms->arena, ms->arena_size, resource_variables, ms->profiler);
#else
    static tflite::MicroInterpreter static_interpreter(
        ms->model, resolver, ms->arena, ms->arena_size, ms->error_reporter, resource_variables,
        ms->profiler);
#endif
    ms->interpreter = &static_interpreter;
    ns_lp_printf("Interpreter built.\n");
    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = ms->interpreter->AllocateTensors();
    ns_lp_printf("Tensors allocated.\n");
    if (allocate_status != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(ms->error_reporter, "AllocateTensors() failed");
        ns_lp_printf("AllocateTensors() failed. Error code %d, arena size %d\n", allocate_status,
                     ms->interpreter->arena_used_bytes());
        ms->computed_arena_size = 0xDEADBEEF;
        return NS_STATUS_FAILURE;
    }

    ms->computed_arena_size = ms->interpreter->arena_used_bytes(); // prep to send back to PC
    ns_lp_printf("Arena size computed., %d\n", ms->computed_arena_size);
    // Obtain pointers to the model's input and output tensors.
    for (uint32_t t = 0; t < ms->numInputTensors; t++) {
        ms->model_input[t] = ms->interpreter->input(t);
    }

    for (uint32_t t = 0; t < ms->numOutputTensors; t++) {
        ms->model_output[t] = ms->interpreter->output(t);
    }

    ms->state = READY;
    return NS_STATUS_SUCCESS;
}
