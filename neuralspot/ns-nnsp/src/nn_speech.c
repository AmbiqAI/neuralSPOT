#include <stdint.h>
#include "ambiq_stdint.h"
#include "s2i_const.h"
#include "ambiq_nnsp_const.h"
#include "nn_speech.h"
#include "feature_module.h"
#include "neural_nets.h"
#include "minmax.h"
#include "nnsp_identification.h"
#include "ambiq_nnsp_debug.h"
#include "nnid_class.h"
#if AMBIQ_NNSP_DEBUG == 1
    #include "debug_files.h"
#endif
#define CHECK_POWER 0

#if CHECK_POWER
    #include "timer.h"
    #include "am_hal_pwrctrl.h"
    #include "apollo4b.h"
#endif

#if DEBUG_PRINT
    #include <math.h>
    #include <stdio.h>
#endif
#include "iir.h"
#if ARM_OPTIMIZED == 3
    #include <arm_mve.h>
    #include "basic_mve.h"
#endif
int16_t glob_se_out[160];
IIR_CLASS inst_dcrm;
int16_t input_tmp[160];
int32_t embd_nnid[64 * 5] = {
    -1628, -1539, -4392, -560,  408,   -7558, 1119,  5851,  -1103, 8611,  -416,  5804,  -4506,
    -2787, -1700, 4906,  2866,  1750,  -235,  -5588, 1712,  6512,  -5315, 1418,  -3975, -4423,
    -2045, 9122,  -3814, 7408,  984,   -330,  1010,  -1376, -2152, 409,   -2607, -7301, -1874,
    -407,  2522,  4440,  -2831, -1328, 1913,  2945,  6214,  1257,  -6725, -4230, -2121, 1773,
    3555,  -1782, -1663, -1325, 3906,  3340,  2317,  -2386, -3092, 2364,  -4930, 4128};

NNID_CLASS state_nnid = {
    .pt_embd = embd_nnid,
    .dim_embd = 64,
    .is_get_corr = 0,
    .thresh_get_corr = 179,
    .thresh_trigger = 0.8,
    .corr = {0, 0, 0, 0, 0}, // TODO
};

int32_t glob_nn_output[512];
int NNSPClass_init(
    NNSPClass *pt_inst, void *pt_net, void *pt_feat, char nn_id, const int32_t *pt_mean,
    const int32_t *pt_stdR, int16_t *pt_thresh_prob, int16_t *pt_th_count_trigger,
    PARAMS_NNSP *pt_params) {
    pt_inst->pt_dcrm = &inst_dcrm;
    IIR_CLASS_init(pt_inst->pt_dcrm);
    pt_inst->pt_params = pt_params;
    pt_inst->nn_id = nn_id;

    pt_inst->pt_feat = (void *)pt_feat;

    pt_inst->pt_net = (void *)pt_net;

    FeatureClass_construct(
        (FeatureClass *)pt_inst->pt_feat, pt_mean, pt_stdR,
        ((NeuralNetClass *)pt_inst->pt_net)->qbit_input[0], pt_params->num_mfltrBank,
        pt_params->winsize_stft, pt_params->hopsize_stft, pt_params->fftsize,
        pt_params->pt_stft_win_coeff);

    pt_inst->num_dnsmpl = pt_params->num_dnsmpl;

    pt_inst->pt_thresh_prob = pt_thresh_prob;

    pt_inst->pt_th_count_trigger = pt_th_count_trigger;

    NeuralNetClass_init((NeuralNetClass *)pt_inst->pt_net);

    pt_inst->pt_se_out = glob_se_out;

    if (pt_inst->nn_id == nnid_id)
        pt_inst->pt_state_nnid = (void *)&state_nnid;
    else
        pt_inst->pt_state_nnid = (void *)0;

    return 0;
}

