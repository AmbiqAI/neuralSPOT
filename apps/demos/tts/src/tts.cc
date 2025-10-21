#include <arm_mve.h>

#include "stft32b.h"
#include <stdint.h>
#include <stdlib.h>
#include "tts.h"
#include "ref_p2f.h"
#include "ref_f2m.h"
#include <math.h>

#include "input_embed.h"
#include "pitch_embed.h"
#include "energy_embed.h"

#include "tflm_ns_model.h"


#include "phone2fuse_init.h"
#include "fuse2mel_init.h"
#include "ns_ambiqsuite_harness.h"
#include "mel_inv_const.h"
#include "window_const.h"


#define QUANTIZE_TO_INT16(val, scale, zero) ((int16_t)MIN(MAX((val) / (scale), INT16_MIN), INT16_MAX) + (zero))

static ns_model_state_t model_state_f2m;
static ns_model_state_t model_state_p2f;
static stftModule32b stft_mod;

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


    stftModule32b_construct(
        &stft_mod, WINDOW_SIZE, HOP_SIZE, FFT_SIZE,
        window_const);
    stftModule32b_setDefault(&stft_mod);
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



int tts(int16_t *output_buffer, int *pt_acc_frames) {

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
            ptr[j*NUM_TOKENS + i] = QUANTIZE_TO_INT16(tmp, scale, zero);
        
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


#endif
#if 1
    int16_t *pt_f2m_input = pt_f2m->interpreter->input(0)->data.i16;


    int zero_out = pt_f2m->interpreter->input(0)->params.zero_point;
    float scale_out = pt_f2m->interpreter->input(0)->params.scale;
    int repeat_last =0;
    int acc_repeat = 0;
    int16_t *pt_tmp;
    int acc_frames = 0;
    int idx_s;
    memset(pt_f2m_input, 0, BYTES_INPUT_SIZE_F2M); // set to zero
    // Concatenate, transpose and quantize inputs
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
            int16_t quant = QUANTIZE_TO_INT16(tmp, scale_out, zero_out);
            pt_f2m_input[jj * TIMESTEPS + acc_frames] = quant; // transpose
            jj++;
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

            int16_t quant = QUANTIZE_TO_INT16(tmp, scale_out, zero_out);
            pt_f2m_input[jj * TIMESTEPS + acc_frames] = quant; // transpose
            jj++;
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

            int16_t quant = QUANTIZE_TO_INT16(tmp, scale_out, zero_out);
            pt_f2m_input[jj * TIMESTEPS + acc_frames] = quant; // transpose
            jj++;
        }

        // duration features-id: 2
        idx_s = 2;
        ptr = 32 * i + (int16_t*) pt_p2f->interpreter->output(idx_s)->data.i16;
        zero = pt_p2f->interpreter->output(idx_s)->params.zero_point;
        scale = pt_p2f->interpreter->output(idx_s)->params.scale;
        for (int j = 0; j < 32; j++) {
            float tmp  = (float) (ptr[j]-zero) * scale;
            int16_t quant = QUANTIZE_TO_INT16(tmp, scale_out, zero_out);
            pt_f2m_input[jj * TIMESTEPS + acc_frames] = quant; // transpose
            jj++;
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
            // A[:,acc_frames+r] = A[:,acc_frames]
            for (int j = 0; j < 128; j++) {
                pt_f2m_input[j*TIMESTEPS + acc_frames + r] = pt_f2m_input[j*TIMESTEPS + acc_frames]; // transpose
            }
        }
        acc_frames += repeat;
    }

    // ============ Model fuse2mel (f2m) inference ===========

    TfLiteStatus status_f2m = pt_f2m->interpreter->Invoke();
    if (status_f2m != kTfLiteOk) 
    {
        ns_lp_printf("Inference failed\\n");
        return 1; 
    }

    // Get output
    ptr = (int16_t*) pt_f2m->interpreter->output(0)->data.i16;
    ns_lp_printf("f2m output:\n");
    for (int i = 0; i < 10; i++) {
        ns_lp_printf("%d, (out,ref)=%d,%d\n",
            i, ptr[i], ref_f2m_output[i]);
    }

    zero_out = pt_f2m->interpreter->output(0)->params.zero_point;
    scale_out = pt_f2m->interpreter->output(0)->params.scale;
    ns_lp_printf("Output dequant params: zero=%d, scale=%f\n", zero_out, scale_out);

    for (int i = 0; i < 80 * TIMESTEPS; i++) {
        output_buffer[i] = ptr[i];
    }
    ns_lp_printf("acc frames = %d\n", acc_frames);
    *pt_acc_frames = acc_frames;
#endif
    return 0;
}


