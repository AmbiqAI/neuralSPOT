#include "ambiq_stdint.h"
#include "affine.h"
#include "affine_acc32b.h"
#include "lstm.h"
#include "minmax.h"
#if DEBUG_PRINT
    #include "extern_files.h"
#endif
__attribute__((aligned(16))) int16_t I_STATES[4];
__attribute__((aligned(16))) int16_t J_STATES[4];
__attribute__((aligned(16))) int16_t F_STATES[4];
__attribute__((aligned(16))) int16_t O_STATES[4];
__attribute__((aligned(16))) int64_t tmp[4];

int lstm_8x16(
    int16_t *p_output, int8_t *p_kernel, int8_t *p_kernel_rec, int16_t *p_bias, int16_t *input,
    int16_t *h_state, int32_t *c_state, int16_t dim_output, int16_t dim_input,
    int16_t dim_input_rec, int16_t qbit_kernel, int16_t qbit_bias, int16_t qbit_input,
    int16_t qbit_input_rec, ACTIVATION_TYPE act_type, void *(*act)(void *, int32_t *, int)) {
    int16_t *po = p_output;
    int8_t *pw = p_kernel;
    int8_t *pw_r = p_kernel_rec;
    int16_t *pb = p_bias;
    int32_t *pt_c = c_state;

    int i, j;
    int groups_4 = dim_output >> 2;
    int rem_rows = dim_output % 4;
    int rows_sub = 4;
    int16_t *p_istate;
    int16_t *p_jstate;
    int16_t *p_fstate;
    int16_t *p_ostate;

    for (i = 0; i < groups_4; i++) {
        p_istate = I_STATES;
        p_jstate = J_STATES;
        p_fstate = F_STATES;
        p_ostate = O_STATES;
        rc_Krows_8x16(
            rows_sub, &p_istate, &pw, &pw_r, &pb, input, h_state, dim_input, dim_input_rec,
            qbit_kernel, qbit_bias, qbit_input, qbit_input_rec,
            (void *(*)(void *, int32_t *, int)) & sigmoid_fix);

        rc_Krows_8x16(
            rows_sub, &p_jstate, &pw, &pw_r, &pb, input, h_state, dim_input, dim_input_rec,
            qbit_kernel, qbit_bias, qbit_input, qbit_input_rec,
            (void *(*)(void *, int32_t *, int)) & tanh_fix);

        rc_Krows_8x16(
            rows_sub, &p_fstate, &pw, &pw_r, &pb, input, h_state, dim_input, dim_input_rec,
            qbit_kernel, qbit_bias, qbit_input, qbit_input_rec,
            (void *(*)(void *, int32_t *, int)) & sigmoid_fix);

        rc_Krows_8x16(
            rows_sub, &p_ostate, &pw, &pw_r, &pb, input, h_state, dim_input, dim_input_rec,
            qbit_kernel, qbit_bias, qbit_input, qbit_input_rec,
            (void *(*)(void *, int32_t *, int)) & sigmoid_fix);

        for (j = 0; j < rows_sub; j++) {
            tmp[j] = ((int64_t)I_STATES[j] * (int64_t)J_STATES[j] +
                      (int64_t)F_STATES[j] * (int64_t)pt_c[j]) >>
                     15;
            pt_c[j] = (int32_t)MIN(MAX(tmp[j], MIN_INT32_T), MAX_INT32_T);
        }
        tanh_fix(po, pt_c, rows_sub);
        for (j = 0; j < rows_sub; j++) {
            po[j] = (int16_t)MIN(
                MAX((((int32_t)po[j] * (int32_t)O_STATES[j]) >> 15), MIN_INT16_T), MAX_INT16_T);
        }

        /*for (j = 0; j < rows_sub; j++)
        {
                fprintf(file_nn_act[1], "%d ", is[j]);
        }*/

        po += rows_sub;
        pt_c += rows_sub;
    }
    if (rem_rows) {
        rows_sub = rem_rows;
        p_istate = I_STATES;
        p_jstate = J_STATES;
        p_fstate = F_STATES;
        p_ostate = O_STATES;

        rc_Krows_8x16(
            rows_sub, &p_istate, &pw, &pw_r, &pb, input, h_state, dim_input, dim_input_rec,
            qbit_kernel, qbit_bias, qbit_input, qbit_input_rec,
            (void *(*)(void *, int32_t *, int)) & sigmoid_fix);

        rc_Krows_8x16(
            rows_sub, &p_jstate, &pw, &pw_r, &pb, input, h_state, dim_input, dim_input_rec,
            qbit_kernel, qbit_bias, qbit_input, qbit_input_rec,
            (void *(*)(void *, int32_t *, int)) & tanh_fix);

        rc_Krows_8x16(
            rows_sub, &p_fstate, &pw, &pw_r, &pb, input, h_state, dim_input, dim_input_rec,
            qbit_kernel, qbit_bias, qbit_input, qbit_input_rec,
            (void *(*)(void *, int32_t *, int)) & sigmoid_fix);

        rc_Krows_8x16(
            rows_sub, &p_ostate, &pw, &pw_r, &pb, input, h_state, dim_input, dim_input_rec,
            qbit_kernel, qbit_bias, qbit_input, qbit_input_rec,
            (void *(*)(void *, int32_t *, int)) & sigmoid_fix);

        for (j = 0; j < rows_sub; j++) {
            tmp[j] = ((int64_t)I_STATES[j] * (int64_t)J_STATES[j] +
                      (int64_t)F_STATES[j] * (int64_t)pt_c[j]) >>
                     15;
            pt_c[j] = (int32_t)MIN(MAX(tmp[j], MIN_INT32_T), MAX_INT32_T);
        }
        tanh_fix(po, pt_c, rows_sub);
        for (j = 0; j < rows_sub; j++) {
            po[j] = (int16_t)MIN(
                MAX((((int32_t)po[j] * (int32_t)O_STATES[j]) >> 15), MIN_INT16_T), MAX_INT16_T);
        }
        po += rows_sub;
        pt_c += rows_sub;

        /*	for (j = 0; j < rows_sub; j++)
                {
                        fprintf(file_nn_act[1], "%d ", is[j]);
                }*/
    }
    // fprintf(file_nn_act[1], "\n");
    for (j = 0; j < dim_output; j++)
        h_state[j] = p_output[j];

#if DEBUG_PRINT
    for (j = 0; j < rows; j++)
        fprintf(file_nn_out, "%d ", (int16_t)pout[j]);
    fprintf(file_nn_out, "\n");
#endif
    return 0;
}

