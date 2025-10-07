/* ----------------------------------------------------------------------
 * Project:      CMSIS NN Library
 * Title:        arm_hard_swish_compat_s8.c
 * Description:  Hard Swish function for int8_t compatible w/ TFLite
 *
 * $Date:        09 September 2025
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
 * Hard Swish activation function for int8_t data type.
 *
 * Refer header file for details.
 *
 */
arm_cmsis_nn_status arm_hard_swish_compat_s8(
    const int8_t *input,
    const int32_t input_offset,
    const int32_t output_offset,
    const int32_t output_multiplier_fp,
    const int32_t output_multiplier_exp,
    const int32_t relu_multiplier_fp,
    const int32_t relu_multiplier_exp,
    int8_t *output,
    const int32_t output_size)
{
    if (output_multiplier_exp > 0)
    {
        return ARM_CMSIS_NN_ARG_ERROR;
    }

#if defined(ARM_MATH_MVEI)

    int32_t rem = output_size;

    while (rem > 0)
    {
        mve_pred16_t pred = vctp16q((uint32_t)rem);

        // Load input
        int16x8_t x = vldrbq_z_s16(input, pred);

        // x on integer domain (remove z_in)
        x = vsubq_s16(x, vdupq_n_s16(input_offset));

        // Hires = x << 7 (saturating)
        int16x8_t hires = vqshlq_n_s16(x, 7);

        // Preshift output scale: y_pre = SQRDMULH(hires, out_mul16)
        int16x8_t y_pre = vqrdmulhq_n_s16(hires, (int16_t)output_multiplier_fp);

        // Reluish path into [-1,1], then [0,1]
        int16x8_t rel = hires;

        if (relu_multiplier_exp > 0)
        {
            rel = vqshlq_s16(rel, vdupq_n_s16(relu_multiplier_exp - 1));
            rel = vqrdmulhq_n_s16(rel, (int16_t)relu_multiplier_fp);
            rel = vqshlq_n_s16(rel, 1);
        }
        else if (relu_multiplier_exp < 0)
        {
            rel = vqrdmulhq_n_s16(rel, relu_multiplier_fp);
            rel = arm_divide_by_power_of_two_mve_s16(rel, -relu_multiplier_exp);
        }
        else
        {
            rel = vqrdmulhq_n_s16(rel, (int16_t)relu_multiplier_fp);
        }

        // shift [-1,1] → [0,1]: (rel + 32768) >> 1  (rounded)
        rel = vrhaddq_s16(rel, vdupq_n_s16(32767));

        // y_pre is on preshift output scale. Multiply by relu using **non-rounded** SDHM
        int16x8_t y = vqdmulhq_s16(rel, y_pre);

        // Finally apply output multiplier exponent
        if (output_multiplier_exp < 0) {
            y = arm_divide_by_power_of_two_mve_s16(y, -output_multiplier_exp);
        }

        // Add output offset and clamp the result
        y = vaddq_n_s16(y, output_offset);
        y = vmaxq_s16(y, vdupq_n_s16(INT8_MIN));
        y = vminq_s16(y, vdupq_n_s16(INT8_MAX));

        // Store output
        vstrbq_p_s16(output, y, pred);

        // Increment pointers
        input += 8;
        output += 8;
        rem -= 8;
    }
#else

    for (int32_t i = 0; i < output_size; ++i)
    {
        // x on integer domain (remove z_in)
        int16_t x = (int16_t)((int32_t)input[i] - input_offset);

        // Hires = x << 7 (saturating)
        int16_t hires = arm_nn_sat_lshift_s16(x, 7);

        // Preshift output scale: y_pre = SQRDMULH(hires, out_mul16)
        int16_t y_pre = arm_nn_sqrdmulh_s16(hires, (int16_t)output_multiplier_fp);

        // Reluish path into [-1,1], then [0,1]
        int16_t rel = hires;

        if (relu_multiplier_exp > 0)
        {
            rel = arm_nn_sat_lshift_s16(rel, relu_multiplier_exp - 1);
            rel = arm_nn_sqrdmulh_s16(rel, (int16_t)relu_multiplier_fp);
            rel = arm_nn_sat_lshift_s16(rel, 1);
        }
        else if (relu_multiplier_exp < 0)
        {
            rel = arm_nn_sqrdmulh_s16(rel, relu_multiplier_fp);
            rel = arm_nn_divide_by_power_of_two_s16(rel, -relu_multiplier_exp);
        }
        else
        {
            rel = arm_nn_sqrdmulh_s16(rel, (int16_t)relu_multiplier_fp);
        }

        // shift [-1,1] → [0,1]: (rel + 32768) >> 1  (rounded)
        rel = (int16_t)(( (int32_t)rel + 32768 ) >> 1);

        // y_pre is on preshift output scale. Multiply by relu using **non-rounded** SDHM
        int16_t y = arm_nn_sqdmulh_s16(rel, y_pre);

        // Finally apply output multiplier exponent
        if (output_multiplier_exp < 0) {
            y = arm_nn_divide_by_power_of_two_s16(y, -output_multiplier_exp);
        }

        // Add output offset and clamp the result
        int32_t y8 = (int32_t)y + output_offset;
        y8 = CLAMP(y8, INT8_MAX, INT8_MIN);

        output[i] = (int8_t)y8;
    }

#endif /* ARM_MATH_MVEI */

    return ARM_CMSIS_NN_SUCCESS;
}

/**
 * @} end of Doxygen group
 */
