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
 *  \addtogroup ns-MFCC
 *  @{
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
    #include "arm_math.h"

    #ifdef __cplusplus
extern "C" {
    #endif

    #include "ns_audio_features_common.h"
    #include "ns_core.h"
    #include "string.h"

    #define NS_MFCC_V0_0_1                                                                         \
        { .major = 0, .minor = 0, .revision = 1 }
    #define NS_MFCC_V1_0_0                                                                         \
        { .major = 1, .minor = 0, .revision = 0 }

    #define NS_MFCC_OLDEST_SUPPORTED_VERSION NS_MFCC_V0_0_1
    #define NS_MFCC_CURRENT_VERSION NS_MFCC_V1_0_0
    #define NS_MFCC_API_ID 0xCA0005

extern const ns_core_api_t ns_mfcc_V0_0_1;
extern const ns_core_api_t ns_mfcc_V1_0_0;
extern const ns_core_api_t ns_mfcc_oldest_supported_version;
extern const ns_core_api_t ns_mfcc_current_version;

/**
 * @brief Config and state for MFCC calculator
 *
 */
typedef struct {
    const ns_core_api_t *api;  ///< API prefix
    uint8_t *arena;            ///< Pointer to arena (must be allocated by caller)
    uint32_t sample_frequency; ///< Sample frequency of audio data
    uint32_t num_fbank_bins;   ///< Number of filterbank bins
    uint32_t low_freq;         ///< Low frequency cutoff
    uint32_t high_freq;        ///< High frequency cutoff
    uint32_t num_frames;       ///< Number of frames
    uint32_t num_coeffs;       ///< Number of MFCC coefficients
    uint32_t num_dec_bits;     ///< Number of decimation bits
    uint32_t frame_shift_ms;   ///< Not used
    uint32_t frame_len_ms;     ///< Not used
    uint32_t frame_len;        ///< Frame length
    uint32_t frame_len_pow2;   ///< Frame length to nearest power of 2
    float *mfccFrame;          ///< pointer to MFCC frame (set internally)
    float *mfccBuffer;         ///< pointer to MFCC buffer (set internally)
    float *mfccEnergies;       ///< pointer to MFCC energies (set internally)
    float *mfccWindowFunction; ///< pointer to MFCC window function (set internally)
    float *mfccDCTMatrix;      ///< pointer to MFCC DCT matrix (set internally)
    ns_fbanks_cfg_t fbc;       ///< Filterbank config (set internally)
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

/**
 * @brief Initializes the MFCC calculator based on desired configuration
 *
 * @param c configuration struct (see ns_mfcc_cfg_t)
 * @return uint32_t status
 */
extern uint32_t ns_mfcc_init(ns_mfcc_cfg_t *c);

/**
 * @brief Computes MFCC based on audio data
 *
 * @param c - configuration struct (see ns_mfcc_cfg_t) from ns_mfcc_init
 * @param audio_data - pointer to audio data (int16_t)
 * @param mfcc_out - pointer to output buffer (float)
 * @return uint32_t
 */
extern uint32_t ns_mfcc_compute(ns_mfcc_cfg_t *c, const int16_t *audio_data, float *mfcc_out);

    #ifdef __cplusplus
}
    #endif
#endif
/** @} */ // End Doxygen Group
