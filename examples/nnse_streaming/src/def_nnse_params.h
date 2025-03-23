#ifndef __DEF_NNSE_PARAMS_H__
#define __DEF_NNSE_PARAMS_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

// feature parameters of NNSE:
#define FEATURE_CONTEXT 6
#define FEATURE_QBIT 8
#define FEATURE_NUM_MFC 72
#define FEATURE_WINSIZE 480
#define FEATURE_HOPSIZE 160
#define FEATURE_FFTSIZE 512

// NN parameters of NNSE:
#define NN_DIM_OUT 257

#ifdef __cplusplus
}
#endif
#endif // __DEF_NNSE_PARAMS_H__