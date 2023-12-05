// #include "tflite.h"
#include <stdint.h>
#include "nnidCntrlClass.h"
#include "am_util_stdio.h"
#include "ns_peripherals_button.h"
#include "ns_peripherals_power.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_audio.h"
#include "ambiq_nnsp_const.h"
#include "ns_timer.h"
#include "ns_energy_monitor.h"

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
int16_t static g_in16AudioDataBuffer[(LEN_STFT_HOP << 1) + 20];
uint32_t static audadcSampleBuffer[(LEN_STFT_HOP << 1) + 3];

typedef enum {
    button_stop = 0,
    button_start = 1,
} button_state_T;

#ifdef DEF_GUI_ENABLE
static char msg_store[30] = "Audio16bPCM_to_WAV";

char msg_compute[30] = "CalculateMFCC_Please";
// Block sent to PC
static dataBlock pcmBlock = { // the block for pcm
    .length = ((LEN_STFT_HOP << 1) + 10) * sizeof(int16_t),
    .dType = uint8_e,
    .description = msg_store,
    .cmd = write_cmd,
    .buffer = {
        .data = (uint8_t *)g_in16AudioDataBuffer, // point this to audio buffer
        .dataLength = ((LEN_STFT_HOP << 1) + 10) * sizeof(int16_t)}};

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
/**
 *
 * @brief Audio Callback (user-defined, executes in IRQ context)
 *
 * When the 'g_audioRecording' flag is set, copy the latest sample to a buffer
 * and set a 'ready' flag. If recording flag isn't set, discard buffer.
 * If 'ready' flag is still set, the last buffer hasn't been consumed yet,
 * print a debug message and overwrite.
 *
 */
void audio_frame_callback(ns_audio_config_t *config, uint16_t bytesCollected) {
    uint32_t *pui32_buffer = (uint32_t *)am_hal_audadc_dma_get_buffer(config->audioSystemHandle);

    if (g_audioRecording) {
        if (g_audioReady)
            ns_lp_printf("Warning - audio buffer wasnt consumed in time\n");

        // Raw PCM data is 32b (12b/channel) - here we only care about one
        // channel For ringbuffer mode, this loop may feel extraneous, but it is
        // needed because ringbuffers are treated a blocks, so there is no way
        // to convert 32b->16b
        for (int i = 0; i < config->numSamples; i++) {
            g_in16AudioDataBuffer[i] = (int16_t)(pui32_buffer[i] & 0x0000FFF0);

            if (i == 4) {
                // Workaround for AUDADC sample glitch, here while it is root caused
                g_in16AudioDataBuffer[3] =
                    (g_in16AudioDataBuffer[2] + g_in16AudioDataBuffer[4]) >> 1;
            }
        }
#ifdef RINGBUFFER_MODE
        ns_ring_buffer_push(
            &(config->bufferHandle[0]), g_in16AudioDataBuffer,
            (config->numSamples * 2), // in bytes
            false);
#endif
        g_audioReady = true;
    }
}

/**
 * @brief NeuralSPOT Audio config struct
 *
 * Populate this struct before calling ns_audio_config()
 *
 */
ns_audio_config_t audio_config = {
#ifdef RINGBUFFER_MODE
    .eAudioApiMode = NS_AUDIO_API_RINGBUFFER,
    .callback = audio_frame_callback,
    .audioBuffer = (void *)&pui8AudioBuff,
#else
    .eAudioApiMode = NS_AUDIO_API_CALLBACK,
    .callback = audio_frame_callback,
    .audioBuffer = (void *)&g_in16AudioDataBuffer,
#endif
    .eAudioSource = NS_AUDIO_SOURCE_AUDADC,
    .sampleBuffer = audadcSampleBuffer,
    .numChannels = NUM_CHANNELS,
    .numSamples = LEN_STFT_HOP,
    .sampleRate = SAMPLING_RATE,
    .audioSystemHandle = NULL, // filled in by audio_init()
#ifdef RINGBUFFER_MODE
    .bufferHandle = audioBuf
#else
    .bufferHandle = NULL
#endif
};

