#ifndef __NEURAL_NETS_H__
#define __NEURAL_NETS_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include "activation.h"
typedef enum { fc, lstm } NET_LAYER_TYPE;

typedef struct {
    int8_t numlayers;
    int16_t size_layer[11];
    NET_LAYER_TYPE net_layer_type[10];
    int8_t qbit_kernel[10];
    int8_t qbit_input[10];
    int8_t qbit_bias[10];
    ACTIVATION_TYPE activation_type[10];
    int32_t *pt_cstate[10];
    int16_t *pt_hstate[10];
    void *(*act_func[10])(void *, int32_t *, int);
    int *(*layer_func[10])();
    int8_t *pt_kernel[10];
    int16_t *pt_bias[10];
    int8_t *pt_kernel_rec[10];

} NeuralNetClass;

void NeuralNetClass_init(NeuralNetClass *pt_inst);

void NeuralNetClass_setDefault(NeuralNetClass *pt_inst);

void NeuralNetClass_exe(
    NeuralNetClass *pt_inst, int16_t *input, int32_t *output, int8_t debug_layer);

#ifdef __cplusplus
}
#endif
#endif
