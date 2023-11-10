// #include "tflite.h"
#include <stdint.h>
#include "seCntrlClass.h"
#include "am_util_stdio.h"
#include "ns_peripherals_button.h"
#include "ns_peripherals_power.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_audio.h"
#include "ambiq_nnsp_const.h"
#include "ns_timer.h"
#include "ns_energy_monitor.h"
#include "nn_speech.h"
#include "ns_perf_profile.h"

#include "ae_api.h"
#include "ns_ble.h"
#include "FreeRTOS.h"
#include "task.h"
#include "arm_math.h"

alignas(16) unsigned char static encodedDataBuffer[80]; // Opus encoder output length is hardcoded
                                                        // to 80 bytes
#include "audio_webble.h"

#ifdef DEF_GUI_ENABLE
    #include "ns_rpc_generic_data.h"
#endif
// #define ENERGY_MEASUREMENT
#define NUM_CHANNELS 1
int volatile g_intButtonPressed = 0;
/// Button Peripheral Config Struct
#ifdef DEF_GUI_ENABLE
ns_button_config_t button_config_nnsp = {
    .button_0_enable = false,
    .button_1_enable = false,
    .button_0_flag = NULL,
    .button_1_flag = NULL};
#else
ns_button_config_t button_config_nnsp = {
    .api = &ns_button_V1_0_0,
    .button_0_enable = true,
    .button_1_enable = false,
    .button_0_flag = &g_intButtonPressed,
    .button_1_flag = NULL};
#endif
/// Set by app when it wants to start recording, used by callback
bool volatile static g_audioRecording = false;
/// Set by callback when audio buffer has been copied, cleared by
/// app when the buffer has been consumed.
bool volatile static g_audioReady = false;
/// Audio buffer for application
int16_t static g_in16AudioDataBuffer[LEN_STFT_HOP << 1];
uint32_t static audadcSampleBuffer[(LEN_STFT_HOP << 1) + 3];
#ifdef USE_AUDADC
am_hal_audadc_sample_t static workingBuffer[SAMPLES_IN_FRAME * NUM_CHANNELS]; // working buffer
                                                                              // used by AUDADC
#endif
// Use this to generate a sinwave for debugging instead
// of using the microphone
alignas(16) int16_t static sinWave[320];

#ifdef DEF_GUI_ENABLE
static char msg_store[30] = "Audio16bPCM_to_WAV";
char msg_compute[30] = "CalculateMFCC_Please";
// Block sent to PC
static dataBlock pcmBlock = { // the block for pcm
    .length = (LEN_STFT_HOP << 1) * sizeof(int16_t),
    .dType = uint8_e,
    .description = msg_store,
    .cmd = write_cmd,
    .buffer = {
        .data = (uint8_t *)g_in16AudioDataBuffer, // point this to audio buffer
        .dataLength = (LEN_STFT_HOP << 1) * sizeof(int16_t)}};
// Block sent to PC for computation
dataBlock computeBlock = { // this block is useless here actually
    .length = (LEN_STFT_HOP << 1) * sizeof(int16_t),
    .dType = uint8_e,
    .description = msg_compute,
    .cmd = extract_cmd,
    .buffer = {
        .data = (uint8_t *)g_in16AudioDataBuffer, // point this to audio buffer
        .dataLength = (LEN_STFT_HOP << 1) * sizeof(int16_t)}};

dataBlock IsRecordBlock;
// Block sent to PC for computation
ns_rpc_config_t rpcConfig = {
    .mode = NS_RPC_GENERICDATA_CLIENT,
    .sendBlockToEVB_cb = NULL,
    .fetchBlockFromEVB_cb = NULL,
    .computeOnEVB_cb = NULL};
#endif

#if !defined(NS_AMBIQSUITE_VERSION_R4_1_0) && defined(NS_AUDADC_PRESENT)
am_hal_offset_cal_coeffs_array_t sOffsetCalib;
#endif

void audio_frame_callback(ns_audio_config_t *config, uint16_t bytesCollected) {
    if (g_audioRecording) {
        // if (g_audioReady)
        //     ns_lp_printf("-");
        ns_audio_getPCM_v2(config, g_in16AudioDataBuffer);
        g_audioReady = true;
    }
}

