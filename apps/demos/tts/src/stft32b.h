#ifndef __STFT32B_H__
#define __STFT32B_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include "ambiq_nnsp_debug.h"
#if ARM_FFT == 1
    #include <arm_math.h>
#endif
#include "ambiq_nnsp_const.h"
typedef struct {
    int16_t len_win;
    int16_t hop;
    int16_t len_fft;
    int32_t *dataBuffer;
    int32_t *odataBuffer;
    const int16_t *window;
#if ARM_FFT == 1
    arm_rfft_instance_q31 fft_st;
    arm_rfft_instance_q31 ifft_st;
#endif
    int32_t *spec;
} stftModule32b;

int stftModule32b_construct(
    stftModule32b *ps,
    int16_t len_win, int16_t hopsize, int16_t fftsize,
    const int16_t *pt_stft_win_coeff);

int stftModule32b_setDefault(stftModule32b *ps);


int stftModule32b_analyze_arm(
    void *ps_t,
    int32_t *fft_in_q16, // q15
    int32_t *spec,       // q20
    int16_t fftsize,
    int16_t qbits_input);

int stftModule32b_synthesize_arm(
    void *ps_t,
    int32_t *spec,
    int32_t *output,
    int right_shift);

#ifdef __cplusplus
}
#endif
#endif
