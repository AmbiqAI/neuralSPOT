#include <stdint.h>
#include "downsample.h"
#define LEN_DNSAMPLE 32
const int16_t glob_dnsample_coeff[LEN_DNSAMPLE]={0x008d, 0x019c, 0x01cb, 0xffef, 0xfdac, 0xfe4c, 0x01be, 0x0342, 0xffd2, 0xfbda, 0xfdd5, 0x03e2, 0x04d4, 0xfdef, 0xf8f0, 0xfe88, 0x07c9, 0x065e, 0xfa45, 0xf48b, 0xffa1, 0x0e25, 0x0ae8, 0xf65f, 0xe906, 0xf666, 0x15e9, 0x2d66, 0x2d4e, 0x1c73, 0x0b1e, 0x022a, };
int16_t glob_dnsample_input[LEN_DNSAMPLE-1+160];

void DOWNSAMPLE_CLASS_init(DOWNSAMPLE_CLASS *pt_inst)
{

    pt_inst->len_filter = LEN_DNSAMPLE;
    pt_inst->states = glob_dnsample_input;
    pt_inst->coef = glob_dnsample_coeff;
}

void DOWNSAMPLE_CLASS_reset(DOWNSAMPLE_CLASS *pt_inst)
{
    for (int16_t i = 0; i < (pt_inst->len_filter-1); i++)
    {
        pt_inst->states[i] = 0;
    }
}

void DOWNSAMPLE_CLASS_exec(
    DOWNSAMPLE_CLASS *pt_inst,
    int16_t *outputs,
    int16_t *inputs,
    int16_t len_inputs)
{
    int i,j;
    int64_t acc;
    int16_t *pt_inputs;
    for (i = 0; i < len_inputs; i++)
    {
        glob_dnsample_input[i + pt_inst->len_filter-1] = inputs[i];
    }
    pt_inputs = glob_dnsample_input;
    for (i = 0; i < (len_inputs >> 1); i++)
    {
        acc = 0;
        for (j = 0; j < pt_inst->len_filter; j++)
        {
            acc += (int64_t) pt_inst->coef[j] * (int64_t) pt_inputs[j];
        }
        pt_inputs += 2;
        outputs[i] = (int16_t)(acc >> 15);
    }
    // udpate states
    for (i = 0; i < pt_inst->len_filter; i++)
    {
        pt_inst->states[i] = inputs[160 - pt_inst->len_filter + i];
    }
}