int lstm_8x16_acc32b(
    int16_t *p_output, int8_t *p_kernel, int8_t *p_kernel_rec, int16_t *p_bias, int16_t *input,
    int16_t *h_state, int32_t *c_state, int16_t dim_output, int16_t dim_input,
    int16_t dim_input_rec, int16_t qbit_kernel, int16_t qbit_bias, int16_t qbit_input,
    int16_t qbit_input_rec, ACTIVATION_TYPE act_type, void *(*act)(void *, int32_t *, int)) {
    int16_t *po = p_output;
    int8_t *pw = p_kernel;
    int8_t *pw_r = p_kernel_rec;
    int16_t *pb = p_bias;
    int32_t *pt_c = c_state;

    int i, j;
    int groups_4 = dim_output >> 2;
    int rem_rows = dim_output % 4;
    int rows_sub = 4;
    int16_t *p_istate;
    int16_t *p_jstate;
    int16_t *p_fstate;
    int16_t *p_ostate;

    for (i = 0; i < groups_4; i++) {
        p_istate = I_STATES;
        p_jstate = J_STATES;
        p_fstate = F_STATES;
        p_ostate = O_STATES;
        rc_Krows_8x16_acc32b(
            rows_sub, &p_istate, &pw, &pw_r, &pb, input, h_state, dim_input, dim_input_rec,
            qbit_kernel, qbit_bias, qbit_input, qbit_input_rec,
            (void *(*)(void *, int32_t *, int)) & sigmoid_fix);

        rc_Krows_8x16_acc32b(
            rows_sub, &p_jstate, &pw, &pw_r, &pb, input, h_state, dim_input, dim_input_rec,
            qbit_kernel, qbit_bias, qbit_input, qbit_input_rec,
            (void *(*)(void *, int32_t *, int)) & tanh_fix);

        rc_Krows_8x16_acc32b(
            rows_sub, &p_fstate, &pw, &pw_r, &pb, input, h_state, dim_input, dim_input_rec,
            qbit_kernel, qbit_bias, qbit_input, qbit_input_rec,
            (void *(*)(void *, int32_t *, int)) & sigmoid_fix);

        rc_Krows_8x16_acc32b(
            rows_sub, &p_ostate, &pw, &pw_r, &pb, input, h_state, dim_input, dim_input_rec,
            qbit_kernel, qbit_bias, qbit_input, qbit_input_rec,
            (void *(*)(void *, int32_t *, int)) & sigmoid_fix);

        for (j = 0; j < rows_sub; j++) {
            tmp[j] = ((int64_t)I_STATES[j] * (int64_t)J_STATES[j] +
                      (int64_t)F_STATES[j] * (int64_t)pt_c[j]) >>
                     15;
            pt_c[j] = (int32_t)MIN(MAX(tmp[j], MIN_INT32_T), MAX_INT32_T);
        }
        tanh_fix(po, pt_c, rows_sub);
        for (j = 0; j < rows_sub; j++) {
            po[j] = (int16_t)MIN(
                MAX((((int32_t)po[j] * (int32_t)O_STATES[j]) >> 15), MIN_INT16_T), MAX_INT16_T);
        }

        /*for (j = 0; j < rows_sub; j++)
        {
                fprintf(file_nn_act[1], "%d ", is[j]);
        }*/

        po += rows_sub;
        pt_c += rows_sub;
    }
    if (rem_rows) {
        rows_sub = rem_rows;
        p_istate = I_STATES;
        p_jstate = J_STATES;
        p_fstate = F_STATES;
        p_ostate = O_STATES;

        rc_Krows_8x16_acc32b(
            rows_sub, &p_istate, &pw, &pw_r, &pb, input, h_state, dim_input, dim_input_rec,
            qbit_kernel, qbit_bias, qbit_input, qbit_input_rec,
            (void *(*)(void *, int32_t *, int)) & sigmoid_fix);

        rc_Krows_8x16_acc32b(
            rows_sub, &p_jstate, &pw, &pw_r, &pb, input, h_state, dim_input, dim_input_rec,
            qbit_kernel, qbit_bias, qbit_input, qbit_input_rec,
            (void *(*)(void *, int32_t *, int)) & tanh_fix);

        rc_Krows_8x16_acc32b(
            rows_sub, &p_fstate, &pw, &pw_r, &pb, input, h_state, dim_input, dim_input_rec,
            qbit_kernel, qbit_bias, qbit_input, qbit_input_rec,
            (void *(*)(void *, int32_t *, int)) & sigmoid_fix);

        rc_Krows_8x16_acc32b(
            rows_sub, &p_ostate, &pw, &pw_r, &pb, input, h_state, dim_input, dim_input_rec,
            qbit_kernel, qbit_bias, qbit_input, qbit_input_rec,
            (void *(*)(void *, int32_t *, int)) & sigmoid_fix);

        for (j = 0; j < rows_sub; j++) {
            tmp[j] = ((int64_t)I_STATES[j] * (int64_t)J_STATES[j] +
                      (int64_t)F_STATES[j] * (int64_t)pt_c[j]) >>
                     15;
            pt_c[j] = (int32_t)MIN(MAX(tmp[j], MIN_INT32_T), MAX_INT32_T);
        }
        tanh_fix(po, pt_c, rows_sub);
        for (j = 0; j < rows_sub; j++) {
            po[j] = (int16_t)MIN(
                MAX((((int32_t)po[j] * (int32_t)O_STATES[j]) >> 15), MIN_INT16_T), MAX_INT16_T);
        }
        po += rows_sub;
        pt_c += rows_sub;

        /*	for (j = 0; j < rows_sub; j++)
                {
                        fprintf(file_nn_act[1], "%d ", is[j]);
                }*/
    }
    // fprintf(file_nn_act[1], "\n");
    for (j = 0; j < dim_output; j++)
        h_state[j] = p_output[j];

#if DEBUG_PRINT
    for (j = 0; j < rows; j++)
        fprintf(file_nn_out, "%d ", (int16_t)pout[j]);
    fprintf(file_nn_out, "\n");
#endif
    return 0;
}
