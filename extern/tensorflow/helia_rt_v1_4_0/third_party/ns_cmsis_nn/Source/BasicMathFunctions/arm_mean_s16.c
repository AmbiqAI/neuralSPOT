/*
 * SPDX-FileCopyrightText: Copyright 2010-2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
 * Title:        arm_mean_s16
 * Description:  Mean reduction operator for quantized s16 tensors
 *
 * $Date:        18 June 2025
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
 * @addtogroup groupReduction
 * @{
 */

/**
 * @brief Generic fallback MEAN operator for quantized s16 tensors.
 *
 * @param[in]  input_data     Pointer to input tensor
 * @param[in]  input_dims     Input tensor dimensions (4D NHWC)
 * @param[in]  input_offset   Zero-point offset for input
 * @param[in]  axis_dims      4D binary axis mask (1 = reduce)
 * @param[out] output_data    Pointer to output buffer
 * @param[in]  output_dims    Output tensor dimensions
 * @param[in]  out_offset     Zero-point offset for output
 * @param[in]  out_mult       Quantized multiplier
 * @param[in]  out_shift      Quantized shift
 *
 * @return     ARM_CMSIS_NN_SUCCESS on success
 */
arm_cmsis_nn_status arm_mean_reduce_generic_s16(const int16_t *input_data,
                                                const cmsis_nn_dims *input_dims,
                                                int32_t input_offset,
                                                const cmsis_nn_dims *axis_dims,
                                                int16_t *output_data,
                                                const cmsis_nn_dims *output_dims,
                                                int32_t out_offset,
                                                int32_t out_mult,
                                                int32_t out_shift)
{
    const int N = input_dims->n;
    const int H = input_dims->h;
    const int W = input_dims->w;
    const int C = input_dims->c;

    const int out_N = output_dims->n;
    const int out_H = output_dims->h;
    const int out_W = output_dims->w;
    const int out_C = output_dims->c;

    const int N_limit = axis_dims->n ? N : 1;
    const int H_limit = axis_dims->h ? H : 1;
    const int W_limit = axis_dims->w ? W : 1;
    const int C_limit = axis_dims->c ? C : 1;

    const int count = N_limit * H_limit * W_limit * C_limit;
    const int32_t zp = input_offset * count;

    for (int n = 0; n < out_N; ++n)
    for (int h = 0; h < out_H; ++h)
    for (int w = 0; w < out_W; ++w)
    for (int c = 0; c < out_C; ++c)
    {
        int32_t acc = zp;

        for (int ni = 0; ni < N_limit; ++ni)
        for (int hi = 0; hi < H_limit; ++hi)
        for (int wi = 0; wi < W_limit; ++wi)
        for (int ci = 0; ci < C_limit; ++ci)
        {
            int idx_n = axis_dims->n ? ni : n;
            int idx_h = axis_dims->h ? hi : h;
            int idx_w = axis_dims->w ? wi : w;
            int idx_c = axis_dims->c ? ci : c;

            int flat_index = ((idx_n * H + idx_h) * W + idx_w) * C + idx_c;
            acc += input_data[flat_index];
        }

        acc = arm_nn_requantize(acc, out_mult, out_shift);
        acc += out_offset;
        acc = CLAMP(acc, 32767, -32768);

        int out_index = ((n * out_H + h) * out_W + w) * out_C + c;
        output_data[out_index] = (int16_t)acc;
    }

    return ARM_CMSIS_NN_SUCCESS;
}


arm_cmsis_nn_status
arm_mean_flatten_reduce_last_dims_s16(const int16_t *input_data,
                                      int32_t input_offset,
                                      int16_t *output_data,
                                      int32_t out_offset,
                                      int32_t out_mult,
                                      int32_t out_shift,
                                      int32_t outer_size,
                                      int32_t inner_size)
{
    const int32_t zp = input_offset * inner_size;

#if defined(ARM_MATH_MVEI)
    for (int i = 0; i < outer_size; ++i)
    {
        int32_t acc = zp;
        int32_t base = i * inner_size;

        int j = 0;
        for (; j <= inner_size - 8; j += 8)
        {
            int16x8_t v = vld1q_s16(&input_data[base + j]);
            acc = vaddvaq_s16(acc, v);
        }

        for (; j < inner_size; ++j)
        {
            acc += input_data[base + j];
        }

        acc = arm_nn_requantize(acc, out_mult, out_shift);
        acc += out_offset;
        acc = CLAMP(acc, 32767, -32768);

        output_data[i] = (int16_t)acc;
    }

#else

    // fallback to simple C implementation
    for (int i = 0; i < outer_size; ++i)
    {
        int32_t acc = zp;
        int32_t base = i * inner_size;
        for (int j = 0; j < inner_size; ++j)
        {
            acc += input_data[base + j];
        }

        acc  = arm_nn_requantize(acc, out_mult, out_shift);
        acc += out_offset;
        acc  = CLAMP(acc, 32767, -32768);
        output_data[i] = (int16_t)acc;
    }

#endif

    return ARM_CMSIS_NN_SUCCESS;
}

#if defined(ARM_MATH_MVEI)

