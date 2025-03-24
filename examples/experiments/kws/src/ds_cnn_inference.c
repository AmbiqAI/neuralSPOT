/******************************************************************************
 * ds_cnn_inference.c
 *
 * Full DS–CNN inference implementation for int8–quantized keyword spotting.
 *
 * This version has been rewritten to avoid dynamic memory allocation and memcpy.
 * Instead, two static buffers are declared for the intermediate results.
 *
 * The network implemented here is:
 *   Input (assumed shape: 1 x 49 x 10 x 1) -->
 *     conv1 (normal convolution) + BN + ReLU -->
 *     4 blocks each: { depthwise conv + BN + ReLU, then pointwise conv + BN + ReLU } -->
 *     Global average pooling -->
 *     Fully connected layer (with per–channel quantization) -->
 *     Softmax.
 *
 ******************************************************************************/

 #include <stdio.h>
 #include <stdint.h>
 #include <string.h>
 #include "ns_ambiqsuite_harness.h"
 
 /* CMSIS–NN functions and types.
    (Assume that arm_nnfunctions.h from CMSIS–NN is in your include path.) */
 #include "arm_nnfunctions.h"
 
 /* Generated data from the TFLite flatbuffer (weights, biases, quantization, test input,
    and expected output). This file is produced by generate_ds_cnn_data.py. */
 #include "ds_cnn_data.h"
 