int NNSPClass_reset(NNSPClass *pt_inst) {
    int i;
    IIR_CLASS_reset(pt_inst->pt_dcrm);
    FeatureClass_setDefault((FeatureClass *)pt_inst->pt_feat);
    NeuralNetClass_setDefault((NeuralNetClass *)pt_inst->pt_net);
    pt_inst->slides = 1;
    pt_inst->trigger = 0;

    for (i = 0; i < DIM_INTENTS; i++)
        pt_inst->counts_category[i] = 0;

    for (i = 0; i < 3; i++)
        pt_inst->outputs[i] = 0;

    pt_inst->argmax_last = 0;
    return 0;
}
int16_t NNSPClass_get_nnOut_dim(NNSPClass *pt_inst) {
    NeuralNetClass *pt_net = (NeuralNetClass *)pt_inst->pt_net;
    return pt_net->size_layer[pt_net->numlayers];
}
int16_t NNSPClass_get_nn_out(int32_t *output, int len) {
    for (int i = 0; i < len; i++) {
        output[i] = glob_nn_output[i];
    }
    return 0;
}
int16_t NNSPClass_get_nn_out_base16b(int16_t *output, int len) {
    int16_t *pt = (int16_t *)glob_nn_output;
    for (int i = 0; i < len; i++) {
        output[i] = pt[i];
    }
    return 0;
}
int16_t NNSPClass_exec(NNSPClass *pt_inst, int16_t *rawPCM) {

    NNID_CLASS *pt_nnid = (NNID_CLASS *)pt_inst->pt_state_nnid;
    FeatureClass *pt_feat = (FeatureClass *)pt_inst->pt_feat;
    NeuralNetClass *pt_net = (NeuralNetClass *)pt_inst->pt_net;
    int8_t debug_layer = -1;
#if AMBIQ_NNSP_DEBUG == 1
    int16_t *tmp;
#endif
    int16_t *pt_inputs;
    if (pt_inst->nn_id == se_id) {

#if ARM_OPTIMIZED == 3
        int16x8_t *pt_rawPCM = (int16x8_t *)rawPCM;
        for (int i = 0; i < (160 >> 3); i++) {
            int16x8_t val = (*pt_rawPCM * pt_inst->pt_params->pre_gain_q1) >> 1;
            *pt_rawPCM = val;
            pt_rawPCM++;
        }
#else
        for (int i = 0; i < 160; i++) {
            rawPCM[i] = (rawPCM[i] * (int16_t)pt_inst->pt_params->pre_gain_q1) >> 1;
        }
#endif
        if (pt_inst->pt_params->is_dcrm) {
            IIR_CLASS_exec(pt_inst->pt_dcrm, input_tmp, rawPCM, 160);
            pt_inputs = input_tmp;
        } else {
            pt_inputs = rawPCM;
        }

        FeatureClass_execute(pt_feat, pt_inputs);
    } else {
        FeatureClass_execute(pt_feat, rawPCM);
    }

    if (pt_inst->slides == 1) {
#ifdef ENERGYMODE
        am_set_power_monitor_state(AM_AI_INFERING);
#endif
#if CHECK_POWER
        am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE);
#endif
        NeuralNetClass_exe(pt_net, pt_feat->normFeatContext, glob_nn_output, debug_layer);

        switch (pt_inst->nn_id) {
        case s2i_id:
            s2i_post_proc(pt_inst, glob_nn_output, &pt_inst->trigger);
            break;

        case kws_galaxy_id:
            binary_post_proc(pt_inst, glob_nn_output, &pt_inst->trigger);
            break;

        case vad_id:
            binary_post_proc(pt_inst, glob_nn_output, &pt_inst->trigger);
            break;

        case nnid_id:
            if (pt_nnid->is_get_corr)
                nnidClass_get_cos(
                    glob_nn_output, pt_nnid->pt_embd, pt_nnid->dim_embd, pt_nnid->total_enroll_ppls,
                    pt_nnid->corr);
            break;

        case se_id:
#if AMBIQ_NNSP_DEBUG == 1
            tmp = glob_nn_output;
            for (int i = 0; i < 257; i++) {
                fprintf(file_mask_c, "%d ", tmp[i]);
            }
            fprintf(file_mask_c, "\n");
#endif
            se_post_proc(pt_inst, (int16_t *)glob_nn_output, pt_inst->pt_se_out);

            break;
        }

#if CHECK_POWER
        am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_LOW_POWER);
#endif
    }

    if (pt_inst->num_dnsmpl == 1)
        pt_inst->slides = 1;
    else
        pt_inst->slides = (pt_inst->slides + 1) % pt_inst->num_dnsmpl;

    return pt_inst->trigger;
}

void my_argmax(int32_t *vals, int len, int16_t *pt_argmax) {
    int i;
    int32_t max_val;
    *pt_argmax = 0;
    max_val = vals[0];
    for (i = 1; i < len; i++) {
        if (vals[i] >= max_val) {
            max_val = vals[i];
            *pt_argmax = i;
        }
    }
}