arm_cmsis_nn_status
arm_mean_reduce_spatial_mve_s16(const int16_t *input_data,
                                const cmsis_nn_dims *input_dims,
                                int32_t input_offset,
                                int16_t *output_data,
                                int32_t out_offset,
                                int32_t out_mult,
                                int32_t out_shift)
{
    const int N       = input_dims->n;
    const int H       = input_dims->h;
    const int W       = input_dims->w;
    const int C       = input_dims->c;
    const int spatial = H * W;

    const int32_t zp = input_offset * spatial;

    for (int n = 0; n < N; ++n)
    {
        const int16_t *in_ptr  = input_data  + n * spatial * C;
              int16_t *out_ptr = output_data + n * C;

        int c = 0;
        // Process blocks of 8 channels
        for (; c <= C - 8; c += 8)
        {
            // 2 32-bit accumulators for 8 lanes
            int32x4_t acc0 = vdupq_n_s32(zp);
            int32x4_t acc1 = vdupq_n_s32(zp);

            // Sum over spatial dims
            for (int i = 0; i < spatial; ++i)
            {
                const int16_t *p = in_ptr + i * C + c;
                int16x8_t v = vld1q_s16(p);

                // Widen int16 to int32
                int32x4_t lo32 = vmovlbq_s16(v);
                int32x4_t hi32 = vmovltq_s16(v);

                acc0 = vaddq_s32(acc0, lo32);
                acc1 = vaddq_s32(acc1, hi32);
            }

            // Requantize (assumes 1/spatial already folded into out_mult/out_shift)
            acc0 = arm_requantize_mve(acc0, out_mult, out_shift);
            acc1 = arm_requantize_mve(acc1, out_mult, out_shift);

            // Add output zero-point
            acc0 = vaddq_n_s32(acc0, out_offset);
            acc1 = vaddq_n_s32(acc1, out_offset);

            // Narrow 32→16 (with saturation)
            int16x8_t outv = vdupq_n_s16(0);
            outv = vqmovnbq_s32(outv, acc0);
            outv = vqmovntq_s32(outv, acc1);

            vst1q_s16(out_ptr + c, outv);
        }

        // Scalar tail for channels < 8
        for (; c < C; ++c)
        {
            int32_t acc = zp;
            for (int i = 0; i < spatial; ++i)
            {
                acc += in_ptr[i * C + c];
            }
            // requantize
            acc  = arm_nn_requantize(acc, out_mult, out_shift);
            acc += out_offset;
            // clamp to int16
            acc = CLAMP(acc, 32767, -32768);
            out_ptr[c] = (int16_t)acc;
        }
    }

    return ARM_CMSIS_NN_SUCCESS;
}

#endif // ARM_MATH_MVEI

/*
 * s16 mean over the specified axis
 *
 * Refer header file for details.
 *
 */
arm_cmsis_nn_status arm_mean_s16(const int16_t *input_data,
                                 const cmsis_nn_dims *input_dims,
                                 const int32_t input_offset,
                                 const cmsis_nn_dims *axis_dims,
                                 int16_t *output_data,
                                 const cmsis_nn_dims *output_dims,
                                 const int32_t out_offset,
                                 const int32_t out_mult,
                                 const int32_t out_shift)
{

    // Validate input
    if (!input_data || !input_dims || !axis_dims || !output_data || !output_dims)
    {
        return ARM_CMSIS_NN_ARG_ERROR;
    }

    int32_t in_dims[4]   = { input_dims->n,
                             input_dims->h,
                             input_dims->w,
                             input_dims->c };
    int32_t axis_arr[4]  = { axis_dims->n ? 1 : 0,
                             axis_dims->h ? 1 : 0,
                             axis_dims->w ? 1 : 0,
                             axis_dims->c ? 1 : 0 };

    int32_t suffix_start = arm_reduce_get_flatten_suffix_start_from_arrays(in_dims, axis_arr);

    // Check for flatten reduction axis=[N,H,W,C]
    if (suffix_start >= 0)
    {
        // compute outer/inner
        int32_t outer_size = 1, inner_size = 1;

        for (int32_t d = 0; d < suffix_start; ++d) outer_size *= in_dims[d];
        for (int32_t d = suffix_start; d < 4;       ++d) inner_size *= in_dims[d];

        return arm_mean_flatten_reduce_last_dims_s16(input_data,
                                                    input_offset,
                                                    output_data,
                                                    out_offset,
                                                    out_mult,
                                                    out_shift,
                                                    outer_size,
                                                    inner_size);
    }

#if defined(ARM_MATH_MVEI)

    // Check for spatial reduction axis=[H,W]
    if (!axis_dims->n && axis_dims->h && axis_dims->w && !axis_dims->c)
    {
        return arm_mean_reduce_spatial_mve_s16(input_data,
                                              input_dims,
                                              input_offset,
                                              output_data,
                                              out_offset,
                                              out_mult,
                                              out_shift);
    }

#endif // ARM_MATH_MVEI

    // Fallback to general-purpose scalar implementation
    return arm_mean_reduce_generic_s16(input_data,
                                      input_dims,
                                      input_offset,
                                      axis_dims,
                                      output_data,
                                      output_dims,
                                      out_offset,
                                      out_mult,
                                      out_shift);

}

/**
 * @} end of Doxygen group
 */
