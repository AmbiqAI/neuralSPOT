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

#include "tensorflow/lite/kernels/internal/reference/mul.h"

#include "Include/arm_nnfunctions.h"
#include "tensorflow/lite/kernels/internal/quantization_util.h"
#include "tensorflow/lite/kernels/internal/reference/integer_ops/mul.h"
#include "tensorflow/lite/kernels/internal/reference/process_broadcast_shapes.h"
#include "tensorflow/lite/kernels/internal/tensor_ctypes.h"
#include "tensorflow/lite/kernels/kernel_util.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"
#include "tensorflow/lite/micro/kernels/mul.h"
#include "tensorflow/lite/micro/memory_helpers.h"
#include "tensorflow/lite/micro/micro_log.h"

namespace tflite {
namespace {

void ArmPopulateCommonParams(const RuntimeShape& unextended_input1_shape,
                             const RuntimeShape& unextended_input2_shape,
                             const RuntimeShape& unextended_output_shape,
                             cmsis_nn_dims* const input1_dims,
                             cmsis_nn_dims* const input2_dims,
                             cmsis_nn_dims* const output_dims)
{

  TFLITE_DCHECK_LE(unextended_input1_shape.DimensionsCount(), 4);
  TFLITE_DCHECK_LE(unextended_input2_shape.DimensionsCount(), 4);
  TFLITE_DCHECK_LE(unextended_output_shape.DimensionsCount(), 4);
  const RuntimeShape input1_shape = RuntimeShape::ExtendedShape(4, unextended_input1_shape);
  const RuntimeShape input2_shape = RuntimeShape::ExtendedShape(4, unextended_input2_shape);
  const RuntimeShape output_shape = RuntimeShape::ExtendedShape(4, unextended_output_shape);

  input1_dims->n = input1_shape.Dims(0);
  input1_dims->h = input1_shape.Dims(1);
  input1_dims->w = input1_shape.Dims(2);
  input1_dims->c = input1_shape.Dims(3);
  input2_dims->n = input2_shape.Dims(0);
  input2_dims->h = input2_shape.Dims(1);
  input2_dims->w = input2_shape.Dims(2);
  input2_dims->c = input2_shape.Dims(3);
  output_dims->n = output_shape.Dims(0);
  output_dims->h = output_shape.Dims(1);
  output_dims->w = output_shape.Dims(2);
  output_dims->c = output_shape.Dims(3);
}


void EvalQuantized(TfLiteContext* context, TfLiteNode* node,
                   const OpDataMul* data, const TfLiteEvalTensor* input1,
                   const TfLiteEvalTensor* input2, TfLiteEvalTensor* output) {
  tflite::ArithmeticParams op_params = {};

  op_params.quantized_activation_min = data->output_activation_min;
  op_params.quantized_activation_max = data->output_activation_max;
  op_params.float_activation_max = data->output_activation_max_f32;
  op_params.input1_offset = -data->input1_zero_point;
  op_params.input2_offset = -data->input2_zero_point;
  op_params.output_offset = data->output_zero_point;
  op_params.output_multiplier = data->output_multiplier;
  op_params.output_shift = data->output_shift;

  const RuntimeShape input1_shape = tflite::micro::GetTensorShape(input1);
  const int32_t input1_dims_count = input1_shape.DimensionsCount();
  const RuntimeShape input2_shape = tflite::micro::GetTensorShape(input2);
  const int32_t input2_dims_count = input2_shape.DimensionsCount();

  if (input1_dims_count > 4 || input2_dims_count > 4) {
    if (input1->type == kTfLiteInt8) {
      reference_integer_ops::BroadcastMul4DSlow(
          op_params, tflite::micro::GetTensorShape(input1),
          tflite::micro::GetTensorData<int8_t>(input1),
          tflite::micro::GetTensorShape(input2),
          tflite::micro::GetTensorData<int8_t>(input2),
          tflite::micro::GetTensorShape(output),
          tflite::micro::GetTensorData<int8_t>(output));
    } else if (input1->type == kTfLiteInt16) {
      reference_integer_ops::BroadcastMul4DSlow(
          op_params, tflite::micro::GetTensorShape(input1),
          tflite::micro::GetTensorData<int16_t>(input1),
          tflite::micro::GetTensorShape(input2),
          tflite::micro::GetTensorData<int16_t>(input2),
          tflite::micro::GetTensorShape(output),
          tflite::micro::GetTensorData<int16_t>(output));
    }

  } else {

    cmsis_nn_dims input1_dims;
    cmsis_nn_dims input2_dims;
    cmsis_nn_dims output_dims;

    ArmPopulateCommonParams(
        tflite::micro::GetTensorShape(input1),
        tflite::micro::GetTensorShape(input2),
        tflite::micro::GetTensorShape(output),
        &input1_dims,
        &input2_dims,
        &output_dims
      );

    if (input1->type == kTfLiteInt8) {
      arm_mul_s8(
        tflite::micro::GetTensorData<int8_t>(input1),
        &input1_dims,
        tflite::micro::GetTensorData<int8_t>(input2),
        &input2_dims,
        op_params.input1_offset,
        op_params.input2_offset,
        tflite::micro::GetTensorData<int8_t>(output),
        &output_dims,
        op_params.output_offset,
        op_params.output_multiplier,
        op_params.output_shift,
        op_params.quantized_activation_min,
        op_params.quantized_activation_max
      );

    } else if (input1->type == kTfLiteInt16) {
      arm_mul_s16(
        tflite::micro::GetTensorData<int16_t>(input1),
        &input1_dims,
        tflite::micro::GetTensorData<int16_t>(input2),
        &input2_dims,
        op_params.input1_offset,
        op_params.input2_offset,
        tflite::micro::GetTensorData<int16_t>(output),
        &output_dims,
        op_params.output_offset,
        op_params.output_multiplier,
        op_params.output_shift,
        op_params.quantized_activation_min,
        op_params.quantized_activation_max
      );
    }
  }
}

}  // namespace

TfLiteStatus Eval(TfLiteContext* context, TfLiteNode* node) {
  TFLITE_DCHECK(node->builtin_data != nullptr);
  auto* params = reinterpret_cast<TfLiteMulParams*>(node->builtin_data);

  TFLITE_DCHECK(node->user_data != nullptr);
  const OpDataMul* data = static_cast<const OpDataMul*>(node->user_data);

  const TfLiteEvalTensor* input1 =
      tflite::micro::GetEvalInput(context, node, kMulInput1Tensor);
  const TfLiteEvalTensor* input2 =
      tflite::micro::GetEvalInput(context, node, kMulInput2Tensor);
  TfLiteEvalTensor* output =
      tflite::micro::GetEvalOutput(context, node, kMulOutputTensor);

  switch (input1->type) {
    case kTfLiteInt8:
      EvalQuantized(context, node, data, input1, input2, output);
      break;
    case kTfLiteInt16:
      EvalQuantized(context, node, data, input1, input2, output);
      break;
    case kTfLiteInt32:
      EvalMulQuantizedReference(context, node, data, input1, input2, output);
      break;
    case kTfLiteFloat32:
      EvalMulFloatReference(context, node, params, data, input1, input2,
                            output);
      break;
    default:
      MicroPrintf("Type %s (%d) not supported.",
                  TfLiteTypeGetName(input1->type), input1->type);
      return kTfLiteError;
  }

  return kTfLiteOk;
}

TfLiteStatus EvalInt8(TfLiteContext* context, TfLiteNode* node) {
  TFLITE_DCHECK(node->builtin_data != nullptr);
  TFLITE_DCHECK(node->user_data != nullptr);

  const OpDataMul* data = static_cast<const OpDataMul*>(node->user_data);
  const TfLiteEvalTensor* input1 =
      tflite::micro::GetEvalInput(context, node, kMulInput1Tensor);
  const TfLiteEvalTensor* input2 =
      tflite::micro::GetEvalInput(context, node, kMulInput2Tensor);
  TfLiteEvalTensor* output =
      tflite::micro::GetEvalOutput(context, node, kMulOutputTensor);
  TFLITE_DCHECK(input1->type == kTfLiteInt8);

  EvalQuantized(context, node, data, input1, input2, output);

  return kTfLiteOk;
}

TfLiteStatus EvalInt16(TfLiteContext* context, TfLiteNode* node) {
  TFLITE_DCHECK(node->builtin_data != nullptr);
  TFLITE_DCHECK(node->user_data != nullptr);

  const OpDataMul* data = static_cast<const OpDataMul*>(node->user_data);
  const TfLiteEvalTensor* input1 =
      tflite::micro::GetEvalInput(context, node, kMulInput1Tensor);
  const TfLiteEvalTensor* input2 =
      tflite::micro::GetEvalInput(context, node, kMulInput2Tensor);
  TfLiteEvalTensor* output =
      tflite::micro::GetEvalOutput(context, node, kMulOutputTensor);
  TFLITE_DCHECK(input1->type == kTfLiteInt16);

  EvalQuantized(context, node, data, input1, input2, output);

  return kTfLiteOk;
}

TFLMRegistration Register_MUL() {
  return tflite::micro::RegisterOp(MulInit, MulPrepare, Eval);
}

TFLMRegistration Register_MUL_INT8() {
  return tflite::micro::RegisterOp(MulInit, MulPrepare, EvalInt8);
}

TFLMRegistration Register_MUL_INT16() {
  return tflite::micro::RegisterOp(MulInit, MulPrepare, EvalInt16);
}

}  // namespace tflite
