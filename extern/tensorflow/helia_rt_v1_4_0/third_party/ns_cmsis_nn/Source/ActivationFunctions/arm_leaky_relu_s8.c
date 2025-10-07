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
 * Title:        arm_leaky_relu_s8.c
 * Description:  Leaky ReLU function for int8_t data type
 *
 * $Date:        21 February 2025
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
 * Leaky ReLU activation function for int8_t data type.
 *
 * Refer header file for details.
 *
 */
arm_cmsis_nn_status arm_leaky_relu_s8(const int8_t *input,
                                      const int32_t input_offset,
                                      const int32_t output_offset,
                                      const int32_t output_multiplier_alpha,
                                      const int32_t output_shift_alpha,
                                      const int32_t output_multiplier_identity,
                                      const int32_t output_shift_identity,
                                      int8_t *output,
                                      const int32_t output_size)
{

    int8_t *input_data = (int8_t *)input;
    int8_t *output_data = output;
    int32_t flat_size = output_size;

    const int32_t quantized_min = INT8_MIN;
    const int32_t quantized_max = INT8_MAX;

#if defined(ARM_MATH_MVEI)
    // Perform 4 operations in parallel
    uint32_t blkCnt = (flat_size + 3) / 4;

    #ifdef CMSIS_NN_USE_SINGLE_ROUNDING
    const int32_t right_shift_alpha = MIN(-1, output_shift_alpha);
    const int32_t left_shift_alpha = output_shift_alpha - right_shift_alpha;
    const int32_t right_shift_identity = MIN(-1, output_shift_identity);
    const int32_t left_shift_identity = output_shift_identity - right_shift_identity;
    #else
    const int32_t left_shift_alpha = LEFT_SHIFT(output_shift_alpha);
    const int32_t right_shift_alpha = -RIGHT_SHIFT(output_shift_alpha);
    const int32_t left_shift_identity = LEFT_SHIFT(output_shift_identity);
    const int32_t right_shift_identity = -RIGHT_SHIFT(output_shift_identity);
    #endif

    while (blkCnt > 0U)
    {
        mve_pred16_t val_pred = vctp32q((uint32_t)flat_size);
        // Load values in 32-bit registers
        int32x4_t res = vldrbq_z_s32(input_data, val_pred);
        // Subtract the input offset
        res = vsubq_s32(res, vdupq_n_s32(input_offset));
        // For values < 0, apply alpha otherwise apply identity
        mve_pred16_t alpha_pred = vcmpltq_n_s32(res, 0);
        int32x4_t left_shift_ident_dup = vdupq_n_s32(left_shift_identity);
        int32x4_t left_shift_dup = vdupq_m_n_s32(left_shift_ident_dup, left_shift_alpha, alpha_pred);
        int32x4_t right_shift_ident_dup = vdupq_n_s32(right_shift_identity);
        int32x4_t right_shift_dup = vdupq_m_n_s32(right_shift_ident_dup, right_shift_alpha, alpha_pred);
        int32x4_t mult_ident_dup = vdupq_n_s32(output_multiplier_identity);
        int32x4_t mult_dup = vdupq_m_n_s32(mult_ident_dup, output_multiplier_alpha, alpha_pred);
    #ifdef CMSIS_NN_USE_SINGLE_ROUNDING
        res = vqdmulhq_s32(vshlq_s32(res, left_shift_dup), mult_dup);
        res = vrshlq_s32(res, right_shift_dup);
    #else
        res = vqrdmulhq_s32(vshlq_s32(res, left_shift_dup), mult_dup);
        int32x4_t fixup = vshrq_n_s32(vandq_s32(res, right_shift_dup), 31);
        int32x4_t fixed_up_dividend = vqaddq_s32(res, fixup);
        res = vrshlq_s32(fixed_up_dividend, right_shift_dup);
    #endif
        // Add the output offset
        res = vaddq_n_s32(res, output_offset);
        // Clamp the result
        res = vmaxq_s32(res, vdupq_n_s32(quantized_min));
        res = vminq_s32(res, vdupq_n_s32(quantized_max));
        // Store the result
        vstrbq_p_s32(output_data, res, val_pred);
        // Increment pointers
        input_data += 4;
        output_data += 4;
        blkCnt -= 1;
        flat_size -= 4;
    }

#else

    int32_t val;
    for (int i = 0; i < flat_size; ++i)
    {
        const int32_t input_i32 = input_data[i] - input_offset;
        if (input_i32 >= 0)
        {
            val = arm_nn_requantize(input_i32, output_multiplier_identity, output_shift_identity);
        }
        else
        {
            val = arm_nn_requantize(input_i32, output_multiplier_alpha, output_shift_alpha);
        }
        val += output_offset;
        val = CLAMP(val, quantized_max, quantized_min);
        output_data[i] = (int8_t)val;
    }

#endif

    return ARM_CMSIS_NN_SUCCESS;
}

/**
 * @} end of Doxygen group
 */
