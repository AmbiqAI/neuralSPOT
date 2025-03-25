#include <stdint.h>
#include "def_AudioSystem.h"
#include "AudioPipe_wrapper.h"

#include "nn_speech.h"
#include "def_nn3_se.h"
#include "feature_module.h"
#include "nnsp_identification.h"
#include "ns_ambiqsuite_harness.h"
#include "third_party/cmsis_nn/Include/arm_nnsupportfunctions.h"

NNSPClass NNSP_INST; // 10ms
FeatureClass FEAT_INST;

int AudioPipe_wrapper_init(void)
{
    NNSPClass_init(
        &NNSP_INST,
        (void *)&net_se,
        (void *)&FEAT_INST,
        se_id,
        feature_mean_se,
        feature_stdR_se,
        (int16_t*) 0,
        (int16_t*) 0,
        &params_nn3_se);
    return 0;
}

int AudioPipe_wrapper_reset(void)
{
    NNSPClass_reset(&NNSP_INST);
    return 0;
}

int AudioPipe_wrapper_frameProc(
        int16_t *pcm_input,
        int16_t *pcm_output)
{

    NNSPClass_exec(&NNSP_INST, pcm_input);
    arm_memcpy_s8(
        (int8_t*) pcm_output,
        (int8_t*) NNSP_INST.pt_se_out,
        SAMPLES_IN_FRAME * sizeof(int16_t));

    return 0;
}