#ifdef __cplusplus
    extern "C" {
#endif

 /*----------------------------------------------------------------------------
   Define layer dimensions and parameters.
  *---------------------------------------------------------------------------*/
 
 #define BATCH_SIZE       1
 
 /* Assume the input is a 49 x 10 mel spectrogram with 1 channel */
 #define INPUT_H          49
 #define INPUT_W          10
 #define INPUT_C          1
 
 /* --- Conv1 parameters (first convolution) --- */
 #define CONV1_OUT_C      64
 #define CONV1_KH         10
 #define CONV1_KW         4
 #define CONV1_STRIDE_H   2
 #define CONV1_STRIDE_W   2
 /* For “SAME” padding, we assume: */
 #define CONV1_PAD_H      ((CONV1_KH - 1) / 2)
 #define CONV1_PAD_W      ((CONV1_KW - 1) / 2)
 /* Output dimensions computed as ceiling(INPUT/(stride)) */
 #define CONV1_OUT_H      (((INPUT_H + CONV1_STRIDE_H - 1)) / (CONV1_STRIDE_H))
 #define CONV1_OUT_W      (((INPUT_W + CONV1_STRIDE_W - 1)) / (CONV1_STRIDE_W))
 
 /* --- DS–CNN Block parameters (each block has a depthwise conv and a pointwise conv) --- */
 #define DW_KH            3
 #define DW_KW            3
 #define DW_STRIDE_H      1
 #define DW_STRIDE_W      1
 #define DW_PAD_H         ((DW_KH - 1) / 2)
 #define DW_PAD_W         ((DW_KW - 1) / 2)
 #define PW_KH            1
 #define PW_KW            1
 
 /* For all blocks, assume the spatial dimensions remain the same as conv1_out */
 #define BLOCK_OUT_H      CONV1_OUT_H
 #define BLOCK_OUT_W      CONV1_OUT_W
 #define BLOCK_OUT_C      CONV1_OUT_C
 
 /* --- Fully connected layer --- */
 #define FC_OUT           12  /* number of keyword classes */
 
 /* Compute size of the conv1 (and block) output buffer */
 #define CONV1_OUT_SIZE   (CONV1_OUT_H * CONV1_OUT_W * CONV1_OUT_C)
 
 /*----------------------------------------------------------------------------
   Statically allocated buffers.
   Two buffers are used to alternate intermediate results.
  *---------------------------------------------------------------------------*/
 static int8_t conv1_output_buffer[CONV1_OUT_SIZE];
 static int8_t block_buffer[CONV1_OUT_SIZE];
 
 /* CMSIS–NN context (using a statically allocated buffer for internal scratch data) */
 cmsis_nn_context fc_ctx;
 static uint8_t ctx_buffer[4096];
 
 /*----------------------------------------------------------------------------
   Helper functions
  *---------------------------------------------------------------------------*/
 
 /* A simple per–element batch normalization followed by ReLU.
    Here we assume that the BN parameters are given per–tensor (or the first element is used).
  */
 static void apply_batch_norm_relu_s8(int8_t *data, int32_t size,
                                      const int32_t *bn_multiplier, const int32_t *bn_shift)
 {
     for (int i = 0; i < size; i++) {
         int32_t x = data[i];
         /* Apply BN: y = x * multiplier + shift. */
         x = x * bn_multiplier[0] + bn_shift[0];
         /* Apply ReLU: clamp negatives to 0, and saturate to 127 */
         if (x < 0)
             x = 0;
         if (x > 127)
             x = 127;
         data[i] = (int8_t)x;
     }
 }
 
 /*----------------------------------------------------------------------------
   Layer wrapper functions
  *---------------------------------------------------------------------------*/
 
 /* Standard 2D convolution with BN and ReLU.
    Uses CMSIS–NN’s arm_convolve_wrapper_s8.
    This version writes directly into the provided output_data buffer.
  */
 static arm_cmsis_nn_status conv2d_bn_relu(const cmsis_nn_conv_params *conv_params,
                                           const cmsis_nn_per_tensor_quant_params *quant_params,
                                           const cmsis_nn_dims *input_dims,
                                           const int8_t *input_data,
                                           const cmsis_nn_dims *filter_dims,
                                           const int8_t *filter_data,
                                           const cmsis_nn_dims *bias_dims,
                                           const int32_t *bias_data,
                                           const cmsis_nn_dims *output_dims,
                                           int8_t *output_data,
                                           const int32_t *bn_multiplier, const int32_t *bn_shift)
 {
     cmsis_nn_per_channel_quant_params local_quant;
     int32_t local_multiplier = quant_params->multiplier;
     int32_t local_shift = quant_params->shift;
     local_quant.multiplier = &local_multiplier;
     local_quant.shift = &local_shift;
 
     int32_t out_size = output_dims->h * output_dims->w * output_dims->c;
     /* Zero out the output buffer if required by the CMSIS–NN function */
     memset(output_data, 0, out_size);
     arm_cmsis_nn_status status = arm_convolve_wrapper_s8(&fc_ctx, conv_params, &local_quant,
                                                          input_dims, input_data,
                                                          filter_dims, filter_data,
                                                          bias_dims, bias_data,
                                                          output_dims, output_data);
     if (status != ARM_CMSIS_NN_SUCCESS)
         return status;
     /* Apply BN and ReLU in place */
     apply_batch_norm_relu_s8(output_data, out_size, bn_multiplier, bn_shift);
     return ARM_CMSIS_NN_SUCCESS;
 }
 
 /* Depthwise convolution with BN and ReLU.
    Uses arm_depthwise_conv_wrapper_s8.
  */
 static arm_cmsis_nn_status depthwise_conv2d_bn_relu(const cmsis_nn_dw_conv_params *dw_conv_params,
                                                      const cmsis_nn_per_tensor_quant_params *quant_params,
                                                      const cmsis_nn_dims *input_dims,
                                                      const int8_t *input_data,
                                                      const cmsis_nn_dims *filter_dims,
                                                      const int8_t *filter_data,
                                                      const cmsis_nn_dims *bias_dims,
                                                      const int32_t *bias_data,
                                                      const cmsis_nn_dims *output_dims,
                                                      int8_t *output_data,
                                                      const int32_t *bn_multiplier, const int32_t *bn_shift)
 {
     cmsis_nn_per_channel_quant_params local_quant;
     int32_t local_multiplier = quant_params->multiplier;
     int32_t local_shift = quant_params->shift;
     local_quant.multiplier = &local_multiplier;
     local_quant.shift = &local_shift;
 
     int32_t out_size = output_dims->h * output_dims->w * output_dims->c;
     memset(output_data, 0, out_size);
     arm_cmsis_nn_status status = arm_depthwise_conv_wrapper_s8(&fc_ctx, dw_conv_params, &local_quant,
                                                                input_dims, input_data,
                                                                filter_dims, filter_data,
                                                                bias_dims, bias_data,
                                                                output_dims, output_data);
     if (status != ARM_CMSIS_NN_SUCCESS)
         return status;
     apply_batch_norm_relu_s8(output_data, out_size, bn_multiplier, bn_shift);
     return ARM_CMSIS_NN_SUCCESS;
 }
 
 /* Pointwise (1x1) convolution with BN and ReLU.
    Uses arm_convolve_wrapper_s8.
  */
 static arm_cmsis_nn_status pointwise_conv2d_bn_relu(const cmsis_nn_conv_params *pw_params,
                                                      const cmsis_nn_per_tensor_quant_params *quant_params,
                                                      const cmsis_nn_dims *input_dims,
                                                      const int8_t *input_data,
                                                      const cmsis_nn_dims *filter_dims,
                                                      const int8_t *filter_data,
                                                      const cmsis_nn_dims *bias_dims,
                                                      const int32_t *bias_data,
                                                      const cmsis_nn_dims *output_dims,
                                                      int8_t *output_data,
                                                      const int32_t *bn_multiplier, const int32_t *bn_shift)
 {
     cmsis_nn_per_channel_quant_params local_quant;
     int32_t local_multiplier = quant_params->multiplier;
     int32_t local_shift = quant_params->shift;
     local_quant.multiplier = &local_multiplier;
     local_quant.shift = &local_shift;
     
     int32_t out_size = output_dims->h * output_dims->w * output_dims->c;
     memset(output_data, 0, out_size);
     arm_cmsis_nn_status status = arm_convolve_wrapper_s8(&fc_ctx, pw_params, &local_quant,
                                                          input_dims, input_data,
                                                          filter_dims, filter_data,
                                                          bias_dims, bias_data,
                                                          output_dims, output_data);
     if (status != ARM_CMSIS_NN_SUCCESS)
         return status;
     apply_batch_norm_relu_s8(output_data, out_size, bn_multiplier, bn_shift);
     return ARM_CMSIS_NN_SUCCESS;
 }
 
 /* Global average pooling over the spatial dimensions.
    For each channel, compute the average over (height*width).
  */
 static void global_average_pooling_s8(const cmsis_nn_dims *input_dims,
                                       const int8_t *input_data, int8_t *output_data)
 {
     int h = input_dims->h;
     int w = input_dims->w;
     int c = input_dims->c;
     for (int ch = 0; ch < c; ch++) {
         int32_t sum = 0;
         for (int i = 0; i < h * w; i++) {
             sum += input_data[i * c + ch];
         }
         int avg = sum / (h * w);
         if (avg < -128) avg = -128;
         if (avg > 127) avg = 127;
         output_data[ch] = (int8_t)avg;
     }
 }
 
 /* Fully connected layer using per–channel quantization.
    Uses arm_fully_connected_wrapper_s8 from CMSIS–NN.
  */
 extern const cmsis_nn_per_channel_quant_params fc_quant_params;  // Declared in ds_cnn_data.h
 
 static arm_cmsis_nn_status fully_connected_layer(const cmsis_nn_dims *input_dims,
                                                    const int8_t *input_data,
                                                    const cmsis_nn_dims *filter_dims,
                                                    const int8_t *filter_data,
                                                    const cmsis_nn_dims *bias_dims,
                                                    const int32_t *bias_data,
                                                    const cmsis_nn_dims *output_dims,
                                                    int8_t *output_data)
 {
     cmsis_nn_fc_params fc_params;
     fc_params.input_offset = 0;
     fc_params.filter_offset = 0;
     fc_params.output_offset = 0;
     fc_params.activation.min = 0;
     fc_params.activation.max = 127;
 
     cmsis_nn_quant_params fc_quant;
     fc_quant.multiplier = fc_quant_params.multiplier;
     fc_quant.shift = fc_quant_params.shift;
 
     return arm_fully_connected_wrapper_s8(&fc_ctx, &fc_params, &fc_quant,
                                             input_dims, input_data,
                                             filter_dims, filter_data,
                                             bias_dims, bias_data,
                                             output_dims, output_data);
 }
 
 /* Softmax using CMSIS–NN. */
 static void softmax_layer_s8(const int8_t *input, int num_rows, int row_size, int8_t *output)
 {
     /* Dummy quantization parameters for softmax; adjust as needed */
     int32_t mult = 1;
     int32_t shift = 0;
     int32_t diff_min = -128;
     arm_softmax_s8(input, num_rows, row_size, mult, shift, diff_min, output);
 }
 
 /*----------------------------------------------------------------------------
   The DS–CNN inference function.
  *---------------------------------------------------------------------------*/
 arm_cmsis_nn_status ds_cnn_inference(const int8_t *input, int8_t *output)
 {
     arm_cmsis_nn_status status;
 
     /* Set up the context with the static buffer */
     fc_ctx.buf = ctx_buffer;
     fc_ctx.size = sizeof(ctx_buffer);
 
     /* --- Layer 1: conv1 --- */
     cmsis_nn_dims in_dims = { .n = BATCH_SIZE, .h = INPUT_H, .w = INPUT_W, .c = INPUT_C };
     cmsis_nn_dims conv1_filter_dims = { .n = CONV1_OUT_C, .h = CONV1_KH, .w = CONV1_KW, .c = INPUT_C };
     cmsis_nn_dims conv1_bias_dims   = { .n = 0, .h = 0, .w = 0, .c = CONV1_OUT_C };
     cmsis_nn_dims conv1_out_dims    = { .n = BATCH_SIZE, .h = CONV1_OUT_H, .w = CONV1_OUT_W, .c = CONV1_OUT_C };
 
     cmsis_nn_conv_params conv1_params = {
         .stride = { .h = CONV1_STRIDE_H, .w = CONV1_STRIDE_W },
         .padding = { .h = CONV1_PAD_H, .w = CONV1_PAD_W },
         .input_offset = 0,
         .output_offset = 0,
         .activation = { .min = 0, .max = 127 }
     };
     /* Per–tensor quantization parameters for conv1 from generated data */
     cmsis_nn_per_tensor_quant_params conv1_quant = {
         .multiplier = conv1_multiplier[0],
         .shift = conv1_shift[0]
     };
 
     /* Use the statically allocated conv1_output_buffer */
     status = conv2d_bn_relu(&conv1_params, &conv1_quant, &in_dims, input,
                             &conv1_filter_dims, conv1_weights,
                             &conv1_bias_dims, conv1_bias,
                             &conv1_out_dims, conv1_output_buffer,
                             conv1_bn_multiplier, conv1_bn_shift);
     if (status != ARM_CMSIS_NN_SUCCESS)
         return status;
 
     /* --- DS–CNN Blocks (4 blocks, each with depthwise conv then pointwise conv) --- */
     cmsis_nn_dims block_in_dims = conv1_out_dims;
     cmsis_nn_dims block_out_dims = conv1_out_dims;
 
     /* Depthwise conv parameters: kernel 3x3, stride 1, SAME padding */
     cmsis_nn_dw_conv_params dw_params = {
         .stride = { .h = DW_STRIDE_H, .w = DW_STRIDE_W },
         .padding = { .h = DW_PAD_H, .w = DW_PAD_W },
         .input_offset = 0,
         .output_offset = 0,
         .activation = { .min = 0, .max = 127 }
     };
     cmsis_nn_dims dw_bias_dims = { .n = 0, .h = 0, .w = 0, .c = BLOCK_OUT_C };
 
     /* Pointwise conv parameters: 1x1 conv, stride 1, no padding */
     cmsis_nn_conv_params pw_params = {
         .stride = { .h = 1, .w = 1 },
         .padding = { .h = 0, .w = 0 },
         .input_offset = 0,
         .output_offset = 0,
         .activation = { .min = 0, .max = 127 }
     };
     cmsis_nn_dims pw_bias_dims = { .n = 0, .h = 0, .w = 0, .c = BLOCK_OUT_C };
 
     /* Quantization parameters for depthwise and pointwise conv layers */
     cmsis_nn_per_tensor_quant_params dw_quant = {
         .multiplier = dw_conv_multiplier[0],
         .shift = dw_conv_shift[0]
     };
     cmsis_nn_per_tensor_quant_params pw_quant = {
         .multiplier = pw_conv_multiplier[0],
         .shift = pw_conv_shift[0]
     };
 
     /* For intermediate results, alternate between conv1_output_buffer and block_buffer */
 
     /* --- Block 1 --- */
     status = depthwise_conv2d_bn_relu(&dw_params, &dw_quant, &block_in_dims, conv1_output_buffer,
                                       &((cmsis_nn_dims){.n=0, .h=DW_KH, .w=DW_KW, .c=BLOCK_OUT_C}),
                                       block1_dw_weights, &dw_bias_dims, block1_dw_bias,
                                       &block_out_dims, block_buffer,
                                       block1_dw_bn_multiplier, block1_dw_bn_shift);
     if (status != ARM_CMSIS_NN_SUCCESS)
         return status;
     status = pointwise_conv2d_bn_relu(&pw_params, &pw_quant, &block_in_dims, block_buffer,
                                       &((cmsis_nn_dims){.n=BLOCK_OUT_C, .h=PW_KH, .w=PW_KW, .c=BLOCK_OUT_C}),
                                       block1_pw_weights, &pw_bias_dims, block1_pw_bias,
                                       &block_out_dims, conv1_output_buffer,
                                       block1_pw_bn_multiplier, block1_pw_bn_shift);
     if (status != ARM_CMSIS_NN_SUCCESS)
         return status;
 
     /* --- Block 2 --- */
     status = depthwise_conv2d_bn_relu(&dw_params, &dw_quant, &block_in_dims, conv1_output_buffer,
                                       &((cmsis_nn_dims){.n=0, .h=DW_KH, .w=DW_KW, .c=BLOCK_OUT_C}),
                                       block2_dw_weights, &dw_bias_dims, block2_dw_bias,
                                       &block_out_dims, block_buffer,
                                       block2_dw_bn_multiplier, block2_dw_bn_shift);
     if (status != ARM_CMSIS_NN_SUCCESS)
         return status;
     status = pointwise_conv2d_bn_relu(&pw_params, &pw_quant, &block_in_dims, block_buffer,
                                       &((cmsis_nn_dims){.n=BLOCK_OUT_C, .h=PW_KH, .w=PW_KW, .c=BLOCK_OUT_C}),
                                       block2_pw_weights, &pw_bias_dims, block2_pw_bias,
                                       &block_out_dims, conv1_output_buffer,
                                       block2_pw_bn_multiplier, block2_pw_bn_shift);
     if (status != ARM_CMSIS_NN_SUCCESS)
         return status;
 
     /* --- Block 3 --- */
     status = depthwise_conv2d_bn_relu(&dw_params, &dw_quant, &block_in_dims, conv1_output_buffer,
                                       &((cmsis_nn_dims){.n=0, .h=DW_KH, .w=DW_KW, .c=BLOCK_OUT_C}),
                                       block3_dw_weights, &dw_bias_dims, block3_dw_bias,
                                       &block_out_dims, block_buffer,
                                       block3_dw_bn_multiplier, block3_dw_bn_shift);
     if (status != ARM_CMSIS_NN_SUCCESS)
         return status;
     status = pointwise_conv2d_bn_relu(&pw_params, &pw_quant, &block_in_dims, block_buffer,
                                       &((cmsis_nn_dims){.n=BLOCK_OUT_C, .h=PW_KH, .w=PW_KW, .c=BLOCK_OUT_C}),
                                       block3_pw_weights, &pw_bias_dims, block3_pw_bias,
                                       &block_out_dims, conv1_output_buffer,
                                       block3_pw_bn_multiplier, block3_pw_bn_shift);
     if (status != ARM_CMSIS_NN_SUCCESS)
         return status;
 
     /* --- Block 4 --- */
     status = depthwise_conv2d_bn_relu(&dw_params, &dw_quant, &block_in_dims, conv1_output_buffer,
                                       &((cmsis_nn_dims){.n=0, .h=DW_KH, .w=DW_KW, .c=BLOCK_OUT_C}),
                                       block4_dw_weights, &dw_bias_dims, block4_dw_bias,
                                       &block_out_dims, block_buffer,
                                       block4_dw_bn_multiplier, block4_dw_bn_shift);
     if (status != ARM_CMSIS_NN_SUCCESS)
         return status;
     status = pointwise_conv2d_bn_relu(&pw_params, &pw_quant, &block_in_dims, block_buffer,
                                       &((cmsis_nn_dims){.n=BLOCK_OUT_C, .h=PW_KH, .w=PW_KW, .c=BLOCK_OUT_C}),
                                       block4_pw_weights, &pw_bias_dims, block4_pw_bias,
                                       &block_out_dims, conv1_output_buffer,
                                       block4_pw_bn_multiplier, block4_pw_bn_shift);
     if (status != ARM_CMSIS_NN_SUCCESS)
         return status;
 
     /* --- Global Average Pooling --- */
     int8_t gap_output[BLOCK_OUT_C];
     global_average_pooling_s8(&block_out_dims, conv1_output_buffer, gap_output);
 
     /* --- Fully Connected Layer --- */
     cmsis_nn_dims fc_in_dims = { .n = 1, .h = 1, .w = 1, .c = BLOCK_OUT_C };
     cmsis_nn_dims fc_bias_dims = { .n = 0, .h = 0, .w = 0, .c = FC_OUT };
     cmsis_nn_dims fc_out_dims = { .n = 1, .h = 1, .w = 1, .c = FC_OUT };
 
     status = fully_connected_layer(&fc_in_dims, gap_output,
                                    &((cmsis_nn_dims){.n = FC_OUT, .h = 1, .w = 1, .c = BLOCK_OUT_C}),
                                    fc_weights,
                                    &fc_bias_dims, fc_bias,
                                    &fc_out_dims, output);
     if (status != ARM_CMSIS_NN_SUCCESS)
         return status;
 
     /* --- Softmax --- */
     softmax_layer_s8(output, 1, FC_OUT, output);
 
     return ARM_CMSIS_NN_SUCCESS;
 }
 
 /*----------------------------------------------------------------------------
   Unit Test Main
  *---------------------------------------------------------------------------*/
 int main_test(void)
 {
     int8_t output[FC_OUT];
     arm_cmsis_nn_status status = ds_cnn_inference(test_input, output);
     if (status != ARM_CMSIS_NN_SUCCESS) {
         ns_lp_printf("DS–CNN inference failed with status %d\n", status);
         return -1;
     }
     
     /* Compare the output with expected_output (both arrays are generated by the Python script) */
     int pass = 1;
     for (int i = 0; i < FC_OUT; i++) {
         if (output[i] != expected_output[i]) {
             pass = 0;
             break;
         }
     }
     if (pass) {
         ns_lp_printf("DS–CNN unit test PASSED.\n");
     } else {
         ns_lp_printf("DS–CNN unit test FAILED.\n");
         ns_lp_printf("Expected: ");
         for (int i = 0; i < FC_OUT; i++)
             ns_lp_printf("%d ", expected_output[i]);
         ns_lp_printf("\nGot: ");
         for (int i = 0; i < FC_OUT; i++)
             ns_lp_printf("%d ", output[i]);
         ns_lp_printf("\n");
     }
     return 0;
 }
 #ifdef __cplusplus
 }
 #endif
 