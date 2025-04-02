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
 * Title:        arm_nnactivations.h
 * Description:  Public header file for CMSIS NN Library
 *
 * $Date:        04 November 2024
 * $Revision:    V.18.0.0
 *
 * Target :  Arm(R) M-Profile Architecture
 * -------------------------------------------------------------------- */

/**
 * @defgroup Public Public
 * A collection of functions to perform basic activations for neural network layers. Functions with a _s8 suffix support
 * TensorFlow Lite framework.
 */

#ifndef ARM_NNACTIVATIONS_H
#define ARM_NNACTIVATIONS_H

#include "arm_nn_math_types.h"
#include "arm_nn_types.h"

#define USE_INTRINSIC

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief S8 Leaky ReLU activation function
 *
 * @param[in]      input                       Pointer to the input buffer
 * @param[in]      input_offset                Input tensor zero offset
 * @param[in]      output_offset               Output tensor zero offset
 * @param[in]      output_multiplier_alpha     Output multiplier for the alpha parameter
 * @param[in]      output_shift_alpha          Output shift for the alpha parameter
 * @param[in]      output_multiplier_identity  Output multiplier for the identity parameter
 * @param[in]      output_shift_identity       Output shift for the identity parameter
 * @param[out]     output                      Pointer to the output buffer
 * @param[in]      output_size                 Number of elements in the input tensor
 * @return         The function returns ARM_MATH_SUCCESS
 *
 */
arm_cmsis_nn_status arm_leaky_relu_s8(const int8_t *input,
                                      const int32_t input_offset,
                                      const int32_t output_offset,
                                      const int32_t output_multiplier_alpha,
                                      const int32_t output_shift_alpha,
                                      const int32_t output_multiplier_identity,
                                      const int32_t output_shift_identity,
                                      int8_t *output,
                                      const int32_t output_size);

/**
 * @brief S16 Leaky ReLU activation function
 *
 * @param[in]      input                       Pointer to the input buffer
 * @param[in]      input_offset                Input tensor zero offset
 * @param[in]      output_offset               Output tensor zero offset
 * @param[in]      output_multiplier_alpha     Output multiplier for the alpha parameter
 * @param[in]      output_shift_alpha          Output shift for the alpha parameter
 * @param[in]      output_multiplier_identity  Output multiplier for the identity parameter
 * @param[in]      output_shift_identity       Output shift for the identity parameter
 * @param[out]     output                      Pointer to the output buffer
 * @param[in]      output_size                 Number of elements in the input tensor
 * @return         The function returns ARM_MATH_SUCCESS
 *
 */
arm_cmsis_nn_status arm_leaky_relu_s16(const int16_t *input,
                                       const int32_t input_offset,
                                       const int32_t output_offset,
                                       const int32_t output_multiplier_alpha,
                                       const int32_t output_shift_alpha,
                                       const int32_t output_multiplier_identity,
                                       const int32_t output_shift_identity,
                                       int16_t *output,
                                       const int32_t output_size);

/**
 * @brief Logistic activation function for s16
 * @param[in]  input              Pointer to the input tensor
 * @param[out] output             Pointer to the output tensor
 * @param[in]  input_size         Number of elements in the input tensor
 * @param[in]  input_multiplier   Input quantization multiplier
 * @param[in]  input_left_shift   Input quantization shift within the range [0, 31]
 * @return                        The function returns
 *                                    <code>ARM_CMSIS_NN_ARG_ERROR</code> Argument error check failed
 *                                    <code>ARM_CMSIS_NN_SUCCESS</code> - Successful operation
 */
arm_cmsis_nn_status arm_logistic_s16(int16_t *input,
                                     int16_t *output,
                                     const int32_t input_size,
                                     int32_t input_multiplier,
                                     int32_t input_left_shift);

#ifdef __cplusplus
}
#endif

#endif /* ARM_NNACTIVATIONS_H */
