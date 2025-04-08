/**
 * @file nnaedrelunopad_example.cc
 * @author Carlos Morales
 * @brief Minimal application instantiating a TFLM model, feeding it
 * a test input tensor, and checking the result
 * @version 0.1
 * @date 2023-02-28
 *
 * @copyright Copyright (c) 2023
 *
 */
#define NURALSPOT 1
#include "ns_perf_profile.h"
#include "ns_timer.h"
#include <arm_math.h>
// #include "nnaedrelunopad_api.h"
#include "nnse_model.h"
// #include "nnaedrelunopad_example_tensors.h"
#include "ns_model.h"
#include "feat_proc.h"
#include "params_feat.h"
#if NURALSPOT==1
#include "ns_ambiqsuite_harness.h"
#include "ns_energy_monitor.h"
#include "ns_peripherals_power.h"
#include <arm_mve.h>
#include <arm_math.h>
#endif

extern int16_t inputs[];
extern int16_t outputs[];
/*
Notes:
- This example is compiled along with startup_gcc and linker_script to generate an
  Ambiq EVB image (axf and bin). However, the following code doesn't make any assumptions
  about platform.

- This example is meant to be walked through using a debugger such as GDB. It doesn't
  have I/O code so it doesn't actual input or output anything.

- It includes sample input and output tensors - the examples operate on these without
  regard to datatype (it treats everything as int8 arrays). The tensors are TF tensor
  types, with all dtype, dimensions, etc, preserved as usual.
*/

// TFLM Config
static ns_model_state_t model;
volatile int example_status = 0; // Prevent the compiler from optimizing out while loops

// Feature class instance
extern uint32_t g_ui32TrimVer;
featClass feat_inst;
extern int16_t inputs_16b[]; // 1 second of 16kHz audio

int
main(void) {
    int16_t transpose = TRANSPOSE_NN_INPUT;
    static ns_perf_counters_t pp;
#if NURALSPOT==1
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};

    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");

#ifdef ENERGY_MONITOR_ENABLE
    // This is for measuring power using an external power monitor such as
    // Joulescope - it sets GPIO pins so the state can be observed externally
    // to help line up the waveforms. It has nothing to do with AI...
    ns_init_power_monitor_state();
#endif
    ns_set_power_monitor_state(NS_IDLE); // no-op if ns_init_power_monitor_state not called
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed.\n");
    // NS_TRY(ns_set_performance_mode(NS_MINIMUM_PERF), "Set CPU Perf mode failed.");

#ifdef LOWEST_POWER_MODE
    // No printing enabled at all - use this to measure power
#else
    ns_itm_printf_enable();
#endif
#endif
    // static float32_t inputs_nn[NUM_MEL_BINS * NUM_FRAMES]; // features in one second of audio

    featClass_init(&feat_inst);

    // Initialize the model, get handle if successful
    int status = nnse_minimal_init(&model); // model init with minimal defaults

    if (status == nnaedrelunopad_STATUS_FAILURE) {
        while (1)
            example_status = nnaedrelunopad_STATUS_INIT_FAILED; // hang
    }
#if NURALSPOT==1    
    ns_lp_printf("Model initialized\n");
    ns_lp_printf("num_frames=%d\n", NUM_FRAMES);
    
#endif

    // At this point, the model is ready to use - init and allocations were successful
    // Note that the model handle is not meant to be opaque, the structure is defined
    // in ns_model.h, and contains state, config details, and model structure information

    // Get data about input and output tensors
    int numInputs = model.numInputTensors;
    int numOutputs = model.numOutputTensors;
    
    #if NURALSPOT==1
    ns_lp_printf("Model has %d inputs and %d outputs\n", numInputs, numOutputs);
    ns_lp_printf("Input tensor 0 has %d bytes\n", model.model_input[0]->bytes);
    ns_lp_printf("Output tensor 0 has %d bytes\n", model.model_output[0]->bytes);
    ns_lp_printf("input scale=%f\n", model.model_input[0]->params.scale);
    ns_lp_printf("input zero_point=%d\n", model.model_input[0]->params.zero_point);
    
    ns_lp_printf("input dims=%d\n", model.model_input[0]->dims->size);
    int input_dim = 1;
    for (int i = 0; i < model.model_input[0]->dims->size; i++) {
        input_dim *= model.model_input[0]->dims->data[i];
        ns_lp_printf("input dim[%d]=%d\n", i, model.model_input[0]->dims->data[i]);
    }
    int output_dim=1;
    for (int i = 0; i < model.model_output[0]->dims->size; i++) {
        output_dim*= model.model_output[0]->dims->data[i];
        ns_lp_printf("output dim[%d]=%d\n", i, model.model_output[0]->dims->data[i]);
    }
#endif
    float input_scale = model.model_input[0]->params.scale;
    int input_zero_point = model.model_input[0]->params.zero_point;    
    int nbit = 16;

        
    // Execute the model
    int16_t *pt_inputs=inputs;
    int16_t *pt_outputs=outputs;
    int32_t e;
    int32_t err=0;
    for (int f=0; f < 2221; f++)
    {
        
        for (int i =0; i < input_dim; i++)
        {
            model.model_input[0]->data.i16[i] = pt_inputs[i];
        }
        TfLiteStatus invoke_status = model.interpreter->Invoke(); 
        if (invoke_status != kTfLiteOk) {
            while (1)
            {
                example_status = nnaedrelunopad_STATUS_FAILURE; // invoke failed, so hang
            }
        }
        for (int i = 0; i < output_dim; i++) {
            e = (int32_t) model.model_output[0]->data.i16[i]-(int32_t)pt_outputs[i];
            if (e < 0)
                e = -e;
            // if (e !=0)
            //     ns_lp_printf("output[%d]=%d, %d\n", i, model.model_output[0]->data.i16[i], pt_outputs[i]);
            err=MAX(err,e);
        }
        
        pt_inputs += input_dim;
        pt_outputs += output_dim;
        
    }
    ns_lp_printf("Max err=%d\n", err);
}
