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
#include "tensorflow/lite/kernels/internal/reference/log_softmax.h"

#include <cstddef>
#include <cstdint>

#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/quantization_util.h"
#include "tensorflow/lite/kernels/internal/tensor_ctypes.h"
#include "tensorflow/lite/kernels/internal/types.h"
#include "tensorflow/lite/kernels/kernel_util.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"
#include "tensorflow/lite/micro/micro_log.h"

namespace tflite {
namespace {

// used only with quantized data
struct LogSoftmaxOpData {
  int32_t input_multiplier;
  int32_t input_left_shift;
  int32_t reverse_scaling_divisor;
  int32_t reverse_scaling_right_shift;
  int diff_min;
  size_t outer_size;  // number of tensor elements skipping computation axis
  size_t depth;       // number of tensor elements on computation axis
};

// input/output tensor index
constexpr int kInputTensor = 0;
constexpr int kOutputTensor = 0;

inline void LogSoftmax_s16(
    const SoftmaxParams& params,
    const size_t outer_size,
    const size_t depth,
    const RuntimeShape& input_shape,
    const int16_t* input_data,
    const RuntimeShape& output_shape,
    int16_t* output_data) {
  const int32_t input_multiplier = params.input_multiplier;
  const int32_t input_left_shift = params.input_left_shift;
  const int32_t reverse_scaling_divisor = params.reverse_scaling_divisor;
  const int32_t reverse_scaling_right_shift = params.reverse_scaling_right_shift;
  const int diff_min = params.diff_min;

  // For int16 range
  static constexpr int16_t kMinT16 = std::numeric_limits<int16_t>::min();  // -32768
  static constexpr int16_t kMaxT16 = std::numeric_limits<int16_t>::max();  // +32767
  static constexpr int32_t kMinInt32 = std::numeric_limits<int32_t>::min();

  static constexpr int kInputIntegerBits = 5;
  static constexpr int kAccumulationIntegerBits = 12;
  static constexpr int kOutputIntegerBits = 12;

  using F5 = gemmlowp::FixedPoint<int32_t, kInputIntegerBits>;
  using F12 = gemmlowp::FixedPoint<int32_t, kAccumulationIntegerBits>;
  // Outer loop: each "row" or "batch"
  for (size_t outer_index = 0; outer_index < outer_size; ++outer_index) {
    // Find max in row for numerical stability
    int16_t max_in_row = kMinT16;
    for (size_t inner_index = 0; inner_index < depth; ++inner_index) {
      max_in_row = std::max(max_in_row, input_data[outer_index * depth + inner_index]);
    }

    // Sum of exponentials in Q12.19
    F12 sum_of_exps_in_q12 = F12::FromRaw(0);
    for (size_t inner_index = 0; inner_index < depth; ++inner_index) {
      int32_t input_diff =
          static_cast<int32_t>(input_data[outer_index * depth + inner_index]) -
          static_cast<int32_t>(max_in_row);

      // If too negative, treat as negligible
      if (input_diff >= diff_min) {
        // Convert input_diff to Q5.26
        const int32_t input_diff_in_q5 =
            MultiplyByQuantizedMultiplier(
                input_diff, input_multiplier, input_left_shift);

        // Approximate exp(input_diff_in_q5) in Q5.26
        const F5 scaled_diff_f5 = F5::FromRaw(input_diff_in_q5);
        const auto exp_in_q5 = exp_on_negative_values(scaled_diff_f5);

        // Rescale from Q5.26 -> Q12.19
        sum_of_exps_in_q12 =
            sum_of_exps_in_q12 +
            gemmlowp::Rescale<kAccumulationIntegerBits>(exp_in_q5);
      }
    }

    // Log(sum_of_exps) in Q5.26
    const int32_t log_sum_of_exps_in_q5 =log_x_for_x_greater_than_or_equal_to_1<kInputIntegerBits, kAccumulationIntegerBits>(
            sum_of_exps_in_q12)
            .raw();
    // Adjust for large negative boundary checks
    const int32_t shifted_log_sum_of_exps_in_q5 =
        log_sum_of_exps_in_q5 + kMinInt32;
    const int32_t adjusted_diff_min = std::max(
        static_cast<int32_t>(diff_min - 1),
        MultiplyByQuantizedMultiplier(
            shifted_log_sum_of_exps_in_q5,
            reverse_scaling_divisor,
            -reverse_scaling_right_shift));

    // final loop: output each log-softmax (in int16 range)
    for (size_t inner_index = 0; inner_index < depth; ++inner_index) {
      int32_t input_diff =
          static_cast<int32_t>(input_data[outer_index * depth + inner_index]) -
          static_cast<int32_t>(max_in_row);

      // If input_diff <= adjusted_diff_min, store the minimum log-softmax
      if (input_diff > adjusted_diff_min) {
        const int32_t input_diff_in_q5 =
            MultiplyByQuantizedMultiplier(
                input_diff, input_multiplier, input_left_shift);

        // Raw log-softmax in Q5.26 = (input_diff_in_q5 - log_sum_of_exps_in_q5).
        // Then we shift down by (31 - 5 - 4) = 22 to get Q4.27 -> "integer"
        // and add offset so the largest value ~ +32767.
        int32_t final_shift = (31 - kInputIntegerBits - kOutputIntegerBits);
        int32_t output_in_q27 = gemmlowp::RoundingDivideByPOT(
            (input_diff_in_q5 - log_sum_of_exps_in_q5),
            final_shift);

        // Offset by +32767
        output_in_q27 += kMaxT16;

        // Clamp to [-32768, 32767]
        output_in_q27 = std::max(std::min(output_in_q27, (int32_t)kMaxT16),
                                 (int32_t)kMinT16);

        output_data[outer_index * depth + inner_index] =
            static_cast<int16_t>(output_in_q27);

      } else {
        // Very negative => clamp to kMinT16
        output_data[outer_index * depth + inner_index] = kMinT16;
      }
    }  // end inner loop
  }  
}



TfLiteStatus CalculateOpData(TfLiteContext* context, TfLiteNode* node) {
  MicroContext* micro_context = GetMicroContext(context);

  TF_LITE_ENSURE_EQ(context, NumInputs(node), 1);
  TF_LITE_ENSURE_EQ(context, NumOutputs(node), 1);
  TfLiteTensor* input =
      micro_context->AllocateTempInputTensor(node, kInputTensor);
  TF_LITE_ENSURE(context, input != nullptr);
  TfLiteTensor* output =
      micro_context->AllocateTempOutputTensor(node, kOutputTensor);
  TF_LITE_ENSURE(context, output != nullptr);
  TF_LITE_ENSURE_TYPES_EQ(context, input->type, output->type);

  TF_LITE_ENSURE(context, HaveSameShapes(input, output));

  if (input->type == kTfLiteInt8) {
    node->user_data =
        context->AllocatePersistentBuffer(context, sizeof(LogSoftmaxOpData));
    auto data = static_cast<LogSoftmaxOpData*>(node->user_data);

    // quantization datum
    constexpr int32_t kOutputZeroPoint = 127;
    constexpr float kOutputScale = 16.0 / 256;
    constexpr double kBeta = 1.0;
    constexpr int kScaledDiffIntegerBits = 5;

    TF_LITE_ENSURE(context, output->params.scale == kOutputScale);
    TF_LITE_ENSURE(context, output->params.zero_point == kOutputZeroPoint);

    int input_left_shift;
    int reverse_scaling_right_shift;
    tflite::PreprocessLogSoftmaxScalingExp(
        kBeta, static_cast<double>(input->params.scale), kScaledDiffIntegerBits,
        &data->input_multiplier, &input_left_shift,
        &data->reverse_scaling_divisor, &reverse_scaling_right_shift);
    data->input_left_shift = static_cast<int32_t>(input_left_shift);
    data->reverse_scaling_right_shift =
        static_cast<int32_t>(-reverse_scaling_right_shift);
    // diff_min has a negative value, and is used to limit the maximum magnitude
    // of the diffs, which are <= 0.
    data->diff_min =
        -tflite::CalculateInputRadius(kScaledDiffIntegerBits, input_left_shift);

    RuntimeShape input_shape = GetTensorShape(input);
    const int trailing_dim = input_shape.DimensionsCount() - 1;
    data->outer_size =
        static_cast<size_t>(FlatSizeSkipDim(input_shape, trailing_dim));
    data->depth = static_cast<size_t>(input_shape.Dims(trailing_dim));
  }
else if (input->type == kTfLiteInt16) {
    node->user_data =
        context->AllocatePersistentBuffer(context, sizeof(LogSoftmaxOpData));
    auto data = static_cast<LogSoftmaxOpData*>(node->user_data);

    constexpr int32_t kOutputZeroPoint = 32767;
    constexpr float kOutputScale =  1.0f / (1 << 12);
    constexpr double kBeta = 1.0;
    constexpr int kScaledDiffIntegerBits = 5;

    TF_LITE_ENSURE(context, output->params.scale == kOutputScale);
    TF_LITE_ENSURE(context, output->params.zero_point == kOutputZeroPoint);

    int input_left_shift;
    int reverse_scaling_right_shift;
    tflite::PreprocessLogSoftmaxScalingExp(
        kBeta, static_cast<double>(input->params.scale), kScaledDiffIntegerBits,
        &data->input_multiplier, &input_left_shift,
        &data->reverse_scaling_divisor, &reverse_scaling_right_shift);

    data->input_left_shift = static_cast<int32_t>(input_left_shift);
    data->reverse_scaling_right_shift = static_cast<int32_t>(-reverse_scaling_right_shift);

    data->diff_min = -tflite::CalculateInputRadius(kScaledDiffIntegerBits, input_left_shift);

    RuntimeShape input_shape = GetTensorShape(input);
    const int trailing_dim = input_shape.DimensionsCount() - 1;
    data->outer_size =
        static_cast<size_t>(FlatSizeSkipDim(input_shape, trailing_dim));
    data->depth = static_cast<size_t>(input_shape.Dims(trailing_dim));
  }
  micro_context->DeallocateTempTfLiteTensor(input);
  micro_context->DeallocateTempTfLiteTensor(output);
  return kTfLiteOk;
}

TfLiteStatus LogSoftmaxPrepare(TfLiteContext* context, TfLiteNode* node) {
  return CalculateOpData(context, node);
}

TfLiteStatus LogSoftmaxEval(TfLiteContext* context, TfLiteNode* node) {
  const LogSoftmaxOpData* data =
      static_cast<LogSoftmaxOpData*>(node->user_data);
  const TfLiteEvalTensor* input =
      tflite::micro::GetEvalInput(context, node, kInputTensor);
  TfLiteEvalTensor* output =
      tflite::micro::GetEvalOutput(context, node, kOutputTensor);
  switch (input->type) {
    case kTfLiteFloat32: {
      SoftmaxParams op_params = {};
      reference_ops::LogSoftmax(op_params, tflite::micro::GetTensorShape(input),
                                tflite::micro::GetTensorData<float>(input),
                                tflite::micro::GetTensorShape(output),
                                tflite::micro::GetTensorData<float>(output));
      return kTfLiteOk;
    }
    case kTfLiteInt8: {
      SoftmaxParams op_params = {};
      op_params.input_multiplier = data->input_multiplier;
      op_params.input_left_shift = data->input_left_shift;
      op_params.reverse_scaling_divisor = data->reverse_scaling_divisor;
      op_params.reverse_scaling_right_shift = data->reverse_scaling_right_shift;
      op_params.diff_min = data->diff_min;
      reference_ops::LogSoftmax(op_params, data->outer_size, data->depth,
                                tflite::micro::GetTensorShape(input),
                                tflite::micro::GetTensorData<int8_t>(input),
                                tflite::micro::GetTensorShape(output),
                                tflite::micro::GetTensorData<int8_t>(output));
      return kTfLiteOk;
    }
    case kTfLiteInt16: {
      SoftmaxParams op_params = {};
      op_params.input_multiplier = data->input_multiplier;
      op_params.input_left_shift = data->input_left_shift;
      op_params.reverse_scaling_divisor = data->reverse_scaling_divisor;
      op_params.reverse_scaling_right_shift = data->reverse_scaling_right_shift;
      op_params.diff_min = data->diff_min;
      LogSoftmax_s16(op_params, data->outer_size, data->depth,
                                      tflite::micro::GetTensorShape(input),
                                      tflite::micro::GetTensorData<int16_t>(input),
                                      tflite::micro::GetTensorShape(output),
                                      tflite::micro::GetTensorData<int16_t>(output));
      return kTfLiteOk;
    }
    default:
      MicroPrintf("LOG_SOFTMAX only supports float32, int8, got %s.",
                  TfLiteTypeGetName(input->type));
      return kTfLiteError;
  }
}

}  // namespace

TFLMRegistration Register_LOG_SOFTMAX() {
  return tflite::micro::RegisterOp(nullptr, LogSoftmaxPrepare, LogSoftmaxEval);
}

}  // namespace tflite
