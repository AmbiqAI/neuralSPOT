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

#ifdef ENERGY_MONITOR_ENABLE
    // This is for measuring power using an external power monitor such as
    // Joulescope - it sets GPIO pins so the state can be observed externally
    // to help line up the waveforms. It has nothing to do with AI...
    ns_init_power_monitor_state();
#endif
    ns_set_power_monitor_state(NS_IDLE); // no-op if ns_init_power_monitor_state not called

    // Configure power - different use modes
    // require different power configs
    // This examples uses pre-populated power config structs -
    // to modify create a local struct and pass it to
    // ns_power_config()

    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed.\n");
    // NS_TRY(ns_set_performance_mode(NS_MINIMUM_PERF), "Set CPU Perf mode failed.");

    // Pick either ns_uart_printf_enable or ns_itm_printf_enable depending on your needs
    // ns_uart_printf_enable(); // use uart to print, uses less power
    ns_itm_printf_enable();

    NS_TRY(ns_timer_init(&basic_tickTimer), "Timer init failed.\n");
    model_init();
    NS_TRY(ns_audio_init(&audio_config), "Audio initialization Failed.\n");
    NS_TRY(ns_audio_set_gain(AM_HAL_PDM_GAIN_P345DB, AM_HAL_PDM_GAIN_P345DB), "Gain set failed.\n"); // PDM gain
#ifdef DYNAMIC_AUDIO_SOURCE
    NS_TRY(ns_start_audio(&audio_config), "Audio start failed.\n");
#endif
    NS_TRY(ns_mfcc_init(&mfcc_config), "MFCC config failed.\n");
    NS_TRY(ns_peripheral_button_init(&button_config), "Button initialization failed.\n")

    if (measure_first_inference) {
        ns_init_perf_profiler(); // count inference cycles the first time it is invoked
    }
    ns_interrupt_master_enable();

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
                ns_set_power_monitor_state(NS_DATA_COLLECTION);
                buttonPressed = false;
                ns_delay_us(250000); // wait for button click noise to die down
                audioRecording = true;
                ns_lp_printf("\nListening for 1 second.\n");
            }
            break;

        case WAIT_FOR_FRAME:
            if (audioReady) {
                ns_set_power_monitor_state(NS_FEATURE_EXTRACTION);

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
                tmp = mfcc_buffer[i] / model_input->params.scale + model_input->params.zero_point;
                tmp = MAX(MIN(tmp, 127), -128);
                model_input->data.int8[i] = (int8_t)tmp;
            }

            ns_set_power_monitor_state(NS_INFERING);

            if (measure_first_inference) {
                ns_start_perf_profiler();
            }
            TfLiteStatus invoke_status = interpreter->Invoke();

            if (measure_first_inference) {
                measure_first_inference = false;
                ns_stop_perf_profiler();
                ns_capture_perf_profiler(&pp);
                ns_print_perf_profile(&pp);
            }
            ns_set_power_monitor_state(NS_IDLE);

            if (invoke_status != kTfLiteOk) {
                ns_lp_printf("Invoke failed\n");
                while (1) {
                }; // hang
            }

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
            state = WAITING_TO_RECORD;
            break;
        }
        // ns_deep_sleep();
    } // while(1)
}
