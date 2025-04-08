/* 
This feature extraction is reproduced from Pytorch STFT and Mel Filter Bank.
The windowing is of length 960 but padded with both sides equal to 32. The resultant
window is 1024. The implementation is based on the float32 so that the error between this
and Pytorch should be ignorable.
*/

#include <stdint.h>
#include "feat_proc.h"
#include <arm_math.h>
#include <math.h>
#include "params_feat.h"

// #define DEBUG_FEAT
#ifdef DEBUG_FEAT
#include "ns_ambiqsuite_harness.h"
int16_t global_count=0;
#endif
extern float32_t mel_banks[]; // mel bank coefficients
extern float32_t win_stft[];  // stft window coefficients

char *targets_aed[DIM_TARGETS] = {
    "airport",              // 0     
    "bus",                  // 1
    "metro",                // 2
    "metro_station",        // 3 
    "park",                 // 4
    "public_square",        // 5
    "shopping_mall",        // 6
    "street_pedestrian",    // 7
    "street_traffic",       // 8
    "tram"};                // 9

void featClass_init(
        featClass *self) 
{
    static arm_rfft_fast_instance_f32  fft_st;
    self->win_stft = win_stft;
    self->mel_banks = mel_banks;
    self->p_fft_st = &fft_st;
    
    self->win_len = FFT_WIN_SIZE;
    self->hop_len = HOPSIZE;
    self->fftsize = FFT_SIZE;
    self->num_frames = NUM_FRAMES;
    self->dim_feat = NUM_MEL_BINS;

    arm_rfft_fast_init_f32(
        (arm_rfft_fast_instance_f32 *) self->p_fft_st,
         self->fftsize);
}

void featClass_proc(
        featClass *self,
        float32_t *feats, // output
        int16_t *sig   // input
        )
{
    static float32_t chunk[(FFT_SIZE << 1) + 4];
    static float32_t spec[(FFT_SIZE << 1) + 4];
    
    uint8_t is_ifft=0;
    float32_t scale=1.0f/32768.0f;
    for (int i = 0; i < self->num_frames; i++) {
        // times the window to the input signal
        for (int j = 0; j < self->fftsize; j++) {
            float32_t sig_f = scale * (float32_t) sig[i * self->hop_len + j];
            chunk[j] = sig_f * self->win_stft[j];
        }
        arm_rfft_fast_f32(
            (arm_rfft_fast_instance_f32 *) self->p_fft_st,
            chunk,
            spec,
            is_ifft);
#ifdef DEBUG_FEAT
        for (int j = 15; j < 18; j++) {
            ns_lp_printf("%d: %f, %f\n", j, spec[2*j], spec[2*j+1]);
        }
#endif
        spec2mel_proc(
            feats+i*self->dim_feat,
            spec,
            self->mel_banks,
            self->dim_feat
            );
// #ifdef DEBUG_FEAT
//         for (int j = 0; j < 10; j++) {
//             ns_lp_printf("%d: %f\n", j, *(feats+i*self->dim_feat + j));
//         }
// #endif
    }
}

void spec2mel_proc(
        float32_t *melspec, // output
        float32_t *spec,
        float32_t *mel_banks,
        int num_banks
        ) 
{
    float32_t *spec_pt;
    float32_t eps=1e-5;
    // *melspec++ = (float32_t) log(eps); // The 0-th mel bin

    // 0-255 mel bins
    for (int i = 0; i < num_banks; i++) {
        int start = (int) *mel_banks++;
        int len = (int) *mel_banks++;
        *melspec = 0;
        spec_pt = spec + (start << 1);
        for (int j = 0; j < len; j++) {
            float32_t real=*spec_pt++;
            float32_t imag=*spec_pt++;
            *melspec += *mel_banks++ * (real * real + imag * imag);
        }
        *melspec++ = (float32_t) log(*melspec + eps);
    }
}

char* argmax_func(int8_t *logits, uint8_t len)
{
    int8_t max_val = logits[0];
    uint8_t max_idx = 0;
    for (uint8_t i = 1; i < len; i++) {
        if (logits[i] > max_val) {
            max_val = logits[i];
            max_idx = i;
        }
    }
    return targets_aed[max_idx];
}