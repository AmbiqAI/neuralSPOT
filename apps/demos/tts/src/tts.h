#ifndef TTS_H
#define TTS_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

int tts(int16_t *output_buffer);
int find_bin(int num_embeds, float *bins, float input);
int unitTest_fuse2mel();

int unitTest_phone2fuse();
int tts_init();
int GRIFFIN_LIN_algorithm_frame(int32_t *wavout);
// int tts_unitTest_f2m(void);
// int find_bin(int num_bins, float *bins, float input);
#ifdef __cplusplus
}
#endif
#endif