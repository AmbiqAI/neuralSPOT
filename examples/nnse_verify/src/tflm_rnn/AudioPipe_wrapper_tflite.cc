#include "arm_mve.h"
#include "def_nnse_params.h"
#include "mut_model_metadata.h"
#include "mut_model_data.h"
#include "tflm_ns_model.h"
#include <stdint.h>
#include "AudioPipe_wrapper.h"

#include "feature_module.h"
#include "ns_ambiqsuite_harness.h"
#include "nn_speech.h"
#include "iir.h"
extern int tflm_validator_model_init(ns_model_state_t *ms);

// Feature class instance
FeatureClass FEAT_INST;
IIR_CLASS dcrm_inst;
// normalized feature parameters of NNSE:
const uint32_t feature_mean_se1[] = {0xfffea3d8, 0xfffef7f1, 0xffff3087, 0xffff541a, 0xffff6bf2, 0xffff71bc, 0xffff7a3f, 0xffff7877, 0xffff6d40, 0xffff6282, 0xffff5c43, 0xffff58f5, 0xffff5781, 0xffff7aad, 0xffff765d, 0xffff4b04, 0xffff68ee, 0xffff60df, 0xffff53a9, 0xffff4bf1, 0xffff41db, 0xffff5067, 0xffff312f, 0xffff2a24, 0xffff399b, 0xffff32a9, 0xffff2bf0, 0xffff26e3, 0xffff2297, 0xffff1d2f, 0xffff2a8f, 0xffff27b3, 0xffff24df, 0xffff2303, 0xffff1dd8, 0xffff2527, 0xffff13b0, 0xffff0d33, 0xffff146f, 0xffff0ed1, 0xffff1327, 0xffff0f1d, 0xffff0c5b, 0xffff09c7, 0xffff06db, 0xffff0a9f, 0xffff04ee, 0xffff007f, 0xfffefc2f, 0xfffefd6f, 0xfffef784, 0xfffef1e7, 0xfffef200, 0xfffeebde, 0xfffee6e5, 0xfffee4c2, 0xfffedd5a, 0xfffed56b, 0xfffed284, 0xfffecb48, 0xfffec7be, 0xfffec4de, 0xfffebee1, 0xfffeb929, 0xfffeb4cb, 0xfffeb486, 0xfffeb34f, 0xfffeae3c, 0xfffeabeb, 0xfffea69e, 0xfffe9da0, 0xfffe788b, };
const uint32_t feature_stdR_se1[] = {0x0000601b, 0x0000560b, 0x0000523c, 0x00004f33, 0x00004e2e, 0x00004e7c, 0x00004ddf, 0x00004df7, 0x00004efe, 0x0000500d, 0x00005082, 0x0000505d, 0x00005019, 0x0000500d, 0x0000502e, 0x00005086, 0x000050e3, 0x00005155, 0x00005217, 0x000052ac, 0x00005365, 0x00005396, 0x0000552d, 0x000055bb, 0x000055b9, 0x00005622, 0x0000565a, 0x0000568b, 0x000056a4, 0x0000571e, 0x000056b9, 0x000056f8, 0x00005755, 0x0000579e, 0x0000580b, 0x000057fe, 0x00005917, 0x0000599c, 0x00005969, 0x000059f3, 0x000059b7, 0x000059ee, 0x000059c8, 0x000059ae, 0x000059ae, 0x0000593a, 0x00005979, 0x000059aa, 0x00005a0d, 0x000059de, 0x000059f6, 0x00005a26, 0x000059de, 0x00005a12, 0x000059eb, 0x00005962, 0x00005969, 0x0000595f, 0x00005902, 0x000058f0, 0x0000587b, 0x00005815, 0x0000581c, 0x0000585d, 0x000058ab, 0x00005877, 0x00005861, 0x000058c7, 0x000058c7, 0x0000593c, 0x000059f9, 0x00005f46, };

// TFLM Config
static ns_model_state_t tflm;

// TF Tensor Arena

#if (TFLM_MODEL_LOCATION == NS_AD_PSRAM)
    unsigned char *mut_model;
#endif

#if (TFLM_ARENA_LOCATION == NS_AD_PSRAM)
    static uint8_t *tensor_arena;
    static constexpr int kTensorArenaSize = 1024 * 1024 * 10; // 10MB
#else
    static constexpr int kTensorArenaSize = 1024 * TFLM_VALIDATOR_ARENA_SIZE;
    // #ifdef AM_PART_APOLLO3
    //     // Apollo3 doesn't have AM_SHARED_RW
    //     alignas(16) static uint8_t tensor_arena[kTensorArenaSize];
    // #else // not AM_PART_APOLLO3
        #if (TFLM_ARENA_LOCATION == NS_AD_SRAM)
            #ifdef keil6
            // Align to 16 bytes
            AM_SHARED_RW __attribute__((aligned(16))) static uint8_t tensor_arena[kTensorArenaSize];
            #else
            AM_SHARED_RW alignas(16) static uint8_t tensor_arena[kTensorArenaSize];
            #endif
        #else
            NS_PUT_IN_TCM alignas(16) static uint8_t tensor_arena[kTensorArenaSize];
        #endif
    // #endif
