/**
 * @file audio_codec.cc
 * @author Carlos Morales
 * @brief Generic Data Operations RPC Example
 * @version 0.1
 * @date 2022-08-26
 *
 * This example 2 functions:
 *  1. Encoded audio using an Opus encoder
 *  2. Sending audio using either BLE or RPC
 *
 * The audio-over-BLE is for demo purposes only. It simply creates
 * a BLE service with a single characteristic, and sends the encoded
 * audio data to the PC - no audio protocols are involved.
 *
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <cstdlib>
#include <cstring>

// Uncomment the following like to enable RPC mode
// and disable BLE mode.
// #define AC_RPC_MODE

// If compiling for a platform that doesn't have BLE use RPC
// otherwise, default to BLE.
#if !defined(NS_BLE_SUPPORTED)
    #define AC_RPC_MODE
#endif

// By default, this example uses PDM microphone data.
// Uncomment the following line to use AUDADC data instead.
// #define USE_AUDADC

#include "ns_ambiqsuite_harness.h"
#include "ns_audio.h"
#include "ns_core.h"
#include "ns_peripherals_button.h"
#include "ns_peripherals_power.h"
#ifdef AC_RPC_MODE
    #include "ns_rpc_generic_data.h"
#else
    #include "ns_ble.h"
    #include "FreeRTOS.h"
    #include "task.h"
#endif
#include "ae_api.h"

#include "arm_math.h"

// -- Audio Stuff - needed for demo, not RPC ----------------------
#define NUM_CHANNELS 1
#define NUM_FRAMES 100
#define SAMPLES_IN_FRAME 320
#define SAMPLE_RATE 16000
alignas(16) unsigned char static encodedDataBuffer[80]; // Opus encoder output length is hardcoded
                                                        // to 80 bytes
#ifndef AC_RPC_MODE
    #include "audio_webble.h" // include this after declarations above
#endif

volatile bool static g_audioReady = false;
volatile bool static g_audioRecording = false;

alignas(16) int16_t static audioDataBuffer[SAMPLES_IN_FRAME]; // incoming PCM audio data

alignas(16) uint32_t static dmaBuffer[SAMPLES_IN_FRAME * NUM_CHANNELS * 2]; // DMA target
#ifdef USE_AUDADC
am_hal_audadc_sample_t static sLGSampleBuffer[SAMPLES_IN_FRAME * NUM_CHANNELS]; // working buffer
                                                                                // used by AUDADC
#endif

#if !defined(NS_AMBIQSUITE_VERSION_R4_1_0) && defined(NS_AUDADC_PRESENT)
am_hal_offset_cal_coeffs_array_t sOffsetCalib;
#endif

void audio_frame_callback(ns_audio_config_t *config, uint16_t bytesCollected) {
    if (g_audioRecording) {
        ns_audio_getPCM_v2(config, audioDataBuffer);
        g_audioReady = true;
    }
}

ns_audio_config_t audioConfig = {
    .api = &ns_audio_V2_0_0,
    .eAudioApiMode = NS_AUDIO_API_CALLBACK,
    .callback = audio_frame_callback,
    .audioBuffer = (void *)&audioDataBuffer,
#ifdef USE_AUDADC
    .eAudioSource = NS_AUDIO_SOURCE_AUDADC,
#else
    .eAudioSource = NS_AUDIO_SOURCE_PDM,
#endif
    .sampleBuffer = dmaBuffer,
#if USE_AUDADC
    .workingBuffer = workingBuffer,
#else
    .workingBuffer = NULL,
#endif
    .numChannels = NUM_CHANNELS,
    .numSamples = SAMPLES_IN_FRAME,
    .sampleRate = SAMPLE_RATE,
    .audioSystemHandle = NULL, // filled in by init
    .bufferHandle = NULL,      // only for ringbuffer mode
#if !defined(NS_AMBIQSUITE_VERSION_R4_1_0) && defined(NS_AUDADC_PRESENT)
    .sOffsetCalib = &sOffsetCalib,
#endif
};
// -- Audio Stuff Ends ----------------------

#ifdef AC_RPC_MODE
    #define MY_USB_RX_BUFSIZE 2048
    #define MY_USB_TX_BUFSIZE 2048
static uint8_t my_cdc_rx_ff_buf[MY_USB_RX_BUFSIZE];
static uint8_t my_cdc_tx_ff_buf[MY_USB_TX_BUFSIZE];
#else
TaskHandle_t audio_task_handle;
TaskHandle_t my_xSetupTask;
TaskHandle_t radio_task_handle;
#endif

typedef void *spl_opus_encoder_h;

// Use this to generate a sinwave for debugging instead
// of using the microphone
alignas(16) int16_t static sinWave[SAMPLES_IN_FRAME];

#ifndef AC_RPC_MODE
void audioTask(void *pvParameters) {
    int ui32EncoderReturn = 80;
    while (1) {
        if (g_audioReady) {
            ui32EncoderReturn =
                audio_enc_encode_frame(audioDataBuffer, SAMPLES_IN_FRAME, encodedDataBuffer);
            if (ui32EncoderReturn >= 0) {
                // This causes a notify to be sent over BLE, all subscribers
                // will get his block
                ns_ble_send_value(&webbleOpusAudio, NULL);
            } else
                ns_lp_printf("Error encoding %d\n", ui32EncoderReturn);
            g_audioReady = false;
        }
        vTaskDelay(pdMS_TO_TICKS(1)); // this could be probably be slower, but we want to keep the
                                      // audio latency low
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

// Note that we turn off USB power when we are in BLE mode
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

    // Initialize the Opus encoder
    audio_enc_init(0);

    // Generate a 400hz sin wave (for debugging)
    for (int i = 0; i < SAMPLES_IN_FRAME; i++) {
        sinWave[i] = (int16_t)(sin(2 * 3.14159 * 400 * i / SAMPLE_RATE) * 32767);
    }

    ns_itm_printf_enable();
    ns_interrupt_master_enable();
    g_audioRecording = true;

#ifndef AC_RPC_MODE
    // BLE is FreeRTOS-driven, everything happens in the tasks set up by setup_task()
    // Audio notifications will start immediately, no waiting for button presses
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
    binary_t binaryBlock = {
        .data = (uint8_t *)encodedDataBuffer, // point this to audio buffer
        .dataLength = sizeof(audioDataBuffer)};

    char msg_store[30] = "Audio16k_OPUS"; // python script can look for this

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
                        ns_lp_printf(".", ui32EncoderReturn);
                        // ns_lp_printf(". %d .", ui32EncoderReturn);
                        // ns_lp_printf("raw 0x%x%x%x\n", audioDataBuffer[0], audioDataBuffer[1],
                        // audioDataBuffer[2]); ns_lp_printf("enc 0x%x%x%x\n", encodedDataBuffer[0],
                        // encodedDataBuffer[1], encodedDataBuffer[2]);
                    } else
                        ns_lp_printf("[%d]+", stat);
                }
            }
            ns_lp_printf(
                "%d bytes, total %d, frames %d\n", ui32EncoderReturn, totalenc, NUM_FRAMES);

            g_audioRecording = false;
            g_intButtonPressed = 0;
            recordingWin = NUM_FRAMES;
        }
        ns_deep_sleep();
    }
#endif
}
