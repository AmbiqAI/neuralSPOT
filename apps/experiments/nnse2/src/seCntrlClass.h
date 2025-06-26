#ifndef __SE_CONTROL_CLASS_H__
#define __SE_CONTROL_CLASS_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#define SAMPLES_FRM_NNCNTRL_CLASS 160
#define MAX_NUM_NN 5

// A collection of NN control paramters
typedef struct {
    // S2I
    int16_t frs_vbufBk_s2i;  // lookback frames when s2i starts
    int16_t thresh_prob_s2i; // threshold for trigger prob
    int16_t thresh_cnts_s2i; // threshold for number of counts to trigger
} ParamCntrlClass;

/*
    NN Control class: manipulate the sequence of sevearal NNs, such
    as VAD, KWS, S2I
*/
typedef struct {
    void *pt_nnsp;
    ParamCntrlClass Params;
} seCntrlClass;

void seCntrlClass_init(seCntrlClass *pt_inst);

void seCntrlClass_reset(seCntrlClass *pt_inst);

void seCntrlClass_exec(seCntrlClass *pt_inst, int16_t *data_fr, int16_t *se_output);
#ifdef __cplusplus
}
#endif
#endif
