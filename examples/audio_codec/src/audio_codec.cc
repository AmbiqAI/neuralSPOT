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

// #define AC_RPC_MODE

#include "ns_ambiqsuite_harness.h"
#include "ns_audio.h"
#include "ns_core.h"
#include "ns_peripherals_button.h"
#include "ns_peripherals_power.h"
#ifdef AC_RPC_MODE
    #include "ns_rpc_generic_data.h"
#endif
#include "ae_api.h"
#include "ns_ble.h"
#include "FreeRTOS.h"
#include "task.h"
#include "arm_math.h"

// -- Audio Stuff - needed for demo, not RPC ----------------------
#define NUM_CHANNELS 1
#define NUM_FRAMES 100
#define SAMPLES_IN_FRAME 320
#define SAMPLE_RATE 16000
alignas(16) unsigned char static encodedDataBuffer[80]; // TODO check this length

#ifndef AC_RPC_MODE
    #include "audio_webble.h" // include this after declarations above
#endif

volatile bool static g_audioReady = false;
volatile bool static g_audioRecording = false;

#if NUM_CHANNELS == 1
alignas(16) int16_t static audioDataBuffer[SAMPLES_IN_FRAME]; // incoming PCM audio data
#else
int32_t static audioDataBuffer[SAMPLES_IN_FRAME];
#endif

alignas(16) uint32_t static dmaBuffer[SAMPLES_IN_FRAME * NUM_CHANNELS * 2]; // DMA target
// am_hal_audadc_sample_t static sLGSampleBuffer[SAMPLES_IN_FRAME * NUM_CHANNELS]; // working buffer
//                                                                                 // used by AUDADC

#ifdef AM_HAL_TEMPCO_LP
am_hal_offset_cal_coeffs_array_t sOffsetCalib;
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
#ifdef AM_HAL_TEMPCO_LP
    .sOffsetCalib = &sOffsetCalib,
#endif
};
// -- Audio Stuff Ends ----------------------

#ifdef AC_RPC_MODE
    // RPC Stuff
    #define MY_USB_RX_BUFSIZE 2048
    #define MY_USB_TX_BUFSIZE 2048
static uint8_t my_cdc_rx_ff_buf[MY_USB_RX_BUFSIZE];
static uint8_t my_cdc_tx_ff_buf[MY_USB_TX_BUFSIZE];
// End RPC Stuff
#else
TaskHandle_t audio_task_handle;
TaskHandle_t my_xSetupTask;
TaskHandle_t radio_task_handle;
#endif

typedef void *spl_opus_encoder_h;

alignas(16) int16_t static sinWave[SAMPLES_IN_FRAME]; // for debugging

#ifndef AC_RPC_MODE
void audioTask(void *pvParameters) {
    int ui32EncoderReturn = 80;
    // uint32_t totalenc = 80;
    while (1) {
        if (g_audioReady) {
            ui32EncoderReturn =
                // audio_enc_encode_frame(sinWave, SAMPLES_IN_FRAME, encodedDataBuffer);
                audio_enc_encode_frame(audioDataBuffer, SAMPLES_IN_FRAME, encodedDataBuffer);
            if (ui32EncoderReturn >= 0) {
                ns_ble_send_value(&webbleOpusAudio, NULL);
                // ns_lp_printf(".");
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
#endif // AC_RPC_MODE

int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\b");

#ifndef AC_RPC_MODE
    NS_TRY(ns_power_config(&ns_power_ble), "Power Init Failed\n");
#else
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed\n");
#endif
    // am_bsp_low_power_init();
    NS_TRY(ns_set_performance_mode(NS_MAXIMUM_PERF), "Set CPU Perf mode failed.");
    NS_TRY(ns_audio_init(&audioConfig), "Audio Initialization Failed.\n");
    audio_enc_init(0);

    // Generate a 400hz sin wave
    for (int i = 0; i < SAMPLES_IN_FRAME; i++) {
        sinWave[i] = (int16_t)(sin(2 * 3.14159 * 400 * i / SAMPLE_RATE) * 32767);
    }

    ns_itm_printf_enable();
    ns_interrupt_master_enable();
    g_audioRecording = true;

#ifndef AC_RPC_MODE
    xTaskCreate(setup_task, "Setup", 512, 0, 3, &my_xSetupTask);
    vTaskStartScheduler();
    while (1) {
    };
#else

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

    // Vars and init the RPC system - note this also inits the USB interface
    status stat;
    binary_t binaryBlock = {// .data = (uint8_t *)audioDataBuffer, // point this to audio buffer
                            .data = (uint8_t *)encodedDataBuffer, // point this to audio buffer
                            .dataLength = sizeof(audioDataBuffer)};

    char msg_store[30] = "Audio16k_OPUS";

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

    ns_lp_printf("Start the PC-side server, then press Button 0 to get started\n");
    while (g_intButtonPressed == 0) {
        ns_delay_us(1000);
    }

    ns_lp_printf("Starting Opus-over-RPC  demo.\n");
    g_audioRecording = false;

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
                    ui32EncoderReturn =
                        audio_enc_encode_frame(sinWave, SAMPLES_IN_FRAME, encodedDataBuffer);
                    totalenc += ui32EncoderReturn;

                    outBlock.length = ui32EncoderReturn;
                    outBlock.buffer.dataLength = ui32EncoderReturn;

                    stat = ns_rpc_data_sendBlockToPC(&outBlock);

                    if (stat == ns_rpc_data_success) {
                        ns_lp_printf(". %d .", ui32EncoderReturn);
                        // ns_lp_printf("raw 0x%x%x%x\n", audioDataBuffer[0], audioDataBuffer[1],
                        // audioDataBuffer[2]); ns_lp_printf("enc 0x%x%x%x\n", encodedDataBuffer[0],
                        // encodedDataBuffer[1], encodedDataBuffer[2]);
                    } else
                        ns_lp_printf("[%d]+", stat);
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
#endif
}
