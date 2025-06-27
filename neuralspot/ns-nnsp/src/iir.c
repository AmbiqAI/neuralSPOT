#include <stdint.h>
#include "minmax.h"
#include "ambiq_stdint.h"
#include "iir.h"
void IIR_CLASS_init(IIR_CLASS *inst) {
    inst->b = -32768;
    inst->a = -31630;
}
void IIR_CLASS_reset(IIR_CLASS *inst) { inst->state = 0; }

void IIR_CLASS_exec(IIR_CLASS *inst, int16_t *outputs, int16_t *inputs, int len) {
    int64_t g;
    int64_t tmp;
    for (int i = 0; i < len; i++) {
        g = ((int64_t)inputs[i]) - ((((int64_t)inst->a) * ((int64_t)inst->state)) >> 15);
        g = MAX(MIN(g, MAX_INT32_T), MIN_INT32_T);
        tmp = ((int64_t)g) + ((((int64_t)inst->state) * ((int64_t)inst->b)) >> 15);
        tmp = MAX(MIN(tmp, MAX_INT16_T), MIN_INT16_T);
        outputs[i] = (int16_t)tmp;

        inst->state = (int32_t)g;
    }
}