const ns_power_config_t ns_lp_audio = {
    .eAIPowerMode = NS_MAXIMUM_PERF,
    .bNeedAudAdc = true,
    .bNeedSharedSRAM = false,
    .bNeedCrypto = false,
    .bNeedBluetooth = false,
    .bNeedUSB = false,
    .bNeedIOM = false,
    .bNeedAlternativeUART = false,
    .b128kTCM = false,
    .bEnableTempCo = false,
    .bNeedITM = false};

int main(void) {
    nnidCntrlClass cntrl_inst;
    float corr_array[5];
    int16_t detected;
    int16_t *pt_debug;
    int16_t *pt_acc_num_enroll = g_in16AudioDataBuffer + (LEN_STFT_HOP << 1);
    int16_t *pt_is_result = pt_acc_num_enroll + 1;
    int16_t *pt_corr = pt_is_result + 1;
    g_audioRecording = false;
    ns_core_init();
    // ns_power_config(&ns_lp_audio);
    ns_power_config(&ns_audio_default);

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

    // initialize neural nets controller
    nnidCntrlClass_init(&cntrl_inst);
    nnidCntrlClass_resetPcmBufClass(&cntrl_inst);
#ifdef DEF_ACC32BIT_OPT
    ns_lp_printf("You are using \"32bit\" accumulator.\n");
#else
    ns_lp_printf("You are using \"64bit\" accumulator.\n");
#endif

#ifdef DEF_GUI_ENABLE
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

    // tflite_init();
    // test_tflite();
    nnidCntrlClass_speed_testing(&cntrl_inst, g_in16AudioDataBuffer);
    while (1) {
        g_audioRecording = false;
        g_intButtonPressed = 0;
        ns_deep_sleep();
#ifdef DEF_GUI_ENABLE
        // This infinite loop is checking whether the start button in PC side is pressed.
        while (1) {
            // EVB received IsRecordBlock sent form PC
            ns_rpc_data_computeOnPC(&computeBlock, &IsRecordBlock);
            if (IsRecordBlock.buffer.data[0] == button_start) {
                ns_printf("Start button pressed!   \n");
                cntrl_inst.id_enroll_ppl = IsRecordBlock.buffer.data[1];
                cntrl_inst.total_enroll_ppls = IsRecordBlock.buffer.data[2];
                cntrl_inst.enroll_state = (enroll_state_T)IsRecordBlock.buffer.data[3];

                // if button pressed, break the loop
                g_intButtonPressed = 1;
                ns_rpc_data_clientDoneWithBlockFromPC(&IsRecordBlock);
                nnidCntrlClass_reset(&cntrl_inst);
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
                    detected = nnidCntrlClass_exec(&cntrl_inst, g_in16AudioDataBuffer, corr_array);

                    // prepare information and send to PC side
                    *pt_acc_num_enroll = (int16_t)cntrl_inst.acc_num_enroll;
                    *pt_is_result = 0;
                    if (cntrl_inst.enroll_state == test_phase) {
                        if (detected) {
                            *pt_is_result = 1;
                            for (int i = 0; i < cntrl_inst.total_enroll_ppls; i++)
                                pt_corr[i] = (int16_t)(corr_array[i] * 32768.0f);
                        }
                    }

                    pt_debug = g_in16AudioDataBuffer + LEN_STFT_HOP;
                    for (int i = 0; i < LEN_STFT_HOP; i++)
                        *pt_debug++ = detected;
#ifdef DEF_GUI_ENABLE
                    ns_rpc_data_sendBlockToPC(&pcmBlock); // send data to PC sice
                    ns_rpc_data_computeOnPC(
                        &computeBlock, &IsRecordBlock); // receive data from PC side
                    // check if PC side asks for stopping the program
                    if (IsRecordBlock.buffer.data[0] == button_stop) {
                        g_audioRecording = false;
                        g_audioReady = false;
                        g_intButtonPressed = 0;
                        ns_printf("Stop button pressed!   \n");
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
