#include "ambiq_nnsp_debug.h"
#if ARM_FFT == 0
    #include "ambiq_stdint.h"
    #include "complex.h"
    #include "minmax.h"
    #include "fft.h"
    #define EXP_TOTAL_NFFT 8 // 2^8 point fft
extern const int32_t fft_tw_coeff[];
extern const int32_t rfft_tw_coeff[];

extern const int16_t br_coeff[];
int32_t M4_[] = {1, 0, 1,  0, 1, 0,  1, 0, 1, 0, 1,  0, -1, 0, -1, 0,
                 1, 0, -1, 0, 0, -1, 0, 1, 1, 0, -1, 0, 0,  1, 0,  -1};

int32_t M2_[] = {1, 0, 1, 0, 1, 0, -1, 0};

__attribute__((aligned(16))) COMPLEX32 ti[4];
__attribute__((aligned(16))) COMPLEX32 to[4];
__attribute__((aligned(16))) COMPLEX32 cinput[256];

__attribute__((aligned(16))) COMPLEX32 Xe[256];
COMPLEX32 *Xo = cinput;

void rfft(int num_rfft, int32_t *input, void *output_) {
    int i;
    int exp_nfft;
    COMPLEX16 *tw = (COMPLEX16 *)rfft_tw_coeff;
    COMPLEX32 *output = (COMPLEX32 *)output_;
    COMPLEX32 tmp;
    COMPLEX32 *po = output;
    COMPLEX32 *pr;

    COMPLEX32 *pXe = Xe;
    COMPLEX32 *pXo = Xo;
    int64_t real, imag;
    int R;
    exp_nfft = 0;
    if (num_rfft == 256) {
        exp_nfft = 7;
    } else if (num_rfft == 512) {
        exp_nfft = 8;
    }
    R = 1 << (EXP_TOTAL_NFFT - exp_nfft);
    for (i = 0; i < (num_rfft >> 1); i++) {
        complex32_init(cinput + i, input[2 * i], input[2 * i + 1]);
    }
    fft(exp_nfft, (void *)cinput, (void *)output);

    /*for (i = 0; i < (num_rfft >> 1); i++)
    {
        printf("%d : (%d, %d)\n", i, output[i].real, output[i].imag);
    }*/
    // tmp = conj(Z(1));
    // Xe(1) = (Z(1) + tmp) / 2;
    // Xo(1) = -1j * (Z(1) - tmp) / 2;
    tmp.real = po->real;
    tmp.imag = -po->imag;
    pXe->real = (po->real + tmp.real) >> 1;
    pXe->imag = (po->imag + tmp.imag) >> 1;

    pXo->real = (po->imag - tmp.imag) >> 1;
    pXo->imag = -(po->real - tmp.real) >> 1;

    pr = output + (num_rfft >> 1) - 1;
    po = output + 1;
    pXe++;
    pXo++;

    // for i = 1 : N/2 - 1
    //     idx = N/2 - i + 1;
    //     tmp = conj(Z(idx));
    //     Xe(i+1) = (Z(i+1) + tmp) / 2;
    //     Xo(i+1) = -1j * (Z(i+1) - tmp) / 2;
    // end

    for (i = 1; i < (num_rfft >> 1); i++) {
        tmp.real = pr->real;
        tmp.imag = -pr->imag;

        pXe->real = (po->real + tmp.real) >> 1;
        pXe->imag = (po->imag + tmp.imag) >> 1;

        pXo->real = (po->imag - tmp.imag) >> 1;
        pXo->imag = -(po->real - tmp.real) >> 1;

        pr--;
        po++;
        pXe++;
        pXo++;
    }

    // k = 0 : N/2-1;
    // tw = exp(-1j*2*pi/N *k);
    // X = Xe + Xo .* tw;

    for (i = 0; i < num_rfft >> 1; i++) {
        real = (int64_t)Xo[i].real * (int64_t)tw[R * i].real -
               (int64_t)Xo[i].imag * (int64_t)tw[R * i].imag;
        imag = (int64_t)Xo[i].real * (int64_t)tw[R * i].imag +
               (int64_t)Xo[i].imag * (int64_t)tw[R * i].real;
        real >>= 15;
        output[i].real = (int32_t)MAX(MIN(real, MAX_INT32_T), MIN_INT32_T);
        imag >>= 15;
        output[i].imag = (int32_t)MAX(MIN(imag, MAX_INT32_T), MIN_INT32_T);
    }
    for (i = 0; i < (num_rfft >> 1); i++) {
        // printf("%d : (%d, %d)\n", i, output[i].real, output[i].imag);
    }
    complexArry32_add(output, Xe, output, num_rfft >> 1);

    // X(N/2+1) = Xe(1) - Xo(1);
    output[num_rfft >> 1].real = Xe[0].real - Xo[0].real;
    output[num_rfft >> 1].imag = Xe[0].imag - Xo[0].imag;
}

