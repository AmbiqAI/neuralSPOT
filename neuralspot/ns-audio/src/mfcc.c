/*
 * MFCC utlities - based on the original C++ version by Arm
 *  this code has been refactored into C with no malloc()
 * 
 * Copyright (C) 2018 Arm Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Description: MFCC feature extraction to match with TensorFlow MFCC Op
 */

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

#include <string.h>
#include "mfcc.h"
#include "float.h"

// Globals
float g_mfccFrame[FRAME_LEN_POW2];
float g_mfccBuffer[FRAME_LEN_POW2];
float g_mfccEnergies[NUM_FBANK_BINS];
float g_mfccWindowFunction[FRAME_LEN];
int32_t g_mfccFbankFirst[NUM_FBANK_BINS];
int32_t g_mfccFbankLast[NUM_FBANK_BINS];
float g_mfccMelFBank[NUM_FBANK_BINS][50];
float g_mfccDCTMatrix[NUM_FBANK_BINS*NUM_MFCC_FEATURES];

float g_audioMax;
float g_audioMin;
float g_audioSum;
float g_audioMaxInt;
float g_audioMinInt;
float g_audioSumInt;

arm_rfft_fast_instance_f32 g_mfccRfft;

static inline float InverseMelScale(float mel_freq) {
  return 700.0f * (expf (mel_freq / 1127.0f) - 1.0f);
}

static inline float MelScale(float freq) {
  return 1127.0f * logf (1.0f + freq / 700.0f);
}

void create_mel_fbank() {
  int32_t bin, i;
  int32_t num_fft_bins = FRAME_LEN_POW2/2;
  float fft_bin_width = ((float)SAMP_FREQ) / FRAME_LEN_POW2;
  float mel_low_freq = MelScale(MEL_LOW_FREQ);
  float mel_high_freq = MelScale(MEL_HIGH_FREQ); 
  float mel_freq_delta = (mel_high_freq - mel_low_freq) / (NUM_FBANK_BINS+1);

  float this_bin[num_fft_bins];

  for (bin = 0; bin < NUM_FBANK_BINS; bin++) {

    float left_mel = mel_low_freq + bin * mel_freq_delta;
    float center_mel = mel_low_freq + (bin + 1) * mel_freq_delta;
    float right_mel = mel_low_freq + (bin + 2) * mel_freq_delta;

    int32_t first_index = -1, last_index = -1;

    for (i = 0; i < num_fft_bins; i++) {

      float freq = (fft_bin_width * i);  // center freq of this fft bin.
      float mel = MelScale(freq);
      this_bin[i] = 0.0;

      if (mel > left_mel && mel < right_mel) {
        float weight;
        if (mel <= center_mel) {
          weight = (mel - left_mel) / (center_mel - left_mel);
        } else {
          weight = (right_mel-mel) / (right_mel-center_mel);
        }
        this_bin[i] = weight;
        if (first_index == -1)
          first_index = i;
        last_index = i;
      }
    }

    g_mfccFbankFirst[bin] = first_index;
    g_mfccFbankLast[bin] = last_index;
    //am_util_stdio_printf("MFCC bin %d, first %d, last %d \n", bin, first_index, last_index);

    int32_t j = 0;
    // copy the part we care about
    for (i = first_index; i <= last_index; i++) {
      g_mfccMelFBank[bin][j++] = this_bin[i];
      if (j>=50) {
        am_util_stdio_printf("MFCC[%d] J %d exceeded 50. First %d Last %d, \n", 
          bin, j, first_index, last_index);
      }
    }
  }

}

void create_dct_matrix(int32_t input_length, int32_t coefficient_count) {
  int32_t k, n;
  float normalizer;
  arm_sqrt_f32(2.0/(float)input_length,&normalizer);
  for (k = 0; k < coefficient_count; k++) {
    for (n = 0; n < input_length; n++) {
      g_mfccDCTMatrix[k*input_length+n] = normalizer * cos( ((double)M_PI)/input_length * (n + 0.5) * k );
    }
  }
}

void mfcc_init(void) {
  int i;
  //int frame_len_padded = pow(2,ceil((log(FRAME_LEN)/log(2))));
  //am_util_stdio_printf("padded is %d\n",frame_len_padded);  

  for(i = 0; i<FRAME_LEN;i++) {
    g_mfccWindowFunction[i] = 0.5 - 0.5*cos(M_2PI * ((float)i) / (FRAME_LEN));
  }

  create_mel_fbank();
  create_dct_matrix(NUM_FBANK_BINS, NUM_MFCC_FEATURES);    

  arm_rfft_fast_init_f32(&g_mfccRfft, FRAME_LEN_POW2);
}

