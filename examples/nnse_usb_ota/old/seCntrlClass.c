#include <stdint.h>
#include "feature_module.h"
#include "def_nn3_se.h"
// #include "PcmBufClass.h"
#include "nn_speech.h"
#include "seCntrlClass.h"
#include "ParamsNNCntrl.h"
#include "ns_ambiqsuite_harness.h"
#include "neural_nets.h"

#ifdef AUDIODEBUG
    #include "SEGGER_RTT.h"
#endif

NNSPClass NNSP_INST; // 10ms

FeatureClass FEAT_INST;

// PcmBufClass PCMBUF_INST;

void seCntrlClass_init(seCntrlClass *pt_inst) {
    /*
    s2iCntrlClass_init: initialization of nnCntrlClass

        Inputs:
                *pt_inst        :  instance pointer
    */
    pt_inst->pt_nnsp = (void *)&NNSP_INST;

    // S2I
    pt_inst->Params.frs_vbufBk_s2i = FRS_VBUFBK_S2IRNN;
    pt_inst->Params.thresh_prob_s2i = THRESHOLD_MASK_S2IRNN;
    pt_inst->Params.thresh_cnts_s2i = THRESHOLD_CNTS_S2IRNN;

    // PcmBufClass_init(&PCMBUF_INST);

    NNSPClass_init(
        (NNSPClass *)pt_inst->pt_nnsp, (void *)&net_se, (void *)&FEAT_INST, 3, feature_mean_se,
        feature_stdR_se, &pt_inst->Params.thresh_prob_s2i, &pt_inst->Params.thresh_cnts_s2i,
        &params_nn3_se);
}

void seCntrlClass_reset(seCntrlClass *pt_inst) {
    /*
    Reset the states in s2iCntrlClass

        Inputs:
                pt_inst:  instance pointer
    */
    // PcmBufClass_reset(&PCMBUF_INST);

    NNSPClass_reset((NNSPClass *)pt_inst->pt_nnsp);
}

void seCntrlClass_exec(seCntrlClass *pt_inst, int16_t *data_fr, int16_t *se_output) {
    /*
    execution of each frame data:

        Inputs:
                pt_inst : instance pointer
                indata  : frame data
    */
    // static int16_t pcmbuf_chunk[SAMPLES_FRM_NNCNTRL_CLASS];
    NNSPClass *pt_nnsp = (NNSPClass *)pt_inst->pt_nnsp;
    // int16_t detected = 0;

    // PcmBufClass_setData(&PCMBUF_INST, data_fr); // put data to voice buffer
    // PcmBufClass_getData(
    //         &PCMBUF_INST,
    //         0,
    //         1,
    //         pcmbuf_chunk); // fetch input data to NN from voice bufffer

    // detected = NNSPClass_exec(pt_nnsp, data_fr);
    NNSPClass_exec(pt_nnsp, data_fr);
    memcpy(se_output, pt_nnsp->pt_se_out, SAMPLES_FRM_NNCNTRL_CLASS * sizeof(int16_t));
    // for (int i = 0; i < SAMPLES_FRM_NNCNTRL_CLASS; i++)
    //     se_output[i] = pt_nnsp->pt_se_out[i];
}
