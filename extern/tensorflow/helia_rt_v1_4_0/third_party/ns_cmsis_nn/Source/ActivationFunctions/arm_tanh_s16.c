/*
 * SPDX-FileCopyrightText: Copyright Ambiq 2025
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
 * Title:        arm_tanh_s16.c
 * Description:  Tanh activation function for s16 using partial look-up tables
 *
 * $Date:        12 September 2025
 * $Revision:    V.1.0.0
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

/*
 * @brief Tanh activation function for s16
 *
 * @note  Refer header file for details.
 *
 */
arm_cmsis_nn_status arm_tanh_s16(
    const int16_t *input,
    int16_t *output,
    const int32_t input_size,
    int32_t input_multiplier,
    int32_t input_left_shift
)
{
    // power-of-two case
    if (input_multiplier == 0)
    {
        input_multiplier = 3 << input_left_shift;
        input_left_shift = 0;
    }
    const int32_t round = (input_left_shift > 0) ? (1 << (input_left_shift - 1)) : 0;

#if defined(ARM_MATH_MVEI)

    int32_t count = input_size;
    int32_t blk_size = (input_size + 7) / 8;

    while (blk_size > 0)
    {
        mve_pred16_t p = vctp16q(count);
        int32_t adv = (count > 8) ? 8 : count;

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

        /* Compute table index: uh = abs >> 8, and fractional part: ut = abs & 0xFF. */
        uint32x4_t uh_lo = vshrq_n_u32(abs_lo, 8);
        uint32x4_t uh_hi = vshrq_n_u32(abs_hi, 8);
        uint16x8_t uh = vdupq_n_u16(0);
        uh = vmovnbq_u32(uh, uh_lo);
        uh = vmovntq_u32(uh, uh_hi);

        uint32x4_t ut_lo = vandq_u32(abs_lo, vdupq_n_u32(0xFF));
        uint32x4_t ut_hi = vandq_u32(abs_hi, vdupq_n_u32(0xFF));

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

        /* Interpolation: result = (ua << 8) + ut * (ub - ua). */
        uint32x4_t interp_lo = vaddq_u32(vshlq_n_u32(ua_lo, 8), vmulq_u32(ut_lo, vsubq_u32(ub_lo, ua_lo)));
        uint32x4_t interp_hi = vaddq_u32(vshlq_n_u32(ua_hi, 8), vmulq_u32(ut_hi, vsubq_u32(ub_hi, ua_hi)));

        /* For values with index (uh) >= 255, saturate to maximum: 0xFFFF << 8 */
        p_sat = vcmpltq_n_s32(vreinterpretq_s32_u32(uh_lo), 255);
        interp_lo = vpselq_u32(interp_lo, vdupq_n_u32(0xFFFF << 8), p_sat);
        p_sat = vcmpltq_n_s32(vreinterpretq_s32_u32(uh_hi), 255);
        interp_hi = vpselq_u32(interp_hi, vdupq_n_u32(0xFFFF << 8), p_sat);

        /* Tanh sign/center correction. */
        const uint32x4_t center = vdupq_n_u32(1u << (14 + 9));   // 1<<23
        mve_pred16_t p_pos_lo = vcmpgeq_n_s32(scaled_lo, 0);
        mve_pred16_t p_pos_hi = vcmpgeq_n_s32(scaled_hi, 0);

        // pos = (interp - center) + (1<<7)
        uint32x4_t pos_lo = vaddq_n_u32(vsubq_u32(interp_lo, center), (1u << 7));
        uint32x4_t pos_hi = vaddq_n_u32(vsubq_u32(interp_hi, center), (1u << 7));

        // neg = (-interp + center) + ((1<<7) - 1)
        uint32x4_t neg_lo = vaddq_u32(center,
                               vaddq_n_u32(vreinterpretq_u32_s32(vnegq_s32(vreinterpretq_s32_u32(interp_lo))),
                                           (1u << 7) - 1u));
        uint32x4_t neg_hi = vaddq_u32(center,
                               vaddq_n_u32(vreinterpretq_u32_s32(vnegq_s32(vreinterpretq_s32_u32(interp_hi))),
                                           (1u << 7) - 1u));

        uint32x4_t final_lo = vpselq_u32(pos_lo, neg_lo, p_pos_lo);
        uint32x4_t final_hi = vpselq_u32(pos_hi, neg_hi, p_pos_hi);

        /* Final >> 8 to Q15. */
        final_lo = vshrq_n_u32(final_lo, 8);
        final_hi = vshrq_n_u32(final_hi, 8);

        uint16x8_t vec_result = vdupq_n_u16(0);
        vec_result = vmovnbq_u32(vec_result, final_lo);
        vec_result = vmovntq_u32(vec_result, final_hi);

        /* Store masked */
        vst1q_p_s16(output, vreinterpretq_s16_u16(vec_result), p);

        input  += adv;
        output += adv;
        count  -= adv;
        blk_size--;
    }

#else

    for (int32_t i = 0; i < input_size; i++)
    {
        int32_t input_data = ((input[i] * input_multiplier + round) >> input_left_shift);
        uint32_t abs_input_data = (uint32_t)(input_data >= 0 ? input_data : -input_data);
        uint32_t uh = abs_input_data >> 8;
        uint32_t result;
        if (uh >= 255)
        {
            result = 0xFFFF << 8;
        }
        else
        {
            uint32_t ua = sigmoid_table_uint16[uh];
            uint32_t ub = sigmoid_table_uint16[uh + 1];
            uint32_t ut = abs_input_data & 0xFF;
            result = (ua << 8) + ut * (ub - ua);
        }
        result = (input_data >= 0)
                    ? (result - (1 << (14 + 9)) + (1 << (9 - 2)))
                    : (-result + (1 << (14 + 9)) + (1 << (9 - 2)) - 1);
        result >>= (9 - 1);
        output[i] = (int16_t)result;
    }

#endif

    return ARM_CMSIS_NN_SUCCESS;
}


/**
 * @} end of Doxygen group
 */