ns_audio_config_t audio_config = {
    .api = &ns_audio_V2_0_0,
    .eAudioApiMode = NS_AUDIO_API_CALLBACK,
    .callback = audio_frame_callback,
    .audioBuffer = (void *)&g_in16AudioDataBuffer,
#ifdef USE_AUDADC
    .eAudioSource = NS_AUDIO_SOURCE_AUDADC,
#else
    .eAudioSource = NS_AUDIO_SOURCE_PDM,
#endif
    .sampleBuffer = audadcSampleBuffer,
#if USE_AUDADC
    .workingBuffer = workingBuffer,
#else
    .workingBuffer = NULL,
#endif
    .numChannels = NUM_CHANNELS,
    .numSamples = LEN_STFT_HOP,
    .sampleRate = SAMPLING_RATE,
    .audioSystemHandle = NULL, // filled in by init
    .bufferHandle = NULL,      // only for ringbuffer mode
#if !defined(NS_AMBIQSUITE_VERSION_R4_1_0) && defined(NS_AUDADC_PRESENT)
    .sOffsetCalib = &sOffsetCalib,
#endif
};

// Opus frames are 20ms, so we need 320 samples at 16kHz
// AudioTask writes clean audio to xmitBuffer
// EndodeAndXferTask watches xmitBuffer, and encodes it when there are 320 samples available (and
// sends that)

#define XBUFSIZE 1280
alignas(16) int16_t static xmitBuffer[XBUFSIZE];
uint16_t static xmitWritePtr = 0;
uint16_t static xmitReadPtr = 0;
uint16_t static xmitAvailable = 0;

seCntrlClass cntrl_inst;
TaskHandle_t audio_task_handle;
TaskHandle_t my_xSetupTask;
TaskHandle_t radio_task_handle;
TaskHandle_t encode_task_handle;

ns_perf_counters_t start, end, delta;
int8_t do_it_once = 1;
ns_timer_config_t basic_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};

int flipSEtoNoSE = 0;

void audioTask(void *pvParameters) {
    uint32_t atStart, atEnd;
    ns_perf_counters_t start, end, delta;

    while (1) {
        if (g_audioReady) {
            // ns_lp_printf(".");
            // ns_capture_perf_profiler(&start);
            // atStart = ns_us_ticker_read(&basic_tickTimer);
            seCntrlClass_exec(&cntrl_inst, g_in16AudioDataBuffer, xmitBuffer + xmitWritePtr);
            flipSEtoNoSE = (flipSEtoNoSE + 1) % 200;
            if (flipSEtoNoSE > 100) {
                // overwrite the SE output with the original audio
                memcpy(
                    xmitBuffer + xmitWritePtr, g_in16AudioDataBuffer,
                    LEN_STFT_HOP * sizeof(int16_t));
            }
            // memcpy(xmitBuffer+xmitWritePtr, g_in16AudioDataBuffer, LEN_STFT_HOP *
            // sizeof(int16_t)); ns_capture_perf_profiler(&end); atEnd =
            // ns_us_ticker_read(&basic_tickTimer); ns_delta_perf(&start, &end, &delta);
            // ns_lp_printf("seCntrlClass_exec took %d us, %d cycles\n", atEnd-atStart,
            // delta.cyccnt); if (do_it_once == 1) {
            //     ns_print_perf_profile(&delta);
            //     do_it_once = 0;
            // }

            xmitAvailable += 160;
            xmitWritePtr = (xmitWritePtr + 160) % XBUFSIZE;
            g_audioReady = false;
            // ns_lp_printf("audio:  %d wp %d rp %d\n", xmitAvailable, xmitWritePtr, xmitReadPtr);
        }
        vTaskDelay(pdMS_TO_TICKS(1)); // this could be probably be slower, but we want to keep the
                                      // audio latency low
    }
}

