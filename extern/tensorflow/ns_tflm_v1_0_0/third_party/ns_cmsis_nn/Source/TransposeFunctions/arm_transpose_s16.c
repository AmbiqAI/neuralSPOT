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
 * Title:        arm_transpose_s8.c
 * Description:  Transpose a s8 vector
 *
 * $Date:        30 October 2024
 * $Revision:    V.1.0.1
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
 * @addtogroup Transpose
 * @{
 */

/*
 * Transpose function specialized for int16 data in a NHCW (batch, height, channels, width)
 * arrangement. This version uses MVE intrinsics when available.
 */
static arm_cmsis_nn_status arm_transpose_s16_nhcw(const int16_t *input,
                                                 int16_t *const output,
                                                 const cmsis_nn_dims *const input_dims,
                                                 const int32_t *const in_strides,
                                                 const int32_t *const out_strides)
{
    const int32_t n = input_dims->n;
    const int32_t h = input_dims->h;
    const int32_t w = input_dims->w;
    const int32_t c = input_dims->c;

    const int16_t *input_n = input;
    int16_t *output_n = output;

    /* Here we interpret the source as a 2D matrix of shape:
       src_rows = w (number of elements per row)
       src_cols = c (number of columns)
       (this arrangement matches the expected layout for the optimized transpose)
    */
    const uint16_t src_rows = (uint16_t)w;
    const uint16_t src_cols = (uint16_t)c;

#if defined(ARM_MATH_MVEI)
    uint16x8_t vec_offsets;
    uint16x8_t vec_input;

    /* Create a vector of indices [0, 1, 2, ..., 7] and multiply by src_cols */
    vec_offsets = vidupq_u16((uint32_t)0, 1);
    vec_offsets = vec_offsets * src_cols;
#endif

    for (int32_t i = 0; i < n; i++)
    {
        const int16_t *input_h = input_n;
        int16_t *output_h = output_n;

        for (int32_t y = 0; y < h; y++)
        {

#if defined(ARM_MATH_MVEI)
            /* For the current row, work on each column of the source matrix */
            const uint16_t *input_c = (const uint16_t *)input_h;
            uint16_t *output_c = (uint16_t *)output_h;

            for (int32_t z = 0; z < src_cols; z++)
            {
                uint16_t const *input_w = (uint16_t const *)input_c;
                uint16_t *output_w = (uint16_t *)output_c;

                int32_t block_count = src_rows;
                while (block_count > 0)
                {
                    mve_pred16_t p = vctp16q(block_count);
                    /* Gather 8 int16 values from input_w using the computed byte offsets */
                    vec_input = vldrhq_gather_shifted_offset_z_u16(input_w, vec_offsets, p);
                    /* Store the gathered vector to output_w with predication */
                    vstrhq_p_u16(output_w, vec_input, p);

                    /* Advance the input pointer by 8 rows. Since each row is src_cols int16 values,
                       we add (src_cols * 8) elements. The output pointer is advanced by 8 elements. */
                    input_w = input_w + src_cols * 8;
                    output_w += 8;
                    block_count -= 8;
                }

                input_c++;  /* Next column */
                output_c += src_rows; /* Advance output pointer by the number of rows */
            }
#else
            const uint16_t *input_w = (const uint16_t *)input_h;
            uint16_t *output_w = (uint16_t *)output_h;

            for (int32_t src_row_i = 0; src_row_i < src_rows; src_row_i++)
            {
                output_w = (uint16_t *)output + src_row_i;

                for (int32_t x = 0; x < src_cols; x++)
                {
                    *output_w = *input_w++;
                    output_w += src_rows;
                }
            }
#endif
            input_h += in_strides[1];
            output_h += out_strides[1];
        }
        input_n += in_strides[0];
        output_n += out_strides[0];
    }

    return ARM_CMSIS_NN_SUCCESS;
}

static arm_cmsis_nn_status arm_transpose_s16_default(const int16_t *input,
                                                    int16_t *const output,
                                                    const cmsis_nn_dims *const input_dims,
                                                    const int32_t *const in_strides,
                                                    const int32_t *const out_strides)
{
    const int32_t n = input_dims->n;
    const int32_t h = input_dims->h;
    const int32_t w = input_dims->w;
    const int32_t c = input_dims->c;

    for (int32_t i = 0; i < n; i++)
    {
        for (int32_t y = 0; y < h; y++)
        {
            for (int32_t x = 0; x < w; x++)
            {
                for (int32_t z = 0; z < c; z++)
                {
                    const int32_t from_index =
                        i * in_strides[0] + y * in_strides[1] + x * in_strides[2] + z * in_strides[3];

                    const int32_t to_index =
                        i * out_strides[0] + y * out_strides[1] + x * out_strides[2] + z * out_strides[3];

                    output[to_index] = input[from_index];
                }
            }
        }
    }

    return ARM_CMSIS_NN_SUCCESS;
}

/*
 * Basic s16 transpose function.
 *
 * Refer header file for details.
 *
 */
arm_cmsis_nn_status arm_transpose_s16(const int16_t *input,
                                     int16_t *const output,
                                     const cmsis_nn_dims *const input_dims,
                                     const cmsis_nn_dims *const output_dims,
                                     const cmsis_nn_transpose_params *const transpose_params)
{
    int32_t in_strides[4];
    int32_t out_strides[4] = {0};

    const uint32_t *const perm = transpose_params->permutations;

    const int32_t n = input_dims->n;
    const int32_t h = input_dims->h;
    const int32_t w = input_dims->w;
    const int32_t c = input_dims->c;

    in_strides[0] = h * w * c;
    in_strides[1] = w * c;
    in_strides[2] = c;
    in_strides[3] = 1;

    if (transpose_params->num_dims == 1)
    {
        arm_memcpy_s16(output, input, input_dims->n);

        return ARM_CMSIS_NN_SUCCESS;
    }
    else if (transpose_params->num_dims == 2)
    {
        const cmsis_nn_dims smaller_input_dims = {1, 1, n, h};

        return arm_transpose_s16_nhcw(input, output, &smaller_input_dims, in_strides, out_strides);
    }
    else if (transpose_params->num_dims == 3)
    {
        const cmsis_nn_dims smaller_input_dims = {1, n, h, w};

        in_strides[0] = 0;
        in_strides[1] = h * w;
        in_strides[2] = w;
        in_strides[3] = 1;

        if (perm[0] > 2 || perm[1] > 2 || perm[2] > 2)
        {
            return ARM_CMSIS_NN_ARG_ERROR;
        }

        out_strides[0] = 0;
        out_strides[perm[0] + 1] = output_dims->h * output_dims->w;
        out_strides[perm[1] + 1] = output_dims->w;
        out_strides[perm[2] + 1] = 1;

        return arm_transpose_s16_default(input, output, &smaller_input_dims, in_strides, out_strides);
    }

    if (perm[0] > 3 || perm[1] > 3 || perm[2] > 3 || perm[3] > 3)
    {
        return ARM_CMSIS_NN_ARG_ERROR;
    }

    out_strides[perm[0]] = output_dims->h * output_dims->w * output_dims->c;
    out_strides[perm[1]] = output_dims->w * output_dims->c;
    out_strides[perm[2]] = output_dims->c;
    out_strides[perm[3]] = 1;

#if defined(ARM_MATH_MVEI)
    if (perm[0] == 0 && perm[1] == 1)
    {
        return arm_transpose_s16_nhcw(input, output, input_dims, in_strides, out_strides);
    }
#endif

    return arm_transpose_s16_default(input, output, input_dims, in_strides, out_strides);
}

/**
 * @} end of Transpose group
 */
