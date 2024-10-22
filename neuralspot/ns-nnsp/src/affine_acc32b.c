#include "ambiq_nnsp_debug.h"
#include "ambiq_stdint.h"
#include "minmax.h"
#include "activation.h"
#if DEBUG_PRINT
    #include "extern_files.h"
#endif
#include "affine_acc32b.h"
#if ARM_OPTIMIZED == 1
    #include <cmsis_gcc.h>
#endif
int32_t accumulators_32b[4] = {0, 0, 0, 0};
#if ARM_OPTIMIZED == 1
int affine_Krows_8x16_acc32b(
    int16_t dim_output, int16_t **pp_output, int8_t **pp_kernel, int16_t **pp_bias, int16_t *input,
    int16_t dim_input, int16_t qbit_kernel, int16_t qbit_bias, int16_t qbit_input,
    int32_t *pt_accum, int8_t is_out, void *(*act)(void *, int32_t *, int)) {
    /*
    Note: this function might cause overflow. To avoid overlow, consider to use
              the less computational efficiency "affine_Krows_8x16".
    See "affine_acc32b.h" for more detail
*/
    int8_t *p_kernel = *pp_kernel;
    int32_t *p_kernel_32b = (int32_t *)*pp_kernel;
    int16_t *p_bias = *pp_bias;
    int16_t *po = *pp_output;
    int16_t *pi = input;
    int32_t *pi_32b = (int32_t *)input;
    int16_t in;
    int32_t in_32b;
    int32_t acc32[4];
    int32_t kernel_val0;
    int32_t kernel_val1;
    int32_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
    int i;
    int shift;
    int qbit_s;

    if (dim_output == 4) {
        sum0 = pt_accum[0];
        sum1 = pt_accum[1];
        sum2 = pt_accum[2];
        sum3 = pt_accum[3];
    } else if (dim_output == 3) {
        sum0 = pt_accum[0];
        sum1 = pt_accum[1];
        sum2 = pt_accum[2];
    } else if (dim_output == 2) {
        sum0 = pt_accum[0];
        sum1 = pt_accum[1];
    } else // if (dim_output==1)
    {
        sum0 = pt_accum[0];
    }

    if (p_bias == 0)
        qbit_s = qbit_input + qbit_kernel;
    else
        qbit_s = MAX(15, qbit_input + qbit_kernel);

    for (i = 0; i < (dim_input >> 1); i++) {
        /*
        only 4 cases to deal with
        */
        in_32b = *pi_32b++;
        if (dim_output == 4) {
            /*  ________________
                    |	1	|	3  |
                    |___2___|___4__|
                    |	5	|	7  |
                    |___6___|___8__|
            */
            kernel_val0 = *p_kernel_32b++;
            kernel_val1 = __SXTB16(__ROR(kernel_val0, 8));
            sum1 = __SMLAD(kernel_val1, in_32b, sum1);

            kernel_val0 = __SXTB16(kernel_val0);
            sum0 = __SMLAD(kernel_val0, in_32b, sum0);

            kernel_val0 = *p_kernel_32b++;
            kernel_val1 = __SXTB16(__ROR(kernel_val0, 8));
            sum3 = __SMLAD(kernel_val1, in_32b, sum3);

            kernel_val0 = __SXTB16(kernel_val0);
            sum2 = __SMLAD(kernel_val0, in_32b, sum2);

        } else if (dim_output == 2) {
            /*  ________________
                    |	1	|	3  |
                    |___2___|___4__|
            */
            kernel_val0 = *p_kernel_32b++;
            kernel_val1 = __SXTB16(__ROR(kernel_val0, 8));
            sum1 = __SMLAD(kernel_val1, in_32b, sum1);

            kernel_val0 = __SXTB16(kernel_val0);
            sum0 = __SMLAD(kernel_val0, in_32b, sum0);

        } else if (dim_output == 3) {
            /*  ________________
                    |	1	|	3  |
                    |___2___|___4__|
                    |___5_______6__|
            */
            kernel_val0 = *p_kernel_32b++;
            kernel_val1 = __SXTB16(__ROR(kernel_val0, 8));
            sum1 = __SMLAD(kernel_val1, in_32b, sum1);

            kernel_val0 = __SXTB16(kernel_val0);
            sum0 = __SMLAD(kernel_val0, in_32b, sum0);

            pi = (int16_t *)&in_32b;
            p_kernel = (int8_t *)p_kernel_32b;
            sum2 += (int32_t)p_kernel[0] * (int32_t)pi[0] + (int32_t)p_kernel[1] * (int32_t)pi[1];

            p_kernel += 2;
            p_kernel_32b = (int32_t *)p_kernel;
        } else // if (dim_output == 1)
        {
            /*  ________________
                    |____1______2__|
            */
            pi = (int16_t *)&in_32b;
            p_kernel = (int8_t *)p_kernel_32b;
            sum0 += (int32_t)p_kernel[0] * (int32_t)pi[0] + (int32_t)p_kernel[1] * (int32_t)pi[1];

            p_kernel += 2;
            p_kernel_32b = (int32_t *)p_kernel;
        }
    }
    p_kernel = (int8_t *)p_kernel_32b;
    /*  _________________
            |			   |1|
            |              |2|
            |______________|3|
    */
    if (dim_input % 2) {
        pi = (int16_t *)pi_32b;
        in = *pi;

        if (dim_output == 4) {
            sum0 += (int32_t)*p_kernel++ * (int32_t)in;
            sum1 += (int32_t)*p_kernel++ * (int32_t)in;
            sum2 += (int32_t)*p_kernel++ * (int32_t)in;
            sum3 += (int32_t)*p_kernel++ * (int32_t)in;
        } else if (dim_output == 3) {
            sum0 += (int32_t)*p_kernel++ * (int32_t)in;
            sum1 += (int32_t)*p_kernel++ * (int32_t)in;
            sum2 += (int32_t)*p_kernel++ * (int32_t)in;
        } else if (dim_output == 2) {
            sum0 += (int32_t)*p_kernel++ * (int32_t)in;
            sum1 += (int32_t)*p_kernel++ * (int32_t)in;
        } else // if (dim_output==1)
            sum0 += (int32_t)*p_kernel++ * (int32_t)in;
    }

    shift = qbit_s - (qbit_input + qbit_kernel);
    shift_32b(pt_accum, shift, dim_output); // align acc to w

    if (p_bias != 0) {
        shift = qbit_s - (qbit_bias);
        // align w to acc & add
        if (dim_output == 4) {
            sum0 += (shift >= 0) ? ((int32_t)*p_bias++) << shift : ((int32_t)*p_bias++) >> -shift;
            sum1 += (shift >= 0) ? ((int32_t)*p_bias++) << shift : ((int32_t)*p_bias++) >> -shift;
            sum2 += (shift >= 0) ? ((int32_t)*p_bias++) << shift : ((int32_t)*p_bias++) >> -shift;
            sum3 += (shift >= 0) ? ((int32_t)*p_bias++) << shift : ((int32_t)*p_bias++) >> -shift;
        } else if (dim_output == 3) {
            sum0 += (shift >= 0) ? ((int32_t)*p_bias++) << shift : ((int32_t)*p_bias++) >> -shift;
            sum1 += (shift >= 0) ? ((int32_t)*p_bias++) << shift : ((int32_t)*p_bias++) >> -shift;
            sum2 += (shift >= 0) ? ((int32_t)*p_bias++) << shift : ((int32_t)*p_bias++) >> -shift;
        } else if (dim_output == 2) {
            sum0 += (shift >= 0) ? ((int32_t)*p_bias++) << shift : ((int32_t)*p_bias++) >> -shift;
            sum1 += (shift >= 0) ? ((int32_t)*p_bias++) << shift : ((int32_t)*p_bias++) >> -shift;
        } else // if (dim_output==1)
        {
            sum0 += (shift >= 0) ? ((int32_t)*p_bias++) << shift : ((int32_t)*p_bias++) >> -shift;
        }
    }

    if (dim_output == 4) {
        pt_accum[0] = sum0;
        pt_accum[1] = sum1;
        pt_accum[2] = sum2;
        pt_accum[3] = sum3;
    } else if (dim_output == 3) {
        pt_accum[0] = sum0;
        pt_accum[1] = sum1;
        pt_accum[2] = sum2;
    } else if (dim_output == 2) {
        pt_accum[0] = sum0;
        pt_accum[1] = sum1;
    } else // if (dim_output==1)
    {
        pt_accum[0] = sum0;
    }

    if (is_out) {
        shift = 15 - qbit_s;
        shift_32b(pt_accum, shift, dim_output);
        for (i = 0; i < dim_output; i++) {
            acc32[i] = pt_accum[i];
        }
        po = *pp_output;
        po = (int16_t *)(*act)(po, acc32, dim_output);
        *pp_output = po;
    }
    *pp_kernel = p_kernel;
    *pp_bias = p_bias;

    return 0;
}
#else
int affine_Krows_8x16_acc32b(
    int16_t dim_output, int16_t **pp_output, int8_t **pp_kernel, int16_t **pp_bias, int16_t *input,
    int16_t dim_input, int16_t qbit_kernel, int16_t qbit_bias, int16_t qbit_input,
    int32_t *pt_accum, int8_t is_out, void *(*act)(void *, int32_t *, int)) {
    int8_t *p_kernel = *pp_kernel;
    int16_t *p_bias = *pp_bias;
    int16_t *po = *pp_output;
    int16_t *pi = input;
    int32_t nbit_out = 32;
    int16_t in[2];
    int32_t acc32[4];
    int i, j;
    int shift;
    int qbit_s;
    if (p_bias == 0)
        qbit_s = qbit_input + qbit_kernel;
    else
        qbit_s = MAX(15, qbit_input + qbit_kernel);

    for (i = 0; i < (dim_input >> 1); i++) {
        in[0] = *pi++;
        in[1] = *pi++;
        for (j = 0; j < dim_output; j++) {
            pt_accum[j] +=
                (int32_t)p_kernel[0] * (int32_t)in[0] + (int32_t)p_kernel[1] * (int32_t)in[1];
            p_kernel += 2;
        }
    }

    if (dim_input % 2) {
        in[0] = *pi++;
        for (j = 0; j < dim_output; j++)
            pt_accum[j] += (int32_t)*p_kernel++ * (int32_t)in[0];
    }

    shift = qbit_s - (qbit_input + qbit_kernel);
    shift_32b(pt_accum, shift, dim_output); // align acc to w

    if (p_bias != 0) {
        shift = qbit_s - (qbit_bias);
        // align w to acc & add
        for (i = 0; i < dim_output; i++) {
            pt_accum[i] +=
                (shift >= 0) ? ((int32_t)*p_bias++) << shift : ((int32_t)*p_bias++) >> -shift;
        }
    }

    if (is_out) {
        shift = 15 - qbit_s;
        shift_32b(pt_accum, shift, dim_output);
        for (i = 0; i < dim_output; i++) {
            acc32[i] = (int32_t)MIN(MAX(pt_accum[i], MIN_INT32_T), MAX_INT32_T);
        }

        po = *pp_output;
        po = (int16_t *)(*act)(po, acc32, dim_output);
        *pp_output = po;
    }
    *pp_kernel = p_kernel;
    *pp_bias = p_bias;

    return 0;
}

