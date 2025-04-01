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
 * Title:        arm_nn_vec_mat_mult_t_per_ch_s16
 * Description:  s16 vector by s8 matrix (transposed) multiplication
 *
 * $Date:        3 Feb 2025
 * $Revision:    V.1.0.0
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

/*
 * s16 vector(lhs) by s8 matrix (transposed) multiplication and per channel quant output
 *
 * Refer header file for details.
 *
 */
arm_cmsis_nn_status arm_nn_vec_mat_mult_t_per_ch_s16(
    const int16_t *lhs,
    const int8_t *rhs,
    const int64_t *bias,
    int16_t *dst,
    const int32_t *dst_multiplier,
    const int32_t *dst_shift,
    const int32_t rhs_cols,
    const int32_t rhs_rows,
    const int32_t activation_min,
    const int32_t activation_max
)
{
#if defined(ARM_MATH_DSP)

    int32_t rhs_cols_fast = rhs_cols;

    if (rhs_cols > MAX_COL_COUNT)
    {
        rhs_cols_fast = MAX_COL_COUNT;
    }

    #if defined(ARM_MATH_MVEI)
    int32_t row_loop_cnt = rhs_rows / 4;

    for (int32_t i_row_loop_count = 0; i_row_loop_count < row_loop_cnt; i_row_loop_count++)
    {

        int32_t result_0 = 0;
        int32_t result_1 = 0;
        int32_t result_2 = 0;
        int32_t result_3 = 0;

        register const int16_t *lhs_ptr __ASM("r4") = lhs;
        register const int8_t *rhs_ptr_0 __ASM("r0") = rhs;
        register const int8_t *rhs_ptr_1 __ASM("r1") = rhs_ptr_0 + rhs_cols;
        register const int8_t *rhs_ptr_2 __ASM("r2") = rhs_ptr_1 + rhs_cols;
        register const int8_t *rhs_ptr_3 __ASM("r3") = rhs_ptr_2 + rhs_cols;

        __ASM volatile(
            " .p2align 2                                 \n"
            "   wlstp.16        lr, %[cnt], 1f           \n"
            "   mov             %[out0], 0               \n"
            "   mov             %[out1], 0               \n"
            "   mov             %[out2], 0               \n"
            "   mov             %[out3], 0               \n"
            "2:                                          \n"
            "   vldrh.s16       q0, [%[col]], #16        \n"
            "   vldrb.s16        q1, [%[row0]], #8       \n"
            "   vmladava.s16     %[out0], q0, q1         \n"
            "   vldrb.s16        q2, [%[row1]], #8       \n"
            "   vmladava.s16     %[out1], q0, q2         \n"
            "   vldrb.s16       q3, [%[row2]], #8       \n"
            "   vmladava.s16     %[out2], q0, q3         \n"
            "   vldrb.s16        q4, [%[row3]], #8       \n"
            "   vmladava.s16     %[out3], q0, q4         \n"
            "   letp            lr, 2b                   \n"
            "1:                                          \n"
        :
            [col] "+r"(lhs_ptr),
            [row0] "+r"(rhs_ptr_0),
            [row1] "+r"(rhs_ptr_1),
            [row2] "+r"(rhs_ptr_2),
            [row3] "+r"(rhs_ptr_3),
            [out0] "=Te"(result_0),
            [out1] "=Te"(result_1),
            [out2] "=Te"(result_2),
            [out3] "=Te"(result_3)
        :
            [cnt] "r"(rhs_cols_fast)
        :
            "q0", "q1", "q2", "q3", "q4", "memory", "r14"
        );

        int64_t result_64_0 = result_0;
        int64_t result_64_1 = result_1;
        int64_t result_64_2 = result_2;
        int64_t result_64_3 = result_3;

        if (rhs_cols > MAX_COL_COUNT)
        {

            int rem_cols = rhs_cols - MAX_COL_COUNT;
            int32_t col_loop_cnt = (rem_cols + 7) / 8;

            for (int i_col_loop_cnt = 0; i_col_loop_cnt < col_loop_cnt; i_col_loop_cnt++)
            {
                mve_pred16_t pred = vctp16q(rem_cols);
                rem_cols -= 8;

                int16x8_t lhs_input = vldrhq_z_s16(lhs_ptr, pred);

                int16x8_t rhs_input_0 = vldrbq_z_s16(rhs_ptr_0, pred);
                int16x8_t rhs_input_1 = vldrbq_z_s16(rhs_ptr_1, pred);
                int16x8_t rhs_input_2 = vldrbq_z_s16(rhs_ptr_2, pred);
                int16x8_t rhs_input_3 = vldrbq_z_s16(rhs_ptr_3, pred);

                result_64_0 = vmlaldavaq_s16(result_64_0, lhs_input, rhs_input_0);
                result_64_1 = vmlaldavaq_s16(result_64_1, lhs_input, rhs_input_1);
                result_64_2 = vmlaldavaq_s16(result_64_2, lhs_input, rhs_input_2);
                result_64_3 = vmlaldavaq_s16(result_64_3, lhs_input, rhs_input_3);

                lhs_ptr += 8;

                rhs_ptr_0 += 8;
                rhs_ptr_1 += 8;
                rhs_ptr_2 += 8;
                rhs_ptr_3 += 8;
            }
        }

        if (bias)
        {
            result_64_0 += *bias++;
            result_64_1 += *bias++;
            result_64_2 += *bias++;
            result_64_3 += *bias++;
        }

        int32_t tmp;
        tmp = arm_nn_requantize_s64(result_64_0, *dst_multiplier++, *dst_shift++);
        tmp = MAX(tmp, activation_min);
        tmp = MIN(tmp, activation_max);
        *dst++ = (int16_t)tmp;

        tmp = 0;
        tmp = arm_nn_requantize_s64(result_64_1, *dst_multiplier++, *dst_shift++);
        tmp = MAX(tmp, activation_min);
        tmp = MIN(tmp, activation_max);
        *dst++ = (int16_t)tmp;

        tmp = 0;
        tmp = arm_nn_requantize_s64(result_64_2, *dst_multiplier++, *dst_shift++);
        tmp = MAX(tmp, activation_min);
        tmp = MIN(tmp, activation_max);
        *dst++ = (int16_t)tmp;

        tmp = 0;
        tmp = arm_nn_requantize_s64(result_64_3, *dst_multiplier++, *dst_shift++);
        tmp = MAX(tmp, activation_min);
        tmp = MIN(tmp, activation_max);
        *dst++ = (int16_t)tmp;

        rhs += 4 * rhs_cols;
    }

    for (int8_t rows_left = rhs_rows & 0x3; rows_left > 0; rows_left--)
    {
        int32_t result = 0;

        register const int16_t *lhs_ptr __ASM("r4") = lhs;
        register const int8_t *rhs_ptr __ASM("r0") = rhs;

        __ASM volatile(
            " .p2align 2                                 \n"
            "   wlstp.16        lr, %[cnt], 1f           \n"
            "   mov             %[out0], 0               \n"
            "2:                                          \n"
            "   vldrh.s16       q0, [%[col]], #16        \n"
            "   vldrb.s16        q1, [%[row0]], #8       \n"
            "   vmladava.s16     %[out0], q0, q1         \n"
            "   letp            lr, 2b                   \n"
            "1:                                          \n"
        :
            [col] "+r"(lhs_ptr),
            [row0] "+r"(rhs_ptr),
            [out0] "=Te"(result)
        :
            [cnt] "r"(rhs_cols_fast)
        :
            "q0", "q1", "q2", "q3", "q4", "memory", "r14"
        );

        int64_t result_64 = result;

        if (bias)
        {
            result_64 += *bias++;
        }

        if (rhs_cols > MAX_COL_COUNT)
        {

            int rem_cols = rhs_cols - MAX_COL_COUNT;
            int32_t col_loop_cnt = (rem_cols + 7) / 8;

            for (int i_col_loop_cnt = 0; i_col_loop_cnt < col_loop_cnt; i_col_loop_cnt++)
            {
                mve_pred16_t pred = vctp16q(rem_cols);
                rem_cols -= 8;

                int16x8_t lhs_input = vldrhq_z_s16(lhs_ptr, pred);

                int16x8_t rhs_input_0 = vldrbq_z_s16(rhs_ptr, pred);

                result_64 = vmlaldavaq_s16(result_64, lhs_input, rhs_input_0);

                lhs_ptr += 8;
                rhs_ptr += 8;
            }
        }

        int32_t tmp = 0;
        tmp = arm_nn_requantize_s64(result_64, *dst_multiplier++, *dst_shift++);
        tmp = MAX(tmp, activation_min);
        tmp = MIN(tmp, activation_max);
        *dst++ = (int16_t)tmp;
        rhs += rhs_cols;
    }

    #else // ARM_MATH_MVEI

    const int32_t row_loop_cnt = rhs_rows / 2;

    for (int32_t i = 0; i < row_loop_cnt; i++)
    {

        int64_t acc_64_0 = 0;
        int64_t acc_64_1 = 0;
        int32_t acc_0 = 0;
        int32_t acc_1 = 0;

        const int32_t col_loop_cnt = rhs_cols_fast / 4;

        const int16_t *lhs_vec = lhs;
        const int8_t *rhs_0 = rhs;
        const int8_t *rhs_1 = rhs + rhs_cols;
        rhs += 2 * rhs_cols;

        for (int j = col_loop_cnt; j != 0; j--)
        {
            int32_t ker_0, ker_1, vec_part_0, vec_part_1;

            vec_part_0 = arm_nn_read_q15x2_ia(&lhs_vec);
            vec_part_1 = arm_nn_read_q15x2_ia(&lhs_vec);

            rhs_0 = read_and_pad(rhs_0, &ker_0, &ker_1);

            acc_0 = SMLAD(ker_0, vec_part_0, acc_0);
            acc_0 = SMLAD(ker_1, vec_part_1, acc_0);

            rhs_1 = read_and_pad(rhs_1, &ker_0, &ker_1);

            acc_1 = SMLAD(ker_0, vec_part_0, acc_1);
            acc_1 = SMLAD(ker_1, vec_part_1, acc_1);
        }

        acc_64_0 += acc_0;
        acc_64_1 += acc_1;

        for (int k = col_loop_cnt * 4; k < rhs_cols; k++)
        {
            const int32_t lhs_temp = (*lhs_vec);
            lhs_vec++;
            acc_64_0 += lhs_temp * (*rhs_0);
            rhs_0++;
            acc_64_1 += lhs_temp * (*rhs_1);
            rhs_1++;
        }

        if (bias)
        {
            acc_64_0 += *bias++;
            acc_64_1 += *bias++;
        }
        int32_t tmp;

        tmp = arm_nn_requantize_s64(acc_64_0, *dst_multiplier++, *dst_shift++);
        tmp = MAX(tmp, activation_min);
        tmp = MIN(tmp, activation_max);
        *dst++ = (int16_t)tmp;

        tmp = arm_nn_requantize_s64(acc_64_1, *dst_multiplier++, *dst_shift++);
        tmp = MAX(tmp, activation_min);
        tmp = MIN(tmp, activation_max);
        *dst++ = (int16_t)tmp;
    }

    if (rhs_rows & 0x1)
    {
        int64_t acc_64_0 = 0;
        int32_t acc_0 = 0;
        const int32_t col_loop_cnt = rhs_cols_fast / 4;

        const int16_t *lhs_vec = lhs;
        const int8_t *rhs_0 = rhs;

        for (int i = col_loop_cnt; i != 0; i--)
        {
            int32_t ker_0, ker_1, vec;
            rhs_0 = read_and_pad(rhs_0, &ker_0, &ker_1);

            vec = arm_nn_read_q15x2_ia(&lhs_vec);
            acc_0 = SMLAD(ker_0, vec, acc_0);

            vec = arm_nn_read_q15x2_ia(&lhs_vec);
            acc_0 = SMLAD(ker_1, vec, acc_0);
        }

        acc_64_0 += acc_0;

        for (int j = col_loop_cnt * 4; j < rhs_cols; j++)
        {
            const int32_t lhs_temp = (*lhs_vec);
            lhs_vec++;
            acc_64_0 += lhs_temp * (*rhs_0);
            rhs_0++;
        }

        if (bias)
        {
            acc_64_0 += *bias++;
        }
        int32_t tmp;
        tmp = arm_nn_requantize_s64(acc_64_0, *dst_multiplier++, *dst_shift++);
        tmp = MAX(tmp, activation_min);
        tmp = MIN(tmp, activation_max);
        *dst++ = (int16_t)tmp;
    }

    #endif // ARM_MATH_MVEI
#else      // ARM_MATH_DSP
    for (int i_row_loop_cnt = 0; i_row_loop_cnt < rhs_rows; i_row_loop_cnt++)
    {
        const int16_t *lhs_ptr = lhs;
        const int8_t *rhs_ptr_0 = &rhs[0];

        int64_t result = 0;

        for (int32_t rhs_cols_idx = 0; rhs_cols_idx < rhs_cols; ++rhs_cols_idx)
        {
            const int64_t rhs_value0 = (int8_t)*rhs_ptr_0;
            const int64_t lhs_value = *lhs_ptr;

            result += lhs_value * rhs_value0;

            ++rhs_ptr_0;
            ++lhs_ptr;
        }

        if (bias)
        {
            result += *bias++;
        }
        // Quantize down
        result = arm_nn_requantize_s64(result, *dst_multiplier++, *dst_shift++);

        // Clamp the result
        result = MAX(result, activation_min);
        result = MIN(result, activation_max);

        *dst++ = (int16_t)result;
        rhs += rhs_cols;
    }
#endif     // ARM_MATH_DSP

    return ARM_CMSIS_NN_SUCCESS;
}

/**
 * @} end of Doxygen group
 */
