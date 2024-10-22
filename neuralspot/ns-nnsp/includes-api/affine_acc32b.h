#ifndef __AFFINE_32B_H__
#define __AFFINE_32B_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "activation.h"
/*
        Note: this function might cause overflow. To avoid overlow, consider to use
                  the less computational efficiency "affine_Krows_8x16".

        "affine_Krows_8x16_acc32b" is an affine (matrix*vec+bias) op .
        It uses the same memory arrangement, fetching scheme, MAC and ARM-intrinsics as
        "arm_fully_connected_mat_q7_vec_q15_opt" in CMSIS-NN.

        The accumulator for MAC is in 32-bit. This might cause overflow.

        "affine_Krows_8x16_acc32" is the main kernel for FC & RNN-like NN layers. We do not directly
   apply "arm_fully_connected_mat_q7_vec_q15_opt" in CMSIS-NN since
        1. This function with slightly interface modification is easier to generalize to FC or
   RNN-like NN layers
        2. We further optimize it if  N % 4 != 0, where N is the number of rows in the matrix.
           This will have a slight gain on the MCPS improvement
*/
int affine_Krows_8x16_acc32b(
    int16_t dim_output, int16_t **pp_output, int8_t **pp_kernel, int16_t **pp_bias, int16_t *input,
    int16_t dim_input, int16_t qbit_kernel, int16_t qbit_bias, int16_t qbit_input,
    int32_t *pt_accum, int8_t is_out, void *(*act)(void *, int32_t *, int));

int rc_Krows_8x16_acc32b(
    int16_t dim_output, int16_t **pp_output, int8_t **pp_kernel, int8_t **pp_kernel_rec,
    int16_t **pp_bias, int16_t *input, int16_t *input_rec, int16_t dim_input, int16_t dim_input_rec,
    int16_t qbit_kernel, int16_t qbit_bias, int16_t qbit_input, int16_t qbit_input_rec,
    void *(*act)(void *, int32_t *, int));

int fc_8x16_acc32b(
    int16_t *p_output, int8_t *p_kernel, int8_t *p_kernel_rec, int16_t *p_bias, int16_t *input,
    int16_t *input_rec, int32_t *c_state, int16_t dim_output, int16_t dim_input,
    int16_t dim_input_rec, int16_t qbit_kernel, int16_t qbit_bias, int16_t qbit_input,
    int16_t qbit_input_rec, ACTIVATION_TYPE act_type, void *(*act)(void *, int32_t *, int));

int rc_8x16_acc32b(
    int16_t *p_output, int8_t *p_kernel, int8_t *p_kernel_rec, int16_t *p_bias, int16_t *input,
    int16_t *input_rec, int16_t dim_output, int16_t dim_input, int16_t dim_input_rec,
    int16_t qbit_kernel, int16_t qbit_bias, int16_t qbit_input, int16_t qbit_input_rec,
    ACTIVATION_TYPE act_type, void *(*act)(void *, int32_t *, int));

void shift_32b(int32_t *x, int8_t shift, int len);

#ifdef __cplusplus
}
#endif
#endif
