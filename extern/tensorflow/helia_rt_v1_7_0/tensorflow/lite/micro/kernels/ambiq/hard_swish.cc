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

#include "tensorflow/lite/kernels/internal/reference/hard_swish.h"

#include "Include/arm_nnsupportfunctions.h"
#include "Include/arm_nnfunctions.h"

#include "tensorflow/lite/c/builtin_op_data.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/common.h"
#include "tensorflow/lite/kernels/internal/quantization_util.h"
#include "tensorflow/lite/kernels/internal/tensor_ctypes.h"
#include "tensorflow/lite/kernels/internal/types.h"
#include "tensorflow/lite/kernels/kernel_util.h"
#include "tensorflow/lite/kernels/op_macros.h"
#include "tensorflow/lite/micro/kernels/hard_swish.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_utils.h"

namespace tflite {
namespace {

struct OpData {
  int16_t input_zero_point;
  int16_t output_zero_point;
  int16_t reluish_multiplier_fixedpoint_int16;
  int reluish_multiplier_exponent;
  int16_t output_multiplier_fixedpoint_int16;
  int output_multiplier_exponent;
  int32_t relu_3;
  int32_t relu_6;
  int32_t output_multiplier;
  int output_shift;
  int32_t prescale;
};

const int kHardSwishInputTensor = 0;
const int kHardSwishOutputTensor = 0;

void* Init(TfLiteContext* context, const char* buffer, size_t length) {
  TFLITE_DCHECK(context->AllocatePersistentBuffer != nullptr);
  return context->AllocatePersistentBuffer(context, sizeof(OpData));
}

TfLiteStatus Prepare(TfLiteContext* context, TfLiteNode* node) {
  MicroContext* micro_context = GetMicroContext(context);

  TFLITE_DCHECK(node->user_data != nullptr);
  TF_LITE_ENSURE_EQ(context, NumInputs(node), 1);
  TF_LITE_ENSURE_EQ(context, NumOutputs(node), 1);

  TfLiteTensor* input =
      micro_context->AllocateTempInputTensor(node, kHardSwishInputTensor);
  TF_LITE_ENSURE(context, input != nullptr);
  TfLiteTensor* output =
      micro_context->AllocateTempOutputTensor(node, kHardSwishOutputTensor);
  TF_LITE_ENSURE(context, output != nullptr);

  if (input->type == kTfLiteInt8) {
    OpData* data = static_cast<OpData*>(node->user_data);

    data->input_zero_point = input->params.zero_point;
    data->output_zero_point = output->params.zero_point;

    const float input_scale = input->params.scale;
    const float hires_input_scale = (1.0f / 128.0f) * input_scale;
    const float reluish_scale = 3.0f / 32768.0f;
    const float output_scale = output->params.scale;

    const double output_multiplier =
        static_cast<double>(hires_input_scale / output_scale);
    int32_t output_multiplier_fixedpoint_int32;
    QuantizeMultiplier(output_multiplier, &output_multiplier_fixedpoint_int32,
                       &data->output_multiplier_exponent);
    DownScaleInt32ToInt16Multiplier(
        output_multiplier_fixedpoint_int32,
        &data->output_multiplier_fixedpoint_int16);

    TF_LITE_ENSURE(context, data->output_multiplier_exponent <= 0);

    const double reluish_multiplier =
        static_cast<double>(hires_input_scale / reluish_scale);
    int32_t reluish_multiplier_fixedpoint_int32;
    QuantizeMultiplier(reluish_multiplier, &reluish_multiplier_fixedpoint_int32,
                       &data->reluish_multiplier_exponent);
    DownScaleInt32ToInt16Multiplier(
        reluish_multiplier_fixedpoint_int32,
        &data->reluish_multiplier_fixedpoint_int16);

  } else if (input->type == kTfLiteInt16) {

    OpData* data = static_cast<OpData*>(node->user_data);

    data->input_zero_point = input->params.zero_point;
    data->output_zero_point = output->params.zero_point;

    const float input_scale = input->params.scale;
    const float output_scale = output->params.scale;

    data->relu_3 = static_cast<int32_t>(3.0f / input_scale + 0.5f);
    data->relu_6 = static_cast<int32_t>(6.0f / input_scale + 0.5f);

    int64_t prod_max = 32767ll * (int64_t)data->relu_6;
    int k = 0;
    while (prod_max > INT32_MAX) { ++k; prod_max >>= 1; }
    data->prescale = k;

    const double M_real = static_cast<double>(input_scale * input_scale) /
                          (6.0*static_cast<double>(output_scale));
    const double M_adj = M_real * (1u << k);
    int32_t output_mult_s32;
    QuantizeMultiplier(M_adj, &output_mult_s32, &data->output_shift);

    data->output_multiplier = output_mult_s32;

  }

  micro_context->DeallocateTempTfLiteTensor(input);
  micro_context->DeallocateTempTfLiteTensor(output);

  return kTfLiteOk;
}

TfLiteStatus Eval(TfLiteContext* context, TfLiteNode* node) {
  const TfLiteEvalTensor* input =
      tflite::micro::GetEvalInput(context, node, kHardSwishInputTensor);
  TfLiteEvalTensor* output =
      tflite::micro::GetEvalOutput(context, node, kHardSwishOutputTensor);
  OpData* data = static_cast<OpData*>(node->user_data);

  switch (input->type) {
    case kTfLiteFloat32: {
      tflite::reference_ops::HardSwish<float>(
          tflite::micro::GetTensorShape(input),
          tflite::micro::GetTensorData<float>(input),
          tflite::micro::GetTensorShape(output),
          tflite::micro::GetTensorData<float>(output));
    } break;
    case kTfLiteInt16: {
      arm_hard_swish_precise_s16(
          tflite::micro::GetTensorData<int16_t>(input),
          data->input_zero_point,
          data->output_zero_point,
          data->output_multiplier,
          data->output_shift,
          data->relu_3,
          data->relu_6,
          data->prescale,
          tflite::micro::GetTensorData<int16_t>(output),
          tflite::micro::GetTensorShape(output).FlatSize()
      );

    } break;
    case kTfLiteInt8: {
      arm_hard_swish_compat_s8(
          tflite::micro::GetTensorData<int8_t>(input),
          data->input_zero_point,
          data->output_zero_point,
          (int32_t)data->output_multiplier_fixedpoint_int16,
          data->output_multiplier_exponent,
          data->reluish_multiplier_fixedpoint_int16,
          data->reluish_multiplier_exponent,
          tflite::micro::GetTensorData<int8_t>(output),
          tflite::micro::GetTensorShape(output).FlatSize()
      );
    } break;
    default: {
      MicroPrintf("Unsupported type %s", TfLiteTypeGetName(input->type));
      return kTfLiteError;
    }
  }
  return kTfLiteOk;
}

}  // namespace

TFLMRegistration Register_HARD_SWISH() {
  return tflite::micro::RegisterOp(Init, Prepare, Eval);
}

}  // namespace tflite
