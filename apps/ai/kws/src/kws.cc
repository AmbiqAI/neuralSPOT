/**
 * @file kws.cc
 * @author Ambiq
 * @brief Basic Keyword Spotting (KWS) using NeuralSPOT and MLCommons KWS model
 * @version 0.1
 * @date 2025-06-23
 * 
 * @copyright Copyright (c) 2025 
 * 
 */

// Comment this out to use AUDADC microphone (only supported on AP4 EVBs)
#define USE_PDM_MICROPHONE

////////////////////////////////////////////////
// Tensorflow Lite for Microcontroller headers
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_profiler.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/micro/tflite_bridge/micro_error_reporter.h"

////////////////////////////////////////////////
// TFLM model
#include "kws_model_analysis.h"
#include "kws_model_data.h"
#include "kws_model_settings.h"

////////////////////////////////////////////////
// NeuralSPOT Headers
#include "ns_core.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_audio.h"
#include "ns_energy_monitor.h"
#include "ns_peripherals_power.h"
#include "ns_audio_mfcc.h"
#include "ns_debug_log.h"
#include "ns_peripherals_button.h"
#include "ns_timer.h"

////////////////////////////////////////////////
// Audio and MFCC Parameters
#define NUM_CHANNELS 1
#define NUM_FRAMES 49 // 20ms frame shift
#define SAMPLES_IN_FRAME 320
#define SAMPLE_RATE 16000

// MFCC 
#define MY_MFCC_FRAME_LEN_POW2 512 // Next power of two size after SAMPLES_IN_FRAME
#define MY_MFCC_NUM_FBANK_BINS 40
#define MY_MFCC_NUM_MFCC_COEFFS 10

////////////////////////////////////////////////
// Allocate memory for MFCC calculations
#define MFCC_ARENA_SIZE                                                                            \
    32 * (MY_MFCC_FRAME_LEN_POW2 * 2 +                                                             \
          MY_MFCC_NUM_FBANK_BINS * (NS_MFCC_SIZEBINS + MY_MFCC_NUM_MFCC_COEFFS))
static uint8_t mfccArena[MFCC_ARENA_SIZE];
static ns_mfcc_cfg_t mfcc_config = {.api = &ns_mfcc_V1_0_0,
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

////////////////////////////////////////////////
// Tensorflow Globals (somewhat boilerplate)
static tflite::ErrorReporter *error_reporter = nullptr;
static const tflite::Model *model = nullptr;
static tflite::MicroInterpreter *interpreter = nullptr;
static TfLiteTensor *model_input = nullptr;
static TfLiteTensor *model_output = nullptr;
static tflite::MicroProfiler *profiler = nullptr;

static constexpr int kTensorArenaSize = 1024 * 29;
alignas(16) static uint8_t tensor_arena[kTensorArenaSize];

////////////////////////////////////////////////
// EVB Peripherals (Audio, Timer, Button, etc.)

////////////////////////////////////////////////
// Timer is used for TF profiling
static ns_timer_config_t basic_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};


////////////////////////////////////////////////
// Audio System & Application State
// Set by app when it wants to start recording, used by callback
bool volatile static audioRecording = false;

// Set by callback when audio buffer has been copied, cleared by
// app when the buffer has been consumed.
bool volatile static audioReady = false;

// Audio buffers
#if NUM_CHANNELS == 1
alignas(16) int16_t static audioDataBuffer[SAMPLES_IN_FRAME]; // incoming PCM audio data
#else
alignas(16) int32_t static audioDataBuffer[SAMPLES_IN_FRAME];
#endif

alignas(16) uint32_t static dmaBuffer[SAMPLES_IN_FRAME * NUM_CHANNELS * 2];   // DMA target
    
#ifndef USE_PDM_MICROPHONE
alignas(16) am_hal_audadc_sample_t static workingBuffer[SAMPLES_IN_FRAME * NUM_CHANNELS]; // working buffer used  // by AUDADC                                                                          
#endif // USE_PDM_MICROPHONE

