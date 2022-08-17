//*****************************************************************************
//
//! @file s2i.cc
//!
//! @brief Speech-to-Intent using NeuralSPOT
//!
//! Speech-to-Intent is a neural-network-based Tensorflow model that listens
//! for 3-5 second phrases and infers the intent of that phrase. It uses
//! NeuralSPOT to collect audio from the AUDADC, calculate MFCC, set power 
//! modes, read button state, and print to the Jlink SWO.
//! 
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2022, Ambiq Micro, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision XXXXX of the AmbiqSuite
// NeuralSPOT
//
//*****************************************************************************

//*****************************************************************************
// Tensorflow Lite for Microcontroller includes (somewhat boilerplate)
//#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
//
//*****************************************************************************


//*****************************************************************************
// NeuralSPOT Includes
#include "ns_ambiqsuite_harness.h"
#include "ns_audio.h"
#ifdef RINGBUFFER_MODE
    #include "ns_ipc_ring_buffer.h"
#endif
#include "ns_peripherals_button.h"
#include "ns_peripherals_power.h"
#ifdef AUDIODEBUG
    #include "SEGGER_RTT.h"
#endif
//
//*****************************************************************************


#include "model.h"

//*****************************************************************************
//*** Assorted Configs and helpers
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define NUM_CHANNELS 1
#define NUM_FRAMES 100
#define SAMPLES_IN_FRAME 480
#define SAMPLE_RATE 16000
//*****************************************************************************


//*****************************************************************************
//*** MFCC Config (define-based for perf)
#define MFCC_OVERRIDE_DEFAULTS 
    #define MFCC_SAMP_FREQ SAMPLE_RATE
    #define MFCC_NUM_FBANK_BINS 40
    #define MFCC_MEL_LOW_FREQ 20
    #define MFCC_MEL_HIGH_FREQ 8000
    #define MFCC_NUM_FRAMES NUM_FRAMES
    #define MFCC_NUM_MFCC_COEFFS 13
    #define MFCC_NUM_MFCC_FEATURES MFCC_NUM_MFCC_COEFFS
    #define MFCC_MFCC_DEC_BITS 4
    #define MFCC_FRAME_SHIFT_MS 30
    #define MFCC_FRAME_SHIFT ((int16_t)(MFCC_SAMP_FREQ * 0.001 * MFCC_FRAME_SHIFT_MS))
    #define MFCC_MFCC_BUFFER_SIZE (MFCC_NUM_FRAMES * MFCC_NUM_MFCC_COEFFS)
    #define MFCC_FRAME_LEN_MS 30
    #define MFCC_FRAME_LEN SAMPLES_IN_FRAME // ((int16_t)(SAMP_FREQ * 0.001 * FRAME_LEN_MS))
    #define MFCC_FRAME_LEN_POW2 512
#include "ns_audio_mfcc.h"
//*** End of MFCC config
//*****************************************************************************


//*****************************************************************************
//*** Model-specific Stuff
int recording_win = NUM_FRAMES;
int num_frames = NUM_FRAMES;
int num_mfcc_features = MFCC_NUM_MFCC_COEFFS;
int frame_shift = MFCC_FRAME_SHIFT;
int frame_len = MFCC_FRAME_LEN;
int mfcc_dec_bits = MFCC_MFCC_DEC_BITS;

const char *intents[] = {
    "decrease",        "deactivate", "increase",
    "change language", "bring",      "activate",
};

const char *slots[] = {
    "socks",  "washroom", "chinese", "juice",   "volume",    "shoes",
    "music",  "heat",     "lights",  "kitchen", "newspaper", "lamp",
    "german", "korean",   "english", "bedroom", "none",
};
//***
//*****************************************************************************


//*****************************************************************************
//*** Tensorflow Globals (somewhat boilerplate)
tflite::ErrorReporter *error_reporter = nullptr;
const tflite::Model *model = nullptr;
tflite::MicroInterpreter *interpreter = nullptr;
TfLiteTensor *input = nullptr;
TfLiteTensor *output_intent = nullptr;
TfLiteTensor *output_slot = nullptr;

constexpr int kTensorArenaSize = 1024 * 70;
alignas(16) uint8_t tensor_arena[kTensorArenaSize];
//***
//*****************************************************************************


/**
 * @brief Initialize TF with s2i model
 * 
 */
