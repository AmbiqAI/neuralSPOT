/*
 * SPDX-FileCopyrightText: <text>Copyright 2010-2020, 2022, 2024 Arm Limited and/or its affiliates
 * <open-source-office@arm.com></text>
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
 * Title:        arm_relu_s8.c
 * Description:  ReLU functions for int8_t data type
 *
 * $Date:        23 April 2025
 * $Revision:    V.1.0.0
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */

#include "arm_nn_types.h"
#include "arm_nnfunctions.h"
#include "arm_nnsupportfunctions.h"

/**
*  @ingroup groupNN
*/

/**
* @addtogroup Acti
* @{
*/

/*
* ReLU activation functions for int8_t data type.
*
* Refer header file for details.
*
*/
arm_cmsis_nn_status arm_relu_generic_s8(const int8_t *input,
                                        const int32_t input_offset,
                                        const int32_t output_offset,
                                        const int32_t output_multiplier,
                                        const int32_t output_shift,
                                        const int32_t act_min,
                                        const int32_t act_max,
                                        int8_t *output,
                                        const int32_t output_size)
{

    int32_t flat_size = output_size;

    const int32_t quantized_min = MAX(INT8_MIN, act_min);
    const int32_t quantized_max = MIN(INT8_MAX, act_max);

#if defined(ARM_MATH_MVEI)
    // Perform 4 operations in parallel
    uint32_t blkCnt = (flat_size + 3) / 4;
    while (blkCnt > 0U)
    {
        mve_pred16_t res_pred = vctp32q((uint32_t)flat_size);
        int32x4_t res = vldrbq_z_s32(input, res_pred);
        // Subtract the input offset
        res = vsubq_s32(res, vdupq_n_s32(input_offset));
        // Multiply by the output multiplier
        res = arm_requantize_mve(res, output_multiplier, output_shift);
        res = vaddq_n_s32(res, output_offset);
        // Clamp the result
        res = vmaxq_s32(res, vdupq_n_s32(quantized_min));
        res = vminq_s32(res, vdupq_n_s32(quantized_max));
        // Store the result
        vstrbq_p_s32(output, res, res_pred);
        // Increment pointers
        input += 4;
        output += 4;
        blkCnt -= 1;
        flat_size -= 4;
    }

#else

    int32_t res;
    for (int i = 0; i < flat_size; ++i)
    {
        // Subtract the input offset
        res = (int32_t)input[i] - input_offset;
        // Multiply by the output multiplier
        res = arm_nn_requantize(res, output_multiplier, output_shift);
        res += output_offset;
        // Clamp the result
        res = CLAMP(res, quantized_max, quantized_min);
        // Store the result
        output[i] = (int8_t)res;
    }

#endif

    return ARM_CMSIS_NN_SUCCESS;
}


arm_cmsis_nn_status arm_relu_s8(const int8_t *input,
                                const int32_t input_offset,
                                const int32_t output_offset,
                                const int32_t output_multiplier,
                                const int32_t output_shift,
                                int8_t *output,
                                const int32_t output_size)
{
    int32_t act_min = output_offset;
    int32_t act_max = INT8_MAX;

    return arm_relu_generic_s8(
        input,
        input_offset,
        output_offset,
        output_multiplier,
        output_shift,
        act_min,
        act_max,
        output,
        output_size
    );
}


/**
 * @} end of Doxygen group
 */
