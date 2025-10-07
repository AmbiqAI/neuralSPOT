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
 * Title:        arm_quantize_s8_s8.c
 * Description:  int8 to int8 requantization
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
 * int8_t to int8_t requantization function.
 *
 * Refer header file for details.
 *
 */
arm_cmsis_nn_status
arm_requantize_s8_s8(
  const int8_t *input,
  int8_t* output,
  int32_t size,
  int32_t effective_scale_multiplier,
  int32_t effective_scale_shift,
  int32_t input_zeropoint,
  int32_t output_zeropoint
) {

#if defined(ARM_MATH_MVEI)
    int32_t count = (size + 3) / 4;
    int32x4_t max = vdupq_n_s32(INT8_MAX);
    int32x4_t min = vdupq_n_s32(INT8_MIN);
    for (int i = 0; i < count; i++) {
        mve_pred16_t pred = vctp32q(size);
        size -= 4;
        int32x4_t vals = vldrbq_z_s32(input, pred);
        vals = vaddq_n_s32(vals, -input_zeropoint);
        vals = arm_requantize_mve(vals, effective_scale_multiplier, effective_scale_shift);
        int32x4_t shifted = vaddq_n_s32(vals, output_zeropoint);
        int32x4_t clamped = vminq_s32(vmaxq_s32(shifted, min), max);
        vstrbq_p_s32(output, clamped, pred);
        input += 4;
        output += 4;
    }
#else
    for (int i = 0; i < size; i++) {
        int32_t val = input[i] - input_zeropoint;
        val = arm_nn_requantize(val, effective_scale_multiplier, effective_scale_shift);
        val += output_zeropoint;
        output[i] = CLAMP(val, INT8_MAX, INT8_MIN);
    }
#endif

    return ARM_CMSIS_NN_SUCCESS;

}
/**
 * @} end of Dequantization group
 */
