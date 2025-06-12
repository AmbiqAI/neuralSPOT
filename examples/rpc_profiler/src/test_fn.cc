
#include "ns_core.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_energy_monitor.h"
#include "ns_peripherals_power.h"
#include "ns_peripherals_button.h"
#include "ns_pmu_utils.h"
#include "ns_power_profile.h"
#include "ns_cmsis_nn/Include/arm_nnfunctions.h"
#include "ns_cmsis_nn/Include/arm_nnsupportfunctions.h"


#define VEC_SIZE 200 

//int8
#define MUL_LHS_OFFSET 128
#define MUL_RHS_OFFSET 128
#define MUL_OUTPUT_OFFSET -128
#define MUL_ACTIVATION_MIN -128
#define MUL_ACTIVATION_MAX 127
#define MUL_OUTPUT_MULTIPLIER 1086432245
#define MUL_OUTPUT_SHIFT -7

//int16
#define MUL_S16_OUT_ACTIVATION_MIN -32768
#define MUL_S16_OUT_ACTIVATION_MAX 32767
#define MUL_S16_INPUT1_OFFSET 0
#define MUL_S16_INPUT2_OFFSET 0
#define MUL_S16_OUTPUT_MULT 1073774592
#define MUL_S16_OUTPUT_SHIFT -14
#define MUL_S16_OUTPUT_OFFSET 0

static int8_t input_1[VEC_SIZE] = {0};
static int8_t input_2[VEC_SIZE] = {0};
static int8_t output[VEC_SIZE] = {0};

static __attribute__((section(".rodata"))) int16_t input1_s16[VEC_SIZE] = {0};
static int16_t input2_s16[VEC_SIZE] = {0};
static int16_t output_s16[VEC_SIZE] = {0};

void mul_arm_elementwise_mul_1_s8(void)
{
    const int8_t *input_data1 = input_1;
    const int8_t *input_data2 = input_2;
    
    const int32_t input_1_offset = MUL_LHS_OFFSET;
    const int32_t input_2_offset = MUL_RHS_OFFSET;

    const int32_t out_offset =  MUL_OUTPUT_OFFSET;
    const int32_t out_mult = MUL_OUTPUT_MULTIPLIER;
    const int32_t out_shift = MUL_OUTPUT_SHIFT;

    const int32_t out_activation_min = MUL_ACTIVATION_MIN; 
    const int32_t out_activation_max = MUL_ACTIVATION_MAX;

    arm_elementwise_mul_s8(input_data1,
                                input_data2,
                                input_1_offset,
                                input_2_offset,
                                output,
                                out_offset,
                                out_mult,
                                out_shift,
                                out_activation_min,
                                out_activation_max,
                                VEC_SIZE);
}

void mul_s16_arm_elementwise_mul_s16(void)
{
    const int16_t *input_data1 = input1_s16;
    const int16_t *input_data2 = input2_s16;

    const int32_t input_1_offset = MUL_S16_INPUT1_OFFSET;
    const int32_t input_2_offset = MUL_S16_INPUT2_OFFSET;

    const int32_t out_offset = MUL_S16_OUTPUT_OFFSET;
    const int32_t out_mult = MUL_S16_OUTPUT_MULT;
    const int32_t out_shift = MUL_S16_OUTPUT_SHIFT;

    const int32_t out_activation_min = MUL_S16_OUT_ACTIVATION_MIN;
    const int32_t out_activation_max = MUL_S16_OUT_ACTIVATION_MAX;

    arm_elementwise_mul_s16(input_data1,
                                     input_data2,
                                     input_1_offset,
                                     input_2_offset,
                                     output_s16,
                                     out_offset,
                                     out_mult,
                                     out_shift,
                                     out_activation_min,
                                     out_activation_max,
                                     VEC_SIZE);

}

#define FC_RHS_COLS 240
#define FC_RHS_ROWS 960
int test_vec_mat_mult_t_s16(void) {
    static __attribute__((section(".rodata"))) int8_t in8_rhs[FC_RHS_COLS*FC_RHS_ROWS] = {0};
    //static __attribute__((section(".shared"))) int8_t in8_rhs[FC_RHS_COLS*FC_RHS_ROWS] = {0};
    //static int8_t in8_rhs[FC_RHS_COLS*FC_RHS_ROWS] = {0};
    //static __attribute__((section(".shared")))int16_t in16_lhs[FC_RHS_ROWS] = {0};
    static int16_t in16_lhs[FC_RHS_ROWS] = {0};
    static int16_t fc_out_dst[FC_RHS_COLS]  = {0};
    static int64_t in64_bias[FC_RHS_COLS] = {0};
    const int32_t out_offset = MUL_S16_OUTPUT_OFFSET;
    const int32_t out_mult = MUL_S16_OUTPUT_MULT;
    const int32_t out_shift = MUL_S16_OUTPUT_SHIFT;

    const int32_t out_activation_min = MUL_S16_OUT_ACTIVATION_MIN;
    const int32_t out_activation_max = MUL_S16_OUT_ACTIVATION_MAX;

    return arm_nn_vec_mat_mult_t_s16(in16_lhs,
            in8_rhs,
            in64_bias,
            fc_out_dst, out_mult, out_shift, FC_RHS_COLS, FC_RHS_ROWS, out_activation_min,out_activation_max);
    return 0;
}