#if !defined(NS_AMBIQSUITE_VERSION_R4_1_0) && defined(NS_AUDADC_PRESENT)
am_hal_offset_cal_coeffs_array_t sOffsetCalib;
#endif

/**
 *
 * @brief Audio Callback (executes in IRQ context)
 *
 * When the 'audioRecording' flag is set, copy the latest sample to a buffer
 * and set a 'ready' flag. If recording flag isn't set, discard buffer.
 * If 'ready' flag is still set, the last buffer hasn't been consumed yet,
 * print a debug message and overwrite.
 *
 */
static void audio_frame_callback(ns_audio_config_t *config, uint16_t bytesCollected) {
   if (audioRecording) {
        ns_audio_getPCM_v2(config, audioDataBuffer);
        audioReady = true;
    }
}

/**
 * @brief NeuralSPOT Audio config struct
 *
 * Populate this struct before calling ns_audio_config()
 *
 */
static ns_audio_config_t audio_config = {

    .api = &ns_audio_V2_1_0,
    .eAudioApiMode = NS_AUDIO_API_CALLBACK,
    .callback = audio_frame_callback,
    .audioBuffer = (void *)&audioDataBuffer,
    #ifdef USE_PDM_MICROPHONE
        .eAudioSource = NS_AUDIO_SOURCE_PDM,
    #else
        .eAudioSource = NS_AUDIO_SOURCE_AUDADC,
    #endif
    .sampleBuffer = dmaBuffer,
    #if !defined(USE_PDM_MICROPHONE)
        .workingBuffer = workingBuffer, // only used for AUDADC microphone
    #endif
    .numChannels = NUM_CHANNELS,
    .numSamples = SAMPLES_IN_FRAME,
    .sampleRate = SAMPLE_RATE,
    .audioSystemHandle = NULL, // filled in by audio_init()
    .bufferHandle = NULL,
    #if !defined(NS_AMBIQSUITE_VERSION_R4_1_0) && defined(NS_AUDADC_PRESENT)
        .sOffsetCalib = &sOffsetCalib,
    #endif
};

////////////////////////////////////////////////
// Button config and application state
// buttonPressed - will be set by neuralSPOT button helper
static int volatile buttonPressed = 0;

// Button Peripheral Config Struct
ns_button_config_t button_config = {.api = &ns_button_V1_0_0,
                                    .button_0_enable = true,
                                    .button_1_enable = false,
                                    .joulescope_trigger_enable = false,
                                    .button_0_flag = &buttonPressed,
                                    .button_1_flag = NULL,
                                    .joulescope_trigger_flag = NULL};

////////////////////////////////////////////////
//*** KWS Application State
static int recording_win = NUM_FRAMES;
typedef enum { WAITING_TO_RECORD, WAIT_FOR_FRAME, INFERING } myState_e;
static void model_init(void);

/**
 * @brief Main basic_tf_stub - infinite loop listening and inferring
 *
 * @return int
 */
