#include "ambiq_stdint.h"
#include "spectrogram_module.h"
#include "feature_module.h"
#include "minmax.h"
#include "ambiq_nnsp_const.h"
#include "melSpecProc.h"
#include "fixlog10.h"
#include "ambiq_nnsp_debug.h"
#include "ns_ambiqsuite_harness.h"
#if ARM_OPTIMIZED == 3
#include "basic_mve.h"
#endif
#if AMBIQ_NNSP_DEBUG == 1
    #include "debug_files.h"
#endif
#define LOG10_2POW_N15_Q15 (-147963)
int32_t GLOBAL_PSPEC[512];
extern const int16_t mfltrBank_coeff_nfilt72_fftsize512[];
extern const int16_t mfltrBank_coeff_nfilt40_fftsize512[];
extern const int16_t mfltrBank_coeff_nfilt22_fftsize256[];

void FeatureClass_construct(
        FeatureClass *ps, 
        const int32_t *norm_mean, 
        const int32_t *norm_stdR, 
        int8_t qbit_output,
        int16_t num_mfltrBank, 
        int16_t winsize, 
        int16_t hopsize, 
        int16_t fftsize,
        const int16_t *pt_stft_win_coeff) 
    {

    stftModule_construct(&ps->state_stftModule, winsize, hopsize, fftsize, pt_stft_win_coeff);
    ps->pt_norm_mean = norm_mean;
    ps->pt_norm_stdR = norm_stdR;
    ps->num_context = NUM_FEATURE_CONTEXT;
    ps->dim_feat = num_mfltrBank;
    ps->qbit_output = qbit_output;
    ps->num_mfltrBank = num_mfltrBank;
    if ((ps->num_mfltrBank == 72) && (fftsize == 512))
        ps->p_melBanks = mfltrBank_coeff_nfilt72_fftsize512;
    else if ((ps->num_mfltrBank == 40) && (fftsize == 512))
        ps->p_melBanks = mfltrBank_coeff_nfilt40_fftsize512;
    else if ((ps->num_mfltrBank == 22) && (fftsize == 256))
        ps->p_melBanks = mfltrBank_coeff_nfilt22_fftsize256;
}

void FeatureClass_setDefault(FeatureClass *ps) {
    int i, j;
    int64_t tmp64;
    int16_t tmp;

    stftModule_setDefault(&ps->state_stftModule);
    for (i = 0; i < ps->dim_feat; i++) {
        tmp64 = (int64_t)((int32_t)LOG10_2POW_N15_Q15 - ps->pt_norm_mean[i]);
        tmp64 = (tmp64 * (int64_t)ps->pt_norm_stdR[i]) >> (30 - ps->qbit_output);
        tmp64 = MIN(MAX(tmp64, (int64_t)MIN_INT16_T), (int64_t)MAX_INT16_T);
        tmp = (int16_t)tmp64;

        for (j = 0; j < (ps->num_context - 1); j++) {
            ps->normFeatContext[i + j * ps->dim_feat] = tmp;
        }
    }
}

void FeatureClass_execute(FeatureClass *ps, int16_t *input) {
    int16_t qbit_out;
    int32_t *pspec = GLOBAL_PSPEC;
    int32_t *spec = ps->state_stftModule.spec;
    int shift = (ps->num_context - 1) * ps->dim_feat;
    int i;
    int64_t tmp;
    if (ps->num_context > 1)
    {
    #if ARM_OPTIMIZED == 3
        move_data_16b(
            ps->normFeatContext+ps->dim_feat, 
            ps->normFeatContext,
            shift);
    #else
        for (i = 0; i < shift; i++) {
            ps->normFeatContext[i] = ps->normFeatContext[i + ps->dim_feat];
        }

    #endif
    }
#if ARM_FFT == 0
    stftModule_analyze(&ps->state_stftModule, input, spec);
    #if AMBIQ_NNSP_DEBUG == 1
    for (i = 0; i < 1 + (LEN_FFT_NNSP >> 1); i++) {
        fprintf(file_spec_c, "%d %d ", spec[2 * i], spec[2 * i + 1]);
    }
    fprintf(file_spec_c, "\n");
    #endif
    spec2pspec(pspec, spec, 1 + (LEN_FFT_NNSP >> 1));
#else

    stftModule_analyze_arm(
        (void *)&ps->state_stftModule,
        input, // q15
        spec,  // q21
        ps->state_stftModule.len_fft, &qbit_out);
    spec2pspec_arm(pspec, spec, 1 + (ps->state_stftModule.len_fft >> 1), qbit_out);
#endif



#if AMBIQ_NNSP_DEBUG == 1
    for (i = 0; i < 1 + (LEN_FFT_NNSP >> 1); i++) {
        fprintf(file_pspec_c, "%d ", pspec[i]);
    }
    fprintf(file_pspec_c, "\n");
#endif
    int32_t *pt_feature;
    if (ps->num_mfltrBank ==257)
    {
        pt_feature = pspec;
    }
    else
    {
        melSpecProc(pspec, ps->feature, ps->p_melBanks, ps->num_mfltrBank);
        pt_feature = ps->feature;
    }
    #if AMBIQ_NNSP_DEBUG == 1
    for (i = 0; i < ps->num_mfltrBank; i++) {
        fprintf(file_melSpec_c, "%d ", ps->feature[i]);
    }
    fprintf(file_melSpec_c, "\n");
#endif
    log10_vec(ps->feature, pt_feature, ps->dim_feat, 15);
#if AMBIQ_NNSP_DEBUG == 1
    for (i = 0; i < ps->dim_feat; i++) {
        fprintf(file_feat_c, "%d ", ps->feature[i]);
    }
    fprintf(file_feat_c, "\n");
#endif

    for (i = 0; i < ps->dim_feat; i++) {
        tmp = (int64_t)ps->feature[i] - (int64_t)ps->pt_norm_mean[i];
        tmp = (tmp * ((int64_t)ps->pt_norm_stdR[i])) >>
              (30 - ps->qbit_output); // Bit_frac_out = 30-22 = 8
        tmp = MIN(MAX(tmp, (int64_t)MIN_INT16_T), (int64_t)MAX_INT16_T);
        ps->normFeatContext[i + shift] = (int16_t)tmp;
    }

}
