/**
 @file har.cc

 @brief Human Activity Recognition using NeuralSPOT

 HAR is a model trained using the scripts found here:
https://github.com/AmbiqAI/Human-Activity-Recognition

**/

// #define ENERGY_MONITOR_ENABLE
// #define LOWEST_POWER_MODE

// ARM perf requires ITM to be enabled, impacting power measurements.
// For profiling measurements to work, example must be compiled using the MLPROFILE=1 make parameter
#ifdef NS_MLPROFILE
    #define MEASURE_ARM_PERF true
#else
    #define MEASURE_ARM_PERF false
#endif



/// NeuralSPOT Includes
#include "ns_ambiqsuite_harness.h"
#include "ns_core.h"
#include "ns_energy_monitor.h"
#include "ns_perf_profile.h"
#include "ns_peripherals_power.h"
#include "arm_nnfunctions.h"   // CMSIS-NN functions
#include "arm_math.h"          // CMSIS-DSP/MVE intrinsics if needed
#include "ds_cnn_data.h"       // Declarations for weights, biases, and quant params
#include "kws_model_settings.h"
#include "ns_audio.h"
#include "ns_peripherals_button.h"
#include "ns_timer.h"
#include "ns_audio_mfcc.h"

#define NUM_CHANNELS 1
#define NUM_FRAMES 49 // 20ms frame shift
#define SAMPLES_IN_FRAME 320
#define SAMPLE_RATE 16000/// Button Peripheral Config

int volatile buttonPressed = 0;
ns_button_config_t button_config = {
    .api = &ns_button_V1_0_0,
    .button_0_enable = true,
    .button_1_enable = false,
    .button_0_flag = &buttonPressed,
    .button_1_flag = NULL};

#ifdef __cplusplus
    extern "C" {
#endif
// extern void ds_cnn_infer(const int8_t *input, int8_t *output);
extern arm_cmsis_nn_status ds_cnn_inference(const int8_t *input, int8_t *output);
#ifdef __cplusplus
    }
#endif

typedef enum { WAITING_TO_START_RPC_SERVER, WAITING_TO_RECORD, WAIT_FOR_FRAME, INFERING } myState_e;

/// Audio Config
bool volatile static audioRecording = false;
bool volatile static audioReady = false;
alignas(16) int16_t static audioDataBuffer[SAMPLES_IN_FRAME << 1];
alignas(16) uint32_t static audadcSampleBuffer[(SAMPLES_IN_FRAME << 1) + 3];
#ifdef USE_AUDADC
alignas(16) am_hal_audadc_sample_t static workingBuffer[SAMPLES_IN_FRAME * NUM_CHANNELS]; // working buffer
                                                                              // used by AUDADC
#endif
#if !defined(NS_AMBIQSUITE_VERSION_R4_1_0) && defined(NS_AUDADC_PRESENT)
am_hal_offset_cal_coeffs_array_t sOffsetCalib;
#endif

void audio_frame_callback(ns_audio_config_t *config, uint16_t bytesCollected) {
    if (audioRecording) {
        ns_audio_getPCM_v2(config, audioDataBuffer);
        audioReady = true;
    }
}

ns_audio_config_t audio_config = {
    .api = &ns_audio_V2_0_0,
    .eAudioApiMode = NS_AUDIO_API_CALLBACK,
    .callback = audio_frame_callback,
    .audioBuffer = (void *)&audioDataBuffer,
#ifdef USE_AUDADC
    .eAudioSource = NS_AUDIO_SOURCE_AUDADC,
#else
    .eAudioSource = NS_AUDIO_SOURCE_PDM,
#endif
    .sampleBuffer = audadcSampleBuffer,
#ifdef USE_AUDADC
    .workingBuffer = workingBuffer,
#else
    .workingBuffer = NULL,
#endif
    .numChannels = NUM_CHANNELS,
    .numSamples = SAMPLES_IN_FRAME,
    .sampleRate = SAMPLE_RATE,
    .audioSystemHandle = NULL, // filled in by init
    .bufferHandle = NULL,      // only for ringbuffer mode
#if !defined(NS_AMBIQSUITE_VERSION_R4_1_0) && defined(NS_AUDADC_PRESENT)
    .sOffsetCalib = &sOffsetCalib,
#endif
};
static int recording_win = NUM_FRAMES;


// MFCC Config
#define MY_MFCC_FRAME_LEN_POW2 512 // Next power of two size after SAMPLES_IN_FRAME
#define MY_MFCC_NUM_FBANK_BINS 40
#define MY_MFCC_NUM_MFCC_COEFFS 10

// Allocate memory for MFCC calculations
#define MFCC_ARENA_SIZE                                                                            \
    32 * (MY_MFCC_FRAME_LEN_POW2 * 2 +                                                             \
          MY_MFCC_NUM_FBANK_BINS * (NS_MFCC_SIZEBINS + MY_MFCC_NUM_MFCC_COEFFS))
static uint8_t mfccArena[MFCC_ARENA_SIZE];

ns_mfcc_cfg_t mfcc_config = {.api = &ns_mfcc_V1_0_0,
                             .arena = mfccArena,
                             .sample_frequency = SAMPLE_RATE,
                             .num_fbank_bins = MY_MFCC_NUM_FBANK_BINS,
                             .low_freq = 20,
                             .high_freq = 4000,
                             .num_frames = NUM_FRAMES,
                             .num_coeffs = MY_MFCC_NUM_MFCC_COEFFS,
                             .num_dec_bits = 0,
                             .frame_shift_ms = 20,
                             .frame_len_ms = 30,
                             .frame_len = SAMPLES_IN_FRAME,
                             .frame_len_pow2 = MY_MFCC_FRAME_LEN_POW2};


