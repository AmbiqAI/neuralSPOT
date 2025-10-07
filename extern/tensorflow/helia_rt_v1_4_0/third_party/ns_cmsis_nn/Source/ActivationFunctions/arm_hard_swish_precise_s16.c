/* ----------------------------------------------------------------------
 * Project:      CMSIS NN Library
 * Title:        arm_hard_swish_precise_s16.c
 * Description:  Hard Swish function for int16_t data type
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
* Hard Swish activation function for int16_t data type.
*
* Refer header file for details.
*
*/
arm_cmsis_nn_status arm_hard_swish_precise_s16(
    const int16_t *input,
    const int32_t input_offset,
    const int32_t output_offset,
    const int32_t output_multiplier,
    const int32_t output_shift,
    const int32_t relu_q3,
    const int32_t relu_q6,
    const int32_t prescale,
    int16_t *output,
    const int32_t output_size)
{

    if (!input || !output || output_size < 0)
    {
        return ARM_CMSIS_NN_ARG_ERROR;
    }

    int32_t flat_size = output_size;
    const int32_t qmin = INT16_MIN;
    const int32_t qmax = INT16_MAX;

#if defined(ARM_MATH_MVEI)

    // Perform 4 operations in parallel
    uint32_t blkCnt = (flat_size + 3) / 4U;

    while (blkCnt > 0U)
    {
        // Load input and create predicate
        mve_pred16_t pred = vctp32q((uint32_t)flat_size);
        int32x4_t x = vldrhq_z_s32(input, pred);

        // Center input to remove zero-point
        x = vsubq_s32(x, vdupq_n_s32(input_offset));

        // Compute xr = clamp(x + relu_q3, 0, relu_q6)
        int32x4_t xr = vaddq_n_s32(x, relu_q3);
        xr = vmaxq_s32(xr, vdupq_n_s32(0));
        xr = vminq_s32(xr, vdupq_n_s32(relu_q6));

        // Prescale to prevent overflow in multiplication
        xr = vrshlq_n_s32(xr, -prescale);

        // Integer product x * xr (unit: s_in^2)
        int32x4_t y = vmulq_s32(x, xr);

        // Requantize the result (includes division by 6)
        y = arm_requantize_mve(y, output_multiplier, output_shift);
        y = vaddq_n_s32(y, output_offset);

        // Clamp and store
        y = vmaxq_s32(y, vdupq_n_s32(qmin));
        y = vminq_s32(y, vdupq_n_s32(qmax));
        vstrhq_p_s32(output, y, pred);

        // Increment pointers
        input += 4;
        output += 4;
        blkCnt -= 1;
        flat_size -= 4;
    }

#else

    for (int32_t i = 0; i < flat_size; i++)
    {
        // Center input to remove zero-point
        int32_t x = (int32_t)input[i] - input_offset;

        // Compute xr = clamp(x + relu_q3, 0, relu_q6)
        int32_t xr = CLAMP(x + relu_q3, relu_q6, 0);

        // Prescale to prevent overflow in multiplication
        xr = arm_nn_nonneg_divide_by_pot_s32(xr, prescale);

        // Integer product x * xr (unit: s_in^2)
        int32_t y = x * xr;

        // Requantize the result (includes division by 6)
        y = arm_nn_requantize(y, output_multiplier, output_shift);
        y += output_offset;

        // Clamp and store
        y = CLAMP(y, qmax, qmin);
        output[i] = (int16_t)y;
    }

#endif

    return ARM_CMSIS_NN_SUCCESS;

}

/**
 * @} end of Doxygen group
 */