void
model_init(void) {

    static tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;

    tflite::InitializeTarget();

    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    model = tflite::GetModel(slu_model_tflite);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        TF_LITE_REPORT_ERROR(error_reporter,
                             "Model provided is schema version %d not equal "
                             "to supported version %d.",
                             model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }

    // This pulls in all the operation implementations we need.

    // static tflite::MicroMutableOpResolver<1> resolver;
    static tflite::AllOpsResolver resolver;
    // Build an interpreter to run the model with.
    static tflite::MicroInterpreter static_interpreter(
        model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
    interpreter = &static_interpreter;

    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
        return;
    }

    // Obtain pointers to the model's input and output tensors.
    input = interpreter->input(0);
    output_slot = interpreter->output(0);
    output_intent = interpreter->output(1);
}

//*****************************************************************************
//*** Button Config
int g_intButtonPressed = 0;
/**
 * @brief Button Peripheral Config Struct
 * 
 */
ns_button_config_t button_config = {
    .button_0_enable = true,
    .button_1_enable = false,
    .button_0_flag = &g_intButtonPressed,
    .button_1_flag = NULL
};
//*****************************************************************************


//*****************************************************************************
//*** Audio and IPC Config
bool static g_audioRecording = false;
bool static g_audioReady = false;

#ifdef RINGBUFFER_MODE
am_app_utils_ring_buffer_t audioBuf[1];
static uint8_t
    pui8AudioBuff[SAMPLES_IN_FRAME * 2 * 2]; // two buffers, 2 bytes/entry
#endif
int16_t static g_in16AudioDataBuffer[SAMPLES_IN_FRAME * 2];

/**
* 
* @brief Audio Callback (user-defined, executes in IRQ context)
* 
* When the 'g_audioRecording' flag is set, copy the latest sample to a buffer
* and set a 'ready' flag. If recording flag isn't set, discard buffer.
* If 'ready' flag is still set, the last buffer hasn't been consumed yet,
* print a debug message and overwrite.
* 
*/
void
audio_frame_callback(ns_audio_config_t *config, uint16_t bytesCollected) {
    uint32_t *pui32_buffer =
        (uint32_t *)am_hal_audadc_dma_get_buffer(config->audioSystemHandle);

    if (g_audioRecording) {
        if (g_audioReady) {
            ns_printf("Warning - audio buffer wasnt consumed in time\n");
        }

        // Raw PCM data is 32b (14b/channel) - here we only care about one
        // channel For ringbuffer mode, this loop may feel extraneous, but it is
        // needed because ringbuffers are treated a blocks, so there is no way
        // to convert 32b->16b
        for (int i = 0; i < config->numSamples; i++) {
            g_in16AudioDataBuffer[i] = (int16_t)(pui32_buffer[i] & 0x0000FFF0);
        }

#ifdef RINGBUFFER_MODE
        ns_ring_buffer_push(&(config->bufferHandle[0]),
                                      g_in16AudioDataBuffer,
                                      (config->numSamples * 2), // in bytes
                                      false);
#endif
        g_audioReady = true;
    }
}

/**
 * @brief NeuralSPOT Audio config struct
 * 
 * Populate this struct before calling ns_audio_config()
 * 
 */
ns_audio_config_t audio_config = {
#ifdef RINGBUFFER_MODE
    .eAudioApiMode = NS_AUDIO_API_RINGBUFFER,
    .callback = audio_frame_callback,
    .audioBuffer = (void *)&pui8AudioBuff,
#else
    .eAudioApiMode = NS_AUDIO_API_CALLBACK,
    .callback = audio_frame_callback,
    .audioBuffer = (void *)&g_in16AudioDataBuffer,
#endif
    .eAudioSource = NS_AUDIO_SOURCE_AUDADC,
    .numChannels = NUM_CHANNELS,
    .numSamples = SAMPLES_IN_FRAME,
    .sampleRate = SAMPLE_RATE,
    .audioSystemHandle = NULL, // filled in by audio_init()
#ifdef RINGBUFFER_MODE
    .bufferHandle = audioBuf
#else
    .bufferHandle = NULL
#endif
};
//***
//*****************************************************************************

/**
 * @brief Main function - infinite loop listening and inferring
 * 
 * @return int 
 */
