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
 * Title:        arm_convolve_weight_sum_s8.c
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

arm_cmsis_nn_status arm_convolve_weight_sum(
        int32_t* vector_sum_buf,
        const int8_t *rhs,
        const cmsis_nn_dims *input_dims,
        const cmsis_nn_dims *filter_dims,
        const cmsis_nn_dims *output_dims,
        const int32_t lhs_offset,
        const int32_t *bias_data )
{
#if !defined(ARM_MATH_MVEI)
    (void)vector_sum_buf;
    (void)rhs;
    (void)input_dims;
    (void)filter_dims;
    (void)output_dims;
    (void)lhs_offset;
    (void)bias_data;
    return ARM_CMSIS_NN_NO_IMPL_ERROR;
#else //defined(ARM_MATH_MVEI)
    (void) input_dims;
    const uint16_t kernel_x = filter_dims->w;
    const uint16_t kernel_y = filter_dims->h;
    const uint16_t kernel_ch = filter_dims->c;
    const uint16_t output_channels = output_dims->c;
    const uint16_t rhs_cols = kernel_x * kernel_y * kernel_ch;
    arm_vector_sum_s8(vector_sum_buf, rhs_cols, output_channels, rhs, lhs_offset, 0, bias_data);
    return ARM_CMSIS_NN_SUCCESS;
#endif
}
