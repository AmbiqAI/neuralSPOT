#include <stdint.h>
#include "tts.h"
#include "ref_p2f.h"
#include "ref_f2m.h"

#include "input_embed.h"
#include "pitch_embed.h"
#include "energy_embed.h"

#include "tflm_ns_model.h"

#include "phone2fuse_init.h"
#include "fuse2mel_init.h"
#include "ns_ambiqsuite_harness.h"
#define NUM_TOKENS 31
#define TIMESTEPS 300
#define DIM_FUSE 128
#define BYTES_INPUT_SIZE_F2M 76800
// static int16_t features_in[31 * DIM_FEAT_INPUT];
static ns_model_state_t model_state_f2m;
static ns_model_state_t model_state_p2f;

int tts_init()
{
    if (phone2fuse_init(&model_state_p2f) != 0) {
        ns_lp_printf("Model p2f initialization failed\\n");
        return 1;
    }

    if (fuse2mel_init(&model_state_f2m) != 0) {
        ns_lp_printf("Model f2m initialization failed\\n");
        return 1;
    }
    return 0;
}
int embedding_bin_process(
        int16_t *outputs, 
        float *inputs_float, 
        int16_t *weights,
        int num_inputs,
        int num_embeds,
        int dim_feat,
        float *bins,
        int num_bins
    ) 
{

    for (int i = 0; i < num_inputs; i++) {
        float input = inputs_float[i];
        int16_t idx = num_embeds - 1;
        for (int b = 0; b < num_bins; b++) {
            if (input < bins[b])
            {
                idx = b;
                break;
            }
        }
        for (int j = 0; j < dim_feat; j++) {
            outputs[i * dim_feat + j] = weights[idx * dim_feat + j];
        }


        for (int j = 0; j < dim_feat; j++) {
            outputs[i * dim_feat + j] = weights[idx * dim_feat + j];
        }
    }
}

int embedding_process(
        int16_t *outputs, 
        int16_t *inputs, 
        int16_t *weights,
        int num_inputs,
        int dim_feat) 
{
    ns_lp_printf("In embedding_process, num_inputs=%d, dim_feat=%d\n", num_inputs, dim_feat);
    for (int i = 0; i < num_inputs; i++) {
        int16_t idx = inputs[i];
        for (int j = 0; j < dim_feat; j++) {
            outputs[i * dim_feat + j] = weights[idx * dim_feat + j];
        }
    }
    ns_lp_printf("Exiting embedding_process\n");
    return 0;
}
int find_bin(int num_embeds, float *bins, float input)
{
    int16_t idx = num_embeds - 1;
    for (int b = 0; b < (num_embeds - 1); b++) {
        if (input < bins[b])
        {
            idx = b;
            break;
        }
    }
    return idx;
}



