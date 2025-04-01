/*
 * SPDX-FileCopyrightText: Copyright 2020-2024 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
 * Title:        arm_nn_vec_mat_mult_t_fp16
 * Description:  fp16 vector by fp16 matrix (transposed) multiplication
 *
 * $Date:        19 June 2024
 * $Revision:    V.2.4.0
 *
 * Target :  Arm(R) M-Profile Architecture
 *
 * -------------------------------------------------------------------- */

#include "arm_nnsupportfunctions.h"

/**
 * @ingroup groupSupport
 */

/**
 * @addtogroup supportFC
 * @{
 */

#if defined(ARM_MATH_MVE_FLOAT16) && !defined(ARM_MATH_AUTOVECTORIZE)

__STATIC_FORCEINLINE float16_t vec_add_across_f16_mve(float16x8_t in)
{
    float16x8_t tmp_vec;
    float16_t acc;

    tmp_vec = (float16x8_t) vrev32q_s16((int16x8_t) in);
    in = vaddq_f16(tmp_vec, in);
    tmp_vec = (float16x8_t) vrev64q_s32((int32x4_t) in);
    in = vaddq_f16(tmp_vec, in);
    acc = (float16_t)vgetq_lane_f16(in, 0) + (float16_t)vgetq_lane_f16(in, 4);

    return acc;
}

/*
 * fp16 vector(lhs) by fp16 matrix (transposed) multiplication
 *
 * Refer header file for details.
 *
 */
