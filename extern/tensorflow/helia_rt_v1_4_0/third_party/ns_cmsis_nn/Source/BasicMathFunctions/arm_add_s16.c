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
 * Title:        arm_add_s16
 * Description:  Elementwise add w/ support for broadcasting and scalar
 *
 * $Date:        11 June 2025
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
 * @addtogroup groupElementwise
 * @{
 */

/*
 * s16 elementwise add w/ support for broadcasting and scalar
 *
 * Refer header file for details.
 *
 */
arm_cmsis_nn_status arm_add_s16(const int16_t *input1_data,
                               const cmsis_nn_dims *input1_dims,
                               const int16_t *input2_data,
                               const cmsis_nn_dims *input2_dims,
                               const int32_t input1_offset,
                               const int32_t input1_mult,
                               const int32_t input1_shift,
                               const int32_t input2_offset,
                               const int32_t input2_mult,
                               const int32_t input2_shift,
                               const int32_t left_shift,
                               int16_t *output_data,
                               const cmsis_nn_dims *output_dims,
                               const int32_t out_offset,
                               const int32_t out_mult,
                               const int32_t out_shift,
                               const int32_t out_activation_min,
                               const int32_t out_activation_max)
{

    // Unpack dims for convenience
    const int32_t out_n = output_dims->n, out_h = output_dims->h;
    const int32_t out_w = output_dims->w;

    const int32_t in1_n = input1_dims->n, in1_h = input1_dims->h;
    const int32_t in1_w = input1_dims->w, in1_c = input1_dims->c;

    const int32_t in2_n = input2_dims->n, in2_h = input2_dims->h;
    const int32_t in2_w = input2_dims->w, in2_c = input2_dims->c;

    // Total element‐counts
    int32_t flat1_total = in1_n * in1_h * in1_w * in1_c;
    int32_t flat2_total = in2_n * in2_h * in2_w * in2_c;

    // 1) No broadcast at all? (identical sizes)
    if (!arm_check_broadcast_required(input1_dims, input2_dims))
    {
        return arm_elementwise_add_s16(
            input1_data, input2_data,
            input1_offset, input1_mult, input1_shift,
            input2_offset, input2_mult, input2_shift,
            left_shift,
            output_data,
            out_offset, out_mult, out_shift,
            out_activation_min, out_activation_max,
            flat1_total
        );
    }

    // 2) Whole‐tensor scalar?
    if (flat1_total == 1)
    {
        return arm_add_scalar_s16(
            input1_data, input2_data,
            input1_offset, input1_mult, input1_shift,
            input2_offset, input2_mult, input2_shift,
            left_shift,
            output_data,
            out_offset, out_mult, out_shift,
            out_activation_min, out_activation_max,
            flat2_total
        );
    }
    if (flat2_total == 1)
    {
        return arm_add_scalar_s16(
            input2_data, input1_data,
            input2_offset, input2_mult, input2_shift,
            input1_offset, input1_mult, input1_shift,
            left_shift,
            output_data,
            out_offset, out_mult, out_shift,
            out_activation_min, out_activation_max,
            flat1_total
        );
    }

    // 3) Full N/H/W/C broadcast sweep
    // Compute “rewind” and “advance” offsets same as arm_maximum_s16
    const int32_t wd1 = (in1_w >= in2_w) ? 0 : in1_c;
    const int32_t wd2 = (in2_w >= in1_w) ? 0 : in2_c;
    const int32_t hd1 = (in1_h >= in2_h) ? wd1 : -in1_w  * (in1_c - wd1);
    const int32_t hd2 = (in2_h >= in1_h) ? wd2 : -in2_w  * (in2_c - wd2);
    const int32_t bd1 = (in1_n >= in2_n) ? in1_h * in1_w * in1_c : 0;
    const int32_t bd2 = (in2_n >= in1_n) ? in2_h * in2_w * in2_c : 0;

    for (int b = 0; b < out_n; b++)
    {
        // Per‐batch pointers
        const int16_t *p1 = input1_data;
        const int16_t *p2 = input2_data;

        // Recompute per‐batch flat sizes
        flat1_total = in1_h * in1_w * in1_c;
        flat2_total = in2_h * in2_w * in2_c;

        // Batch‐level no‐broadcast?
        if (in1_h == in2_h && in1_w == in2_w && in1_c == in2_c)
        {
            arm_elementwise_add_s16(
                p1, p2,
                input1_offset, input1_mult, input1_shift,
                input2_offset, input2_mult, input2_shift,
                left_shift,
                output_data,
                out_offset, out_mult, out_shift,
                out_activation_min, out_activation_max,
                flat1_total
            );
            output_data += flat1_total;
        }
        else
        {
            // Sweep height
            flat1_total = in1_w * in1_c;
            flat2_total = in2_w * in2_c;
            for (int h = 0; h < out_h; h++)
            {
                // A) Row‐level no‐broadcast
                if (in1_w == in2_w && in1_c == in2_c)
                {
                    arm_elementwise_add_s16(
                        p1, p2,
                        input1_offset, input1_mult, input1_shift,
                        input2_offset, input2_mult, input2_shift,
                        left_shift,
                        output_data,
                        out_offset, out_mult, out_shift,
                        out_activation_min, out_activation_max,
                        flat1_total
                    );
                    p1 += flat1_total;
                    p2 += flat1_total;
                    output_data += flat1_total;
                }
                // B) scalar‐broadcast on input1
                else if (flat1_total == 1)
                {
                    arm_add_scalar_s16(
                        p1, p2,
                        input1_offset, input1_mult, input1_shift,
                        input2_offset, input2_mult, input2_shift,
                        left_shift,
                        output_data,
                        out_offset, out_mult, out_shift,
                        out_activation_min, out_activation_max,
                        flat2_total
                    );
                    p1++;
                    p2 += flat2_total;
                    output_data += flat2_total;
                }
                // C) scalar‐broadcast on input2
                else if (flat2_total == 1)
                {
                    arm_add_scalar_s16(
                        p2, p1,
                        input2_offset, input2_mult, input2_shift,
                        input1_offset, input1_mult, input1_shift,
                        left_shift,
                        output_data,
                        out_offset, out_mult, out_shift,
                        out_activation_min, out_activation_max,
                        flat1_total
                    );
                    p1 += flat1_total;
                    p2++;
                    output_data += flat1_total;
                }
                // D) per‐pixel broadcast sweep
                else
                {
                    for (int w = 0; w < out_w; w++)
                    {
                        // 1) per‐pixel no‐broadcast
                        if (in1_c == in2_c)
                        {
                            arm_elementwise_add_s16(
                                p1, p2,
                                input1_offset, input1_mult, input1_shift,
                                input2_offset, input2_mult, input2_shift,
                                left_shift,
                                output_data,
                                out_offset, out_mult, out_shift,
                                out_activation_min, out_activation_max,
                                in1_c
                            );
                            p1 += in1_c;
                            p2 += in1_c;
                            output_data += in1_c;
                        }
                        // 2) scalar‐broadcast1
                        else if (in1_c == 1)
                        {
                            arm_add_scalar_s16(
                                p1, p2,
                                input1_offset, input1_mult, input1_shift,
                                input2_offset, input2_mult, input2_shift,
                                left_shift,
                                output_data,
                                out_offset, out_mult, out_shift,
                                out_activation_min, out_activation_max,
                                in2_c
                            );
                            p1++;
                            p2 += in2_c;
                            output_data += in2_c;
                        }
                        // 3) scalar‐broadcast2
                        else  // in2_c == 1
                        {
                            arm_add_scalar_s16(
                                p2, p1,
                                input2_offset, input2_mult, input2_shift,
                                input1_offset, input1_mult, input1_shift,
                                left_shift,
                                output_data,
                                out_offset, out_mult, out_shift,
                                out_activation_min, out_activation_max,
                                in1_c
                            );
                            p1 += in1_c;
                            p2++;
                            output_data += in1_c;
                        }
                        // rewind for next width
                        p1 -= wd1;
                        p2 -= wd2;
                    }
                }
                // advance row
                p1 += hd1;
                p2 += hd2;
            }
        }

        // advance batch
        input1_data += bd1;
        input2_data += bd2;
    }

    return ARM_CMSIS_NN_SUCCESS;
}

/**
 * @} end of Doxygen group
 */
