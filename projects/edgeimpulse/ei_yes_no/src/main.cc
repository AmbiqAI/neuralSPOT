#include <stdio.h>

#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_malloc.h"
#include "ns_timer.h"
#include "ns_peripherals_button.h"
#include "ns_peripherals_power.h"
#include "ns_audio.h"

// Audio Configuration
/// High level audio parameters
#define NUM_CHANNELS 1
#define NUM_FRAMES 100
#define SAMPLES_IN_FRAME 480
#define SAMPLE_RATE 16000

// Callback function declaration
static int get_signal_data(size_t offset, size_t length, float *out_ptr);

// Raw features copied from test sample (Edge Impulse > Model testing)
static float input_buf[SAMPLE_RATE]; //= {
//     /* Paste your raw features here! */
// 8.3245, -0.1543, 9.0725, 8.3245, -0.1543, 9.0725, 8.3245, -0.1543, 9.0725, 8.3245, -0.1543, 9.0725, 8.1145, -0.1777, 9.0442, 8.1145, -0.1777, 9.0442, 8.1145, -0.1777, 9.0442, 8.1145, -0.1777, 9.0442, 8.1145, -0.1777, 9.0442, 8.1145, -0.1777, 9.0442, 8.1145, -0.1777, 9.0442, 8.1145, -0.1777, 9.0442, 8.0823, -0.4580, 8.9446, 8.0823, -0.4580, 8.9446, 8.0823, -0.4580, 8.9446, 8.0823, -0.4580, 8.9446, 8.0823, -0.4580, 8.9446, 8.0823, -0.4580, 8.9446, 8.0823, -0.4580, 8.9446, 8.1126, -0.1123, 8.8303, 8.1126, -0.1123, 8.8303, 8.1126, -0.1123, 8.8303, 8.1126, -0.1123, 8.8303, 8.1126, -0.1123, 8.8303, 8.1126, -0.1123, 8.8303, 8.1126, -0.1123, 8.8303, 7.9856, -0.3379, 8.9036, 7.9856, -0.3379, 8.9036, 7.9856, -0.3379, 8.9036, 7.9856, -0.3379, 8.9036, 7.9856, -0.3379, 8.9036, 7.9856, -0.3379, 8.9036, 7.9856, -0.3379, 8.9036, 8.2258, -0.4404, 8.9475, 8.2258, -0.4404, 8.9475, 8.2258, -0.4404, 8.9475, 8.2258, -0.4404, 8.9475, 8.2258, -0.4404, 8.9475, 8.2258, -0.4404, 8.9475, 8.2258, -0.4404, 8.9475, 8.1565, -0.1387, 9.0647, 8.1565, -0.1387, 9.0647, 8.1565, -0.1387, 9.0647, 8.1565, -0.1387, 9.0647, 8.1565, -0.1387, 9.0647, 8.1565, -0.1387, 9.0647, 8.1565, -0.1387, 9.0647, 8.2356, -0.4561, 9.0755, 8.2356, -0.4561, 9.0755, 8.2356, -0.4561, 9.0755, 8.2356, -0.4561, 9.0755, 8.2356, -0.4561, 9.0755, 8.2356, -0.4561, 9.0755, 8.2356, -0.4561, 9.0755, 8.2991, -0.1631, 9.1009, 8.2991, -0.1631, 9.1009, 8.2991, -0.1631, 9.1009, 8.2991, -0.1631, 9.1009, 8.2991, -0.1631, 9.1009, 8.2991, -0.1631, 9.1009, 8.2991, -0.1631, 9.1009, 8.0256, -0.2891, 9.0540, 8.0256, -0.2891, 9.0540, 8.0256, -0.2891, 9.0540, 8.0256, -0.2891, 9.0540, 8.0256, -0.2891, 9.0540, 8.0256, -0.2891, 9.0540, 8.0256, -0.2891, 9.0540, 8.1770, -0.3799, 8.7913, 8.1770, -0.3799, 8.7913, 8.1770, -0.3799, 8.7913, 8.1770, -0.3799, 8.7913, 8.1770, -0.3799, 8.7913, 8.1770, -0.3799, 8.7913, 8.1770, -0.3799, 8.7913, 8.0549, -0.1055, 8.8128, 8.0549, -0.1055, 8.8128, 8.0549, -0.1055, 8.8128, 8.0549, -0.1055, 8.8128, 8.0549, -0.1055, 8.8128, 8.0549, -0.1055, 8.8128, 8.0549, -0.1055, 8.8128, 8.0735, -0.3926, 8.8216, 8.0735, -0.3926, 8.8216, 8.0735, -0.3926, 8.8216, 8.0735, -0.3926, 8.8216, 8.0735, -0.3926, 8.8216, 8.0735, -0.3926, 8.8216, 8.0735, -0.3926, 8.8216, 8.2297, -0.2783, 9.0100, 8.2297, -0.2783, 9.0100, 8.2297, -0.2783, 9.0100, 8.2297, -0.2783, 9.0100, 8.2297, -0.2783, 9.0100, 8.2297, -0.2783, 9.0100, 8.2297, -0.2783, 9.0100, 8.1965, -0.2002, 9.0784, 8.1965, -0.2002, 9.0784, 8.1965, -0.2002, 9.0784, 8.1965, -0.2002, 9.0784, 8.1965, -0.2002, 9.0784, 8.1965, -0.2002, 9.0784, 8.1965, -0.2002, 9.0784, 8.2766, -0.3233, 9.0149, 8.2766, -0.3233, 9.0149, 8.2766, -0.3233, 9.0149, 8.2766, -0.3233, 9.0149, 8.2766, -0.3233, 9.0149, 8.2766, -0.3233, 9.0149, 8.2766, -0.3233, 9.0149, 8.1751, -0.0449, 9.1282, 8.1751, -0.0449, 9.1282, 8.1751, -0.0449, 9.1282, 8.1751, -0.0449, 9.1282, 8.1751, -0.0449, 9.1282, 8.1751, -0.0449, 9.1282, 8.1751, -0.0449, 9.1282, 8.0413, -0.3731, 8.9348, 8.0413, -0.3731, 8.9348, 8.0413, -0.3731, 8.9348, 8.0413, -0.3731, 8.9348, 8.0413, -0.3731, 8.9348, 8.0413, -0.3731, 8.9348, 8.0413, -0.3731, 8.9348, 8.1585, -0.1914, 8.8841, 8.1585, -0.1914, 8.8841, 8.1585, -0.1914, 8.8841, 8.1585, -0.1914, 8.8841, 8.1585, -0.1914, 8.8841, 8.1585, -0.1914, 8.8841, 8.1585, -0.1914, 8.8841, 7.9661, -0.1992, 8.8694, 7.9661, -0.1992, 8.8694, 7.9661, -0.1992, 8.8694, 7.9661, -0.1992, 8.8694, 7.9661, -0.1992, 8.8694, 7.9661, -0.1992, 8.8694, 7.9661, -0.1992, 8.8694, 8.1819, -0.3711, 8.8694, 8.1819, -0.3711, 8.8694, 8.1819, -0.3711, 8.8694, 8.1819, -0.3711, 8.8694, 8.1819, -0.3711, 8.8694, 8.1819, -0.3711, 8.8694, 8.1819, -0.3711, 8.8694, 8.1438, -0.0947, 8.9925, 8.1438, -0.0947, 8.9925, 8.1438, -0.0947, 8.9925, 8.1438, -0.0947, 8.9925, 8.1438, -0.0947, 8.9925, 8.1438, -0.0947, 8.9925, 8.1438, -0.0947, 8.9925, 8.1702, -0.3125, 9.0999, 8.1702, -0.3125, 9.0999, 8.1702, -0.3125, 9.0999, 8.1702, -0.3125, 9.0999, 8.1702, -0.3125, 9.0999, 8.1702, -0.3125, 9.0999, 8.1702, -0.3125, 9.0999, 8.1702, -0.3125, 9.0999, 8.3235, -0.0957, 9.0891, 8.3235, -0.0957, 9.0891, 8.3235, -0.0957, 9.0891, 8.3235, -0.0957, 9.0891, 8.3235, -0.0957, 9.0891, 8.3235, -0.0957, 9.0891, 8.3235, -0.0957, 9.0891, 8.0315, -0.1387, 9.0305, 8.0315, -0.1387, 9.0305, 8.0315, -0.1387, 9.0305, 8.0315, -0.1387, 9.0305, 8.0315, -0.1387, 9.0305, 8.0315, -0.1387, 9.0305, 8.0315, -0.1387, 9.0305, 8.1145, -0.3633, 8.8548, 8.1145, -0.3633, 8.8548, 8.1145, -0.3633, 8.8548, 8.1145, -0.3633, 8.8548, 8.1145, -0.3633, 8.8548, 8.1145, -0.3633, 8.8548, 8.1145, -0.3633, 8.8548, 8.0540, -0.0772, 8.8196, 8.0540, -0.0772, 8.8196, 8.0540, -0.0772, 8.8196, 8.0540, -0.0772, 8.8196, 8.0540, -0.0772, 8.8196, 8.0540, -0.0772, 8.8196, 8.0540, -0.0772, 8.8196, 8.0081, -0.3555, 8.8704, 8.0081, -0.3555, 8.8704, 8.0081, -0.3555, 8.8704, 8.0081, -0.3555, 8.8704, 8.0081, -0.3555, 8.8704, 8.0081, -0.3555, 8.8704, 8.0081, -0.3555, 8.8704, 8.1936, -0.1689, 9.0013, 8.1936, -0.1689, 9.0013, 8.1936, -0.1689, 9.0013, 8.1936, -0.1689, 9.0013, 8.1936, -0.1689, 9.0013
// };


