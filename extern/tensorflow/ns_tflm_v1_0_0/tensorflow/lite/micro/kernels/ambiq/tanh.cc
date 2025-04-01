/* Copyright 2023 The TensorFlow Authors. All Rights Reserved.

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

#include "tensorflow/lite/kernels/internal/reference/integer_ops/tanh.h"

#include "tensorflow/lite/c/builtin_op_data.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/common.h"
#include "tensorflow/lite/kernels/internal/quantization_util.h"
#include "tensorflow/lite/kernels/internal/reference/tanh.h"
#include "tensorflow/lite/kernels/internal/tensor_ctypes.h"
#include "tensorflow/lite/kernels/kernel_util.h"
#include "tensorflow/lite/kernels/op_macros.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_utils.h"

namespace tflite {

namespace {

constexpr int kInputTensor = 0;
constexpr int kOutputTensor = 0;

struct OpData {
  int32_t input_zero_point;
  int32_t input_range_radius;
  int32_t input_multiplier;
  int input_left_shift;
  int8_t table[256];
};

void* TanhInit(TfLiteContext* context, const char* buffer, size_t length) {
  TFLITE_DCHECK(context->AllocatePersistentBuffer != nullptr);
  return context->AllocatePersistentBuffer(context, sizeof(OpData));
}

void PopulateLookupTable(int32_t input_zero_point, int32_t input_range_radius,
                 int32_t input_multiplier, int32_t input_shift,
                 OpData* data) {

  static constexpr int32_t kInputIntegerBits = 4;
  static constexpr int32_t kOutputScale = 7;
  static constexpr int32_t kMinInt8 = std::numeric_limits<int8_t>::min();
  static constexpr int32_t kMaxInt8 = std::numeric_limits<int8_t>::max();
  using F4 = gemmlowp::FixedPoint<int32_t, kInputIntegerBits>;


  for (int32_t i = kMinInt8; i <= kMaxInt8; ++i) {
    const int32_t input =
        static_cast<int32_t>(i) - input_zero_point;
    if (input <= -input_range_radius) {
      data->table[static_cast<uint8_t>(i)] = kMinInt8;
    } else if (input >= input_range_radius) {
      data->table[static_cast<uint8_t>(i)] = kMaxInt8;
    } else {
      const int32_t input_in_q4 =
          MultiplyByQuantizedMultiplier(input, input_multiplier, input_shift);
      const int32_t output_in_q0 =
          gemmlowp::tanh(F4::FromRaw(input_in_q4)).raw();

      // Rescale and downcast.
      using gemmlowp::RoundingDivideByPOT;
      int32_t output_in_q24 =
          RoundingDivideByPOT(output_in_q0, 31 - kOutputScale);
      output_in_q24 = std::min(std::max(output_in_q24, kMinInt8), kMaxInt8);
      data->table[static_cast<uint8_t>(i)] = static_cast<int8_t>(output_in_q24);
    }
  }
}

void EvalUsingLookupTable(const OpData* data, const TfLiteEvalTensor* input,
                          TfLiteEvalTensor* output) {
  const int size = MatchingFlatSize(tflite::micro::GetTensorShape(input),
                                    tflite::micro::GetTensorShape(output));
  int8_t* output_data = tflite::micro::GetTensorData<int8_t>(output);
  const int8_t* input_data = tflite::micro::GetTensorData<int8_t>(input);

  for (int i = 0; i < size; ++i) {
    output_data[i] = data->table[static_cast<uint8_t>(input_data[i])];
  }
}

TfLiteStatus CalculateArithmeticOpData(TfLiteContext* context, TfLiteNode* node,
                                       OpData* data) {
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

  if (input->type == kTfLiteInt8) {
    static constexpr int kInputIntegerBits = 4;
    const double input_real_multiplier =
        static_cast<double>(input->params.scale) *
        static_cast<double>(1 << (31 - kInputIntegerBits));

    const double q = std::frexp(input_real_multiplier, &data->input_left_shift);
    data->input_multiplier = static_cast<int32_t>(TfLiteRound(q * (1ll << 31)));

    data->input_range_radius =
        CalculateInputRadius(kInputIntegerBits, data->input_left_shift, 31);

    PopulateLookupTable(data->input_zero_point, data->input_range_radius,
                data->input_multiplier, data->input_left_shift,
                data);
  }

  if (input->type == kTfLiteInt16) {
    static constexpr int kInputIntegerBits = 3;
    static constexpr int kOutputFractionalBits = 15;

    // These operators are implemented in fixed-point arithmetic,
    // which intrinsically wants symmetric ranges (zero_point==0)
    // and power-of-two scales (power-of-two is abbreviated below as POT).
    // While more general support would be possible by means of rescaling,
    // that would add some overhead and some loss of accuracy and wouldn't
    // be used at the moment as current quantized LSTM applications are
    // happy with symmetric, power-of-two-scales quantization. So we just
    // implement that narrow case only for now.

    TF_LITE_ENSURE_EQ(context, input->params.zero_point, 0);
    TF_LITE_ENSURE_EQ(context, output->params.zero_point, 0);

    int input_scale_log2_rounded;
    bool param_scale_pot =
        CheckedLog2(input->params.scale, &input_scale_log2_rounded);

    data->input_left_shift =
        (15 - kInputIntegerBits) + input_scale_log2_rounded;
    param_scale_pot &=
        (data->input_left_shift == 0 || data->input_left_shift == 1);

    if (param_scale_pot) {
      data->input_multiplier = 0;
    } else {
      // Calculate multiplier to change input scale to 1/(3*4096)
      // as required by the table lookup.
      // The number 3.0 in the multiplier comes from here,
      // because the interval is [-10.7, 10.7] instead of [-8, 8].
      // So, in this scaling +/-2^17 represents +/-10.7.

      double multiplier =
          static_cast<double>(input->params.scale) * 4096.0 * 3.0;
      data->input_left_shift = 0;

      while (multiplier <= 32767.0 / 2.0 && data->input_left_shift <= 30) {
        data->input_left_shift++;
        multiplier = multiplier * 2.0;
      }

      data->input_multiplier = static_cast<int32_t>(multiplier);
    }
    TFLITE_DCHECK_LE(data->input_multiplier, 32767);
    int output_scale_log2_rounded;
    TF_LITE_ENSURE(
        context, CheckedLog2(output->params.scale, &output_scale_log2_rounded));
    TF_LITE_ENSURE_EQ(context, output_scale_log2_rounded,
                      -kOutputFractionalBits);
  }

  micro_context->DeallocateTempTfLiteTensor(input);
  micro_context->DeallocateTempTfLiteTensor(output);
  return kTfLiteOk;
}

TfLiteStatus TanhPrepare(TfLiteContext* context, TfLiteNode* node) {
  TFLITE_DCHECK(node->user_data != nullptr);

  OpData* data = static_cast<OpData*>(node->user_data);

  MicroContext* micro_context = GetMicroContext(context);
  TfLiteTensor* input =
      micro_context->AllocateTempInputTensor(node, kInputTensor);
  TF_LITE_ENSURE(context, input != nullptr);
  data->input_zero_point = input->params.zero_point;
  TF_LITE_ENSURE_OK(context, CalculateArithmeticOpData(context, node, data));

  micro_context->DeallocateTempTfLiteTensor(input);
  return kTfLiteOk;
}

TfLiteStatus TanhEval(TfLiteContext* context, TfLiteNode* node) {
  const TfLiteEvalTensor* input =
      tflite::micro::GetEvalInput(context, node, kInputTensor);
  TfLiteEvalTensor* output =
      tflite::micro::GetEvalOutput(context, node, kOutputTensor);

  TFLITE_DCHECK(node->user_data != nullptr);
  const OpData& data = *(static_cast<const OpData*>(node->user_data));

  switch (input->type) {
    case kTfLiteFloat32: {
      reference_ops::Tanh(tflite::micro::GetTensorShape(input),
                          tflite::micro::GetTensorData<float>(input),
                          tflite::micro::GetTensorShape(output),
                          tflite::micro::GetTensorData<float>(output));
      return kTfLiteOk;
    } break;
    case kTfLiteInt16: {
      reference_integer_ops::Tanh(
          data.input_multiplier, data.input_left_shift,
          tflite::micro::GetTensorShape(input),
          tflite::micro::GetTensorData<int16_t>(input),
          tflite::micro::GetTensorShape(output),
          tflite::micro::GetTensorData<int16_t>(output));
      return kTfLiteOk;
    } break;
    case kTfLiteInt8: {
      //reference_integer_ops::Tanh(
      //    data.input_zero_point, data.input_range_radius, data.input_multiplier,
      //    data.input_left_shift, tflite::micro::GetTensorShape(input),
      //    tflite::micro::GetTensorData<int8_t>(input),
      //    tflite::micro::GetTensorShape(output),
      //    tflite::micro::GetTensorData<int8_t>(output));
      EvalUsingLookupTable(&data, input, output);
      return kTfLiteOk;
    } break;
    default:
      MicroPrintf("Input %s, output %s not supported.",
                  TfLiteTypeGetName(input->type),
                  TfLiteTypeGetName(output->type), context);
      return kTfLiteError;
  }
}

}  // namespace

TFLMRegistration Register_TANH() {
  return tflite::micro::RegisterOp(TanhInit, TanhPrepare, TanhEval);
}

}  // namespace tflite
