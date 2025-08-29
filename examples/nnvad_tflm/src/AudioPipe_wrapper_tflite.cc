#include "arm_mve.h"
#include "def_nnvad_params.h"
#include "mut_model_metadata.h"
#include "mut_model_data.h"
#include "def_nn1_nnvad.h"
#include "tflm_ns_model.h"
#include <stdint.h>
#include "AudioPipe_wrapper.h"

#include "feature_module.h"
#include "ns_ambiqsuite_harness.h"
#include "nn_speech.h"
#include "iir.h"
#include "third_party/ns_cmsis_nn/Include/arm_nnsupportfunctions.h"
#include <math.h>
extern int tflm_validator_model_init(ns_model_state_t *ms);
#define FEATURE_QBIT 15 // Q-bit for feature extraction
#define NN_DIM_OUT 2 // Number of output classes for the NN model
#define NN_DIM_IN 160 // Number of input features for the NN model
// Feature class instance

PARAMS_NNSP *pt_param = &params_nn1_nnvad;
// TFLM Config
static ns_model_state_t tflm;
extern int16_t inputs[];
extern int16_t outputs[];
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
static constexpr int kVarArenaSize =
    4 * (TFLM_VALIDATOR_MAX_RESOURCE_VARIABLES + 1) * sizeof(tflite::MicroResourceVariables);
alignas(16) static uint8_t var_arena[kVarArenaSize];
// Resource Variable Arena - always in TCM for now
// static constexpr int kVarArenaSize = 4096;
//     // 4 * (TFLM_VALIDATOR_MAX_RESOURCE_VARIABLES + 1) * sizeof(tflite::MicroResourceVariables);
// alignas(16) static uint8_t var_arena[kVarArenaSize];
// Validator Stuff

volatile int example_status = 0; // Prevent the compiler from optimizing out while loops



int16_t vad_trigger_counts=0;
int16_t nn_dim_input = 1;
int16_t nn_dim_output = 1;
int AudioPipe_wrapper_init(void)
{ 
    ns_model_state_t *pt_tflm = &tflm;

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
    int numInputs = pt_tflm->interpreter->inputs_size();
    int numOutputs = pt_tflm->interpreter->outputs_size();

    ns_lp_printf("Model has %d inputs and %d outputs\n", numInputs, numOutputs);
        
    for (int m = 0; m < numInputs; m++) {

        ns_lp_printf("Input tensor %d has %d bytes\n", m, pt_tflm->interpreter->input(m)->bytes);

        ns_lp_printf("input scale=%f\n", pt_tflm->interpreter->input(m)->params.scale);
        ns_lp_printf("input zero_point=%d\n", pt_tflm->interpreter->input(m)->params.zero_point);

        ns_lp_printf("input dims=%d\n", pt_tflm->interpreter->input(m)->dims->size);
        nn_dim_input = 1;
        for (int i = 0; i < pt_tflm->interpreter->input(m)->dims->size; i++) {
            nn_dim_input *= pt_tflm->interpreter->input(m)->dims->data[i];
            ns_lp_printf("input dim[%d]=%d\n", i, pt_tflm->interpreter->input(m)->dims->data[i]);
        }
    }
    
    for (int m = 0; m < numOutputs; m++) {
        ns_lp_printf("Output tensor %d has %d bytes\n", m, pt_tflm->interpreter->output(m)->bytes);
        nn_dim_output = 1;
        for (int i = 0; i < pt_tflm->interpreter->output(m)->dims->size; i++) {
            nn_dim_output *= pt_tflm->interpreter->output(m)->dims->data[i];
            ns_lp_printf("output dim[%d]=%d\n", i, pt_tflm->interpreter->output(m)->dims->data[i]);
        }
        // self->nn_dim_out = output_dim; // Set the number of output dimensions for the model
    }

    TfLiteStatus invoke_status = tflm.interpreter->Invoke(); 
    if (invoke_status != kTfLiteOk) {
        while (1)
        {
            example_status = NS_STATUS_FAILURE; // invoke failed, so hang
        }
    }
    ns_lp_printf("Inference success\n");
    return 0;
}

int AudioPipe_wrapper_reset(void)
{
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
    ns_model_state_t *pt_tflm = &tflm;
    // ns_printf("dim input: %d, dim output: %d\n", nn_dim_input, nn_dim_output);
    // for (int i =0; i < nn_dim_input; i++)
    // {
    //     pt_tflm->interpreter->input(0)->data.i16[i] = inputs[i];
    //     if (i < 10)
    //     {
    //         ns_printf("input %d: %d\n", i, inputs[i]);
    //     }
    // }

    TfLiteStatus invoke_status = tflm.interpreter->Invoke(); 
    if (invoke_status != kTfLiteOk) {
        while (1)
        {
            example_status = NS_STATUS_FAILURE; // invoke failed, so hang
        }
    }

    // int max_err = 0;
    // for (int i =0; i < nn_dim_output; i++)
    // {
    //     int err = (int) pt_tflm->interpreter->output(0)->data.i16[i] - (int) outputs[i];

    //     if (err < 0) err = -err;

    //     if (err > max_err) max_err = err;

    //     if (i < MIN(10, nn_dim_output))
    //     {
    //         ns_printf("output %d: %d, %d (ref)\n",
    //              i,
    //              pt_tflm->interpreter->output(0)->data.i16[i],
    //              outputs[i]);
    //     }
    // }

    // ns_printf("max err: %d\n", max_err);
    return 0;
}

