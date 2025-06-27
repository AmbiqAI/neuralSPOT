#ifndef __ACTIVATION_H__
#define __ACTIVATION_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
void *relu6_fix(
    int16_t *y, // Q12
    int32_t *x, // Q15
    int len);
void *linear_fix(
    int32_t *y, // Q15
    int32_t *x, // Q15
    int len);
void *tanh_fix(int16_t *y, int32_t *x, int len);
void *sigmoid_fix(int16_t *y, int32_t *x, int len);
typedef enum { relu6, ftanh, sigmoid, linear } ACTIVATION_TYPE;

#ifdef __cplusplus
}
#endif
#endif
