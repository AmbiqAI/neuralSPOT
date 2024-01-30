#ifndef __PARAMS_NN_CNTRL_H__
#define __PARAMS_NN_CNTRL_H__
#ifdef __cplusplus
extern "C" {
#endif
// vad
#define THRESHOLD_MASK_VAD (32767 >> 1)
#define THRESHOLD_CNTS_VAD 4

// S2I_RNN
#define FRS_VBUFBK_S2IRNN 80
#define TIMEOUT_FRS_S2IRNN 1000
#define THRESHOLD_MASK_S2IRNN (32767 >> 1)
#define THRESHOLD_CNTS_S2IRNN 4

// KWS
#define FRS_VBUFBK_KWS FRS_VBUFBK_S2IRNN
#define TIMEOUT_FRS_KWS 1000
#define THRESHOLD_MASK_KWS (32767 >> 1)
#define THRESHOLD_CNTS_KWS THRESHOLD_CNTS_S2IRNN

#ifdef __cplusplus
}
#endif
#endif
