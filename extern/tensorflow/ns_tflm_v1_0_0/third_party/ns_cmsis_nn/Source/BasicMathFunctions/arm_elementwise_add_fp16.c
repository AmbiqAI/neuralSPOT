/*
 * SPDX-FileCopyrightText: Copyright 2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
 * Title:        arm_elementwise_add_fp16
 * Description:  Elementwise add
 *
 * $Date:        11 Feb 2025
 * $Revision:    V.1.0.0
 *
 * Target Processor:  Cortex-M CPUs
 *
 * -------------------------------------------------------------------- */

 #include "arm_nnfunctions.h"
 #include "arm_nnsupportfunctions.h"

 /**
  *  @ingroup Public
  */

 /**
  * @addtogroup groupElementwise
  * @{
  */

 /*
  * fp16 elementwise add
  *
  * Refer header file for details.
  *
  */

#if defined(ARM_MATH_MVE_FLOAT16) && !defined(ARM_MATH_AUTOVECTORIZE)

arm_cmsis_nn_status arm_elementwise_add_fp16(
    const float16_t *input_1_vect,
    const float16_t *input_2_vect,
    float16_t *output,
    const float16_t out_activation_min,
    const float16_t out_activation_max,
    const int32_t block_size
) {

    int32_t count = block_size;

    __ASM volatile(
        " .p2align 2                                 \n"
        "   wlstp.16         lr, %[cnt], 1f          \n"
        // Place min and max into q4 and q5
        "   vdup.16          q4, %[min]              \n"
        "   vdup.16          q5, %[max]              \n"
        "2:                                          \n"
        "   vldrh.16         q1, [%[lhs]], #16       \n"
        "   vldrh.16         q2, [%[rhs]], #16       \n"
        "   vadd.f16         q3, q1, q2              \n"
        // Clamp the result
        "   vminnm.f16       q3, q3, q5              \n"
        "   vmaxnm.f16       q3, q3, q4              \n"
        "   vstrh.16         q3, [%[dst]], #16       \n"
        "   letp             lr, 2b                  \n"
        "1:                                          \n"
    :
        [lhs] "+r"(input_1_vect),
        [rhs] "+r"(input_2_vect),
        [dst] "+r"(output),
        [cnt] "+r"(count)
    :
        [min] "r"(out_activation_min),
        [max] "r"(out_activation_max)
    :
        "q1", "q2", "q3", "q4", "q5", "memory", "r14"
    );

    return (ARM_CMSIS_NN_SUCCESS);
}

#else
#if defined(ARM_FLOAT16_SUPPORTED)

arm_cmsis_nn_status arm_elementwise_add_fp16(
    const float16_t *input_1_vect,
    const float16_t *input_2_vect,
    float16_t *output,
    const float16_t out_activation_min,
    const float16_t out_activation_max,
    const int32_t block_size
) {

    float16_t sum;

    for(int32_t i = 0; i < block_size; i++)
    {
        sum = input_1_vect[i] + input_2_vect[i];
        sum = MAX(sum, out_activation_min);
        sum = MIN(sum, out_activation_max);
        output[i] = sum;
    }
    return (ARM_CMSIS_NN_SUCCESS);
}

#endif /* defined(ARM_FLOAT16_SUPPORTED */
#endif /* defined(ARM_MATH_MVEF) && !defined(ARM_MATH_AUTOVECTORIZE) */

 /**
  * @} end of Doxygen group
  */
