/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

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

#include "Include/arm_nnsupportfunctions.h"

#include <limits>
#include <vector>
#include <algorithm>


#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/quantization_util.h"
#include "tensorflow/lite/kernels/internal/reference/quantize.h"
#include "tensorflow/lite/kernels/internal/reference/requantize.h"
#include "tensorflow/lite/kernels/internal/tensor_ctypes.h"
#include "tensorflow/lite/kernels/kernel_util.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"
#include "tensorflow/lite/micro/kernels/quantize.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_utils.h"

namespace tflite {

TfLiteStatus PrepareQuantizeReference(TfLiteContext* context,
                                      TfLiteNode* node) {
  TFLITE_DCHECK(node->user_data != nullptr);
  auto* data = static_cast<OpDataQuantizeReference*>(node->user_data);

  TF_LITE_ENSURE_EQ(context, NumInputs(node), 1);
  TF_LITE_ENSURE_EQ(context, NumOutputs(node), 1);

  MicroContext* micro_context = GetMicroContext(context);

  TfLiteTensor* input = micro_context->AllocateTempInputTensor(node, 0);
  TF_LITE_ENSURE(context, input != nullptr);
  TfLiteTensor* output = micro_context->AllocateTempOutputTensor(node, 0);
  TF_LITE_ENSURE(context, output != nullptr);

  // TODO(b/128934713): Add support for fixed-point per-channel quantization.
  // Currently this only support affine per-layer quantization.
  TF_LITE_ENSURE_EQ(context, output->quantization.type,
                    kTfLiteAffineQuantization);
  const auto* affine_quantization =
      reinterpret_cast<TfLiteAffineQuantization*>(output->quantization.params);
  TF_LITE_ENSURE(context, affine_quantization);
  TF_LITE_ENSURE(context, affine_quantization->scale);
  TF_LITE_ENSURE(context, affine_quantization->scale->size == 1);

  TF_LITE_ENSURE(
      context, input->type == kTfLiteFloat32 || input->type == kTfLiteInt32 ||
                   input->type == kTfLiteInt16 || input->type == kTfLiteInt8 ||
                   input->type == kTfLiteUInt8);
  TF_LITE_ENSURE(context, output->type == kTfLiteInt8 ||
                              output->type == kTfLiteInt16 ||
                              output->type == kTfLiteInt32 ||
                              output->type == kTfLiteUInt8);

  if ((input->type == kTfLiteInt16 && output->type == kTfLiteInt8) ||
      (input->type == kTfLiteInt8 && output->type == kTfLiteInt8) ||
      (input->type == kTfLiteInt8 && output->type == kTfLiteUInt8) ||
      (input->type == kTfLiteUInt8 && output->type == kTfLiteInt8) ||
      (input->type == kTfLiteInt8 && output->type == kTfLiteInt16) ||
      (input->type == kTfLiteInt8 && output->type == kTfLiteInt32) ||
      (input->type == kTfLiteInt16 && output->type == kTfLiteInt16) ||
      (input->type == kTfLiteInt16 && output->type == kTfLiteInt32) ||
      (input->type == kTfLiteInt32 && output->type == kTfLiteInt8) ||
      (input->type == kTfLiteInt32 && output->type == kTfLiteInt16)) {
    double effective_scale = static_cast<double>(input->params.scale) /
                             static_cast<double>(output->params.scale);

    QuantizeMultiplier(effective_scale, &data->requantize_output_multiplier,
                       &data->requantize_output_shift);
  }

  data->quantization_params.zero_point = output->params.zero_point;
  data->quantization_params.scale = static_cast<double>(output->params.scale);

  data->input_zero_point = input->params.zero_point;

  micro_context->DeallocateTempTfLiteTensor(input);
  micro_context->DeallocateTempTfLiteTensor(output);
  return kTfLiteOk;
}

void
arm_quantize_f32_s8(
  const float* input,
  int8_t* output,
  int32_t size,
  int32_t zero_point,
  float scale
) {
#if defined(ARM_MATH_MVEI)
  int32_t count = (size + 3) / 4;
  float mul_scale = 1.0f / scale;
  int32x4_t max = vdupq_n_s32(INT8_MAX);
  int32x4_t min = vdupq_n_s32(INT8_MIN);
  for (int i = 0; i < count; i++) {
    mve_pred16_t pred = vctp32q(size);
    size -= 4;
    float32x4_t vals = vldrwq_z_f32(input, pred);
    int32x4_t rounded = vcvtaq_s32_f32(vrndnq_f32(vmulq_n_f32(vals, mul_scale)));
    int32x4_t shifted = vaddq_n_s32(rounded, zero_point);
    int32x4_t clamped = vminq_s32(vmaxq_s32(shifted, min), max);
    vstrbq_p_s32(output, clamped, pred);
    input += 4;
    output += 4;
  }
#else
  for (int i = 0; i < size; i++) {
    float val = input[i];
    int32_t rounded = static_cast<int32_t>(std::round(val / scale)) + zero_point;
    int32_t clamped = CLAMP(rounded, INT8_MAX, INT8_MIN);
    output[i] = static_cast<int8_t>(clamped);
  }
#endif
}

void
arm_quantize_f32_s16(
  const float* input,
  int16_t* output,
  int32_t size,
  int32_t zero_point,
  float scale
) {
#if defined(ARM_MATH_MVEI)
  int32_t count = (size + 3) / 4;
  float mul_scale = 1.0f / scale;
  int32x4_t max = vdupq_n_s32(32767);
  int32x4_t min = vdupq_n_s32(-32768);
  for (int i = 0; i < count; i++) {
    mve_pred16_t pred = vctp32q(size);
    size -= 4;
    float32x4_t vals = vldrwq_z_f32(input, pred);
    int32x4_t rounded = vcvtaq_s32_f32(vrndnq_f32(vmulq_n_f32(vals, mul_scale)));
    int32x4_t shifted = vaddq_n_s32(rounded, zero_point);
    int32x4_t clamped = vminq_s32(vmaxq_s32(shifted, min), max);
    vstrhq_p_s32(output, clamped, pred);
    input += 4;
    output += 4;
  }
#else
  for (int i = 0; i < size; i++) {
    float val = input[i];
    int32_t rounded = static_cast<int32_t>(std::round(val / scale)) + zero_point;
    int32_t clamped = CLAMP(rounded, INT16_MAX, INT16_MIN);
    output[i] = static_cast<int16_t>(clamped);
  }
#endif
}

void
arm_requantize_s8_s8(
  const int8_t *input,
  int8_t* output,
  int32_t size,
  int32_t effective_scale_multiplier,
  int32_t effective_scale_shift,
  int32_t input_zeropoint,
  int32_t output_zeropoint
) {

#if defined(ARM_MATH_MVEI)
  int32_t count = (size + 3) / 4;
  int32x4_t max = vdupq_n_s32(INT8_MAX);
  int32x4_t min = vdupq_n_s32(INT8_MIN);
  for (int i = 0; i < count; i++) {
    mve_pred16_t pred = vctp32q(size);
    size -= 4;
    int32x4_t vals = vldrbq_z_s32(input, pred);
    vals = vaddq_n_s32(vals, -input_zeropoint);
    vals = arm_requantize_mve(vals, effective_scale_multiplier, effective_scale_shift);
    int32x4_t shifted = vaddq_n_s32(vals, output_zeropoint);
    int32x4_t clamped = vminq_s32(vmaxq_s32(shifted, min), max);
    vstrbq_p_s32(output, clamped, pred);
    input += 4;
    output += 4;
  }
#else
  for (int i = 0; i < size; i++) {
    int32_t val = input[i] - input_zeropoint;
    val = arm_nn_requantize(val, effective_scale_multiplier, effective_scale_shift);
    val += output_zeropoint;
    output[i] = CLAMP(val, INT8_MAX, INT8_MIN);
  }
#endif
}

void
arm_requantize_s16_s16(
  const int16_t *input,
  int16_t* output,
  int32_t size,
  int32_t effective_scale_multiplier,
  int32_t effective_scale_shift,
  int32_t input_zeropoint,
  int32_t output_zeropoint
) {

#if defined(ARM_MATH_MVEI)
  int32_t count = (size + 3) / 4;
  int32x4_t max = vdupq_n_s32(INT16_MAX);
  int32x4_t min = vdupq_n_s32(INT16_MIN);
  for (int i = 0; i < count; i++) {
    mve_pred16_t pred = vctp32q(size);
    size -= 4;
    int32x4_t vals = vldrhq_z_s32(input, pred);
    vals = vaddq_n_s32(vals, -input_zeropoint);
    vals = arm_requantize_mve(vals, effective_scale_multiplier, effective_scale_shift);
    int32x4_t shifted = vaddq_n_s32(vals, output_zeropoint);
    int32x4_t clamped = vminq_s32(vmaxq_s32(shifted, min), max);
    vstrhq_p_s32(output, clamped, pred);
    input += 4;
    output += 4;
  }
#else
  for (int i = 0; i < size; i++) {
    int32_t val = input[i] - input_zeropoint;
    val = arm_nn_requantize(val, effective_scale_multiplier, effective_scale_shift);
    val += output_zeropoint;
    output[i] = CLAMP(val, INT16_MAX, INT16_MIN);
  }
#endif
}

TfLiteStatus EvalQuantizeReference(TfLiteContext* context, TfLiteNode* node) {
  TFLITE_DCHECK(node->user_data != nullptr);
  auto* data = static_cast<OpDataQuantizeReference*>(node->user_data);

  const TfLiteEvalTensor* input = tflite::micro::GetEvalInput(context, node, 0);
  TfLiteEvalTensor* output = tflite::micro::GetEvalOutput(context, node, 0);

  if (input->type == kTfLiteFloat32) {
    switch (output->type) {
      case kTfLiteInt8:
        arm_quantize_f32_s8(
          tflite::micro::GetTensorData<float>(input),
          tflite::micro::GetTensorData<int8_t>(output),
          ElementCount(*input->dims),
          data->quantization_params.zero_point,
          data->quantization_params.scale
        );
        break;
      case kTfLiteInt16:
        arm_quantize_f32_s16(
          tflite::micro::GetTensorData<float>(input),
          tflite::micro::GetTensorData<int16_t>(output),
          ElementCount(*input->dims),
          data->quantization_params.zero_point,
          data->quantization_params.scale
        );
        break;
      default:
        MicroPrintf("Input %s, output %s not supported.",
                    TfLiteTypeGetName(input->type),
                    TfLiteTypeGetName(output->type));
        return kTfLiteError;
    }
  } else if (input->type == kTfLiteInt32) {
    size_t size = ElementCount(*input->dims);
    switch (output->type) {
      case kTfLiteInt8:
        reference_ops::Requantize(
            tflite::micro::GetTensorData<int32_t>(input), size,
            data->requantize_output_multiplier, data->requantize_output_shift,
            data->input_zero_point, data->quantization_params.zero_point,
            tflite::micro::GetTensorData<int8_t>(output));
        break;
      case kTfLiteInt16:
        reference_ops::Requantize(
            tflite::micro::GetTensorData<int32_t>(input), size,
            data->requantize_output_multiplier, data->requantize_output_shift,
            data->input_zero_point, data->quantization_params.zero_point,
            tflite::micro::GetTensorData<int16_t>(output));
        break;
      default:
        MicroPrintf("Input %s, output %s not supported.",
                    TfLiteTypeGetName(input->type),
                    TfLiteTypeGetName(output->type));
        return kTfLiteError;
    }
  } else if (input->type == kTfLiteInt16) {
    size_t size = ElementCount(*input->dims);
    switch (output->type) {
      case kTfLiteInt8:
        reference_ops::Requantize(
            tflite::micro::GetTensorData<int16_t>(input), size,
            data->requantize_output_multiplier, data->requantize_output_shift,
            data->input_zero_point, data->quantization_params.zero_point,
            tflite::micro::GetTensorData<int8_t>(output));
        break;
      case kTfLiteInt16:
        arm_requantize_s16_s16(
          tflite::micro::GetTensorData<int16_t>(input),
          tflite::micro::GetTensorData<int16_t>(output),
          size,
          data->requantize_output_multiplier,
          data->requantize_output_shift,
          data->input_zero_point,
          data->quantization_params.zero_point
        );
        break;
      case kTfLiteInt32:
        reference_ops::Requantize(
            tflite::micro::GetTensorData<int16_t>(input), size,
            data->requantize_output_multiplier, data->requantize_output_shift,
            data->input_zero_point, data->quantization_params.zero_point,
            tflite::micro::GetTensorData<int32_t>(output));
        return kTfLiteOk;
      default:
        MicroPrintf("Input %s, output %s not supported.",
                    TfLiteTypeGetName(input->type),
                    TfLiteTypeGetName(output->type));
        return kTfLiteError;
    }
  } else if (input->type == kTfLiteInt8) {
    // Int8 to Int8 requantization, required if the input and output tensors
    // have different scales and/or zero points.
    size_t size = ElementCount(*input->dims);
    switch (output->type) {
      case kTfLiteInt8:
        arm_requantize_s8_s8(
          tflite::micro::GetTensorData<int8_t>(input),
          tflite::micro::GetTensorData<int8_t>(output),
          size,
          data->requantize_output_multiplier,
          data->requantize_output_shift,
          data->input_zero_point,
          data->quantization_params.zero_point
        );
        break;
        // reference_ops::Requantize(
        //     tflite::micro::GetTensorData<int8_t>(input), size,
        //     data->requantize_output_multiplier, data->requantize_output_shift,
        //     data->input_zero_point, data->quantization_params.zero_point,
        //     tflite::micro::GetTensorData<int8_t>(output));
        // break;
      case kTfLiteUInt8:
        reference_ops::Requantize(
            tflite::micro::GetTensorData<int8_t>(input), size,
            data->requantize_output_multiplier, data->requantize_output_shift,
            data->input_zero_point, data->quantization_params.zero_point,
            tflite::micro::GetTensorData<uint8_t>(output));
        break;
      case kTfLiteInt16:
        reference_ops::Requantize(
            tflite::micro::GetTensorData<int8_t>(input), size,
            data->requantize_output_multiplier, data->requantize_output_shift,
            data->input_zero_point, data->quantization_params.zero_point,
            tflite::micro::GetTensorData<int16_t>(output));
        break;
      case kTfLiteInt32:
        reference_ops::Requantize(
            tflite::micro::GetTensorData<int8_t>(input), size,
            data->requantize_output_multiplier, data->requantize_output_shift,
            data->input_zero_point, data->quantization_params.zero_point,
            tflite::micro::GetTensorData<int32_t>(output));
        break;
      default:
        MicroPrintf("Input %s, output %s not supported.",
                    TfLiteTypeGetName(input->type),
                    TfLiteTypeGetName(output->type));
        return kTfLiteError;
    }
  } else if (input->type == kTfLiteUInt8) {
    size_t size = ElementCount(*input->dims);
    switch (output->type) {
      case kTfLiteInt8:
        reference_ops::Requantize(
            tflite::micro::GetTensorData<uint8_t>(input), size,
            data->requantize_output_multiplier, data->requantize_output_shift,
            data->input_zero_point, data->quantization_params.zero_point,
            tflite::micro::GetTensorData<int8_t>(output));
        break;
      default:
        MicroPrintf("Input %s, output %s not supported.",
                    TfLiteTypeGetName(input->type),
                    TfLiteTypeGetName(output->type));
        return kTfLiteError;
    }
  } else {
    MicroPrintf("Input %s, output %s not supported.",
                TfLiteTypeGetName(input->type),
                TfLiteTypeGetName(output->type));
    return kTfLiteError;
  }

  return kTfLiteOk;
}

}  // namespace tflite
