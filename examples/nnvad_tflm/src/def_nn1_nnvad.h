#ifndef __DEF_NN3_VAD2__
#define __DEF_NN3_VAD2__

#include <stdint.h>
#include "neural_nets.h"
#include "nn_speech.h"

extern const int32_t feature_mean_vad2[];
extern const int32_t feature_stdR_vad2[];

#define NUM_LOOKAHEAD 0
extern PARAMS_NNSP params_nn1_nnvad;
#define FEATURE_EXTRACTION 0
#endif  // __DEF_NN3_SE__
