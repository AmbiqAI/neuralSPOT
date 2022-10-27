/**
 * @file rpc_client.cc
 * @author Carlos Morales
 * @brief Generic Data Operations RPC Example
 * @version 0.1
 * @date 2022-08-26
 * 
 * This example showcases the NS Generic Data RPC interface, which
 * is useful for sending data back and forth across the EVB's second
 * USB interface via remote procedure calls.
 * 
 * Both remote procedure call directions (EVB to PC and PC to EVB) are
 * demonstrated, using audio as an example data source.
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <cstdlib>
#include <cstring>

#include "ns_ambiqsuite_harness.h"
#include "ns_audio.h"
#include "ns_rpc_generic_data.h"
#include "ns_peripherals_button.h"
#include "ns_peripherals_power.h"

// -- Audio Stuff - needed for demo, not RPC ----------------------
#define NUM_CHANNELS 1
#define NUM_FRAMES 100
#define SAMPLES_IN_FRAME 480
#define SAMPLE_RATE 16000

bool static g_audioReady = false;
bool static g_audioRecording = false;
int16_t static g_in16AudioDataBuffer[SAMPLES_IN_FRAME * 2];
uint32_t static audadcSampleBuffer[SAMPLES_IN_FRAME * 2 + 3];

void audio_frame_callback(ns_audio_config_t *config, uint16_t bytesCollected) {
    uint32_t *pui32_buffer =
        (uint32_t *)am_hal_audadc_dma_get_buffer(config->audioSystemHandle);

    if (g_audioRecording) {
        for (int i = 0; i < config->numSamples; i++) {
            g_in16AudioDataBuffer[i] = (int16_t)(pui32_buffer[i] & 0x0000FFF0);
        }
        g_audioReady = true;
    }
}

ns_audio_config_t audioConfig = {
    .eAudioApiMode = NS_AUDIO_API_CALLBACK,
    .callback = audio_frame_callback,
    .audioBuffer = (void *)&g_in16AudioDataBuffer,
    .eAudioSource = NS_AUDIO_SOURCE_AUDADC,
    .sampleBuffer = audadcSampleBuffer,
    .numChannels = NUM_CHANNELS,
    .numSamples = SAMPLES_IN_FRAME,
    .sampleRate = SAMPLE_RATE,
    .audioSystemHandle = NULL, 
    .bufferHandle = NULL
};
// -- Audio Stuff Ends ----------------------

int main(void) {
    ns_itm_printf_enable();
    ns_debug_printf_enable();

    // ----- Non-RPC Init ------
    // -- These are needed for the demo, not directly related to RPC
    am_hal_interrupt_master_enable();
    ns_power_config(&ns_development_default);

    // -- Init the button handler, used in the example, not needed by RPC
    int g_intButtonPressed = 0;
    ns_button_config_t button_config = {
        .button_0_enable = true,
        .button_1_enable = false,
        .button_0_flag = &g_intButtonPressed,
        .button_1_flag = NULL
    };
    ns_peripheral_button_init(&button_config);

    // -- Audio init
    int recordingWin = NUM_FRAMES;
    ns_audio_init(&audioConfig);

    // Vars and init the RPC system - note this also inits the USB interface
    status stat;
    binary_t binaryBlock = {
        .data = (uint8_t *) g_in16AudioDataBuffer, // point this to audio buffer
        .dataLength = SAMPLES_IN_FRAME * sizeof(int16_t)
    };
    char msg_store[30] = "Audio16bPCM_to_WAV";
    char msg_compute[30] = "CalculateMFCC_Please";

    // Block sent to PC
    dataBlock outBlock = {
        .length = SAMPLES_IN_FRAME * sizeof(int16_t),
        .dType = uint8_e,
        .description = msg_store,
        .cmd = write_cmd,
        .buffer = binaryBlock
    };

    // Block sent to PC for computation
    dataBlock computeBlock = {
        .length = SAMPLES_IN_FRAME * sizeof(int16_t),
        .dType = uint8_e,
        .description = msg_compute,
        .cmd = extract_cmd,
        .buffer = binaryBlock
    };

    ns_rpc_config_t rpcConfig = {
        .mode = NS_RPC_GENERICDATA_CLIENT,
        .sendBlockToEVB_cb = NULL,
        .fetchBlockFromEVB_cb = NULL,
        .computeOnEVB_cb = NULL
    };
    // Result of computation
    dataBlock resultBlock;
    ns_rpc_genericDataOperations_init(&rpcConfig); // init RPC and USB

    // There is a chicken-and-egg thing involved in getting the RPC
    // started. The PC-side server cant start until the USB TTY interface
    // shows up as a device, and that doesn't happen until we start servicing
    // TinyUSB, and even then it doesn't happen immediately.
    //
    // To address this, we loop waiting for a button press, servicing
    // USB. This gives the user a chance to start the server then
    // pressing the button to let the EVB it is ready to start RPCing.

    ns_printf("Start the PC-side server, then press Button 0 to get started\n");
    while (g_intButtonPressed == 0) {
        ns_delay_us(1000);
    }
    // g_intButtonPressed = 0;
    g_audioRecording = false;

    ns_printf("Starting remote procedure call demo.\n");

    // In the app loop we service USB and the RPC server while
    // we collect data and send it over the various RPC
    // interfaces. Any incoming RPC calls will result in calls to the 
    // RPC handler functions defined above.

    while (1) {
        if ((g_intButtonPressed) == 1 && !g_audioRecording) {
            g_audioRecording = true;
            ns_printf("Listening for 3 seconds.\n");
            ns_rpc_data_remotePrintOnPC("EVB Says this: Listening for 3 seconds.\n");

            while (recordingWin > 0) {
                if (g_audioReady) { // got an audio sample
                    recordingWin--;
                    g_audioReady = false;
                    
                    stat = ns_rpc_data_sendBlockToPC(&outBlock);

                    if (stat == ns_rpc_data_success)
                        ns_printf("-");
                    else
                        ns_printf("[%d]+",stat);

                    // Compute something remotely based on the collected sample (e.g. MFCC)
                    stat = ns_rpc_data_computeOnPC(&computeBlock, &resultBlock);

                    // ns_rpc_data_computeOnPC silently mallocs memory for
                    // block-description and block->buffer.data. After using
                    // the block, we must free those two struct members.
                    ns_rpc_data_clientDoneWithBlockFromPC(&resultBlock);
                }
            }
            ns_printf("\n");
            ns_rpc_data_remotePrintOnPC("EVB Says this: 3s Sample Sent. Press button for next sample\n");

            g_audioRecording = false;
            g_intButtonPressed = 0;
            recordingWin = 100;
        }
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
    }
}