int
main(void) {
    float tmp;
    float mfcc_buffer[NUM_FRAMES * MFCC_NUM_MFCC_COEFFS];
    float y_intent[6];
    float y_slot[2][17];
    uint8_t y_slot_max[2];
    uint8_t y_intent_max;
    float max_val = 0.0;

    g_audioRecording = false;

    ns_itm_printf_enable();

    // Configure power - different use modes
    // require different power configs
    // This examples uses pre-populated power config structs - 
    // to modify create a local struct and pass it to
    // ns_power_config()
#ifdef AUDIODEBUG
    // This mode uses RTT, which needs SRAM
    ns_debug_printf_enable();
    ns_power_config(&ns_development_default);
#else
    #ifdef ENERGYMODE
    ns_uart_printf_enable(); // use uart to print, turn off crypto
    // This is only for measuring power using an external power monitor such as
    // Joulescope - it sets GPIO pins so the state can be observed externally
    // to help line up the waveforms. It has nothing to do with AI...
    ns_init_power_monitor_state();
    ns_power_set_monitor_state(&am_ai_audio_default);
    #else
    ns_debug_printf_enable(); // Leave crypto on for ease of debugging
    ns_power_config(&ns_development_default);
    #endif
#endif

    // Initialize everything else
    model_init();
    ns_audio_init(&audio_config);
    ns_peripheral_button_init(&button_config);
    ns_mfcc_init();

    ns_printf("Press button to start listening...\n");

#ifdef ENERGYMODE
    ns_power_set_monitor_state(AM_AI_DATA_COLLECTION);
#endif

    while (1) {
        if ((g_intButtonPressed) == 1 && !g_audioRecording) {
            ns_delay_us(1000);
            g_audioRecording = true;
            ns_printf("Listening for 3 seconds.\n");

#ifdef ENERGYMODE
            ns_power_set_monitor_state(AM_AI_FEATURE_EXTRACTION);
#endif

            while (recording_win > 0) {
                ns_delay_us(1);
                if (g_audioReady) {
                    int32_t mfcc_buffer_head =
                        (num_frames - recording_win) * num_mfcc_features;

#ifdef RINGBUFFER_MODE
                    ns_ring_buffer_pop(audioBuf,
                                                 &g_in16AudioDataBuffer,
                                                 audio_config.numSamples * 2);
#endif

                    ns_mfcc_compute(g_in16AudioDataBuffer,
                                 &mfcc_buffer[mfcc_buffer_head]);

                    recording_win--;
                    g_audioReady = false;
#ifdef AUDIODEBUG
                    SEGGER_RTT_Write(1, g_in16AudioDataBuffer,
                                     SAMPLES_IN_FRAME * sizeof(int16_t));
#endif
                }
            }

            g_audioRecording = false;
            g_intButtonPressed = 0;
            recording_win = 100;

            for (uint16_t i = 0; i < MFCC_MFCC_BUFFER_SIZE; i = i + 1) {
                tmp = mfcc_buffer[i] / input->params.scale +
                      input->params.zero_point;
                tmp = MAX(MIN(tmp, 127), -128);
                input->data.int8[i] = (int8_t)tmp;
            }

#ifdef ENERGYMODE
            am_set_power_monitor_state(AM_AI_INFERING);
#endif

            TfLiteStatus invoke_status = interpreter->Invoke();
            if (invoke_status != kTfLiteOk) {
                ns_printf("Invoke failed\n");
                while (1) { // hang
                };
            }

#ifdef ENERGYMODE
            am_set_power_monitor_state(AM_AI_DATA_COLLECTION);
#endif

            ns_printf("\n");
            for (uint8_t i = 0; i < 6; i = i + 1) {
                y_intent[i] = (output_intent->data.int8[i] -
                               output_intent->params.zero_point) *
                              output_intent->params.scale;
                ns_printf("Intent[%i]: %f %s\n", i, y_intent[i], intents[i]);

                if (y_intent[i] > max_val) {
                    max_val = y_intent[i];
                    y_intent_max = i;
                }
            }

            ns_printf("**Max Intent: %s\n", intents[y_intent_max]);

            for (uint8_t i = 0; i < 2; i = i + 1) {

                max_val = 0.0;

                for (uint8_t j = 0; j < 17; j = j + 1) {
                    y_slot[i][j] = (output_slot->data.int8[i * 17 + j] -
                                    output_slot->params.zero_point) *
                                   output_slot->params.scale;
                    ns_printf("slot[%i, %i]: %f %s\n", i, j,
                                         y_slot[i][j], slots[j]);
                    if (y_slot[i][j] > max_val) {
                        max_val = y_slot[i][j];
                        y_slot_max[i] = j;
                    }
                }

                ns_printf("**Slot[%d]: %s \n", i,
                                     slots[y_slot_max[i]]);
            }
            ns_printf("\n%s %s %s\n", intents[y_intent_max],
                                 slots[y_slot_max[0]], slots[y_slot_max[1]]);
        }

    } // while(1)
}
