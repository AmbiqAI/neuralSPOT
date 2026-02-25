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
#include "tensorflow/lite/kernels/internal/reference/strided_slice.h"

#include <cstdint>
#include <cstring>

#include "Include/arm_nnfunctions.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/tensor_ctypes.h"
#include "tensorflow/lite/kernels/kernel_util.h"
#include "tensorflow/lite/kernels/op_macros.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"
#include "tensorflow/lite/micro/kernels/strided_slice.h"
#include "tensorflow/lite/micro/micro_log.h"

namespace tflite {

namespace {

void ArmPopulateCommonParams(const tflite::StridedSliceParams& op_params,
                             const RuntimeShape& unextended_input_shape,
                             const RuntimeShape& unextended_output_shape,
                             cmsis_nn_dims* const input_dims,
                             cmsis_nn_dims* const output_dims,
                             cmsis_nn_dims* const begin_dims,
                             cmsis_nn_dims* const stride_dims)
{

  using ::tflite::strided_slice::StartForAxis;
  using ::tflite::strided_slice::StopForAxis;

  ruy::profiler::ScopeLabel label("StridedSlice");

  tflite::StridedSliceParams params_copy = op_params;

  TFLITE_DCHECK_LE(unextended_input_shape.DimensionsCount(), 4);
  TFLITE_DCHECK_LE(unextended_output_shape.DimensionsCount(), 4);
  const RuntimeShape input_shape = RuntimeShape::ExtendedShape(4, unextended_input_shape);
  const RuntimeShape output_shape = RuntimeShape::ExtendedShape(4, unextended_output_shape);

  // Reverse and pad to 4 dimensions
  ::tflite::strided_slice::StridedSlicePadIndices(&params_copy, 4);

  const int start_0 = StartForAxis(params_copy, input_shape, 0);
  // const int stop_0 = StopForAxis(params_copy, input_shape, 0, start_0);
  const int start_1 = StartForAxis(params_copy, input_shape, 1);
  // const int stop_1 = StopForAxis(params_copy, input_shape, 1, start_1);
  const int start_2 = StartForAxis(params_copy, input_shape, 2);
  // const int stop_2 = StopForAxis(params_copy, input_shape, 2, start_2);
  const int start_3 = StartForAxis(params_copy, input_shape, 3);
  // const int stop_3 = StopForAxis(params_copy, input_shape, 3, start_3);

  input_dims->n = input_shape.Dims(0);
  input_dims->h = input_shape.Dims(1);
  input_dims->w = input_shape.Dims(2);
  input_dims->c = input_shape.Dims(3);

  output_dims->n = output_shape.Dims(0);
  output_dims->h = output_shape.Dims(1);
  output_dims->w = output_shape.Dims(2);
  output_dims->c = output_shape.Dims(3);

  begin_dims->n = start_0;
  begin_dims->h = start_1;
  begin_dims->w = start_2;
  begin_dims->c = start_3;

  stride_dims->n = params_copy.strides[0];
  stride_dims->h = params_copy.strides[1];
  stride_dims->w = params_copy.strides[2];
  stride_dims->c = params_copy.strides[3];

}


int32_t
ArmStridedSliceS8(
  const tflite::StridedSliceParams& op_params,
  const RuntimeShape& unextended_input_shape,
  const int8_t* input_data,
  const RuntimeShape& unextended_output_shape,
  int8_t* output_data
) {
  cmsis_nn_dims input_dims;
  cmsis_nn_dims output_dims;
  cmsis_nn_dims begin_dims;
  cmsis_nn_dims stride_dims;

  ArmPopulateCommonParams(
    op_params,
    unextended_input_shape,
    unextended_output_shape,
    &input_dims,
    &output_dims,
    &begin_dims,
    &stride_dims
  );

  return arm_strided_slice_s8(
    input_data,
    output_data,
    &input_dims,
    &begin_dims,
    &stride_dims,
    &output_dims
  );
}

int32_t
ArmStridedSliceS16(
  const tflite::StridedSliceParams& op_params,
  const RuntimeShape& unextended_input_shape,
  const int16_t* input_data,
  const RuntimeShape& unextended_output_shape,
  int16_t* output_data
) {

  cmsis_nn_dims input_dims;
  cmsis_nn_dims output_dims;
  cmsis_nn_dims begin_dims;
  cmsis_nn_dims stride_dims;

  ArmPopulateCommonParams(
    op_params,
    unextended_input_shape,
    unextended_output_shape,
    &input_dims,
    &output_dims,
    &begin_dims,
    &stride_dims
  );

  return arm_strided_slice_s16(
    input_data,
    output_data,
    &input_dims,
    &begin_dims,
    &stride_dims,
    &output_dims
  );
}

TfLiteStatus StridedSliceEval(TfLiteContext* context, TfLiteNode* node) {
  TFLITE_DCHECK(node->user_data != nullptr);
  const StridedSliceParams& op_params =
      *(static_cast<const StridedSliceParams*>(node->user_data));

  const TfLiteEvalTensor* input =
      tflite::micro::GetEvalInput(context, node, kStridedSliceInputTensor);
  TfLiteEvalTensor* output =
      tflite::micro::GetEvalOutput(context, node, kStridedSliceOutputTensor);

  const RuntimeShape input_shape = tflite::micro::GetTensorShape(input);
  const int32_t input_dims_count = input_shape.DimensionsCount();

  switch (output->type) {
    case kTfLiteFloat32:
      reference_ops::StridedSlice(
        op_params,
        tflite::micro::GetTensorShape(input),
        tflite::micro::GetTensorData<float>(input),
        tflite::micro::GetTensorShape(output),
        tflite::micro::GetTensorData<float>(output)
      );
      break;
    case kTfLiteInt8:
      if (input_dims_count <= 4) {
        ArmStridedSliceS8(
          op_params,
          tflite::micro::GetTensorShape(input),
          tflite::micro::GetTensorData<int8_t>(input),
          tflite::micro::GetTensorShape(output),
          tflite::micro::GetTensorData<int8_t>(output)
        );
      }
      else {
        reference_ops::StridedSlice(
          op_params,
          tflite::micro::GetTensorShape(input),
          tflite::micro::GetTensorData<int8_t>(input),
          tflite::micro::GetTensorShape(output),
          tflite::micro::GetTensorData<int8_t>(output)
        );
      }
      break;
    case kTfLiteInt16:
      if (input_dims_count <= 4) {
        ArmStridedSliceS16(
          op_params,
          tflite::micro::GetTensorShape(input),
          tflite::micro::GetTensorData<int16_t>(input),
          tflite::micro::GetTensorShape(output),
          tflite::micro::GetTensorData<int16_t>(output)
        );
      }
      else {
        reference_ops::StridedSlice(
          op_params,
          tflite::micro::GetTensorShape(input),
          tflite::micro::GetTensorData<int16_t>(input),
          tflite::micro::GetTensorShape(output),
          tflite::micro::GetTensorData<int16_t>(output)
        );
      }
      break;
    case kTfLiteInt32:
      reference_ops::StridedSlice(
        op_params,
        tflite::micro::GetTensorShape(input),
        tflite::micro::GetTensorData<int32_t>(input),
        tflite::micro::GetTensorShape(output),
        tflite::micro::GetTensorData<int32_t>(output)
      );
      break;
    case kTfLiteBool:
      reference_ops::StridedSlice(
        op_params,
        tflite::micro::GetTensorShape(input),
        tflite::micro::GetTensorData<bool>(input),
        tflite::micro::GetTensorShape(output),
        tflite::micro::GetTensorData<bool>(output)
      );
      break;
    default:
      MicroPrintf("Type %s (%d) not supported.", TfLiteTypeGetName(input->type),
                  input->type);
      return kTfLiteError;
  }
  return kTfLiteOk;
}

}  // namespace

TFLMRegistration Register_STRIDED_SLICE() {
  return tflite::micro::RegisterOp(StridedSliceInit, StridedSlicePrepare, StridedSliceEval);
}

}  // namespace tflite
