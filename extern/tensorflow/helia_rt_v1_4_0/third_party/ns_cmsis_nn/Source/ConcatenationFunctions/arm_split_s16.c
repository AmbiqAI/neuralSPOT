/*
 * SPDX-FileCopyrightText: Copyright 2024 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
 * Title:        arm_split_s16.c
 * Description:  Split s16 vectors
 *
 * $Date:        06 August 2026
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
 * @addtogroup Concatenation
 * @{
 */


arm_cmsis_nn_status arm_split_s16(const int16_t *input_data,
                                  const int32_t input_dims,
                                  const int32_t *input_shape,
                                  const int32_t axis,
                                  const int32_t num_splits,
                                  const int32_t *split_dims,
                                  int16_t *const *output_data)
{
    // Compute "outer" size: product of dims before 'axis'.
    int64_t outer_size = 1;
    for (int32_t d = 0; d < axis; d++)
    {
        outer_size *= input_shape[d];
    }
    // Compute base "inner" size: product of dims after 'axis'.
    int64_t base_inner_size = 1;
    for (int32_t d = axis + 1; d < input_dims; d++) {
        base_inner_size *= input_shape[d];
    }

    // For each outer index...
    for (int k = 0; k < outer_size; k++) {

        // For each output tensor (split)...
        for (int s = 0; s < num_splits; s++) {

            // The number of elements to copy for this output.
            const int copy_size = split_dims[s] * base_inner_size;
            int16_t *out_ptr = output_data[s] + k * copy_size;
            arm_memcpy_s16(out_ptr, input_data, copy_size);
            input_data += copy_size;
        }
    }

    return ARM_CMSIS_NN_SUCCESS;
}

/**
 * @} end of Concatenation group
 */