#endif
int rc_Krows_8x16_acc32b(
    int16_t dim_output, int16_t **pp_output, int8_t **pp_kernel, int8_t **pp_kernel_rec,
    int16_t **pp_bias, int16_t *input, int16_t *input_rec, int16_t dim_input, int16_t dim_input_rec,
    int16_t qbit_kernel, int16_t qbit_bias, int16_t qbit_input, int16_t qbit_input_rec,
    void *(*act)(void *, int32_t *, int)) {
    int16_t *p_output = *pp_output;
    int8_t *p_kernel = *pp_kernel;
    int8_t *p_kernel_rec = *pp_kernel_rec;
    int16_t *p_bias = *pp_bias;
    int16_t *p_bias_null = (int16_t *)0;
    int8_t is_out;
    int j;
    int shift = qbit_input_rec - qbit_input;

    for (j = 0; j < dim_output; j++)
        accumulators_32b[j] = 0;

    is_out = 0;
    affine_Krows_8x16_acc32b(
        dim_output, &p_output, &p_kernel, &p_bias_null, input, dim_input, qbit_kernel, qbit_bias,
        qbit_input, accumulators_32b, is_out, act);
    shift_32b(accumulators_32b, shift, dim_output);

    is_out = 1;
    affine_Krows_8x16_acc32b(
        dim_output, &p_output, &p_kernel_rec, &p_bias, input_rec, dim_input_rec, qbit_kernel,
        qbit_bias, qbit_input_rec, accumulators_32b, is_out, act);
#if DEBUG_PRINT
    if (is_out) {
        for (j = 0; j < rows; j++)
            fprintf(file_nn_pre, "%d ", (int32_t)acc[j]);
    }
#endif
    *pp_output = p_output;
    *pp_kernel = p_kernel;
    *pp_kernel_rec = p_kernel_rec;
    *pp_bias = p_bias;

    return 0;
}

