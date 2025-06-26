#include <stdint.h>
#include <string.h>
#include "ns_ambiqsuite_harness.h"
#include "PcmBufClass.h"
#include "minmax.h"
//#include "nnCntrlClass.h"
#define NUM_FRS_VBUF 100
#define SAMPLES_FRM_NNCNTRL_CLASS 160
int16_t PCM_BUFFER[SAMPLES_FRM_NNCNTRL_CLASS * NUM_FRS_VBUF];

void PcmBufClass_init(PcmBufClass *pt_inst) {
    /*
    Initialize PcmBuffer class
    */
    pt_inst->pcm_buffer = PCM_BUFFER;
    pt_inst->num_frs = NUM_FRS_VBUF;
    pt_inst->smpls_fr = SAMPLES_FRM_NNCNTRL_CLASS;
}

void PcmBufClass_reset(PcmBufClass *pt_inst) {
    /*
    Reset PcmBuffer class
    */
    int i;
    for (i = 0; i < (pt_inst->num_frs * pt_inst->smpls_fr); i++)
        pt_inst->pcm_buffer[i] = 0;
    pt_inst->idx_set = 0;
    pt_inst->idx_data_latest = pt_inst->num_frs - 1;
}

void PcmBufClass_setData(PcmBufClass *pt_inst, int16_t *pcm_input) {
    /*
    Put data from microphone to pcm buffer

    Inputs:
            pt_inst   : instance pointer
            pcm_input : data from microphone
    */
    int i;
    int16_t *pt_dst;
    pt_dst = pt_inst->pcm_buffer + pt_inst->idx_set * pt_inst->smpls_fr;
    for (i = 0; i < pt_inst->smpls_fr; i++) {
        *pt_dst++ = pcm_input[i];
    }

    pt_inst->idx_data_latest = pt_inst->idx_set;
    pt_inst->idx_set = (pt_inst->idx_set + 1) % pt_inst->num_frs;
}

void PcmBufClass_getData(
    PcmBufClass *pt_inst, int16_t lookbk_frs, int16_t fetching_frames, int16_t *pcm_out) {
    /*
    Fetch data from pcm buffer

    Inputs:
            pt_inst: instance pointer
            lookbk_frs: the number of frames to look back
            fetching_frames : the number of frames to fetch
    Output:
            pcm_out: pcm from pcm buffer
    */
    // int i;
    int16_t *pt_src, *pt_dst;
    int16_t start;
    int16_t num_fw;

    start = (pt_inst->idx_data_latest - lookbk_frs) % pt_inst->num_frs;
    start = (start >= 0) ? start : start + pt_inst->num_frs;

    num_fw = fetching_frames;
    num_fw = MIN(num_fw, pt_inst->num_frs - start) * pt_inst->smpls_fr;
    pt_src = pt_inst->pcm_buffer + start * pt_inst->smpls_fr;
    pt_dst = pcm_out;

    memcpy(pt_dst, pt_src, num_fw * sizeof(int16_t));
    // for (i = 0; i < num_fw; i++)
    //     *pt_dst++ = pt_src[i];

    memcpy(
        pt_dst + num_fw, pt_inst->pcm_buffer,
        (fetching_frames * pt_inst->smpls_fr - num_fw) * sizeof(int16_t));
    // for (i = 0; i < (fetching_frames * pt_inst->smpls_fr - num_fw); i++)
    //     *pt_dst++ = pt_inst->pcm_buffer[i];
}
