/**
 * @file ns_features_common.c
 * @author Carlos Morales
 * @brief Based on original Arm version, see below
 * @version 0.1
 * @date 2022-08-09
 *
 * @copyright Copyright (c) 2022
 *
 */

/*
 * MFCC utlities - based on the original C++ version by Arm
 *  this code has been refactored into C with no malloc()
 *
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

/*
 * Description: MFCC feature extraction to match with TensorFlow MFCC Op
 */

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"

#include "float.h"
#include "ns_audio_features_common.h"
#include <string.h>

// int32_t g_mfccFbankFirst[MFCC_NUM_FBANK_BINS];
// int32_t g_mfccFbankLast[MFCC_NUM_FBANK_BINS];
// float g_mfccMelFBank[MFCC_NUM_FBANK_BINS][50];

static inline float ns_audio_InverseMelScale(float mel_freq) {
    return 700.0f * (expf(mel_freq / 1127.0f) - 1.0f);
}

static inline float ns_audio_MelScale(float freq) {
    return 1127.0f * logf(1.0f + freq / 700.0f);
}

static void ns_fbanks_map_arena(ns_fbanks_cfg_t *cfg) {
    cfg->mfccFbankFirst = (int32_t*)cfg->arena_fbanks;
    cfg->mfccFbankLast = (int32_t*)(cfg->mfccFbankFirst + cfg->num_fbank_bins*sizeof(int32_t));
    cfg->mfccMelFBank = (ns_fbank_t*)(cfg->mfccFbankLast + cfg->num_fbank_bins*sizeof(int32_t));
}


void create_mel_fbank(ns_fbanks_cfg_t *cfg) {
    int32_t bin, i;
    int32_t num_fft_bins = cfg->frame_len_pow2 / 2;
    float fft_bin_width = ((float)cfg->sample_frequency) / cfg->frame_len_pow2;
    float mel_low_freq = ns_audio_MelScale(cfg->low_freq);
    float mel_high_freq = ns_audio_MelScale(cfg->high_freq);
    float mel_freq_delta = (mel_high_freq - mel_low_freq) / (cfg->num_fbank_bins + 1);

    float this_bin[num_fft_bins];

    for (bin = 0; bin < cfg->num_fbank_bins; bin++) {

        float left_mel = mel_low_freq + bin * mel_freq_delta;
        float center_mel = mel_low_freq + (bin + 1) * mel_freq_delta;
        float right_mel = mel_low_freq + (bin + 2) * mel_freq_delta;

        int32_t first_index = -1, last_index = -1;

        for (i = 0; i < num_fft_bins; i++) {

            float freq = (fft_bin_width * i); // center freq of this fft bin.
            float mel = ns_audio_MelScale(freq);
            this_bin[i] = 0.0;

            if (mel > left_mel && mel < right_mel) {
                float weight;
                if (mel <= center_mel) {
                    weight = (mel - left_mel) / (center_mel - left_mel);
                } else {
                    weight = (right_mel - mel) / (right_mel - center_mel);
                }
                this_bin[i] = weight;
                if (first_index == -1)
                    first_index = i;
                last_index = i;
            }
        }

        cfg->mfccFbankFirst[bin] = first_index;
        cfg->mfccFbankLast[bin] = last_index;
        // am_util_stdio_printf("MFCC bin %d, first %d, last %d \n", bin,
        // first_index, last_index);

        int32_t j = 0;
        // copy the part we care about
        for (i = first_index; i <= last_index; i++) {
            (*(cfg->mfccMelFBank))[bin][j++] = this_bin[i];
            if (j >= 50) {
                am_util_stdio_printf(
                    "MFCC[%d] J %d exceeded 50. First %d Last %d, \n", bin, j,
                    first_index, last_index);
            }
        }
    }
}

void ns_fbanks_init(ns_fbanks_cfg_t *c) {
    ns_fbanks_map_arena(c);
    create_mel_fbank(c);
}