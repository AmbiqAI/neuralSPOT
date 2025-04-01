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

#include "tensorflow/lite/kernels/internal/reference/cumsum.h"

#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/quantization_util.h"
#include "tensorflow/lite/kernels/internal/types.h"
#include "tensorflow/lite/kernels/kernel_util.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"
#include "tensorflow/lite/micro/micro_log.h"

namespace tflite {
namespace {

constexpr int kInputTensor = 0;
constexpr int kAxisTensor = 1;
constexpr int kOutputTensor = 0;

constexpr int kCumSumIntegerShift = 20;
constexpr int kCumSumIntegerShift_s16 = 10;
// only used with INT8 and INT16 tensors
struct OpData {
  int32_t output_activation_min;
  int32_t output_activation_max;
  int32_t input_offset;
  int32_t output_offset;
  int32_t input_multiplier;
  int32_t output_multiplier;
  int input_shift;
  int output_shift;
  int left_shift;
};


inline void CumSum_s16(const ArithmeticParams& params, const int16_t* input_data,
                   const RuntimeShape& shape, int32_t axis, bool exclusive,
                   bool reverse, int16_t* output_data) {
  TFLITE_DCHECK_LE(params.quantized_activation_min,
                   params.quantized_activation_max);
  TFLITE_DCHECK_GE(-params.input1_offset, std::numeric_limits<int16_t>::min());
  TFLITE_DCHECK_LE(-params.input1_offset, std::numeric_limits<int16_t>::max());
  const int32_t rank = shape.DimensionsCount();
  TFLITE_DCHECK_GE(rank, 1);
  TFLITE_DCHECK_GE(axis, 0);
  TFLITE_DCHECK_LT(axis, rank);

  size_t inner = 1;
  size_t outer = 1;
  size_t depth = 1;
  for (int32_t i = 0; i < rank; i++) {
    if (i < axis)
      inner *= shape.Dims(i);
    else if (i > axis)
      outer *= shape.Dims(i);
    else
      depth = shape.Dims(i);
  }

  for (size_t outer_index = 0; outer_index < outer; outer_index++) {
    size_t outer_index_adj;
    if (reverse)
      outer_index_adj = (outer - 1) - outer_index;
    else
      outer_index_adj = outer_index;
    for (size_t inner_index = 0; inner_index < inner; inner_index++) {
      int32_t accumulator = params.input1_offset;  // accumulator = 0
      accumulator *= (1 << params.left_shift);
      accumulator = MultiplyByQuantizedMultiplierSmallerThanOneExp(
          accumulator, params.input1_multiplier, params.input1_shift);

      size_t inner_index_adj;
      if (reverse)
        inner_index_adj = (inner - 1) - inner_index;
      else
        inner_index_adj = inner_index;

      for (size_t depth_index = 0; depth_index < depth; depth_index++) {
        size_t depth_index_adj;
        if (reverse)
          depth_index_adj = (depth - 1) - depth_index;
        else
          depth_index_adj = depth_index;

        size_t index = outer_index_adj;
        index += inner_index_adj * depth * outer;
        index += depth_index_adj * outer;

        const int32_t y = params.input1_offset + input_data[index];
        const int32_t shifted_y = y * (1 << params.left_shift);
        const int32_t scaled_y = MultiplyByQuantizedMultiplierSmallerThanOneExp(
            shifted_y, params.input1_multiplier, params.input1_shift);

        int32_t scaled_output;
        if (exclusive) {
          scaled_output = accumulator;
          accumulator += scaled_y;
        } else {
          accumulator += scaled_y;
          scaled_output = accumulator;
        }

        const int32_t raw_output =
            MultiplyByQuantizedMultiplierSmallerThanOneExp(
                scaled_output, params.output_multiplier, params.output_shift) +
            params.output_offset;
        const int32_t clamped_output =
            std::min(params.quantized_activation_max,
                     std::max(params.quantized_activation_min, raw_output));
        output_data[index] = static_cast<int16_t>(clamped_output);
      }
    }
  }
}


TfLiteStatus CalculateOpData(TfLiteContext* context, TfLiteNode* node) {
  TF_LITE_ENSURE_EQ(context, NumInputs(node), 2);
  TF_LITE_ENSURE_EQ(context, NumOutputs(node), 1);

  MicroContext* micro_context = GetMicroContext(context);

  TfLiteTensor* input =
      micro_context->AllocateTempInputTensor(node, kInputTensor);
  TfLiteTensor* axis =
      micro_context->AllocateTempInputTensor(node, kAxisTensor);

  TF_LITE_ENSURE(context,
                 input->type == kTfLiteFloat32 || input->type == kTfLiteInt8 || input->type == kTfLiteInt16);
  TF_LITE_ENSURE_EQ(context, axis->type, kTfLiteInt32);

  TF_LITE_ENSURE_EQ(context, NumElements(axis), 1);

  TF_LITE_ENSURE(context, NumDimensions(input) >= 1);

  TfLiteTensor* output =
      micro_context->AllocateTempOutputTensor(node, kOutputTensor);

  TF_LITE_ENSURE_EQ(context, input->type, output->type);
  TF_LITE_ENSURE(context, HaveSameShapes(input, output));

  if (output->type == kTfLiteInt8) {
    node->user_data =
        context->AllocatePersistentBuffer(context, sizeof(OpData));
    OpData* data = static_cast<OpData*>(node->user_data);

    // 8bit -> 8bit general quantized path, with general rescalings
    data->input_offset = -input->params.zero_point;
    data->output_offset = output->params.zero_point;
    data->left_shift = kCumSumIntegerShift;
    const double twice_max_input_scale =
        2 * static_cast<double>(input->params.scale);
    const double real_input_multiplier =
        static_cast<double>(input->params.scale) / twice_max_input_scale;
    const double real_output_multiplier =
        twice_max_input_scale /
        ((1 << data->left_shift) * static_cast<double>(output->params.scale));

    QuantizeMultiplierSmallerThanOneExp(
        real_input_multiplier, &data->input_multiplier, &data->input_shift);

    QuantizeMultiplierSmallerThanOneExp(
        real_output_multiplier, &data->output_multiplier, &data->output_shift);

    TF_LITE_ENSURE_STATUS(CalculateActivationRangeQuantized(
        context, kTfLiteActNone, output, &data->output_activation_min,
        &data->output_activation_max));
  }
  else if (output->type == kTfLiteInt16) {
    node->user_data =
        context->AllocatePersistentBuffer(context, sizeof(OpData));
    OpData* data = static_cast<OpData*>(node->user_data);

    data->input_offset = -input->params.zero_point;
    data->output_offset = output->params.zero_point;
    data->left_shift = kCumSumIntegerShift_s16;
    const double twice_max_input_scale =
        2 * static_cast<double>(input->params.scale);
    const double real_input_multiplier =
        static_cast<double>(input->params.scale) / twice_max_input_scale;
    const double real_output_multiplier =
        twice_max_input_scale /
        ((1 << data->left_shift) * static_cast<double>(output->params.scale));
    QuantizeMultiplierSmallerThanOneExp(
        real_input_multiplier, &data->input_multiplier, &data->input_shift);

    QuantizeMultiplierSmallerThanOneExp(
        real_output_multiplier, &data->output_multiplier, &data->output_shift);
        
    TF_LITE_ENSURE_STATUS(CalculateActivationRangeQuantized(
        context, kTfLiteActNone, output, &data->output_activation_min,
        &data->output_activation_max));

  }

  micro_context->DeallocateTempTfLiteTensor(input);
  micro_context->DeallocateTempTfLiteTensor(axis);
  micro_context->DeallocateTempTfLiteTensor(output);

  return kTfLiteOk;
}

TfLiteStatus CumSumPrepare(TfLiteContext* context, TfLiteNode* node) {
  return CalculateOpData(context, node);
}

TfLiteStatus CumSumEval(TfLiteContext* context, TfLiteNode* node) {
  const TfLiteEvalTensor* input =
      tflite::micro::GetEvalInput(context, node, kInputTensor);
  const TfLiteEvalTensor* axis_tensor =
      tflite::micro::GetEvalInput(context, node, kAxisTensor);

  TfLiteEvalTensor* output =
      tflite::micro::GetEvalOutput(context, node, kOutputTensor);

  auto* cs_params = static_cast<TfLiteCumsumParams*>(node->builtin_data);
  auto input_shape = tflite::micro::GetTensorShape(input);

  int32_t axis = *tflite::micro::GetTensorData<int32_t>(axis_tensor);
  if (axis < 0) axis += input_shape.DimensionsCount();

  if (axis < 0 || axis >= input_shape.DimensionsCount()) {
    MicroPrintf("CUMSUM Invalid axis: %d", axis);
    return kTfLiteError;
  }

  switch (input->type) {
    case kTfLiteFloat32: {
      reference_ops::CumSum(tflite::micro::GetTensorData<float>(input),
                            input_shape, axis, cs_params->exclusive,
                            cs_params->reverse,
                            tflite::micro::GetTensorData<float>(output));
      return kTfLiteOk;
    } break;

    case kTfLiteInt8: {
      auto* data = static_cast<OpData*>(node->user_data);
      ArithmeticParams params;
      params.left_shift = data->left_shift;
      params.input1_offset = data->input_offset;
      params.input1_multiplier = data->input_multiplier;
      params.input1_shift = data->input_shift;
      params.output_offset = data->output_offset;
      params.output_multiplier = data->output_multiplier;
      params.output_shift = data->output_shift;
      SetActivationParams(data->output_activation_min,
                          data->output_activation_max, &params);
      reference_ops::CumSum(params, tflite::micro::GetTensorData<int8_t>(input),
                            input_shape, axis, cs_params->exclusive,
                            cs_params->reverse,
                            tflite::micro::GetTensorData<int8_t>(output));
      return kTfLiteOk;
    } break;

    case kTfLiteInt16: {
      auto* data = static_cast<OpData*>(node->user_data);
      ArithmeticParams params;
      params.left_shift = data->left_shift;
      params.input1_offset = data->input_offset;
      params.input1_multiplier = data->input_multiplier;
      params.input1_shift = data->input_shift;
      params.output_offset = data->output_offset;
      params.output_multiplier = data->output_multiplier;
      params.output_shift = data->output_shift;
      SetActivationParams(data->output_activation_min,
                          data->output_activation_max, &params);
      CumSum_s16(params, tflite::micro::GetTensorData<int16_t>(input),
                            input_shape, axis, cs_params->exclusive,
                            cs_params->reverse,
                            tflite::micro::GetTensorData<int16_t>(output));
      return kTfLiteOk;
    }

    default: {
      MicroPrintf("CUMSUM only supports FLOAT32, INT8, and INT16, got %s.",
                  TfLiteTypeGetName(output->type));
      return kTfLiteError;
    }
  }

  return kTfLiteError;
}

}  // namespace

TFLMRegistration Register_CUMSUM() {
  return tflite::micro::RegisterOp(nullptr, CumSumPrepare, CumSumEval);
}

}  // namespace tflite
