#include "spectrogram_module.h"
#include "ambiq_stdint.h"
#include "ambiq_nnsp_const.h"
#include "ambiq_nnsp_debug.h"
#include "minmax.h"
#if ARM_FFT == 0
    #include "fft.h"
#else
    #include "fft_arm.h"
#endif
int16_t dataBuffer[LEN_FFT_NNSP];
int32_t odataBuffer[LEN_FFT_NNSP];
int32_t glob_spec[1026];
int32_t glob_fft_buf[LEN_FFT_NNSP << 1];

int stftModule_construct(
    stftModule *ps, int16_t len_win, int16_t hopsize, int16_t fftsize,
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

int stftModule_setDefault(stftModule *ps) {
    for (int i = 0; i < ps->len_win; i++) {
        ps->dataBuffer[i] = 0;
        ps->odataBuffer[i] = 0;
    }
    return 0;
}

#if ARM_FFT == 0
void spec2pspec(int32_t *y, int32_t *x, int len) {
    int i;
    int64_t tmp;
    for (i = 0; i < len; i++) {
        tmp = (int64_t)x[2 * i] * (int64_t)x[2 * i] + (int64_t)x[2 * i + 1] * (int64_t)x[2 * i + 1];
        y[i] = (int32_t)(tmp >> 15);
    }
}

int stftModule_analyze(stftModule *ps, int16_t *x, int32_t *y) {
    int i;
    int32_t tmp;
    static int32_t fft_in[LEN_FFT_NNSP];
    for (i = 0; i < (ps->len_win - ps->hop); i++)
        ps->dataBuffer[i] = ps->dataBuffer[i + ps->hop];

    tmp = ps->len_win - ps->hop;
    for (i = 0; i < ps->hop; i++)
        ps->dataBuffer[i + tmp] = x[i];

    for (i = 0; i < ps->len_win; i++) {
        tmp = ((int32_t)ps->window[i] * (int32_t)ps->dataBuffer[i]);
        fft_in[i] = tmp >> 15; // Frac15
    }

    for (i = 0; i < (ps->len_fft - ps->len_win); i++) {
        fft_in[i + ps->len_win] = 0;
    }
    rfft(ps->len_fft, fft_in,
         (void *)y); // Frac15

    return 0;
}
#else
void spec2pspec_arm(
    int32_t *pspec, // q15
    int32_t *spec,  // q21
    int len, int16_t qbit_in) {
    int i;
    int64_t acc, tmp_t;
    int32_t *pt_spec = spec;
    int rshift = (qbit_in << 1) - 15;

    for (i = 0; i < len; i++) {
        acc = 0;
        tmp_t = (int64_t)*pt_spec++;
        acc = tmp_t * tmp_t;
        tmp_t = (int64_t)*pt_spec++;
        acc += tmp_t * tmp_t;
        pspec[i] = (int32_t)MIN(MAX(acc >> rshift, INT32_MIN), INT32_MAX);
    }
}

/*
        stftModule_analyze_arm: stft analysis
        overlap-and-add approach
*/
int stftModule_analyze_arm(
    void *ps_t,
    int16_t *fft_in_q16, // q15
    int32_t *spec,       // q21
    int16_t fftsize, int16_t *pt_qbit_out) {
    int i;
    int32_t tmp;
    stftModule *ps = (stftModule *)ps_t;

    for (i = 0; i < (ps->len_win - ps->hop); i++)
        ps->dataBuffer[i] = ps->dataBuffer[i + ps->hop];

    tmp = ps->len_win - ps->hop;
    for (i = 0; i < ps->hop; i++)
        ps->dataBuffer[i + tmp] = fft_in_q16[i];

    for (i = 0; i < ps->len_win; i++) {
        tmp = (int32_t)ps->window[i] * (int32_t)ps->dataBuffer[i];
        glob_fft_buf[i] = tmp; // Q30
    }

    for (i = 0; i < (ps->len_fft - ps->len_win); i++) {
        glob_fft_buf[i + ps->len_win] = 0;
    }

    arm_fft_exec(
        &ps->fft_st,
        spec,          // fft_out, Q21
        glob_fft_buf); // fft_in,  Q30
    if (fftsize == 512)
        *pt_qbit_out = 21;
    else
        *pt_qbit_out = 22;
    return 0;
}

/*
        stftModule_synthesize_arm: inverse stft synthesization
        overlap-and-add approach
*/
int stftModule_synthesize_arm(
    void *ps_t,
    int32_t *spec,   // Q21
    int16_t *output) // Q15
{
    int i;
    int64_t tmp64;
    stftModule *ps = (stftModule *)ps_t;
    int32_t *pt_out;
    arm_rfft_q31(
        &ps->ifft_st,
        spec,          // Q21
        glob_fft_buf); // Q21

    for (i = 0; i < ps->len_win; i++) {
        tmp64 = ((int64_t)ps->window[i]) * (int64_t)glob_fft_buf[i];
        tmp64 >>= 21;
        tmp64 = (int64_t)ps->odataBuffer[i] + (int64_t)tmp64;
        tmp64 = MIN(MAX(tmp64, INT32_MIN), INT32_MAX);
        ps->odataBuffer[i] = (int32_t)tmp64;
    }

    for (i = 0; i < ps->hop; i++)
        output[i] = (int16_t)MIN(MAX(ps->odataBuffer[i], INT16_MIN), INT16_MAX);
    ;

    for (i = 0; i < ps->len_win - ps->hop; i++) {
        ps->odataBuffer[i] = ps->odataBuffer[i + ps->hop];
    }

    pt_out = ps->odataBuffer + ps->len_win - ps->hop;
    for (i = 0; i < ps->hop; i++) {
        pt_out[i] = 0;
    }

    return 0;
}
#endif
