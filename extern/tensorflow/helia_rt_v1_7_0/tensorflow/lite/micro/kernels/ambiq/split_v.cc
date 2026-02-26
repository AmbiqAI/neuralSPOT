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
#include "tensorflow/lite/kernels/op_macros.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"
#include "tensorflow/lite/micro/micro_log.h"

namespace tflite {
namespace {

constexpr int kInputTensor = 0;
constexpr int kSizesTensor = 1;
constexpr int kAxisTensor = 2;

constexpr int kMaxSplitRank = 6;
constexpr int kMaxSplitOutputs = 16;

// ---------------- Reference path (works for all T types) ----------------
template <typename T>
TfLiteStatus SplitVImplRef(TfLiteContext* context, TfLiteNode* node,
                           const TfLiteEvalTensor* input,
                           int axis, const int32_t* sizes, int num_splits) {
  const TfLiteIntArray* in_dims = input->dims;
  const int rank = in_dims->size;

  // Validate sizes sum matches input along axis
  int axis_len = in_dims->data[axis];
  int infer_idx = -1;
  int sum_known = 0;
  for (int i = 0; i < num_splits; ++i) {
    if (sizes[i] == -1) {
      TF_LITE_ENSURE_MSG(context, infer_idx == -1, "Only one -1 allowed in size_splits");
      infer_idx = i;
    } else {
      TF_LITE_ENSURE(context, sizes[i] >= 0);
      sum_known += sizes[i];
    }
  }
  int32_t resolved_sizes[kMaxSplitOutputs]; // small, but used only in ref path too
  for (int i = 0; i < num_splits; ++i) resolved_sizes[i] = sizes[i];
  if (infer_idx >= 0) {
    resolved_sizes[infer_idx] = axis_len - sum_known;
  }
  int total = 0; for (int i = 0; i < num_splits; ++i) total += resolved_sizes[i];
  TF_LITE_ENSURE_EQ(context, total, axis_len);

  // Outer / inner products
  int64_t outer = 1;
  for (int i = 0; i < axis; ++i) outer *= in_dims->data[i];

  int64_t base_inner = 1;
  for (int i = axis + 1; i < rank; ++i) base_inner *= in_dims->data[i];

  const T* in_ptr = tflite::micro::GetTensorData<T>(input);
  for (int k = 0; k < outer; ++k) {
    for (int i = 0; i < num_splits; ++i) {
      TfLiteEvalTensor* out_t = tflite::micro::GetEvalOutput(context, node, i);
      T* out_data = tflite::micro::GetTensorData<T>(out_t);
      const int copy_elems = resolved_sizes[i] * base_inner;
      T* dst = out_data + k * copy_elems;
      for (int j = 0; j < copy_elems; ++j) dst[j] = *in_ptr++;
    }
  }
  return kTfLiteOk;
}

// --------------- Helpers: read and normalize size_splits ----------------
static TfLiteStatus ReadAndResolveSizes(TfLiteContext* context,
                                        const TfLiteEvalTensor* input,
                                        const TfLiteEvalTensor* sizes_t,
                                        int axis,
                                        int32_t* out_sizes,
                                        int* out_num_splits) {
  // size_splits must be int32 1-D of length num_splits
  TF_LITE_ENSURE_EQ(context, sizes_t->type, kTfLiteInt32);
  TF_LITE_ENSURE_EQ(context, sizes_t->dims->size, 1);

  const int num_splits = sizes_t->dims->data[0];
  *out_num_splits = num_splits;

  const int32_t* sizes = tflite::micro::GetTensorData<int32_t>(sizes_t);
  for (int i = 0; i < num_splits; ++i) out_sizes[i] = sizes[i];

  // Resolve optional -1 and validate sum matches input axis length.
  const int axis_len = input->dims->data[axis];
  int infer_idx = -1;
  int sum_known = 0;
  for (int i = 0; i < num_splits; ++i) {
    if (out_sizes[i] == -1) {
      TF_LITE_ENSURE_MSG(context, infer_idx == -1, "Only one -1 allowed");
      infer_idx = i;
    } else {
      TF_LITE_ENSURE(context, out_sizes[i] >= 0);
      sum_known += out_sizes[i];
    }
  }
  if (infer_idx >= 0) {
    out_sizes[infer_idx] = axis_len - sum_known;
  }

  int sum_all = 0;
  for (int i = 0; i < num_splits; ++i) sum_all += out_sizes[i];
  TF_LITE_ENSURE_EQ(context, sum_all, axis_len);

  return kTfLiteOk;
}

// ---------------- CMSIS-NN fast paths (int8 / int16) ----------------
static TfLiteStatus SplitVImplCmsisS8(TfLiteContext* context, TfLiteNode* node,
                                      const TfLiteEvalTensor* input,
                                      const TfLiteEvalTensor* sizes_t,
                                      int axis_value) {
  const int rank = input->dims->size;
  int axis = axis_value; if (axis < 0) axis += rank;
  if (rank > kMaxSplitRank) return kTfLiteError;

  int32_t shape[kMaxSplitRank];
  for (int i = 0; i < rank; ++i) shape[i] = input->dims->data[i];

  int32_t split_dims[kMaxSplitOutputs];
  int num_splits = 0;
  TF_LITE_ENSURE_STATUS(ReadAndResolveSizes(context, input, sizes_t, axis,
                                            split_dims, &num_splits));
  TF_LITE_ENSURE(context, num_splits <= kMaxSplitOutputs);

  int8_t* out_ptrs[kMaxSplitOutputs];
  for (int i = 0; i < num_splits; ++i) {
    TfLiteEvalTensor* out_t = tflite::micro::GetEvalOutput(context, node, i);
    out_ptrs[i] = tflite::micro::GetTensorData<int8_t>(out_t);
  }

  const int8_t* in_ptr = tflite::micro::GetTensorData<int8_t>(input);
  const arm_cmsis_nn_status st =
      arm_split_s8(in_ptr, rank, shape, axis, num_splits, split_dims, out_ptrs);
  return (st == ARM_CMSIS_NN_SUCCESS) ? kTfLiteOk : kTfLiteError;
}

static TfLiteStatus SplitVImplCmsisS16(TfLiteContext* context, TfLiteNode* node,
                                       const TfLiteEvalTensor* input,
                                       const TfLiteEvalTensor* sizes_t,
                                       int axis_value) {
  const int rank = input->dims->size;
  int axis = axis_value; if (axis < 0) axis += rank;
  if (rank > kMaxSplitRank) return kTfLiteError;

  int32_t shape[kMaxSplitRank];
  for (int i = 0; i < rank; ++i) shape[i] = input->dims->data[i];

  int32_t split_dims[kMaxSplitOutputs];
  int num_splits = 0;
  TF_LITE_ENSURE_STATUS(ReadAndResolveSizes(context, input, sizes_t, axis,
                                            split_dims, &num_splits));
  TF_LITE_ENSURE(context, num_splits <= kMaxSplitOutputs);

  int16_t* out_ptrs[kMaxSplitOutputs];
  for (int i = 0; i < num_splits; ++i) {
    TfLiteEvalTensor* out_t = tflite::micro::GetEvalOutput(context, node, i);
    out_ptrs[i] = tflite::micro::GetTensorData<int16_t>(out_t);
  }

  const int16_t* in_ptr = tflite::micro::GetTensorData<int16_t>(input);
  const arm_cmsis_nn_status st =
      arm_split_s16(in_ptr, rank, shape, axis, num_splits, split_dims, out_ptrs);
  return (st == ARM_CMSIS_NN_SUCCESS) ? kTfLiteOk : kTfLiteError;
}

// ---------------- Prepare / Eval ----------------

TfLiteStatus SplitVPrepare(TfLiteContext* context, TfLiteNode* node) {
  TF_LITE_ENSURE_EQ(context, NumInputs(node), 3);
  MicroContext* micro_context = GetMicroContext(context);

  // Micro requires constant axis AND size_splits to have fixed shape/values,
  // since we can’t allocate dynamic outputs at Eval.
  TfLiteTensor* axis = micro_context->AllocateTempInputTensor(node, kAxisTensor);
  TF_LITE_ENSURE_MSG(context, IsConstantTensor(axis),
                     "Non constant axis tensor not supported");

  TfLiteTensor* sizes = micro_context->AllocateTempInputTensor(node, kSizesTensor);
  TF_LITE_ENSURE_MSG(context, IsConstantTensor(sizes),
                     "Non constant size_splits not supported");

  micro_context->DeallocateTempTfLiteTensor(axis);
  micro_context->DeallocateTempTfLiteTensor(sizes);
  return kTfLiteOk;
}

TfLiteStatus SplitVEval(TfLiteContext* context, TfLiteNode* node) {
  const TfLiteEvalTensor* input  = tflite::micro::GetEvalInput(context, node, kInputTensor);
  const TfLiteEvalTensor* sizes  = tflite::micro::GetEvalInput(context, node, kSizesTensor);
  const TfLiteEvalTensor* axis_t = tflite::micro::GetEvalInput(context, node, kAxisTensor);

  int axis_value = tflite::micro::GetTensorData<int32_t>(axis_t)[0];

  switch (input->type) {
    case kTfLiteInt8: {
      if (SplitVImplCmsisS8(context, node, input, sizes, axis_value) == kTfLiteOk)
        return kTfLiteOk;
      // Fallback reference
      {
        // Read sizes directly for ref path
        const int num_splits = sizes->dims->data[0];
        int32_t split_dims[kMaxSplitOutputs];
        TF_LITE_ENSURE(context, num_splits <= kMaxSplitOutputs);
        TF_LITE_ENSURE_STATUS(ReadAndResolveSizes(context, input, sizes,
                                                  (axis_value < 0) ? axis_value + input->dims->size : axis_value,
                                                  split_dims, &const_cast<int&>(num_splits)));
        return SplitVImplRef<int8_t>(context, node, input,
                                     (axis_value < 0) ? axis_value + input->dims->size : axis_value,
                                     split_dims, num_splits);
      }
    }
    case kTfLiteInt16: {
      if (SplitVImplCmsisS16(context, node, input, sizes, axis_value) == kTfLiteOk)
        return kTfLiteOk;
      // Fallback reference
      {
        const int num_splits = sizes->dims->data[0];
        int32_t split_dims[kMaxSplitOutputs];
        TF_LITE_ENSURE(context, num_splits <= kMaxSplitOutputs);
        TF_LITE_ENSURE_STATUS(ReadAndResolveSizes(context, input, sizes,
                                                  (axis_value < 0) ? axis_value + input->dims->size : axis_value,
                                                  split_dims, &const_cast<int&>(num_splits)));
        return SplitVImplRef<int16_t>(context, node, input,
                                      (axis_value < 0) ? axis_value + input->dims->size : axis_value,
                                      split_dims, num_splits);
      }
    }
    case kTfLiteFloat32: {
      // Pure reference path for float
      const int num_splits = sizes->dims->data[0];
      int32_t split_dims[kMaxSplitOutputs];
      TF_LITE_ENSURE(context, num_splits <= kMaxSplitOutputs);
      TF_LITE_ENSURE_STATUS(ReadAndResolveSizes(context, input, sizes,
                                                (axis_value < 0) ? axis_value + input->dims->size : axis_value,
                                                split_dims, &const_cast<int&>(num_splits)));
      return SplitVImplRef<float>(context, node, input,
                                  (axis_value < 0) ? axis_value + input->dims->size : axis_value,
                                  split_dims, num_splits);
    }
    case kTfLiteInt32: {
      const int num_splits = sizes->dims->data[0];
      int32_t split_dims[kMaxSplitOutputs];
      TF_LITE_ENSURE(context, num_splits <= kMaxSplitOutputs);
      TF_LITE_ENSURE_STATUS(ReadAndResolveSizes(context, input, sizes,
                                                (axis_value < 0) ? axis_value + input->dims->size : axis_value,
                                                split_dims, &const_cast<int&>(num_splits)));
      return SplitVImplRef<int32_t>(context, node, input,
                                    (axis_value < 0) ? axis_value + input->dims->size : axis_value,
                                    split_dims, num_splits);
    }
    default:
      MicroPrintf("Type %s currently not supported.", TfLiteTypeGetName(input->type));
      return kTfLiteError;
  }
}

}  // namespace

TFLMRegistration Register_SPLIT_V() {
  return tflite::micro::RegisterOp(nullptr, SplitVPrepare, SplitVEval);
}

}  // namespace tflite
