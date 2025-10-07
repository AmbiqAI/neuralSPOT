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
 * Title:        arm_depthwise_convolve_weight_sum.c
 * Description:  s8 version of convolution using symmetric quantization.
 *
 * $Date:        30 April 2025
 * $Revision:    V.4.0.0
 *
 * Target :  Arm(R) M-Profile Architecture
 *
 * -------------------------------------------------------------------- */

#include "arm_nnfunctions.h"
#include "arm_nnsupportfunctions.h"

arm_cmsis_nn_status arm_depthwise_convolve_weight_sum(int32_t* vector_sum_buf,
                                                      int8_t* scratch_buf,
                                                      const int8_t *rhs,
                                                      const cmsis_nn_dw_conv_params *dw_conv_params,
                                                      const cmsis_nn_dims *input_dims,
                                                      const cmsis_nn_dims *filter_dims,
                                                      const cmsis_nn_dims *output_dims,
                                                      const int32_t lhs_offset,
                                                      const int32_t *bias_data)
{

#if !defined(ARM_MATH_MVEI)
    (void)vector_sum_buf;
    (void)scratch_buf;
    (void)rhs;
    (void)dw_conv_params;
    (void)input_dims;
    (void)filter_dims;
    (void)output_dims;
    (void)lhs_offset;
    (void)bias_data;
    return ARM_CMSIS_NN_NO_IMPL_ERROR;
#else //defined(ARM_MATH_MVEI)
    if (vector_sum_buf == NULL)
    {
        return ARM_CMSIS_NN_ARG_ERROR;
    }

    const uint16_t kernel_x = filter_dims->w;
    const uint16_t kernel_y = filter_dims->h;
    const uint16_t output_channels = output_dims->c;
    (void)input_dims;
    (void)scratch_buf;
    (void)dw_conv_params;
    int32_t total_ch = output_channels;
    int32_t row_x_col = kernel_x * kernel_y;
    int32_t ch_left = total_ch;
    const int8_t* rhs_base = rhs;
    const int8_t* rhs_runner = rhs_base;

    if (bias_data)
    {
        memcpy(vector_sum_buf, bias_data, output_channels * sizeof(int32_t));
    }
    else
    {
        memset(vector_sum_buf, 0, output_channels * sizeof(int32_t));
    }
    if (lhs_offset ) {
        for (int i =0; i < (total_ch + 3) / 4; ++i) {
            mve_pred16_t p = vctp32q(ch_left);
            int32x4_t ker_sum = vdupq_n_s32(0);
            rhs_runner = rhs_base;
            for (int i_row_x_col = 0; i_row_x_col < row_x_col; i_row_x_col++) {
                //run through all of the kernel values for these 4 channels
                const int32x4_t ker_0 = vldrbq_z_s32(rhs_runner, p);
                ker_sum = vaddq_s32(ker_sum, ker_0);
                rhs_runner += total_ch;
            }
            ker_sum = vmulq_n_s32(ker_sum, lhs_offset);
            //vstrwq_p_s32(sum_buf_runner, ker_sum, p);
            for(int j = 0; j < MIN(4, ch_left); ++j) {
                vector_sum_buf[j] += ker_sum[j];
            }
            rhs_base += 4;
            vector_sum_buf += 4;
            ch_left -= 4;

        }
    }
#endif
    return ARM_CMSIS_NN_SUCCESS;
}
