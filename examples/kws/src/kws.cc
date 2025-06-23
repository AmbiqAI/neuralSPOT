/**
 @file main.cc

 @brief Speech-to-Intent using NeuralSPOT

The basic_tf_stub example is based on a speech to intent model.
 Speech-to-Intent is a neural-network-based Tensorflow model that listens
 for 3-5 second phrases and infers the intent of that phrase. It uses
 NeuralSPOT to collect audio from the AUDADC, calculate MFCC, set power
 modes, read button state, and print to the Jlink SWO.

**/

// #ifndef NS_AUDADC_PRESENT 
    #define USE_PDM_MICROPHONE
// #endif


#include "basic_tf_stub.h"
#include "basic_audio.h"
#include "basic_mfcc.h"
#include "basic_model.h"
#include "basic_peripherals.h"
#include "ns_core.h"

/// NeuralSPOT Includes
#include "ns_ambiqsuite_harness.h"
#include "ns_energy_monitor.h"
#include "ns_peripherals_power.h"

// Audio variables and config


static int recording_win = NUM_FRAMES;

typedef enum { WAITING_TO_RECORD, WAIT_FOR_FRAME, INFERING } myState_e;

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
    bool measure_first_inference = true;
    ns_perf_counters_t pp;
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};

    myState_e state = WAITING_TO_RECORD;

    // Tells callback if it should be recording audio
    audioRecording = false;

    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed.\n");
    ns_itm_printf_enable();
    ns_interrupt_master_enable();
    NS_TRY(ns_timer_init(&basic_tickTimer), "Timer init failed.\n");
    NS_TRY(ns_audio_init(&audio_config), "Audio initialization Failed.\n");
    NS_TRY(ns_audio_set_gain(AM_HAL_PDM_GAIN_P345DB, AM_HAL_PDM_GAIN_P345DB), "Gain set failed.\n"); // PDM gain
#ifdef DYNAMIC_AUDIO_SOURCE
    NS_TRY(ns_start_audio(&audio_config), "Audio start failed.\n");
#endif
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


