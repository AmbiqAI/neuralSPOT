#include "arm_mve.h"
#include "def_nnse_params.h"
#include "mut_model_metadata.h"
#include "mut_model_data.h"
#include "def_nn3_se.h"
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
        (const int32_t*) feature_mean_se,
        (const int32_t*) feature_stdR_se,
        FEATURE_QBIT,
        params_nn3_se.num_mfltrBank, // FEATURE_NUM_MFC
        params_nn3_se.winsize_stft, // FEATURE_WINSIZE,
        params_nn3_se.hopsize_stft, // FEATURE_HOPSIZE,
        params_nn3_se.fftsize, // FEATURE_FFTSIZE,
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
    

    ns_lp_printf("Model initialized\n");
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
    IIR_CLASS_exec(&dcrm_inst, tmp_16s, pcm_input, params_nn3_se.hopsize_stft);
    FeatureClass_execute(&FEAT_INST, tmp_16s);

    int16_t *ptfeat = FEAT_INST.normFeatContext + params_nn3_se.num_mfltrBank * (FEATURE_CONTEXT-1);

    float32_t input_scale = tflm.model_input[0]->params.scale;
    int input_zero_point = tflm.model_input[0]->params.zero_point;    

    for (int i =0; i < params_nn3_se.num_mfltrBank; i++)
    {
        float32_t val = ((float32_t) ptfeat[i] ) * scalar_norm;
        int16_t input = (int16_t) ((float32_t) val / (float32_t) input_scale + (float32_t) input_zero_point);
        tflm.model_input[0]->data.i16[i] =  input;
    }

    TfLiteStatus invoke_status = tflm.interpreter->Invoke(); 
    if (invoke_status != kTfLiteOk) {
        while (1)
        {
            example_status = NS_STATUS_FAILURE; // invoke failed, so hang
        }
    }
    float32_t output_scale = tflm.model_output[0]->params.scale;
    int output_zero_point = tflm.model_output[0]->params.zero_point;
    
    for (int i = 0; i < NN_DIM_OUT; i++) {
        float32_t out; 
        out = (float32_t) (tflm.model_output[0]->data.i16[i] - output_zero_point);
        out = out * output_scale;
        int32_t out_32s = (int32_t)(out * 32768.0f); // scale to 16-bit range
        // ns_lp_printf("%f ", out);
        tmp_16s[i] = (int16_t) MAX(MIN(out_32s, 32767), -32768); // clamp to 16-bit range
    }

    // ns_lp_printf("\n");
    // // get the tf mask
    se_post_proc(
        &FEAT_INST,
        tmp_16s,
        pcm_output,
        0,
        NN_DIM_OUT);
    return 0;
}

