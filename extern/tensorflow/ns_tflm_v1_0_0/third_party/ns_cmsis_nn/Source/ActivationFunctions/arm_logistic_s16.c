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
 * Title:        arm_nn_activation_s16.c
 * Description:  Q15 neural network activation function using direct table look-up
 *
 * $Date:        19 January 2024
 * $Revision:    V.2.0.0
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */

#include "arm_nn_tables.h"
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

arm_cmsis_nn_status arm_logistic_s16_default(int16_t *input,
                                             int16_t *output,
                                             const int32_t input_size,
                                             int32_t input_multiplier,
                                             int32_t input_left_shift)
{
    // TFLITE_DCHECK_GE(input_left_shift, 0);
    if (input_multiplier == 0)
    { // power-of-two case
        input_multiplier = 3 << input_left_shift;
        input_left_shift = 0;
    }
    const int32_t round = (input_left_shift > 0) ? (1 << (input_left_shift - 1)) : 0;

    // Non-MVE (scalar) version.
    for (int32_t i = 0; i < input_size; i++)
    {
        int32_t input_data = ((input[i] * input_multiplier + round) >> input_left_shift);
        uint32_t abs_input_data = (uint32_t)(input_data >= 0 ? input_data : -input_data);
        uint32_t uh = abs_input_data >> 9;
        uint32_t result;
        if (uh >= 255)
        {
            result = 0x7FFF << 10;
        }
        else
        {
            uint32_t ua = sigmoid_table_uint16[uh];
            uint32_t ub = sigmoid_table_uint16[uh + 1];
            uint32_t ut = abs_input_data & 0x1FF;
            result = (ua << 9) + ut * (ub - ua);
        }
        result = (input_data >= 0) ? (result + (1 << 9)) : ((1 << (16 + 9)) - result + (1 << 9) - 1);
        result >>= 10;
        output[i] = (int16_t)result;
    }

    return ARM_CMSIS_NN_SUCCESS;
}

/*
 * @brief Neural network activation function using direct table look-up
 *
 * @note  Refer header file for details.
 *
 */
