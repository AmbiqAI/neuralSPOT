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
 * Title:        arm_nn_vec_mat_mult_t_per_ch_s8
 * Description:  s8 vector by matrix (transposed) multiplication
 *
 * $Date:        4 Mar 2025
 * $Revision:    V.1.2.0
 *
 * Target :  Arm(R) M-Profile Architecture
 *
 * -------------------------------------------------------------------- */

 #include "arm_nnsupportfunctions.h"

 /**
  * @ingroup groupSupport
  */

 /**
  * @defgroup supportFC Fully Connected
  *
  * Support functions for Fully Connected
  *
  */

 /**
  * @addtogroup supportFC
  * @{
  */

 /*
 * s8 vector(lhs) by matrix (transposed) multiplication and per channel quant output
 *
 * Refer header file for details.
 *
 */
 #if !defined(ARM_MATH_MVEI) && defined(ARM_MATH_DSP) && !defined(__ARMCC_VERSION) && !defined(__ICCARM__)
     #pragma GCC optimize("unroll-loops")
 #endif
 arm_cmsis_nn_status arm_nn_vec_mat_mult_t_per_ch_s8(const int8_t *lhs,
                                                     const int8_t *rhs,
                                                     const int32_t *kernel_sum,
                                                     const int32_t *bias,
                                                     int8_t *dst,
                                                     const int32_t lhs_offset,
                                                     const int32_t dst_offset,
                                                     const int32_t *dst_multiplier,
                                                     const int32_t *dst_shift,
                                                     const int32_t rhs_cols,
                                                     const int32_t rhs_rows,
                                                     const int32_t activation_min,
                                                     const int32_t activation_max,
                                                     const int32_t address_offset,
                                                     const int32_t rhs_offset)
 {
     if (rhs_offset)
     {
 #if defined(ARM_MATH_MVEI)
         (void)bias;
         (void)lhs_offset;

         const int32_t row_loop_cnt = rhs_rows / 4;
         const uint32x4_t address_offset_array = {0, address_offset, address_offset * 2, address_offset * 3};
         const uint32_t next_dst_offset = 4 * address_offset;

         int32_t lhs_sum = 0;

         for (int i_row_loop_cnt = 0; i_row_loop_cnt < row_loop_cnt; i_row_loop_cnt++)
         {

             int32_t acc_0 = 0;
             int32_t acc_1 = 0;
             int32_t acc_2 = 0;
             int32_t acc_3 = 0;

             const int8_t *lhs_vec = lhs;
             const int8_t *rhs_0_ptr = rhs;
             const int8_t *rhs_1_ptr = rhs + rhs_cols;
             const int8_t *rhs_2_ptr = rhs_1_ptr + rhs_cols;
             const int8_t *rhs_3_ptr = rhs_2_ptr + rhs_cols;
             rhs = rhs_3_ptr + rhs_cols;

             __ASM volatile(
                 " .p2align 2                                 \n"
                 "   wlstp.8         lr, %[cnt], 1f           \n"
                 "   mov             %[out0], 0               \n"
                 "   mov             %[out1], 0               \n"
                 "   mov             %[out2], 0               \n"
                 "   mov             %[out3], 0               \n"
                 "   mov             %[sum], 0                \n"
                 "   vldrb.8         q0, [%[col]], #16        \n"
                 "2:                                          \n"
                 "   vaddva.s8      %[sum], q0                \n"
                 "   vldrb.8         q1, [%[row0]], #16       \n"
                 "   vmladava.s8     %[out0], q0, q1          \n"
                 "   vldrb.8         q2, [%[row1]], #16       \n"
                 "   vmladava.s8     %[out1], q0, q2          \n"
                 "   vldrb.8         q3, [%[row2]], #16       \n"
                 "   vmladava.s8     %[out2], q0, q3          \n"
                 "   vldrb.8         q4, [%[row3]], #16       \n"
                 "   vmladava.s8     %[out3], q0, q4          \n"
                 "   vldrb.8         q0, [%[col]], #16        \n"
                 "   letp            lr, 2b                   \n"
                 "1:                                          \n"
             :
                 [col] "+r"(lhs_vec),
                 [sum] "=Te"(lhs_sum),
                 [row0] "+r"(rhs_0_ptr),
                 [row1] "+r"(rhs_1_ptr),
                 [row2] "+r"(rhs_2_ptr),
                 [row3] "+r"(rhs_3_ptr),
                 [out0] "=Te"(acc_0),
                 [out1] "=Te"(acc_1),
                 [out2] "=Te"(acc_2),
                 [out3] "=Te"(acc_3)
             :
                 [cnt] "r"(rhs_cols)
             :
                 "q0", "q1", "q2", "q3", "q4", "memory", "r14"
             );
             lhs_vec -= 8;

             int32x4_t acc = {acc_0, acc_1, acc_2, acc_3};

             acc = vaddq_s32(acc, vldrwq_s32(kernel_sum));
             kernel_sum += 4;

             acc += vdupq_n_s32(rhs_offset) * vdupq_n_s32(lhs_sum);

             acc = arm_requantize_mve_32x4(acc, vldrwq_s32(dst_multiplier), vldrwq_s32(dst_shift));
             dst_multiplier += 4;
             dst_shift += 4;

             acc = vaddq_s32(acc, vdupq_n_s32(dst_offset));
             acc = vmaxq_s32(acc, vdupq_n_s32(activation_min));
             acc = vminq_s32(acc, vdupq_n_s32(activation_max));

             vstrbq_scatter_offset_s32(dst, address_offset_array, acc);

             dst += next_dst_offset;
         }

         const int loop_cnt = rhs_rows % 4;
         for (int32_t i_row_loop_cnt = 0; i_row_loop_cnt < loop_cnt; i_row_loop_cnt++)
         {
             int32_t acc_0 = 0;
             const int8_t *lhs_vec = lhs;
             const int8_t *rhs_ptr = rhs;
             uint32_t col_cnt = (uint32_t)rhs_cols;
             rhs += rhs_cols;

             lhs_sum = 0;

             __ASM volatile(
                 " .p2align 2                                 \n"
                 "   wlstp.8         lr, %[cnt], 1f           \n"
                 "   mov             %[out0], 0               \n"
                 "   mov             %[sum], 0                \n"
                 "   vldrb.8         q0, [%[col]], #16        \n"
                 "2:                                          \n"
                 "   vaddva.s8      %[sum], q0                \n"
                 "   vldrb.8         q1, [%[row0]], #16       \n"
                 "   vmladava.s8     %[out0], q0, q1          \n"
                 "   vldrb.8         q0, [%[col]], #16        \n"
                 "   letp            lr, 2b                   \n"
                 "1:                                          \n"
             :
                 [col] "+r"(lhs_vec),
                 [sum] "=Te"(lhs_sum),
                 [row0] "+r"(rhs_ptr),
                 [out0] "=Te"(acc_0)
             :
                 [cnt] "r"(col_cnt)
             :
             "q0", "q1", "q2", "q3", "q4", "memory", "r14"
             );
             lhs_vec -= 8;

             acc_0 += *kernel_sum++;
             acc_0 += lhs_sum * rhs_offset;
             acc_0 = arm_nn_requantize(acc_0, *dst_multiplier++, *dst_shift++);
             acc_0 += dst_offset;

             // Clamp the result
             acc_0 = MAX(acc_0, activation_min);
             *dst = MIN(acc_0, activation_max);
             dst += address_offset;
         }

 #elif defined(ARM_MATH_DSP)
         (void)kernel_sum;

         const int32_t row_loop_cnt = rhs_rows / 2;
         const int16_t lhs_offset_s16 = (int16_t)lhs_offset;
         const uint32_t lhs_offset_s16x2 = PKHBT(lhs_offset_s16, lhs_offset_s16, 16);

         const int16_t rhs_offset_s16 = (int16_t)rhs_offset;
         const uint32_t rhs_offset_s16x2 = PKHBT(rhs_offset_s16, rhs_offset_s16, 16);

         for (int32_t i = 0; i < row_loop_cnt; i++)
         {
             int32_t acc_0 = 0;
             int32_t acc_1 = 0;
             if (bias)
             {
                 acc_0 = *bias++;
                 acc_1 = *bias++;
             }

             const int32_t col_loop_cnt = rhs_cols / 4;

             const int8_t *lhs_vec = lhs;
             const int8_t *rhs_0_ptr = rhs;
             const int8_t *rhs_1_ptr = rhs + rhs_cols;
             rhs += 2 * rhs_cols;

             for (int32_t j = col_loop_cnt; j != 0; j--)
             {
                 int32_t vec_0 = arm_nn_read_s8x4_ia(&lhs_vec);
                 int32_t vec_1 = SXTAB16_RORn(lhs_offset_s16x2, (uint32_t)vec_0, 8);

                 vec_0 = SXTAB16(lhs_offset_s16x2, vec_0);

                 int32_t ker_0 = arm_nn_read_s8x4_ia(&rhs_0_ptr);
                 int32_t ker_1 = SXTAB16_RORn(rhs_offset_s16x2, (uint32_t)ker_0, 8);
                 ker_0 = SXTAB16(rhs_offset_s16x2, ker_0);

                 acc_0 = SMLAD(ker_1, vec_1, acc_0);
                 acc_0 = SMLAD(ker_0, vec_0, acc_0);

                 ker_0 = arm_nn_read_s8x4_ia(&rhs_1_ptr);
                 ker_1 = SXTAB16_RORn(rhs_offset_s16x2, (uint32_t)ker_0, 8);
                 ker_0 = SXTAB16(rhs_offset_s16x2, ker_0);

                 acc_1 = SMLAD(ker_1, vec_1, acc_1);
                 acc_1 = SMLAD(ker_0, vec_0, acc_1);
             }

             for (int32_t k = col_loop_cnt * 4; k < rhs_cols; k++)
             {
                 const int32_t lhs_temp = (*lhs_vec + lhs_offset);
                 lhs_vec++;
                 acc_0 += lhs_temp * (*rhs_0_ptr + rhs_offset);
                 rhs_0_ptr++;
                 acc_1 += lhs_temp * (*rhs_1_ptr + rhs_offset);
                 rhs_1_ptr++;
             }

             acc_0 = arm_nn_requantize(acc_0, *dst_multiplier++, *dst_shift++);
             acc_1 = arm_nn_requantize(acc_1, *dst_multiplier++, *dst_shift++);

             // Add offset
             acc_0 += dst_offset;
             acc_1 += dst_offset;
             // Clamp the result
             acc_0 = MAX(acc_0, activation_min);
             acc_0 = MIN(acc_0, activation_max);
             acc_1 = MAX(acc_1, activation_min);
             acc_1 = MIN(acc_1, activation_max);
             *dst = (int8_t)acc_0;
             *(dst + address_offset) = (int8_t)acc_1;
             dst += 2 * address_offset;
         }

         if (rhs_rows & 0x1)
         {
             int32_t acc_0 = 0;
             if (bias)
             {
                 acc_0 = *bias++;
             }
             const int32_t col_loop_cnt = rhs_cols / 4;

             const int8_t *lhs_vec = lhs;
             const int8_t *rhs_ptr = rhs;

             for (int32_t i = col_loop_cnt; i != 0; i--)
             {
                 int32_t vec_0 = arm_nn_read_s8x4_ia(&lhs_vec);
                 int32_t vec_1 = SXTAB16_RORn(lhs_offset_s16x2, (uint32_t)vec_0, 8);
                 vec_0 = SXTAB16(lhs_offset_s16x2, vec_0);

                 int32_t ker_0 = arm_nn_read_s8x4_ia(&rhs_ptr);
                 int32_t ker_1 = SXTAB16_RORn(rhs_offset_s16x2, (uint32_t)ker_0, 8);
                 ker_0 = SXTAB16(rhs_offset_s16x2, ker_0);

                 acc_0 = SMLAD(ker_1, vec_1, acc_0);
                 acc_0 = SMLAD(ker_0, vec_0, acc_0);
             }

             for (int32_t j = col_loop_cnt * 4; j < rhs_cols; j++)
             {
                 const int32_t lhs_temp = (*lhs_vec + lhs_offset);
                 lhs_vec++;
                 acc_0 += lhs_temp * (*rhs_ptr + rhs_offset);
                 rhs_ptr++;
             }

             acc_0 = arm_nn_requantize(acc_0, *dst_multiplier++, *dst_shift++);

             // Add offset
             acc_0 += dst_offset;
             // Clamp the result
             acc_0 = MAX(acc_0, activation_min);
             acc_0 = MIN(acc_0, activation_max);
             *dst = (int8_t)acc_0;
             dst += address_offset;
         }

 #else
         (void)kernel_sum;

         const int32_t row_loop_cnt = rhs_rows / 3;

         for (int32_t i_row_loop_cnt = 0; i_row_loop_cnt < row_loop_cnt; i_row_loop_cnt++)
         {
             const int8_t *lhs_ptr = lhs;
             const int8_t *rhs_ptr_0 = &rhs[0];
             const int8_t *rhs_ptr_1 = &rhs[rhs_cols];
             const int8_t *rhs_ptr_2 = &rhs[rhs_cols * 2];

             int32_t res00 = 0;
             int32_t res01 = 0;
             int32_t res02 = 0;
             if (bias)
             {
                 res00 = *bias++;
                 res01 = *bias++;
                 res02 = *bias++;
             }
             for (int32_t rhs_cols_idx = 0; rhs_cols_idx < rhs_cols; ++rhs_cols_idx)
             {
                 const int32_t rhs_value0 = (int8_t)*rhs_ptr_0 + rhs_offset;
                 const int32_t rhs_value1 = (int8_t)*rhs_ptr_1 + rhs_offset;
                 const int32_t rhs_value2 = (int8_t)*rhs_ptr_2 + rhs_offset;
                 const int32_t lhs_value = (int8_t)*lhs_ptr + lhs_offset;

                 res00 += lhs_value * rhs_value0;
                 res01 += lhs_value * rhs_value1;
                 res02 += lhs_value * rhs_value2;

                 ++rhs_ptr_0;
                 ++rhs_ptr_1;
                 ++rhs_ptr_2;
                 ++lhs_ptr;
             }

             // Quantize down
             res00 = arm_nn_requantize(res00, *dst_multiplier++, *dst_shift++);
             res01 = arm_nn_requantize(res01, *dst_multiplier++, *dst_shift++);
             res02 = arm_nn_requantize(res02, *dst_multiplier++, *dst_shift++);

             // Add offset
             res00 += dst_offset;
             res01 += dst_offset;
             res02 += dst_offset;

             // Clamp the result
             res00 = MAX(res00, activation_min);
             res00 = MIN(res00, activation_max);
             res01 = MAX(res01, activation_min);
             res01 = MIN(res01, activation_max);
             res02 = MAX(res02, activation_min);
             res02 = MIN(res02, activation_max);

             *dst = (int8_t)res00;
             *(dst + address_offset) = (int8_t)res01;
             *(dst + 2 * address_offset) = (int8_t)res02;
             dst += 3 * address_offset;

             rhs += 3 * rhs_cols;
         }

         const int loop_cnt = rhs_rows % 3;

         for (int32_t i_loop_cnt = 0; i_loop_cnt < loop_cnt; i_loop_cnt++)
         {
             const int8_t *lhs_ptr = &lhs[0];
             const int8_t *rhs_ptr = &rhs[0];

             int32_t res00 = 0;
             if (bias)
             {
                 res00 = *bias++;
             }

             for (int32_t rhs_cols_idx = 0; rhs_cols_idx < rhs_cols; ++rhs_cols_idx)
             {
                 int32_t rhs_value0 = (int8_t)rhs_ptr[0] + rhs_offset;
                 int32_t lhs_value = (int8_t)lhs_ptr[0] + lhs_offset;

                 res00 += lhs_value * rhs_value0;

                 ++rhs_ptr;
                 ++lhs_ptr;
             }

             // Quantize down
             res00 = arm_nn_requantize(res00, *dst_multiplier++, *dst_shift++);

             // Add offset
             res00 += dst_offset;

             // Clamp the result
             res00 = MAX(res00, activation_min);
             res00 = MIN(res00, activation_max);

             *dst = (int8_t)res00;
             dst += address_offset;
             rhs += rhs_cols;
         }
 #endif
     }

     else
     {
 #if defined(ARM_MATH_MVEI)
         const int32_t row_loop_cnt = rhs_rows / 4;
         const uint32x4_t address_offset_array = {0, address_offset, address_offset * 2, address_offset * 3};
         const uint32_t next_dst_offset = 4 * address_offset;

         for (int32_t i_row_loop_cnt = 0; i_row_loop_cnt < row_loop_cnt; i_row_loop_cnt++)
         {
             int32_t acc_0 = 0;
             int32_t acc_1 = 0;
             int32_t acc_2 = 0;
             int32_t acc_3 = 0;

             const int8_t *lhs_vec = lhs;
             const int8_t *rhs_0_ptr = rhs;
             const int8_t *rhs_1_ptr = rhs_0_ptr + rhs_cols;
             const int8_t *rhs_2_ptr = rhs_1_ptr + rhs_cols;
             const int8_t *rhs_3_ptr = rhs_2_ptr + rhs_cols;
             rhs = rhs_3_ptr + rhs_cols;

             __ASM volatile(
                 " .p2align 2                                 \n"
                 "   wlstp.8         lr, %[cnt], 1f           \n"
                 "   mov             %[out0], 0               \n"
                 "   mov             %[out1], 0               \n"
                 "   mov             %[out2], 0               \n"
                 "   mov             %[out3], 0               \n"
                 "   vldrb.8         q0, [%[col]], #16        \n"
                 "2:                                          \n"
                 "   vldrb.8         q1, [%[row0]], #16       \n"
                 "   vmladava.s8     %[out0], q0, q1          \n"
                 "   vldrb.8         q2, [%[row1]], #16       \n"
                 "   vmladava.s8     %[out1], q0, q2          \n"
                 "   vldrb.8         q3, [%[row2]], #16       \n"
                 "   vmladava.s8     %[out2], q0, q3          \n"
                 "   vldrb.8         q4, [%[row3]], #16       \n"
                 "   vmladava.s8     %[out3], q0, q4          \n"
                 "   vldrb.8         q0, [%[col]], #16        \n"
                 "   letp            lr, 2b                   \n"
                 "1:                                          \n"
             :
                 [col] "+r"(lhs_vec),
                 [row0] "+r"(rhs_0_ptr),
                 [row1] "+r"(rhs_1_ptr),
                 [row2] "+r"(rhs_2_ptr),
                 [row3] "+r"(rhs_3_ptr),
                 [out0] "=Te"(acc_0),
                 [out1] "=Te"(acc_1),
                 [out2] "=Te"(acc_2),
                 [out3] "=Te"(acc_3)
             :
                 [cnt] "r"(rhs_cols)
             :
                 "q0", "q1", "q2", "q3", "q4", "memory", "r14"
             );
             lhs_vec -= 8;

             int32x4_t acc = {acc_0, acc_1, acc_2, acc_3};

             acc = vaddq_s32(acc, vldrwq_s32(kernel_sum));
             kernel_sum += 4;

             acc = arm_requantize_mve_32x4(acc, vldrwq_s32(dst_multiplier), vldrwq_s32(dst_shift));
             dst_multiplier += 4;
             dst_shift += 4;

             acc = vaddq_s32(acc, vdupq_n_s32(dst_offset));
             acc = vmaxq_s32(acc, vdupq_n_s32(activation_min));
             acc = vminq_s32(acc, vdupq_n_s32(activation_max));

             vstrbq_scatter_offset_s32(dst, address_offset_array, acc);

             dst += next_dst_offset;
         }

     const int loop_cnt = rhs_rows % 4;
     for (int32_t i_row_loop_cnt = 0; i_row_loop_cnt < loop_cnt; i_row_loop_cnt++)
     {
         int32_t acc_0 = 0;
         const int8_t *lhs_vec = lhs;
         const int8_t *rhs_ptr = rhs;
         uint32_t col_cnt = (uint32_t)rhs_cols;
         rhs += rhs_cols;

         __ASM volatile(
             " .p2align 2                                 \n"
             "   wlstp.8         lr, %[cnt], 1f           \n"
             "   mov             %[out0], 0               \n"
             "   vldrb.8         q0, [%[col]], #16        \n"
             "2:                                          \n"
             "   vldrb.8         q1, [%[row0]], #16       \n"
             "   vmladava.s8     %[out0], q0, q1          \n"
             "   vldrb.8         q0, [%[col]], #16        \n"
             "   letp            lr, 2b                   \n"
             "1:                                          \n"
         :
             [col] "+r"(lhs_vec),
             [row0] "+r"(rhs_ptr),
             [out0] "=Te"(acc_0)
         :
             [cnt] "r"(col_cnt)
         :
             "q0", "q1", "q2", "q3", "q4", "memory", "r14"
         );
         lhs_vec -= 8;

         acc_0 += *kernel_sum++;

         acc_0 = arm_nn_requantize(acc_0, *dst_multiplier++, *dst_shift++);

         acc_0 += dst_offset;

         // Clamp the result
         acc_0 = MAX(acc_0, activation_min);
         *dst = MIN(acc_0, activation_max);
         dst += address_offset;
     }

 #elif defined(ARM_MATH_DSP)
         (void)kernel_sum;

         const int32_t row_loop_cnt = rhs_rows / 2;
         const int16_t lhs_offset_s16 = (int16_t)lhs_offset;
         const uint32_t lhs_offset_s16x2 = PKHBT(lhs_offset_s16, lhs_offset_s16, 16);

         for (int32_t i = 0; i < row_loop_cnt; i++)
         {
             int32_t acc_0 = 0;
             int32_t acc_1 = 0;
             if (bias)
             {
                 acc_0 = *bias++;
                 acc_1 = *bias++;
             }

             const int32_t col_loop_cnt = rhs_cols / 4;

             const int8_t *lhs_vec = lhs;
             const int8_t *rhs_0_ptr = rhs;
             const int8_t *rhs_1_ptr = rhs + rhs_cols;
             rhs += 2 * rhs_cols;

             for (int32_t j = col_loop_cnt; j != 0; j--)
             {
                 int32_t vec_0 = arm_nn_read_s8x4_ia(&lhs_vec);
                 int32_t vec_1 = SXTAB16_RORn(lhs_offset_s16x2, (uint32_t)vec_0, 8);

                 vec_0 = SXTAB16(lhs_offset_s16x2, vec_0);

                 int32_t ker_0 = arm_nn_read_s8x4_ia(&rhs_0_ptr);
                 int32_t ker_1 = SXTB16_RORn((uint32_t)ker_0, 8);
                 ker_0 = SXTB16(ker_0);

                 acc_0 = SMLAD(ker_1, vec_1, acc_0);
                 acc_0 = SMLAD(ker_0, vec_0, acc_0);

                 ker_0 = arm_nn_read_s8x4_ia(&rhs_1_ptr);
                 ker_1 = SXTB16_RORn((uint32_t)ker_0, 8);
                 ker_0 = SXTB16(ker_0);

                 acc_1 = SMLAD(ker_1, vec_1, acc_1);
                 acc_1 = SMLAD(ker_0, vec_0, acc_1);
             }

             for (int32_t k = col_loop_cnt * 4; k < rhs_cols; k++)
             {
                 const int32_t lhs_temp = (*lhs_vec + lhs_offset);
                 lhs_vec++;
                 acc_0 += lhs_temp * (*rhs_0_ptr);
                 rhs_0_ptr++;
                 acc_1 += lhs_temp * (*rhs_1_ptr);
                 rhs_1_ptr++;
             }

             acc_0 = arm_nn_requantize(acc_0, *dst_multiplier++, *dst_shift++);
             acc_1 = arm_nn_requantize(acc_1, *dst_multiplier++, *dst_shift++);

             // Add offset
             acc_0 += dst_offset;
             acc_1 += dst_offset;
             // Clamp the result
             acc_0 = MAX(acc_0, activation_min);
             acc_0 = MIN(acc_0, activation_max);
             acc_1 = MAX(acc_1, activation_min);
             acc_1 = MIN(acc_1, activation_max);
             *dst = (int8_t)acc_0;
             *(dst + address_offset) = (int8_t)acc_1;
             dst += 2 * address_offset;
         }

         if (rhs_rows & 0x1)
         {
             int32_t acc_0 = 0;
             if (bias)
             {
                 acc_0 = *bias++;
             }
             const int32_t col_loop_cnt = rhs_cols / 4;

             const int8_t *lhs_vec = lhs;
             const int8_t *rhs_ptr = rhs;

             for (int32_t i = col_loop_cnt; i != 0; i--)
             {
                 int32_t vec_0 = arm_nn_read_s8x4_ia(&lhs_vec);
                 int32_t vec_1 = SXTAB16_RORn(lhs_offset_s16x2, (uint32_t)vec_0, 8);
                 vec_0 = SXTAB16(lhs_offset_s16x2, vec_0);

                 int32_t ker_0 = arm_nn_read_s8x4_ia(&rhs_ptr);
                 int32_t ker_1 = SXTB16_RORn((uint32_t)ker_0, 8);
                 ker_0 = SXTB16(ker_0);

                 acc_0 = SMLAD(ker_1, vec_1, acc_0);
                 acc_0 = SMLAD(ker_0, vec_0, acc_0);
             }

             for (int32_t j = col_loop_cnt * 4; j < rhs_cols; j++)
             {
                 const int32_t lhs_temp = (*lhs_vec + lhs_offset);
                 lhs_vec++;
                 acc_0 += lhs_temp * (*rhs_ptr);
                 rhs_ptr++;
             }

             acc_0 = arm_nn_requantize(acc_0, *dst_multiplier++, *dst_shift++);

             // Add offset
             acc_0 += dst_offset;
             // Clamp the result
             acc_0 = MAX(acc_0, activation_min);
             acc_0 = MIN(acc_0, activation_max);
             *dst = (int8_t)acc_0;
             dst += address_offset;
         }

 #else
         (void)kernel_sum;

         const int32_t row_loop_cnt = rhs_rows / 3;

         for (int32_t i_row_loop_cnt = 0; i_row_loop_cnt < row_loop_cnt; i_row_loop_cnt++)
         {
             const int8_t *lhs_ptr = lhs;
             const int8_t *rhs_ptr_0 = &rhs[0];
             const int8_t *rhs_ptr_1 = &rhs[rhs_cols];
             const int8_t *rhs_ptr_2 = &rhs[rhs_cols * 2];

             int32_t res00 = 0;
             int32_t res01 = 0;
             int32_t res02 = 0;
             if (bias)
             {
                 res00 = *bias++;
                 res01 = *bias++;
                 res02 = *bias++;
             }
             for (int32_t rhs_cols_idx = 0; rhs_cols_idx < rhs_cols; ++rhs_cols_idx)
             {
                 const int32_t rhs_value0 = (int8_t)*rhs_ptr_0;
                 const int32_t rhs_value1 = (int8_t)*rhs_ptr_1;
                 const int32_t rhs_value2 = (int8_t)*rhs_ptr_2;
                 const int32_t lhs_value = (int8_t)*lhs_ptr + lhs_offset;

                 res00 += lhs_value * rhs_value0;
                 res01 += lhs_value * rhs_value1;
                 res02 += lhs_value * rhs_value2;

                 ++rhs_ptr_0;
                 ++rhs_ptr_1;
                 ++rhs_ptr_2;
                 ++lhs_ptr;
             }
             // Quantize down
             res00 = arm_nn_requantize(res00, *dst_multiplier++, *dst_shift++);
             res01 = arm_nn_requantize(res01, *dst_multiplier++, *dst_shift++);
             res02 = arm_nn_requantize(res02, *dst_multiplier++, *dst_shift++);

             // Add offset
             res00 += dst_offset;
             res01 += dst_offset;
             res02 += dst_offset;

             // Clamp the result
             res00 = MAX(res00, activation_min);
             res00 = MIN(res00, activation_max);
             res01 = MAX(res01, activation_min);
             res01 = MIN(res01, activation_max);
             res02 = MAX(res02, activation_min);
             res02 = MIN(res02, activation_max);

             *dst = (int8_t)res00;
             *(dst + address_offset) = (int8_t)res01;
             *(dst + 2 * address_offset) = (int8_t)res02;
             dst += 3 * address_offset;

             rhs += 3 * rhs_cols;
         }

         const int loop_cnt = rhs_rows % 3;

         for (int32_t i_loop_cnt = 0; i_loop_cnt < loop_cnt; i_loop_cnt++)
         {
             const int8_t *lhs_ptr = &lhs[0];
             const int8_t *rhs_ptr = &rhs[0];

             int32_t res00 = 0;
             if (bias)
             {
                 res00 = *bias++;
             }

             for (int32_t rhs_cols_idx = 0; rhs_cols_idx < rhs_cols; ++rhs_cols_idx)
             {
                 int32_t rhs_value0 = (int8_t)rhs_ptr[0];
                 int32_t lhs_value = (int8_t)lhs_ptr[0] + lhs_offset;

                 res00 += lhs_value * rhs_value0;

                 ++rhs_ptr;
                 ++lhs_ptr;
             }

             // Quantize down
             res00 = arm_nn_requantize(res00, *dst_multiplier++, *dst_shift++);

             // Add offset
             res00 += dst_offset;

             // Clamp the result
             res00 = MAX(res00, activation_min);
             res00 = MIN(res00, activation_max);

             *dst = (int8_t)res00;
             dst += address_offset;
             rhs += rhs_cols;
         }
 #endif
     }
     return ARM_CMSIS_NN_SUCCESS;
 }

 /**
  * @} end of Doxygen group
  */
