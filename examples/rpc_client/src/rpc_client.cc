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
#include "ns_core.h"
#include "ns_peripherals_button.h"
#include "ns_peripherals_power.h"
#include "ns_rpc_generic_data.h"

// -- Audio Stuff - needed for demo, not RPC ----------------------
#define NUM_CHANNELS 1
#define NUM_FRAMES 100
#define SAMPLES_IN_FRAME 480
#define SAMPLE_RATE 16000

volatile bool static g_audioReady = false;
volatile bool static g_audioRecording = false;

#if NUM_CHANNELS == 1
int16_t static audioDataBuffer[SAMPLES_IN_FRAME]; // incoming PCM audio data
#else
int32_t static audioDataBuffer[SAMPLES_IN_FRAME];
#endif

alignas(16) uint32_t static dmaBuffer[SAMPLES_IN_FRAME * NUM_CHANNELS * 2];     // DMA target
am_hal_audadc_sample_t static sLGSampleBuffer[SAMPLES_IN_FRAME * NUM_CHANNELS]; // working buffer
                                                                                // used by AUDADC

#ifndef NS_AMBIQSUITE_VERSION_R4_1_0
am_hal_offset_cal_coeffs_array_t sOffsetCalib;
#endif

void audio_frame_callback(ns_audio_config_t *config, uint16_t bytesCollected) {
    if (g_audioRecording) {
        if (g_audioReady) {
            ns_lp_printf("Overflow!\n");
        }
        ns_audio_getPCM_v2(config, audioDataBuffer);
        g_audioReady = true;
    }
}

ns_audio_config_t audioConfig = {
    .api = &ns_audio_V2_0_0,
    .eAudioApiMode = NS_AUDIO_API_CALLBACK,
    .callback = audio_frame_callback,
    .audioBuffer = (void *)&audioDataBuffer,
    // .eAudioSource = NS_AUDIO_SOURCE_PDM,
    .eAudioSource = NS_AUDIO_SOURCE_AUDADC,
    .sampleBuffer = dmaBuffer,
    .workingBuffer = sLGSampleBuffer,
    .numChannels = NUM_CHANNELS,
    .numSamples = SAMPLES_IN_FRAME,
    .sampleRate = SAMPLE_RATE,
    .audioSystemHandle = NULL, // filled in by init
    .bufferHandle = NULL,      // only for ringbuffer mode
#ifndef NS_AMBIQSUITE_VERSION_R4_1_0
    .sOffsetCalib = &sOffsetCalib,
#endif
};
// -- Audio Stuff Ends ----------------------

// RPC Stuff
#define MY_USB_RX_BUFSIZE 2048
#define MY_USB_TX_BUFSIZE 2048
static uint8_t my_cdc_rx_ff_buf[MY_USB_RX_BUFSIZE];
static uint8_t my_cdc_tx_ff_buf[MY_USB_TX_BUFSIZE];
// End RPC Stuff

int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};

    // ----- Non-RPC Init ------
    // -- These are needed for the demo, not directly related to RPC
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\b");
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed\n");
    // NS_TRY(ns_power_config(&ns_audio_default), "Power Init Failed\n");
    NS_TRY(ns_set_performance_mode(NS_MINIMUM_PERF), "Set CPU Perf mode failed.");
    // ---

    ns_itm_printf_enable();
    ns_interrupt_master_enable();

    // -- Init the button handler, used in the example, not needed by RPC
    volatile int g_intButtonPressed = 0;
    ns_button_config_t button_config = {
        .api = &ns_button_V1_0_0,
        .button_0_enable = true,
        .button_1_enable = false,
        .button_0_flag = &g_intButtonPressed,
        .button_1_flag = NULL};
    NS_TRY(ns_peripheral_button_init(&button_config), "Button init failed\n");

    // -- Audio init
    int recordingWin = NUM_FRAMES;
    NS_TRY(ns_audio_init(&audioConfig), "Audio Initialization Failed.\n");

    // Vars and init the RPC system - note this also inits the USB interface
    status stat;
    binary_t binaryBlock = {
        .data = (uint8_t *)audioDataBuffer, // point this to audio buffer
        .dataLength = sizeof(audioDataBuffer)};

#if NUM_CHANNELS == 1
    char msg_store[30] = "Audio16bPCM_to_WAV";
#else
    char msg_store[30] = "Audio32bPCM_to_WAV";
#endif

    char msg_compute[30] = "CalculateMFCC_Please";

    // Block sent to PC
    dataBlock outBlock = {
        .length = sizeof(audioDataBuffer),
        .dType = uint8_e,
        .description = msg_store,
        .cmd = write_cmd,
        .buffer = binaryBlock};

    // Block sent to PC for computation
    dataBlock computeBlock = {
        .length = SAMPLES_IN_FRAME * sizeof(int16_t),
        .dType = uint8_e,
        .description = msg_compute,
        .cmd = extract_cmd,
        .buffer = binaryBlock};

    ns_rpc_config_t rpcConfig = {
        .api = &ns_rpc_gdo_V1_0_0,
        .mode = NS_RPC_GENERICDATA_CLIENT,
        .rx_buf = my_cdc_rx_ff_buf,
        .rx_bufLength = MY_USB_RX_BUFSIZE,
        .tx_buf = my_cdc_tx_ff_buf,
        .tx_bufLength = MY_USB_TX_BUFSIZE,
        .sendBlockToEVB_cb = NULL,
        .fetchBlockFromEVB_cb = NULL,
        .computeOnEVB_cb = NULL};
    // Result of computation
    dataBlock resultBlock;
    NS_TRY(ns_rpc_genericDataOperations_init(&rpcConfig), "RPC Init Failed\n"); // init RPC and USB

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
                        ns_printf(".");
                    else
                        ns_printf("[%d]+", stat);

                    // Compute something remotely based on the collected sample (e.g. MFCC)
                    stat = ns_rpc_data_computeOnPC(&computeBlock, &resultBlock);

                    // ns_rpc_data_computeOnPC silently mallocs memory for
                    // block-description and block->buffer.data. After using
                    // the block, we must free those two struct members.
                    ns_rpc_data_clientDoneWithBlockFromPC(&resultBlock);
                }
            }
            ns_printf("\n");
            ns_rpc_data_remotePrintOnPC(
                "EVB Says this: 3s Sample Sent. Press button for next sample\n");

            g_audioRecording = false;
            g_intButtonPressed = 0;
            recordingWin = 100;
        }
        ns_deep_sleep();
    }
}