static uint32_t elapsedTime = 0;
ns_timer_config_t tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};
 
void tic() {
    uint32_t oldTime = elapsedTime;
    elapsedTime = ns_us_ticker_read(&tickTimer);
    if (elapsedTime == oldTime) {
        // We've saturated the timer, reset it
        ns_timer_clear(&tickTimer);
    }
}
 
uint32_t toc() { return ns_us_ticker_read(&tickTimer) - elapsedTime; }

/**
 * @brief Main HAR - infinite loop listening and inferring
 *
 * @return int
 */
int main(void) {
    float tmp = 0;
    float mfcc_buffer[NUM_FRAMES * MY_MFCC_NUM_MFCC_COEFFS];
    int8_t input[NUM_FRAMES * MY_MFCC_NUM_MFCC_COEFFS];
    int8_t output[kCategoryCount];
    uint8_t output_max = 0;
    float max_val = 0.0;
    bool measure_first_inference = MEASURE_ARM_PERF;
    myState_e state = WAITING_TO_RECORD;
    arm_cmsis_nn_status status;

    ns_perf_counters_t pp;
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    // myState_e state = WAITING_TO_RECORD;
    NS_TRY(ns_timer_init(&tickTimer), "Timer Init Failed\n");
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed.\n");
    NS_TRY(ns_set_performance_mode(NS_MINIMUM_PERF), "Set CPU Perf mode failed.");
    NS_TRY(ns_audio_init(&audio_config), "Audio initialization Failed.\n");
    NS_TRY(ns_audio_set_gain(AM_HAL_PDM_GAIN_P345DB, AM_HAL_PDM_GAIN_P345DB), "Gain set failed.\n"); // PDM gain
    NS_TRY(ns_peripheral_button_init(&button_config), "Button initialization failed.\n")
    NS_TRY(ns_mfcc_init(&mfcc_config), "MFCC config failed.\n");

#ifdef DYNAMIC_AUDIO_SOURCE
    NS_TRY(ns_start_audio(&audio_config), "Audio start failed.\n");
#endif

    ns_itm_printf_enable();
    ns_interrupt_master_enable();

    // int8_t output[FC_OUT_FEATURES];
    memset(output, 0, sizeof(output));
    
    // ns_lp_printf("Starting KWS\n");
    // tic();
    // for (int i = 0; i < 100; i++) {
    //     ds_cnn_inference(test_input, output);
    // }
    // ns_lp_printf("First inference time: %d us\n", toc());

    ns_lp_printf("This KWS example listens for 1 second, then classifies\n");
    ns_lp_printf("the captured audio into one of the following phrases:\n");
    ns_lp_printf("yes, no, up, down, left, right, on, off, or unknown/silence\n\n");
    ns_lp_printf("Press Button 0 to start listening...\n");

    // Event loop
    while (1) {
        switch (state) {

        case WAITING_TO_RECORD:
            if (buttonPressed) {
                state = WAIT_FOR_FRAME;
                buttonPressed = false;
                ns_delay_us(250000); // wait for button click noise to die down
                audioRecording = true;
                ns_lp_printf("\nListening for 1 second.\n");
            }
            break;

        case WAIT_FOR_FRAME:
            if (audioReady) {

                int32_t mfcc_buffer_head = (NUM_FRAMES - recording_win) * MY_MFCC_NUM_MFCC_COEFFS;
                ns_mfcc_compute(&mfcc_config, audioDataBuffer, &mfcc_buffer[mfcc_buffer_head]);
                recording_win--;
                audioReady = false;

                ns_lp_printf(".");
            }

            if (recording_win == 0) {
                ns_lp_printf("\n");
                audioRecording = false;
                recording_win = NUM_FRAMES;
                state = INFERING; // have full sample
            } else {
                ns_set_power_monitor_state(NS_DATA_COLLECTION);
            }

            break;

        case INFERING:
            for (uint16_t i = 0; i < (NUM_FRAMES * MY_MFCC_NUM_MFCC_COEFFS); i = i + 1) {
                tmp = mfcc_buffer[i] / input_scale + input_zero_point;
                tmp = MAX(MIN(tmp, 127), -128);
                // input[i] = (int8_t)tmp;
                input[i] = 80;
            }

            // TfLiteStatus invoke_status = interpreter->Invoke();
            status = ds_cnn_inference(input, output);
            if (status != ARM_CMSIS_NN_SUCCESS) {
                ns_lp_printf("Inference failed with status %d\n", status);
                while (1) {
                }; // hang
            }

            // if (invoke_status != kTfLiteOk) {
            //     ns_lp_printf("Invoke failed\n");
            //     while (1) {
            //     }; // hang
            // }

            max_val = 0.0;
            for (uint8_t i = 0; i < kCategoryCount; i = i + 1) {
                ns_lp_printf("Output[%d] = %d\n", i, output[i]);
                // output[i] = (model_output->data.int8[i] - model_output->params.zero_point) *
                //             model_output->params.scale;
                // if (output[i] > 0.3) {

                if (output[i] > max_val) {
                    max_val = output[i];
                    output_max = i;
                }
            }

            ns_lp_printf("\n**** Most probably: [%s]\n\n", kCategoryLabels[output_max]);
            ns_lp_printf("Press Button 0 to start listening...\n");
            buttonPressed = false; // thoroughly debounce the button
            state = WAITING_TO_RECORD;
            break;
        }
        // ns_deep_sleep();
    } // while(1)
}

//     ns_lp_printf("Inference done\n");
//     // Print out output
//     for (int i = 0; i < FC_OUT_FEATURES; i++) {
//         ns_lp_printf("Output[%d] = %d\n", i, output[i]);
//     }

// }
