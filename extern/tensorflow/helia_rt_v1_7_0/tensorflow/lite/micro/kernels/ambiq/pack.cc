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
#include "Include/arm_nnsupportfunctions.h"
#include "Include/arm_nnfunctions.h"

#include "tensorflow/lite/c/builtin_op_data.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/tensor_ctypes.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"
#include "tensorflow/lite/micro/micro_log.h"

namespace tflite {

namespace {

constexpr int kOutputTensor = 0;
constexpr int kMaxPackRank = 6;   // R+1 (PACK output rank) must be <= 6
constexpr int kMaxPackInputs = 16;  // cap so we can use small stack arrays

template <typename T>
TfLiteStatus PackImpl(TfLiteContext* context, TfLiteNode* node,
                      TfLiteEvalTensor* output, int values_count, int axis) {
  const TfLiteEvalTensor* input0 =
      tflite::micro::GetEvalInput(context, node, 0);

  const int dimensions = output->dims->size;
  const TfLiteIntArray* input_dims = input0->dims;
  const TfLiteIntArray* output_dims = output->dims;

  if (axis < 0) {
    axis += dimensions;
  }

  int outer_size = 1;
  for (int i = 0; i < axis; ++i) {
    outer_size *= output_dims->data[i];
  }
  int copy_size = 1;
  for (int i = axis + 1; i < dimensions; ++i) {
    copy_size *= output_dims->data[i];
  }
  int input_size = 1;
  for (int i = 0; i < input_dims->size; ++i) {
    input_size *= input_dims->data[i];
  }
  TFLITE_DCHECK_EQ(input_size, copy_size * outer_size);

  T* output_data = tflite::micro::GetTensorData<T>(output);

  for (int i = 0; i < values_count; ++i) {
    const TfLiteEvalTensor* t = tflite::micro::GetEvalInput(context, node, i);
    const T* input_data = tflite::micro::GetTensorData<T>(t);
    for (int k = 0; k < outer_size; ++k) {
      const T* input_ptr = input_data + copy_size * k;
      int loc = k * values_count * copy_size + i * copy_size;
      T* output_ptr = output_data + loc;
      for (int j = 0; j < copy_size; ++j) output_ptr[j] = input_ptr[j];
    }
  }

  return kTfLiteOk;
}

static TfLiteStatus PackImplConcatS8(TfLiteContext* context, TfLiteNode* node,
                                     TfLiteEvalTensor* output,
                                     int values_count, int axis) {
  const int out_rank = output->dims->size;
  if (out_rank > kMaxPackRank) {
    MicroPrintf("PACK(s8): output rank %d > %d not supported by concat fastpath",
                out_rank, kMaxPackRank);
    return kTfLiteError;
  }
  if (values_count > kMaxPackInputs) {
    MicroPrintf("PACK(s8): inputs %d > %d not supported by concat fastpath",
                values_count, kMaxPackInputs);
    return kTfLiteError;
  }
  if (axis < 0) axis += out_rank;

  // Build arrays for concat
  const int8_t* in_ptrs[kMaxPackInputs];
  int32_t       input_concat_dims[kMaxPackInputs];  // all 1s
  int32_t       out_shape[kMaxPackRank];

  for (int i = 0; i < values_count; ++i) {
    const TfLiteEvalTensor* ti = tflite::micro::GetEvalInput(context, node, i);
    in_ptrs[i] = tflite::micro::GetTensorData<int8_t>(ti);
    input_concat_dims[i] = 1;
  }
  for (int d = 0; d < out_rank; ++d) out_shape[d] = output->dims->data[d];

  int8_t* out_ptr = tflite::micro::GetTensorData<int8_t>(output);
  const arm_cmsis_nn_status st =
      arm_concatenation_s8(in_ptrs, values_count, input_concat_dims,
                           axis, out_ptr, out_rank, out_shape);

  return (st == ARM_CMSIS_NN_SUCCESS) ? kTfLiteOk : kTfLiteError;
}

static TfLiteStatus PackImplConcatS16(TfLiteContext* context, TfLiteNode* node,
                                      TfLiteEvalTensor* output,
                                      int values_count, int axis) {
  const int out_rank = output->dims->size;
  if (out_rank > kMaxPackRank) {
    MicroPrintf("PACK(s16): output rank %d > %d not supported by concat fastpath",
                out_rank, kMaxPackRank);
    return kTfLiteError;
  }
  if (values_count > kMaxPackInputs) {
    MicroPrintf("PACK(s16): inputs %d > %d not supported by concat fastpath",
                values_count, kMaxPackInputs);
    return kTfLiteError;
  }
  if (axis < 0) axis += out_rank;

  const int16_t* in_ptrs[kMaxPackInputs];
  int32_t        input_concat_dims[kMaxPackInputs]; // all 1s
  int32_t        out_shape[kMaxPackRank];

  for (int i = 0; i < values_count; ++i) {
    const TfLiteEvalTensor* ti = tflite::micro::GetEvalInput(context, node, i);
    in_ptrs[i] = tflite::micro::GetTensorData<int16_t>(ti);
    input_concat_dims[i] = 1;
  }
  for (int d = 0; d < out_rank; ++d) out_shape[d] = output->dims->data[d];

  int16_t* out_ptr = tflite::micro::GetTensorData<int16_t>(output);
  const arm_cmsis_nn_status st =
      arm_concatenation_s16(in_ptrs, values_count, input_concat_dims,
                            axis, out_ptr, out_rank, out_shape);

  return (st == ARM_CMSIS_NN_SUCCESS) ? kTfLiteOk : kTfLiteError;
}

TfLiteStatus PackEval(TfLiteContext* context, TfLiteNode* node) {
  const TfLitePackParams* data =
      reinterpret_cast<TfLitePackParams*>(node->builtin_data);
  TfLiteEvalTensor* output = tflite::micro::GetEvalOutput(context, node, kOutputTensor);

  switch (output->type) {
    case kTfLiteInt8: {
      if (PackImplConcatS8(context, node, output, data->values_count, data->axis) == kTfLiteOk)
        return kTfLiteOk;
      return PackImpl<int8_t>(context, node, output, data->values_count, data->axis);
    }
    case kTfLiteInt16: {
      if (PackImplConcatS16(context, node, output, data->values_count, data->axis) == kTfLiteOk)
        return kTfLiteOk;
      return PackImpl<int16_t>(context, node, output, data->values_count, data->axis);
    }
    case kTfLiteFloat32:
      return PackImpl<float>(context, node, output, data->values_count, data->axis);
    case kTfLiteInt32:
      return PackImpl<int32_t>(context, node, output, data->values_count, data->axis);
    case kTfLiteInt64:
      return PackImpl<int64_t>(context, node, output, data->values_count, data->axis);
    default:
      MicroPrintf("Type '%s' is not supported by pack.", TfLiteTypeGetName(output->type));
      return kTfLiteError;
  }
}

}  // namespace

TFLMRegistration Register_PACK() {
  return tflite::micro::RegisterOp(nullptr, nullptr, PackEval);
}

}  // namespace tflite
