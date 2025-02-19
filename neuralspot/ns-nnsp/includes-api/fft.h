#ifndef __FFT_H__
#define __FFT_H__
#include <stdint.h>
void fft(int exp_nfft, void *input_, void *output_);
void rfft(int num_rfft, int32_t *input, void *output_);
#endif
