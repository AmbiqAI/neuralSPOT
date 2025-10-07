/*
 * SPDX-FileCopyrightText: Copyright 2010-2024 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
 * Title:        arm_strided_slice_s8.c
 * Description:  StridedSlice function for s8 data compatible with TF Lite.
 *
 * $Date:        16 May 2025
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

//-----------------------------------------------------------------------------
// Inline helper: choose < or > based on stride sign
static inline bool stride_continue(int32_t idx, int32_t stop, int32_t stride)
{
    return (stride > 0) ? (idx < stop) : (idx > stop);
}


/**
 * @addtogroup StridedSlice
 * @{
 */

/*
 * S8 strided slice function for TensorFlow Lite
 *
 * Refer header file for details.
 *
 */
arm_cmsis_nn_status arm_strided_slice_s8(const int8_t *input_data,
                                         int8_t *output_data,
                                         const cmsis_nn_dims *const  input_dims,
                                         const cmsis_nn_dims *const  begin_dims,
                                         const cmsis_nn_dims *const  stride_dims,
                                         const cmsis_nn_dims *const  output_dims)
{
    // Precompute dims
    const int32_t in_h = input_dims->h,
                  in_w = input_dims->w,
                  in_c = input_dims->c;
    const int32_t b_n = begin_dims->n,
                  b_h = begin_dims->h,
                  b_w = begin_dims->w,
                  b_c = begin_dims->c;
    const int32_t s_n = stride_dims->n,
                  s_h = stride_dims->h,
                  s_w = stride_dims->w,
                  s_c = stride_dims->c;
    const int32_t o_n = output_dims->n,
                  o_h = output_dims->h,
                  o_w = output_dims->w,
                  o_c = output_dims->c;

    const int32_t plane_elems = in_h * in_w * in_c;  // H×W×C
    const int32_t slice_elems = in_w * in_c;         // W×C
    const int32_t row_elems   = in_c;                // C

    //---------------------------------------
    // Case 1: whole-slab copy: only slice N, full H/W/C
    if (s_n==1 && s_h==1 && s_w==1 && s_c==1
     && b_h==0     && o_h==in_h
     && b_w==0     && o_w==in_w
     && b_c==0     && o_c==in_c)
    {
        int32_t offset = b_n * plane_elems;
        int32_t total  = o_n * plane_elems;
        arm_memcpy_s8(output_data,
                      input_data + offset,
                      total * sizeof(int8_t));
        return ARM_CMSIS_NN_SUCCESS;
    }

    //---------------------------------------
    // Case 2: per‐batch slice: slice N & H, full W/C
    //   copy exactly o_h rows of W×C for each batch‐slice
    if (s_h == 1 && s_w == 1 && s_c == 1
     && b_w == 0 && o_w == in_w
     && b_c == 0 && o_c == in_c)
    {
        // slice_elems = in_w * in_c  (W×C)
        // batch_block = number of elements per batch‐slice = o_h × W×C
        int32_t batch_block = o_h * slice_elems;
        for (int32_t n = 0; n < o_n; n++)
        {
            // compute start of that batch‐window:
            //   n‐offset into planes + b_h‐offset into rows
            int32_t start = (b_n + n * s_n) * plane_elems
                          +  b_h            * slice_elems;
            arm_memcpy_s8(
              output_data + n * batch_block,
              input_data  + start,
              batch_block * sizeof(int8_t)
            );
        }
        return ARM_CMSIS_NN_SUCCESS;
    }

    //---------------------------------------
    // Case 3: per‐row slice: stride in N/H, full channels,
    //         but possibly partial width → copy only o_w columns
    if (s_w == 1 && s_c == 1
     && b_c == 0 && o_c == in_c)
    {
        int32_t out_idx   = 0;
        int32_t row_block = o_w * in_c;   // <-- use slice width, not full width
        for (int32_t n = 0; n < o_n; n++)
        {
            int32_t base_n = (b_n + n * s_n) * plane_elems;
            for (int32_t h = 0; h < o_h; h++)
            {
                // start of the row slice in the big buffer:
                int32_t start = base_n
                              + (b_h + h * s_h) * slice_elems
                              + b_w * in_c;   // row_elems == in_c

                // copy only o_w*in_c elements (i.e. the slice’s columns)
                arm_memcpy_s8(
                  output_data + out_idx,
                  input_data  + start,
                  row_block * sizeof(int8_t)
                );
                out_idx += row_block;
            }
        }
        return ARM_CMSIS_NN_SUCCESS;
    }

    //---------------------------------------
    // Case 4: per-pixel channel slice
    if (s_c == 1)
    {
        int32_t out_idx = 0;
        for (int32_t n = 0; n < o_n; n++)
        {
            int32_t base_n = (b_n + n*s_n) * plane_elems;
            for (int32_t h = 0; h < o_h; h++)
            {
                int32_t base_h = base_n
                               + (b_h + h*s_h)*slice_elems;
                for (int32_t w = 0; w < o_w; w++)
                {
                    int32_t start = base_h
                                  + (b_w + w*s_w)*row_elems
                                  + b_c;
                    arm_memcpy_s8(output_data + out_idx,
                                  input_data  + start,
                                  o_c * sizeof(int8_t));
                    out_idx += o_c;
                }
            }
        }
        return ARM_CMSIS_NN_SUCCESS;
    }

    //---------------------------------------
    // General case: truly arbitrary strides (including negative)
    {
        const int32_t stop0 = b_n + s_n * o_n;
        const int32_t stop1 = b_h + s_h * o_h;
        const int32_t stop2 = b_w + s_w * o_w;
        const int32_t stop3 = b_c + s_c * o_c;

        int32_t out_idx = 0;
        for (int32_t off0 = b_n; stride_continue(off0, stop0, s_n); off0 += s_n)
        {
            for (int32_t off1 = b_h; stride_continue(off1, stop1, s_h); off1 += s_h)
            {
                for (int32_t off2 = b_w; stride_continue(off2, stop2, s_w); off2 += s_w)
                {
                    for (int32_t off3 = b_c; stride_continue(off3, stop3, s_c); off3 += s_c)
                    {
                        const int32_t idx =
                            off0 * plane_elems +
                            off1 * slice_elems +
                            off2 * row_elems +
                            off3;
                        output_data[out_idx++] = input_data[idx];
                    }
                }
            }
        }
    }

    return ARM_CMSIS_NN_SUCCESS;
}

/**
 * @} end of StridedSlice group
 */
