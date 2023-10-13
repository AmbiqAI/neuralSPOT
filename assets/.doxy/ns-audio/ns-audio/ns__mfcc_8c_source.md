

# File ns\_mfcc.c

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-audio**](dir_45211a8475460839574f71aa108f4957.md) **>** [**src**](dir_e70eef2d5115541d1d6cb7ad27f30382.md) **>** [**ns\_mfcc.c**](ns__mfcc_8c.md)

[Go to the documentation of this file](ns__mfcc_8c.md)

```C++


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
#include "ns_audio_mfcc.h"
#include "ns_core.h"

const ns_core_api_t ns_mfcc_V0_0_1 = {.apiId = NS_MFCC_API_ID, .version = NS_MFCC_V0_0_1};

const ns_core_api_t ns_mfcc_V1_0_0 = {.apiId = NS_MFCC_API_ID, .version = NS_MFCC_V1_0_0};

const ns_core_api_t ns_mfcc_oldest_supported_version = {
    .apiId = NS_MFCC_API_ID, .version = NS_MFCC_V0_0_1};

const ns_core_api_t ns_mfcc_current_version = {.apiId = NS_MFCC_API_ID, .version = NS_MFCC_V1_0_0};

// float g_mfccFrame[MFCC_FRAME_LEN_POW2];
// float g_mfccBuffer[MFCC_FRAME_LEN_POW2];
// float g_mfccEnergies[MFCC_NUM_FBANK_BINS];
// float g_mfccWindowFunction[MFCC_FRAME_LEN];
// float g_mfccDCTMatrix[MFCC_NUM_FBANK_BINS * MFCC_NUM_MFCC_FEATURES];

// WARNING this is some hacky, finicky pointer math. Sizes and order have to match
// Fiddle with this at your own peril
static void ns_mfcc_map_arena(ns_mfcc_cfg_t *cfg) {
    cfg->mfccFrame = (float *)cfg->arena;
    cfg->mfccDCTMatrix = (float *)(cfg->mfccFrame + cfg->frame_len_pow2 * sizeof(float));
    cfg->mfccBuffer =
        (float *)(cfg->mfccDCTMatrix + cfg->num_fbank_bins * cfg->num_coeffs * sizeof(float));
    cfg->mfccEnergies = (float *)(cfg->mfccBuffer + cfg->frame_len_pow2 * sizeof(float));
    cfg->mfccWindowFunction = (float *)(cfg->mfccEnergies + cfg->num_fbank_bins * sizeof(float));
}
// --------------------

#ifdef MFCC_DEBUG
float g_audioMax;
float g_audioMin;
float g_audioSum;
float g_audioMaxInt;
float g_audioMinInt;
float g_audioSumInt;
#endif

arm_rfft_fast_instance_f32 g_mfccRfft;

static void create_dct_matrix(ns_mfcc_cfg_t *cfg, int32_t input_length, int32_t coefficient_count) {
    int32_t k, n;
    float normalizer;
    arm_sqrt_f32(2.0 / (float)input_length, &normalizer);
    for (k = 0; k < coefficient_count; k++) {
        for (n = 0; n < input_length; n++) {
            cfg->mfccDCTMatrix[k * input_length + n] =
                normalizer * cos(((double)M_PI) / input_length * (n + 0.5) * k);
        }
    }
}

uint32_t ns_mfcc_init(ns_mfcc_cfg_t *c) {
    int i;
#ifndef NS_DISABLE_API_VALIDATION
    if (c == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }

    if (ns_core_check_api(c->api, &ns_mfcc_oldest_supported_version, &ns_mfcc_current_version)) {
        return NS_STATUS_INVALID_VERSION;
    }
#endif
    ns_mfcc_map_arena(c);

    c->fbc.arena_fbanks = (uint8_t *)(c->mfccWindowFunction) + c->frame_len * sizeof(float);
    c->fbc.sample_frequency = c->sample_frequency;
    c->fbc.num_fbank_bins = c->num_fbank_bins;
    c->fbc.low_freq = c->low_freq;
    c->fbc.high_freq = c->high_freq;
    c->fbc.frame_len_pow2 = c->frame_len_pow2;

    ns_fbanks_init(&(c->fbc));

    for (i = 0; i < c->frame_len; i++) {
        c->mfccWindowFunction[i] = 0.5 - 0.5 * cos(M_2PI * ((float)i) / (c->frame_len));
    }

    create_dct_matrix(c, c->num_fbank_bins, c->num_coeffs);

    arm_rfft_fast_init_f32(&g_mfccRfft, c->frame_len_pow2);
    return NS_STATUS_SUCCESS;
}

uint32_t ns_mfcc_compute(ns_mfcc_cfg_t *cfg, const int16_t *audio_data, float *mfcc_out) {

    int32_t i, j, bin;

    // TensorFlow way of normalizing int16_t data to (-1,1)
    for (i = 0; i < cfg->frame_len; i++) {
        cfg->mfccFrame[i] = ((float)audio_data[i] / (1 << 15)) * cfg->mfccWindowFunction[i];
    }
    // Fill up remaining with zeros
    memset(
        &(cfg->mfccFrame[cfg->frame_len]), 0,
        sizeof(float) * (cfg->frame_len_pow2 - cfg->frame_len));

    // Compute FFT
    arm_rfft_fast_f32(&g_mfccRfft, cfg->mfccFrame, cfg->mfccBuffer, 0);

    // Convert to power spectrum
    // frame is stored as [real0, realN/2-1, real1, im1, real2, im2, ...]
    int32_t half_dim = cfg->frame_len_pow2 / 2;
    float first_energy = cfg->mfccBuffer[0] * cfg->mfccBuffer[0],
          last_energy = cfg->mfccBuffer[1] * cfg->mfccBuffer[1]; // handle this special case
    for (i = 1; i < half_dim; i++) {
        float real = cfg->mfccBuffer[i * 2];
        float im = cfg->mfccBuffer[i * 2 + 1];
        cfg->mfccBuffer[i] = real * real + im * im;
    }
    cfg->mfccBuffer[0] = first_energy;
    cfg->mfccBuffer[half_dim] = last_energy;

    float sqrt_data;

    // Apply mel filterbanks
    for (bin = 0; bin < cfg->num_fbank_bins; bin++) {
        j = 0;
        float mel_energy = 0;
        int32_t first_index = cfg->fbc.mfccFbankFirst[bin];
        int32_t last_index = cfg->fbc.mfccFbankLast[bin];
        for (i = first_index; i <= last_index; i++) {
            arm_sqrt_f32(cfg->mfccBuffer[i], &sqrt_data);
            mel_energy += (sqrt_data) * (*(cfg->fbc.melFBank))[bin][j++];
        }
        cfg->mfccEnergies[bin] = mel_energy;

        // avoid log of zero
        if (mel_energy == 0.0)
            cfg->mfccEnergies[bin] = FLT_MIN;
    }

    // Take log
    for (bin = 0; bin < cfg->num_fbank_bins; bin++)
        cfg->mfccEnergies[bin] = logf(cfg->mfccEnergies[bin]);

    // Take DCT. Uses matrix mul.
    for (i = 0; i < cfg->num_coeffs; i++) {
        float sum = 0.0;
        for (j = 0; j < cfg->num_fbank_bins; j++) {
            sum += cfg->mfccDCTMatrix[i * cfg->num_fbank_bins + j] * cfg->mfccEnergies[j];
        }

        sum *= (0x1 << cfg->num_dec_bits);
        sum = round(sum);

        // This is usually done after dequantization anyway, so preserve accuracy
        // if(sum >= 127)
        //     mfcc_out[i] = 127;
        // else if(sum <= -128)
        //     mfcc_out[i] = -128;
        // else
        mfcc_out[i] = sum;
    }
    return NS_STATUS_SUCCESS;
}

```

