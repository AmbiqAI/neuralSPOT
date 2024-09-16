#ifndef __FIX_LOG10_H__
#define __FIX_LOG10_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
void norm_oneTwo(int32_t x, int32_t *y, int8_t *shift);

void my_log10(int32_t *out, int32_t x);

void log10_vec(int32_t *out, int32_t *x, int32_t len, int16_t bit_frac_in);
#ifdef __cplusplus
}
#endif
#endif /*  */
