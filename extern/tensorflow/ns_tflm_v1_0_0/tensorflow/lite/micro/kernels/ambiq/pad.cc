#include "tensorflow/lite/kernels/internal/reference/pad.h"

#include <string.h>

#include "Include/arm_nn_types.h"
#include "Include/arm_nnfunctions.h"
#include "Include/arm_nnsupportfunctions.h"
#include "tensorflow/lite/c/builtin_op_data.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/types.h"
#include "tensorflow/lite/kernels/kernel_util.h"
#include "tensorflow/lite/kernels/op_macros.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"
#include "tensorflow/lite/micro/micro_log.h"

namespace tflite
{

namespace
{

struct OpDataPad
{
  PadParams params;
  int32_t output_zero_point;
};

void PopulateCommonParams(
    cmsis_nn_dims *const input_size,
    cmsis_nn_dims *const pre_pad,
    cmsis_nn_dims *const post_pad,
    OpDataPad *data,
    const RuntimeShape &input_shape
  )
{
  input_size->n = input_shape.Dims(0);
  input_size->h = input_shape.Dims(1);
  input_size->w = input_shape.Dims(2);
  input_size->c = input_shape.Dims(3);
  // Need to check left_padding_count and put 0 for the rest of the dimensions.
  pre_pad->n = data->params.left_padding_count >= 1 ? data->params.left_padding[0] : 0;
  pre_pad->h = data->params.left_padding_count >= 2 ? data->params.left_padding[1] : 0;
  pre_pad->w = data->params.left_padding_count >= 3 ? data->params.left_padding[2] : 0;
  pre_pad->c = data->params.left_padding_count >= 4 ? data->params.left_padding[3] : 0;

  post_pad->n = data->params.right_padding_count >= 1 ? data->params.right_padding[0] : 0;
  post_pad->h = data->params.right_padding_count >= 2 ? data->params.right_padding[1] : 0;
  post_pad->w = data->params.right_padding_count >= 3 ? data->params.right_padding[2] : 0;
  post_pad->c = data->params.right_padding_count >= 4 ? data->params.right_padding[3] : 0;
}

void *Init(TfLiteContext *context, const char *buffer, size_t length)
{
  TFLITE_DCHECK(context->AllocatePersistentBuffer != nullptr);
  return context->AllocatePersistentBuffer(context, sizeof(OpDataPad));
}

TfLiteStatus Prepare(TfLiteContext *context, TfLiteNode *node)
{
  MicroContext *micro_context = GetMicroContext(context);

  TFLITE_DCHECK(node->user_data != nullptr);
  OpDataPad *data = static_cast<OpDataPad *>(node->user_data);

  TF_LITE_ENSURE(context, NumInputs(node) == 2 || NumInputs(node) == 3);
  TF_LITE_ENSURE_EQ(context, NumOutputs(node), 1);

  TfLiteTensor *input = micro_context->AllocateTempInputTensor(node, /*index=*/0);
  TF_LITE_ENSURE(context, input != nullptr);
  TfLiteTensor *paddings = micro_context->AllocateTempInputTensor(node, /*index=*/1);
  TF_LITE_ENSURE(context, paddings != nullptr);
  TfLiteTensor *constant_values =
      NumInputs(node) == 3 ? micro_context->AllocateTempInputTensor(node, /*index=*/2) : nullptr;
  TfLiteTensor *output = micro_context->AllocateTempOutputTensor(node, /*index=*/0);
  TF_LITE_ENSURE(context, output != nullptr);

  TF_LITE_ENSURE_EQ(context, input->type, output->type);

  // Current implementations rely on the inputs being <= 4D.
  TF_LITE_ENSURE(context, NumDimensions(input) <= reference_ops::PadKernelMaxDimensionCount());

  if (constant_values != nullptr)
  {
    TF_LITE_ENSURE_EQ(context, input->type, constant_values->type);
    // Ensure that constant_values is a scalar.
    TF_LITE_ENSURE_EQ(context, NumElements(constant_values), 1);
  }

  // There must be a pair of paddings for each output dimension.
  TF_LITE_ENSURE_EQ(context, GetTensorShape(paddings).FlatSize(), output->dims->size * 2);

  // On Micro, outputs must be properly sized by the converter.
  // NOTE: This data is only available because the paddings buffer is stored in
  // the flatbuffer:
  TF_LITE_ENSURE(context, IsConstantTensor(paddings));
  const int32_t *paddings_data = GetTensorData<int32_t>(paddings);
  for (int i = 0; i < output->dims->size; i++)
  {
    int output_dim = output->dims->data[i];
    int expected_dim = input->dims->data[i] + paddings_data[i * 2] + paddings_data[i * 2 + 1];
    TF_LITE_ENSURE_EQ(context, output_dim, expected_dim);
  }

  // Calculate OpDataPad:
  data->params.resizing_category = ResizingCategory::kGenericResize;
  const int paddings_total = GetTensorShape(paddings).FlatSize();
  if (paddings_total == 8 && (paddings_data[0] == 0 && paddings_data[1] == 0) &&
      (paddings_data[6] == 0 && paddings_data[7] == 0))
  {
    data->params.resizing_category = ResizingCategory::kImageStyle;
  }

  const int num_input_dimensions = NumDimensions(input);
  data->params.left_padding_count = num_input_dimensions;
  data->params.right_padding_count = num_input_dimensions;

  for (int idx = num_input_dimensions - 1; idx >= 0; --idx)
  {
    data->params.left_padding[idx] = paddings_data[idx * 2];
    data->params.right_padding[idx] = paddings_data[idx * 2 + 1];
  }

  if (input->type == kTfLiteInt8)
  {
    if (constant_values == nullptr)
    {
      // Quantized Pad requires that 0 is represented in the quantized
      // range.
      TF_LITE_ENSURE(context, output->params.zero_point >= std::numeric_limits<int8_t>::min());
      TF_LITE_ENSURE(context, output->params.zero_point <= std::numeric_limits<int8_t>::max());
    }
    else
    {
      // Quantized Pad requires that 'constant_values' is represented in the
      // same quantized range as the input and output tensors.
      TF_LITE_ENSURE_EQ(context, output->params.zero_point, constant_values->params.zero_point);
      TF_LITE_ENSURE_EQ(
          context, static_cast<double>(output->params.scale), static_cast<double>(constant_values->params.scale));
    }
    data->output_zero_point = output->params.zero_point;
  }

  micro_context->DeallocateTempTfLiteTensor(input);
  micro_context->DeallocateTempTfLiteTensor(paddings);
  if (constant_values != nullptr)
  {
    micro_context->DeallocateTempTfLiteTensor(constant_values);
  }
  micro_context->DeallocateTempTfLiteTensor(output);

  return kTfLiteOk;
}

TfLiteStatus Eval(TfLiteContext *context, TfLiteNode *node)
{
  TFLITE_DCHECK(node->user_data != nullptr);
  OpDataPad *data = static_cast<OpDataPad *>(node->user_data);

  const TfLiteEvalTensor *input = tflite::micro::GetEvalInput(context, node, /*index=*/0);
  const TfLiteEvalTensor *constant_values =
      NumInputs(node) == 3 ? tflite::micro::GetEvalInput(context, node, /*index=*/2) : nullptr;
  TfLiteEvalTensor *output = tflite::micro::GetEvalOutput(context, node, /*index=*/0);

  switch (input->type)
  {

  case kTfLiteFloat32:
  {
    float pad_value = constant_values == nullptr ? 0.f : *tflite::micro::GetTensorData<float>(constant_values);
    if (data->params.resizing_category == ResizingCategory::kImageStyle)
    {
      reference_ops::PadImageStyle(
          data->params, tflite::micro::GetTensorShape(input), tflite::micro::GetTensorData<float>(input), &pad_value,
          tflite::micro::GetTensorShape(output), tflite::micro::GetTensorData<float>(output));
    }
    else
    {
      reference_ops::Pad(
          data->params, tflite::micro::GetTensorShape(input), tflite::micro::GetTensorData<float>(input), &pad_value,
          tflite::micro::GetTensorShape(output), tflite::micro::GetTensorData<float>(output));
    }
  }
  break;

  case kTfLiteInt8:
  {
    int8_t pad_value;
    if (constant_values == nullptr)
    {
      pad_value = static_cast<uint8_t>(data->output_zero_point);
    }
    else
    {
      pad_value = *tflite::micro::GetTensorData<int8_t>(constant_values);
    }

    if (tflite::micro::GetTensorShape(input).DimensionsCount() == 4)
    {
      cmsis_nn_dims input_size;
      cmsis_nn_dims pre_pad;
      cmsis_nn_dims post_pad;
      PopulateCommonParams(&input_size, &pre_pad, &post_pad, data, tflite::micro::GetTensorShape(input));
      arm_pad_s8(
          tflite::micro::GetTensorData<int8_t>(input), tflite::micro::GetTensorData<int8_t>(output), pad_value,
          &input_size, &pre_pad, &post_pad);
    }
    else
    {
      if (data->params.resizing_category == ResizingCategory::kImageStyle)
      {
        reference_ops::PadImageStyle(
            data->params, tflite::micro::GetTensorShape(input), tflite::micro::GetTensorData<int8_t>(input), &pad_value,
            tflite::micro::GetTensorShape(output), tflite::micro::GetTensorData<int8_t>(output));
      }
      else
      {
        reference_ops::Pad(
            data->params, tflite::micro::GetTensorShape(input), tflite::micro::GetTensorData<int8_t>(input), &pad_value,
            tflite::micro::GetTensorShape(output), tflite::micro::GetTensorData<int8_t>(output));
      }
    }
  }
  break;

  case kTfLiteInt16:
  {
    int16_t pad_value = constant_values == nullptr ? 0 : *tflite::micro::GetTensorData<int16_t>(constant_values);
    if (tflite::micro::GetTensorShape(input).DimensionsCount() == 4)
    {
      cmsis_nn_dims input_size;
      cmsis_nn_dims pre_pad;
      cmsis_nn_dims post_pad;
      PopulateCommonParams(&input_size, &pre_pad, &post_pad, data, tflite::micro::GetTensorShape(input));
      arm_pad_s16(
          tflite::micro::GetTensorData<int16_t>(input),
          tflite::micro::GetTensorData<int16_t>(output),
          pad_value, &input_size, &pre_pad, &post_pad
      );
    }
    else {
      reference_ops::Pad(
          data->params, tflite::micro::GetTensorShape(input), tflite::micro::GetTensorData<int16_t>(input), &pad_value,
          tflite::micro::GetTensorShape(output), tflite::micro::GetTensorData<int16_t>(output));
    }
  }
  break;

  case kTfLiteInt32:
  {
    int32_t pad_value = constant_values == nullptr ? 0 : *tflite::micro::GetTensorData<int32_t>(constant_values);
    reference_ops::Pad(
        data->params, tflite::micro::GetTensorShape(input), tflite::micro::GetTensorData<int32_t>(input), &pad_value,
        tflite::micro::GetTensorShape(output), tflite::micro::GetTensorData<int32_t>(output));
  }
  break;

  default:

    MicroPrintf("Type %s not currently supported by Pad.", TfLiteTypeGetName(input->type));
    return kTfLiteError;
  }
  return kTfLiteOk;
}

} // namespace

TFLMRegistration Register_PAD() { return tflite::micro::RegisterOp(Init, Prepare, Eval); }

// Also register Pad as PadV2.
TFLMRegistration Register_PADV2() { return tflite::micro::RegisterOp(Init, Prepare, Eval); }

} // namespace tflite
