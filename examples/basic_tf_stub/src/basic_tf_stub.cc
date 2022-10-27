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

#include "basic_tf_stub.h"
#include "basic_audio.h"
#include "basic_mfcc.h"
#include "basic_model.h"
#include "basic_peripherals.h"
#ifdef RPC_ENABLED
#include "basic_rpc_client.h"
#endif

/// NeuralSPOT Includes
#include "ns_ambiqsuite_harness.h"
#include "ns_peripherals_power.h"
#include "ns_usb.h"
// #include "mfcc-old.h"

static int recording_win = NUM_FRAMES;

static const char *intents[] = {
    "decrease",        "deactivate", "increase",
    "change language", "bring",      "activate",
};

static const char *slots[] = {
    "socks",  "washroom", "chinese", "juice",   "volume",    "shoes",
    "music",  "heat",     "lights",  "kitchen", "newspaper", "lamp",
    "german", "korean",   "english", "bedroom", "none",
};

typedef enum {
    WAITING_TO_START_RPC_SERVER,
    WAITING_TO_RECORD,
    WAIT_FOR_FRAME,
    INFERING
} myState_e;

/**
 * @brief Main function - infinite loop listening and inferring
 * 
 * @return int 
 */
int
main(void) {
    float tmp = 0;
    float mfcc_buffer[NUM_FRAMES * MY_MFCC_NUM_MFCC_COEFFS];
    // float mfcc_bufferold[NUM_FRAMES * MY_MFCC_NUM_MFCC_COEFFS];
    float y_intent[6];
    float y_slot[2][17];
    uint8_t y_slot_max[2];
    uint8_t y_intent_max = 0;
    float max_val = 0.0;
    #ifdef RPC_ENABLED
        myState_e state = WAITING_TO_START_RPC_SERVER;
    #else
        myState_e state = WAITING_TO_RECORD;
    #endif

    // Tells callback if it should be recording audio
    audioRecording = false;

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

    model_init();
    ns_audio_init(&audio_config);
    ns_mfcc_init(&mfcc_config);
    // ns_mfcc_init_old();
    ns_peripheral_button_init(&button_config);
    am_hal_interrupt_master_enable();

    #ifdef RPC_ENABLED
        ns_rpc_genericDataOperations_init(&rpcConfig); // init RPC and USB
        ns_printf("Start the PC-side server, then press Button 0 to get started\n");
    #else
        ns_printf("Press Button 0 to start listening...\n");
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
                buttonPressed = false;
                audioRecording = true;
                ns_lp_printf("Listening for 3 seconds.\n");
                #ifdef RPC_ENABLED
                    ns_rpc_data_remotePrintOnPC("EVB Says this: Listening for 3 seconds.\n");
                #endif
            }
            break;

        case WAIT_FOR_FRAME:
            if (audioReady) {
                int32_t mfcc_buffer_head =
                    (NUM_FRAMES - recording_win) * MY_MFCC_NUM_MFCC_COEFFS;

                #ifdef RINGBUFFER_MODE
                    ns_ipc_ring_buffer_pop(audioBuf,
                                                &in16AudioDataBuffer,
                                                audio_config.numSamples * 2);
                #endif
                #ifdef RPC_ENABLED
                    ns_rpc_data_sendBlockToPC(&outBlock);
                #endif
                ns_mfcc_compute(&mfcc_config, in16AudioDataBuffer,
                                &mfcc_buffer[mfcc_buffer_head]);
                // ns_mfcc_compute_old(in16AudioDataBuffer,
                //                 &mfcc_bufferold[mfcc_buffer_head]);
                recording_win--;
                audioReady = false;

                ns_lp_printf(".");
            }    

            if (recording_win == 0) {
                ns_lp_printf("\n");
                audioRecording = false;
                recording_win = 100;
                state = INFERING; // have full 3 seconds
            }
            break;

        case INFERING:
            for (uint16_t i = 0; i < (NUM_FRAMES*MY_MFCC_NUM_MFCC_COEFFS); i = i + 1) {
                tmp = mfcc_buffer[i] / input->params.scale +
                      input->params.zero_point;
                tmp = MAX(MIN(tmp, 127), -128);
                input->data.int8[i] = (int8_t)tmp;
                // if (mfcc_buffer[i] != mfcc_bufferold[i]) {
                //     ns_lp_printf("%d: %f neq %f\n", mfcc_buffer[i], mfcc_bufferold[i]);
                // }
            }

            TfLiteStatus invoke_status = interpreter->Invoke();
            if (invoke_status != kTfLiteOk) {
                ns_lp_printf("Invoke failed\n");
                while (1) {}; // hang
            }
            
            max_val = 0.0;
            for (uint8_t i = 0; i < 6; i = i + 1) {
                y_intent[i] = (output_intent->data.int8[i] -
                               output_intent->params.zero_point) *
                              output_intent->params.scale;
                ns_lp_printf("Intent[%i]: %f %s\n", i, y_intent[i], intents[i]);

                if (y_intent[i] > max_val) {
                    max_val = y_intent[i];
                    y_intent_max = i;
                }
            }

            ns_lp_printf("**Max Intent: %s\n", intents[y_intent_max]);

            for (uint8_t i = 0; i < 2; i = i + 1) {
                max_val = 0.0;

                for (uint8_t j = 0; j < 17; j = j + 1) {
                    y_slot[i][j] = (output_slot->data.int8[i * 17 + j] -
                                    output_slot->params.zero_point) *
                                   output_slot->params.scale;
                    ns_lp_printf("slot[%i, %i]: %f %s\n", i, j,
                                         y_slot[i][j], slots[j]);
                    if (y_slot[i][j] > max_val) {
                        max_val = y_slot[i][j];
                        y_slot_max[i] = j;
                    }
                }

                ns_lp_printf("**Slot[%d]: %s \n", i,
                                     slots[y_slot_max[i]]);
            }
            ns_lp_printf("\n%s %s %s\n", intents[y_intent_max],
                                 slots[y_slot_max[0]], slots[y_slot_max[1]]);

            state = WAITING_TO_RECORD;
            break;
        }
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
    } // while(1)
}
