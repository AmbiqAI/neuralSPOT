#ifndef __FEAT_PROC_H__
#define __FEAT_PROC_H__
#include <arm_math.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float32_t *win_stft;
    float32_t *mel_banks;
    void *p_fft_st;
    int win_len;
    int hop_len;
    int fftsize;
    int num_frames;
    int dim_feat;
} featClass;

void featClass_init(
        featClass *self);

void featClass_proc(
        featClass *self,
        float32_t *feats, // output
        int16_t *sig   // input
        );

void spec2mel_proc(
        float32_t *melspec,     // output
        float32_t *spec,        
        float32_t *mel_banks,
        int num_banks
        );

void tflite_float2fix(
        float32_t *output_fix,
        int16_t *input_float,
        int len
        );

char* argmax_func(int8_t *logits, uint8_t len);

#ifdef __cplusplus
}
#endif
#endif // __FEAT_PROC_H__