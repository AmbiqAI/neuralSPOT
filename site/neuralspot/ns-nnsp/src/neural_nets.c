#include <stdint.h>
#include "ambiq_nnsp_const.h"
#include "minmax.h"
#if DEBUG_PRINT
    #include "extern_files.h"
#endif
#include "neural_nets.h"
#include "lstm.h"
#include "affine.h"
#include "ambiq_nnsp_debug.h"
#include "ns_ambiqsuite_harness.h"
#if ARM_OPTIMIZED == 3
    #include "basic_mve.h"
#endif
int16_t input0[MAX_SIZE_FEATURE * NUM_FEATURE_CONTEXT];
int16_t input1[MAX_SIZE_FEATURE * NUM_FEATURE_CONTEXT];

static void pointer_exchange(void **ppt1, void **ppt2);

static void pointer_exchange(void **ppt1, void **ppt2) {
    void *pt;
    pt = *ppt1;
    *ppt1 = *ppt2;
    *ppt2 = pt;
}

void NeuralNetClass_init(NeuralNetClass *pt_inst) {}

void NeuralNetClass_setDefault(NeuralNetClass *pt_inst) {
    int i, j;
    for (i = 0; i < pt_inst->numlayers; i++) {
        if (pt_inst->net_layer_type[i] == lstm) {
            for (j = 0; j < pt_inst->size_layer[i + 1]; j++) {
                pt_inst->pt_cstate[i][j] = 0;
                pt_inst->pt_hstate[i][j] = 0;
            }
        }
    }
}

void NeuralNetClass_exe(
    NeuralNetClass *pt_inst, int16_t *input, int32_t *output,
    int8_t debug_layer) // 0, 1, ..., num_layers-1
{
    int16_t dim_output, dim_input;
    int i; //, j;
    int16_t *pt0, *pt1;
    int8_t qbit_kernel;
    int8_t qbit_input;
    int8_t qbit_input_rec;
    int8_t qbit_bias;
    ACTIVATION_TYPE activation_type;
    int8_t *pt_kernel;
    int8_t *pt_kernel_rec;
    int16_t *pt_bias;
    int32_t *pt32;
    int16_t *pt16;
    int32_t *c_state;
    int16_t *h_state;
    int8_t numlayers = (debug_layer < 0) ? pt_inst->numlayers : debug_layer;
    int (*pt_layer_func)(
        int16_t *,       // pout,
        int8_t *,        // pweight,
        int8_t *,        // pweight_r,
        int16_t *,       // pbias,
        int16_t *,       // input,
        int16_t *,       // input_r,
        int32_t *,       // c_state,
        int16_t,         // rows,
        int16_t,         // cols,
        int16_t,         // cols_r,
        int16_t,         // qbit_w,
        int16_t,         // qbit_b,
        int16_t,         // qbit_i,
        int16_t,         // qbit_ir,
        ACTIVATION_TYPE, // act_type,
        void *(*)(void *, int32_t *, int));

    void *(*act_func)(void *, int32_t *, int);

    if (numlayers == 0) {
        pt16 = (int16_t *)output;
        for (i = 0; i < pt_inst->size_layer[0]; i++)
            pt16[i] = input[i];
        return;
    }

    pt0 = (int16_t *)input0;
    pt1 = (int16_t *)input1;
#if ARM_OPTIMIZED==3
    move_data_16b(
        input,
        pt0,
        pt_inst->size_layer[0]);
#else
    for (i = 0; i < pt_inst->size_layer[0]; i++) {
        pt0[i] = input[i];
    }
#endif
    for (i = 0; i < numlayers; i++) {

        dim_input = pt_inst->size_layer[i];
        dim_output = pt_inst->size_layer[i + 1];
        qbit_kernel = pt_inst->qbit_kernel[i];
        qbit_input = pt_inst->qbit_input[i];
        qbit_input_rec = pt_inst->qbit_input[i + 1];
        qbit_bias = pt_inst->qbit_bias[i];
        activation_type = pt_inst->activation_type[i];
        pt_kernel = pt_inst->pt_kernel[i];
        pt_kernel_rec = pt_inst->pt_kernel_rec[i];
        pt_bias = pt_inst->pt_bias[i];
        act_func = pt_inst->act_func[i];
        h_state = pt_inst->pt_hstate[i];
        c_state = pt_inst->pt_cstate[i];
        pt_layer_func = (int (*)(
            int16_t *, int8_t *, int8_t *, int16_t *, int16_t *, int16_t *, int32_t *, int16_t,
            int16_t, int16_t, int16_t, int16_t, int16_t, int16_t, ACTIVATION_TYPE,
            void *(*)(void *, int32_t *, int)))pt_inst->layer_func[i];
        pt_layer_func(
            pt1, pt_kernel, pt_kernel_rec, pt_bias, pt0, h_state, c_state, dim_output, dim_input,
            dim_output, // input_r
            qbit_kernel, qbit_bias, qbit_input, qbit_input_rec, activation_type, act_func);
        pointer_exchange((void **)&pt0, (void **)&pt1);
    }

    if (pt_inst->activation_type[numlayers - 1] == linear) {
        pt32 = (int32_t *)pt0;
#if ARM_OPTIMIZED==3
        move_data_16b(
            (int16_t*) pt32,
            (int16_t*) output,
            pt_inst->size_layer[numlayers] << 1 // double the size
            );
#else
        for (int j = 0; j < pt_inst->size_layer[numlayers]; j++) {
            output[j] = pt32[j];
        }
#endif
    } else {
        pt16 = (int16_t *)output;
        
#if ARM_OPTIMIZED==3
        move_data_16b(
            pt0, pt16, pt_inst->size_layer[numlayers]);
#else
        for (int j = 0; j < pt_inst->size_layer[numlayers]; j++) {
            pt16[j] = pt0[j];
        }
#endif
    }
}
