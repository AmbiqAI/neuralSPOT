//#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"

#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include "am_util_stdio.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "am_ai_audio.h"
#include "am_app_utils_ring_buffer.h"
#include "button.h"
#include "mfcc.h"
#include "power.h"
// #include "timer.h"
#include "slu_model_act8.h"

#ifdef AUDIODEBUG
    #include "SEGGER_RTT.h"
#endif
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define NUM_FRAMES 100
#define NUM_MFCC_COEFFS 13
#define NUM_MFCC_FEATURES NUM_MFCC_COEFFS
#define MFCC_DEC_BITS 4
#define FRAME_SHIFT_MS 30
#define FRAME_SHIFT ((int16_t)(SAMP_FREQ * 0.001 * FRAME_SHIFT_MS))
#define MFCC_BUFFER_SIZE (NUM_FRAMES * NUM_MFCC_COEFFS)
#define FRAME_LEN_MS 30
#define FRAME_LEN 480 // ((int16_t)(SAMP_FREQ * 0.001 * FRAME_LEN_MS))

int recording_win = NUM_FRAMES;
int num_frames = NUM_FRAMES;
int num_mfcc_features = NUM_MFCC_COEFFS;
int frame_shift = FRAME_SHIFT;
int frame_len = FRAME_LEN;
int mfcc_dec_bits = MFCC_DEC_BITS;

const char *intents[] = {
    "decrease",        "deactivate", "increase",
    "change language", "bring",      "activate",
};

const char *slots[] = {
    "socks",  "washroom", "chinese", "juice",   "volume",    "shoes",
    "music",  "heat",     "lights",  "kitchen", "newspaper", "lamp",
    "german", "korean",   "english", "bedroom", "none",
};

tflite::ErrorReporter *error_reporter = nullptr;
const tflite::Model *model = nullptr;
tflite::MicroInterpreter *interpreter = nullptr;
TfLiteTensor *input = nullptr;
TfLiteTensor *output_intent = nullptr;
TfLiteTensor *output_slot = nullptr;

constexpr int kTensorArenaSize = 1024 * 70;
alignas(16) uint8_t tensor_arena[kTensorArenaSize];

void
s2i_init(void) {

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

#ifdef DEBUG
    am_util_stdio_printf("input->dims->size %d\n", input->dims->size);
    am_util_stdio_printf("input->dims->data[0] %d\n", input->dims->data[0]);
    am_util_stdio_printf("input->dims->data[1] %d\n", input->dims->data[1]);
    am_util_stdio_printf("input->dims->data[2] %d\n", input->dims->data[2]);
    am_util_stdio_printf("type input->type %d\n", input->type);

    am_util_stdio_printf("\n");

    am_util_stdio_printf("output_intent->dims->size %d\n",
                         output_intent->dims->size);
    am_util_stdio_printf("output_intent->dims->data[0] %d\n",
                         output_intent->dims->data[0]);
    am_util_stdio_printf("debug: output_intent->dims->data[1] %d\n",
                         output_intent->dims->data[1]);
    am_util_stdio_printf("output_intent->type %d\n", output_intent->type);

    am_util_stdio_printf("\n");

    am_util_stdio_printf("output_slot->dims->size %d\n",
                         output_slot->dims->size);
    am_util_stdio_printf("output_slot->dims->data[0] %d\n",
                         output_slot->dims->data[0]);
    am_util_stdio_printf("output_slot->dims->data[1] %d\n",
                         output_slot->dims->data[1]);
    am_util_stdio_printf("output_slot->dims->data[2] %d\n",
                         output_slot->dims->data[2]);
    am_util_stdio_printf("output_slot->type %d\n", output_slot->type);
#endif
}

// Audio Config
#define NUM_CHANNELS 1
#define NUM_FRAMES 100
#define SAMPLES_IN_FRAME 480
#define SAMPLE_RATE 16000

// IPC Globals
bool static g_audioRecording = false;
bool static g_audioReady = false;

//#define RINGBUFFER_MODE
#ifdef RINGBUFFER_MODE
am_app_utils_ring_buffer_t audioBuf[1];
static uint8_t
    pui8AudioBuff[SAMPLES_IN_FRAME * 2 * 2]; // two buffers, 2 bytes/entry
#endif
int16_t static g_in16AudioDataBuffer[SAMPLES_IN_FRAME * 2];

//*****************************************************************************
//
// Callback (executes in IRQ context)
//
// When the 'g_audioRecording' flag is set, copy the latest sample to a buffer
// and set a 'ready' flag. If recording flag isn't set, discard buffer.
// If 'ready' flag is still set, the last buffer hasn't been consumed yet,
// print a debug message and overwrite.
//
//*****************************************************************************
void
audio_frame_callback(am_ai_audio_config_t *config, uint16_t bytesCollected) {
    uint32_t *pui32_buffer =
        (uint32_t *)am_hal_audadc_dma_get_buffer(config->audioSystemHandle);

    if (g_audioRecording) {
        if (g_audioReady) {
            am_util_stdio_printf(
                "Warning - audio buffer wasnt consumed in time\n");
        }

        // Raw PCM data is 32b (14b/channel) - here we only care about one
        // channel For ringbuffer mode, this loop may feel extraneous, but it is
        // needed because ringbuffers are treated a blocks, so there is no way
        // to convert 32b->16b
        for (int i = 0; i < config->numSamples; i++) {
            g_in16AudioDataBuffer[i] = (int16_t)(pui32_buffer[i] & 0x0000FFF0);
        }

#ifdef RINGBUFFER_MODE
        am_app_utils_ring_buffer_push(&(config->bufferHandle[0]),
                                      g_in16AudioDataBuffer,
                                      (config->numSamples * 2), // in bytes
                                      false);
#endif
        g_audioReady = true;
    }
}

