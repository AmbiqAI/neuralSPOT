/**
 @file main.cc

 @brief Speech-to-Intent using NeuralSPOT

Load a TFLite model into SRAM over RPC.

**/

#define RPC_ENABLED
#define USE_PDM_MICROPHONE

// Model array - will be filled by RPC
#define KWS_MODEL_SIZE 53744
unsigned int kws_ref_model_aligned_tflite_len = KWS_MODEL_SIZE;
alignas(16) unsigned char g_kws_model_data[KWS_MODEL_SIZE];

#include "basic_tf_stub.h"
#include "basic_audio.h"
#include "basic_mfcc.h"
#include "basic_model.h"
#include "basic_peripherals.h"
#include "ns_core.h"
#include "ns_rpc_generic_data.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_energy_monitor.h"
// #include "ns_perf_profile.h"
#include "ns_peripherals_power.h"

volatile uint32_t model_chunk_offset = 0;

static int recording_win = NUM_FRAMES;
typedef enum { WAITING_TO_START_RPC_SERVER, WAITING_TO_RECORD, WAIT_FOR_FRAME, INFERING } myState_e;

#if (configAPPLICATION_ALLOCATED_HEAP == 1)
size_t ucHeapSize = (NS_RPC_MALLOC_SIZE_IN_K + 4) * 1024;
uint8_t ucHeap[(NS_RPC_MALLOC_SIZE_IN_K + 4) * 1024] __attribute__((aligned(4)));
#endif
uint8_t cdc_rx_ff_buf[4096];
uint8_t cdc_tx_ff_buf[4096];

status incomingModelChunk(const dataBlock *in) {
    ns_lp_printf(
        "[INFO] PC Sent Model Chunk of %d bytes, copied to %d\n", in->buffer.dataLength,
        model_chunk_offset);
    // Get latest chunk, copy into next spot in model array
    memcpy(&g_kws_model_data[model_chunk_offset], in->buffer.data, in->buffer.dataLength);
    model_chunk_offset += in->buffer.dataLength;
    ns_lp_printf("Model chunk offset: %d\n", model_chunk_offset);
    return ns_rpc_data_success;
}

/**
 * @brief Main basic_tf_stub - infinite loop listening and inferring
 *
 * @return int
 */
int main(void) {
    float tmp = 0;
    float mfcc_buffer[NUM_FRAMES * MY_MFCC_NUM_MFCC_COEFFS];
    float output[kCategoryCount];
    uint8_t output_max = 0;
    float max_val = 0.0;
    // bool measure_first_inference = MEASURE_ARM_PERF;
    ns_perf_counters_t pp;
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};

    myState_e state = WAITING_TO_RECORD;

    // Tells callback if it should be recording audio
    audioRecording = false;

    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed.\n");
    ns_interrupt_master_enable();

    ns_itm_printf_enable();

#ifdef NS_MLPROFILE
    NS_TRY(ns_timer_init(&basic_tickTimer), "Timer init failed.\n");
#endif

    NS_TRY(ns_audio_init(&audio_config), "Audio initialization Failed.\n");
    NS_TRY(ns_mfcc_init(&mfcc_config), "MFCC config failed.\n");
    NS_TRY(ns_peripheral_button_init(&button_config), "Button initialization failed.\n")

    ns_rpc_config_t rpcConfig = {
        .api = &ns_rpc_gdo_V1_0_0,
        .mode = NS_RPC_GENERICDATA_SERVER, // Puts EVB in RPC server mode
        .rx_buf = cdc_rx_ff_buf,
        .rx_bufLength = 4096,
        .tx_buf = cdc_tx_ff_buf,
        .tx_bufLength = 4096,
        .sendBlockToEVB_cb = incomingModelChunk,
        .fetchBlockFromEVB_cb = NULL,
        .computeOnEVB_cb = NULL};
    NS_TRY(ns_rpc_genericDataOperations_init(&rpcConfig), "RPC Init Failed\n"); // init RPC and USB

    // Wait for button before loading model
    ns_lp_printf("Run Model Loader\n");

    // Wait for model to finish loading
    while (model_chunk_offset < KWS_MODEL_SIZE) {
        ns_rpc_genericDataOperations_pollServer(&rpcConfig);
        ns_delay_us(1000);
    }
    ns_lp_printf("Model loaded (%d bytes)\n", model_chunk_offset);
    model_init();

    ns_lp_printf("This KWS example listens for 1 second, then classifies\n");
    ns_lp_printf("the captured audio into one of the following phrases:\n");
    ns_lp_printf("yes, no, up, down, left, right, on, off, or unknown/silence\n\n");

    ns_lp_printf("Start the PC-side server, then press Button 0 to get started\n");

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
                ns_delay_us(250000); // wait for button click noise to die down
                audioRecording = true;
                ns_lp_printf("\nListening for 1 second.\n");
            }
            break;

        case WAIT_FOR_FRAME:
            if (audioReady) {
                ns_set_power_monitor_state(NS_FEATURE_EXTRACTION);

                int32_t mfcc_buffer_head = (NUM_FRAMES - recording_win) * MY_MFCC_NUM_MFCC_COEFFS;

                ns_mfcc_compute(&mfcc_config, audioDataBuffer, &mfcc_buffer[mfcc_buffer_head]);

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

            // if (measure_first_inference) {
            //     ns_start_perf_profiler();
            // }
            TfLiteStatus invoke_status = interpreter->Invoke();

            // if (measure_first_inference) {
            //     measure_first_inference = false;
            //     ns_stop_perf_profiler();
            //     ns_capture_perf_profiler(&pp);
            //     ns_print_perf_profile(&pp);
            // }
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
                    ns_lp_printf(
                        "\n[%s] with %d%% certainty\n", kCategoryLabels[i],
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
        // ns_deep_sleep();
    } // while(1)
}
