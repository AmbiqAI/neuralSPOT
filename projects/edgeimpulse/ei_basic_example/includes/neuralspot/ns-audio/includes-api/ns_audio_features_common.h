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

#ifndef __NS_AUDIO_FEATURES_COMMON_H__
#define __NS_AUDIO_FEATURES_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "arm_math.h"
#include "string.h"

typedef float ns_fbank_t[][50];

typedef struct {
    uint8_t *arena_fbanks;
    uint32_t sample_frequency;
    uint32_t num_fbank_bins;
    uint32_t low_freq;
    uint32_t high_freq;
    uint32_t frame_len_pow2;
    int32_t *mfccFbankFirst;
    int32_t *mfccFbankLast;
    ns_fbank_t *melFBank;
} ns_fbanks_cfg_t;

extern void
ns_fbanks_init(ns_fbanks_cfg_t *c);
extern void
create_mel_fbank(ns_fbanks_cfg_t *cfg);

#ifdef __cplusplus
}
#endif
#endif