int fc_8x16_acc32b(
    int16_t *p_output, int8_t *p_kernel, int8_t *p_kernel_rec, int16_t *p_bias, int16_t *input,
    int16_t *input_rec, int32_t *c_state, int16_t dim_output, int16_t dim_input,
    int16_t dim_input_rec, int16_t qbit_kernel, int16_t qbit_bias, int16_t qbit_input,
    int16_t qbit_input_rec, ACTIVATION_TYPE act_type, void *(*act)(void *, int32_t *, int)) {
    int16_t *po = p_output;
    int8_t *pw = p_kernel;
    int16_t *pb = p_bias;

    int i, j;
    int rem_rows = dim_output % 4;
    int groups_4 = dim_output >> 2;

    int8_t is_out = 1;
    for (i = 0; i < groups_4; i++) {

        for (j = 0; j < 4; j++)
            accumulators_32b[j] = 0;

        affine_Krows_8x16_acc32b(
            4, &po, &pw, &pb, input, dim_input, qbit_kernel, qbit_bias, qbit_input,
            accumulators_32b, is_out, act);
#if DEBUG_PRINT
        for (j = 0; j < 4; j++)
            fprintf(file_nn_pre, "%d ", (int32_t)acc[j]);
#endif
    }
    if (rem_rows) {

        for (j = 0; j < rem_rows; j++)
            accumulators_32b[j] = 0;

        affine_Krows_8x16_acc32b(
            rem_rows, &po, &pw, &pb, input, dim_input, qbit_kernel, qbit_bias, qbit_input,
            accumulators_32b, is_out, act);
#if DEBUG_PRINT
        for (j = 0; j < rem_rows; j++)
            fprintf(file_nn_pre, "%d ", (int32_t)acc[j]);
        fprintf(file_nn_pre, "\n");
#endif
    }

#if DEBUG_PRINT
    if (act_type != none) {
        po = pout;
        for (j = 0; j < rows; j++)
            fprintf(file_nn_out, "%d ", (int16_t)po[j]);
        fprintf(file_nn_out, "\n");
    } else {
        po32 = (int32_t *)pout;
        for (j = 0; j < rows; j++)
            fprintf(file_nn_out, "%d ", (int32_t)po32[j]);
        fprintf(file_nn_out, "\n");
    }
#endif
    return 0;
}

