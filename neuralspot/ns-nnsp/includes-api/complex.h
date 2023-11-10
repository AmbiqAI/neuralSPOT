#ifndef __COMPLEX_H__
#define __COMPLEX_H__
#include <stdint.h>
typedef struct {
    int32_t real;
    int32_t imag;
} COMPLEX32;

typedef struct {
    int16_t real;
    int16_t imag;
} COMPLEX16;

void complex32_copy(COMPLEX32 *dst, COMPLEX32 *src);

void complex32_affine(COMPLEX32 *out, COMPLEX32 *Mat, COMPLEX32 *input, int shift_r, int len);
void complex32_interprod(COMPLEX32 *out, COMPLEX32 *arry1, COMPLEX32 *arry2, int shift_r, int len);

void complex32_complex16_elmtprod(COMPLEX32 *out, COMPLEX32 *arry1, COMPLEX16 *arry2, int len);

void complex32_add(COMPLEX32 *out, COMPLEX32 *addr1, COMPLEX32 *addr2);
void complexArry32_add(COMPLEX32 *out, COMPLEX32 *addr1, COMPLEX32 *addr2, int len);
void complex32_neg(COMPLEX32 *out, COMPLEX32 *in);

void complex32_sub(COMPLEX32 *out, COMPLEX32 *a, COMPLEX32 *b);

void complex32_mul(COMPLEX32 *out, COMPLEX32 *addr1, COMPLEX32 *addr2);

void complex32_init(COMPLEX32 *inst, int32_t real, int32_t imag);

void complex32_real2cmplx(COMPLEX32 *inst, int32_t real);

void complexArry32_real2cmplx(COMPLEX32 *inst, int32_t *real, int32_t len);
void complexArry32_init(COMPLEX32 *inst, int32_t *real, int32_t *imag, int len);
void complexArry32_print(COMPLEX32 *inst, int len);
#endif
