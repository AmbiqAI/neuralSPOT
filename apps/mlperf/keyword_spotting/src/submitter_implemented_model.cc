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
/// \brief C++ implementations of submitter_implemented_model.h

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "ns_uart.h"
#include "ns_timer.h"
#include "ns_peripherals_power.h"

#include "submitter_implemented.h"

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
#include "kws_input_data.h"
#include "kws_model_data.h"
#include "kws_model_settings.h"

// Globals for model execution and instantiation
// Good targets for optimization - move to lowest power or fastest mem
// Maybe instantiate twice - once for perf and once for power

#define ARENA_SIZE 30
constexpr int kTensorArenaSize = ARENA_SIZE * 1024;
alignas(16) uint8_t tensor_arena[kTensorArenaSize];

// Model pointers
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* model_input = nullptr;
// static tflite::MicroProfiler *profiler = nullptr;

typedef struct {
  uint32_t daccess;
  uint32_t dtaglookup;
  uint32_t dhitslookup;
  uint32_t dhitsline;
  uint32_t iaccess;
  uint32_t itaglookup;
  uint32_t ihitslookup;
  uint32_t ihitsline;
} ns_cache_dump_t;

ns_cache_dump_t startDump;
ns_cache_dump_t endDump;

// void captureCacheStats(ns_cache_dump_t *dump) {
//   dump->daccess     = CPU->DMON0;
//   dump->dtaglookup  = CPU->DMON1;
//   dump->dhitslookup = CPU->DMON2;
//   dump->dhitsline   = CPU->DMON3;
//   dump->iaccess     = CPU->IMON0;
//   dump->itaglookup  = CPU->IMON1;
//   dump->ihitslookup = CPU->IMON2;
//   dump->ihitsline   = CPU->IMON3;
// }

void printCacheDump(ns_cache_dump_t *dump) {
  th_printf("****** Dcache Accesses :         %d\r\n",dump->daccess    );
  th_printf("****** Dcache Tag Lookups :      %d\r\n",dump->dtaglookup );
  th_printf("****** Dcache hits for lookups : %d\r\n",dump->dhitslookup);
  th_printf("****** Dcache hits for lines :   %d\r\n",dump->dhitsline  );
  th_printf("****** Icache Accesses :         %d\r\n",dump->iaccess    );
  th_printf("****** Icache Tag Lookups :      %d\r\n",dump->itaglookup );
  th_printf("****** Icache hits for lookups : %d\r\n",dump->ihitslookup);
  th_printf("****** Icache hits for lines :   %d\r\n",dump->ihitsline  );
}

void printCacheDumpDelta(ns_cache_dump_t *start, ns_cache_dump_t *end) {
  th_printf("****** Delta Dcache Accesses :         %d\r\n",end->daccess     - start->daccess    );
  th_printf("****** Delta Dcache Tag Lookups :      %d\r\n",end->dtaglookup  - start->dtaglookup );
  th_printf("****** Delta Dcache hits for lookups : %d\r\n",end->dhitslookup - start->dhitslookup);
  th_printf("****** Delta Dcache hits for lines :   %d\r\n",end->dhitsline   - start->dhitsline  );
  th_printf("****** Delta Icache Accesses :         %d\r\n",end->iaccess     - start->iaccess    );
  th_printf("****** Delta Icache Tag Lookups :      %d\r\n",end->itaglookup  - start->itaglookup );
  th_printf("****** Delta Icache hits for lookups : %d\r\n",end->ihitslookup - start->ihitslookup);
  th_printf("****** Delta Icache hits for lines :   %d\r\n",end->ihitsline   - start->ihitsline  );
}

/// \brief Instantiate the model
void th_final_initialize(void) {
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // copy model into TCM (tightly coupled memory)
  // memcpy(g_kws_model_data_tcm, g_kws_model_data, g_kws_model_data_len);

  tflite::InitializeTarget();

  // Load model from flatbuffer
  // model = tflite::GetModel(g_kws_model_data_tcm); // experiment
  model = tflite::GetModel(g_kws_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.",
                         model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // Build mutable resolver with minimum opset
  static tflite::MicroMutableOpResolver<6> resolver;
  resolver.AddFullyConnected();
  resolver.AddConv2D();
  resolver.AddDepthwiseConv2D();
  resolver.AddReshape();
  resolver.AddSoftmax();
  resolver.AddAveragePool2D();

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize, nullptr, nullptr);
  interpreter = &static_interpreter;

  // Allocate memory for all model tensors
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
    return;
  }
  
  if (interpreter->arena_used_bytes() > kTensorArenaSize)
    th_printf("arena used size %d, but allocated %d\r\n", interpreter->arena_used_bytes(), kTensorArenaSize);

  model_input = interpreter->input(0);

  // After initializing the model, set perf or power mode
  // #if EE_CFG_ENERGY_MODE==1
  //   ns_power_config(&ns_mlperf_mode3);
  // #else
  //   #ifdef AM_MLPERF_PERFORMANCE_MODE
  //     ns_power_config(&ns_development_default);
  //   #else
  //     ns_power_config(&ns_mlperf_mode2);
  //   #endif // AM_MLPERF_PERFORMANCE_MODE  
  // #endif
  
  // char dummy = 0;
  // am_hal_cachectrl_control(AM_HAL_CACHECTRL_CONTROL_MONITOR_ENABLE, (void*)&dummy);
}

// Prepare for inference and preprocess inputs.
void th_load_tensor() {
  uint8_t *model_input_buffer = model_input->data.uint8;
 
  size_t bytes = ee_get_buffer(model_input_buffer,
                               kKwsInputSize * sizeof(uint8_t));

   if (bytes / sizeof(uint8_t) != kKwsInputSize) {
    th_printf("e-Input db has %d elemented, expected %d\r\n", bytes / sizeof(int8_t),
              kKwsInputSize);
    return;
  }
}

// Return real inference results.
void th_results() {
  TfLiteTensor* output = interpreter->output(0);

  /**
   * The results need to be printed back in exactly this format; if easier
   * to just modify this loop than copy to results[] above, do that.
   */
  th_printf("m-results-[");
  unsigned int kCategoryCount = 12;

  for (size_t i = 0; i < kCategoryCount; i++) {
    float converted =
        DequantizeInt8ToFloat(output->data.int8[i], output->params.scale,
                              output->params.zero_point);

	// Some platforms don't implement floating point formatting.
    th_printf("0.%d", static_cast<int>(converted * 10));
    th_printf("%d", static_cast<int>(converted * 100) % 10);
    th_printf("%d", static_cast<int>(converted * 1000) % 10);

    if (i < (kCategoryCount - 1)) {
      th_printf(",");
    }
  }
  th_printf("]\r\n");
}

// Logic to perform one inference cycle.
void th_infer() { 

  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed");
    return;
  }
}

void th_pre() {
  // captureCacheStats(&startDump);
  // printCacheDump(&startDump);
}

void th_post() {
  // captureCacheStats(&endDump);
  // printCacheDump(&endDump); 
  // printCacheDumpDelta(&startDump, &endDump);
}