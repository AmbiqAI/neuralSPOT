/*
Copyright 2020 EEMBC and The MLPerf Authors. All Rights Reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

This file reflects a modified version of th_lib from EEMBC. The reporting logic
in th_results is copied from the original in EEMBC.
==============================================================================*/
/// \file
/// \brief C++ implementations of submitter_implemented.h

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "ns_uart.h"
#include "ns_timer.h"
#include "submitter_implemented.h"
#include "ns_peripherals_power.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "internally_implemented.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/tflite_bridge/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "quantization_helpers.h"
#include "tf_micro_model_runner.h"
#include "model.h"
#include "micro_model_settings.h"

//#define AM_MLPERF_PERFORMANCE_MODE

// Globals for model execution and instantiation
// Good targets for optimization - move to lowest power or fastest mem
// Maybe instantiate twice - once for perf and once for power
constexpr int kTensorArenaSize = 10 * 1024;
uint8_t tensor_arena[kTensorArenaSize];

typedef int8_t model_input_t;
typedef int8_t model_output_t;

float input_float[kInputSize];
int8_t input_quantized[kInputSize];
float result;
 
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* model_input = nullptr;

// calculate |output - input|
void calculate_result(){
  float diffsum = 0;

  TfLiteTensor* output = interpreter->output(0);
  for (size_t i = 0; i < kFeatureElementCount; i++) {
    float converted = DequantizeInt8ToFloat(output->data.int8[i], interpreter->output(0)->params.scale,
                                            interpreter->output(0)->params.zero_point);
    float diff = converted - input_float[i];
    diffsum += diff * diff;
  }
  diffsum /= kFeatureElementCount;

  result = diffsum;
}

// Implement this method to prepare for inference and preprocess inputs.
void th_load_tensor() {
  int8_t *model_input_buffer = model_input->data.int8;

  size_t bytes = ee_get_buffer(reinterpret_cast<uint8_t *>(input_float),
                               kInputSize * sizeof(float));
  if (bytes / sizeof(float) != kInputSize) {
    th_printf("Input db has %d elemented, expected %d\n", bytes / sizeof(float),
              kInputSize);
    return;
  }

  float input_scale = interpreter->input(0)->params.scale;
  int input_zero_point = interpreter->input(0)->params.zero_point;
  for (int i = 0; i < kInputSize; i++) {
    model_input_buffer[i] = QuantizeFloatToInt8(
        input_float[i], input_scale, input_zero_point);
  }

  // copy input into interpreter's buffer
  //copy_input();
}

// Add to this method to return real inference results.
void th_results() {

  // calculate |output - input|
  calculate_result();

  /**
   * The results need to be printed back in exactly this format; if easier
   * to just modify this loop than copy to results[] above, do that.
   */
  th_printf("m-results-[%0.3f]\r\n", result);
}

// Implement this method with the logic to perform one inference cycle.
void th_infer() {

  // Run the model on the spectrogram input and make sure it succeeds.
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed");
    return;
  }
}

/// \brief optional API.
void th_final_initialize(void) {

  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(g_model);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.",
                         model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // Pull in only the operation implementations we need.
  // This relies on a complete list of all the ops needed by this graph.
  // An easier approach is to just use the AllOpsResolver, but this will
  // incur some penalty in code space for op implementations that are not
  // needed by this graph.
  //
  // tflite::AllOpsResolver resolver;
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroMutableOpResolver<3> micro_op_resolver;
  if (micro_op_resolver.AddFullyConnected() != kTfLiteOk) {
    return;
  }
  if (micro_op_resolver.AddQuantize() != kTfLiteOk) {
    return;
  }
  if (micro_op_resolver.AddDequantize() != kTfLiteOk) {
    return;
  }
  
  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, micro_op_resolver, tensor_arena, kTensorArenaSize, nullptr, nullptr);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
    return;
  }

  // Get information about the memory area to use for the model's input.
  model_input = interpreter->input(0);
  if ((model_input->dims->size != 2) || (model_input->dims->data[0] != 1) ||
      (model_input->dims->data[1] !=
       (kFeatureSliceCount * kFeatureSliceSize)) ||
      (model_input->type != kTfLiteInt8)) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Bad input tensor parameters in model");
    return;
  }
  if (interpreter->arena_used_bytes() > kTensorArenaSize)
    th_printf("arena used size %d, but allocated %d\r\n", interpreter->arena_used_bytes(), kTensorArenaSize);

  // After initializing the model, set perf or power mode
  #if EE_CFG_ENERGY_MODE==1
    ns_power_config(&ns_mlperf_mode1);
  #else
    #ifdef AM_MLPERF_PERFORMANCE_MODE
      ns_power_config(&ns_development_default);
    #else
      ns_power_config(&ns_mlperf_mode2);
    #endif // AM_MLP
  #endif

  th_printf("Initialized\r\n");
}

void th_pre() {
}
void th_post() {
}
