#ifndef __BASIC_MVE_H__
#define __BASIC_MVE_H__
#include <stdint.h>
void vec16_vec16_mul_32b(
    int32_t *y,
    int16_t *x1,
    int16_t *x2,
    int16_t len);

void move_data_16b(
    int16_t *src,
    int16_t *dst,
    int len);
void set_zero_32b(
    int32_t *dst,
    int len);
int64_t interproduct_32x16(
    int32_t *in32,
    int16_t *in16,
    int16_t len);
#endif // __BASIC_MVE_H__