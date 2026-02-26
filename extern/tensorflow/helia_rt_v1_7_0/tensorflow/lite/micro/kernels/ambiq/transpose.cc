/* Copyright 2025 The TensorFlow Authors. All Rights Reserved.

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
#include "tensorflow/lite/kernels/internal/reference/transpose.h"

#include "Include/arm_nnfunctions.h"

#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/tensor_ctypes.h"
#include "tensorflow/lite/kernels/internal/types.h"
#include "tensorflow/lite/kernels/kernel_util.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"
#include "tensorflow/lite/micro/micro_log.h"

namespace tflite {
namespace {

constexpr int kInputTensor = 0;
constexpr int kPermTensor = 1;
constexpr int kOutputTensor = 0;

static inline void BuildCmsisTransposeArgs(
    const TfLiteEvalTensor* in,
    const TfLiteEvalTensor* out,
    const int32_t* perm,
    int rank,
    cmsis_nn_dims* in_dims,
    cmsis_nn_dims* out_dims,
    uint32_t (&perm_u32)[4],
    int* out_num_dims) {

  // Helper: read d[k] or 1 if k out of range.
  auto at_or_one = [](const TfLiteIntArray* d, int k) -> int32_t {
    return (k < d->size) ? d->data[k] : 1;
  };

  // Fill N,H,W,C directly from axis 0..3 (missing -> 1)
  in_dims->n  = at_or_one(in->dims, 0);
  in_dims->h  = at_or_one(in->dims, 1);
  in_dims->w  = at_or_one(in->dims, 2);
  in_dims->c  = at_or_one(in->dims, 3);

  out_dims->n = at_or_one(out->dims, 0);
  out_dims->h = at_or_one(out->dims, 1);
  out_dims->w = at_or_one(out->dims, 2);
  out_dims->c = at_or_one(out->dims, 3);

  for (int i = 0; i < rank; ++i) perm_u32[i] = static_cast<uint32_t>(perm[i]);
  *out_num_dims = rank;
}

struct TransposeContext {
  TransposeContext(TfLiteContext* context, TfLiteNode* node) {
    micro_context = GetMicroContext(context);
    input = micro_context->AllocateTempInputTensor(node, kInputTensor);
    perm = micro_context->AllocateTempInputTensor(node, kPermTensor);
    output = micro_context->AllocateTempOutputTensor(node, kOutputTensor);
  }
  ~TransposeContext() {
    micro_context->DeallocateTempTfLiteTensor(input);
    micro_context->DeallocateTempTfLiteTensor(perm);
    micro_context->DeallocateTempTfLiteTensor(output);
  }
  MicroContext* micro_context;
  TfLiteTensor* input;
  TfLiteTensor* perm;
  TfLiteTensor* output;
};

TfLiteStatus TransposePrepare(TfLiteContext* context, TfLiteNode* node) {
  TF_LITE_ENSURE_EQ(context, NumInputs(node), 2);
  TF_LITE_ENSURE_EQ(context, NumOutputs(node), 1);

  TransposeContext op_context(context, node);

  // Ensure validity of input tensor.
  TF_LITE_ENSURE_MSG(context, NumDimensions(op_context.input) <= 5,
                     "Transpose op only supports 1D-5D input arrays.");
  TF_LITE_ENSURE_TYPES_EQ(context, op_context.input->type,
                          op_context.output->type);

  int dims = NumDimensions(op_context.input);
  const int32_t* perm_data = GetTensorData<int32_t>(op_context.perm);

  // Ensure validity of the permutations tensor as a 1D tensor.
  TF_LITE_ENSURE_EQ(context, NumDimensions(op_context.perm), 1);
  TF_LITE_ENSURE_EQ(context, op_context.perm->dims->data[0], dims);
  for (int idx = 0; idx < dims; ++idx) {
    TF_LITE_ENSURE_MSG(context, (perm_data[idx] >= 0 && perm_data[idx] < dims),
                       "Transpose op permutations array is out of bounds.");
  }

  return kTfLiteOk;
}

TfLiteStatus TransposeEval(TfLiteContext* context, TfLiteNode* node) {
  const TfLiteEvalTensor* perm_tensor =
      tflite::micro::GetEvalInput(context, node, kPermTensor);
  const int32_t* perm_data = perm_tensor->data.i32;
  const int size = perm_tensor->dims->data[0];

  // Reference params (fallback supports up to 5D as enforced in Prepare)
  TransposeParams ref_params;
  ref_params.perm_count = size;
  for (int i = 0; i < size; ++i) {
    ref_params.perm[i] = perm_data[i];
  }

  const TfLiteEvalTensor* input =
      tflite::micro::GetEvalInput(context, node, kInputTensor);
  TfLiteEvalTensor* output =
      tflite::micro::GetEvalOutput(context, node, kOutputTensor);

  const auto in_shape  = tflite::micro::GetTensorShape(input);
  const auto out_shape = tflite::micro::GetTensorShape(output);
  const int in_rank = in_shape.DimensionsCount();

  // ---- CMSIS-NN fast path: int8/int16 and rank ≤ 4 ----
  if (in_rank <= 4 &&
      (input->type == kTfLiteInt8 || input->type == kTfLiteInt16)) {
    cmsis_nn_dims in_dims{}, out_dims{};
    uint32_t perm_u32[4] = {0,0,0,0};
    int num_dims = 0;

    BuildCmsisTransposeArgs(
      input, output, perm_data, in_rank,
      &in_dims, &out_dims, perm_u32, &num_dims
    );

    const cmsis_nn_transpose_params tp = {
      .num_dims=num_dims,
      .permutations=perm_u32
    };

    arm_cmsis_nn_status st = ARM_CMSIS_NN_SUCCESS;
    if (input->type == kTfLiteInt8) {
      st = arm_transpose_s8(
          tflite::micro::GetTensorData<int8_t>(input),
          tflite::micro::GetTensorData<int8_t>(output),
          &in_dims, &out_dims, &tp);
    } else { // kTfLiteInt16
      st = arm_transpose_s16(
          tflite::micro::GetTensorData<int16_t>(input),
          tflite::micro::GetTensorData<int16_t>(output),
          &in_dims, &out_dims, &tp);
    }

    if (st == ARM_CMSIS_NN_SUCCESS) return kTfLiteOk;
    MicroPrintf("CMSIS-NN transpose fallback (status=%d)", (int)st);
  }


  // ---- Reference fallback (all types, 1–5D) ----
  switch (input->type) {
    case kTfLiteFloat32:
      reference_ops::Transpose(ref_params, in_shape,
                               tflite::micro::GetTensorData<float>(input),
                               out_shape,
                               tflite::micro::GetTensorData<float>(output));
      break;
    case kTfLiteInt16:
      reference_ops::Transpose(ref_params, in_shape,
                               tflite::micro::GetTensorData<int16_t>(input),
                               out_shape,
                               tflite::micro::GetTensorData<int16_t>(output));
      break;
    case kTfLiteInt8:
      reference_ops::Transpose(ref_params, in_shape,
                               tflite::micro::GetTensorData<int8_t>(input),
                               out_shape,
                               tflite::micro::GetTensorData<int8_t>(output));
      break;
    default:
      MicroPrintf("Type %s not supported by Transpose.",
                  TfLiteTypeGetName(input->type));
      return kTfLiteError;
  }
  return kTfLiteOk;
}
}  // namespace

TFLMRegistration Register_TRANSPOSE() {
  return tflite::micro::RegisterOp(nullptr, TransposePrepare, TransposeEval);
}

}  // namespace tflite
