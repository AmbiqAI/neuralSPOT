#include "ambiq_nnsp_debug.h"
#if ARM_FFT == 1
    #include <stdint.h>
    #include <arm_math.h>
    #include "fft_arm.h"
    #include "ambiq_nnsp_const.h"

void arm_fft_init(void *p_fft_st_t, uint32_t is_ifft, int16_t fftsize) {
    arm_rfft_instance_q31 *p_fft_st = (arm_rfft_instance_q31 *)p_fft_st_t;
    uint32_t bitReverseFlag = 1;
    arm_rfft_init_q31(p_fft_st, fftsize, is_ifft, bitReverseFlag);
}

void arm_fft_exec(
    void *p_fft_st_t,
    int32_t *y, // Q21
    int32_t *x) // Q30
{
    arm_rfft_instance_q31 *p_fft_st = (arm_rfft_instance_q31 *)p_fft_st_t;
    arm_rfft_q31(p_fft_st, x, y);
}
#endif
