#include <stdint.h>
#include <arm_mve.h>
#include "fft_arm.h"
#include "basic_mve.h"
#include "stft32b.h"

static int32_t dataBuffer[2050];
static int32_t odataBuffer[2050];
static int32_t glob_spec[2050];
static int32_t glob_fft_buf[2050];

int stftModule32b_construct(
    stftModule32b *ps, int16_t len_win, int16_t hopsize, int16_t fftsize,
    const int16_t *pt_stft_win_coeff) {
    ps->spec = glob_spec;
    ps->dataBuffer = dataBuffer;
    ps->odataBuffer = odataBuffer;
    ps->len_win = len_win;
    ps->hop = hopsize;
    ps->len_fft = fftsize;
    ps->window = pt_stft_win_coeff;
#if ARM_FFT == 1
    arm_fft_init(&ps->fft_st, 0, fftsize);
    arm_fft_init(&ps->ifft_st, 1, fftsize);
#endif
    return 0;
}

int stftModule32b_setDefault(stftModule32b *ps) {
    for (int i = 0; i < ps->len_win; i++) {
        ps->dataBuffer[i] = 0;
        ps->odataBuffer[i] = 0;
    }
    return 0;
}


int stftModule32b_analyze_arm(
    void *ps_t,
    int32_t *fft_in,  // q15
    int32_t *spec,       // q20
    int16_t fftsize,
    int16_t qbits_input) {

    stftModule32b *ps = (stftModule32b *)ps_t;
    

    memcpy(
        ps->dataBuffer,
        ps->dataBuffer+ps->hop,
        (ps->len_win - ps->hop) << 2 );

    memcpy(
        ps->dataBuffer+ps->len_win - ps->hop,
        fft_in,
        ps->hop << 2 );

    
    // q15 as input until now

    for (int i = 0 ; i < ps->len_win; i++) {
        glob_fft_buf[i] = (int32_t) ps->window[i] * ps->dataBuffer[i];
    }

    
    set_zero_32b(
        glob_fft_buf+ps->len_win,
        ps->len_fft - ps->len_win);
    // q30 until now
    arm_fft_exec(
        &ps->fft_st,
        spec,          // fft_out, Q20
        glob_fft_buf); // fft_in,  Q30
    // q20 as the output
    for (int i = 0; i < (fftsize + 2); i++) {
        spec[i] >>= (20 - qbits_input);
    }
    return 0;
}

/*
        stftModule_synthesize_arm: inverse stft synthesization
        overlap-and-add approach
*/
int stftModule32b_synthesize_arm(
    void *ps_t,
    int32_t *spec,   // Q21
    int32_t *output, // Q15
    int right_shift) 
{
    int i;
    int64_t tmp64;
    stftModule32b *ps = (stftModule32b *)ps_t;
    int32_t *pt_out;

    arm_rfft_q31(
        &ps->ifft_st,
        spec,          // Q21
        glob_fft_buf); // Q21

    for (i = 0; i < ps->len_win; i++) {
        tmp64 = ((int64_t)ps->window[i]) * (int64_t)glob_fft_buf[i];
        tmp64 >>= right_shift;
        tmp64 = (int64_t)ps->odataBuffer[i] + (int64_t)tmp64;
        tmp64 = MIN(MAX(tmp64, INT32_MIN), INT32_MAX);
        ps->odataBuffer[i] = (int32_t)tmp64;
    }

    for (i= 0; i < ps->hop; i++)
    {
        output[i] = ps->odataBuffer[i];
    }

    // for (i = 0; i < ps->len_win - ps->hop; i++) {
    //     ps->odataBuffer[i] = ps->odataBuffer[i + ps->hop];
    // }
    
    memcpy(
        ps->odataBuffer,
        ps->odataBuffer + ps->hop,
        (ps->len_win - ps->hop) << 2 );
    
    // move_data_16b(
    //     (int16_t*) (ps->odataBuffer + ps->hop),
    //     (int16_t*) ps->odataBuffer,
    //     (ps->len_win - ps->hop) << 1 );

    pt_out = ps->odataBuffer + ps->len_win - ps->hop;
    // for (i = 0; i < ps->hop; i++) {
    //     pt_out[i] = 0;
    // }
    set_zero_32b(pt_out, ps->hop);
    return 0;
}