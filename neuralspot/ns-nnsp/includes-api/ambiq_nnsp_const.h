#ifndef __AMBIQ_NNSP_CONST_H__
#define __AMBIQ_NNSP_CONST_H__
#ifdef __cplusplus
extern "C" {
#endif
#define LEN_FFT_NNSP 512
#define LEN_STFT_WIN_COEFF 480
#define LEN_STFT_HOP 160
#define NUM_MELBANKS 40
#define NUM_FEATURE_CONTEXT 6
#define MAX_SIZE_FEATURE 257
#define DIMEMSION_FEATURE NUM_MELBANKS
#define SAMPLING_RATE 16000
#ifdef __cplusplus
}
#endif
#endif
