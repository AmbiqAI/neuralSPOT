#ifndef __FEATURE_MODULE_H__
#define __FEATURE_MODULE_H__
#include <stdint.h>
#include "spectrogram_module.h"
#include "ambiq_nnsp_const.h"

typedef struct {
    stftModule state_stftModule;
    int32_t feature[MAX_SIZE_FEATURE];
    int16_t num_mfltrBank;
    const int16_t *p_melBanks;
    // contextual normalized features
    int16_t normFeatContext[NUM_FEATURE_CONTEXT * MAX_SIZE_FEATURE];
    int16_t num_context;
    int16_t dim_feat;
    const int32_t *pt_norm_mean;
    const int32_t *pt_norm_stdR;
    int8_t qbit_output;
    void *pt_dcrm;
} FeatureClass;

void FeatureClass_construct(
    FeatureClass *ps, const int32_t *norm_mean, const int32_t *norm_stdR, int8_t qbit_output,
    int16_t num_mfltrBank, int16_t winsize, int16_t hopsize, int16_t fftsize,
    const int16_t *pt_stft_win_coeff);

void FeatureClass_setDefault(FeatureClass *ps);

void FeatureClass_execute(FeatureClass *ps, int16_t *input);

#endif