void se_post_proc(NNSPClass *pt_inst, int16_t *pt_nn_est, int16_t *pt_se_out) {
    FeatureClass *pt_feat = (FeatureClass *)pt_inst->pt_feat;
    stftModule *pt_stft_state = &(pt_feat->state_stftModule);
    int32_t *spec = pt_stft_state->spec;
    int64_t tmp;
    int start_bin = pt_inst->pt_params->start_bin;
    NeuralNetClass *pt_net = (NeuralNetClass *)pt_inst->pt_net;
    int dim_out = pt_net->size_layer[pt_net->numlayers];

    for (int i = 0; i < start_bin; i++) {
        spec[2 * i] = 0;
        spec[2 * i + 1] = 0;
    }
    for (int i = start_bin; i < start_bin + dim_out; i++) { // TODO: check
        tmp = (int64_t)pt_nn_est[i] * (int64_t)spec[2 * i];
        tmp >>= 15;
        spec[2 * i] = (int32_t)tmp;

        tmp = (int64_t)pt_nn_est[i] * (int64_t)spec[2 * i + 1];
        tmp >>= 15;
        spec[2 * i + 1] = (int32_t)tmp;
    }

    for (int i = start_bin + dim_out; i < 257; i++) {
        spec[2 * i] = 0;
        spec[2 * i + 1] = 0;
    }

    stftModule_synthesize_arm(pt_stft_state, spec, pt_se_out);
}

void s2i_post_proc(NNSPClass *pt_inst, int32_t *pt_nn_est, int16_t *pt_trigger) {
    int i;
    int32_t *intent = pt_nn_est;
    int32_t *slot0 = pt_nn_est + DIM_INTENTS;
    int32_t *slot1 = slot0 + DIM_SLOTS;
    int16_t argmax_intent, argmax;

    *pt_trigger = 0;
    for (i = 0; i < 3; i++)
        pt_inst->outputs[i] = 0;

    my_argmax(intent, DIM_INTENTS, &argmax_intent);

    if ((pt_inst->argmax_last == 0) || (pt_inst->argmax_last == argmax_intent)) {
        if (argmax_intent != 0) {
            pt_inst->counts_category[argmax_intent]++;
            if (pt_inst->counts_category[argmax_intent] > *pt_inst->pt_th_count_trigger) {
                *pt_trigger = 1;
                pt_inst->outputs[0] = argmax_intent;

                my_argmax(slot0, DIM_SLOTS, &argmax);
                pt_inst->outputs[1] = argmax;

                my_argmax(slot1, DIM_SLOTS, &argmax);
                pt_inst->outputs[2] = argmax;
            }
        }
    } else {
        for (i = 0; i < DIM_INTENTS; i++)
            pt_inst->counts_category[i] = 0;
    }

    pt_inst->argmax_last = argmax_intent;
}

void binary_post_proc(NNSPClass *pt_inst, int32_t *pt_est, int16_t *pt_trigger) {
    int32_t max_val;
    int32_t i;
    static int32_t log2_e = (int32_t)0x0000B8AA;
    int32_t thresh;
    int32_t tmp;
    int32_t val;
    int32_t den;
    int64_t ref;
    int16_t pre_trigger;

    max_val = MAX(pt_est[0], pt_est[1]);

    for (i = 0; i < 2; i++) {
        val = pt_est[i] - max_val;
        ref = ((int64_t)val * (int64_t)log2_e) >> 15;
        ref = MIN(MAX(ref, MIN_INT32_T), MAX_INT32_T);
        pt_est[i] = compute_pwr2((int32_t)ref);
    }
    den = pt_est[0] + pt_est[1];

    thresh = ONE_N32_Q15 - (int)*pt_inst->pt_thresh_prob;
    ref = ((int64_t)thresh * (int64_t)den) >> 15;
    tmp = (int32_t)ref;
    pre_trigger = (pt_est[0] <= tmp) ? 1 : 0;
    if (pre_trigger)
        pt_inst->counts_category[0] += 1;
    else
        pt_inst->counts_category[0] = 0;

    *pt_trigger = (pt_inst->counts_category[0] >= *pt_inst->pt_th_count_trigger) ? 1 : 0;
}

int32_t ceiling(int32_t input) {
    int32_t output = (input >> 15);
    output <<= 15;
    output = ((output - input) == 0) ? output : output + ONE_N32_Q15;
    return output;
}

int32_t compute_pwr2(int32_t input) {
    static int16_t coeff_pwr[] = {0x5a82, 0x1fd7, 0x057a};
    int32_t t;
    int32_t output;
    int32_t input_ceil = ceiling(input);
    int32_t shift;

    input = input - input_ceil;
    shift = input_ceil >> 15;

    if (shift <= -15)
        output = 0;
    else {
        t = (input << 1) + ONE_N32_Q15;
        output = (int32_t)coeff_pwr[1] + ((t * (int32_t)coeff_pwr[2]) >> 15);
        output = (int32_t)coeff_pwr[0] + ((t * output) >> 15);
        output = (shift < 0) ? output >> -shift : output << shift;
    }
    return output;
}
