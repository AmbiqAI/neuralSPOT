#include <stdio.h>

#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_malloc.h"
#include "ns_peripherals_button.h"
#include "ns_peripherals_power.h"
#include "ns_audio.h"
#include "ns_core.h"

// Audio Configuration
// High level audio parameters
#define NUM_CHANNELS 1
#define SAMPLE_RATE 16000
#define EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW 8

//  Callback function declarations
static int get_signal_data(size_t offset, size_t length, float *out_ptr);
void audio_frame_callback(ns_audio_config_t *config, uint16_t bytesCollected);

// Audio IPC and config
int16_t static g_in16AudioDataBuffer[2][EI_CLASSIFIER_SLICE_SIZE]; // 1s
uint8_t static g_bufsel = 0; // for pingponging
uint32_t static audadcSampleBuffer[EI_CLASSIFIER_SLICE_SIZE * 2 + 3];

bool volatile static g_audioRecording = false;
bool volatile static g_audioReady = false;

ns_audio_config_t audio_config = {
    .api = &ns_audio_V1_0_0,
    .eAudioApiMode = NS_AUDIO_API_CALLBACK,
    .callback = audio_frame_callback,
    .audioBuffer = (void *)&g_in16AudioDataBuffer,
    .eAudioSource = NS_AUDIO_SOURCE_AUDADC,
    .sampleBuffer = audadcSampleBuffer,
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
    .api = &ns_button_V1_0_0,
    .button_0_enable = true,
    .button_1_enable = false,
    .button_0_flag = &g_buttonPressed,
    .button_1_flag = NULL
};

ns_timer_config_t ei_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};

typedef enum {
    WAITING_TO_RECORD,
    SAMPLING,
    INFERING
} myState_e;

// Moving average filter for Yes and No inference results
#define FILTER_DEPTH 4

typedef struct {
    float samples[FILTER_DEPTH];
    uint8_t ptr;
    float total;
} ns_moving_avg_t;

ns_moving_avg_t yesFilter = {
    .ptr = 0,
    .total = 0
};

ns_moving_avg_t noFilter = {
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
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};

    for (int i; i< FILTER_DEPTH; i++) {
        yesFilter.samples[i] = 0;
        noFilter.samples[i] = 0;
    }

    // NeuralSPOT inits
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed.\n");
    ns_itm_printf_enable();
    ns_malloc_init(); // needed by EI
	NS_TRY(ns_timer_init(&ei_tickTimer), "Timer init failed.\n");
    NS_TRY(ns_peripheral_button_init(&button_config), "Button initialization failed.\n")
    NS_TRY(ns_audio_init(&audio_config), "Audio initialization Failed.\n");
    ns_interrupt_master_enable();

    // Assign callback function to fill buffer used for preprocessing/inference
    signal.total_length = EI_CLASSIFIER_SLICE_SIZE;
    signal.get_data = &get_signal_data;
                
    ns_lp_printf("Press Button0 to start listening...\n");

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
            if (!g_audioReady) {
                // We exited deep_sleep for a non-audio reason, sample isn't ready
                // Go back to sleep...
                break;
            }

            res = run_classifier_continuous(&signal, &result, false);

            // Continuous classfication slices the audio into windows it
            // slides across the samples. For best results, we calculate
            // a moving average across the last <FILTER_DEPTH> inferences (1.25s)

            y = updateFilter(&yesFilter, result.classification[3].value);
            n = updateFilter(&noFilter, result.classification[0].value);
            if (y>0.6) ns_lp_printf("Y");       // Strong Yes
            else if (y>0.4) ns_lp_printf("y");  // Probably Yes
            if (n>0.6) ns_lp_printf("N");       // Strong No
            else if (n>0.4) ns_lp_printf("n");  // Probably No

            #ifdef EI_DEBUG_PRINTS
            // Print return code and how long it took to perform inference
            ns_lp_printf("run_classifier returned: %d\r\n", res);
            ns_lp_printf("Timing: DSP %d ms, inference %d ms, anomaly %d ms\r\n", 
                    result.timing.dsp, 
                    result.timing.classification, 
                    result.timing.anomaly);

            ns_lp_printf("Predictions:\r\n");
            for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
                ns_lp_printf("  %s: ", ei_classifier_inferencing_categories[i]);
                ns_lp_printf("%.5f\r\n", result.classification[i].value);
            }
            #endif

            g_audioReady = false;
            break;
        } // switch
        ns_deep_sleep();

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
        ns_audio_getPCM(&(g_in16AudioDataBuffer[g_bufsel][0]), pui32_buffer, config->numSamples);
        g_audioReady = true;
        g_bufsel ^=1; // pingpong
    }
}