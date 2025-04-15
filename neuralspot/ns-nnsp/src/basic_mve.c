/*
Basic MVE Function for Helium architecture
*/
#include <string.h>
#include <stdint.h>
#include "ambiq_nnsp_debug.h"
// #include "third_party/ns_cmsis_nn/Include/arm_nnsupportfunctions.h"
#if ARM_OPTIMIZED == 3
#include "basic_mve.h"
#include <arm_mve.h>
#include "ns_ambiqsuite_harness.h"
/**
 * @brief           memcpy optimized for MVE
 * @param[in, out]  dst         Destination pointer
 * @param[in]       src         Source pointer.
 * @param[in]       block_size  Number of bytes to copy.
 *
 */
__STATIC_FORCEINLINE void
arm_memcpy_int8(int8_t *__RESTRICT dst, const int8_t *__RESTRICT src, uint32_t block_size) {
#if defined(ARM_MATH_MVEI)
    __asm volatile("   wlstp.8                 lr, %[cnt], 1f             \n"
                   "2:                                                    \n"
                   "   vldrb.8                 q0, [%[in]], #16            \n"
                   "   vstrb.8                 q0, [%[out]], #16           \n"
                   "   letp                    lr, 2b                     \n"
                   "1:                                                    \n"
                   : [in] "+r"(src), [out] "+r"(dst)
                   : [cnt] "r"(block_size)
                   : "q0", "memory", "r14");
#else
    memcpy(dst, src, block_size);
#endif
}
void vec16_vec16_mul_32b(
        int32_t *y,
        int16_t *x1,
        int16_t *x2,
        int16_t len) {
    int i;
    int32x4_t m1, m2;
    int num = len >> 2;
    int rem = len & 0x3;
    int32x4_t *pt_y = (int32x4_t*) y;

    for (i = 0; i < num; i++) {
        m1 = vldrhq_s32(x1);
        x1 += 4;
        m2 = vldrhq_s32(x2);
        x2 += 4;
        *pt_y = m1 * m2;
        pt_y++;
    }
    if (rem) {
        mve_pred16_t mask = vctp32q((uint32_t) rem);
        m1 = vldrhq_z_s32(x1, mask);
        m2 = vldrhq_z_s32(x2, mask);
        int32x4_t out = m1 * m2;
        vst1q_p_s32((int32_t*) pt_y, out, mask);
    }
}

void move_data_16b(
        int16_t *src,
        int16_t *dst,
        int len)
{
    arm_memcpy_int8(
        (int8_t*) dst,
        (int8_t*) src,
        (len << 1));
}

void set_zero_32b(int32_t *dst, int len)
{
    int i;
    int num = len >> 2;
    int rem = len & 0x3;
    int32x4_t zero= {0, 0, 0, 0};
    int32x4_t *pt_dst = (int32x4_t*) dst;
    for (i = 0; i < num; i++)
    {
        *pt_dst++ = zero;
    }
    if (rem) {
        mve_pred16_t mask = vctp32q((uint32_t) rem);
        vst1q_p_s32((int32_t*) pt_dst, zero, mask);
    }
}

int64_t interproduct_32x16(
    int32_t *in32,
    int16_t *in16,
    int16_t len)
{
    int16_t num = len >> 2;
    int16_t rem = len & 0x3;
    int64_t result = 0;

    for (int i = 0; i < num; i++)
    {
        int32x4_t m1 = vldrwq_s32(in32);
        in32 += 4;
        int32x4_t m2 = vldrhq_s32(in16);
        in16 += 4;
        result = vmlaldavaq_s32(result, m1, m2);
    }
    if (rem)
    {
        mve_pred16_t mask = vctp32q((uint32_t) rem);
        int32x4_t m1 = vldrwq_z_s32(in32, mask);
        int32x4_t m2 = vldrhq_z_s32(in16, mask);
        result = vmlaldavaq_s32(result, m1, m2);
    }

    return result;

}
#endif