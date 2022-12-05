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
#include "ns_audio_features_common.h"
#include "string.h"

typedef struct {
    uint8_t *arena;
    uint32_t sample_frequency;
    uint32_t num_fbank_bins;
    uint32_t low_freq;
    uint32_t high_freq;
    uint32_t num_frames;
    uint32_t num_coeffs;
    uint32_t num_dec_bits;
    uint32_t frame_shift_ms; ///< Not used
    uint32_t frame_len_ms;   ///< Not used
    uint32_t frame_len;
    uint32_t frame_len_pow2;
    float *mfccFrame;
    float *mfccBuffer;
    float *mfccEnergies;
    float *mfccWindowFunction;
    float *mfccDCTMatrix;
    ns_fbanks_cfg_t fbc;
} ns_mfcc_cfg_t;

#define NS_MFCC_SIZEBINS 53

// Arena should be enough to accomodate the various buffers
// e.g. MFCC_ARENA_SIZE  32*(MY_MFCC_FRAME_LEN_POW2*2 +
// MY_MFCC_NUM_FBANK_BINS*(NS_MFCC_SIZEBINS+MY_MFCC_NUM_MFCC_COEFFS)) where '32' is size of float
// and int32_t

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
ns_mfcc_init(ns_mfcc_cfg_t *c);
extern void
ns_mfcc_compute(ns_mfcc_cfg_t *c, const int16_t *audio_data, float *mfcc_out);

#ifdef __cplusplus
}
#endif
#endif