#endif

// Resource Variable Arena - always in TCM for now
static constexpr int kVarArenaSize = 4096;
    // 4 * (TFLM_VALIDATOR_MAX_RESOURCE_VARIABLES + 1) * sizeof(tflite::MicroResourceVariables);
alignas(16) static uint8_t var_arena[kVarArenaSize];
// Validator Stuff

volatile int example_status = 0; // Prevent the compiler from optimizing out while loops

extern const int16_t stft_win_coeff_w480_h160[];
int AudioPipe_wrapper_init(void)
{ 
    FeatureClass_construct(
        &FEAT_INST,
        (const int32_t*) feature_mean_se1,
        (const int32_t*) feature_stdR_se1,
        FEATURE_QBIT,
        FEATURE_NUM_MFC,
        FEATURE_WINSIZE,
        FEATURE_HOPSIZE,
        FEATURE_FFTSIZE,
        stft_win_coeff_w480_h160);

    IIR_CLASS_init(&dcrm_inst);
    
    // Initialize the model, get handle if successful
    
    tflm.runtime = TFLM;
    tflm.model_array = mut_model;
    tflm.arena = tensor_arena;
    tflm.arena_size = kTensorArenaSize;
    tflm.rv_arena = var_arena;
    tflm.rv_arena_size = kVarArenaSize;
    tflm.rv_count = TFLM_VALIDATOR_MAX_RESOURCE_VARIABLES;
    tflm.numInputTensors = 1;
    tflm.numOutputTensors = 1;

    int status = tflm_validator_model_init(&tflm); // model init with minimal defaults

    if (status == NS_STATUS_FAILURE) {
        while (1)
            example_status = NS_STATUS_INIT_FAILED; // hang
    }

    ns_lp_printf("Model initialized\n");

    // Get data about input and output tensors
    int numInputs = tflm.numInputTensors;
    int numOutputs = tflm.numOutputTensors;
    
    ns_lp_printf("Model has %d inputs and %d outputs\n", numInputs, numOutputs);
    ns_lp_printf("Input tensor 0 has %d bytes\n", tflm.model_input[0]->bytes);
    ns_lp_printf("Output tensor 0 has %d bytes\n", tflm.model_output[0]->bytes);
    ns_lp_printf("input scale=%f\n", tflm.model_input[0]->params.scale);
    ns_lp_printf("input zero_point=%d\n", tflm.model_input[0]->params.zero_point);
    
    ns_lp_printf("input dims=%d\n", tflm.model_input[0]->dims->size);
    int input_dim = 1;
    for (int i = 0; i < tflm.model_input[0]->dims->size; i++) {
        input_dim *= tflm.model_input[0]->dims->data[i];
        ns_lp_printf("input dim[%d]=%d\n", i, tflm.model_input[0]->dims->data[i]);
    }
    int output_dim=1;
    for (int i = 0; i < tflm.model_output[0]->dims->size; i++) {
        output_dim*= tflm.model_output[0]->dims->data[i];
        ns_lp_printf("output dim[%d]=%d\n", i, tflm.model_output[0]->dims->data[i]);
    }
    
    return 0;
}

int AudioPipe_wrapper_reset(void)
{
    FeatureClass_setDefault(&FEAT_INST);
    IIR_CLASS_reset(&dcrm_inst);
    return 0;
}

int AudioPipe_wrapper_frameProc(
        int16_t *pcm_input,
        int16_t *pcm_output)
{
    /* feature extraction
    1. iir for dc remove
    2. melspectrogram
    */

    static int16_t tmp_16s[300];
    static float scalar_norm = 1.0 / (float) (1 << FEATURE_QBIT);

    int16_t *ptfeat = FEAT_INST.normFeatContext + FEATURE_NUM_MFC * (FEATURE_CONTEXT-1);

    float input_scale = tflm.model_input[0]->params.scale;
    int input_zero_point = tflm.model_input[0]->params.zero_point;    

    for (int i =0; i < FEATURE_NUM_MFC; i++)
    {
        tflm.model_input[0]->data.i16[i] = pcm_input[i];
    }

    TfLiteStatus invoke_status = tflm.interpreter->Invoke(); 
    if (invoke_status != kTfLiteOk) {
        while (1)
        {
            example_status = NS_STATUS_FAILURE; // invoke failed, so hang
        }
    }
    float32_t max_amp=0;
    float32_t min_amp=0;
    float32_t output_scale = tflm.model_output[0]->params.scale;
    int output_zero_point = tflm.model_output[0]->params.zero_point;

    for (int i = 0; i < NN_DIM_OUT; i++) {
        
        pcm_output[i] = (int16_t) (tflm.model_output[0]->data.i16[i]);
        float32_t out; 
        out = (float32_t) (pcm_output[i] - output_zero_point);
        out = out * output_scale;
        max_amp = MAX(max_amp, out);
        min_amp = MIN(min_amp, out);
    }
    if (max_amp > 1)
        ns_lp_printf("Max amp=%f\n", max_amp);
    if (min_amp < -1)
        ns_lp_printf("Min amp=%f\n", min_amp);
    return 0;
}

