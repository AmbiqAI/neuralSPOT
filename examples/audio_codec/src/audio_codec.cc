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
// #include "ns_rpc_generic_data.h"
#include "ae_api.h"
#include "ns_ble.h"
#include "FreeRTOS.h"
#include "task.h"

// -- Audio Stuff - needed for demo, not RPC ----------------------
#define NUM_CHANNELS 1
#define NUM_FRAMES 100
#define SAMPLES_IN_FRAME 320
#define SAMPLE_RATE 16000
alignas(16) unsigned char static encodedDataBuffer[80]; // TODO check this
#include "audio_webble.h"                               // include this after declarations above

volatile bool static g_audioReady = false;
volatile bool static g_audioRecording = false;

#if NUM_CHANNELS == 1
alignas(16) int16_t static audioDataBuffer[SAMPLES_IN_FRAME]; // incoming PCM audio data
// unsigned char static encodedDataBuffer[SAMPLES_IN_FRAME/2]; // TODO check this
#else
int32_t static audioDataBuffer[SAMPLES_IN_FRAME];
#endif

alignas(16) uint32_t static dmaBuffer[SAMPLES_IN_FRAME * NUM_CHANNELS * 2]; // DMA target
// am_hal_audadc_sample_t static sLGSampleBuffer[SAMPLES_IN_FRAME * NUM_CHANNELS]; // working buffer
//                                                                                 // used by AUDADC

#ifndef NS_AMBIQSUITE_VERSION_R4_1_0
// am_hal_offset_cal_coeffs_array_t sOffsetCalib;
#endif

void audio_frame_callback(ns_audio_config_t *config, uint16_t bytesCollected) {
    if (g_audioRecording) {
        // if (g_audioReady) {
        //     ns_lp_printf("Overflow!\n");
        // }
        ns_audio_getPCM_v2(config, audioDataBuffer);
        g_audioReady = true;
    }
}