#define CONV_C 96
#define CONV_H 7 
#define KERNEL_H 3
#define CONV_DILATION_H 3 
#define BASIC_INPUT_OFFSET 128
#define BASIC_OUTPUT_OFFSET 127
#define BASIC_OUT_ACTIVATION_MIN -128
#define BASIC_OUT_ACTIVATION_MAX 127
static cmsis_nn_context ctx;
static cmsis_nn_context weights_sum_ctx;
static cmsis_nn_conv_params conv_params;
static cmsis_nn_per_channel_quant_params quant_params;
static cmsis_nn_dims input_dims;
static cmsis_nn_dims filter_dims;
static cmsis_nn_dims bias_dims;
static cmsis_nn_dims output_dims;
static int32_t basic_output_mult[CONV_C] = {1625013239};
static int32_t basic_output_shift[CONV_C] = {-8};
static int32_t bias_data[CONV_C] = {0};
static int8_t conv_kernel_data[CONV_C * KERNEL_H] = {0};
static int8_t conv_input_data[CONV_H * CONV_C] = {0};
static int8_t conv_output[CONV_C] = {0};
static int8_t big_buf[60000] = {0};
static int8_t small_buf[2000] = {0};

void prep_convolve_s8(void) {

    input_dims.n = 1;
    input_dims.w = 1;
    input_dims.h = CONV_H;
    input_dims.c = CONV_C;
    filter_dims.n = 1;
    filter_dims.w = 1;
    filter_dims.h = KERNEL_H;
    filter_dims.c = CONV_C;
    output_dims.n = 1;
    output_dims.w = 1;
    output_dims.h = 1;
    output_dims.c = CONV_C;
    for (size_t i = 0; i < CONV_C; i++) {
        basic_output_mult[i] = 1625013239;
        basic_output_shift[i] = -8;
    }

    conv_params.padding.w = 0;
    conv_params.padding.h = 0;
    conv_params.stride.w = 1;
    conv_params.stride.h = 1;
    conv_params.dilation.w = 1;
    conv_params.dilation.h = CONV_DILATION_H;

    conv_params.input_offset = BASIC_INPUT_OFFSET;
    conv_params.output_offset = BASIC_OUTPUT_OFFSET;
    conv_params.activation.min = BASIC_OUT_ACTIVATION_MIN;
    conv_params.activation.max = BASIC_OUT_ACTIVATION_MAX;

    quant_params.multiplier = (int32_t *)basic_output_mult;
    quant_params.shift = (int32_t *)basic_output_shift;

    int32_t buf_size = arm_convolve_wrapper_s8_get_buffer_size(
          &conv_params, &input_dims, &filter_dims, &output_dims);

    ctx.buf = big_buf; 
    ctx.size = 0;

    int32_t weights_sum_buf_size = arm_convolve_s8_get_weights_sum_size(&output_dims);
    weights_sum_ctx.buf = small_buf; 
    weights_sum_ctx.size = weights_sum_buf_size;

}
int test_arm_convolve_s8(void) {


    return  arm_convolve_1_x_1_out_s8(&ctx,
                                     &weights_sum_ctx,
                                     &conv_params,
                                     &quant_params,
                                     &input_dims,
                                     conv_input_data,
                                     &filter_dims,
                                     conv_kernel_data,
                                     &bias_dims,
                                     bias_data,
                                     NULL,
                                     &output_dims,
                                     conv_output);
}



int  test_fn(void) {
    //mul_arm_elementwise_mul_1_s8();
    //mul_s16_arm_elementwise_mul_s16();
    //
    test_arm_convolve_s8();
    return 0;
}

void init_load(void) {

    //am_hal_cachectrl_prefetch_t prefetch;
    //prefetch.ui8MaxOutTransactions = 6;
    //prefetch.ui8MaxLookAhead = 6;
    //prefetch.ui8MinLookAhead = 6;
    //am_hal_cachectrl_dcache_disable();




    //am_hal_cachectrl_control(AM_HAL_CACHECTRL_REQ_PREFETCH_CFG, (void *)&prefetch);
    //MEMSYSCTL->PFCR =  (1 << 7) | (6 << 4) | (6 <<1) | 1;
    //MEMSYSCTL->PFCR |=  (4 << 7);

    prep_convolve_s8();
    volatile uint32_t* reg_mram_tmc_ctrl =  (uint32_t*)0x40014050;

    volatile uint32_t* reg_pwrctrl_mramextctrl =  (uint32_t*)0x400211C8;

    //*reg_pwrctrl_mramextctrl &= ~(0x1 << 4);
    //*reg_pwrctrl_mramextctrl |= (0x1 << 4);
    //*reg_mram_tmc_ctrl &= ~( (1 <<1 ) | (1 << 6) );
    //*reg_mram_tmc_ctrl |= ( (1 <<1 ) | (1 << 6) );

    for(size_t i = 0; i < VEC_SIZE; i++) {
        input_1[i] =  i + 1;
        input_2[i] = 2*i;
        //input1_s16[i] = 3*i;
        input2_s16[i] = 4*i; 
    }
}

int  run_load(void) {
    return test_fn();
}

