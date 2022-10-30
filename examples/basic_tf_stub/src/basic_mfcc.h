/**
 * @file basic_mfcc.h
 * @author Carlos Morales
 * @brief NS_MFCC example
 * @version 0.1
 * @date 2022-10-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "ns_audio_mfcc.h"

// MFCC Config
#define MY_MFCC_FRAME_LEN_POW2  512 // Next power of two size after SAMPLES_IN_FRAME
#define MY_MFCC_NUM_FBANK_BINS  40
#define MY_MFCC_NUM_MFCC_COEFFS 13

// Allocate memory for MFCC calculations
#define MFCC_ARENA_SIZE  32*(MY_MFCC_FRAME_LEN_POW2*2 + MY_MFCC_NUM_FBANK_BINS*(NS_MFCC_SIZEBINS+MY_MFCC_NUM_MFCC_COEFFS))
static uint8_t mfccArena[MFCC_ARENA_SIZE];

ns_mfcc_cfg_t mfcc_config = {
    .arena = mfccArena,
    .sample_frequency = SAMPLE_RATE,
    .num_fbank_bins = MY_MFCC_NUM_FBANK_BINS,
    .low_freq = 20,
    .high_freq = 8000,
    .num_frames = NUM_FRAMES,
    .num_coeffs = MY_MFCC_NUM_MFCC_COEFFS,
    .num_dec_bits = 4,
    .frame_shift_ms = 30,
    .frame_len_ms = 30,
    .frame_len = SAMPLES_IN_FRAME,
    .frame_len_pow2 = MY_MFCC_FRAME_LEN_POW2    
};