ns_audio_config_t audioConfig = {
    .api = &ns_audio_V2_0_0,
    .eAudioApiMode = NS_AUDIO_API_CALLBACK,
    .callback = audio_frame_callback,
    .audioBuffer = (void *)&audioDataBuffer,
    .eAudioSource = NS_AUDIO_SOURCE_PDM,
    // .eAudioSource = NS_AUDIO_SOURCE_AUDADC,
    .sampleBuffer = dmaBuffer,
    .workingBuffer = NULL,
    .numChannels = NUM_CHANNELS,
    .numSamples = SAMPLES_IN_FRAME,
    .sampleRate = SAMPLE_RATE,
    .audioSystemHandle = NULL, // filled in by init
    .bufferHandle = NULL,      // only for ringbuffer mode
#ifndef AM_HAL_TEMPCO_LP
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

typedef void *spl_opus_encoder_h;

TaskHandle_t audio_task_handle;
TaskHandle_t my_xSetupTask;
TaskHandle_t radio_task_handle;

void audioTask(void *pvParameters) {
    int ui32EncoderReturn = 80;
    uint32_t totalenc = 80;

    while (1) {
        if (g_audioReady) {
            ui32EncoderReturn =
                audio_enc_encode_frame(audioDataBuffer, SAMPLES_IN_FRAME, encodedDataBuffer);
            if (ui32EncoderReturn >= 0) {
                ns_ble_send_value(&webbleOpusAudio, NULL);
                ns_lp_printf(".");
            } else
                ns_lp_printf("Error encoding %d\n", ui32EncoderReturn);
            g_audioReady = false;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void setup_task(void *pvParameters) {
    ns_lp_printf("setup_task Hi!\n");
    ns_ble_pre_init(); // Set NVIC priorities
    xTaskCreate(RadioTask, "RadioTask", 512, 0, 3, &radio_task_handle);
    ns_lp_printf("setup_task1\n");

    xTaskCreate(audioTask, "AudioTask", 4096, 0, 3, &audio_task_handle);
    ns_lp_printf("setup_task2\n");

    vTaskSuspend(NULL);
    while (1)
        ;
}

const ns_power_config_t ns_power_ble = {
    .api = &ns_power_V1_0_0,
    .eAIPowerMode = NS_MAXIMUM_PERF,
    .bNeedAudAdc = true,
    .bNeedSharedSRAM = true,
    .bNeedCrypto = false,
    .bNeedBluetooth = true,
    .bNeedUSB = false,
    .bNeedIOM = true,
    .bNeedAlternativeUART = false,
    .b128kTCM = false,
    .bEnableTempCo = false,
    .bNeedITM = true};

// int work_main(void) {
//     ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
//     volatile int g_intButtonPressed = 0;
//     int recordingWin = NUM_FRAMES;
//     uint32_t ui32EncoderReturn;
//     uint32_t totalenc = 0;
//     NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\b");
//     // NS_TRY(ns_power_config(&ns_power_ble), "Power Init Failed\n");
//     // NS_TRY(ns_set_performance_mode(NS_MINIMUM_PERF), "Set CPU Perf mode failed.");
//     am_bsp_low_power_init();
//     audio_enc_init(0);

//     ns_itm_printf_enable();
//     ns_interrupt_master_enable();
//     xTaskCreate(setup_task, "Setup", 512, 0, 3, &my_xSetupTask);
//     vTaskStartScheduler();
//     while (1) {
//     };
// }

int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};

    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\b");
    NS_TRY(ns_power_config(&ns_power_ble), "Power Init Failed\n");
    // am_bsp_low_power_init();
    NS_TRY(ns_set_performance_mode(NS_MAXIMUM_PERF), "Set CPU Perf mode failed.");
    NS_TRY(ns_audio_init(&audioConfig), "Audio Initialization Failed.\n");
    // ---
    audio_enc_init(0);

    ns_itm_printf_enable();
    ns_interrupt_master_enable();
    g_audioRecording = true;

    xTaskCreate(setup_task, "Setup", 512, 0, 3, &my_xSetupTask);
    vTaskStartScheduler();
    while (1) {
    };
    // -- Init the button handler, used in the example, not needed by RPC
    volatile int g_intButtonPressed = 0;
    // ns_button_config_t button_config = {
    //     .api = &ns_button_V1_0_0,
    //     .button_0_enable = true,
    //     .button_1_enable = false,
    //     .button_0_flag = &g_intButtonPressed,
    //     .button_1_flag = NULL};
    // NS_TRY(ns_peripheral_button_init(&button_config), "Button init failed\n");

    // -- Audio init
    int recordingWin = NUM_FRAMES;
    //    NS_TRY(ns_audio_init(&audioConfig), "Audio Initialization Failed.\n");

#ifdef RPC_MODE
    // Vars and init the RPC system - note this also inits the USB interface
    status stat;
    binary_t binaryBlock = {// .data = (uint8_t *)audioDataBuffer, // point this to audio buffer
                            .data = (uint8_t *)encodedDataBuffer, // point this to audio buffer
                            .dataLength = sizeof(audioDataBuffer)};

    #if NUM_CHANNELS == 1
    char msg_store[30] = "Audio16k_OPUS";
    #endif

    // Block sent to PC
    dataBlock outBlock = {
        .length = sizeof(audioDataBuffer),
        .dType = uint8_e,
        .description = msg_store,
        .cmd = write_cmd,
        .buffer = binaryBlock,
    };

    ns_rpc_config_t rpcConfig = {
        .api = &ns_rpc_gdo_V1_0_0,
        .mode = NS_RPC_GENERICDATA_CLIENT,
        .rx_buf = my_cdc_rx_ff_buf,
        .rx_bufLength = MY_USB_RX_BUFSIZE,
        .tx_buf = my_cdc_tx_ff_buf,
        .tx_bufLength = MY_USB_TX_BUFSIZE,
        .sendBlockToEVB_cb = NULL,
        .fetchBlockFromEVB_cb = NULL,
        .computeOnEVB_cb = NULL,
    };

    // Result of computation
    NS_TRY(ns_rpc_genericDataOperations_init(&rpcConfig), "RPC Init Failed\n"); // init RPC and USB

#else
// Web BLE Mode
#endif

    // ns_lp_printf("Start the PC-side server, then press Button 0 to get started fff\n");
    // while (g_intButtonPressed == 0) {
    //     ns_delay_us(1000);
    // }

    g_audioRecording = false;

    // ns_lp_printf("Starting remote procedure call demo.\n");
    // g_audioRecording = true; // BLE is constantly streaming
    xTaskCreate(setup_task, "Setup", 512, 0, 3, &my_xSetupTask);
    vTaskStartScheduler();
    // audioBLEInit();

    // In the app loop we service USB and the RPC server while
    // we collect data and send it over the various RPC
    // interfaces. Any incoming RPC calls will result in calls to the
    // RPC handler functions defined above.
    uint32_t ui32EncoderReturn;
    uint32_t totalenc = 0;
    while (1) {
        if ((g_intButtonPressed) == 1 && !g_audioRecording) {
            g_audioRecording = true;
            ns_printf("Listening for 3 seconds.\n");
            // ns_rpc_data_remotePrintOnPC("EVB Says this: Listening for 3 seconds.\n");

            while (recordingWin > 0) {
                if (g_audioReady) { // got an audio sample
                    recordingWin--;
                    g_audioReady = false;
                    ui32EncoderReturn = audio_enc_encode_frame(
                        audioDataBuffer, SAMPLES_IN_FRAME, encodedDataBuffer);
                    totalenc += ui32EncoderReturn;

#ifdef RPC_MODE

                    outBlock.length = ui32EncoderReturn;
                    outBlock.buffer.dataLength = ui32EncoderReturn;
                    // octopus_encode(codec, audioDataBuffer, SAMPLES_IN_FRAME, encodedDataBuffer,
                    // SAMPLES_IN_FRAME); // play with this
                    stat = ns_rpc_data_sendBlockToPC(&outBlock);

                    if (stat == ns_rpc_data_success) {
                        ns_lp_printf(". %d .", ui32EncoderReturn);
                        // ns_lp_printf("raw 0x%x%x%x\n", audioDataBuffer[0], audioDataBuffer[1],
                        // audioDataBuffer[2]); ns_lp_printf("enc 0x%x%x%x\n", encodedDataBuffer[0],
                        // encodedDataBuffer[1], encodedDataBuffer[2]);
                    } else
                        ns_lp_printf("[%d]+", stat);
#else
                    // Web BLE Mode
                    ns_ble_send_value(&webbleOpusAudio, NULL);
#endif
                }
            }
            ns_lp_printf(
                "%d bytes, total %d, frames %d\n", ui32EncoderReturn, totalenc, NUM_FRAMES);
            // ns_rpc_data_remotePrintOnPC(
            //     "EVB Says this: 3s Sample Sent. Press button for next sample\n");

            g_audioRecording = false;
            g_intButtonPressed = 0;
            recordingWin = NUM_FRAMES;
        }
        ns_deep_sleep();
    }
}
