/*
This module integrates neural nets, feature extraction & post processing
*/
#ifndef __NN_SPEECH_H__
#define __NN_SPEECH_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

typedef struct {
    int16_t samplingRate;
    int16_t fftsize;
    int16_t winsize_stft;
    int16_t hopsize_stft;
    int16_t num_mfltrBank;
    int16_t num_dnsmpl;
    const int16_t *pt_stft_win_coeff;
    int16_t start_bin;
    int16_t is_dcrm;
    int16_t pre_gain_q1; // q1
} PARAMS_NNSP;

typedef struct {
    char nn_id;
    void *pt_net;
    void *pt_feat;
    int8_t slides;
    int16_t trigger;
    int16_t *pt_thresh_prob;
    int16_t counts_category[8];
    int16_t *pt_th_count_trigger;
    int16_t num_dnsmpl;
    int16_t outputs[3];
    int16_t argmax_last;
    int16_t *pt_se_out;
    void *pt_state_nnid;
    PARAMS_NNSP *pt_params;
    void *pt_dcrm;
} NNSPClass;

int NNSPClass_init(
    NNSPClass *pt_inst, void *pt_net, void *pt_feat, char nn_id, const int32_t *pt_mean,
    const int32_t *pt_stdR, int16_t *pt_thresh_prob, int16_t *pt_th_count_trigger,
    PARAMS_NNSP *pt_params);

int NNSPClass_reset(NNSPClass *pt_inst);
int16_t NNSPClass_get_nnOut_dim(NNSPClass *pt_inst);
int16_t NNSPClass_get_nn_out(int32_t *output, int len);
int16_t NNSPClass_get_nn_out_base16b(int16_t *output, int len);
int16_t NNSPClass_exec(NNSPClass *pt_inst, int16_t *rawPCM);

void my_argmax(int32_t *vec, int len, int16_t *Imax);

int32_t compute_pwr2(int32_t input);

int32_t ceiling(int32_t input);

void binary_post_proc(NNSPClass *pt_inst, int32_t *pt_nn_est, int16_t *pt_trigger);

void s2i_post_proc(NNSPClass *pt_inst, int32_t *pt_nn_est, int16_t *pt_trigger);

void se_post_proc(NNSPClass *pt_inst, int16_t *pt_nn_est, int16_t *pt_se_out);

#ifdef __cplusplus
}
#endif
#endif
