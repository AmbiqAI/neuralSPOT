#ifndef TTS_H
#define TTS_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#define NUM_TOKENS 31
#define TIMESTEPS 300
#define DIM_FUSE 128
#define BYTES_INPUT_SIZE_F2M 76800
#define WINDOW_SIZE 1024
#define HOP_SIZE 256
#define FFT_SIZE 1024
#define ITERS_GRIFFIN_LIN 5
#define NUM_FFT_BINS ((FFT_SIZE >> 1) + 1)
int tts(int16_t *output_buffer, int *pt_acc_frames);
int find_bin(int num_embeds, float *bins, float input);
int unitTest_fuse2mel();

int unitTest_phone2fuse();
int tts_init();
int GRIFFIN_LIN_algorithm_frame(int32_t *wavout, int total_frames);
// int tts_unitTest_f2m(void);
// int find_bin(int num_bins, float *bins, float input);
#ifdef __cplusplus
}
#endif
#endif