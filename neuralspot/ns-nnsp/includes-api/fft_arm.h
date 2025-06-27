#ifndef __FFT_ARM_H__
#define __FFT_ARM_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
void arm_fft_init(void *p_fft_st_t, uint32_t is_ifft, int16_t fftsize);

void arm_fft_exec(
    void *p_fft_st_t,
    int32_t *y,  // Q21
    int32_t *x); // Q30

#ifdef __cplusplus
}
#endif
#endif
