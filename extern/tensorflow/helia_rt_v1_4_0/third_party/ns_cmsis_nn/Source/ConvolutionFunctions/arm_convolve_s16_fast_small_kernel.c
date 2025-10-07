/*
 * SPDX-FileCopyrightText: Copyright 2010-2024 Arm Limited and/or its affiliates <open-source-office@arm.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* ----------------------------------------------------------------------
 * Project:      CMSIS NN Library
 * Title:        arm_convolve_s16.c
 * Description:  s16 version of convolution.
 *
 * $Date:        22 April 2024
 * $Revision:    V.4.0.0
 *
 * Target :  Arm(R) M-Profile Architecture
 *
 * -------------------------------------------------------------------- */

#include "arm_nnfunctions.h"
#include "arm_nnsupportfunctions.h"

/**
 *  @ingroup Public
 */

/**
 * @addtogroup NNConv
 * @{
 */

/*
 * arm_convolve_s16_fast_small_kernel function.
 *
 * Refer header file for details. Optimal use case for the DSP/MVE implementation is when input and output channels
 * are multiples of 4 or atleast greater than 4.
 *
 */
// #include <stdio.h>
arm_cmsis_nn_status arm_convolve_s16_fast_small_kernel(
    const cmsis_nn_context *ctx,
    const cmsis_nn_conv_params *conv_params,
    const cmsis_nn_per_channel_quant_params *quant_params,
    const cmsis_nn_dims *input_dims,
    const int16_t *input_data, // Format: [N, H, W, C_IN]
    const cmsis_nn_dims *filter_dims,// Format: [C_OUT, HK, WK, C_IN]
    const int8_t *filter_data,
    const cmsis_nn_dims *bias_dims,
    const cmsis_nn_bias_data *bias_data,
    const cmsis_nn_dims *output_dims,
    int16_t *output_data)
{
#if defined(ARM_MATH_MVEI)
    (void)ctx;
    (void)bias_dims;

    const int32_t input_batches = input_dims->n;
    const int32_t input_x = input_dims->w;
    const int32_t input_y = input_dims->h;
    const int32_t input_ch = input_dims->c;
    const int32_t kernel_x = filter_dims->w;
    const int32_t kernel_y = filter_dims->h;
    const int32_t output_x = output_dims->w;
    const int32_t output_y = output_dims->h;
    const int32_t output_ch = output_dims->c;


    const int32_t dilation_x = conv_params->dilation.w;
    const int32_t dilation_y = conv_params->dilation.h;

    const int32_t stride_x = conv_params->stride.w;
    const int32_t stride_y = conv_params->stride.h;

    const int32_t out_activation_min = conv_params->activation.min;
    const int32_t out_activation_max = conv_params->activation.max;
    int32_t *output_mult = quant_params->multiplier;
    int32_t *output_shift = quant_params->shift;

    const int32_t kernel_ch = filter_dims->c;
    const int32_t rhs_cols = kernel_ch * kernel_y * kernel_x;
    const int32_t groups = input_ch / kernel_ch;
    const int32_t output_ch_per_group = output_ch / groups;

    if (input_ch % groups != 0 || output_ch % groups != 0)
    {
        return ARM_CMSIS_NN_ARG_ERROR;
    }
    int32_t stride_edge = input_x - (output_x-1) * stride_x + (stride_y-1) * input_x;
    uint16x8_t offset_src;
    if (rhs_cols < 9)
    {
        for (int i = 0; i < kernel_y; i++)
        {
            int id = i * dilation_y;
            for (int j = 0; j < kernel_x; j++)
            {
                int jd= j * dilation_x;
                int idx = (i*kernel_x+j);
                for (int c = 0; c < kernel_ch; c++)
                {
                    int idc = idx * kernel_ch + c;
                    offset_src[idc] = (id * input_x + jd) * input_ch + c;
                    offset_src[idc] *= sizeof(int16_t);
                }

            }
        }
    }

    mve_pred16_t p = vctp16q(rhs_cols);
    for (int i_batch = 0; i_batch < input_batches; i_batch++)
    {

        const int8_t *filter_data_ptr = &filter_data[0];
        const int32_t *output_mult_ptr = &output_mult[0];
        const int32_t *output_shift_ptr = &output_shift[0];
        const bool is_int32_bias = bias_data->is_int32_bias;
        const int64_t *bias_s64 = (const int64_t *)bias_data->data;
        const int32_t *bias_s32 = (const int32_t *)bias_data->data;

        for (int32_t i_group = 0; i_group < groups; i_group++){
            /*
                Output shape= [HK, WK, C_OUT]
                where  C_OUT = groups * output_ch_per_group
                Hence, at every group, output channel points to

                    output + i_group * output_ch_per_group
            */


            for (int c = 0; c < output_ch_per_group; c++)
            {
                int16_t *input_data_pr = (int16_t*) input_data + i_group * kernel_ch;
                int16_t *out_c = output_data + i_group * output_ch_per_group + c;
                int16x8_t weight = vldrbq_z_s16(filter_data_ptr, p); // load weight
                filter_data_ptr+= rhs_cols;

                int32_t bias_s32_val=0;
                int64_t bias_s64_val=0;

                int32_t reduced_multiplier=0;
                int32_t multiplier = *output_mult_ptr++;
                int32_t shift = *output_shift_ptr++;

                if (is_int32_bias)
                {
                    if (bias_s32)
                    {
                        bias_s32_val = *bias_s32++;
                    }

                }
                else
                {
                    if (bias_s64)
                    {
                        bias_s64_val = *bias_s64++;
                    }
                    reduced_multiplier = REDUCE_MULTIPLIER(multiplier);
                }

                for (int32_t i_out_y = 0; i_out_y < output_y; i_out_y++)
                {
                    for (int32_t i_out_x = 0; i_out_x < output_x-1; i_out_x++)
                    {
                        /*
                        Extract kernel_y * kernel_x * kernel_ch
                        and put it to a row of the im2col (input) matrix
                        im2col = [  kernel_y * kernel_x * kernel_ch;
                                    kernel_y * kernel_x * kernel_ch;
                                    kernel_y * kernel_x * kernel_ch;
                                    kernel_y * kernel_x * kernel_ch ],
                        where lhs_rows = 4 in this example

                        Filter has the shape  [C_OUT, HK, WK, kernel_ch]
                        Think it as the shape [C_OUT, rhs_cols],
                            where rhs_cols = kernel_ch * HK * WK

                        At every group, filter_data_ptr points to
                            filter + i_group * output_ch_per_group * rhs_cols
                        */

                        int16x8_t in = vldrhq_gather_offset_z_s16(input_data_pr, offset_src, p);
                        input_data_pr += input_ch * stride_x;
                        int32_t result = vmladavq_s16(weight, in); // perform MAC
                        if (is_int32_bias)
                        {

                            if (bias_s32)
                            {
                                result += bias_s32_val;
                            }

                            result = arm_nn_requantize(result, multiplier, shift);

                        }
                        else
                        {
                            int64_t acc_s64 = result;

                            if (bias_s64)
                            {
                                acc_s64 += bias_s64_val;
                            }
                            result = arm_nn_requantize_s64(acc_s64, reduced_multiplier, shift);
                        }
                        result = MAX(result, out_activation_min);
                        result = MIN(result, out_activation_max);
                        *out_c = (int16_t) result;
                        out_c += output_ch;
                    }
                    // in the boundary of output_x
                    {

                        int16x8_t in = vldrhq_gather_offset_z_s16(input_data_pr, offset_src,p);
                        input_data_pr += input_ch * stride_edge;
                        int32_t result = vmladavq_s16(weight, in); // perform MAC
                        if (is_int32_bias)
                        {

                            if (bias_s32)
                            {
                                result += bias_s32_val;
                            }
                            result = arm_nn_requantize(result, multiplier, shift);

                        }
                        else
                        {
                            int64_t acc_s64 = result;

                            if (bias_s64)
                            {
                                acc_s64 += bias_s64_val;
                            }

                            result = arm_nn_requantize_s64(acc_s64, reduced_multiplier, shift);

                        }
                        result = MAX(result, out_activation_min);
                        result = MIN(result, out_activation_max);
                        *out_c = (int16_t) result;
                        out_c += output_ch;
                    }

                    if (out_c == NULL)
                    {
                        return ARM_CMSIS_NN_NO_IMPL_ERROR;
                    }

                }

            } // (int c = 0; c < output_ch_per_group; c++)
        } // i_group
        /* Advance to the next batch */

        input_data += (input_x * input_y * input_ch);
        output_data += (output_x * output_y * output_ch);
    }
#else
    (void)ctx;
    (void)conv_params;
    (void)quant_params;
    (void)input_dims;
    (void)input_data;
    (void)filter_dims;
    (void)filter_data;
    (void)bias_dims;
    (void)bias_data;
    (void)output_dims;
    (void)output_data;
    return ARM_CMSIS_NN_NO_IMPL_ERROR;
#endif
    /* Return to application */
    return ARM_CMSIS_NN_SUCCESS;
}

/**
 * @} end of NNConv group
 */