arm_cmsis_nn_status arm_logistic_s16(int16_t *input,
                                     int16_t *output,
                                     const int32_t input_size,
                                     int32_t input_multiplier,
                                     int32_t input_left_shift)
{

#if defined(ARM_MATH_MVEI)

    // TFLITE_DCHECK_GE(input_left_shift, 0);
    if (input_multiplier == 0)
    { // power-of-two case
        input_multiplier = 3 << input_left_shift;
        input_left_shift = 0;
    }
    const int32_t round = (input_left_shift > 0) ? (1 << (input_left_shift - 1)) : 0;

    int32_t count = input_size;
    int32_t blk_size = input_size / 8;

    while (blk_size > 0)
    {

        mve_pred16_t p = vctp16q(count);

        /* Load up to 8 int16_t input values with masked load. */
        int16x8_t vec_in = vld1q_z_s16(input, p);

        /* Extend the int16 vector to two int32 vectors. */
        int32x4_t vec_in_lo = vmovlbq_s16(vec_in);
        int32x4_t vec_in_hi = vmovltq_s16(vec_in);

        /* Multiply by input_multiplier, add rounding and shift right by input_left_shift. */
        int32x4_t scaled_lo = vaddq_n_s32(vmulq_n_s32(vec_in_lo, input_multiplier), round);
        scaled_lo = vshlq_s32(scaled_lo, vdupq_n_s32(-input_left_shift));
        int32x4_t scaled_hi = vaddq_n_s32(vmulq_n_s32(vec_in_hi, input_multiplier), round);
        scaled_hi = vshlq_s32(scaled_hi, vdupq_n_s32(-input_left_shift));

        /* Compute absolute values (for LUT lookup) */
        uint32x4_t abs_lo = vreinterpretq_u32_s32(vabsq_s32(scaled_lo));
        uint32x4_t abs_hi = vreinterpretq_u32_s32(vabsq_s32(scaled_hi));

        /* Compute table index: uh = abs >> 9, and fractional part: ut = abs & 0x1FF. */
        uint32x4_t uh_lo = vshrq_n_u32(abs_lo, 9);
        uint32x4_t uh_hi = vshrq_n_u32(abs_hi, 9);
        uint16x8_t uh = vdupq_n_u16(0);
        uh = vmovnbq_u32(uh, uh_lo);
        uh = vmovntq_u32(uh, uh_hi);

        uint32x4_t ut_lo = vandq_u32(abs_lo, vdupq_n_u32(0x1FF));
        uint32x4_t ut_hi = vandq_u32(abs_hi, vdupq_n_u32(0x1FF));

        /* Sature when uh >= 255 */
        mve_pred16_t p_sat = vcmpltq_n_s16(vreinterpretq_s16_u16(uh), 255);

        /* Gather LUT values for ua and ub (ua = LUT[uh], ub = LUT[uh+1]). */
        uint16x8_t ua = vldrhq_gather_shifted_offset_z_u16(sigmoid_table_uint16, uh, p_sat);
        uint16x8_t ub = vldrhq_gather_shifted_offset_z_u16(sigmoid_table_uint16, vaddq_n_u16(uh, 1), p_sat);

        /* Extend LUT values to 32-bit for interpolation. */
        uint32x4_t ua_lo = vmovlbq_u16(ua);
        uint32x4_t ua_hi = vmovltq_u16(ua);
        uint32x4_t ub_lo = vmovlbq_u16(ub);
        uint32x4_t ub_hi = vmovltq_u16(ub);

        /* Interpolation: result = (ua << 9) + ut * (ub - ua). */
        uint32x4_t interp_lo = vaddq_u32(vshlq_n_u32(ua_lo, 9), vmulq_u32(ut_lo, vsubq_u32(ub_lo, ua_lo)));
        uint32x4_t interp_hi = vaddq_u32(vshlq_n_u32(ua_hi, 9), vmulq_u32(ut_hi, vsubq_u32(ub_hi, ua_hi)));

        /* For values with index (uh) >= 255, saturate to maximum: 0x7FFF << 10 */
        p_sat = vcmpltq_n_s32(vreinterpretq_s32_u32(uh_lo), 255);
        interp_lo = vpselq_u32(interp_lo, vdupq_n_u32(0x7FFF << 10), p_sat);
        p_sat = vcmpltq_n_s32(vreinterpretq_s32_u32(uh_hi), 255);
        interp_hi = vpselq_u32(interp_hi, vdupq_n_u32(0x7FFF << 10), p_sat);

        /* Sign correction:
           - For non-negative scaled input: result = interp + (1 << 9).
           - For negative scaled input: result = (1 << (16+9)) - interp + (1 << 9) - 1.
        */
        mve_pred16_t p_pos_lo = vcmpgeq_n_s32(scaled_lo, 0);
        mve_pred16_t p_pos_hi = vcmpgeq_n_s32(scaled_hi, 0);

        uint32x4_t pos_res_lo = vaddq_n_u32(interp_lo, 1 << 9);
        uint32x4_t pos_res_hi = vaddq_n_u32(interp_hi, 1 << 9);

        uint32x4_t neg_base = vdupq_n_u32((1 << (16 + 9)));
        uint32x4_t neg_res_lo = vsubq_u32(neg_base, interp_lo);
        uint32x4_t neg_res_hi = vsubq_u32(neg_base, interp_hi);
        neg_base = vdupq_n_u32((1 << 9) - 1);
        neg_res_lo = vaddq_u32(neg_res_lo, neg_base);
        neg_res_hi = vaddq_u32(neg_res_hi, neg_base);

        uint32x4_t final_lo = vpselq_u32(pos_res_lo, neg_res_lo, p_pos_lo);
        uint32x4_t final_hi = vpselq_u32(pos_res_hi, neg_res_hi, p_pos_hi);

        /* Shift right by 10 to convert back to 16-bit range. */
        final_lo = vshrq_n_u32(final_lo, 10);
        final_hi = vshrq_n_u32(final_hi, 10);

        uint16x8_t vec_result = vdupq_n_u16(0);
        vec_result = vmovnbq_u32(vec_result, final_lo);
        vec_result = vmovntq_u32(vec_result, final_hi);

        /* Store the results using a masked store */
        p = vctp16q(count);
        vst1q_p_s16(output, vreinterpretq_s16_u16(vec_result), p);

        input += 8;
        output += 8;
        count -= 8;
        blk_size--;
    }

    // Handle the remaining elements
    count = input_size % 8;
    return arm_logistic_s16_default(input, output, count, input_multiplier, input_left_shift);

#else
    return arm_logistic_s16_default(input, output, input_size, input_multiplier, input_left_shift);
#endif

    return ARM_CMSIS_NN_SUCCESS;
}
