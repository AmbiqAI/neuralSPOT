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
#include "ns_ambiqsuite_harness.h"
#include "basic_tf_stub.h"


/// Tensorflow Lite for Microcontroller includes (somewhat boilerplate)
//#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

/// TFLM model
#include "model.h"

/// Tensorflow Globals (somewhat boilerplate)
static tflite::ErrorReporter *error_reporter = nullptr;
static const tflite::Model *model = nullptr;
static tflite::MicroInterpreter *interpreter = nullptr;
static TfLiteTensor *input = nullptr;
static TfLiteTensor *output_intent = nullptr;
static TfLiteTensor *output_slot = nullptr;

static constexpr int kTensorArenaSize = 1024 * 70;
alignas(16) static uint8_t tensor_arena[kTensorArenaSize];

/**
 * @brief Initialize TF with s2i model
 * 
 * This code is fairly common across most TF-based models.
 * The major differences relate to input and output tensor
 * handling.
 * 
 */
static void model_init(void) {

    static tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;

    tflite::InitializeTarget();

    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    model = tflite::GetModel(slu_model_tflite);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        TF_LITE_REPORT_ERROR(error_reporter,
                             "Model provided is schema version %d not equal "
                             "to supported version %d.",
                             model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }

    // This pulls in all the operation implementations we need.

    // static tflite::MicroMutableOpResolver<1> resolver;
    static tflite::AllOpsResolver resolver;
    // Build an interpreter to run the model with.
    static tflite::MicroInterpreter static_interpreter(
        model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
    interpreter = &static_interpreter;

    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
        return;
    }

    // Obtain pointers to the model's input and output tensors.
    input         = interpreter->input(0);
    output_slot   = interpreter->output(0);
    output_intent = interpreter->output(1);
}