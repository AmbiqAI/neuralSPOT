#include <stdio.h>

#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_malloc.h"
#include "ns_timer.h"
#include "ns_i2c_register_driver.h"
#include "ns_mpu6050_i2c_driver.h"
#include "ns_core.h"
#include "ns_peripherals_button.h"
#include "ns_peripherals_power.h"

// Callback function declaration
static int get_signal_data(size_t offset, size_t length, float *out_ptr);

static float input_buf[600]; 

#define AXES 3
#define MPUREADLEN 32

ns_i2c_config_t i2cConfig = {
    .api = &ns_i2c_V1_0_0,
    .iom = 1};

uint32_t mpuAddr = MPU_I2CADDRESS_AD0_LOW;

mpu6050_config_t mpu_config = {.clock_src = CLOCK_GZ_PLL,
                                .dlpf_cfg = DLPF_044HZ,
                                .gyro_fullscale_range = GYRO_FS_500DPS,
                                .accel_fullscale_range = ACCEL_FS_4G,
                                .sample_rate = 100,
                                .sleep_cfg = 0};

ns_timer_config_t ei_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};

// Button Peripheral Config
int volatile g_intButtonPressed = 0;
ns_button_config_t button_config = {.api = &ns_button_V1_0_0,
                                    .button_0_enable = true,
                                    .button_1_enable = false,
                                    .button_0_flag = &g_intButtonPressed,
                                    .button_1_flag = NULL};

static void read_1_second() {
    int t, axis;
    int16_t accelVals[AXES];

    for (t=0; t<200; t++) {
        mpu6050_get_accel_values(&i2cConfig, mpuAddr, &accelVals[0], &accelVals[1], &accelVals[2]);
        for (axis = 0; axis<AXES; axis++) {
            // input_buf[axis+t*AXES] = accelVals[axis];
            input_buf[axis+t*AXES] = mpu6050_accel_to_gravity(accelVals[axis], ACCEL_FS_4G);
        }
        ns_delay_us(5000);
    }
}

int main(int argc, char **argv) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    signal_t signal;            // Wrapper for raw input buffer
    ei_impulse_result_t result; // Used to store inference output
    EI_IMPULSE_ERROR res;       // Return code from inference

    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\b");
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed\n");
    ns_interrupt_master_enable();
    ns_itm_printf_enable();
	NS_TRY(ns_timer_init(&ei_tickTimer), "Timer init failed.\n");
    NS_TRY(ns_peripheral_button_init(&button_config), "Button init failed\n");

    ns_lp_printf("Please avoid moving sensor until calibration is finished (~20s).\n");
    ns_lp_printf("Press Button 0 to begin calibration\n");
    while (g_intButtonPressed == 0) {
        ns_deep_sleep();
    }
    g_intButtonPressed = 0;
    ns_lp_printf("Running calibration...\n");
    NS_TRY(ns_i2c_interface_init(&i2cConfig, 100000), "i2c Interface Init Failed.\n");
    NS_TRY(mpu6050_init(&i2cConfig, &mpu_config, mpuAddr), "MPU6050 Init Failed.\n");
    NS_TRY(mpu6050_calibrate(&i2cConfig, mpuAddr), "MPU6050 Calibration Failed.\n");
    ns_malloc_init();
    ns_lp_printf("Calibration Successful.\n");

    // Calculate the length of the buffer
    size_t buf_len = sizeof(input_buf) / sizeof(input_buf[0]);

    // Make sure that the length of the buffer matches expected input length
    if (buf_len != EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
        ns_lp_printf("ERROR: The size of the input buffer is not correct.\r\n");
        ns_lp_printf("Expected %d items, but got %d\r\n", 
                EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, 
                (int)buf_len);
        return 1;
    }

    // Assign callback function to fill buffer used for preprocessing/inference
    signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
    signal.get_data = &get_signal_data;

    while(1) {
        // Perform DSP pre-processing and inference
        // Collect a 1s sample

        read_1_second();

        res = run_classifier(&signal, &result, false);

        // Print return code and how long it took to perform inference
        ns_lp_printf("run_classifier returned: %d\r\n", res);
        ns_lp_printf("Timing: DSP %d ms, inference %d ms, anomaly %d ms\r\n", 
                result.timing.dsp, 
                result.timing.classification, 
                result.timing.anomaly);

        // Print the prediction results (object detection)
        #if EI_CLASSIFIER_OBJECT_DETECTION == 1
            ns_lp_printf("Object detection bounding boxes:\r\n");
            for (uint32_t i = 0; i < EI_CLASSIFIER_OBJECT_DETECTION_COUNT; i++) {
                ei_impulse_result_bounding_box_t bb = result.bounding_boxes[i];
                if (bb.value == 0) {
                    continue;
                }
                ns_lp_printf("  %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\r\n", 
                        bb.label, 
                        bb.value, 
                        bb.x, 
                        bb.y, 
                        bb.width, 
                        bb.height);
            }

        // Print the prediction results (classification)
        #else
            ns_lp_printf("Predictions:\r\n");
            for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
                ns_lp_printf("  %s: ", ei_classifier_inferencing_categories[i]);
                ns_lp_printf("%.5f\r\n", result.classification[i].value);
            }
        #endif

        // Print anomaly result (if it exists)
        #if EI_CLASSIFIER_HAS_ANOMALY == 1
            ns_lp_printf("Anomaly prediction: %.3f\r\n", result.anomaly);
        #endif
    }
    return 0;
}

// Callback: fill a section of the out_ptr buffer when requested
static int get_signal_data(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        out_ptr[i] = (input_buf + offset)[i];
    }
    // ns_lp_printf("AccX[%d] = %f\n", offset, out_ptr[0]);
    return EIDSP_OK;
}