int tts(void) {

    int16_t inputs[31] = {92,  74, 117, 145, 110, 117,  89, 131,  83, 120, 105,  67, 117,
       132, 116,  75, 119, 130, 132, 124, 144,  98,  92,  74, 118, 103,
       147, 113,  91,  79, 106};

    // ============ Models initialization ===========

    ns_model_state_t *pt_p2f = &model_state_p2f;
    ns_model_state_t *pt_f2m = &model_state_f2m;

    // Initialize model
    float weight_scale = 1.0f / (1 << 11);
    int16_t *ptr = (int16_t*) pt_p2f->interpreter->input(0)->data.i16;

    int zero = pt_p2f->interpreter->input(0)->params.zero_point;
    float scale = pt_p2f->interpreter->input(0)->params.scale;
    
    // need to transpose embedding
    for (int i = 0; i < NUM_TOKENS; i++) {
        int idx = inputs[i];

        for (int j = 0; j < DIM_FEAT_INPUT; j++) {
            float tmp = (float) input_embed_weights[idx * DIM_FEAT_INPUT + j] * weight_scale;
            
            // transpose
            ptr[j*NUM_TOKENS + i] = (int16_t) MIN(MAX(tmp / scale, INT16_MIN), INT16_MAX) + zero;
        }
    }

#if 1
    // Run inference
    TfLiteStatus status_p2f = pt_p2f->interpreter->Invoke();
    if (status_p2f != kTfLiteOk) { ns_lp_printf("Inference failed\\n"); return 1; }

    // Get output
    ptr = (int16_t*) pt_p2f->interpreter->output(0)->data.i16;
    ns_lp_printf("duration_target:\n");
    for (int i = 0; i < 10; i++) {
        ns_lp_printf("%d, (out,ref)=%d,%d\n",
            i, ptr[i], ref_p2f_duration_target[i]);
    }

    ptr = (int16_t*) pt_p2f->interpreter->output(1)->data.i16;
    ns_lp_printf("pitch_pred:\n");
    for (int i = 0; i < 10; i++) {
        ns_lp_printf("%d, (out,ref)=%d,%d\n",
            i, ptr[i], ref_p2f_pitch_pred[i]);
    }

    ptr = (int16_t*) pt_p2f->interpreter->output(2)->data.i16;
    ns_lp_printf("duration_features:\n");
    for (int i = 0; i < 10; i++) {
        ns_lp_printf("%d, (out,ref)=%d,%d\n",
            i, ptr[i], ref_p2f_duration_features[i]);
    }

    ptr = (int16_t*) pt_p2f->interpreter->output(3)->data.i16;
    ns_lp_printf("fused_features:\n");
    for (int i = 0; i < 10; i++) {
        ns_lp_printf("%d, (out,ref)=%d,%d\n",
            i, ptr[i], ref_p2f_fused_features[i]);
    }

    ptr = (int16_t*) pt_p2f->interpreter->output(4)->data.i16;
    ns_lp_printf("energy_pred:\n");
    for (int i = 0; i < 10; i++) {
        ns_lp_printf("%d, (out,ref)=%d,%d\n",
            i, ptr[i], ref_p2f_energy_pred[i]);
    }

    // int zero_out = pt_f2m->interpreter->input(0)->params.zero_point;
    // float scale_out = pt_f2m->interpreter->input(0)->params.scale;
    
    // for (int i = 0; i < 1; i++)
    // {
    //     // find repeat
    //     int jj = 0;
    //     int idx_s = 0;
    //     ptr = i + (int16_t*) pt_p2f->interpreter->output(idx_s)->data.i16;
    //     zero = pt_p2f->interpreter->output(idx_s)->params.zero_point;
    //     scale = pt_p2f->interpreter->output(idx_s)->params.scale;
    //     float repeat_float  = (float) (*ptr-zero) * scale;
    //     int repeat = (int) round(repeat_float);
    //     // ns_lp_printf("Token %d: repeat=%d\n", i, repeat);

    //     // fused_features-id=3
    //     idx_s = 3;
    //     ptr = 32 * i + ref_p2f_fused_features;
    //     zero = pt_p2f->interpreter->output(idx_s)->params.zero_point;
    //     scale = pt_p2f->interpreter->output(idx_s)->params.scale;

    //     for (int j = 0; j < 32; j++) {
    //         float tmp  = (float) (ptr[j]-zero) * scale;
    //         int16_t quant = (int16_t) MIN(MAX(tmp / scale_out, INT16_MIN), INT16_MAX) + zero_out;
    //         ns_lp_printf("%d %d\n", quant, ref_f2m_input[jj * 300 + i]);
    //         jj++;
    //     }
        
    //     // pitch features-id=1 (embedding process)
    //     idx_s = 1;
    //     ptr = i + ref_p2f_pitch_pred;
    //     zero = pt_p2f->interpreter->output(idx_s)->params.zero_point;
    //     scale = pt_p2f->interpreter->output(idx_s)->params.scale;
    //     float tmp  = (float) (*ptr-zero) * scale;
    //     int bin = find_bin(NUM_EMBEDS_PITCH, pitch_bins, tmp);
    //     // ns_lp_printf("%d, ", bin);
    //     int16_t *pt_embed =  pitch_embed + bin * DIM_FEAT_ENERGY;
    //     for (int j = 0; j < 32; j++) {
    //         int16_t tmp_int = pt_embed[j];
    //         tmp =  (float) tmp_int * weight_scale;

    //         int16_t quant = (int16_t) MIN(MAX(tmp / scale_out, INT16_MIN), INT16_MAX) + zero_out;

    //         ns_lp_printf("%d %d\n", quant, ref_f2m_input[jj*300 + i]);
    //         jj++;
    //     }

    //     // energy features-id=4 (embedding process)
    //     idx_s = 4;
    //     ptr = i + ref_p2f_energy_pred;
    //     zero = pt_p2f->interpreter->output(idx_s)->params.zero_point;
    //     scale = pt_p2f->interpreter->output(idx_s)->params.scale;
    //     tmp  = (float) (*ptr-zero) * scale;
    //     bin = find_bin(NUM_EMBEDS_ENERGY, energy_bins, tmp);
    //     // ns_lp_printf("%d, ", bin);
    //     pt_embed =  energy_embed + bin * DIM_FEAT_ENERGY;
    //     for (int j = 0; j < 32; j++) {
    //         int16_t tmp_int = pt_embed[j];
    //         tmp =  (float) tmp_int * weight_scale;

    //         int16_t quant = (int16_t) MIN(MAX(tmp / scale_out, INT16_MIN), INT16_MAX) + zero_out;

    //        ns_lp_printf("%d %d\n", quant, ref_f2m_input[jj * 300 + i]);
    //         jj++;
    //     }

    //     // duration features-id=2
    //     idx_s = 2;
    //     ptr = 32 * i + ref_p2f_duration_features;
    //     zero = pt_p2f->interpreter->output(idx_s)->params.zero_point;
    //     scale = pt_p2f->interpreter->output(idx_s)->params.scale;

    //     for (int j = 0; j < 32; j++) {
    //         float tmp  = (float) (ptr[j]-zero) * scale;
    //         int16_t quant = (int16_t) MIN(MAX(tmp / scale_out, INT16_MIN), INT16_MAX) + zero_out;
    //         ns_lp_printf("%d %d\n", quant, ref_f2m_input[jj * 300 + i]);
    //         jj++;
    //     }

    //     // // repeat
    //     // pt_tmp = pt_f2m_input + 128;
    //     // for (int j = 0; j < (repeat - 1); j++)
    //     // {
    //     //     memcpy(pt_tmp, pt_f2m_input, 128 * sizeof(int16_t));
    //     //     pt_tmp += 128;
    //     // }
    // }



#endif
#if 1
    int16_t *pt_f2m_input = pt_f2m->interpreter->input(0)->data.i16;
    int idx_s;

    int zero_out = pt_f2m->interpreter->input(0)->params.zero_point;
    float scale_out = pt_f2m->interpreter->input(0)->params.scale;
    int repeat_last =0;
    int acc_repeat = 0;
    int16_t *pt_tmp;
    int ii = 0;

    memset(pt_f2m_input, 0, BYTES_INPUT_SIZE_F2M);
    // Transpose and quantize inputs
    for (int i = 0; i < NUM_TOKENS; i++)
    {
        // find repeat
        int jj = 0;

        // fused_features-id: 3
        idx_s = 3;
        ptr = 32 * i + (int16_t*) pt_p2f->interpreter->output(idx_s)->data.i16;
        zero = pt_p2f->interpreter->output(idx_s)->params.zero_point;
        scale = pt_p2f->interpreter->output(idx_s)->params.scale;

        for (int j = 0; j < 32; j++) {
            float tmp  = (float) (ptr[j]-zero) * scale;
            int16_t quant = (int16_t) MIN(MAX(tmp / scale_out, INT16_MIN), INT16_MAX) + zero_out;
            pt_f2m_input[jj * 300 + ii] = quant; // transpose
            jj++;
            // *pt_tmp++ = quant;
        }
        
        // pitch features-id: 1 (embedding process)
        idx_s = 1;
        ptr = i + (int16_t*) pt_p2f->interpreter->output(idx_s)->data.i16;
        zero = pt_p2f->interpreter->output(idx_s)->params.zero_point;
        scale = pt_p2f->interpreter->output(idx_s)->params.scale;
        float tmp  = (float) (*ptr-zero) * scale;
        int bin = find_bin(NUM_EMBEDS_PITCH, pitch_bins, tmp);
        int16_t *pt_embed =  pitch_embed + bin * DIM_FEAT_ENERGY;
        for (int j = 0; j < 32; j++) {
            int16_t tmp_int = pt_embed[j];
            tmp =  (float) tmp_int * weight_scale; // convert weights to float

            int16_t quant = (int16_t) MIN(MAX(tmp / scale_out, INT16_MIN), INT16_MAX) + zero_out;
            pt_f2m_input[jj * TIMESTEPS + ii] = quant; // transpose
            jj++;
            // *pt_tmp++ = quant;
        }

        // energy features-id: 4 (embedding process)
        idx_s = 4;
        ptr = i + (int16_t*) pt_p2f->interpreter->output(idx_s)->data.i16;
        zero = pt_p2f->interpreter->output(idx_s)->params.zero_point;
        scale = pt_p2f->interpreter->output(idx_s)->params.scale;
        tmp  = (float) (*ptr-zero) * scale;
        bin = find_bin(NUM_EMBEDS_ENERGY, energy_bins, tmp);
        pt_embed =  energy_embed + bin * DIM_FEAT_ENERGY;
        for (int j = 0; j < 32; j++) {
            int16_t tmp_int = pt_embed[j];
            tmp =  (float) tmp_int * weight_scale; // convert weights to float

            int16_t quant = (int16_t) MIN(MAX(tmp / scale_out, INT16_MIN), INT16_MAX) + zero_out;
            pt_f2m_input[jj * TIMESTEPS + ii] = quant; // transpose
            jj++;
            // *pt_tmp++ = quant;
        }

        // duration features-id: 2
        idx_s = 2;
        ptr = 32 * i + (int16_t*) pt_p2f->interpreter->output(idx_s)->data.i16;
        zero = pt_p2f->interpreter->output(idx_s)->params.zero_point;
        scale = pt_p2f->interpreter->output(idx_s)->params.scale;

        for (int j = 0; j < 32; j++) {
            float tmp  = (float) (ptr[j]-zero) * scale;
            int16_t quant = (int16_t) MIN(MAX(tmp / scale_out, INT16_MIN), INT16_MAX) + zero_out;
            pt_f2m_input[jj * TIMESTEPS + ii] = quant; // transpose
            jj++;
            // *pt_tmp++ = quant;
        }
        // find repeat
        idx_s = 0;
        ptr = i + (int16_t*) pt_p2f->interpreter->output(idx_s)->data.i16;
        zero = pt_p2f->interpreter->output(idx_s)->params.zero_point;
        scale = pt_p2f->interpreter->output(idx_s)->params.scale;
        float repeat_float  = (float) (*ptr-zero) * scale;
        int repeat = (int) round(repeat_float);
        
        // copy repeated frames
        for (int r = 1; r < repeat; r++) {
            // A[:,ii+r] = A[:,ii]
            for (int j = 0; j < 128; j++) {
                pt_f2m_input[j*TIMESTEPS + ii + r] = pt_f2m_input[j*TIMESTEPS + ii]; // transpose
            }
        }
        ii += repeat;
    }

    // ============ Model fuse2mel (f2m) inference ===========

    // ptr = (int16_t*) pt_f2m->interpreter->input(0)->data.i16;
    
    // memcpy(
    //     ptr, pt_f2m_input, BYTES_INPUT_SIZE_F2M
    // );
    // for (int i = 0; i < 10; i++) {
    //     ns_lp_printf("input[%d] = %d\n", i, ptr[i]);
    // }
    // Run inference
    TfLiteStatus status_f2m = pt_f2m->interpreter->Invoke();
    if (status_f2m != kTfLiteOk) { ns_lp_printf("Inference failed\\n"); return 1; }

    // Get output
    ptr = (int16_t*) pt_f2m->interpreter->output(0)->data.i16;
    ns_lp_printf("f2m output:\n");
    for (int i = 0; i < 10; i++) {
        ns_lp_printf("%d, (out,ref)=%d,%d\n",
            i, ptr[i], ref_f2m_output[i]);
    }
#endif
    return 0;
}

