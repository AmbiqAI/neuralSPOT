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
  * Pointwise s16 convolution function: no stride, no padding, no dilation.
  *
  * Refer header file for details. Optimal use case for the DSP/MVE implementation is when input and output channels
  * are multiples of 4 or atleast greater than 4.
  *
  */

 arm_cmsis_nn_status arm_convolve_1x1_s16_ns_np_nd(
    const cmsis_nn_conv_params *conv_params,
    const cmsis_nn_per_channel_quant_params *quant_params,
    const cmsis_nn_dims *input_dims,
    const int16_t *input_data,
    const cmsis_nn_dims *filter_dims,
    const int8_t *filter_data,
    const cmsis_nn_dims *bias_dims,
    const cmsis_nn_bias_data *bias_data,
    const cmsis_nn_dims *output_dims,
    int16_t *output_data)
 {
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
    const int32_t rhs_cols = input_ch * kernel_y * kernel_x;
    const int32_t out_activation_min = conv_params->activation.min;
    const int32_t out_activation_max = conv_params->activation.max;
    int32_t *output_mult = quant_params->multiplier;
    int32_t *output_shift = quant_params->shift;
    
    int32_t lhs_rows = output_x * output_y;
    int16_t *out = output_data;
    for (int i_batch = 0; i_batch < input_batches; i_batch++)
    {   
        arm_nn_mat_mult_nt_t_s16(input_data,
            filter_data,
            bias_data,
            out,
            output_mult,
            output_shift,
            lhs_rows,
            output_dims->c,
            rhs_cols,
            out_activation_min,
            out_activation_max,
            output_dims->c);
        
        if (out == NULL)
        {
            return ARM_CMSIS_NN_NO_IMPL_ERROR;
        }
        
        /* Advance to the next batch */
        input_data += (input_x * input_y * input_ch);
        output_data += (output_x * output_y * output_ch);
    }

    /* Return to application */
    return ARM_CMSIS_NN_SUCCESS;
 }

 /**
  * @} end of NNConv group
  */