void fft(int exp_nfft, void *input_, void *output_) {
    int eNt = EXP_TOTAL_NFFT;
    int s, g, m, k;
    int r = 0;
    int stages = 0;
    int s_idx, idx;
    COMPLEX16 *pt_tw = (COMPLEX16 *)fft_tw_coeff;
    int Ng;
    int Nfd4, Nf;
    int S;
    int nfft = 1 << exp_nfft;
    COMPLEX32 *output = (COMPLEX32 *)output_;
    COMPLEX32 *input = (COMPLEX32 *)input_;
    COMPLEX32 *p_cin = input;
    COMPLEX32 *M4 = (COMPLEX32 *)M4_;
    COMPLEX32 *M2 = (COMPLEX32 *)M2_;
    int Rs = (eNt - exp_nfft);

    // complexArry_int32_real2cmplx(ctmp, input, nfft);

    if (nfft == 128) {
        stages = 3;
        r = 1;
    } else if (nfft == 256) {
        stages = 4;
        r = 0;
    }
    Nf = nfft;
    Ng = 1;
    S = 1 << (eNt - exp_nfft);
    for (s = 0; s < stages; s++) {
        Nfd4 = Nf >> 2;
        for (g = 0; g < Ng; g++) {
            k = 0;
            s_idx = g * Nf;
            for (m = 0; m < Nfd4; m++) {
                idx = s_idx;
                complex32_copy(ti + 0, p_cin + idx);
                idx += Nfd4;
                complex32_copy(ti + 2, p_cin + idx);
                idx += Nfd4;
                complex32_copy(ti + 1, p_cin + idx);
                idx += Nfd4;
                complex32_copy(ti + 3, p_cin + idx);

                // t = (M*t) .* TW(k,:).';
                complex32_affine(to, M4, ti, 0, 4);
                complex32_complex16_elmtprod(ti, to, pt_tw + k * 4, 4);
                k += S;

                idx = s_idx;
                complex32_copy(p_cin + idx, ti + 0);
                idx += Nfd4;
                complex32_copy(p_cin + idx, ti + 1);
                idx += Nfd4;
                complex32_copy(p_cin + idx, ti + 2);
                idx += Nfd4;
                complex32_copy(p_cin + idx, ti + 3);

                s_idx += 1;
            }
        }
        Nf >>= 2;
        Ng <<= 2;
        S <<= 2;
    }

    if (r == 1) {
        Ng = nfft >> 1;
        p_cin = input;
        for (m = 0; m < Ng; m++) {
            // t = M * y(idx);
            complex32_affine(to, M2, p_cin, 0, 2);
            // y(idx) = t;
            complex32_copy(p_cin, to);
            complex32_copy(p_cin + 1, to + 1);
            p_cin += 2;
        }
    }

    for (m = 0; m < nfft; m++) {
        complex32_copy(output + m, input + (br_coeff[m] >> Rs));
    }
}
#endif
