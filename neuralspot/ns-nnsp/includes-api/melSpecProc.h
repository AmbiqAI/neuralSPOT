#ifndef __MELSPECPROC_H__
#define __MELSPECPROC_H__
#ifdef __cplusplus
extern "C" {
#endif
#include "ambiq_stdint.h"
void melSpecProc(
    int32_t *specs, int32_t *melSpecs, const int16_t *p_melBanks, int16_t num_mfltrBank);
#ifdef __cplusplus
}
#endif
#endif
