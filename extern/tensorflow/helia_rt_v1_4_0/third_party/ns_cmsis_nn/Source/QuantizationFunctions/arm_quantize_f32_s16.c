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
 * Title:        arm_quantize_f32_s16.c
 * Description:  float32 to int16 Quantization
 *
 * $Date:        15 April 2025
 * $Revision:    V.1.0.0
 *
 * Target :  Arm(R) M-Profile Architecture
 *
 * -------------------------------------------------------------------- */
#include "arm_nnsupportfunctions.h"
#include "arm_nnfunctions.h"
#include <math.h>

/**
 *  @ingroup Public
 */

/**
 * @addtogroup Quantization
 * @{
 */

/*
 * float32_t to int16_t quantization function.
 *
 * Refer header file for details.
 *
 */
arm_cmsis_nn_status
arm_quantize_f32_s16(
    const float* input,
    int16_t* output,
    int32_t size,
    int32_t zero_point,
    float scale
) {

#if defined(ARM_MATH_MVEI)
    int32_t count = (size + 3) / 4;
    float mul_scale = 1.0f / scale;
    int32x4_t max = vdupq_n_s32(INT16_MAX);
    int32x4_t min = vdupq_n_s32(INT16_MIN);
    for (int i = 0; i < count; i++) {
        mve_pred16_t pred = vctp32q(size);
        size -= 4;
        float32x4_t vals = vldrwq_z_f32(input, pred);
        int32x4_t rounded = vcvtaq_s32_f32(vrndnq_f32(vmulq_n_f32(vals, mul_scale)));
        int32x4_t shifted = vaddq_n_s32(rounded, zero_point);
        int32x4_t clamped = vminq_s32(vmaxq_s32(shifted, min), max);
        vstrhq_p_s32(output, clamped, pred);
        input += 4;
        output += 4;
    }
#else
    for (int i = 0; i < size; i++) {
        float val = input[i];
        int32_t rounded = (int32_t)(round(val / scale)) + zero_point;
        int32_t clamped = CLAMP(rounded, INT16_MAX, INT16_MIN);
        output[i] = (int16_t)(clamped);
    }
#endif

    return ARM_CMSIS_NN_SUCCESS;

}

/**
 * @} end of Quantization group
 */
