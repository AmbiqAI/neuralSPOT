/* Copyright 2021 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef TENSORFLOW_LITE_MICRO_KERNELS_DEQUANTIZE_H_
#define TENSORFLOW_LITE_MICRO_KERNELS_DEQUANTIZE_H_

#include "tensorflow/lite/c/builtin_op_data.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/types.h"

namespace tflite {

struct DequantizeOpData {
    tflite::DequantizationParams quantization_params;
    // The scaling factor from input to output (aka the 'real multiplier') can
    // be represented as a fixed point multiplier plus a left shift.
    int32_t output_multiplier;
    int output_shift;
    int32_t output_zero_point;
};

TfLiteStatus DequantizePrepare(TfLiteContext *context, TfLiteNode *node);

} // namespace tflite

#endif // TENSORFLOW_LITE_MICRO_KERNELS_DEQUANTIZE_H_
