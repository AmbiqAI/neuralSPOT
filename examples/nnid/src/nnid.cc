#include <stdint.h>
#include "nnidCntrlClass.h"
#include "am_util_stdio.h"
#include "ns_peripherals_button.h"
#include "ns_peripherals_power.h"
#include "ns_ambiqsuite_harness.h"
#include "am_util.h"
#include "ns_audio.h"
#include "ambiq_nnsp_const.h"
#include "ns_timer.h"
#include "ns_energy_monitor.h"
#include "ae_api.h"
#include "ns_ble.h"
#include "FreeRTOS.h"
#include "task.h"

alignas(16) unsigned char static encodedDataBuffer[80]; // Opus encoder output length is hardcoded
                                                        // to 80 bytes
alignas(16) int16_t static sinWave[320];

alignas(16) char msgBuf[120];
char vad = 0;

// Buttons
int idBtn = 0;
int enrollBtn = 0;
uint8_t numUtterances = 0;
#include "nnid_webble.h"

// #define ENERGY_MEASUREMENT
#define NUM_CHANNELS 1
#define SAMPLES_IN_FRAME 320
#define SAMPLE_RATE 16000

// BLE uses malloc, so we need a heap
#if (configAPPLICATION_ALLOCATED_HEAP == 1)
    #define NNID_HEAP_SIZE (NS_BLE_DEFAULT_MALLOC_K * 4 * 1024)
size_t ucHeapSize = NNID_HEAP_SIZE;
uint8_t ucHeap[NNID_HEAP_SIZE] __attribute__((aligned(4)));
#endif

ns_button_config_t button_config_nnsp = {
    .api = &ns_button_V1_0_0,
    .button_0_enable = true,
    .button_1_enable = true,
    .button_0_flag = &idBtn,
    .button_1_flag = &enrollBtn};

// Audio
bool volatile static g_audioRecording = false; // Start listening, app to audio system
bool volatile static g_audioReady = false;     // audio cpatures

int16_t static g_in16AudioDataBuffer[LEN_STFT_HOP];
alignas(16) uint32_t static audadcSampleBuffer[LEN_STFT_HOP * 2];
#ifdef USE_AUDADC
am_hal_audadc_sample_t static workingBuffer[SAMPLES_IN_FRAME * NUM_CHANNELS]; // working buffer
                                                                              // used by AUDADC
#endif
#if !defined(NS_AMBIQSUITE_VERSION_R4_1_0) && defined(NS_AUDADC_PRESENT)
am_hal_offset_cal_coeffs_array_t sOffsetCalib;
#endif

uint16_t framesSampled = 0;
void audio_frame_callback(ns_audio_config_t *config, uint16_t bytesCollected) {

    if (g_audioRecording) {
        ns_audio_getPCM_v2(config, g_in16AudioDataBuffer);
        if (g_audioReady) {
            ns_lp_printf("audio_frame_callback: audioReady already set\n");
        }
        g_audioReady = true;
    }
}

