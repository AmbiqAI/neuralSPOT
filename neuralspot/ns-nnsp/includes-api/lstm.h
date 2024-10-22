#ifndef __LSTM_H__
#define __LSTM_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include "activation.h"
// typedef struct
//{
//	int16_t h_states[130];
//	int32_t c_states[130];
// }LSTM_LAYER;

int lstm_8x16(
    int16_t *p_output, int8_t *p_kernel, int8_t *p_kernel_rec, int16_t *p_bias, int16_t *input,
    int16_t *h_state, int32_t *c_state, int16_t dim_output, int16_t dim_input,
    int16_t dim_input_rec, int16_t qbit_kernel, int16_t qbit_bias, int16_t qbit_input,
    int16_t qbit_input_rec, ACTIVATION_TYPE act_type, void *(*act)(void *, int32_t *, int));

int lstm_8x16_acc32b(
    int16_t *p_output, int8_t *p_kernel, int8_t *p_kernel_rec, int16_t *p_bias, int16_t *input,
    int16_t *h_state, int32_t *c_state, int16_t dim_output, int16_t dim_input,
    int16_t dim_input_rec, int16_t qbit_kernel, int16_t qbit_bias, int16_t qbit_input,
    int16_t qbit_input_rec, ACTIVATION_TYPE act_type, void *(*act)(void *, int32_t *, int));
#ifdef __cplusplus
}
#endif
#endif
