/**
 * @file model.cc
 * @author Adam Page (adam.page@ambiq.com)
 * @brief Performs inference using TinyEngine or TFLM
 * @version 0.1
 * @date 2022-11-02
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "model.h"
#include "ns_camera_constants.h"
#include "ns_ambiqsuite_harness.h"
#if INFERENCE_ENGINE == TINY_ENGINE
extern "C" {
    #include "tinyengine/genNN.h"
    #include "tinyengine/tinyengine_function.h"
}
#else
    #include "model_buffer.h"
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

//*****************************************************************************
//*** TFLM Globals
static tflite::ErrorReporter *error_reporter = nullptr;
const tflite::Model *model = nullptr;
tflite::MicroInterpreter *interpreter = nullptr;
TfLiteTensor *modelInput = nullptr;
TfLiteTensor *modelOutput = nullptr;
constexpr int kTensorArenaSize = 1024 * 225;
alignas(16) static uint8_t tensorArena[kTensorArenaSize];
#endif
static tflite::MicroProfiler *profiler = nullptr;

img_t *imgBuffer;

int init_model() {
    /**
     * @brief Initialize TFLM model block
     *
     */

    tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;

#if INFERENCE_ENGINE == TINY_ENGINE
    imgBuffer = (int8_t *)getInput();
#else
    // static tflite::AllOpsResolver model_op_resolver;
    // static tflite::MicroMutableOpResolver<13> model_op_resolver(error_reporter);
    static tflite::MicroMutableOpResolver<13> resolver;
    resolver.AddQuantize();
    resolver.AddConv2D();
    resolver.AddDepthwiseConv2D();
    resolver.AddMean();
    resolver.AddMinimum();
    resolver.AddRelu();
    resolver.AddRelu6();
    resolver.AddMul();
    resolver.AddFullyConnected();
    resolver.AddSoftmax();
    resolver.AddDequantize();
    resolver.AddMaxPool2D();
    resolver.AddAdd();

    tflite::InitializeTarget();

    // Map the model into a usable data structure.
    model = tflite::GetModel(g_tflm_model);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        TF_LITE_REPORT_ERROR(
            errorReporter,
            "Model provided is schema version %d not equal to "
            "supported version %d.",
            model->version(), TFLITE_SCHEMA_VERSION);
        return 1;
    }

    // Build an TFLM interpreter
    static tflite::MicroInterpreter static_interpreter(
        model, resolver, tensorArena, kTensorArenaSize, nullptr, profiler);
    interpreter = &static_interpreter;

    // Allocate memory from the tensorArena for the model's tensors.
    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(errorReporter, "AllocateTensors() failed");
        return 1;
    }

    size_t bytesUsed = interpreter->arena_used_bytes();
    // ns_printf("%u bytes used\n", bytesUsed);
    if (bytesUsed > kTensorArenaSize) {
        TF_LITE_REPORT_ERROR(
            errorReporter, "Model requires %d bytes for arena but given only %d bytes.", bytesUsed,
            kTensorArenaSize);
        return 1;
    }

    // Obtain pointers to the model's input and output tensors.
    modelInput = interpreter->input(0);
    imgBuffer = modelInput->data.int8;
    modelOutput = interpreter->output(0);
#endif
    return 0;
}

int model_inference(float32_t *y) {
    /**
     * @brief Run inference
     * @param y Model output logits
     * @return Output label index
     */
    int y_idx = -1;
    float32_t y_val = 0;
#if INFERENCE_ENGINE == TINY_ENGINE
    invoke(y);
    signed char *output = (signed char *)getOutput();
    for (int i = 0; i < NUM_CLASSES; i++) {
        y[i] = (float32_t)output[i];
        ns_printf("y[%d] = %f\n", i, y[i]);
        if ((y_idx == -1) || (y[i] > y_val)) {
            y_val = y[i];
            y_idx = i;
        }
    }
#else
    TfLiteStatus invokeStatus = interpreter->Invoke();
    if (invokeStatus != kTfLiteOk) {
        return -1;
    }
    for (int i = 0; i < modelOutput->dims->data[1]; i++) {
        y[i] = ((float32_t)modelOutput->data.int8[i] - modelOutput->params.zero_point) *
               modelOutput->params.scale;
        ns_printf("y[%d] = %f\n", i, y[i]);
        if ((y_idx == -1) || (y[i] > y_val)) {
            y_val = y[i];
            y_idx = i;
        }
    }
#endif
    return y_idx;
}
