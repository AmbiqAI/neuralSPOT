/* Copyright 2022 The TensorFlow Authors. All Rights Reserved.

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

#include <cstdint>
#include <limits>

#include "tensorflow/lite/kernels/internal/reference/neg.h"

#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/types.h"
#include "tensorflow/lite/kernels/internal/tensor_ctypes.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"
#include "tensorflow/lite/micro/micro_log.h"

namespace tflite {

namespace {

constexpr int kInputTensor = 0;
constexpr int kOutputTensor = 0;

struct OpData {
  // Store the zero point and scale for quantized operations.
  int32_t quant_zero_point;
};


template <typename T>
T NegateSaturateQuantized(int32_t value) {
  if (value > std::numeric_limits<T>::max()) {
    return std::numeric_limits<T>::max();
  }
  if (value < std::numeric_limits<T>::min()) {
    return std::numeric_limits<T>::min();
  }
  return static_cast<T>(value);
}

template <typename T>
void NegateQuantized(
  const RuntimeShape& input_shape, const T* input_data,
  const RuntimeShape& output_shape, T* output_data,
  int32_t zero_point
) {

  const int flat_size = MatchingFlatSize(input_shape, output_shape);

  for (int i = 0; i < flat_size; ++i) {
    int32_t unclamped = 2 * zero_point - static_cast<int32_t>(input_data[i]);
    output_data[i] = NegateSaturateQuantized<T>(unclamped);
  }
}


void* NegInit(TfLiteContext* context, const char* buffer, size_t length) {
  TFLITE_DCHECK(context->AllocatePersistentBuffer != nullptr);
  return context->AllocatePersistentBuffer(context, sizeof(OpData));
}

TfLiteStatus NegPrepare(TfLiteContext* context, TfLiteNode* node) {
  TFLITE_DCHECK(node->user_data != nullptr);

  OpData* data = static_cast<OpData*>(node->user_data);
  MicroContext* micro_context = GetMicroContext(context);
  TfLiteTensor* input1 = micro_context->AllocateTempInputTensor(node, kInputTensor);

  if (input1->type == kTfLiteInt8 || input1->type == kTfLiteInt16) {
    data->quant_zero_point = input1->params.zero_point;
  }

  micro_context->DeallocateTempTfLiteTensor(input1);

  return kTfLiteOk;
}

TfLiteStatus NegEval(TfLiteContext* context, TfLiteNode* node) {
  const TfLiteEvalTensor* input =
      tflite::micro::GetEvalInput(context, node, kInputTensor);
  TfLiteEvalTensor* output =
      tflite::micro::GetEvalOutput(context, node, kOutputTensor);
  switch (input->type) {
    case kTfLiteFloat32:
      reference_ops::Negate(
        tflite::micro::GetTensorShape(input),
        tflite::micro::GetTensorData<float>(input),
        tflite::micro::GetTensorShape(output),
        tflite::micro::GetTensorData<float>(output)
      );
      break;
    case kTfLiteInt16: {
      TFLITE_DCHECK(node->user_data != nullptr);
      const OpData* data = static_cast<const OpData*>(node->user_data);
      NegateQuantized(
        tflite::micro::GetTensorShape(input),
        tflite::micro::GetTensorData<int16_t>(input),
        tflite::micro::GetTensorShape(output),
        tflite::micro::GetTensorData<int16_t>(output),
        data->quant_zero_point
      );
      break;
    }
    case kTfLiteInt8: {
      TFLITE_DCHECK(node->user_data != nullptr);
      const OpData* data = static_cast<const OpData*>(node->user_data);
      NegateQuantized(
        tflite::micro::GetTensorShape(input),
        tflite::micro::GetTensorData<int8_t>(input),
        tflite::micro::GetTensorShape(output),
        tflite::micro::GetTensorData<int8_t>(output),
        data->quant_zero_point
      );
      break;
    }

    default:
      MicroPrintf("Type %s (%d) not supported.", TfLiteTypeGetName(input->type),
                  input->type);
      return kTfLiteError;
  }
  return kTfLiteOk;
}

}  // namespace

TFLMRegistration Register_NEG() {
  return tflite::micro::RegisterOp(NegInit, NegPrepare, NegEval);
}

}  // namespace tflite
