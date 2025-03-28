#include "ambiq_stdint.h"
#include "melSpecProc.h"
#include "minmax.h"
#include "ambiq_nnsp_debug.h"
#if ARM_OPTIMIZED==3
#include "arm_mve.h"
#include "basic_mve.h"
#include "ns_ambiqsuite_harness.h"
void melSpecProc(
        int32_t *pspec,
        int32_t *melSpecs,
        const int16_t *p_melBanks,
        int16_t num_mfltrBank) {

    int16_t start_bin, end_bin;
    int64_t mac;
    int16_t len;
    for (int i = 0; i < num_mfltrBank; i++) {
        start_bin = *p_melBanks++;
        end_bin = *p_melBanks++;
        len = end_bin - start_bin + 1;
        mac = interproduct_32x16(pspec+start_bin, (int16_t*) p_melBanks, len);
        p_melBanks += len;
        mac >>= 15;
        melSpecs[i] = (int32_t) MIN(MAX(mac, MIN_INT32_T), MAX_INT32_T);
    }
}
#else
void melSpecProc(
    int32_t *pspec, int32_t *melSpecs, const int16_t *p_melBanks, int16_t num_mfltrBank) {
    int i, j;
    int16_t start_bin, end_bin;
    int64_t mac;

    for (i = 0; i < num_mfltrBank; i++) {
        start_bin = *p_melBanks++;
        end_bin = *p_melBanks++;
        mac = 0;
        for (j = start_bin; j <= end_bin; j++) {
            mac += ((int64_t)*p_melBanks++) * ((int64_t)pspec[j]);
        }
        mac >>= 15;
        melSpecs[i] = (int32_t)MIN(MAX(mac, MIN_INT32_T), MAX_INT32_T);
    }
}
#endif