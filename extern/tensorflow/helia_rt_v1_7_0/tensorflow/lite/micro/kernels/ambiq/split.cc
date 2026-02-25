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

#include "Include/arm_nnsupportfunctions.h"
#include "Include/arm_nnfunctions.h"

#include "tensorflow/lite/c/builtin_op_data.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/tensor_ctypes.h"
#include "tensorflow/lite/kernels/kernel_util.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"
#include "tensorflow/lite/micro/micro_log.h"

namespace tflite {

namespace {

constexpr int kAxisTensor = 0;
constexpr int kInputTensor = 1;

// Fast track limits
constexpr int kMaxSplitRank = 6;
constexpr int kMaxSplitOutputs = 16;

template <typename T>
TfLiteStatus SplitImplRef(TfLiteContext* context, TfLiteNode* node,
                          const TfLiteEvalTensor* input, int axis_value) {
  const int output_count = NumOutputs(node);
  const TfLiteIntArray* in_dims   = input->dims;
  const TfLiteEvalTensor* out0    = tflite::micro::GetEvalOutput(context, node, 0);
  const TfLiteIntArray* out_dims  = out0->dims;

  const int rank = in_dims->size;
  const int axis = (axis_value < 0) ? axis_value + rank : axis_value;

  TFLITE_DCHECK_LT(axis, rank);
  TFLITE_DCHECK_EQ(out_dims->size, rank);

  const int64_t split_size = static_cast<int64_t>(out_dims->data[axis]) * output_count;
  TFLITE_DCHECK_EQ(split_size, in_dims->data[axis]);

  int64_t outer = 1;
  for (int i = 0; i < axis; ++i) outer *= in_dims->data[i];

  int64_t base_inner = 1;
  for (int i = axis + 1; i < rank; ++i) base_inner *= in_dims->data[i];

  const T* in_ptr = tflite::micro::GetTensorData<T>(input);
  for (int k = 0; k < outer; ++k) {
    for (int i = 0; i < output_count; ++i) {
      TfLiteEvalTensor* out_t = tflite::micro::GetEvalOutput(context, node, i);
      T* out_ptr = tflite::micro::GetTensorData<T>(out_t);
      const int copy_elems = out_dims->data[axis] * base_inner;
      T* dst = out_ptr + k * copy_elems;
      for (int j = 0; j < copy_elems; ++j) dst[j] = in_ptr[j];
      in_ptr += copy_elems;
    }
  }
  return kTfLiteOk;
}

// -------- CMSIS-NN fast paths ----------

static TfLiteStatus SplitImplCmsisS8(TfLiteContext* context, TfLiteNode* node,
                                     const TfLiteEvalTensor* input,
                                     int axis_value) {
  const int output_count = NumOutputs(node);
  const TfLiteIntArray* in_dims = input->dims;
  const int rank = in_dims->size;
  int axis = axis_value;
  if (axis < 0) axis += rank;

  if (rank > kMaxSplitRank || output_count > kMaxSplitOutputs) {
    return kTfLiteError;
  }

  // Build input shape
  int32_t shape[kMaxSplitRank];
  for (int i = 0; i < rank; ++i) shape[i] = in_dims->data[i];

  // All outputs must have equal size along axis in TFLM SPLIT.
  // Get that from any output tensor's dims.
  const TfLiteEvalTensor* out0 = tflite::micro::GetEvalOutput(context, node, 0);
  const int piece = out0->dims->data[axis];

  // Per-split lengths (all equal)
  int32_t split_dims[kMaxSplitOutputs];
  for (int i = 0; i < output_count; ++i) split_dims[i] = piece;

  // Output data pointers
  int8_t* out_ptrs[kMaxSplitOutputs];
  for (int i = 0; i < output_count; ++i) {
    TfLiteEvalTensor* out_t = tflite::micro::GetEvalOutput(context, node, i);
    out_ptrs[i] = tflite::micro::GetTensorData<int8_t>(out_t);
  }

  const int8_t* in_ptr = tflite::micro::GetTensorData<int8_t>(input);

  const arm_cmsis_nn_status st =
      arm_split_s8(in_ptr, /*input_dims=*/rank, /*input_shape=*/shape,
                   /*axis=*/axis, /*num_splits=*/output_count,
                   /*split_dims=*/split_dims, /*output_data=*/out_ptrs);

  return (st == ARM_CMSIS_NN_SUCCESS) ? kTfLiteOk : kTfLiteError;
}

static TfLiteStatus SplitImplCmsisS16(TfLiteContext* context, TfLiteNode* node,
                                      const TfLiteEvalTensor* input,
                                      int axis_value) {
  const int output_count = NumOutputs(node);
  const TfLiteIntArray* in_dims = input->dims;
  const int rank = in_dims->size;
  int axis = axis_value;
  if (axis < 0) axis += rank;

  if (rank > kMaxSplitRank || output_count > kMaxSplitOutputs) {
    return kTfLiteError;
  }

  int32_t shape[kMaxSplitRank];
  for (int i = 0; i < rank; ++i) shape[i] = in_dims->data[i];

  const TfLiteEvalTensor* out0 = tflite::micro::GetEvalOutput(context, node, 0);
  const int piece = out0->dims->data[axis];

  int32_t split_dims[kMaxSplitOutputs];
  for (int i = 0; i < output_count; ++i) split_dims[i] = piece;

  int16_t* out_ptrs[kMaxSplitOutputs];
  for (int i = 0; i < output_count; ++i) {
    TfLiteEvalTensor* out_t = tflite::micro::GetEvalOutput(context, node, i);
    out_ptrs[i] = tflite::micro::GetTensorData<int16_t>(out_t);
  }

  const int16_t* in_ptr = tflite::micro::GetTensorData<int16_t>(input);

  const arm_cmsis_nn_status st =
      arm_split_s16(in_ptr, /*input_dims=*/rank, /*input_shape=*/shape,
                    /*axis=*/axis, /*num_splits=*/output_count,
                    /*split_dims=*/split_dims, /*output_data=*/out_ptrs);

  return (st == ARM_CMSIS_NN_SUCCESS) ? kTfLiteOk : kTfLiteError;
}

// -------- Prepare / Eval ----------

TfLiteStatus SplitPrepare(TfLiteContext* context, TfLiteNode* node) {
  MicroContext* micro_context = GetMicroContext(context);
  TfLiteTensor* axis = micro_context->AllocateTempInputTensor(node, kAxisTensor);
  TF_LITE_ENSURE(context, axis != nullptr);

  // Micro requires constant axis
  TF_LITE_ENSURE_MSG(context, IsConstantTensor(axis),
                     "Non constant axis tensor not supported");

  micro_context->DeallocateTempTfLiteTensor(axis);
  return kTfLiteOk;
}

TfLiteStatus SplitEval(TfLiteContext* context, TfLiteNode* node) {
  const TfLiteEvalTensor* axis_t = tflite::micro::GetEvalInput(context, node, kAxisTensor);
  const TfLiteEvalTensor* input  = tflite::micro::GetEvalInput(context, node, kInputTensor);

  int axis_value = tflite::micro::GetTensorData<int32_t>(axis_t)[0];

  switch (input->type) {
    case kTfLiteInt8: {
      // Try CMSIS-NN fast-path first
      if (SplitImplCmsisS8(context, node, input, axis_value) == kTfLiteOk) return kTfLiteOk;
      return SplitImplRef<int8_t>(context, node, input, axis_value);
    }
    case kTfLiteInt16: {
      if (SplitImplCmsisS16(context, node, input, axis_value) == kTfLiteOk) return kTfLiteOk;
      return SplitImplRef<int16_t>(context, node, input, axis_value);
    }
    case kTfLiteFloat32:
      return SplitImplRef<float>(context, node, input, axis_value);
    case kTfLiteInt32:
      return SplitImplRef<int32_t>(context, node, input, axis_value);
    default:
      MicroPrintf("Type %s currently not supported.",
                  TfLiteTypeGetName(input->type));
      return kTfLiteError;
  }
}

}  // namespace

TFLMRegistration Register_SPLIT() {
  return tflite::micro::RegisterOp(nullptr, SplitPrepare, SplitEval);
}

}  // namespace tflite
