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
 * Description:  Clamp activation for int8_t data type
 *
 * $Date:        17 September 2025
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
* Clamp activation functions for int8_t data type.
*
* Refer header file for details.
*
*/
arm_cmsis_nn_status arm_clamp_s8(
    const int8_t *input,
    const int8_t act_min,
    const int8_t act_max,
    int8_t *output,
    const int32_t output_size
) {

    int32_t flat_size = output_size;

#if defined(ARM_MATH_MVEI)

    uint32_t blkCnt = (flat_size + 15) / 16;
    int8x16_t qmin = vdupq_n_s8(act_min);
    int8x16_t qmax = vdupq_n_s8(act_max);

    while (blkCnt > 0U)
    {
        // Load
        mve_pred16_t pred = vctp8q((uint32_t)flat_size);
        int8x16_t res = vldrbq_z_s8(input, pred);
        // Clamp
        res = vmaxq_s8(res, qmin);
        res = vminq_s8(res, qmax);
        // Store
        vstrbq_p_s8(output, res, pred);
        // Increment pointers
        input += 16;
        output += 16;
        blkCnt -= 1;
        flat_size -= 16;
    }

#else

    for (int i = 0; i < flat_size; ++i)
    {
        output[i] = CLAMP(input[i], act_max, act_min);
    }

#endif

    return ARM_CMSIS_NN_SUCCESS;
}

/**
 * @} end of Doxygen group
 */
