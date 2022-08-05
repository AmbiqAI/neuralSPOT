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

#define SAMP_FREQ 16000
#define NUM_FBANK_BINS 40
#define MEL_LOW_FREQ 20
#define MEL_HIGH_FREQ 8000

#ifndef FRAME_LEN
    #define NUM_FRAMES 100
    #define NUM_MFCC_COEFFS 13
    #define NUM_MFCC_FEATURES NUM_MFCC_COEFFS
    #define MFCC_DEC_BITS 4
    #define FRAME_SHIFT_MS 30
    #define FRAME_SHIFT ((int16_t)(SAMP_FREQ * 0.001 * FRAME_SHIFT_MS))
    #define MFCC_BUFFER_SIZE (NUM_FRAMES * NUM_MFCC_COEFFS)
    #define FRAME_LEN_MS 30
    #define FRAME_LEN 480 // ((int16_t)(SAMP_FREQ * 0.001 * FRAME_LEN_MS))
    #define FRAME_LEN_POW2 512
#endif

#define M_2PI 6.283185307179586476925286766559005

#ifndef M_PI
    #define M_PI 3.14159265358979323846264338328
#endif

extern float g_audioMax;
extern float g_audioMin;
extern float g_audioSum;
extern float g_audioMaxInt;
extern float g_audioMinInt;
extern float g_audioSumInt;

extern void
mfcc_init(void);
extern void
mfcc_compute(const int16_t *audio_data, float *mfcc_out);

#ifdef __cplusplus
}
#endif
#endif
