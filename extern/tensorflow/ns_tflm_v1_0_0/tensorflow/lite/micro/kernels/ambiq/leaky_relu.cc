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

#include "tensorflow/lite/kernels/internal/reference/leaky_relu.h"

#include "Include/arm_nnsupportfunctions.h"
#include "Include/arm_nnactivations.h"

#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/quantization_util.h"
#include "tensorflow/lite/kernels/internal/reference/process_broadcast_shapes.h"
#include "tensorflow/lite/kernels/internal/types.h"
#include "tensorflow/lite/kernels/kernel_util.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"
#include "tensorflow/lite/micro/kernels/leaky_relu.h"
#include "tensorflow/lite/micro/micro_log.h"

namespace tflite {

static inline void leaky_relu_s8(
  const LeakyReluOpData& data,
  const RuntimeShape& input_shape,
  const int8_t* input_data,
  const RuntimeShape& output_shape,
  int8_t* output_data
) {

  arm_leaky_relu_s8(
    input_data,
    data.input_zero_point,
    data.output_zero_point,
    data.output_multiplier_alpha,
    data.output_shift_alpha,
    data.output_multiplier_identity,
    data.output_shift_identity,
    output_data,
    MatchingFlatSize(input_shape, output_shape)
  );

}

static inline void leaky_relu_s16(
    const LeakyReluOpData& data,
    const RuntimeShape& input_shape,
    const int16_t* input_data,
    const RuntimeShape& output_shape,
    int16_t* output_data
  ) {

  arm_leaky_relu_s16(
    input_data,
    data.input_zero_point,
    data.output_zero_point,
    data.output_multiplier_alpha,
    data.output_shift_alpha,
    data.output_multiplier_identity,
    data.output_shift_identity,
    output_data,
    MatchingFlatSize(input_shape, output_shape)
  );

}


void* LeakyReluInit(TfLiteContext* context, const char* buffer, size_t length) {
  TFLITE_DCHECK(context->AllocatePersistentBuffer != nullptr);
  return context->AllocatePersistentBuffer(context, sizeof(LeakyReluOpData));
}

TfLiteStatus LeakyReluEval(TfLiteContext* context, TfLiteNode* node) {
  const TfLiteEvalTensor* input =
      tflite::micro::GetEvalInput(context, node, kInputTensor);
  TfLiteEvalTensor* output =
      tflite::micro::GetEvalOutput(context, node, kOutputTensor);
  const LeakyReluOpData& data = *static_cast<LeakyReluOpData*>(node->user_data);

  switch (input->type) {
    case kTfLiteFloat32: {
      LeakyReluParams op_params = {};
      const auto* params = static_cast<TfLiteLeakyReluParams*>(node->builtin_data);
      op_params.alpha = params->alpha;
      reference_ops::LeakyRelu(
        op_params, tflite::micro::GetTensorShape(input),
        tflite::micro::GetTensorData<float>(input),
        tflite::micro::GetTensorShape(output),
        tflite::micro::GetTensorData<float>(output)
      );
      return kTfLiteOk;
    } break;
    case kTfLiteInt8: {
      leaky_relu_s8(
        data,
        tflite::micro::GetTensorShape(input),
        tflite::micro::GetTensorData<int8_t>(input),
        tflite::micro::GetTensorShape(output),
        tflite::micro::GetTensorData<int8_t>(output)
      );
      return kTfLiteOk;
    } break;
    case kTfLiteInt16: {
      leaky_relu_s16(
        data,
        tflite::micro::GetTensorShape(input),
        tflite::micro::GetTensorData<int16_t>(input),
        tflite::micro::GetTensorShape(output),
        tflite::micro::GetTensorData<int16_t>(output)
      );
      return kTfLiteOk;
    } break;
    default:
      MicroPrintf("Only float32, int8, int16 are supported by LEAKY_RELU, got %s.",
                  TfLiteTypeGetName(input->type));
      return kTfLiteError;
  }

  return kTfLiteError;
}

TFLMRegistration Register_LEAKY_RELU() {
  return tflite::micro::RegisterOp(LeakyReluInit, LeakyReluPrepare, LeakyReluEval);
}

}  // namespace tflite
