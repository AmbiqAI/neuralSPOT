#include "fuse2mel_init.h"
#include "ns_core.h"
#include "ns_peripherals_power.h"
#include "mut_model_metadata_f2m.h"
#include "validator_mem_f2m.h"
#include "tflm_ns_model.h"

int fuse2mel_init(ns_model_state_t *self) {
    static ns_timer_config_t tickTimer = {
        .api = &ns_timer_V1_0_0,
        .timer = NS_TIMER_COUNTER,
        .enableInterrupt = false,
    };
    self->runtime = TFLM;
    ns_mem_init_defaults_f2m();

    self->tickTimer = &tickTimer;

    self->model_array = ns_mem_model_ptr_f2m();
    ns_printf("Model ptr: 0x%08x\n", 
        (uintptr_t)self->model_array);

    self->arena = ns_mem_arena_ptr_f2m();
    self->arena_size = ns_mem_arena_size_f2m();
    ns_printf("Arena ptr: 0x%08x, size: %d bytes\n", 
        (uintptr_t)self->arena, self->arena_size);
    // Allocate resource variable arena
    static constexpr int kVarArenaSize = 4 * (TFLM_VALIDATOR_MAX_RESOURCE_VARIABLES_F2M + 1) * sizeof(tflite::MicroResourceVariables);
    alignas(16) static uint8_t s_var_arena[kVarArenaSize];
    self->rv_arena = s_var_arena;
    self->rv_arena_size = kVarArenaSize;
    self->rv_count = TFLM_VALIDATOR_MAX_RESOURCE_VARIABLES_F2M;
    self->numInputTensors = 1;
    self->numOutputTensors = 1;
    ns_lp_printf("Calling tflm_validator_model_init...\n");
    int rc = tflm_validator_model_init_f2m(self);
    if (rc != 0) { ns_lp_printf("Model init failed\n"); return 1; }
    // Set input/output tensor counts (replace with actual values if known)
    

    ns_lp_printf("Model has %d inputs and %d outputs\n", self->numInputTensors, self->numOutputTensors);
    int16_t nn_input_dim;
    int16_t nn_output_dim;
    for (int m = 0; m < (int) self->numInputTensors; m++) {
        ns_lp_printf("Input tensor %d has %d bytes\n",
            m, self->interpreter->input(m)->bytes);
        ns_lp_printf("input scale=%f\n",
            self->interpreter->input(m)->params.scale);
        ns_lp_printf("input zero_point=%d\n",
            self->interpreter->input(m)->params.zero_point);
        ns_lp_printf("input dims=%d\n",
            self->interpreter->input(m)->dims->size);
        nn_input_dim = 1;
        for (int i = 0; i < self->interpreter->input(m)->dims->size; i++) {
            nn_input_dim *= self->interpreter->input(m)->dims->data[i];
            ns_lp_printf("input dim[%d]=%d\n",
                i, self->interpreter->input(m)->dims->data[i]);
        }
    }

    for (int m = 0; m < (int) self->numOutputTensors; m++) {
        ns_lp_printf("Output tensor %d has %d bytes\n",
            m, self->interpreter->output(m)->bytes);
        ns_lp_printf("output scale=%f\n",
            self->interpreter->output(m)->params.scale);
        ns_lp_printf("output zero_point=%d\n",
            self->interpreter->output(m)->params.zero_point);
        ns_lp_printf("output dims=%d\n",
            self->interpreter->output(m)->dims->size);
        nn_output_dim = 1;
        for (int i = 0; i < self->interpreter->output(m)->dims->size; i++) {
            nn_output_dim *= self->interpreter->output(m)->dims->data[i];
            ns_lp_printf("output dim[%d]=%d\n",
                i, self->interpreter->output(m)->dims->data[i]);
        }
    }
    self->interpreter->Reset();
    ns_lp_printf("Model initialization complete.\n");
    return 0;
}
