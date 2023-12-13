#ifndef __NNID_CNTRL_CLASS__
#define __NNID_CNTRL_CLASS__
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    enroll_phase = 0, // Enrolling new speaker
    test_phase = 1,   // Identifying enrolled speakers
} enroll_state_T;

typedef struct {
    void *pt_feat_vad;
    void *pt_feat_nnid;
    void *pt_nnst_vad;
    void *pt_nnst_nnid;
    void *pt_pcmBuf;
    int16_t count_vad_trigger;
    enroll_state_T enroll_state; // set by invoker, enroll or test (identify)
    int8_t acc_num_enroll;       // set internally, number of utterances in current enroll phase
    int8_t num_enroll;           // set internally, needed number of utterances to enroll
    int8_t id_enroll_ppl;        // Set by invoker, id of speaker being enrolled
    int8_t total_enroll_ppls;    // Set by invoker, total enrolled so far
} nnidCntrlClass;

void nnidCntrlClass_speed_testing(nnidCntrlClass *pt_inst, int16_t *rawPCM);
void nnidCntrlClass_reset(nnidCntrlClass *pt_inst);
void nnidCntrlClass_resetPcmBufClass(nnidCntrlClass *pt_inst);
void nnidCntrlClass_init(nnidCntrlClass *pt_inst);

int16_t nnidCntrlClass_exec(nnidCntrlClass *pt_inst, int16_t *rawPCM, float *pt_corr);

void norm_then_ave(int32_t *outputs, int32_t *inputs, int num_sents, int len_vec);
#ifdef __cplusplus
}
#endif
#endif
