/*
 * SPDX-FileCopyrightText: Copyright 2024 Arm Limited and/or its affiliates <open-source-office.com>
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
 * Title:        arm_dequantize_s8_f32.c
 * Description:  int8 to float32 dequantization
 *
 * $Date:        15 April 2025
 * $Revision:    V.1.0.0
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
 * @addtogroup Quantization
 * @{
 */

/*
 * int8_t to float32_t dequantization function.
 *
 * Refer header file for details.
 *
 */
arm_cmsis_nn_status
arm_dequantize_s8_f32(
    const int8_t *input,
    float *output,
    int32_t size,
    int32_t zero_point,
    float scale
){

#if defined(ARM_MATH_MVEI)

    __ASM volatile(
      " .p2align 2                                 \n"
      "   wlstp.32         lr, %[size], 1f         \n"
      "   vdup.32           q4, %[zero_point]      \n"
      "   vdup.32           q5, %[scale]           \n"
      "2:                                          \n"
      "   vldrb.s32       q0, [%[input]], #4       \n"
      "   vsub.s32        q0, q0, q4               \n"
      "   vcvt.f32.s32    q1, q0                   \n"
      "   vmul.f32        q1, q1, q5               \n"
      "   vstrw.32        q1, [%[output]], #16     \n"
      "   letp             lr, 2b                  \n"
      "1:                                          \n"
    :
      [input] "+r" (input),
      [output] "+r" (output),
      [size] "+r" (size)
    :
      [scale] "r" (scale),
      [zero_point] "r" (zero_point)
    :
      "q0", "q1", "q4", "q5", "memory", "r14"
    );

#else
    for (int i = 0; i < size; i++) {
        output[i] = (input[i] - zero_point) * scale;
    }
#endif

    return ARM_CMSIS_NN_SUCCESS;

}

/**
 * @} end of Quantization group
 */
