#ifndef TTS_H
#define TTS_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

// int embedding_bin_process(
//         int16_t *outputs, 
//         float *inputs_float, 
//         int16_t *weights,
//         int num_inputs,
//         int num_embeds,
//         int dim_feat,
//         float *bins,
//         int num_bins
//     );

// int embedding_process(
//         int16_t *outputs, 
//         int16_t *inputs, 
//         int16_t *weights,
//         int num_inputs,
//         int dim_feat);
int tts(void);
int unitTest_fuse2mel();

int unitTest_phone2fuse();
int tts_init();
// int tts_unitTest_f2m(void);
// int find_bin(int num_bins, float *bins, float input);
#ifdef __cplusplus
}
#endif
#endif