void mfcc_compute(const int16_t * audio_data, float* mfcc_out) {

  int32_t i, j, bin;


  //TensorFlow way of normalizing .wav data to (-1,1)
  for (i = 0; i < FRAME_LEN; i++) {
    g_mfccFrame[i] = (float)audio_data[i]/(1<<15);
    //g_mfccFrame[i] = (float)audio_data[i]/(1<<12);
    //if (i<2) am_util_stdio_printf("incoming mccframe[%d]: %f %d\n",i, g_mfccFrame[i], audio_data[i]);

    //if (audio_data[i]<g_audioMinInt) g_audioMinInt = audio_data[i];
    //if (audio_data[i]>g_audioMaxInt) g_audioMaxInt = audio_data[i];
    //g_audioSumInt+=audio_data[i];

    //if (g_mfccFrame[i]<g_audioMin) g_audioMin = g_mfccFrame[i];
    //if (g_mfccFrame[i]>g_audioMax) g_audioMax = g_mfccFrame[i];
    //g_audioSum+=g_mfccFrame[i];
  }
  //Fill up remaining with zeros
  memset(&g_mfccFrame[FRAME_LEN], 0, sizeof(float) * (FRAME_LEN_POW2-FRAME_LEN));

  for (i = 0; i < FRAME_LEN; i++) {
    //am_util_stdio_printf("g_mfccFrame[%d]: %f , audio %d win %f\n",
    // i, g_mfccFrame[i], audio_data[i], g_mfccWindowFunction[i]);  

    g_mfccFrame[i] *= g_mfccWindowFunction[i];
  }

  //Compute FFT
  arm_rfft_fast_f32(&g_mfccRfft, g_mfccFrame, g_mfccBuffer, 0);

  //Convert to power spectrum
  //frame is stored as [real0, realN/2-1, real1, im1, real2, im2, ...]
  int32_t half_dim = FRAME_LEN_POW2/2;
  float first_energy = g_mfccBuffer[0] * g_mfccBuffer[0],
        last_energy =  g_mfccBuffer[1] * g_mfccBuffer[1];  // handle this special case
  for (i = 1; i < half_dim; i++) {
    float real = g_mfccBuffer[i*2];
    float im = g_mfccBuffer[i*2 + 1];
    g_mfccBuffer[i] = real*real + im*im;
  }
  g_mfccBuffer[0] = first_energy;
  g_mfccBuffer[half_dim] = last_energy;  
 
  float sqrt_data;

  //Apply mel filterbanks
  for (bin = 0; bin < NUM_FBANK_BINS; bin++) {
    j = 0;
    float mel_energy = 0;
    int32_t first_index = g_mfccFbankFirst[bin];
    int32_t last_index = g_mfccFbankLast[bin];
    for (i = first_index; i <= last_index; i++) {
      arm_sqrt_f32(g_mfccBuffer[i],&sqrt_data);
      mel_energy += (sqrt_data) * g_mfccMelFBank[bin][j++];
    }
    g_mfccEnergies[bin] = mel_energy;

    //avoid log of zero
    if (mel_energy == 0.0)
      g_mfccEnergies[bin] = FLT_MIN;
  }

  //Take log
  for (bin = 0; bin < NUM_FBANK_BINS; bin++)
    g_mfccEnergies[bin] = logf(g_mfccEnergies[bin]);

  //Take DCT. Uses matrix mul.
  for (i = 0; i < NUM_MFCC_FEATURES; i++) {
    float sum = 0.0;
    for (j = 0; j < NUM_FBANK_BINS; j++) {
      sum += g_mfccDCTMatrix[i*NUM_FBANK_BINS+j] * g_mfccEnergies[j];
    }

    //Input is Qx.mfcc_dec_bits (from quantization step)
    //sum *= (0x1<<MFCC_DEC_BITS);
    //sum = round(sum); 
    //if(sum >= 127)
    //  mfcc_out[i] = 127;
    //else if(sum <= -128)
    //  mfcc_out[i] = -128;
    //else
    mfcc_out[i] = sum; 
  }

}
