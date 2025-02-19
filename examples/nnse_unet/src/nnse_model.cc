/**
 * @file <nnaedrelunopad>_model.c (generated from template.cc)
 * @author autodeploy.py
 * @brief Generated automatically from template code by NS autodeploy script
 * @version 0.1
 * @date 2023-03-15
 *
 * @copyright Copyright (c) 2023
 *
 */
// #include "nnaedrelunopad_api.h"
#include "nnse_model.h"
#include "model_data_int16.h" // tflite model
#include "ns_model.h"
#include "ns_debug_log.h"
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

static constexpr int nnse_tensor_arena_size = 1024 * NNSE_COMPUTED_ARENA_SIZE;
#ifdef AM_PART_APOLLO3
    // Apollo3 doesn't have AM_SHARED_RW
    alignas(16) static uint8_t nnse_tensor_arena[nnse_tensor_arena_size];
#else // not AM_PART_APOLLO3
    #if (nnaedrelunopad_ARENA_LOCATION == NS_AD_SRAM)
        #ifdef keil6
        // Align to 16 bytesnnaedrelunopad_tensor_arena
        __attribute__((section("SHARED_RW"))) __attribute__((used)) __attribute__((aligned(16))) static uint8_t [nnse_tensor_arena_size];
        #else
        __attribute__((section(".shared"))) alignas(16) static uint8_t nnse_tensor_arena[nnse_tensor_arena_size];
        #endif
    #else
        AM_SHARED_RW alignas(16) static uint8_t nnse_tensor_arena[nnse_tensor_arena_size];
    #endif
#endif
// Resource Variable Arena
// static constexpr int nnse_resource_var_arena_size =
    // 4 * (0 + 1) * sizeof(tflite::MicroResourceVariables);
static constexpr int nnse_resource_var_arena_size = 4096 << 4; // 11293
alignas(16) static uint8_t nnse_var_arena[nnse_resource_var_arena_size];
#ifdef NS_MLPROFILE
// Timer is used for TF profiling
ns_timer_config_t basic_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};
uint32_t nnaedrelunopad_mac_estimates[NS_AD_MAC_ESTIMATE_COUNT] = {NS_AD_MAC_ESTIMATE_LIST};

#endif

int nnse_minimal_init(ns_model_state_t *ms) {
    ms->model_array = tflite_test_tflite;
    ms->arena = nnse_tensor_arena;
    ms->arena_size = nnse_tensor_arena_size;
    ms->rv_arena = nnse_var_arena;
    ms->rv_arena_size = nnse_resource_var_arena_size;
    ms->rv_count = 20;
    ms->numInputTensors = 1;
    ms->numOutputTensors = 1;

#ifdef NS_MLPROFILE
    ns_perf_mac_count_t basic_mac = {
        .number_of_layers = NS_AD_MAC_ESTIMATE_COUNT, .mac_count_map = nnaedrelunopad_mac_estimates};
    ms->tickTimer = &basic_tickTimer;
    ms->mac_estimates = &basic_mac;
#else
    ms->tickTimer = NULL;
    ms->mac_estimates = NULL;
#endif

    int status = nnse_init(ms);
    return status;
}

int nnse_init(ns_model_state_t *ms) {
    ms->state = NOT_READY;

    tflite::MicroErrorReporter micro_error_reporter;
    ms->error_reporter = &micro_error_reporter;

#ifdef NS_MLPROFILE
    // Need a timer for the profiler to collect latencies
    NS_TRY(ns_timer_init(ms->tickTimer), "Timer init failed.\n");
    static tflite::MicroProfiler micro_profiler;
    ms->profiler = &micro_profiler;
    ns_TFDebugLogInit(ms->tickTimer);
#else
    #ifdef NS_MLDEBUG
    ns_TFDebugLogInit(NULL);
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
        return nnaedrelunopad_STATUS_FAILURE;
    }

// Build mutable resolver with minimum opset
#ifdef NS_TFSTRUCTURE_RECENT
    static tflite::MicroMutableOpResolver<37> resolver;
#else
    static tflite::MicroMutableOpResolver<37> resolver(error_reporter);
#endif
    resolver.AddFullyConnected(); // 1
    resolver.AddConv2D();         // 2
    resolver.AddDepthwiseConv2D(); // 3  
    resolver.AddReshape(); // 4 
    resolver.AddSoftmax(); // 5
    resolver.AddAveragePool2D(); // 6
    resolver.AddMul();  // 7
    resolver.AddAdd();  // 8
    resolver.AddBatchMatMul();  // 9 
    resolver.AddPad();  // 10
    resolver.AddMean(); // 11
    resolver.AddPadV2();    // 12
    resolver.AddDiv();  // 13
    resolver.AddLogistic(); // 14
    resolver.AddSplit(); // 15
    resolver.AddTanh(); // 16
    resolver.AddTranspose(); // 17
    resolver.AddSquaredDifference(); // 18
    resolver.AddRsqrt(); // 19
    resolver.AddSub(); // 20
    resolver.AddStridedSlice(); // 21
    resolver.AddLogSoftmax(); // 22
    resolver.AddDequantize(); // 23
    resolver.AddPack(); // 24
    resolver.AddQuantize(); // 25
    resolver.AddCallOnce(); // 26
    resolver.AddUnidirectionalSequenceLSTM(); // 27
    resolver.AddUnpack(); // 28
    resolver.AddVarHandle(); // 29
    resolver.AddReadVariable(); // 30
    resolver.AddAssignVariable(); // 31
    resolver.AddMaximum(); // 32
    resolver.AddMinimum(); // 33
    resolver.AddConcatenation(); // 34
    resolver.AddResizeNearestNeighbor(); // 35
    resolver.AddTransposeConv(); // 36
    resolver.AddRelu(); // 37
    // Allocate ResourceVariable stuff if needed
    tflite::MicroResourceVariables *resource_variables;
    tflite::MicroAllocator *var_allocator;
    ns_printf("rv_count=%d\n", ms->rv_count);
    if (ms->rv_count != 0) {
        var_allocator = tflite::MicroAllocator::Create(ms->rv_arena, ms->rv_arena_size);
        resource_variables = tflite::MicroResourceVariables::Create(var_allocator, ms->rv_count);
    } else {
        resource_variables = nullptr;
    }
    
    // Build an interpreter to run the model with.
#ifdef NS_TFSTRUCTURE_RECENT
    static tflite::MicroInterpreter static_interpreter(
        ms->model, 
        resolver, 
        ms->arena, 
        ms->arena_size, 
        resource_variables, //resource_variables
        ms->profiler);
#else
    static tflite::MicroInterpreter static_interpreter(
        ms->model,
        resolver,
        ms->arena,
        ms->arena_size,
        ms->error_reporter,
        nullptr,
        ms->profiler);
#endif
    ms->interpreter = &static_interpreter;

    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = ms->interpreter->AllocateTensors();

    if (allocate_status != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(ms->error_reporter, "AllocateTensors() failed");
        return nnaedrelunopad_STATUS_FAILURE;
    }

    // Obtain pointers to the model's input and output tensors.
    for (uint32_t t = 0; t <= ms->numInputTensors; t++) {
        ms->model_input[t] = ms->interpreter->input(t);
    }

    for (uint32_t t = 0; t <= ms->numOutputTensors; t++) {
        ms->model_output[t] = ms->interpreter->output(t);
    }

    ms->state = READY;
    return nnaedrelunopad_STATUS_SUCCESS;
}
