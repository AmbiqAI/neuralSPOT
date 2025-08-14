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

#include "submitter_implemented.h"
#include "ns_peripherals_power.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "internally_implemented.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/tflite_bridge/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "quantization_helpers.h"
#include "tf_micro_model_runner.h"
#include "vww_inputs.h"
#include "vww_model_data.h"
#include "vww_model_settings.h"

constexpr int kTensorArenaSize = 200 * 1024;
uint8_t tensor_arena[kTensorArenaSize];
//int8_t input[kVwwInputSize];

tflite::MicroModelRunner<int8_t, int8_t, 6> *runner;

// Implement this method to prepare for inference and preprocess inputs.
void th_load_tensor() {
  int8_t* input = runner->get_input();
  size_t bytes = ee_get_buffer((uint8_t*)input,
                               kVwwInputSize * sizeof(int8_t));
  if (bytes / sizeof(int8_t) != kVwwInputSize) {
    th_printf("Input db has %d elemented, expected %d\n",
              bytes / sizeof(int8_t), kVwwInputSize);
    return;
  }

  for (size_t i = 0; i < bytes; i++) {
    input[i] -= 128;
  }

  //runner->SetInput(input);
}

// Add to this method to return real inference results.
void th_results() {
  const int nresults = 3;
  /**
   * The results need to be printed back in exactly this format; if easier
   * to just modify this loop than copy to results[] above, do that.
   */
  th_printf("m-results-[");
  unsigned int kCategoryCount = 2;
  for (size_t i = 0; i < kCategoryCount; i++) {
    float converted =
        DequantizeInt8ToFloat(runner->GetOutput()[i], runner->output_scale(),
                              runner->output_zero_point());

    // Some platforms don't implement floating point formatting.
    th_printf("%0.3f", converted);
    if (i < (nresults - 1)) {
      th_printf(",");
    }
  }
  th_printf("]\r\n");
}

// Implement this method with the logic to perform one inference cycle.
void th_infer() { runner->Invoke(); }

/// \brief optional API.
void th_final_initialize(void) {
  static tflite::MicroMutableOpResolver<6> resolver;
  resolver.AddFullyConnected();
  resolver.AddConv2D();
  resolver.AddDepthwiseConv2D();
  resolver.AddReshape();
  resolver.AddSoftmax();
  resolver.AddAveragePool2D();

  static tflite::MicroModelRunner<int8_t, int8_t, 6> model_runner(
      g_person_detect_model_data, resolver, tensor_arena, kTensorArenaSize);
  runner = &model_runner;

  if (runner->arena_used_bytes() > kTensorArenaSize)
    th_printf("arena used size %d, but allocated %d\r\n", runner->arena_used_bytes(), kTensorArenaSize);
  
  // // After initializing the model, set perf or power mode
  // #if EE_CFG_ENERGY_MODE==1
  //   //ns_power_config(&ns_mlperf_recommended_default);
  //   ns_power_config(&ns_mlperf_mode1);

  // #else
  //   #ifdef AM_MLPERF_PERFORMANCE_MODE
  //     ns_power_config(&ns_development_default);
  //   #else
  //     ns_power_config(&ns_mlperf_mode2);
  //   #endif
  // #endif

}

void th_pre() {}
void th_post() {}