int GRIFFIN_LIN_algorithm_frame(int32_t *wavout, int total_frames) {
    ns_model_state_t *pt_f2m = &model_state_f2m;
    int num_fft_bins = (FFT_SIZE >> 1) + 1;
    // Calculate the total number of elements in ptr

    int zero_out = pt_f2m->interpreter->output(0)->params.zero_point;
    float scale_out = pt_f2m->interpreter->output(0)->params.scale;
    // __attribute__((section(".sram_bss"))) static float phases[513*300];
    __attribute__((section(".sram_bss"))) static float magnitudes[NUM_FFT_BINS * TIMESTEPS];
    __attribute__((section(".sram_bss"))) static int32_t spec_tf[NUM_FFT_BINS * 2 * TIMESTEPS];
 
    int16_t *ptr = (int16_t*) pt_f2m->interpreter->output(0)->data.i16;
    float *pt_mag = magnitudes;
    int32_t *pt_spec = spec_tf;
    float upscale = 32768.0f; // q10 for stft compen
    ns_lp_printf("scale = %f, upscale=%f\n", scale_out, upscale);


    // Initialize magnitude and phase
    for (int i = 0; i < total_frames; i++) {
        float *pt_mel_inv = (float*) mel_inv_const;
        // int32_t *pt_spec = stft_mod.spec;
        
        for (int j = 0; j < NUM_FFT_BINS; j++) {

            float mag_spec = 0.0f;
    
            for (int k = 0; k < 80; k++) {
                float tmp = (float) ptr[k] * scale_out;
                tmp = expf(tmp);
                mag_spec += *pt_mel_inv++ * tmp;
            }
            float phase;
            if (j==0 || j==(NUM_FFT_BINS-1)) // fix phase for DC and Nyquist
                phase = 0.0f;
            else
            {
                phase = (float) rand() / (float) RAND_MAX;
                phase = phase * 2.0f * M_PI;
            }
            

            float real = mag_spec * cosf(phase);
            float imag = mag_spec * sinf(phase);

            *pt_mag++ = mag_spec;


            *pt_spec++ = (int32_t) (real * upscale); // Q(15 + 10) format
            *pt_spec++ = (int32_t) (imag * upscale); // Q(15 + 10) format
            // stft_mod.spec[2*j] = (int32_t) (real * upscale); // Q(15 + 10) format
            // stft_mod.spec[2*j+1] = (int32_t) (imag * upscale); // Q(15 + 10) format

        }
        ptr += 80;

    }

    int32_t *pt_wav = wavout;
    for (int i = 0 ; i < ITERS_GRIFFIN_LIN; i++) {
        // ifft
        pt_spec = spec_tf;
        pt_wav = wavout;
        for (int t = 0; t < total_frames; t++) {
            // copy spectrum for frame t
            memcpy(
                stft_mod.spec,
                pt_spec,
                (FFT_SIZE + 2) * sizeof(int32_t));
            pt_spec += (FFT_SIZE + 2);
            // Synthesize frame
            stftModule32b_synthesize_arm(
                &stft_mod,
                stft_mod.spec,
                pt_wav,
                15);
            pt_wav += HOP_SIZE;
        }


        // fft
        pt_spec = spec_tf;
        pt_mag = magnitudes;
        pt_wav = wavout;
        for (int i = 0; i < total_frames; i++) {
            stftModule32b_analyze_arm(
                &stft_mod,
                pt_wav,
                stft_mod.spec,
                FFT_SIZE,
                15);
            pt_wav += HOP_SIZE;
            // keep the phase
            float real;
            float imag;
            for (int j = 0; j < NUM_FFT_BINS; j++) {
                real = (float) stft_mod.spec[2 * j] / upscale;
                imag = (float) stft_mod.spec[2 * j + 1] / upscale;
                float mag = sqrtf(real * real + imag * imag);
                real = real / (mag + 1e-10f) * *pt_mag;
                imag = imag / (mag + 1e-10f) * *pt_mag;
                *pt_spec++ = (int32_t) (real * upscale); // Q(15 + 10) format
                *pt_spec++ = (int32_t) (imag * upscale); // Q(15 + 10) format
                pt_mag++;
                
            }
            

        }
    }

    // Final synthesis
    pt_wav = wavout;
    pt_spec = spec_tf;
    for (int i = 0; i < total_frames; i++){

        memcpy(
            stft_mod.spec,
            pt_spec,
            (NUM_FFT_BINS * 2) * sizeof(int32_t)
        );
        pt_spec += (NUM_FFT_BINS * 2);
        stftModule32b_synthesize_arm(
            &stft_mod,
            stft_mod.spec,
            pt_wav,
            15);
        pt_wav += HOP_SIZE;

    }
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