int unitTest_phone2fuse() {

    // ============ Model phone2fuse (p2f) inference ===========

    ns_model_state_t *pt_p2f = (ns_model_state_t *) &model_state_p2f;

    int16_t *ptr = (int16_t*) pt_p2f->interpreter->input(0)->data.i16;

    memcpy(
        ptr, ref_p2f_input, 3968 * 2
    );

    ptr = (int16_t*) pt_p2f->interpreter->input(0)->data.i16;
    for (int i = 0; i < 10; i++) {
        ns_lp_printf("input[%d] = %d\n", i, ptr[i]);
    }
    // Run inference
    TfLiteStatus status_p2f = pt_p2f->interpreter->Invoke();
    if (status_p2f != kTfLiteOk) { ns_lp_printf("Inference failed\\n"); return 1; }

    // Get output
    ptr = (int16_t*) pt_p2f->interpreter->output(0)->data.i16;
    ns_lp_printf("duration_target:\n");
    for (int i = 0; i < 10; i++) {
        ns_lp_printf("%d, (out,ref)=%d,%d\n",
            i, ptr[i], ref_p2f_duration_target[i]);
    }

    ptr = (int16_t*) pt_p2f->interpreter->output(1)->data.i16;
    ns_lp_printf("pitch_pred:\n");
    for (int i = 0; i < 10; i++) {
        ns_lp_printf("%d, (out,ref)=%d,%d\n",
            i, ptr[i], ref_p2f_pitch_pred[i]);
    }

    ptr = (int16_t*) pt_p2f->interpreter->output(2)->data.i16;
    ns_lp_printf("duration_features:\n");
    for (int i = 0; i < 10; i++) {
        ns_lp_printf("%d, (out,ref)=%d,%d\n",
            i, ptr[i], ref_p2f_duration_features[i]);
    }

    ptr = (int16_t*) pt_p2f->interpreter->output(3)->data.i16;
    ns_lp_printf("fused_features:\n");
    for (int i = 0; i < 10; i++) {
        ns_lp_printf("%d, (out,ref)=%d,%d\n",
            i, ptr[i], ref_p2f_fused_features[i]);
    }

    ptr = (int16_t*) pt_p2f->interpreter->output(4)->data.i16;
    ns_lp_printf("energy_pred:\n");
    for (int i = 0; i < 10; i++) {
        ns_lp_printf("%d, (out,ref)=%d,%d\n",
            i, ptr[i], ref_p2f_energy_pred[i]);
    }

    

    return 0;
}
int unitTest_fuse2mel() {

    // ============ Model phone2fuse (p2f) inference ===========
    // static ns_model_state_t model_state_f2m;
    // static ns_model_state_t *pt_f2m = &model_state_f2m;
    ns_model_state_t *self = (ns_model_state_t *) &model_state_f2m;


    int16_t *ptr = (int16_t*) self->interpreter->input(0)->data.i16;

    for (int i = 0; i < 38400; i++) {
        ptr[i] = ref_f2m_input[i];
    }
    // Run inference
    TfLiteStatus status_f2m = self->interpreter->Invoke();
    if (status_f2m != kTfLiteOk) { ns_lp_printf("Inference failed\\n"); return 1; }

    ns_lp_printf("Inference succeeded\n");
    // Get output
    ptr = (int16_t*) self->interpreter->output(0)->data.i16;
    ns_lp_printf("fuse2mel output:\n");
    for (int i = 0; i < 10; i++) {
        ns_lp_printf("%d, (out,ref)=%d,%d\n",
            i, ptr[i], ref_f2m_output[i]);
    }
    return 0;
}