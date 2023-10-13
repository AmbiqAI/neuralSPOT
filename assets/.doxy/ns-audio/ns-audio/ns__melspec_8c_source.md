

# File ns\_melspec.c

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-audio**](dir_45211a8475460839574f71aa108f4957.md) **>** [**src**](dir_e70eef2d5115541d1d6cb7ad27f30382.md) **>** [**ns\_melspec.c**](ns__melspec_8c.md)

[Go to the documentation of this file](ns__melspec_8c.md)

```C++


//*****************************************************************************
//
// Copyright (c) 2022, Ambiq Micro, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//
//*****************************************************************************

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"

#include "float.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_audio_melspec.h"
#include <string.h>

// Globals
// float32_t g_melspecBuffer[2 * MELSPEC_FRAME_LEN]; // interleaved real + imaginary parts
// int32_t g_melspecFbankFirst[MELSPEC_NUM_FBANK_BINS];
// int32_t g_melspecFbankLast[MELSPEC_NUM_FBANK_BINS];
// float g_melspecMelFBank[MELSPEC_NUM_FBANK_BINS][50];

arm_cfft_instance_f32 g_melspecRfft;

void
ns_melspec_init(ns_melspec_cfg_t *cfg) {
    arm_status status = arm_cfft_init_f32(&g_melspecRfft, cfg->frame_len);
    if (status != ARM_MATH_SUCCESS) {
        ns_printf("problem initializing melspec: status enum %d\n", status);
    }
    cfg->melspecBuffer = (float *)(cfg->arena);

    cfg->fbc.arena_fbanks = (uint8_t *)(cfg->melspecBuffer) + 2 * cfg->frame_len * sizeof(float);
    cfg->fbc.sample_frequency = cfg->sample_frequency;
    cfg->fbc.num_fbank_bins = cfg->num_fbank_bins;
    cfg->fbc.low_freq = cfg->low_freq;
    cfg->fbc.high_freq = cfg->high_freq;
    cfg->fbc.frame_len_pow2 = cfg->frame_len_pow2;
    ns_fbanks_init(&(cfg->fbc));
}

void
ns_melspec_audio_to_stft(ns_melspec_cfg_t *cfg, const int16_t *audio_in, float32_t *stft_out) {
    int16_t i;

    // copy the integer audio buffer into complex valued stft input buffer
    for (i = 0; i < cfg->frame_len; i++) {
        stft_out[2 * i] = (float32_t)audio_in[i]; // real part
        stft_out[2 * i + 1] = (float32_t)0.0;     // imaginary part
    }

    // compute short-time fourier transform in-place.
    arm_cfft_f32(&g_melspecRfft, stft_out, 0, 0);
}

void
ns_melspec_stft_to_audio(ns_melspec_cfg_t *cfg, float32_t *stft_in, int16_t *audio_out) {
    int16_t i;

    // compute the inverse stft in-place (destroys original input!!)
    arm_cfft_f32(&g_melspecRfft, stft_in, 1, 0);
    for (i = 0; i < cfg->frame_len; i++) {
        // TODO: check correctness of this calculation
        audio_out[i] = (int16_t)stft_in[2 * i];
    }
}

void
ns_melspec_stft_to_compressed_melspec(ns_melspec_cfg_t *cfg, const float32_t *stft_in,
                                      float32_t *melspec_out) {
    int16_t i, j, k;

    for (i = 0; i < cfg->num_fbank_bins; i++) {
        k = 0;
        float curr_val = 0.0;
        for (j = cfg->fbc.mfccFbankFirst[i]; j < cfg->fbc.mfccFbankLast[i]; j++) {
            curr_val += stft_in[2 * j] * (*(cfg->fbc.melFBank))[i][k++];
        }
        melspec_out[i] = (float)pow(curr_val, cfg->compression_exponent);
    }
}

void
ns_melspec_melspec_to_stft(ns_melspec_cfg_t *cfg, const float32_t *melspec_in,
                           float32_t *stft_out) {
    int16_t i, j, k;

    // TODO: check correctness of this calculation with python implementation
    // assumed melspec_in shape: [num_frames, num_mel_bins]  (num_frames likely 1)
    // g_melspecMelFBank shape: [num_mel_bins, num_spec_bins[first_nonzero,last_nonzero]]
    // stft_out shape: [num_frames, num_spec_bins]
    // stft_out <- MatMul(melspec_in, g_melspecMelFBank)
    for (i = 0; i < cfg->num_fbank_bins; i++) {
        k = 0;
        float curr_val = 0.0;
        for (j = cfg->fbc.mfccFbankFirst[i]; j < cfg->fbc.mfccFbankLast[i]; j++) {
            curr_val += (*(cfg->fbc.melFBank))[i][k++] * stft_out[j * 2];
        }
        stft_out[i] = curr_val;
    }
}

```

