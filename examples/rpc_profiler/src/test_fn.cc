
#include "ns_core.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_energy_monitor.h"
#include "ns_peripherals_power.h"
#include "ns_peripherals_button.h"
#include "ns_pmu_utils.h"
#include "ns_power_profile.h"
#include "nn/arm_nnfunctions.h"

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

static int16_t input1_s16[VEC_SIZE] = {0};
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
int test_fn(void) {
    //mul_arm_elementwise_mul_1_s8();
    mul_s16_arm_elementwise_mul_s16();
    return 0;
}

void init_load(void) {
    for(size_t i = 0; i < VEC_SIZE; i++) {
        input_1[i] =  i + 1;
        input_2[i] = 2*i;
        input1_s16[i] = 3*i;
        input2_s16[i] = 4*i; 
    }
}

int run_load(void) {
    return test_fn();
}

