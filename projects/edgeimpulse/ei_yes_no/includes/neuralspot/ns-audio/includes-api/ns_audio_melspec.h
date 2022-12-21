/**
 * @file ns_audio_stft.h
 * @author Scott Leishman
 * @brief Confifurable mel scaled spectrogram transform functions
 * @version 0.1
 * @date 2022-10-10
 *
 * This mel spectrogram utility takes inspiration from the python librosa library:
 * https://librosa.org/doc/latest/generated/librosa.feature.melspectrogram.html
 *
 * @copyright Copyright (c) 2022
 *
 *  \addtogroup NeuralSPOT-MelSpec
 *  @{
 *  @ingroup NeuralSPOT-Audio
 */

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
//*****************************************************************************

#ifndef __NS_AUDIO_MELSPEC_H__
#define __NS_AUDIO_MELSPEC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "arm_math.h"
#include "ns_audio_features_common.h"

typedef struct {
    uint8_t *arena;
    uint32_t sample_frequency;
    uint32_t num_fbank_bins;
    uint32_t low_freq;
    uint32_t high_freq;
    uint32_t num_frames;
    uint32_t frame_len;
    uint32_t frame_len_pow2;
    float compression_exponent;
    float *melspecBuffer; //[2 * MELSPEC_FRAME_LEN]; // interleaved real + imaginary parts
    ns_fbanks_cfg_t fbc;
} ns_melspec_cfg_t;

// Arena should be enough to accomodate the various buffers
// e.g. MFCC_ARENA_SIZE  32*(MELSPEC_FRAME_LEN_POW2*2 + MELSPEC_NUM_FBANK_BINS*52))
// where '32' is size of float and int32_t

// #ifndef STFT_OVERRIDE_DEFAULTS
//     #define MELSPEC_SAMP_FREQ 16000
//     #define MELSPEC_FRAME_LEN 512
//     #define MELSPEC_FRAME_LEN_POW2 512
//     #define MELSPEC_NUM_FRAMES 100
//     #define MELSPEC_NUM_FBANK_BINS 128
//     #define MELSPEC_MEL_LOW_FREQ 0
//     #define MELSPEC_MEL_HIGH_FREQ 8000
//     #define MELSPEC_COMPRESSION_EXPONENT 0.3
// #endif

extern void
ns_melspec_init(ns_melspec_cfg_t *c);

extern void
ns_melspec_audio_to_stft(ns_melspec_cfg_t *c, const int16_t *audio_data, float32_t *stft_out);

extern void
ns_melspec_stft_to_audio(ns_melspec_cfg_t *c, float32_t *stft_in, int16_t *audio_out);

extern void
ns_melspec_stft_to_compressed_melspec(ns_melspec_cfg_t *c, const float32_t *stft_in,
                                      float32_t *melspec_out);

extern void
ns_melspec_melspec_to_stft(ns_melspec_cfg_t *c, const float32_t *melspec_in, float32_t *stft_out);

#ifdef __cplusplus
}
#endif
#endif
