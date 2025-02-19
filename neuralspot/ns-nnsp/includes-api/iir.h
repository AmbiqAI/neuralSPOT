#ifndef __IIR_CLASS_H__
#define __IIR_CLASS_H__

#include <stdint.h>
typedef struct {
    int16_t a;
    int16_t b;
    int32_t state;
} IIR_CLASS;
void IIR_CLASS_init(IIR_CLASS *inst);
void IIR_CLASS_reset(IIR_CLASS *inst);
void IIR_CLASS_exec(IIR_CLASS *inst, int16_t *outputs, int16_t *inputs, int len);

#endif
