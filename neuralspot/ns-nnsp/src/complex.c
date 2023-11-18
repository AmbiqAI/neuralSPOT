#include "ambiq_stdint.h"
#include "ambiq_nnsp_debug.h"
#include "minmax.h"
#include "complex.h"
#if AMBIQ_NNSP_DEBUG
    #include <stdio.h>
#endif
void complex32_copy(COMPLEX32 *dst, COMPLEX32 *src) {
    dst->real = src->real;
    dst->imag = src->imag;
}
void complex32_affine(COMPLEX32 *out, COMPLEX32 *Mat, COMPLEX32 *input, int shift_r, int len) {
    int i;
    COMPLEX32 *pMat = Mat;
    for (i = 0; i < len; i++) {
        complex32_interprod(out + i, input, pMat, shift_r, len);
        pMat += len;
    }
}

void complex32_interprod(COMPLEX32 *out, COMPLEX32 *arry1, COMPLEX32 *arry2, int shift_r, int len) {
    int i;
    int64_t real, imag;
    real = 0;
    imag = 0;
    for (i = 0; i < len; i++) {
        real += (int64_t)arry1[i].real * (int64_t)arry2[i].real -
                (int64_t)arry1[i].imag * (int64_t)arry2[i].imag;
        imag += (int64_t)arry1[i].real * (int64_t)arry2[i].imag +
                (int64_t)arry1[i].imag * (int64_t)arry2[i].real;
    }
    real >>= shift_r;
    out->real = (int32_t)MAX(MIN(real, MAX_INT32_T), MIN_INT32_T);
    imag >>= shift_r;
    out->imag = (int32_t)MAX(MIN(imag, MAX_INT32_T), MIN_INT32_T);
}

void complex32_complex16_elmtprod(COMPLEX32 *out, COMPLEX32 *arry1, COMPLEX16 *arry2, int len) {
    int i;
    int64_t real, imag;

    for (i = 0; i < len; i++) {
        real = (int64_t)arry1[i].real * (int64_t)arry2[i].real -
               (int64_t)arry1[i].imag * (int64_t)arry2[i].imag;
        imag = (int64_t)arry1[i].real * (int64_t)arry2[i].imag +
               (int64_t)arry1[i].imag * (int64_t)arry2[i].real;
        real >>= 15;
        out[i].real = (int32_t)MAX(MIN(real, MAX_INT32_T), MIN_INT32_T);
        imag >>= 15;
        out[i].imag = (int32_t)MAX(MIN(imag, MAX_INT32_T), MIN_INT32_T);
    }
}

void complex32_add(COMPLEX32 *out, COMPLEX32 *addr1, COMPLEX32 *addr2) {
    out->real = addr1->real + addr2->real;
    out->imag = addr1->imag + addr2->imag;
}

void complexArry32_add(COMPLEX32 *out, COMPLEX32 *addr1, COMPLEX32 *addr2, int len) {
    int i;
    for (i = 0; i < len; i++) {
        complex32_add(out + i, addr1 + i, addr2 + i);
    }
}

void complex32_neg(COMPLEX32 *out, COMPLEX32 *in) {
    out->real = -in->real;
    out->imag = -in->imag;
}

void complex32_sub(COMPLEX32 *out, COMPLEX32 *a, COMPLEX32 *b) {
    complex32_neg(b, b);
    complex32_add(out, a, b);
}

void complex32_mul(COMPLEX32 *out, COMPLEX32 *addr1, COMPLEX32 *addr2) {
    out->real = addr1->real * addr2->real - addr1->imag * addr2->imag;
    out->imag = addr1->real * addr2->imag + addr1->imag * addr2->real;
}

void complex32_init(COMPLEX32 *inst, int32_t real, int32_t imag) {
    inst->real = real;
    inst->imag = imag;
}

void complex32_real2cmplx(COMPLEX32 *inst, int32_t real) { complex32_init(inst, real, 0); }

void complexArry32_real2cmplx(COMPLEX32 *inst, int32_t *real, int32_t len) {
    int i;
    for (i = 0; i < len; i++) {
        complex32_real2cmplx(inst + i, real[i]);
    }
}

void complexArry32_init(COMPLEX32 *inst, int32_t *real, int32_t *imag, int len) {
    int i;
    for (i = 0; i < len; i++) {
        complex32_init(inst + i, real[i], imag[i]);
    }
}
#if AMBIQ_NNSP_DEBUG
void complexArry32_print(COMPLEX32 *inst, int len) {
    int i;
    for (i = 0; i < len; i++) {
        printf("%d: (%d, %d)\n", i, inst[i].real, inst[i].imag);
    }
}
#endif
