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
 * Title:        arm_nn_vec_mat_mult_t_s16
 * Description:  s16 vector by s8 matrix (transposed) multiplication
 *
 * $Date:        19 June 2024
 * $Revision:    V.2.4.0
 *
 * Target :  Arm(R) M-Profile Architecture
 *
 * -------------------------------------------------------------------- */

#include "third_party/ns_cmsis_nn/Include/arm_nnsupportfunctions.h"
#include "third_party/ns_cmsis_nn/Include/Internal/arm_nn_compiler.h"
/**
 * @ingroup groupSupport
 */

/**
 * @addtogroup supportFC
 * @{
 */

/*
 * s16 vector(lhs) by s8 matrix (transposed) multiplication
 *
 * Refer header file for details.
 *
 */
#define ARM_MATH_DSP
#define ARM_MATH_MVEI
arm_cmsis_nn_status arm_nn_vec_mat_mult_t_s16_test(
    const int16_t *lhs,
    const int8_t *rhs,
    const int64_t *bias,
    int32_t *dst,
    const int32_t rhs_cols,
    const int32_t rhs_rows)
{

    int32_t rhs_cols_fast = 240 >> 3;

    if (rhs_cols > MAX_COL_COUNT)
    {
        rhs_cols_fast = MAX_COL_COUNT;
    }

    int32_t row_loop_cnt = rhs_rows / 4;
    const int8_t *rhs_ptr_0 = rhs;
    for (int32_t i_row_loop_count = 0; i_row_loop_count < row_loop_cnt; i_row_loop_count++)
    {

        int32_t result_0 = 0;
        int32_t result_1 = 0;
        int32_t result_2 = 0;
        int32_t result_3 = 0;

        const int16_t *lhs_ptr = lhs;
        
        int16x8_t in;
        int16x8_t w;
        for (int i=0; i< rhs_cols_fast; i++)
        {
            in = vldrhq_s16(lhs_ptr);
            lhs_ptr += 8;

            w = vldrbq_s16(rhs_ptr_0);
            rhs_ptr_0 += 8;
            result_0 = vmladavaq_s16(result_0, in, w);

            w = vldrbq_s16(rhs_ptr_0);
            rhs_ptr_0 += 8;
            result_1 = vmladavaq_s16(result_1, in, w);

            w = vldrbq_s16(rhs_ptr_0);
            rhs_ptr_0 += 8;
            result_2 = vmladavaq_s16(result_2, in, w);

            w = vldrbq_s16(rhs_ptr_0);
            rhs_ptr_0 += 8;
            result_3 = vmladavaq_s16(result_3, in, w);
        }



        // __ASM volatile(
        //     " .p2align 2                                 \n"
        //     "   wlstp.16        lr, %[cnt], 1f           \n"
        //     "   mov             %[out0], 0               \n"
        //     "   mov             %[out1], 0               \n"
        //     "   mov             %[out2], 0               \n"
        //     "   mov             %[out3], 0               \n"
        //     "   vldrh.s16       q0, [%[col]], #16        \n"
        //     "2:                                          \n"
        //     "   vldrb.s16        q1, [%[row0]], #8       \n"
        //     "   vmladava.s16     %[out0], q0, q1         \n"
        //     "   vldrb.s16        q2, [%[row1]], #8       \n"
        //     "   vmladava.s16     %[out1], q0, q2         \n"
        //     "   vldrb.s16       q3, [%[row2]], #8       \n"
        //     "   vmladava.s16     %[out2], q0, q3         \n"
        //     "   vldrb.s16        q4, [%[row3]], #8       \n"
        //     "   vmladava.s16     %[out3], q0, q4         \n"
        //     "   vldrh.s16        q0, [%[col]], #16       \n"
        //     "   letp            lr, 2b                   \n"
        //     "1:                                          \n"
        // :
        //     [col] "+r"(lhs_ptr),
        //     [row0] "+r"(rhs_ptr_0),
        //     [row1] "+r"(rhs_ptr_1),
        //     [row2] "+r"(rhs_ptr_2),
        //     [row3] "+r"(rhs_ptr_3),
        //     [out0] "=Te"(result_0),
        //     [out1] "=Te"(result_1),
        //     [out2] "=Te"(result_2),
        //     [out3] "=Te"(result_3)
        // :
        //     [cnt] "r"(rhs_cols_fast)
        // :
        //     "q0", "q1", "q2", "q3", "q4", "memory", "r14"
        // );
        *dst++ = result_0;
        *dst++ = result_1;
        *dst++ = result_2;
        *dst++ = result_3;
    }



    return ARM_CMSIS_NN_SUCCESS;
}

/**
 * @} end of Doxygen group
 */
