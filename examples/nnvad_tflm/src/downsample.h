#ifndef __DOWNSAMPLE_H__
#define __DOWNSAMPLE_H__
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
/*
low-cost downsample filter (16kHz -> 8kHz)
*/
typedef struct 
{
    int16_t len_filter;
    int16_t *states;
    const int16_t *coef;
}DOWNSAMPLE_CLASS;
void DOWNSAMPLE_CLASS_init(DOWNSAMPLE_CLASS *pt_inst);
void DOWNSAMPLE_CLASS_reset(DOWNSAMPLE_CLASS *pt_inst);
void DOWNSAMPLE_CLASS_exec(
    DOWNSAMPLE_CLASS *pt_inst,
    int16_t *outputs,
    int16_t *inputs,
    int16_t len_inputs);
#ifdef __cplusplus
}
#endif
#endif // __DOWNSAMPLE_H__
