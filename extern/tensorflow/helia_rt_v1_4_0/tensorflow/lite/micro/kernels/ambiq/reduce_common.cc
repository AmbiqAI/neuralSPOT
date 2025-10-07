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

#include "Include/arm_nnfunctions.h"

#include "tensorflow/lite/c/builtin_op_data.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/common.h"
#include "tensorflow/lite/kernels/internal/quantization_util.h"
#include "tensorflow/lite/kernels/internal/reference/integer_ops/mean.h"
#include "tensorflow/lite/kernels/internal/reference/reduce.h"
#include "tensorflow/lite/kernels/internal/tensor_ctypes.h"
#include "tensorflow/lite/kernels/internal/types.h"
#include "tensorflow/lite/kernels/kernel_util.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"
#include "tensorflow/lite/micro/kernels/reduce.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_utils.h"

namespace tflite {

const int kMaxNumberOfAxis = 5;
const int kMaxNumberOfReducedAxis = 2;
constexpr int kCmsisRank = 4;

/**
 * Populate the CMSIS NN parameters from the TFLite tensors and axis information.
 * This function extracts the input and output dimensions, builds the axis
 * mask, and computes the output multiplier and shift based on the number of
 * elements in the reduced axis for mean.
 *
 * @param input_t The input tensor to the reduce operation.
 * @param output_t The output tensor from the reduce operation.
 * @param axis_data Pointer to the axis data array.
 * @param axis_count The number of axes to reduce.
 * @param in_dims Pointer to the cmsis_nn_dims structure for input dimensions.
 * @param axis_dims Pointer to the cmsis_nn_dims structure for axis dimensions.
 * @param out_dims Pointer to the cmsis_nn_dims structure for output dimensions.
 * @param output_mult Pointer to the output multiplier.
 * @param output_shift Pointer to the output shift value.
 * @param compute_sum Boolean indicating whether to compute the sum (true) or mean (false).
 */
static void PopulateCmsisParams(const TfLiteEvalTensor* input_t,
                                const TfLiteEvalTensor* output_t,
                                const int32_t* axis_data,
                                int axis_count,
                                cmsis_nn_dims* in_dims,
                                cmsis_nn_dims* axis_dims,
                                cmsis_nn_dims* out_dims,
                                int32_t *output_mult,
                                int32_t *output_shift,
                                bool compute_sum) {

  (void)output_t;  // Unused in this function, but may be used later.

  // --- 1) Pad input shape at end ---
  int32_t in_shape[kCmsisRank] = {1, 1, 1, 1};
  int in_rank = input_t->dims->size;
  for (int i = 0; i < in_rank && i < kCmsisRank; ++i) {
    in_shape[i] = input_t->dims->data[i];
  }
  in_dims->n = in_shape[0];
  in_dims->h = in_shape[1];
  in_dims->w = in_shape[2];
  in_dims->c = in_shape[3];

  // --- 2) Build axis mask & count elements ---
  int64_t num_elements_in_axis = 1;
  axis_dims->n = axis_dims->h = axis_dims->w = axis_dims->c = 0;
  for (int i = 0; i < axis_count; ++i) {
    int a = axis_data[i];
    if (a < 0) a += in_rank;
    switch (a) {
      case 0:
        if (!axis_dims->n) {
          axis_dims->n = 1;
          num_elements_in_axis *= in_shape[0];
        }
        break;
      case 1:
        if (!axis_dims->h) {
          axis_dims->h = 1;
          num_elements_in_axis *= in_shape[1];
        }
        break;
      case 2:
        if (!axis_dims->w) {
          axis_dims->w = 1;
          num_elements_in_axis *= in_shape[2];
        }
        break;
      case 3:
        if (!axis_dims->c) {
          axis_dims->c = 1;
          num_elements_in_axis *= in_shape[3];
        }
        break;
      default:
        break;  // out of range gets ignored
    }
  }

  // --- 3) Pad output shape at end ---
  // NOTE: We do not rely on provided output tensor shape because it could be flattened or a
  // different view (e.g. [1, 2] instead of [2, 1]). This is atypical but unit tests do this...
  out_dims->n = axis_dims->n ? 1 : in_dims->n;
  out_dims->h = axis_dims->h ? 1 : in_dims->h;
  out_dims->w = axis_dims->w ? 1 : in_dims->w;
  out_dims->c = axis_dims->c ? 1 : in_dims->c;

  // Fold 1/count into output multiplier & shift (only for MEAN)
  if (!compute_sum) {
    TFLITE_DCHECK_GE(num_elements_in_axis, 0);
    int shift = 63 - CountLeadingZeros(static_cast<uint64_t>(num_elements_in_axis));
    shift = std::min(shift, 32);
    shift = std::min(shift, 31 + static_cast<int>(*output_shift));
    *output_mult = static_cast<int32_t>(
        (static_cast<int64_t>(*output_mult) << shift) /
        num_elements_in_axis);
    *output_shift = *output_shift - shift;
  }

}

TfLiteStatus PrepareSimple(TfLiteContext* context, TfLiteNode* node,
                           int32_t* multiplier, int* shift) {
  MicroContext* micro_context = GetMicroContext(context);

  // Inputs Tensor (dtype depends on quantization):
  // [0] = Input
  // [1] = Axis
  TfLiteTensor* input = micro_context->AllocateTempInputTensor(node, 0);

  // Outputs Tensor (dtype depends on quantization):
  // [0] = Output

  // Validate number of inputs and outputs
  TF_LITE_ENSURE_EQ(context, node->inputs->size, 2);
  TF_LITE_ENSURE_EQ(context, node->outputs->size, 1);

  // Validate axis type
  TfLiteTensor* axis = micro_context->AllocateTempInputTensor(node, 1);
  TF_LITE_ENSURE(context, axis != nullptr);
  TF_LITE_ENSURE_TYPES_EQ(context, axis->type, kTfLiteInt32);

  if (input->type == kTfLiteInt8) {
    TfLiteTensor* output = micro_context->AllocateTempOutputTensor(node, 0);
    const double real_multiplier = static_cast<double>(input->params.scale) /
                                   static_cast<double>(output->params.scale);
    QuantizeMultiplier(real_multiplier, multiplier, shift);
    micro_context->DeallocateTempTfLiteTensor(output);
  }
  micro_context->DeallocateTempTfLiteTensor(axis);
  micro_context->DeallocateTempTfLiteTensor(input);
  return kTfLiteOk;
}

TfLiteStatus PrepareMaxHelper(TfLiteContext* context, TfLiteNode* node,
                              OpDataReduce* op_data) {
  TF_LITE_ENSURE_OK(context, PrepareSimple(context, node, &op_data->multiplier,
                                           &op_data->shift));

  MicroContext* micro_context = GetMicroContext(context);
  TfLiteTensor* input = micro_context->AllocateTempInputTensor(node, 0);
  TfLiteTensor* output = micro_context->AllocateTempOutputTensor(node, 0);
  TfLiteTensor* axis = micro_context->AllocateTempInputTensor(node, 1);

  op_data->input_zp = input->params.zero_point;
  op_data->input_scale = input->params.scale;
  op_data->output_zp = output->params.zero_point;
  op_data->output_scale = output->params.scale;
  op_data->num_output_elements = NumElements(output);

  context->RequestScratchBufferInArena(context, sizeof(int) * input->dims->size,
                                       &op_data->temp_buffer_idx);
  context->RequestScratchBufferInArena(
      context, sizeof(int) * static_cast<int>(ElementCount(*axis->dims)),
      &op_data->resolved_axis_idx);

  micro_context->DeallocateTempTfLiteTensor(input);
  micro_context->DeallocateTempTfLiteTensor(output);
  micro_context->DeallocateTempTfLiteTensor(axis);
  return kTfLiteOk;
}

TfLiteStatus PrepareMeanOrSumHelper(TfLiteContext* context, TfLiteNode* node,
                                    OpDataReduce* op_data) {
  MicroContext* micro_context = GetMicroContext(context);
  TfLiteTensor* input = micro_context->AllocateTempInputTensor(node, 0);
  TfLiteTensor* output = micro_context->AllocateTempOutputTensor(node, 0);
  TfLiteTensor* axis = micro_context->AllocateTempInputTensor(node, 1);
  if (input->type == kTfLiteInt8 || input->type == kTfLiteInt16) {
    const double real_multiplier = static_cast<double>(input->params.scale) /
                                   static_cast<double>(output->params.scale);
    QuantizeMultiplier(real_multiplier, &op_data->multiplier, &op_data->shift);
  }

  int output_size = NumElements(output);
  op_data->num_axis = NumElements(axis);

  if (input->type == kTfLiteInt8 || input->type == kTfLiteInt16) {
    context->RequestScratchBufferInArena(context, output_size * sizeof(int32_t),
                                         &op_data->temp_buffer_idx);
    op_data->input_zp = input->params.zero_point;
    op_data->input_scale = input->params.scale;
    op_data->output_zp = output->params.zero_point;
    op_data->output_scale = output->params.scale;
  }

  TF_LITE_ENSURE_OK(
      context,
      PrepareSimple(context, node, &(op_data->multiplier), &(op_data->shift)));
  // TODO(b/144955155): Support uint8_t(b/144955155) and int8_t(b/144955018)
  micro_context->DeallocateTempTfLiteTensor(input);
  micro_context->DeallocateTempTfLiteTensor(output);
  micro_context->DeallocateTempTfLiteTensor(axis);
  return kTfLiteOk;
}

void ResolveAxis(const int* axis_data, int axis_count,
                 tflite::MeanParams* op_params) {
  int i = 0;
  for (; i < axis_count; ++i) {
    op_params->axis[i] = static_cast<int16_t>(axis_data[i]);
  }
  for (; i < 4; ++i) {
    op_params->axis[i] = 1;
  }
  op_params->axis_count = axis_count;
}

template <typename T>
TfLiteStatus QuantizedMeanOrSum(TfLiteContext* context, TfLiteNode* node,
                                int* temp_index, int* resolved_axis,
                                int32_t* temp_sum, OpDataReduce* op_data,
                                bool compute_sum) {
  const TfLiteEvalTensor* input = tflite::micro::GetEvalInput(context, node, 0);
  const TfLiteEvalTensor* axis = tflite::micro::GetEvalInput(context, node, 1);
  TfLiteEvalTensor* output = tflite::micro::GetEvalOutput(context, node, 0);
  TfLiteReducerParams* params =
      static_cast<TfLiteReducerParams*>(node->builtin_data);

  bool result = reference_ops::QuantizedMeanOrSumExtraArgs<T, int32_t>(
      tflite::micro::GetTensorData<T>(input), op_data->input_zp,
      op_data->input_scale, &input->dims->data[0], input->dims->size,
      tflite::micro::GetTensorData<T>(output), op_data->output_scale,
      op_data->multiplier, op_data->shift, op_data->output_zp,
      &output->dims->data[0], output->dims->size,
      tflite::micro::GetTensorData<int>(axis), op_data->num_axis,
      params->keep_dims, temp_index, resolved_axis, temp_sum, compute_sum);
  TF_LITE_ENSURE(context, result);

  return kTfLiteOk;
}

template <typename integer_type>
TfLiteStatus EvalIntegerMean(TfLiteContext* context, TfLiteNode* node,
                             int num_axis, OpDataReduce* op_data,
                             int* temp_index, int* resolved_axis) {
  int32_t* temp_sum = static_cast<int32_t*>(
      context->GetScratchBuffer(context, op_data->temp_buffer_idx));

  QuantizedMeanOrSum<integer_type>(context, node, temp_index, resolved_axis,
                                   temp_sum, op_data, /*compute_sum=*/false);

  return kTfLiteOk;
}

TfLiteStatus EvalMeanHelper(TfLiteContext* context, TfLiteNode* node,
                            OpDataReduce* op_data) {
  const TfLiteEvalTensor* input = tflite::micro::GetEvalInput(context, node, 0);
  const TfLiteEvalTensor* axis = tflite::micro::GetEvalInput(context, node, 1);
  TfLiteEvalTensor* output = tflite::micro::GetEvalOutput(context, node, 0);
  TfLiteReducerParams* params =
      reinterpret_cast<TfLiteReducerParams*>(node->builtin_data);

  int num_axis = static_cast<int>(ElementCount(*axis->dims));
  int temp_index[kMaxNumberOfAxis];
  int resolved_axis[kMaxNumberOfReducedAxis];

  switch (input->type) {
    case kTfLiteFloat32: {
      tflite::MeanParams op_params;
      ResolveAxis(tflite::micro::GetTensorData<int>(axis), num_axis,
                  &op_params);

      // Special case mean implementation exists for 4D mean across axes 1 and 2.
      bool special_case_4d_axes_1_and_2 =
          input->dims->size == 4 && op_params.axis_count == 2 &&
          ((op_params.axis[0] == 1 && op_params.axis[1] == 2) ||
           (op_params.axis[0] == 2 && op_params.axis[1] == 1));

      // Defer to specialized implementation for 4D Mean across axes 1 & 2.
      if (params->keep_dims && special_case_4d_axes_1_and_2) {
        reference_ops::Mean(op_params, tflite::micro::GetTensorShape(input),
                            tflite::micro::GetTensorData<float>(input),
                            tflite::micro::GetTensorShape(output),
                            tflite::micro::GetTensorData<float>(output));
      } else {
        TF_LITE_ENSURE(
            context,
            reference_ops::Mean(
                tflite::micro::GetTensorData<float>(input), input->dims->data,
                input->dims->size, tflite::micro::GetTensorData<float>(output),
                output->dims->data, output->dims->size,
                tflite::micro::GetTensorData<int>(axis), num_axis,
                params->keep_dims, temp_index, resolved_axis,
                tflite::micro::GetTensorData<float>(output)));
      }
    } break;
    case kTfLiteInt8: {
      if (input->dims->size <= kCmsisRank) {
        cmsis_nn_dims in_d, ax_d, out_d;
        int32_t output_mult = op_data->multiplier;
        int32_t output_shift = op_data->shift;

        PopulateCmsisParams(
          input, output,
          tflite::micro::GetTensorData<int32_t>(axis),
          num_axis,
          &in_d, &ax_d, &out_d,
          &output_mult, &output_shift,
          /*compute_sum=*/false
        );

        arm_cmsis_nn_status rst = arm_mean_s8(
          tflite::micro::GetTensorData<int8_t>(input),
          &in_d,
          -op_data->input_zp,
          &ax_d,
          tflite::micro::GetTensorData<int8_t>(output),
          &out_d,
          op_data->output_zp,
          output_mult,
          output_shift
        );
        return rst == ARM_CMSIS_NN_SUCCESS ? kTfLiteOk : kTfLiteError;
      }

      TF_LITE_ENSURE_OK(
          context, EvalIntegerMean<int8_t>(context, node, num_axis, op_data,
                                           temp_index, resolved_axis));
    } break;
    case kTfLiteInt16: {
      if (input->dims->size <= kCmsisRank) {
        cmsis_nn_dims in_d, ax_d, out_d;
        int32_t output_mult = op_data->multiplier;
        int32_t output_shift = op_data->shift;

        PopulateCmsisParams(
          input, output,
          tflite::micro::GetTensorData<int32_t>(axis),
          num_axis,
          &in_d, &ax_d, &out_d,
          &output_mult, &output_shift,
          /*compute_sum=*/false
        );

        arm_cmsis_nn_status rst = arm_mean_s16(
          tflite::micro::GetTensorData<int16_t>(input),
          &in_d,
          -op_data->input_zp,
          &ax_d,
          tflite::micro::GetTensorData<int16_t>(output),
          &out_d,
          op_data->output_zp,
          output_mult,
          output_shift
        );
        return rst == ARM_CMSIS_NN_SUCCESS ? kTfLiteOk : kTfLiteError;
      }

      TF_LITE_ENSURE_OK(
          context, EvalIntegerMean<int16_t>(context, node, num_axis, op_data,
                                            temp_index, resolved_axis));
    } break;
    default:
      TF_LITE_ENSURE_MSG(context, false,
                         "Currently, only float32, int8 or int16 input type "
                         "is supported.");
  }
  return kTfLiteOk;
}

TfLiteStatus EvalMaxHelper(TfLiteContext* context, TfLiteNode* node,
                           OpDataReduce* op_data) {
  const TfLiteEvalTensor* input = tflite::micro::GetEvalInput(context, node, 0);
  const TfLiteEvalTensor* axis = tflite::micro::GetEvalInput(context, node, 1);
  TfLiteEvalTensor* output = tflite::micro::GetEvalOutput(context, node, 0);
  TF_LITE_ENSURE_TYPES_EQ(context, input->type, output->type);
  TfLiteReducerParams* params =
      static_cast<TfLiteReducerParams*>(node->builtin_data);

  // Interpret an axis tensor with null dimensions as a scalar
  int num_axis = static_cast<int>(ElementCount(*axis->dims));
  int* temp_buffer = static_cast<int*>(
      context->GetScratchBuffer(context, op_data->temp_buffer_idx));
  int* resolved_axis = static_cast<int*>(
      context->GetScratchBuffer(context, op_data->resolved_axis_idx));
  switch (input->type) {
    case kTfLiteFloat32:
      TF_LITE_ENSURE(
          context,
          reference_ops::ReduceGeneric<float>(
              tflite::micro::GetTensorData<float>(input), input->dims->data,
              input->dims->size, tflite::micro::GetTensorData<float>(output),
              output->dims->data, output->dims->size,
              tflite::micro::GetTensorData<int>(axis), num_axis,
              params->keep_dims, temp_buffer, resolved_axis,
              std::numeric_limits<float>::lowest(),
              [](const float current, const float in) -> float {
                return (in > current) ? in : current;
              }));
      break;
    case kTfLiteInt8: {
      TF_LITE_ENSURE_EQ(context, static_cast<double>(op_data->input_scale),
                        static_cast<double>(op_data->output_scale));
      TF_LITE_ENSURE_EQ(context, op_data->input_zp, op_data->output_zp);

      if (input->dims->size <= kCmsisRank) {
        cmsis_nn_dims in_d, ax_d, out_d;
        int32_t output_mult = op_data->multiplier; // Not used
        int32_t output_shift = op_data->shift; // Not used

        PopulateCmsisParams(
          input, output,
          tflite::micro::GetTensorData<int32_t>(axis),
          num_axis,
          &in_d, &ax_d, &out_d,
          &output_mult, &output_shift,
          /*compute_sum=*/true
        );

        arm_cmsis_nn_status rst = arm_reduce_max_s8(
          tflite::micro::GetTensorData<int8_t>(input),
          &in_d,
          &ax_d,
          tflite::micro::GetTensorData<int8_t>(output),
          &out_d
        );
        return rst == ARM_CMSIS_NN_SUCCESS ? kTfLiteOk : kTfLiteError;
      }

      TF_LITE_ENSURE(
          context,
          reference_ops::ReduceGeneric<int8_t>(
              tflite::micro::GetTensorData<int8_t>(input), input->dims->data,
              input->dims->size, tflite::micro::GetTensorData<int8_t>(output),
              output->dims->data, output->dims->size,
              tflite::micro::GetTensorData<int>(axis), num_axis,
              params->keep_dims, temp_buffer, resolved_axis,
              std::numeric_limits<int8_t>::lowest(),
              [](const int8_t current, const int8_t in) -> int8_t {
                return (in > current) ? in : current;
              }));
      } break;

    case kTfLiteInt16: {
      TF_LITE_ENSURE_EQ(context, static_cast<double>(op_data->input_scale),
                        static_cast<double>(op_data->output_scale));
      TF_LITE_ENSURE_EQ(context, op_data->input_zp, op_data->output_zp);

      if (input->dims->size <= kCmsisRank) {
        cmsis_nn_dims in_d, ax_d, out_d;
        int32_t output_mult = op_data->multiplier; // Not used
        int32_t output_shift = op_data->shift; // Not used

        PopulateCmsisParams(
          input, output,
          tflite::micro::GetTensorData<int32_t>(axis),
          num_axis,
          &in_d, &ax_d, &out_d,
          &output_mult, &output_shift,
          /*compute_sum=*/true
        );

        arm_cmsis_nn_status rst = arm_reduce_max_s16(
          tflite::micro::GetTensorData<int16_t>(input),
          &in_d,
          &ax_d,
          tflite::micro::GetTensorData<int16_t>(output),
          &out_d
        );
        return rst == ARM_CMSIS_NN_SUCCESS ? kTfLiteOk : kTfLiteError;
      }

      TF_LITE_ENSURE(
          context,
          reference_ops::ReduceGeneric<int16_t>(
              tflite::micro::GetTensorData<int16_t>(input), input->dims->data,
              input->dims->size, tflite::micro::GetTensorData<int16_t>(output),
              output->dims->data, output->dims->size,
              tflite::micro::GetTensorData<int>(axis), num_axis,
              params->keep_dims, temp_buffer, resolved_axis,
              std::numeric_limits<int16_t>::lowest(),
              [](const int16_t current, const int16_t in) -> int16_t {
                return (in > current) ? in : current;
              }));
      } break;

    default:
      MicroPrintf("Only float32, int8, and int16 types are supported.");
      return kTfLiteError;
  }
  return kTfLiteOk;
}

TfLiteStatus EvalSumHelper(TfLiteContext* context, TfLiteNode* node,
                           OpDataReduce* op_data) {
  const TfLiteEvalTensor* input = tflite::micro::GetEvalInput(context, node, 0);
  const TfLiteEvalTensor* axis = tflite::micro::GetEvalInput(context, node, 1);
  TfLiteEvalTensor* output = tflite::micro::GetEvalOutput(context, node, 0);
  TF_LITE_ENSURE_TYPES_EQ(context, input->type, output->type);
  TfLiteReducerParams* params =
      static_cast<TfLiteReducerParams*>(node->builtin_data);

  // Interpret an axis tensor with null dimensions as a scalar.
  int num_axis = static_cast<int>(ElementCount(*axis->dims));
  int temp_index[kMaxNumberOfAxis];
  int resolved_axis[kMaxNumberOfReducedAxis];

  switch (input->type) {
    case kTfLiteFloat32: {
      TF_LITE_ENSURE(
          context,
          reference_ops::ReduceGeneric<float>(
              tflite::micro::GetTensorData<float>(input), input->dims->data,
              input->dims->size, tflite::micro::GetTensorData<float>(output),
              output->dims->data, output->dims->size,
              tflite::micro::GetTensorData<int>(axis), num_axis,
              params->keep_dims, temp_index, resolved_axis, /*init_value=*/0.f,
              [](const float current, const float in) -> float {
                return in + current;
              }));
    } break;
    case kTfLiteInt8: {

      if (input->dims->size <= kCmsisRank) {
        cmsis_nn_dims in_d, ax_d, out_d;
        int32_t output_mult = op_data->multiplier;
        int32_t output_shift = op_data->shift;

        PopulateCmsisParams(
          input, output,
          tflite::micro::GetTensorData<int32_t>(axis),
          num_axis,
          &in_d, &ax_d, &out_d,
          &output_mult, &output_shift,
          /*compute_sum=*/true
        );

        arm_cmsis_nn_status rst = arm_mean_s8(
          tflite::micro::GetTensorData<int8_t>(input),
          &in_d,
          -op_data->input_zp,
          &ax_d,
          tflite::micro::GetTensorData<int8_t>(output),
          &out_d,
          op_data->output_zp,
          output_mult,
          output_shift
        );
        return rst == ARM_CMSIS_NN_SUCCESS ? kTfLiteOk : kTfLiteError;
      }

      int32_t* temp_sum = static_cast<int32_t*>(
          context->GetScratchBuffer(context, op_data->temp_buffer_idx));
      QuantizedMeanOrSum<int8_t>(context, node, temp_index, resolved_axis,
                                 temp_sum, op_data, /*compute_sum=*/true);
    } break;
    case kTfLiteInt16: {
      if (input->dims->size <= kCmsisRank) {
        cmsis_nn_dims in_d, ax_d, out_d;
        int32_t output_mult = op_data->multiplier;
        int32_t output_shift = op_data->shift;

        PopulateCmsisParams(
          input, output,
          tflite::micro::GetTensorData<int32_t>(axis),
          num_axis,
          &in_d, &ax_d, &out_d,
          &output_mult, &output_shift,
          /*compute_sum=*/true
        );

        arm_cmsis_nn_status rst = arm_mean_s16(
          tflite::micro::GetTensorData<int16_t>(input),
          &in_d,
          -op_data->input_zp,
          &ax_d,
          tflite::micro::GetTensorData<int16_t>(output),
          &out_d,
          op_data->output_zp,
          output_mult,
          output_shift
        );
        return rst == ARM_CMSIS_NN_SUCCESS ? kTfLiteOk : kTfLiteError;
      }
      int32_t* temp_sum = static_cast<int32_t*>(
          context->GetScratchBuffer(context, op_data->temp_buffer_idx));
      QuantizedMeanOrSum<int16_t>(context, node, temp_index, resolved_axis,
                                  temp_sum, op_data, /*compute_sum=*/true);
    } break;
    default:
      MicroPrintf("Only float32, int8, and int16 types are supported.");
      return kTfLiteError;
  }
  return kTfLiteOk;
}

}  // namespace tflite
