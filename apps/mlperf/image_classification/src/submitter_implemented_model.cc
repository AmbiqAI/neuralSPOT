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
#include "ic_inputs.h"
#include "ic_model_quant_data.h"
#include "ic_model_settings.h"
#include "ns_peripherals_power.h"

static constexpr int kTensorArenaSize = 100 * 1024;
alignas(16) uint8_t tensor_arena[kTensorArenaSize];

tflite::MicroModelRunner<int8_t, int8_t, 7> *runner;
uint8_t input_quantized[kIcInputSize];
int8_t input_asint[kIcInputSize];

// Implement this method to prepare for inference and preprocess inputs.
void th_load_tensor() {
  size_t bytes = ee_get_buffer(reinterpret_cast<uint8_t *>(input_quantized),
                               kIcInputSize * sizeof(uint8_t));
  if (bytes / sizeof(uint8_t) != kIcInputSize) {
    th_printf("Input db has %d elements, expected %d\n", bytes / sizeof(uint8_t),
              kIcInputSize);
    return;
  }
  uint16_t i = 0;
  for(i=0; i<kIcInputSize;i++)
  {
	  if(input_quantized[i]<=127)
	    input_asint[i] = ((int8_t)input_quantized[i]) - 128;
	  else
	    input_asint[i] = (int8_t)(input_quantized[i] - 128);
  }

  runner->SetInput(input_asint);
}

// Add to this method to return real inference results.
void th_results() {
  const int nresults = 10;
  /**
   * The results need to be printed back in exactly this format; if easier
   * to just modify this loop than copy to results[] above, do that.
   */
  th_printf("m-results-[");
  unsigned int kCategoryCount = 10;

  for (size_t i = 0; i < kCategoryCount; i++) {
    float converted =
        DequantizeInt8ToFloat(runner->GetOutput()[i], runner->output_scale(),
                              runner->output_zero_point());

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

  static tflite::MicroMutableOpResolver<7> resolver;

  resolver.AddAdd();
  resolver.AddFullyConnected();
  resolver.AddConv2D();
  resolver.AddDepthwiseConv2D();
  resolver.AddReshape();
  resolver.AddSoftmax();
  resolver.AddAveragePool2D();
  static tflite::MicroModelRunner<int8_t, int8_t, 7> model_runner(
      pretrainedResnet_quant_tflite, resolver, tensor_arena, kTensorArenaSize);
  runner = &model_runner;

  if (runner->arena_used_bytes() > kTensorArenaSize)
    th_printf("arena used size %d, but allocated %d\r\n", runner->arena_used_bytes(), kTensorArenaSize);

  // After initializing the model, set perf or power mode
  #if EE_CFG_ENERGY_MODE==1
    ns_power_config(&ns_mlperf_mode3);
  #else
    #ifdef AM_MLPERF_PERFORMANCE_MODE
      ns_power_config(&ns_development_default);
    #else
      ns_power_config(&ns_mlperf_mode2);
    #endif // AM_MLPERF_PERFORMANCE_MODE  
  #endif

}
void th_pre() {}
void th_post() {}