void encodeAndXferTask(void *pvParameters) {
    uint32_t ret, oldTime, newTime, enStart, enEnd, bleStart, bleEnd;
    ns_perf_counters_t start, end, delta;

    while (1) {
        if (xmitAvailable > 640) {
            // ret = audio_enc_encode_frame(sinWave, 320, encodedDataBuffer);
            // ns_capture_perf_profiler(&start);
            // enStart = ns_us_ticker_read(&basic_tickTimer);
            // ns_lp_printf("-");
            ret = audio_enc_encode_frame(xmitBuffer + xmitReadPtr, 320, encodedDataBuffer);
            // enEnd = ns_us_ticker_read(&basic_tickTimer);
            // ns_capture_perf_profiler(&end);
            ns_delta_perf(&start, &end, &delta);
            // ns_lp_printf("audio_enc_encode_frame took %d us\n", enEnd-enStart);
            if (ret >= 0) {
                // bleStart = ns_us_ticker_read(&basic_tickTimer);
                ns_ble_send_value(&webbleOpusAudio, NULL);
                // bleEnd = ns_us_ticker_read(&basic_tickTimer);
                // oldTime = newTime;
                // newTime = ns_us_ticker_read(&basic_tickTimer);

            } else {
                ns_lp_printf("Error encoding %d\n", ret);
            }
            xmitAvailable -= 320;
            xmitReadPtr = (xmitReadPtr + 320) % XBUFSIZE;
            // ns_lp_printf("enTime %d bleTime %d enCyc %d \n", enEnd-enStart, bleEnd-bleStart,
            // delta.cyccnt); ns_lp_printf("encode: %d wp %d rp %d %d\n", xmitAvailable,
            // xmitWritePtr, xmitReadPtr, newTime-oldTime);
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void setup_task(void *pvParameters) {
    ns_lp_printf("Setting up BLE FreeRTOS Tasks\n");
    ns_ble_pre_init(); // Set NVIC priorities
    xTaskCreate(RadioTask, "RadioTask", 512, 0, 3, &radio_task_handle);
    xTaskCreate(audioTask, "AudioTask", 2048, 0, 3, &audio_task_handle);
    xTaskCreate(encodeAndXferTask, "encodeAndXferTask", 4096, 0, 3, &encode_task_handle);
    vTaskSuspend(NULL);
    while (1)
        ;
}

const ns_power_config_t ns_power_ble_orig = {
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

const ns_power_config_t ns_power_ble = {
    .api = &ns_power_V1_0_0,
    .eAIPowerMode = NS_MAXIMUM_PERF,
    .bNeedAudAdc = true,
    .bNeedSharedSRAM = false,
    .bNeedCrypto = false,
    .bNeedBluetooth = true,
    .bNeedUSB = false,
    .bNeedIOM = false,
    .bNeedAlternativeUART = false,
    .b128kTCM = false,
    .bEnableTempCo = false,
    .bNeedITM = false};

int main_tiny(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};

    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\b");
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_AUDADC);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_USB);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_USBPHY);
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();
    ns_itm_printf_enable();

    xTaskCreate(setup_task, "Setup", 512, 0, 3, &my_xSetupTask);
    vTaskStartScheduler();
    while (1) {
    };
}

int main(void) {
    int16_t *se_output = g_in16AudioDataBuffer + LEN_STFT_HOP;
    g_audioRecording = false;

    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\b");
    // ns_power_config(&ns_lp_audio);
    ns_power_config(&ns_power_ble);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_AUDADC);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_USB);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_USBPHY);
    // am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    // am_hal_cachectrl_enable();
    NS_TRY(ns_set_performance_mode(NS_MAXIMUM_PERF), "Set CPU Perf mode failed. ");

#ifdef ENERGY_MEASUREMENT
    // ns_uart_printf_enable(); // use uart to print, uses less power
    ns_itm_printf_enable();
    ns_init_power_monitor_state();
    ns_set_power_monitor_state(NS_IDLE);
#else
    ns_itm_printf_enable();
#endif

    ns_audio_init(&audio_config);
    ns_peripheral_button_init(&button_config_nnsp);
    ns_init_perf_profiler();
    ns_start_perf_profiler();

    // Initialize the Opus encoder
    audio_enc_init(0);

    NS_TRY(ns_timer_init(&basic_tickTimer), "Timer init failed.\n");

    // enc = ns_opus_encoder_default_create();

    // Generate a 400hz sin wave (for debugging)
    for (int i = 0; i < 320; i++) {
        sinWave[i] = (int16_t)(sin(2 * 3.14159 * 400 * i / SAMPLING_RATE) * 32767);
    }

    // initialize neural nets controller
    seCntrlClass_init(&cntrl_inst);
#ifdef DEF_ACC32BIT_OPT
    ns_lp_printf("You are using \"32bit\" accumulator.\n");
