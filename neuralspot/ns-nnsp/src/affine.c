#include "ambiq_nnsp_debug.h"
#include "ambiq_stdint.h"
#include "minmax.h"
#include "activation.h"
#include "ns_ambiqsuite_harness.h"
#define MAX_FAST_ACC_SIZE 512
#if DEBUG_PRINT
    #include "extern_files.h"
#endif
#include "affine.h"
#include <arm_math.h>
#if ARM_OPTIMIZED == 1
    // #include <cmsis_gcc.h>
#elif ARM_OPTIMIZED == 3
    #include <arm_mve.h>
    // #include "third_party/ns_cmsis_nn/Include/arm_nnsupportfunctions.h"
    // #include "third_party/ns_cmsis_nn/Include/Internal/arm_nn_compiler.h"
#endif
__attribute__((aligned(16))) int64_t accumulators[4] = {0, 0, 0, 0};
#define OPT_ASM 1
#if ARM_OPTIMIZED == 3
// #if 1
int affine_Krows_8x16(
	int16_t dim_output,
	int16_t** pp_output,
	int8_t** pp_kernel,
	int16_t** pp_bias,
	int16_t* input,
	int16_t dim_input,
	int16_t qbit_kernel,
	int16_t qbit_bias,
	int16_t qbit_input,
	int64_t* pt_accum,
	int8_t is_out,
	void* (*act)(void*, int32_t*, int))
{
	/*
		"affine_Krows_8x16" is an affine (matrix*vec+bias) op.
        8-bit for weight table
        16-bit for input vector
		See "affine.h" for more detail
	*/
	

	int16_t* p_bias = *pp_bias;
	int16_t* po = *pp_output;
	int16_t* pi = input;

	int16x8_t in;
    int16x8_t w;

    __attribute__((aligned(16))) int32_t acc32[4];
	int32_t sum0=0;
    int32_t sum1=0;
    int32_t sum2=0;
    int32_t sum3=0;
    int16_t num = dim_input >> 3;
    int16_t num_8 = num << 3;
    int16_t rem = dim_input & 0x7;
	int i;
	int lshift;
	int qbit_s;

	if (p_bias == 0)
		qbit_s = qbit_input + qbit_kernel;
	else
		qbit_s = MAX(15, qbit_input + qbit_kernel);
    //
    // 512 512 left
    //
    int count = dim_input;
    int8_t* p_kernel_0 = *pp_kernel;
    int8_t *p_kernel_1 = p_kernel_0 + dim_input;
    int8_t *p_kernel_2 = p_kernel_1 + dim_input;
    int8_t *p_kernel_3 = p_kernel_2 + dim_input;

    // ns_printf("p_kernel_0: %ld\n",(int32_t) p_kernel_0);
    while (count > 0)
    {
        int num_acc = (count < MAX_FAST_ACC_SIZE) ?  count : MAX_FAST_ACC_SIZE;
        count -= MAX_FAST_ACC_SIZE;
    
        if (dim_output == 4)
        {
            #if OPT_ASM==1
            __ASM volatile(
                " .p2align 2                                 \n"
                "   wlstp.16        lr, %[cnt], 1f           \n"
                "   mov             %[out0], 0               \n"
                "   mov             %[out1], 0               \n"
                "   mov             %[out2], 0               \n"
                "   mov             %[out3], 0               \n"
                "   vldrh.s16       q0, [%[col]], #16        \n"
                "2:                                          \n"
                "   vldrb.s16        q1, [%[row0]], #8       \n"
                "   vmladava.s16     %[out0], q0, q1         \n"
                "   vldrb.s16        q2, [%[row1]], #8       \n"
                "   vmladava.s16     %[out1], q0, q2         \n"
                "   vldrb.s16       q3, [%[row2]], #8       \n"
                "   vmladava.s16     %[out2], q0, q3         \n"
                "   vldrb.s16        q4, [%[row3]], #8       \n"
                "   vmladava.s16     %[out3], q0, q4         \n"
                "   vldrh.s16        q0, [%[col]], #16       \n"
                "   letp            lr, 2b                   \n"
                "1:                                          \n"
            :
                [col] "+r"(pi),
                [row0] "+l"(p_kernel_0),
                [row1] "+l"(p_kernel_1),
                [row2] "+l"(p_kernel_2),
                [row3] "+l"(p_kernel_3),
                [out0] "=Te"(sum0),
                [out1] "=Te"(sum1),
                [out2] "=Te"(sum2),
                [out3] "=Te"(sum3)
            :
                [cnt] "r"(num_acc)
            :
                "q0", "q1", "q2", "q3", "q4", "memory", "r14"
            );
            pi -= 8;  // Fix lhs_ptr by subtracting 8
            #else
            sum0 = 0;
            sum1 = 0;
            sum2 = 0;
            sum3 = 0;
            for (int i = 0; i < dim_input; i++)
            {
                sum0 += (int32_t)pi[i] * (int32_t)p_kernel_0[i];
                sum1 += (int32_t)pi[i] * (int32_t)p_kernel_1[i];
                sum2 += (int32_t)pi[i] * (int32_t)p_kernel_2[i];
                sum3 += (int32_t)pi[i] * (int32_t)p_kernel_3[i];
            }
            #endif
        }
        else if (dim_output == 3)
        {
            #if OPT_ASM==1
            __ASM volatile(
                " .p2align 2                                 \n"
                "   wlstp.16        lr, %[cnt], 1f           \n"
                "   mov             %[out0], 0               \n"
                "   mov             %[out1], 0               \n"
                "   mov             %[out2], 0               \n"
                "   vldrh.s16       q0, [%[col]], #16        \n"
                "2:                                          \n"
                "   vldrb.s16        q1, [%[row0]], #8       \n"
                "   vmladava.s16     %[out0], q0, q1         \n"
                "   vldrb.s16        q2, [%[row1]], #8       \n"
                "   vmladava.s16     %[out1], q0, q2         \n"
                "   vldrb.s16       q3, [%[row2]], #8       \n"
                "   vmladava.s16     %[out2], q0, q3         \n"
                "   vldrh.s16        q0, [%[col]], #16       \n"
                "   letp            lr, 2b                   \n"
                "1:                                          \n"
            :
                [col] "+r"(pi),
                [row0] "+l"(p_kernel_0),
                [row1] "+l"(p_kernel_1),
                [row2] "+l"(p_kernel_2),
                [out0] "=Te"(sum0),
                [out1] "=Te"(sum1),
                [out2] "=Te"(sum2)
            :
                [cnt] "r"(num_acc)
            :
                "q0", "q1", "q2", "q3", "memory", "r14"
            );
            pi -= 8;  // Fix lhs_ptr by subtracting 8
            #else
            sum0 = 0;
            sum1 = 0;
            sum2 = 0;

            for (int i = 0; i < dim_input; i++)
            {
                sum0 += (int32_t)pi[i] * (int32_t)p_kernel_0[i];
                sum1 += (int32_t)pi[i] * (int32_t)p_kernel_1[i];
                sum2 += (int32_t)pi[i] * (int32_t)p_kernel_2[i];
            }
            #endif
        }
        else if (dim_output == 2)
        {
            #if OPT_ASM==1
            __ASM volatile(
                " .p2align 2                                 \n"
                "   wlstp.16        lr, %[cnt], 1f           \n"
                "   mov             %[out0], 0               \n"
                "   mov             %[out1], 0               \n"
                "   vldrh.s16       q0, [%[col]], #16        \n"
                "2:                                          \n"
                "   vldrb.s16        q1, [%[row0]], #8       \n"
                "   vmladava.s16     %[out0], q0, q1         \n"
                "   vldrb.s16        q2, [%[row1]], #8       \n"
                "   vmladava.s16     %[out1], q0, q2         \n"
                "   vldrh.s16        q0, [%[col]], #16       \n"
                "   letp            lr, 2b                   \n"
                "1:                                          \n"
            :
                [col] "+r"(pi),
                [row0] "+l"(p_kernel_0),
                [row1] "+l"(p_kernel_1),
                [out0] "=Te"(sum0),
                [out1] "=Te"(sum1)
            :
                [cnt] "r"(num_acc)
            :
                "q0", "q1", "q2", "memory", "r14"
            );
            pi -= 8;  // Fix lhs_ptr by subtracting 8
            #else
            sum0 = 0;
            sum1 = 0;
            for (int i = 0; i < dim_input; i++)
            {
                sum0 += (int32_t)pi[i] * (int32_t)p_kernel_0[i];
                sum1 += (int32_t)pi[i] * (int32_t)p_kernel_1[i];
            }

            #endif
        }
        else // (dim_output == 1)
        {
            #if OPT_ASM==1
            __ASM volatile(
                " .p2align 2                                 \n"
                "   wlstp.16        lr, %[cnt], 1f           \n"
                "   mov             %[out0], 0               \n"
                "   vldrh.s16       q0, [%[col]], #16        \n"
                "2:                                          \n"
                "   vldrb.s16        q1, [%[row0]], #8       \n"
                "   vmladava.s16     %[out0], q0, q1         \n"
                "   vldrh.s16        q0, [%[col]], #16       \n"
                "   letp            lr, 2b                   \n"
                "1:                                          \n"
            :
                [col] "+r"(pi),
                [row0] "+l"(p_kernel_0),
                [out0] "=Te"(sum0)
            :
                [cnt] "r"(num_acc)
            :
                "q0", "q1", "memory", "r14"
            );
            pi -= 8;  // Fix lhs_ptr by subtracting 8
            #else
            sum0 = 0;
            for (int i = 0; i < dim_input; i++)
            {
                sum0 += (int32_t)pi[i] * (int32_t)p_kernel_0[i];
            }
            #endif
        }
        // ns_printf("p_kernel_0: %ld\n", (int32_t) p_kernel_0);
        // ns_printf("dim_input: %d\n", dim_input);
        if (dim_output == 4)
        {
            pt_accum[0] += (int64_t) sum0;
            pt_accum[1] += (int64_t) sum1;
            pt_accum[2] += (int64_t) sum2;
            pt_accum[3] += (int64_t) sum3;
        }
        else if (dim_output == 3)
        {
            pt_accum[0] += (int64_t) sum0;
            pt_accum[1] += (int64_t) sum1;
            pt_accum[2] += (int64_t) sum2;
            
        }
        else if (dim_output == 2)
        {
            pt_accum[0] += (int64_t) sum0;
            pt_accum[1] += (int64_t) sum1;
           
        }
        else if (dim_output == 1)
        {
            pt_accum[0] += (int64_t) sum0;

        }
    }

    lshift = (qbit_input + qbit_kernel) - qbit_s;

    if (lshift != 0 )
    {
        for (i = 0; i < dim_output; i++)
        {
            pt_accum[i] = sqrshrl(pt_accum[i], lshift);
        }
    }

    // add bias
	if (p_bias != 0)
	{
		lshift = qbit_bias - qbit_s;
		// align w to acc & add
		if (lshift == 0)
        {
            for (i = 0; i < dim_output; i++)
            {
                pt_accum[i] += (int64_t) *p_bias++;
            }
        }
        else
        {
            for (i = 0; i < dim_output; i++)
            {
                pt_accum[i] += sqrshrl((int64_t) *p_bias++, lshift);
            }
        }
	}

	if (is_out)
	{
		lshift = qbit_s - 15 ;
        if (lshift != 0)
        {
            for (i = 0; i < dim_output; i++)
            {
                pt_accum[i] = sqrshrl(pt_accum[i], lshift);
            }
        }

		for (i = 0; i < dim_output; i++)
		{
			acc32[i] = (int32_t)MIN(MAX(pt_accum[i], MIN_INT32_T), MAX_INT32_T);
		}

		po = *pp_output;
		po = (int16_t*)(*act)(po, acc32, dim_output);
		*pp_output = po;
	}

    *pp_kernel = *pp_kernel + dim_input * dim_output;

	*pp_bias = p_bias;

	return 0;
}
#elif ARM_OPTIMIZED == 1
int affine_Krows_8x16(
    int16_t dim_output, int16_t **pp_output, int8_t **pp_kernel, int16_t **pp_bias, int16_t *input,
    int16_t dim_input, int16_t qbit_kernel, int16_t qbit_bias, int16_t qbit_input,
    int64_t *pt_accum, int8_t is_out, void *(*act)(void *, int32_t *, int)) {
    /*
            "affine_Krows_8x16" is an affine (matrix*vec+bias) op.
            See "affine.h" for more detail
    */
    int8_t *p_kernel = *pp_kernel;
    int32_t *p_kernel_32b = (int32_t *)*pp_kernel;
    int16_t *p_bias = *pp_bias;
    int16_t *po = *pp_output;
    int16_t *pi = input;
    int32_t *pi_32b = (int32_t *)input;
    int16_t in;
    int32_t in_32b;
    __attribute__((aligned(16))) int32_t acc32[4];
    int32_t kernel_val0;
    int32_t kernel_val1;
    int64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
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
            sum1 = __SMLALD(kernel_val1, in_32b, sum1);

            kernel_val0 = __SXTB16(kernel_val0);
            sum0 = __SMLALD(kernel_val0, in_32b, sum0);

            kernel_val0 = *p_kernel_32b++;
            kernel_val1 = __SXTB16(__ROR(kernel_val0, 8));
            sum3 = __SMLALD(kernel_val1, in_32b, sum3);

            kernel_val0 = __SXTB16(kernel_val0);
            sum2 = __SMLALD(kernel_val0, in_32b, sum2);

        } else if (dim_output == 2) {
            /*  ________________
                    |	1	|	3  |
                    |___2___|___4__|
            */
            kernel_val0 = *p_kernel_32b++;
            kernel_val1 = __SXTB16(__ROR(kernel_val0, 8));
            sum1 = __SMLALD(kernel_val1, in_32b, sum1);

            kernel_val0 = __SXTB16(kernel_val0);
            sum0 = __SMLALD(kernel_val0, in_32b, sum0);

        } else if (dim_output == 3) {
            /*  ________________
                    |	1	|	3  |
                    |___2___|___4__|
                    |___5_______6__|
            */
            kernel_val0 = *p_kernel_32b++;
            kernel_val1 = __SXTB16(__ROR(kernel_val0, 8));
            sum1 = __SMLALD(kernel_val1, in_32b, sum1);

            kernel_val0 = __SXTB16(kernel_val0);
            sum0 = __SMLALD(kernel_val0, in_32b, sum0);

            pi = (int16_t *)&in_32b;
            p_kernel = (int8_t *)p_kernel_32b;
            sum2 += (int64_t)p_kernel[0] * (int64_t)pi[0] + (int64_t)p_kernel[1] * (int64_t)pi[1];

            p_kernel += 2;
            p_kernel_32b = (int32_t *)p_kernel;
        } else // if (dim_output == 1)
        {
            /*  ________________
                    |____1______2__|
            */
            pi = (int16_t *)&in_32b;
            p_kernel = (int8_t *)p_kernel_32b;
            sum0 += (int64_t)p_kernel[0] * (int64_t)pi[0] + (int64_t)p_kernel[1] * (int64_t)pi[1];

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
            sum0 += (int64_t)*p_kernel++ * (int64_t)in;
            sum1 += (int64_t)*p_kernel++ * (int64_t)in;
            sum2 += (int64_t)*p_kernel++ * (int64_t)in;
            sum3 += (int64_t)*p_kernel++ * (int64_t)in;
        } else if (dim_output == 3) {
            sum0 += (int64_t)*p_kernel++ * (int64_t)in;
            sum1 += (int64_t)*p_kernel++ * (int64_t)in;
            sum2 += (int64_t)*p_kernel++ * (int64_t)in;
        } else if (dim_output == 2) {
            sum0 += (int64_t)*p_kernel++ * (int64_t)in;
            sum1 += (int64_t)*p_kernel++ * (int64_t)in;
        } else // if (dim_output==1)
            sum0 += (int64_t)*p_kernel++ * (int64_t)in;
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
    } else if (dim_output == 1) {
        pt_accum[0] = sum0;
    }

    shift = qbit_s - (qbit_input + qbit_kernel);
    shift_64b(pt_accum, shift, dim_output); // align acc to w

    if (p_bias != 0) {
        shift = qbit_s - (qbit_bias);
        // align w to acc & add
        for (i = 0; i < dim_output; i++) {
            pt_accum[i] +=
                (shift >= 0) ? ((int64_t)*p_bias++) << shift : ((int64_t)*p_bias++) >> -shift;
        }
    }

    if (is_out) {
        shift = 15 - qbit_s;
        shift_64b(pt_accum, shift, dim_output);
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
#elif ARM_OPTIMIZED == 2
int affine_Krows_8x16(
    int16_t dim_output, int16_t **pp_output, int8_t **pp_kernel, int16_t **pp_bias, int16_t *input,
    int16_t dim_input, int16_t qbit_kernel, int16_t qbit_bias, int16_t qbit_input,
    int64_t *pt_accum, int8_t is_out, void *(*act)(void *, int32_t *, int)) {
    /*
            "affine_Krows_8x16" is an affine (matrix*vec+bias) op.
            See "affine.h" for more detail
    */
    int8_t *p_kernel = *pp_kernel;
    int32_t *p_kernel_32b = (int32_t *)*pp_kernel;
    int16_t *p_bias = *pp_bias;
    int16_t *po = *pp_output;
    int16_t *pi = input;
    int32_t *pi_32b = (int32_t *) input;
    int16_t in;
    int32_t in_32b;
    __attribute__((aligned(16))) int32_t acc32[4];
    int32_t kernel_val0;
    int32_t kernel_val1;
    int64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
    int8_t *pt8;
    int16_t *pt16;

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

    pt8 = (int8_t *)&kernel_val0;
    pt16 = (int16_t *)&in_32b;

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
            /*kernel_val1 = __SXTB16(__ROR(kernel_val0, 8));
                        sum1 = __SMLALD(kernel_val1, in_32b, sum1);*/

            sum1 += (int64_t)pt16[0] * (int64_t)pt8[1] + (int64_t)pt16[1] * (int64_t)pt8[3];
            sum0 += (int64_t)pt16[0] * (int64_t)pt8[0] + (int64_t)pt16[1] * (int64_t)pt8[2];

            /*kernel_val0 = __SXTB16(kernel_val0);
sum0 = __SMLALD(kernel_val0, in_32b, sum0);*/

            kernel_val0 = *p_kernel_32b++;
            /*kernel_val1 = __SXTB16(__ROR(kernel_val0, 8));
sum3 = __SMLALD(kernel_val1, in_32b, sum3);*/

            sum3 += (int64_t)pt16[0] * (int64_t)pt8[1] + (int64_t)pt16[1] * (int64_t)pt8[3];
            sum2 += (int64_t)pt16[0] * (int64_t)pt8[0] + (int64_t)pt16[1] * (int64_t)pt8[2];

            /*kernel_val0 = __SXTB16(kernel_val0);
sum2 = __SMLALD(kernel_val0, in_32b, sum2);*/

        } else if (dim_output == 2) {
            /*  ________________
                    |	1	|	3  |
                    |___2___|___4__|
            */
            kernel_val0 = *p_kernel_32b++;
            /*kernel_val1 = __SXTB16(__ROR(kernel_val0, 8));
sum1 = __SMLALD(kernel_val1, in_32b, sum1);

            kernel_val0 = __SXTB16(kernel_val0);
sum0 = __SMLALD(kernel_val0, in_32b, sum0);*/
            sum1 += (int64_t)pt16[0] * (int64_t)pt8[1] + (int64_t)pt16[1] * (int64_t)pt8[3];
            sum0 += (int64_t)pt16[0] * (int64_t)pt8[0] + (int64_t)pt16[1] * (int64_t)pt8[2];

        } else if (dim_output == 3) {
            /*  ________________
                    |	1	|	3  |
                    |___2___|___4__|
                    |___5_______6__|
            */
            kernel_val0 = *p_kernel_32b++;
            /*kernel_val1 = __SXTB16(__ROR(kernel_val0, 8));
                        sum1 = __SMLALD(kernel_val1, in_32b, sum1);

                        kernel_val0 = __SXTB16(kernel_val0);
            sum0 = __SMLALD(kernel_val0, in_32b, sum0);*/
            sum1 += (int64_t)pt16[0] * (int64_t)pt8[1] + (int64_t)pt16[1] * (int64_t)pt8[3];
            sum0 += (int64_t)pt16[0] * (int64_t)pt8[0] + (int64_t)pt16[1] * (int64_t)pt8[2];

            pi = (int16_t *)&in_32b;
            p_kernel = (int8_t *)p_kernel_32b;
            sum2 += (int64_t)p_kernel[0] * (int64_t)pi[0] + (int64_t)p_kernel[1] * (int64_t)pi[1];

            p_kernel += 2;
            p_kernel_32b = (int32_t *)p_kernel;
        } else // if (dim_output == 1)
        {
            /*  ________________
                    |____1______2__|
            */
            pi = (int16_t *)&in_32b;
            p_kernel = (int8_t *)p_kernel_32b;
            sum0 += (int64_t)p_kernel[0] * (int64_t)pi[0] + (int64_t)p_kernel[1] * (int64_t)pi[1];

            p_kernel += 2;
            p_kernel_32b = (int32_t *)p_kernel;
        }
    }
    #if AMBIQ_NNSP_DEBUG == 1
    printf("%lld\n", sum0);
    #endif
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
            sum0 += (int64_t)*p_kernel++ * (int64_t)in;
            sum1 += (int64_t)*p_kernel++ * (int64_t)in;
            sum2 += (int64_t)*p_kernel++ * (int64_t)in;
            sum3 += (int64_t)*p_kernel++ * (int64_t)in;
        } else if (dim_output == 3) {
            sum0 += (int64_t)*p_kernel++ * (int64_t)in;
            sum1 += (int64_t)*p_kernel++ * (int64_t)in;
            sum2 += (int64_t)*p_kernel++ * (int64_t)in;
        } else if (dim_output == 2) {
            sum0 += (int64_t)*p_kernel++ * (int64_t)in;
            sum1 += (int64_t)*p_kernel++ * (int64_t)in;
        } else // if (dim_output==1)
            sum0 += (int64_t)*p_kernel++ * (int64_t)in;
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
    } else if (dim_output == 1) {
        pt_accum[0] = sum0;
    }

    shift = qbit_s - (qbit_input + qbit_kernel);
    shift_64b(pt_accum, shift, dim_output); // align acc to w
    #if AMBIQ_NNSP_DEBUG == 1
    printf("%lld\n", pt_accum[0]);
    #endif
    if (p_bias != 0) {
        shift = qbit_s - (qbit_bias);
        // align w to acc & add
        for (i = 0; i < dim_output; i++) {
            pt_accum[i] +=
                (shift >= 0) ? ((int64_t)*p_bias++) << shift : ((int64_t)*p_bias++) >> -shift;
        }
    }
    #if AMBIQ_NNSP_DEBUG == 1
    printf("%lld\n", pt_accum[0]);
    #endif
    if (is_out) {
        shift = 15 - qbit_s;
        shift_64b(pt_accum, shift, dim_output);
        for (i = 0; i < dim_output; i++) {
            acc32[i] = (int32_t)MIN(MAX(pt_accum[i], MIN_INT32_T), MAX_INT32_T);
        }

        po = *pp_output;
        po = (int16_t *)(*act)(po, acc32, dim_output);
        *pp_output = po;
    }
    #if AMBIQ_NNSP_DEBUG == 1
    printf("%d\n", acc32[0]);
    #endif
    *pp_kernel = p_kernel;
    *pp_bias = p_bias;

    return 0;
}
#else
int affine_Krows_8x16(
    int16_t dim_output, int16_t **pp_output, int8_t **pp_kernel, int16_t **pp_bias, int16_t *input,
    int16_t dim_input, int16_t qbit_kernel, int16_t qbit_bias, int16_t qbit_input,
    int64_t *pt_accum, int8_t is_out, void *(*act)(void *, int32_t *, int)) {
    int8_t *p_kernel = *pp_kernel;
    int16_t *p_bias = *pp_bias;
    int16_t *po = *pp_output;
    int16_t *pi = input;
    int32_t nbit_out = 32;
    __attribute__((aligned(16))) int16_t in[2];
    __attribute__((aligned(16))) int32_t acc32[4];
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
                (int64_t)p_kernel[0] * (int64_t)in[0] + (int64_t)p_kernel[1] * (int64_t)in[1];
            p_kernel += 2;
        }
    }
    #if AMBIQ_NNSP_DEBUG == 1
    printf("%lld\n", pt_accum[0]);
    #endif
    if (dim_input % 2) {
        in[0] = *pi++;
        for (j = 0; j < dim_output; j++)
            pt_accum[j] += (int64_t)*p_kernel++ * (int64_t)in[0];
    }

    shift = qbit_s - (qbit_input + qbit_kernel);
    shift_64b(pt_accum, shift, dim_output); // align acc to w
    #if AMBIQ_NNSP_DEBUG == 1
    printf("%lld\n", pt_accum[0]);
    #endif
    if (p_bias != 0) {
        shift = qbit_s - (qbit_bias);
        // align w to acc & add
        for (i = 0; i < dim_output; i++) {
            pt_accum[i] +=
                (shift >= 0) ? ((int64_t)*p_bias++) << shift : ((int64_t)*p_bias++) >> -shift;
        }
    }
    #if AMBIQ_NNSP_DEBUG == 1
    printf("%lld\n", pt_accum[0]);
    #endif
    if (is_out) {
        shift = 15 - qbit_s;
        shift_64b(pt_accum, shift, dim_output);
        for (i = 0; i < dim_output; i++) {
            acc32[i] = (int32_t)MIN(MAX(pt_accum[i], MIN_INT32_T), MAX_INT32_T);
        }

        po = *pp_output;
        po = (int16_t *)(*act)(po, acc32, dim_output);
        *pp_output = po;
    }
    #if AMBIQ_NNSP_DEBUG == 1
    printf("%d\n", acc32[0]);
    #endif
    *pp_kernel = p_kernel;
    *pp_bias = p_bias;

    return 0;
}

