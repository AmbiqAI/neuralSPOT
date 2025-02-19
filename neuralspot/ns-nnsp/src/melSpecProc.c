#include "ambiq_stdint.h"
#include "melSpecProc.h"
#include "minmax.h"
void melSpecProc(
    int32_t *specs, int32_t *melSpecs, const int16_t *p_melBanks, int16_t num_mfltrBank) {
    int i, j;
    int16_t start_bin, end_bin;
    int64_t mac;

    for (i = 0; i < num_mfltrBank; i++) {
        start_bin = *p_melBanks++;
        end_bin = *p_melBanks++;
        mac = 0;
        for (j = start_bin; j <= end_bin; j++) {
            mac += ((int64_t)*p_melBanks++) * ((int64_t)specs[j]);
        }
        mac >>= 15;
        melSpecs[i] = (int32_t)MIN(MAX(mac, MIN_INT32_T), MAX_INT32_T);
    }
}