int16_t static g_in16AudioDataBuffer[SAMPLE_RATE]; // 1s
bool volatile static g_audioRecording = false;
bool volatile static g_audioReady = false;

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

ns_audio_config_t audio_config = {
    .eAudioApiMode = NS_AUDIO_API_CALLBACK,
    .callback = audio_frame_callback,
    .audioBuffer = (void *)&g_in16AudioDataBuffer,
    .eAudioSource = NS_AUDIO_SOURCE_AUDADC,
    .numChannels = NUM_CHANNELS,
    .numSamples = SAMPLE_RATE,
    .sampleRate = SAMPLE_RATE,
    .audioSystemHandle = NULL, // filled in by audio_init()
    .bufferHandle = NULL
};

/// Button global - will be set by neuralSPOT button helper
int volatile g_buttonPressed = 0;

///Button Peripheral Config Struct
ns_button_config_t button_config = {
    .button_0_enable = true,
    .button_1_enable = false,
    .button_0_flag = &g_buttonPressed,
    .button_1_flag = NULL
};

#define WAITING_TO_RECORD 0
#define RECORDING 1
#define INFERING 2

int main(int argc, char **argv) {
    
    signal_t signal;            // Wrapper for raw input buffer
    ei_impulse_result_t result; // Used to store inference output
    EI_IMPULSE_ERROR res;       // Return code from inference

    am_hal_interrupt_master_enable();
    ns_malloc_init();
    ns_timer_init(0);
    ns_peripheral_button_init(&button_config);
    ns_audio_init(&audio_config);

    ns_itm_printf_enable();
    ns_debug_printf_enable();

    // Calculate the length of the buffer
    size_t buf_len = sizeof(input_buf) / sizeof(input_buf[0]);

    // Make sure that the length of the buffer matches expected input length
    if (buf_len != EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
        ns_printf("ERROR: The size of the input buffer is not correct.\r\n");
        ns_printf("Expected %d items, but got %d\r\n", 
                EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, 
                (int)buf_len);
        return 1;
    }

    // Assign callback function to fill buffer used for preprocessing/inference
    signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
    signal.get_data = &get_signal_data;
                
    ns_printf("Press Button0 to start listening for 1s\n");

    uint8_t state = WAITING_TO_RECORD;
    while(1) {
        // Perform DSP pre-processing and inference
        // Collect a 1s sample

        switch (state) {
        case WAITING_TO_RECORD:
            if (g_buttonPressed) {
                g_buttonPressed = false;
                ns_printf("Listening for 1s\n");
                g_audioReady = false;
                g_audioRecording = true;
                state = RECORDING;
            }
            break;
        case RECORDING:
            if (g_audioReady) {
                state = INFERING;
                ns_printf("Captured 1s of audio\n");

                // don't turn off g_audioReady until we're done with the buffer
            }
            break;

        case INFERING:
            res = run_classifier(&signal, &result, false);

            // Print return code and how long it took to perform inference
            ns_printf("run_classifier returned: %d\r\n", res);
            ns_printf("Timing: DSP %d ms, inference %d ms, anomaly %d ms\r\n", 
                    result.timing.dsp, 
                    result.timing.classification, 
                    result.timing.anomaly);

            // Print the prediction results (object detection)
            #if EI_CLASSIFIER_OBJECT_DETECTION == 1
                ns_printf("Object detection bounding boxes:\r\n");
                for (uint32_t i = 0; i < EI_CLASSIFIER_OBJECT_DETECTION_COUNT; i++) {
                    ei_impulse_result_bounding_box_t bb = result.bounding_boxes[i];
                    if (bb.value == 0) {
                        continue;
                    }
                    ns_printf("  %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\r\n", 
                            bb.label, 
                            bb.value, 
                            bb.x, 
                            bb.y, 
                            bb.width, 
                            bb.height);
                }

            // Print the prediction results (classification)
            #else
                ns_printf("Predictions:\r\n");
                for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
                    ns_printf("  %s: ", ei_classifier_inferencing_categories[i]);
                    ns_printf("%.5f\r\n", result.classification[i].value);
                }
            #endif

            // Print anomaly result (if it exists)
            #if EI_CLASSIFIER_HAS_ANOMALY == 1
                ns_printf("Anomaly prediction: %.3f\r\n", result.anomaly);
            #endif
            g_audioReady = false;
            state = WAITING_TO_RECORD;
            break;
        } // switch
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);

    } // while
    return 0;
}

// Callback: fill a section of the out_ptr buffer when requested
static int get_signal_data(size_t offset, size_t length, float *out_ptr) {
    numpy::int16_to_float(&g_in16AudioDataBuffer[offset], out_ptr, length);

    // for (size_t i = 0; i < length; i++) {
    //     out_ptr[i] = (input_buf + offset)[i];
    // }
    // ns_printf("AccX[%d] = %f\n", offset, out_ptr[0]);
    return EIDSP_OK;
}