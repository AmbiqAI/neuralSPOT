/**
 * @file ns_audio_mfcc.h
 * @author Carlos Morales
 * @brief Confifurable MFCC Calculator
 * @version 0.1
 * @date 2022-08-12
 * 
 * This MFCC utility is adapted from Arm's example C++ implementation.
 * It can be configured via DEFINES, but better config support is planned.
 * 
 * @copyright Copyright (c) 2022
 * 
 *  \addtogroup NeuralSPOT-MFCC
 *  @{
 *  @ingroup NeuralSPOT-Audio
 */

/*
 * Copyright (C) 2018 Arm Limited or its affiliates. All rights reserved.
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

#ifndef __AM_MFCC_H__
#define __AM_MFCC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "arm_math.h"
#include "string.h"

#ifndef MFCC_OVERRIDE_DEFAULTS
    #define MFCC_SAMP_FREQ 16000
    #define MFCC_NUM_FBANK_BINS 40
    #define MFCC_MEL_LOW_FREQ 20
    #define MFCC_MEL_HIGH_FREQ 8000
    #define MFCC_NUM_FRAMES 100
    #define MFCC_NUM_MFCC_COEFFS 13
    #define MFCC_NUM_MFCC_FEATURES MFCC_NUM_MFCC_COEFFS
    #define MFCC_MFCC_DEC_BITS 4
    #define MFCC_FRAME_SHIFT_MS 30
    #define MFCC_FRAME_SHIFT ((int16_t)(SAMP_FREQ * 0.001 * MFCC_FRAME_SHIFT_MS))
    #define MFCC_MFCC_BUFFER_SIZE (MFCC_NUM_FRAMES * MFCC_NUM_MFCC_COEFFS)
    #define MFCC_FRAME_LEN_MS 30
    #define MFCC_FRAME_LEN 480 // ((int16_t)(SAMP_FREQ * 0.001 * FRAME_LEN_MS))
    #define MFCC_FRAME_LEN_POW2 512
#endif

#define M_2PI 6.283185307179586476925286766559005
#ifndef M_PI
    #define M_PI 3.14159265358979323846264338328
#endif

#ifdef MFCC_DEBUG
extern float g_audioMax;
extern float g_audioMin;
extern float g_audioSum;
extern float g_audioMaxInt;
extern float g_audioMinInt;
extern float g_audioSumInt;
#endif

extern void
ns_mfcc_init(void);
extern void
ns_mfcc_compute(const int16_t *audio_data, float *mfcc_out);

#ifdef __cplusplus
}
#endif
#endif
