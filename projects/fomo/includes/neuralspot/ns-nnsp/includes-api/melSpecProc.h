#ifndef __MELSPECPROC_H__
#define __MELSPECPROC_H__
#include "ambiq_stdint.h"
void melSpecProc(
    int32_t *specs, int32_t *melSpecs, const int16_t *p_melBanks, int16_t num_mfltrBank);
#endif
