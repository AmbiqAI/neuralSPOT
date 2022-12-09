/**
 @file main.cc

 @brief Speech-to-Intent using NeuralSPOT

The basic_tf_stub example is based on a speech to intent model.
 Speech-to-Intent is a neural-network-based Tensorflow model that listens
 for 3-5 second phrases and infers the intent of that phrase. It uses
 NeuralSPOT to collect audio from the AUDADC, calculate MFCC, set power
 modes, read button state, and print to the Jlink SWO.

**/

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

#define RINGBUFFER_MODE
// #define RPC_ENABLED
// #define ENERGY_MONITOR_ENABLE
// #define LOWEST_POWER_MODE

#include "basic_tf_stub.h"
#include "basic_audio.h"
#include "basic_mfcc.h"
#include "basic_model.h"
#include "basic_peripherals.h"
#include "ns_core.h"
#ifdef RPC_ENABLED
    #include "basic_rpc_client.h"
#endif

/// NeuralSPOT Includes
#include "ns_ambiqsuite_harness.h"
#include "ns_energy_monitor.h"
#include "ns_perf_profile.h"
#include "ns_peripherals_power.h"
#include "ns_usb.h"

static int recording_win = NUM_FRAMES;

typedef enum { WAITING_TO_START_RPC_SERVER, WAITING_TO_RECORD, WAIT_FOR_FRAME, INFERING } myState_e;

/**
 * @brief Main basic_tf_stub - infinite loop listening and inferring
 *
 * @return int
 */
int
main(void) {
    float tmp = 0;
    float mfcc_buffer[NUM_FRAMES * MY_MFCC_NUM_MFCC_COEFFS];
    float output[kCategoryCount];
    uint8_t output_max = 0;
    float max_val = 0.0;
    bool measure_first_inference = true;
    ns_perf_counters_t pp;

#ifdef RPC_ENABLED
    myState_e state = WAITING_TO_START_RPC_SERVER;
#else
    myState_e state = WAITING_TO_RECORD;
#endif

    // Tells callback if it should be recording audio
    audioRecording = false;

    ns_core_init();

#ifdef ENERGY_MONITOR_ENABLE
    // This is for measuring power using an external power monitor such as
    // Joulescope - it sets GPIO pins so the state can be observed externally
    // to help line up the waveforms. It has nothing to do with AI...
    ns_init_power_monitor_state();
    ns_set_power_monitor_state(NS_IDLE);
#endif

    // Configure power - different use modes
    // require different power configs
    // This examples uses pre-populated power config structs -
    // to modify create a local struct and pass it to
    // ns_power_config()

    ns_power_config(&ns_audio_default);

#ifdef LOWEST_POWER_MODE
    ns_uart_printf_enable(); // use uart to print, uses less power
#else
    ns_itm_printf_enable();
    /* A note about printf and low power: printing over ITM impacts low power in two
        ways:
        1) enabling ITM prevents SoC from entering deep sleep, and
        2) ITM initialization requires crypto to be enabled.

        While ITM printing isn't something a deployed application would enable, NS does the
        following to mitigate power usage during ITM:

        1) ns_power_config() lets you set bNeedITM
        2) ns_itm_printf_enable() will temporarily enable crypto if needed
        3) ns_lp_printf() enables TPIU and ITM when needed
        4) ns_deep_sleep() disables crypto, TPIU and ITM if enabled to allow full deep sleep
    */
#endif

    model_init();

    ns_audio_init(&audio_config);
    ns_mfcc_init(&mfcc_config);
    ns_peripheral_button_init(&button_config);
    if (measure_first_inference) {
        ns_init_perf_profiler(); // count inference cycles the first time it is invoked
    }
    am_hal_interrupt_master_enable();

    ns_lp_printf("This KWS example listens for 1 second, then classifies\n");
    ns_lp_printf("the captured audio into one of the following phrases:\n");
    ns_lp_printf("yes, no, up, down, left, right, on, off, or unknown/silence\n\n");

#ifdef RPC_ENABLED
    ns_rpc_genericDataOperations_init(&rpcConfig); // init RPC and USB
    ns_lp_printf("Start the PC-side server, then press Button 0 to get started\n");
#else
    ns_lp_printf("Press Button 0 to start listening...\n");
#endif

    // Event loop
    while (1) {
        switch (state) {
        case WAITING_TO_START_RPC_SERVER:
            if (buttonPressed) {
                state = WAITING_TO_RECORD;
                ns_lp_printf("Press Button 0 to start listening...\n");
                buttonPressed = false;
            }
            break;

        case WAITING_TO_RECORD:
            if (buttonPressed) {
                state = WAIT_FOR_FRAME;
                ns_set_power_monitor_state(NS_DATA_COLLECTION);
                buttonPressed = false;
                audioRecording = true;
                ns_lp_printf("\nListening for 1 second.\n");
#ifdef RPC_ENABLED
                ns_rpc_data_remotePrintOnPC("EVB Says this: Listening for 1 second.\n");
#endif
            }
            break;

        case WAIT_FOR_FRAME:
            if (audioReady) {
                ns_set_power_monitor_state(NS_FEATURE_EXTRACTION);

                int32_t mfcc_buffer_head = (NUM_FRAMES - recording_win) * MY_MFCC_NUM_MFCC_COEFFS;

#ifdef RINGBUFFER_MODE
                ns_ipc_ring_buffer_pop(audioBuf, &in16AudioDataBuffer, audio_config.numSamples * 2);
#endif
#ifdef RPC_ENABLED
                ns_rpc_data_sendBlockToPC(&outBlock);
#endif
                ns_mfcc_compute(&mfcc_config, in16AudioDataBuffer, &mfcc_buffer[mfcc_buffer_head]);

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
                    ns_lp_printf("\n[%s] with %d%% certainty\n", kCategoryLabels[i],
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
        ns_deep_sleep();
    } // while(1)
}