am_ai_audio_config_t audio_config = {
#ifdef RINGBUFFER_MODE
    .eAudioApiMode = AM_AI_AUDIO_API_RINGBUFFER,
    .callback = audio_frame_callback,
    .audioBuffer = (void *)&pui8AudioBuff,
#else
    .eAudioApiMode = AM_AI_AUDIO_API_CALLBACK,
    .callback = audio_frame_callback,
    .audioBuffer = (void *)&g_in16AudioDataBuffer,
#endif
    .eAudioSource = AM_AI_AUDIO_SOURCE_AUDADC,
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

int
main(void) {
    g_audioRecording = false;
    float tmp;
    float mfcc_buffer[NUM_FRAMES * NUM_MFCC_COEFFS];

    am_bsp_itm_printf_enable();

    // Configure power - different use modes
    // require different power configs

#ifdef AUDIODEBUG
    // This mode uses RTT, which needs SRAM
    am_bsp_debug_printf_enable();
    am_ai_set_power_mode(&am_ai_development_default);
#else
    #ifdef ENERGYMODE
    am_bsp_uart_printf_enable();
    am_ai_set_power_mode(&am_ai_audio_default);
    #else
    am_bsp_debug_printf_enable(); // Leave crypto on for ease of debugging
    am_ai_set_power_mode(&am_ai_development_default);
    #endif
#endif

    // Initialized everything else
    s2i_init();
    audio_init(&audio_config);
    button_init();

// This is only for measuring power using an external power monitor such as
// Joulescope - it sets GPIO pins so the state can be observed externally
// to help line up the waveforms. It has nothing to do with AI...
#ifdef ENERGYMODE
    am_init_power_monitor_state();
#endif

    mfcc_init();

    am_util_stdio_printf("Press button to start listening...\n");

#ifdef ENERGYMODE
    am_set_power_monitor_state(AM_AI_DATA_COLLECTION);
#endif

    while (1) {

        if ((g_intButtonPressed) == 1 && !g_audioRecording) {

            am_hal_delay_us(1000);
            g_audioRecording = true;
            am_util_stdio_printf("Listening for 3 seconds.\n");

#ifdef ENERGYMODE
            am_set_power_monitor_state(AM_AI_FEATURE_EXTRACTION);
#endif

            while (recording_win > 0) {
                am_hal_delay_us(1);
                if (g_audioReady) {
                    int32_t mfcc_buffer_head =
                        (num_frames - recording_win) * num_mfcc_features;

#ifdef RINGBUFFER_MODE
                    am_app_utils_ring_buffer_pop(audioBuf,
                                                 &g_in16AudioDataBuffer,
                                                 audio_config.numSamples * 2);
#endif

                    mfcc_compute(g_in16AudioDataBuffer,
                                 &mfcc_buffer[mfcc_buffer_head]);

                    recording_win--;
                    g_audioReady = false;
#ifdef AUDIODEBUG
                    SEGGER_RTT_Write(1, g_in16AudioDataBuffer,
                                     AUDADC_SAMPLE_BUF_SIZE * sizeof(int16_t));
#endif
                }
            }

            g_audioRecording = false;
            g_intButtonPressed = 0;
            recording_win = 100;

#ifdef DEBUG
            am_util_stdio_printf("MFCC: \n");
            for (int i = 0; i < 100; i++) {
                am_util_stdio_printf("%f ", mfcc_buffer[i]);
                if (i % 13 == 0)
                    am_util_stdio_printf("\n ");
            }

            for (uint16_t f = 0; f < num_frames * num_mfcc_features; f++) {
                am_util_stdio_printf("mfcc_buff: %d\n", mfcc_buffer[f]);
            }
#endif

            for (uint16_t i = 0; i < MFCC_BUFFER_SIZE; i = i + 1) {
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
                TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed");
                while (1) {
                };
            }

#ifdef ENERGYMODE
            am_set_power_monitor_state(AM_AI_DATA_COLLECTION);
#endif

            float y_intent[6];
            float y_slot[2][17];

            uint8_t y_slot_max[2];
            uint8_t y_intent_max;

            float max_val = 0.0;
            am_util_stdio_printf("\n");
            for (uint8_t i = 0; i < 6; i = i + 1) {
                y_intent[i] = (output_intent->data.int8[i] -
                               output_intent->params.zero_point) *
                              output_intent->params.scale;
                am_util_stdio_printf("Intent[%i]: %f %s\n", i, y_intent[i],
                                     intents[i]);

                if (y_intent[i] > max_val) {
                    max_val = y_intent[i];
                    y_intent_max = i;
                }
            }

            am_util_stdio_printf("**Max Intent: %s \n", intents[y_intent_max]);

            for (uint8_t i = 0; i < 2; i = i + 1) {

                max_val = 0.0;

                for (uint8_t j = 0; j < 17; j = j + 1) {
                    y_slot[i][j] = (output_slot->data.int8[i * 17 + j] -
                                    output_slot->params.zero_point) *
                                   output_slot->params.scale;
                    am_util_stdio_printf("slot[%i, %i]: %f %s\n", i, j,
                                         y_slot[i][j], slots[j]);
                    if (y_slot[i][j] > max_val) {
                        max_val = y_slot[i][j];
                        y_slot_max[i] = j;
                    }
                }

                am_util_stdio_printf("**Slot[%d]: %s \n", i,
                                     slots[y_slot_max[i]]);
            }
            am_util_stdio_printf("\n%s %s %s\n", intents[y_intent_max],
                                 slots[y_slot_max[0]], slots[y_slot_max[1]]);
        }

    } // while(1)
}