#endif
int rc_Krows_8x16(
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
        accumulators[j] = 0;

    is_out = 0;
    affine_Krows_8x16(
        dim_output, &p_output, &p_kernel, &p_bias_null, input, dim_input, qbit_kernel, qbit_bias,
        qbit_input, accumulators, is_out, act);
    shift_64b(accumulators, shift, dim_output);

    is_out = 1;
    affine_Krows_8x16(
        dim_output, &p_output, &p_kernel_rec, &p_bias, input_rec, dim_input_rec, qbit_kernel,
        qbit_bias, qbit_input_rec, accumulators, is_out, act);
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

int fc_8x16(
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
            accumulators[j] = 0;

        affine_Krows_8x16(
            4, &po, &pw, &pb, input, dim_input, qbit_kernel, qbit_bias, qbit_input, accumulators,
            is_out, act);
#if DEBUG_PRINT
        for (j = 0; j < 4; j++)
            fprintf(file_nn_pre, "%d ", (int32_t)acc[j]);
#endif
    }
    if (rem_rows) {

        for (j = 0; j < rem_rows; j++)
            accumulators[j] = 0;

        affine_Krows_8x16(
            rem_rows, &po, &pw, &pb, input, dim_input, qbit_kernel, qbit_bias, qbit_input,
            accumulators, is_out, act);
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

int rc_8x16(
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
        rc_Krows_8x16(
            4, &po, &pw, &pw_r, &pb, input, input_rec, dim_input, dim_input_rec, qbit_kernel,
            qbit_bias, qbit_input, qbit_input_rec, act);
    }
    if (rem_rows) {
        rc_Krows_8x16(
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

void shift_64b(int64_t *x, int8_t shift, int len) {
    int i;
    int64_t M, m;
    if (shift == 0) {

    } else if (shift < 0) {
        shift = -shift;
        for (i = 0; i < len; i++)
            x[i] >>= shift;
    } else {
        M = (int64_t)1 << (63 - shift);
        m = -M;
        M -= 1;
        for (i = 0; i < len; i++) {
            x[i] = MIN(MAX(x[i], m), M);
            x[i] <<= shift;
        }
    }
}