int rc_8x16_acc32b(
    int16_t *p_output, int8_t *p_kernel, int8_t *p_kernel_rec, int16_t *p_bias, int16_t *input,
    int16_t *input_rec, int16_t dim_output, int16_t dim_input, int16_t dim_input_rec,
    int16_t qbit_kernel, int16_t qbit_bias, int16_t qbit_input, int16_t qbit_input_rec,
    ACTIVATION_TYPE act_type, void *(*act)(void *, int32_t *, int)) {
    int16_t *po = p_output;
    int8_t *pw = p_kernel;
    int8_t *pw_r = p_kernel_rec;
    int16_t *pb = p_bias;
    int i;
    int groups_4 = dim_output >> 2;
    int rem_rows = dim_output % 4;
    for (i = 0; i < groups_4; i++) {
        rc_Krows_8x16_acc32b(
            4, &po, &pw, &pw_r, &pb, input, input_rec, dim_input, dim_input_rec, qbit_kernel,
            qbit_bias, qbit_input, qbit_input_rec, act);
    }
    if (rem_rows) {
        rc_Krows_8x16_acc32b(
            rem_rows, &po, &pw, &pw_r, &pb, input, input_rec, dim_input, dim_input_rec, qbit_kernel,
            qbit_bias, qbit_input, qbit_input_rec, act);
    }

#if DEBUG_PRINT
    if (act_type != none) {
        po = pout;
        for (j = 0; j < rows; j++)
            fprintf(file_nn_out, "%d ", (int16_t)po[j]);
        fprintf(file_nn_out, "\n");
    } else {
        po32 = (int32_t *)pout;
        for (j = 0; j < rows; j++)
            fprintf(file_nn_out, "%d ", (int32_t)po32[j]);
        fprintf(file_nn_out, "\n");
    }
#endif
    return 0;
}
void shift_32b(int32_t *x, int8_t shift, int len) {
    int i;
    int32_t M, m;
    if (shift == 0) {

    } else if (shift < 0) {
        shift = -shift;
        for (i = 0; i < len; i++)
            x[i] >>= shift;
    } else {
        M = (int32_t)1 << (31 - shift);
        m = -M;
        M -= 1;
        for (i = 0; i < len; i++) {
            x[i] = MIN(MAX(x[i], m), M);
            x[i] <<= shift;
        }
    }
}
