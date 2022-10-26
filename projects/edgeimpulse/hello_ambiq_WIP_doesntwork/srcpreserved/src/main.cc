#include <stdio.h>

#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_malloc.h"
#include "ns_peripherals_button.h"
#include "ns_peripherals_power.h"
#include "ns_audio.h"

// Audio Configuration
// High level audio parameters
#define NUM_CHANNELS 1
#define SAMPLE_RATE 16000
#define EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW 8
#define EI_DEBUG_PRINTS

//  Callback function declarations
static int get_signal_data(size_t offset, size_t length, float *out_ptr);
void audio_frame_callback(ns_audio_config_t *config, uint16_t bytesCollected);

// Audio IPC and config
int16_t static g_in16AudioDataBuffer[2][EI_CLASSIFIER_SLICE_SIZE]; // 1s
uint8_t static g_bufsel = 0; // for pingponging
bool volatile static g_audioRecording = false;
bool volatile static g_audioReady = false;

ns_audio_config_t audio_config = {
    .eAudioApiMode = NS_AUDIO_API_CALLBACK,
    .callback = audio_frame_callback,
    .audioBuffer = (void *)&g_in16AudioDataBuffer,
    .eAudioSource = NS_AUDIO_SOURCE_AUDADC,
    .numChannels = NUM_CHANNELS,
    .numSamples = EI_CLASSIFIER_SLICE_SIZE,
    .sampleRate = SAMPLE_RATE,
    .audioSystemHandle = NULL, // filled in by audio_init()
    .bufferHandle = NULL
};

// Button config
// Button global - will be set by neuralSPOT button helper
int volatile g_buttonPressed = 0;

// Button Peripheral Config Struct
ns_button_config_t button_config = {
    .button_0_enable = true,
    .button_1_enable = false,
    .button_0_flag = &g_buttonPressed,
    .button_1_flag = NULL
};

typedef enum {
    WAITING_TO_RECORD,
    RECORDING,
    INFERING
} myState_e;

// Moving average filter for Yes and No inference results
#define FILTER_DEPTH 10

typedef struct {
    float samples[FILTER_DEPTH];
    uint8_t ptr;
    float total;
} ns_moving_avg_t;

ns_moving_avg_t yesFilter = {
    .samples = {0,0,0,0,0,0,0,0,0,0},
    .ptr = 0,
    .total = 0
};

ns_moving_avg_t noFilter = {
    .samples = {0,0,0,0,0,0,0,0,0,0},
    .ptr = 0,
    .total = 0
};

/**
 * @brief Calculate average over last FILTER_DEPTH results
 * 
 * @param filter - filter state
 * @param newValue - new result
 * @return float - new average
 */
float updateFilter(ns_moving_avg_t *filter, float newValue) {
    // ns_lp_printf(".... Add %f to %f, remove %f,  ptr was %d\n", newValue, filter->total, filter->samples[filter->ptr], filter->ptr);
    filter->total = filter->total - filter->samples[filter->ptr] + newValue;
    filter->samples[filter->ptr] = newValue;
    if (filter->ptr == FILTER_DEPTH-1) {
        filter->ptr = 0;
    } 
    else {
        filter->ptr++;
    }
    return filter->total/FILTER_DEPTH;
}

int main(int argc, char **argv) {
    
    signal_t signal;            // Wrapper for raw input buffer
    ei_impulse_result_t result; // Used to store inference output
    EI_IMPULSE_ERROR res;       // Return code from inference
    myState_e state = WAITING_TO_RECORD;

    // NeuralSPOT inits
    am_hal_interrupt_master_enable();
    ns_power_config(&ns_development_default);
    ns_malloc_init(); // needed by EI
    ns_peripheral_button_init(&button_config);
    ns_audio_init(&audio_config);

    // ns_itm_printf_enable();
    ns_debug_printf_enable();

    // Assign callback function to fill buffer used for preprocessing/inference
    signal.total_length = EI_CLASSIFIER_SLICE_SIZE;
    signal.get_data = &get_signal_data;
                
    ns_lp_printf("Press Button0 to start listening for 1s\n");

    float y, n;
    while(1) {

        switch (state) {
        case WAITING_TO_RECORD:
            if (g_buttonPressed) {
                g_buttonPressed = false;
                ns_lp_printf("Listening...\n");
                g_audioReady = false;
                g_audioRecording = true;
                state = INFERING;
            }
            break;

        case INFERING:
            res = run_classifier_continuous(&signal, &result, false);

            // Continuous classfication slices the audio into windows it
            // slides across the samples. For best results, we calculate
            // a moving average across the last 10 inferences (1.125s)

            // y = updateFilter(&yesFilter, result.classification[3].value);
            // n = updateFilter(&noFilter, result.classification[0].value);
            // if (y>0.6) ns_lp_printf("Y");       // Strong Yes
            // else if (y>0.4) ns_lp_printf("y");  // Probably Yes
            // if (n>0.6) ns_lp_printf("N");       // Strong No
            // else if (n>0.4) ns_lp_printf("n");  // Probably No

            #ifdef EI_DEBUG_PRINTS
            // Print return code and how long it took to perform inference
            ns_lp_printf("run_classifier returned: %d\r\n", res);
            ns_lp_printf("Timing: DSP %d ms, inference %d ms, anomaly %d ms\r\n", 
                    result.timing.dsp, 
                    result.timing.classification, 
                    result.timing.anomaly);


            ns_printf("Predictions:\r\n");
            for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
                ns_lp_printf("  %s: ", ei_classifier_inferencing_categories[i]);
                ns_lp_printf("%.5f\r\n", result.classification[i].value);
            }
            #endif

            g_audioReady = false;
            break;
        } // switch
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);

    } // while
    return 0;
}

// Callback: fill a section of the out_ptr buffer when requested
static int get_signal_data(size_t offset, size_t length, float *out_ptr) {
    numpy::int16_to_float(&g_in16AudioDataBuffer[g_bufsel^1][offset], out_ptr, length);

    return EIDSP_OK;
}

void audio_frame_callback(ns_audio_config_t *config, uint16_t bytesCollected) {
    uint32_t *pui32_buffer =
        (uint32_t *)am_hal_audadc_dma_get_buffer(config->audioSystemHandle);

    if (g_audioRecording) {
        for (int i = 0; i < config->numSamples; i++) {
            g_in16AudioDataBuffer[g_bufsel][i] = (int16_t)(pui32_buffer[i] & 0x0000FFF0);
        }
        g_audioReady = true;
        g_bufsel ^=1; // pingpong
    }
}