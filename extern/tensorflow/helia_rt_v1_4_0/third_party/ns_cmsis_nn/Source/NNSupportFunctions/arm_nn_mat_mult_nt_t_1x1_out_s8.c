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
 * Title:        arm_nn_mat_mult_nt_t_1x1_out_s8.c
 * Description:  Matrix multiplication support function with the right-hand-side (rhs) matrix transposed, optimized for fetching lhs and keeping rhs stationary
 *
 * $Date:        04 January 2024
 * $Revision:    V.3.0.0
 *
 * Target :  Arm(R) M-Profile Architecture
 *
 * -------------------------------------------------------------------- */

#include "arm_nnsupportfunctions.h"

/**
 * @ingroup groupSupport
 */

/**
 * @addtogroup supportConvolution
 * @{
 */

/*
 * s8 matrix multiplication with the right-hand-side matrix transposed
 *
 * Refer header file for details.
 *
 */


arm_cmsis_nn_status arm_nn_mat_mult_nt_t_1x1_out_s8(const int32_t *weight_sum_buf,
                                            const int8_t *lhs,
                                            const int8_t *rhs,
                                            const int32_t *bias,
                                            int8_t *dst,
                                            const int32_t *dst_multipliers,
                                            const int32_t *dst_shifts,
                                            const int32_t lhs_rows,
                                            const int32_t rhs_rows,
                                            const int32_t rhs_cols,
                                            const int32_t lhs_offset,
                                            const int32_t dst_offset,
                                            const int32_t activation_min,
                                            const int32_t activation_max,
                                            const int32_t row_address_offset,
                                            const int32_t lhs_cols_offset)
{

#if defined(ARM_MATH_MVEI)
    (void)bias;
    (void)row_address_offset;
    (void)lhs_cols_offset;
    (void)lhs_offset;
    //lhs rows is 1
    if (lhs_rows != 1) {
        return ARM_CMSIS_NN_ARG_ERROR;
    }

    int out_idx;
    const int num_elems = 4;
    for (out_idx = 0; out_idx < rhs_rows; out_idx+=num_elems)
    {
        int32_t acc_n0 = 0;
        int32_t acc_n1 = 0;
        int32_t acc_n2 = 0;
        int32_t acc_n3 = 0;
        const int8_t *row_base = lhs;
        const int8_t *col_base = rhs + out_idx * rhs_cols;
        const int8_t *ip_col_1 = col_base + rhs_cols;
        const int8_t *ip_col_2 = col_base + (2 * rhs_cols);
        const int8_t *ip_col_3 = col_base + (3 * rhs_cols);

#if defined(ARM_MATH_AUTOVECTORIZE)
        for (int j = 0; j < rhs_cols; j++)
        {
            int8_t row = lhs[j];

            acc_n0 += row *col_base[j];
            acc_n1 += row *ip_col_1[j];
            acc_n2 += row *ip_col_2[j];
            acc_n3 += row *ip_col_3[j];
        }
#else
        // Note: If operand initialization is moved around, use '&' constraint to
        // specify earlyclobber operands.
        //
        __ASM volatile(" .p2align 2                             \n"
                       "   wlstp.8         lr, %[cnt], 1f       \n"
                       "   mov             %[out0], 0           \n"
                       "   mov             %[out1], 0           \n"
                       "   mov             %[out2], 0           \n"
                       "   mov             %[out3], 0           \n"
                       "   vldrb.8         q0, [%[row]], #16    \n"
                       "2:                                      \n"
                       "   vldrb.8         q1, [%[col0]], #16   \n"
                       "   vmladava.s8    %[out0], q0, q1       \n"
                       "   vldrb.8         q2, [%[col1]], #16   \n"
                       "   vmladava.s8     %[out1], q0, q2      \n"
                       "   vldrb.8         q3, [%[col2]], #16   \n"
                       "   vmladava.s8     %[out2], q0, q3      \n"
                       "   vldrb.8         q3, [%[col3]], #16   \n"
                       "   vmladava.s8     %[out3], q0, q3      \n"
                       "   vldrb.8         q0, [%[row]], #16    \n"
                       "   letp            lr, 2b               \n"
                       "1:                                      \n"
                       : [row] "+r"(row_base),
                         [col0] "+r"(col_base),
                         [col1] "+r"(ip_col_1),
                         [col2] "+r"(ip_col_2),
                         [col3] "+r"(ip_col_3),
                         [out0] "=Te"(acc_n0),
                         [out1] "=Te"(acc_n1),
                         [out2] "=Te"(acc_n2),
                         [out3] "=Te"(acc_n3)
                       : [cnt] "r"(rhs_cols)
                       : "q0", "q1", "q2", "q3", "memory", "r14");
#endif
        int32x4_t res = {acc_n0, acc_n1, acc_n2, acc_n3};
        int32x4_t sum_tmps = vldrwq_s32(weight_sum_buf + out_idx);
        res = vaddq_s32(res, sum_tmps);
        int32x4_t vec_dst_mults = vldrwq_s32(dst_multipliers + out_idx);
        int32x4_t vec_dst_shfts = vldrwq_s32(dst_shifts + out_idx);
        res = arm_requantize_mve_32x4(res, vec_dst_mults, vec_dst_shfts);

        res = vaddq_n_s32(res, dst_offset);

        res = vmaxq_s32(res, vdupq_n_s32(activation_min));
        res = vminq_s32(res, vdupq_n_s32(activation_max));
        vstrbq_s32(dst, res);
        dst += num_elems;
    }

    const int32_t *multipliers = dst_multipliers;
    const int32_t *shifts = dst_shifts;
    //finish last rows that aren't multiple of 4
    out_idx -= num_elems;
    for (; out_idx < rhs_rows; out_idx++)
    {
        int32_t acc_n0 = 0;
        const int8_t *lhs_vec = lhs;
        const int8_t *col_base = rhs + out_idx * rhs_cols;

#if defined(ARM_MATH_AUTOVECTORIZE)
        for (int j = 0; j < rhs_cols; j++)
        {
            int32_t col = col_base[j];
            acc_n0 += lhs_vec[j] * col;
        }
#else
        __ASM volatile(" .p2align 2                             \n"
                       "   wlstp.8         lr, %[cnt], 1f       \n"
                       "   mov             %[out0], 0            \n"
                       "   vldrb.8         q0, [%[col]], #16    \n"
                       "2:                                      \n"
                       "   vldrb.8         q1, [%[row0]], #16   \n"
                       "   vmladava.s8    %[out0], q0, q1       \n"
                       "   vldrb.8         q0, [%[col]], #16    \n"
                       "   letp            lr, 2b               \n"
                       "1:                                      \n"
                       : [col] "+r"(col_base), [row0] "+r"(lhs_vec), [out0] "=Te"(acc_n0)
                       : [cnt] "r"(rhs_cols)
                       : "q0", "q1", "memory", "r14");
#endif
        int32_t sum_tmp = weight_sum_buf[out_idx];
        acc_n0 += sum_tmp;
        acc_n0 = arm_nn_requantize(acc_n0, multipliers[out_idx], shifts[out_idx]);
        acc_n0 += dst_offset;
        acc_n0 = MAX(acc_n0, activation_min);
        acc_n0 = MIN(acc_n0, activation_max);
        *dst++ = (int8_t)acc_n0;
    }
#else
    (void)weight_sum_buf;
    (void)lhs;
    (void)rhs;
    (void)bias;
    (void)dst;
    (void)dst_multipliers;
    (void)dst_shifts;
    (void)lhs_rows;
    (void)rhs_rows;
    (void)rhs_cols;
    (void)lhs_offset;
    (void)dst_offset;
    (void)activation_min;
    (void)activation_max;
    (void)row_address_offset;
    (void)lhs_cols_offset;
    return ARM_CMSIS_NN_NO_IMPL_ERROR;
#endif //defined(ARM_MATH_MVEI)
    return ARM_CMSIS_NN_SUCCESS;
}

/**
 * @} end of Doxygen group
 */