int main(void) {
    float tmp = 0;
    float mfcc_buffer[NUM_FRAMES * MY_MFCC_NUM_MFCC_COEFFS];
    float output[kCategoryCount];
    uint8_t output_max = 0;
    float max_val = 0.0;
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};

    myState_e state = WAITING_TO_RECORD;

    // Tells callback if it should be recording audio
    audioRecording = false;

    // Pile of inits
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed.\n");
    ns_itm_printf_enable();
    ns_interrupt_master_enable();
    NS_TRY(ns_timer_init(&basic_tickTimer), "Timer init failed.\n");
    NS_TRY(ns_audio_init(&audio_config), "Audio initialization Failed.\n");
    NS_TRY(ns_audio_set_gain(AM_HAL_PDM_GAIN_P345DB, AM_HAL_PDM_GAIN_P345DB), "Gain set failed.\n"); // PDM gain
    NS_TRY(ns_start_audio(&audio_config), "Audio start failed.\n");
    NS_TRY(ns_mfcc_init(&mfcc_config), "MFCC config failed.\n");
    NS_TRY(ns_peripheral_button_init(&button_config), "Button initialization failed.\n")

    model_init();

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
                audioRecording = true; // Global to tell callback to start recording
                ns_lp_printf("\nListening for 1 second.\n");
            }
            break;

        case WAIT_FOR_FRAME:
            if (audioReady) { // Set by audio callback when there is a new frame ready
                int32_t mfcc_buffer_head = (NUM_FRAMES - recording_win) * MY_MFCC_NUM_MFCC_COEFFS;
                // audioDataBuffer contains one frame of audio data
                ns_mfcc_compute(&mfcc_config, audioDataBuffer, &mfcc_buffer[mfcc_buffer_head]);
                recording_win--; // number of frames left to record before it is ready for inference
                audioReady = false; // pause callback from recording more audio 
                ns_lp_printf(".");
            }

            if (recording_win == 0) { // We have enough frames to run inference
                ns_lp_printf("\n");
                audioRecording = false;
                recording_win = NUM_FRAMES;
                state = INFERING; // have full sample
            } else {
                ns_set_power_monitor_state(NS_DATA_COLLECTION);
            }

            break;

        case INFERING:
            // Prepare the model input
            for (uint16_t i = 0; i < (NUM_FRAMES * MY_MFCC_NUM_MFCC_COEFFS); i = i + 1) {
                tmp = mfcc_buffer[i] / model_input->params.scale + model_input->params.zero_point;
                tmp = MAX(MIN(tmp, 127), -128);
                model_input->data.int8[i] = (int8_t)tmp;
            }

            // Call the model
            TfLiteStatus invoke_status = interpreter->Invoke();

            if (invoke_status != kTfLiteOk) {
                ns_lp_printf("Invoke failed\n");
                while (1) {
                }; // hang
            }

            // Decode the output
            max_val = 0.0;
            for (uint8_t i = 0; i < kCategoryCount; i = i + 1) {
                output[i] = (model_output->data.int8[i] - model_output->params.zero_point) *
                            model_output->params.scale;
                if (output[i] > 0.3) {
                    ns_lp_printf(
                        "\n[%s] with %d%% certainty\n", kCategoryLabels[i],
                        (uint8_t)(output[i] * 100));
                }

                if (output[i] > max_val) {
                    max_val = output[i];
                    output_max = i;
                }
            }

            ns_lp_printf("\n**** Most probably: [%s]\n\n", kCategoryLabels[output_max]);
            ns_lp_printf("Press Button 0 to start listening...\n");
            buttonPressed = false; // thoroughly debounce the button
            state = WAITING_TO_RECORD; // wait for button press to start recording again
            break;
        }
        // ns_deep_sleep();
    } // while(1)
}


/**
 * @brief Initialize TF with KWS model
 *
 * This code is fairly common across most TF-based models.
 * The major differences relate to input and output tensor
 * handling.
 *
 */
static void
model_init(void) {

    tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;

    tflite::InitializeTarget();

    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    model = tflite::GetModel(g_kws_model_data);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        TF_LITE_REPORT_ERROR(error_reporter,
                             "Model provided is schema version %d not equal "
                             "to supported version %d.",
                             model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }

    // Build mutable resolver with minimum opset
    static tflite::MicroMutableOpResolver<6> resolver;
    resolver.AddFullyConnected();
    resolver.AddConv2D();
    resolver.AddDepthwiseConv2D();
    resolver.AddReshape();
    resolver.AddSoftmax();
    resolver.AddAveragePool2D();

    // Build an interpreter to run the model with.
    static tflite::MicroInterpreter static_interpreter(model, resolver, tensor_arena,
                                                       kTensorArenaSize, nullptr, profiler);

    interpreter = &static_interpreter;

    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed\n");
        ns_lp_printf("Arena size computed: %d\n", interpreter->arena_used_bytes());
        ns_lp_printf("AllocateTensors() failed with status %d\n", allocate_status);
        return;
    }

    ns_lp_printf("Arena size computed: %d\n", interpreter->arena_used_bytes());

    // Obtain pointers to the model's input and output tensors.
    model_input = interpreter->input(0);
    model_output = interpreter->output(0);
}