ns_audio_config_t audio_config = {
    .api = &ns_audio_V2_1_0,
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

// Custom power mode for BLE+Audio
const ns_power_config_t ns_power_ble = {
    .api = &ns_power_V1_0_0,
    .eAIPowerMode = NS_MAXIMUM_PERF,
    .bNeedAudAdc = false,
    .bNeedSharedSRAM = false,
    .bNeedCrypto = false,
    .bNeedBluetooth = true,
    .bNeedUSB = false,
    .bNeedIOM = false,
    .bNeedAlternativeUART = false,
    .b128kTCM = false,
    .bEnableTempCo = false,
    .bNeedITM = false};

// BLE and FreeRTOS state
TaskHandle_t audio_task_handle; // Collects audio identifies it
TaskHandle_t radio_task_handle; // Services WSF for BLE
TaskHandle_t my_xSetupTask;     // Starts the other tasks and suspends itself

// Application State
#define MAX_ENROLLEES 5
nnidCntrlClass cntrl_inst;
float corr_array[MAX_ENROLLEES];

typedef enum {
    WAIT_FOR_BTN = 0,
    WAIT_FOR_ENROLL_AUDIO,
    WAIT_FOR_ID_AUDIO,
    AUDIO_CAPTURED,
    ENROLL,
    IDENTIFY
} state_t;

enroll_state_T currentEnrollmentPhase = enroll_phase;
uint8_t numEnrolledSpeakers = 0;
uint8_t idCurrentlyEnrollingSpeaker = 0;
uint8_t numUtterancesInCurrentEnrollment = 0;
//#define bleMessage ns_lp_printf
uint16_t framesSent = 0;
void bleMessage(const char *fmt, ...) {
    va_list args;
    memset(msgBuf, 0, sizeof(msgBuf));
    va_start(args, fmt);
    am_util_stdio_vsnprintf(msgBuf, sizeof(msgBuf), fmt, args);
    am_util_stdio_vprintf(fmt, args);
    va_end(args);
    ns_ble_send_value(&webbleMessage, NULL); // Send over BLE
}

bool enoughToSend = false;
int16_t encoderInput[320];
void encodeAndSendAudio(
    void) { // send once every two 10ms frames, only used for visualization purposes
    // return;
    if (enoughToSend) {
        memcpy(encoderInput + 160, g_in16AudioDataBuffer, sizeof(int16_t) * 160);
        // memcpy(encoderInput + 160, sinWave, sizeof(int16_t) * 160);
        audio_enc_encode_frame(encoderInput, 320, encodedDataBuffer);
        framesSent++;
        ns_ble_send_value(&webbleOpusAudio, NULL); // Send the encoded audio over BLE
        if (framesSent == 500) {                   // 10 seconds
            framesSent = 0;
            ns_lp_printf("Sent 10 seconds of audio\n");
        }
        enoughToSend = false;
    } else {
        memcpy(encoderInput, g_in16AudioDataBuffer, sizeof(int16_t) * 160);
        // memcpy(encoderInput, sinWave, sizeof(int16_t) * 160);
        enoughToSend = true;
    }
}

void audioTask(void *pvParameters) {
    state_t state = WAIT_FOR_BTN;
    int16_t detected = 0;
    ns_lp_printf("Starting audio task\n");
    g_audioRecording = true;

    bleMessage("Press Button 1 to start enrollment, Button 0 to identify");
    audio_enc_encode_frame(sinWave, 320, encodedDataBuffer);

    while (1) {
        switch (state) {
        case WAIT_FOR_BTN:
            // audio_enc_encode_frame(sinWave, 320, encodedDataBuffer);
            // ns_ble_send_value(&webbleOpusAudio, NULL); // Send the encoded audio over BLE
            if (g_audioReady) {
                // This doesn't actually do anything except send audio
                // to browser for visualization purposes - not used by the model

                encodeAndSendAudio();
                g_audioReady = false;
                vad = 0;
                // ns_ble_send_value(&webbleVad, NULL); // send VAD detection state
            }

            if (enrollBtn) {
                enrollBtn = 0;
                bleMessage(
                    "Enrolling Speaker %d: speak naturally for 3s",
                    idCurrentlyEnrollingSpeaker + 1);
                if (numEnrolledSpeakers < MAX_ENROLLEES) {
                    g_audioRecording = true;
                    state = WAIT_FOR_ENROLL_AUDIO;
                } else {
                    // Error - we only support 5 enrollees
                    bleMessage("Max enrollees reached");
                }
            } else if (idBtn) {
                idBtn = 0;

                if (numUtterancesInCurrentEnrollment != 0) {
                    bleMessage(
                        "Please finish enrollment before identification. Still only %d of 4",
                        numUtterancesInCurrentEnrollment);
                } else if (numEnrolledSpeakers == 0) {
                    bleMessage("No enrollees");
                } else {
                    ns_lp_printf("Identifying: speak naturally");
                    g_audioRecording = true;
                    state = WAIT_FOR_ID_AUDIO;
                }
            }
            break;
        case WAIT_FOR_ENROLL_AUDIO:
            if (g_audioReady) {
                // execution of each time frame data
                // ns_lp_printf(".");
                // encodeAndSendAudio();
                cntrl_inst.enroll_state = enroll_phase;
                cntrl_inst.id_enroll_ppl = idCurrentlyEnrollingSpeaker;
                cntrl_inst.total_enroll_ppls = numEnrolledSpeakers;
                nnidCntrlClass_exec(&cntrl_inst, g_in16AudioDataBuffer, corr_array);
                ns_ble_send_value(&webbleVad, NULL); // send VAD detection state
                if (numUtterancesInCurrentEnrollment != cntrl_inst.acc_num_enroll) {
                    // If acc_num_enroll has changed, we have a new utterance
                    numUtterancesInCurrentEnrollment = cntrl_inst.acc_num_enroll;
                    // g_audioRecording = false; // stop recording
                    numUtterances = numUtterancesInCurrentEnrollment;
                    ns_ble_send_value(&webbleNumUtterances, NULL); // Send over BLE

                    // If we have 4 utterances, that speaker is considered enrolled
                    // Bump idCurrentlyEnrollingSpeaker to the next speaker
                    if (numUtterancesInCurrentEnrollment == 4) {
                        bleMessage("Speaker %d enrolled", idCurrentlyEnrollingSpeaker + 1);
                        numEnrolledSpeakers++;
                        cntrl_inst.total_enroll_ppls = numEnrolledSpeakers;
                        idCurrentlyEnrollingSpeaker++;
                        numUtterancesInCurrentEnrollment = 0;
                        cntrl_inst.acc_num_enroll = 0;
                        state = WAIT_FOR_BTN;
                    } else {
                        bleMessage(
                            "Utterance %d/4 for speaker %d Captured. Press Button 1 for next.",
                            numUtterancesInCurrentEnrollment, idCurrentlyEnrollingSpeaker + 1);
                        state = WAIT_FOR_ENROLL_AUDIO;
                    }
                    // state = WAIT_FOR_BTN;
                } else {
                    // ns_lp_printf("-");
                }
                g_audioReady = false;
            }
            break;
        case WAIT_FOR_ID_AUDIO:
            // If we are here, we have at least one enrolled speaker and the idBtn was pressed
            if (g_audioReady) {
                g_audioReady = false;
                encodeAndSendAudio();
                cntrl_inst.enroll_state = test_phase;
                detected = nnidCntrlClass_exec(&cntrl_inst, g_in16AudioDataBuffer, corr_array);
                ns_ble_send_value(&webbleVad, NULL); // send VAD detection state

                if (detected) {
                    // ns_lp_printf("Model got enough data\n");
                    // Collected enough data - pick highest value. If none exceeds threshold,
                    // then no match and speaker is 'unknown'
                    float highest = 0.0;
                    int highestId = 0;
                    for (int i = 0; i < 5; i++) {
                        if (corr_array[i] > highest) {
                            highest = corr_array[i];
                            highestId = i;
                        }
                    }

                    if (highest > 0.7) {
                        bleMessage(
                            "Speaker %d identified with %.2f\% confidence", highestId + 1,
                            highest * 100);
                    } else {
                        bleMessage("Speaker not identified, best match: %.2f\%", highest * 100);
                    }
                    // g_audioRecording = false; // stop recording
                    state = WAIT_FOR_BTN;
                }
            }
            break;

        default:
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
        // ns_deep_sleep();
    }
}

void setup_task(void *pvParameters) {
    ns_lp_printf("Setting up BLE FreeRTOS Tasks\n");
    ns_ble_pre_init(); // Set NVIC  priorities
    xTaskCreate(RadioTask, "RadioTask", 512, 0, 3, &radio_task_handle);
    xTaskCreate(audioTask, "AudioTask", 2048, 0, 1, &audio_task_handle);
    vTaskSuspend(NULL);
    while (1)
        ;
}

int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\b");
    ns_power_config(&ns_power_ble);

    // NS_TRY(ns_set_performance_mode(NS_MINIMUM_PERF), "Set CPU Perf mode failed. ");

    ns_itm_printf_enable();

    ns_audio_init(&audio_config);
    ns_start_audio(&audio_config);
    ns_peripheral_button_init(&button_config_nnsp);
    // Initialize the Opus encoder
    audio_enc_init(0);
    // NS_TRY(ns_timer_init(&basic_tickTimer), "Timer init failed.\n");

    // Generate a 400hz sin wave (for debugging)
    for (int i = 0; i < 320; i++) {
        sinWave[i] = (int16_t)(sin(2 * 3.14159 * 400 * i / SAMPLE_RATE) * 32767);
    }

    // This is only for WebBLE visualization purposes. In reality, you only need
    // to capture during enrollment and identification phases.

    // initialize neural net controllers
    nnidCntrlClass_init(&cntrl_inst);
    nnidCntrlClass_resetPcmBufClass(&cntrl_inst);
#ifdef DEF_ACC32BIT_OPT
    ns_lp_printf("You are using \"32bit\" accumulator.\n");
#else
    ns_lp_printf("You are using \"64bit\" accumulator.\n");
#endif

    // Initial state
    cntrl_inst.id_enroll_ppl = 0;
    cntrl_inst.total_enroll_ppls = 0;
    cntrl_inst.enroll_state = enroll_phase;
    nnidCntrlClass_reset(&cntrl_inst);

    // BLE is FreeRTOS-driven, everything happens in the tasks set up by setup_task()
    xTaskCreate(setup_task, "Setup", 512, 0, 3, &my_xSetupTask);
    vTaskStartScheduler();
    while (1) {
        ns_deep_sleep();
    };
}
