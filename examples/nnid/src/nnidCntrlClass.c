#include <stdint.h>
#include "feature_module.h"
#include "def_nn1_nnvad.h"
#include "def_nn4_nnid.h"
#include "ambiq_nnsp_debug.h"
#include "nnsp_identification.h"
#include "nnid_class.h"
#include "PcmBufClass.h"
#include "nnidCntrlClass.h"
#include <math.h>
#include "ns_timer.h"
FeatureClass feat_vad, feat_nnid;
NNSPClass nnst_vad, nnst_nnid;
PcmBufClass pcmBuf_inst;
int16_t glob_th_prob = 0x7fff >> 1;
int16_t glob_count_trigger = 1;
// Model Stuff
ns_timer_config_t tickTimer = {
    .prefix = {0},
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};

void nnidCntrlClass_speed_testing(nnidCntrlClass *pt_inst, int16_t *rawPCM) {
    int fr;
    uint32_t elapsed_time;
    int NUM_FRAMES = 100;
    nnidCntrlClass_reset(pt_inst);
    ns_timer_init(&tickTimer);

    for (fr = 0; fr < NUM_FRAMES; fr++)
        NNSPClass_exec(&nnst_vad, rawPCM);
    elapsed_time = ns_us_ticker_read(&tickTimer);
    ns_lp_printf("vad:\n");
    ns_lp_printf("nn: %3.2f ms/inference\n", ((float)elapsed_time) / NUM_FRAMES / 1000);

    ns_timer_init(&tickTimer);
    for (fr = 0; fr < 180; fr++)
        NNSPClass_exec(&nnst_nnid, rawPCM);
    elapsed_time = ns_us_ticker_read(&tickTimer);
    ns_lp_printf("nnid:\n");
    ns_lp_printf("nn: %3.2f ms/inference\n", ((float)elapsed_time) / NUM_FRAMES / 1000);
}

void nnidCntrlClass_reset(nnidCntrlClass *pt_inst) {

    NNSPClass_reset(&nnst_vad);
    NNSPClass_reset(&nnst_nnid);
    pt_inst->count_vad_trigger = 0;
    pt_inst->acc_num_enroll = 0;
}

void nnidCntrlClass_resetPcmBufClass(nnidCntrlClass *pt_inst) { PcmBufClass_reset(&pcmBuf_inst); }

void nnidCntrlClass_init(nnidCntrlClass *pt_inst) {

    pt_inst->pt_feat_nnid = (void *)&feat_nnid;
    pt_inst->pt_feat_vad = (void *)&feat_vad;
    pt_inst->pt_nnst_nnid = (void *)&nnst_nnid;
    pt_inst->pt_nnst_vad = (void *)&nnst_vad;
    pt_inst->pt_pcmBuf = (void *)&pcmBuf_inst;
    pt_inst->num_enroll = 4;

    // PCM_BUF init, reset
    PcmBufClass_init(&pcmBuf_inst);

    // nnvad init, reset
    NNSPClass_init(
        &nnst_vad,
        &net_nnvad, // NeuralNetClass
        &feat_vad,  // featureModule
        vad_id, feature_mean_nnvad, feature_stdR_nnvad, &glob_th_prob, &glob_count_trigger,
        &params_nn1_nnvad);

    // nnid init, reset
    NNSPClass_init(
        &nnst_nnid,
        &net_nnid,  // NeuralNetClass
        &feat_nnid, // featureModule
        nnid_id, feature_mean_nnid, feature_stdR_nnid, &glob_th_prob, &glob_count_trigger,
        &params_nn4_nnid);
}

void norm_then_ave(int32_t *outputs, int32_t *inputs, int num_sents, int len_vec) {
    float acc;
    float norm[4];

    for (int i = 0; i < num_sents; i++) {
        norm[i] = 0;
        for (int j = 0; j < len_vec; j++) {
            acc = 3.0518e-05 * ((float)inputs[i * len_vec + j]);
            norm[i] += acc * acc;
        }
        norm[i] = 1.0f / sqrtf(norm[i]);
    }

    for (int i = 0; i < len_vec; i++) {
        acc = 0;
        for (int j = 0; j < num_sents; j++) {
            acc += norm[j] * 3.0518e-05 * ((float)inputs[j * len_vec + i]);
        }
        acc *= 0.25;
        outputs[i] = (int32_t)(acc * 32768.0f);
    }
}

int16_t nnidCntrlClass_exec(nnidCntrlClass *pt_inst, int16_t *rawPCM, float *pt_corr) {
    int16_t detected;
    NNID_CLASS *pt_nnid;
    int16_t is_get_corr = 0;
    int16_t is_detected = 0;
    static int32_t embds_enroll[4 * 64];
    pt_nnid = (NNID_CLASS *)nnst_nnid.pt_state_nnid;
    for (int i = 0; i < pt_inst->total_enroll_ppls; i++)
        pt_corr[i] = pt_nnid->corr[i];

    PcmBufClass_setData(&pcmBuf_inst, rawPCM);
    detected = NNSPClass_exec(&nnst_vad, rawPCM);
    pt_inst->count_vad_trigger = (detected) ? pt_inst->count_vad_trigger + 1 : 0;

    if (pt_inst->count_vad_trigger == pt_nnid->thresh_get_corr) {

        is_get_corr = 1;
        for (int f = 0; f < pt_nnid->thresh_get_corr; f++) {
            PcmBufClass_getData(
                &pcmBuf_inst,                 // inputs
                pt_nnid->thresh_get_corr - f, // the lookback frame
                1,                            // frames to read
                rawPCM);                      // outputs
            if (pt_inst->enroll_state == test_phase) {
                if (f == pt_nnid->thresh_get_corr - 1) {
                    pt_nnid->total_enroll_ppls = pt_inst->total_enroll_ppls;
                    pt_nnid->is_get_corr = 1;
                } else
                    pt_nnid->is_get_corr = 0;
            } else {
                pt_nnid->is_get_corr = 0;
            }

            NNSPClass_exec(&nnst_nnid, rawPCM);
        }

        if (pt_inst->enroll_state == enroll_phase) {
            NNSPClass_get_nn_out(
                embds_enroll + pt_inst->acc_num_enroll * pt_nnid->dim_embd, pt_nnid->dim_embd);
            pt_inst->acc_num_enroll += 1;
            if (pt_inst->acc_num_enroll == pt_inst->num_enroll) {
                norm_then_ave(
                    pt_nnid->pt_embd + pt_inst->id_enroll_ppl * pt_nnid->dim_embd, embds_enroll,
                    pt_inst->num_enroll, pt_nnid->dim_embd);
            }
            for (int i = 0; i < pt_inst->total_enroll_ppls; i++)
                pt_nnid->corr[i] = -0.5;
            is_detected = -(0x7fff >> 1);
        } else {
            is_detected = 0x7fff >> 1;
            for (int i = 0; i < pt_inst->total_enroll_ppls; i++)
                pt_corr[i] = pt_nnid->corr[i];
        }
        NNSPClass_reset(&nnst_vad);
        NNSPClass_reset(&nnst_nnid);
    }
    return is_detected;
}
