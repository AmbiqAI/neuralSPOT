/**
 * @file basic_model.h
 * @author your name (you@domain.com)
 * @brief Basic TF Model init example
 * @version 0.1
 * @date 2022-10-26
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "har.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_debug_log.h"

// Tensorflow Lite for Microcontroller includes (somewhat boilerplate)
// #include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_profiler.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/micro/tflite_bridge/micro_error_reporter.h"


// TFLM model
#include "har_model_data.h"

// Tensorflow Globals (somewhat boilerplate)
static tflite::ErrorReporter *error_reporter = nullptr;
static const tflite::Model *model = nullptr;
static tflite::MicroInterpreter *interpreter = nullptr;
static TfLiteTensor *model_input = nullptr;
static TfLiteTensor *model_output = nullptr;
static tflite::MicroProfiler *profiler = nullptr;

static constexpr int kTensorArenaSize = 1024 * 56;
alignas(16) static uint8_t tensor_arena[kTensorArenaSize];


/**
 * @brief Initialize TF with KWS model
 *
 * This code is fairly common across most TF-based models.
 * The major differences relate to input and output tensor
 * handling.
 *
 */
static void
model_init(void) {

    tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;
    tflite::InitializeTarget();

    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    model = tflite::GetModel(har_model);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        TF_LITE_REPORT_ERROR(error_reporter,
                             "Model provided is schema version %d not equal "
                             "to supported version %d.",
                             model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }

    static tflite::MicroMutableOpResolver<6> resolver;

    resolver.AddExpandDims();
    resolver.AddConv2D();
    resolver.AddReshape();
    resolver.AddMaxPool2D();
    resolver.AddFullyConnected();
    resolver.AddSoftmax();

    // Build an interpreter to run the model with.
    static tflite::MicroInterpreter static_interpreter(model, resolver, tensor_arena,
                                                       kTensorArenaSize, nullptr, profiler);

    interpreter = &static_interpreter;
    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        ns_lp_printf("Needs %d arena bytes\n", interpreter->arena_used_bytes());
        ns_lp_printf("AllocateTensors() failed");
        return;
    }

    // Obtain pointers to the model's input and output tensors.
    model_input = interpreter->input(0);
    model_output = interpreter->output(0);
}