arm_cmsis_nn_status arm_nn_vec_mat_mult_t_fp16(
    const float16_t *lhs,
    const float16_t *rhs,
    const float16_t *bias,
    float16_t       *dst,
    const int32_t    rhs_cols,
    const int32_t    rhs_rows,
    const float16_t  activation_min,
    const float16_t  activation_max
) {

    const int32_t unroll_factor = 4;
    const int32_t row_unroll = rhs_rows / unroll_factor;
    const int32_t rem_rows = rhs_rows - (row_unroll * unroll_factor);

    for (int i = 0; i < row_unroll; i++)
    {
        // Create four pointers to consecutive rows of the rhs matrix.
        const float16_t *rhs_ptr0 = rhs + (i * unroll_factor + 0) * rhs_cols;
        const float16_t *rhs_ptr1 = rhs + (i * unroll_factor + 1) * rhs_cols;
        const float16_t *rhs_ptr2 = rhs + (i * unroll_factor + 2) * rhs_cols;
        const float16_t *rhs_ptr3 = rhs + (i * unroll_factor + 3) * rhs_cols;
        const float16_t *lhs_ptr = lhs; // lhs is common to all dot products

        // Initialize vector accumulators for each output row.
        float16x8_t acc0 = vdupq_n_f16(0);
        float16x8_t acc1 = vdupq_n_f16(0);
        float16x8_t acc2 = vdupq_n_f16(0);
        float16x8_t acc3 = vdupq_n_f16(0);

        int32_t col_cnt = rhs_cols;
        // Process in chunks of 8 elements
        int32_t col_loop = (rhs_cols + 7) / 8;
        for (int j = 0; j < col_loop; j++)
        {
            // Set up a predicate for the remaining elements.
            mve_pred16_t pred = vctp16q(col_cnt);
            col_cnt -= 8;

            // Load eight float16 elements from the lhs vector.
            float16x8_t lhs_val = vldrhq_z_f16(lhs_ptr, pred);
            // Load eight elements from each of the four rows.
            float16x8_t rhs_val0 = vldrhq_z_f16(rhs_ptr0, pred);
            float16x8_t rhs_val1 = vldrhq_z_f16(rhs_ptr1, pred);
            float16x8_t rhs_val2 = vldrhq_z_f16(rhs_ptr2, pred);
            float16x8_t rhs_val3 = vldrhq_z_f16(rhs_ptr3, pred);

            // Perform multiply–accumulate on each row.
            // (Using vfmaq_f16() to fuse the multiply and add.)
            acc0 = vfmaq_f16(acc0, lhs_val, rhs_val0);
            acc1 = vfmaq_f16(acc1, lhs_val, rhs_val1);
            acc2 = vfmaq_f16(acc2, lhs_val, rhs_val2);
            acc3 = vfmaq_f16(acc3, lhs_val, rhs_val3);

            lhs_ptr   += 8;
            rhs_ptr0  += 8;
            rhs_ptr1  += 8;
            rhs_ptr2  += 8;
            rhs_ptr3  += 8;
        }

        // Manually reduce each accumulator vector to a scalar.
        float16_t sum0 = vec_add_across_f16_mve(acc0);
        float16_t sum1 = vec_add_across_f16_mve(acc1);
        float16_t sum2 = vec_add_across_f16_mve(acc2);
        float16_t sum3 = vec_add_across_f16_mve(acc3);

        // Add bias if provided.
        if (bias)
        {
            sum0 += *bias++;
            sum1 += *bias++;
            sum2 += *bias++;
            sum3 += *bias++;
        }

        // Clamp results to the activation range.
        sum0 = MIN(MAX(sum0, activation_min), activation_max);
        sum1 = MIN(MAX(sum1, activation_min), activation_max);
        sum2 = MIN(MAX(sum2, activation_min), activation_max);
        sum3 = MIN(MAX(sum3, activation_min), activation_max);

        // Store the four results.
        *dst++ = sum0;
        *dst++ = sum1;
        *dst++ = sum2;
        *dst++ = sum3;
    }

    // Process any remaining rows that could not be unrolled by 4.
    for (int i = 0; i < rem_rows; i++)
    {
        const float16_t *rhs_ptr = rhs + ((row_unroll * unroll_factor) + i) * rhs_cols;
        const float16_t *lhs_ptr = lhs;
        float16x8_t acc = vdupq_n_f16(0);

        int32_t col_cnt = rhs_cols;
        int32_t col_loop = (rhs_cols + 7) / 8;
        for (int j = 0; j < col_loop; j++)
        {
            mve_pred16_t pred = vctp16q(col_cnt);
            col_cnt -= 8;

            float16x8_t lhs_val = vldrhq_z_f16(lhs_ptr, pred);
            float16x8_t rhs_val = vldrhq_z_f16(rhs_ptr, pred);

            acc = vfmaq_f16(acc, lhs_val, rhs_val);

            lhs_ptr += 8;
            rhs_ptr += 8;
        }
        float16_t sum = vec_add_across_f16_mve(acc);
        if (bias)
        {
            sum += *bias++;
        }
        sum = MIN(MAX(sum, activation_min), activation_max);
        *dst++ = sum;
    }

    return ARM_CMSIS_NN_SUCCESS;
}

#else
#if defined(ARM_FLOAT16_SUPPORTED)

/*
 * fp16 vector(lhs) by fp16 matrix (transposed) multiplication
 *
 * Refer header file for details.
 *
 */
arm_cmsis_nn_status arm_nn_vec_mat_mult_t_fp16(
    const float16_t *lhs,
    const float16_t *rhs,
    const float16_t *bias,
    float16_t       *dst,
    const int32_t    rhs_cols,
    const int32_t    rhs_rows,
    const float16_t  activation_min,
    const float16_t  activation_max
) {

    for (int i = 0; i < rhs_rows; i++)
    {
        const float16_t *lhs_ptr = lhs;
        const float16_t *rhs_ptr = rhs + i;
        float16_t sum = 0.0f;

        for (int j = 0; j < rhs_cols; j++)
        {
            sum += lhs_ptr[j] * rhs_ptr[j * rhs_rows];
        }

        if (bias)
        {
            sum += bias[i];
        }

        sum = MAX(sum, activation_min);
        sum = MIN(sum, activation_max);

        dst[i] = sum;
    }

    return ARM_CMSIS_NN_SUCCESS;
}

#endif /* defined(ARM_FLOAT16_SUPPORTED */
#endif /* defined(ARM_MATH_MVEF) && !defined(ARM_MATH_AUTOVECTORIZE) */

 /**
  * @} end of Doxygen group
  */