#else
    ns_lp_printf("You are using \"64bit\" accumulator.\n");
#endif

#ifdef DEF_GUI_ENABLE
    NNSPClass *pt_nnsp;
    pt_nnsp = (NNSPClass *)cntrl_inst.pt_nnsp;
    ns_rpc_genericDataOperations_init(&rpcConfig); // init RPC and USB
    ns_lp_printf("\nTo start recording, on your cmd, type\n\n");
    ns_lp_printf(
        "\t$ python ../python/tools/audioview_se.py --tty=/dev/tty.usbmodem1234561 # MacOS \n");
    ns_lp_printf("\t\tor\n");
    ns_lp_printf("\t> python ../python/tools/audioview_se.py --tty=COM4 # Windows \n");
    ns_lp_printf(
        "\nPress \'record\' on GUI to start, and then \'stop\' on GUI to stop recording.\n");
    ns_lp_printf("(You might change the \"--tty\" option based on your OS.)\n\n");
    ns_lp_printf("After \'stop\', check the raw recorded speech \'audio_raw.wav\' and enhanced "
                 "speech \'audio_se.wav\'\n");
    ns_lp_printf("under the folder \'nnsp/evb/audio_result/\'\n\n");
#else
    ns_lp_printf("\nPress button to start!\n");
#endif

    uint32_t start, end;

    start = ns_us_ticker_read(&basic_tickTimer);
    for (int i = 0; i < 100; i++) {
        seCntrlClass_exec(&cntrl_inst, sinWave, se_output);
    }
    end = ns_us_ticker_read(&basic_tickTimer);
    ns_lp_printf("Time for %d frames: %d us\n", 100, end - start);

    // BLE is FreeRTOS-driven, everything happens in the tasks set up by setup_task()
    // Audio notifications will start immediately, no waiting for button presses
    g_audioRecording = true;
    xTaskCreate(setup_task, "Setup", 512, 0, 3, &my_xSetupTask);
    vTaskStartScheduler();
    while (1) {
    };

    // tflite_init();
    // test_tflite();

    while (1) {
        g_audioRecording = false;
        g_intButtonPressed = 0;

        ns_deep_sleep();
#ifdef DEF_GUI_ENABLE
        while (1) {

            ns_rpc_data_computeOnPC(&computeBlock, &IsRecordBlock);
            if (IsRecordBlock.buffer.data[0] == 1) {
                seCntrlClass_reset(&cntrl_inst);
                g_intButtonPressed = 1;
                ns_rpc_data_clientDoneWithBlockFromPC(&IsRecordBlock);
                break;
            }
            ns_rpc_data_clientDoneWithBlockFromPC(&IsRecordBlock);
            am_hal_delay_us(20000);
        }
#endif
        if ((g_intButtonPressed == 1) && (!g_audioRecording)) {
            ns_lp_printf("\nYou'd pressed the button. Program start!\n");
            g_intButtonPressed = 0;
            g_audioRecording = true;
            am_hal_delay_us(10);
            while (1) {
                ns_set_power_monitor_state(NS_DATA_COLLECTION);
                ns_deep_sleep();
                if (g_audioReady) {
                    // execution of each time frame data
#ifdef DEF_GUI_ENABLE
                    pt_nnsp->pt_params->pre_gain_q1 = IsRecordBlock.buffer.data[1];
#endif
                    seCntrlClass_exec(&cntrl_inst, g_in16AudioDataBuffer, se_output);
                    ns_lp_printf(".");
#ifdef DEF_GUI_ENABLE
                    ns_rpc_data_sendBlockToPC(&pcmBlock);
                    ns_rpc_data_computeOnPC(&computeBlock, &IsRecordBlock);
                    if (IsRecordBlock.buffer.data[0] == 0) {
                        g_audioRecording = false;
                        g_audioReady = false;
                        g_intButtonPressed = 0;
                        ns_rpc_data_clientDoneWithBlockFromPC(&IsRecordBlock);
                        break;
                    }
                    ns_rpc_data_clientDoneWithBlockFromPC(&IsRecordBlock);
#endif
                    g_audioReady = false;
                }

            } // while(1)
            ns_lp_printf("\nPress button to start!\n");
        }
    } // while(1)
}
