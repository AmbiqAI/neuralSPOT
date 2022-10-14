#include <stdio.h>

#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_malloc.h"
#include "ns_timer.h"
#include "ns_i2c_register_driver.h"
#include "ns_mpu6050_i2c_driver.h"

// Callback function declaration
static int get_signal_data(size_t offset, size_t length, float *out_ptr);

// Raw features copied from test sample (Edge Impulse > Model testing)
static float input_buf[600]; //= {
//     /* Paste your raw features here! */
// 8.3245, -0.1543, 9.0725, 8.3245, -0.1543, 9.0725, 8.3245, -0.1543, 9.0725, 8.3245, -0.1543, 9.0725, 8.1145, -0.1777, 9.0442, 8.1145, -0.1777, 9.0442, 8.1145, -0.1777, 9.0442, 8.1145, -0.1777, 9.0442, 8.1145, -0.1777, 9.0442, 8.1145, -0.1777, 9.0442, 8.1145, -0.1777, 9.0442, 8.1145, -0.1777, 9.0442, 8.0823, -0.4580, 8.9446, 8.0823, -0.4580, 8.9446, 8.0823, -0.4580, 8.9446, 8.0823, -0.4580, 8.9446, 8.0823, -0.4580, 8.9446, 8.0823, -0.4580, 8.9446, 8.0823, -0.4580, 8.9446, 8.1126, -0.1123, 8.8303, 8.1126, -0.1123, 8.8303, 8.1126, -0.1123, 8.8303, 8.1126, -0.1123, 8.8303, 8.1126, -0.1123, 8.8303, 8.1126, -0.1123, 8.8303, 8.1126, -0.1123, 8.8303, 7.9856, -0.3379, 8.9036, 7.9856, -0.3379, 8.9036, 7.9856, -0.3379, 8.9036, 7.9856, -0.3379, 8.9036, 7.9856, -0.3379, 8.9036, 7.9856, -0.3379, 8.9036, 7.9856, -0.3379, 8.9036, 8.2258, -0.4404, 8.9475, 8.2258, -0.4404, 8.9475, 8.2258, -0.4404, 8.9475, 8.2258, -0.4404, 8.9475, 8.2258, -0.4404, 8.9475, 8.2258, -0.4404, 8.9475, 8.2258, -0.4404, 8.9475, 8.1565, -0.1387, 9.0647, 8.1565, -0.1387, 9.0647, 8.1565, -0.1387, 9.0647, 8.1565, -0.1387, 9.0647, 8.1565, -0.1387, 9.0647, 8.1565, -0.1387, 9.0647, 8.1565, -0.1387, 9.0647, 8.2356, -0.4561, 9.0755, 8.2356, -0.4561, 9.0755, 8.2356, -0.4561, 9.0755, 8.2356, -0.4561, 9.0755, 8.2356, -0.4561, 9.0755, 8.2356, -0.4561, 9.0755, 8.2356, -0.4561, 9.0755, 8.2991, -0.1631, 9.1009, 8.2991, -0.1631, 9.1009, 8.2991, -0.1631, 9.1009, 8.2991, -0.1631, 9.1009, 8.2991, -0.1631, 9.1009, 8.2991, -0.1631, 9.1009, 8.2991, -0.1631, 9.1009, 8.0256, -0.2891, 9.0540, 8.0256, -0.2891, 9.0540, 8.0256, -0.2891, 9.0540, 8.0256, -0.2891, 9.0540, 8.0256, -0.2891, 9.0540, 8.0256, -0.2891, 9.0540, 8.0256, -0.2891, 9.0540, 8.1770, -0.3799, 8.7913, 8.1770, -0.3799, 8.7913, 8.1770, -0.3799, 8.7913, 8.1770, -0.3799, 8.7913, 8.1770, -0.3799, 8.7913, 8.1770, -0.3799, 8.7913, 8.1770, -0.3799, 8.7913, 8.0549, -0.1055, 8.8128, 8.0549, -0.1055, 8.8128, 8.0549, -0.1055, 8.8128, 8.0549, -0.1055, 8.8128, 8.0549, -0.1055, 8.8128, 8.0549, -0.1055, 8.8128, 8.0549, -0.1055, 8.8128, 8.0735, -0.3926, 8.8216, 8.0735, -0.3926, 8.8216, 8.0735, -0.3926, 8.8216, 8.0735, -0.3926, 8.8216, 8.0735, -0.3926, 8.8216, 8.0735, -0.3926, 8.8216, 8.0735, -0.3926, 8.8216, 8.2297, -0.2783, 9.0100, 8.2297, -0.2783, 9.0100, 8.2297, -0.2783, 9.0100, 8.2297, -0.2783, 9.0100, 8.2297, -0.2783, 9.0100, 8.2297, -0.2783, 9.0100, 8.2297, -0.2783, 9.0100, 8.1965, -0.2002, 9.0784, 8.1965, -0.2002, 9.0784, 8.1965, -0.2002, 9.0784, 8.1965, -0.2002, 9.0784, 8.1965, -0.2002, 9.0784, 8.1965, -0.2002, 9.0784, 8.1965, -0.2002, 9.0784, 8.2766, -0.3233, 9.0149, 8.2766, -0.3233, 9.0149, 8.2766, -0.3233, 9.0149, 8.2766, -0.3233, 9.0149, 8.2766, -0.3233, 9.0149, 8.2766, -0.3233, 9.0149, 8.2766, -0.3233, 9.0149, 8.1751, -0.0449, 9.1282, 8.1751, -0.0449, 9.1282, 8.1751, -0.0449, 9.1282, 8.1751, -0.0449, 9.1282, 8.1751, -0.0449, 9.1282, 8.1751, -0.0449, 9.1282, 8.1751, -0.0449, 9.1282, 8.0413, -0.3731, 8.9348, 8.0413, -0.3731, 8.9348, 8.0413, -0.3731, 8.9348, 8.0413, -0.3731, 8.9348, 8.0413, -0.3731, 8.9348, 8.0413, -0.3731, 8.9348, 8.0413, -0.3731, 8.9348, 8.1585, -0.1914, 8.8841, 8.1585, -0.1914, 8.8841, 8.1585, -0.1914, 8.8841, 8.1585, -0.1914, 8.8841, 8.1585, -0.1914, 8.8841, 8.1585, -0.1914, 8.8841, 8.1585, -0.1914, 8.8841, 7.9661, -0.1992, 8.8694, 7.9661, -0.1992, 8.8694, 7.9661, -0.1992, 8.8694, 7.9661, -0.1992, 8.8694, 7.9661, -0.1992, 8.8694, 7.9661, -0.1992, 8.8694, 7.9661, -0.1992, 8.8694, 8.1819, -0.3711, 8.8694, 8.1819, -0.3711, 8.8694, 8.1819, -0.3711, 8.8694, 8.1819, -0.3711, 8.8694, 8.1819, -0.3711, 8.8694, 8.1819, -0.3711, 8.8694, 8.1819, -0.3711, 8.8694, 8.1438, -0.0947, 8.9925, 8.1438, -0.0947, 8.9925, 8.1438, -0.0947, 8.9925, 8.1438, -0.0947, 8.9925, 8.1438, -0.0947, 8.9925, 8.1438, -0.0947, 8.9925, 8.1438, -0.0947, 8.9925, 8.1702, -0.3125, 9.0999, 8.1702, -0.3125, 9.0999, 8.1702, -0.3125, 9.0999, 8.1702, -0.3125, 9.0999, 8.1702, -0.3125, 9.0999, 8.1702, -0.3125, 9.0999, 8.1702, -0.3125, 9.0999, 8.1702, -0.3125, 9.0999, 8.3235, -0.0957, 9.0891, 8.3235, -0.0957, 9.0891, 8.3235, -0.0957, 9.0891, 8.3235, -0.0957, 9.0891, 8.3235, -0.0957, 9.0891, 8.3235, -0.0957, 9.0891, 8.3235, -0.0957, 9.0891, 8.0315, -0.1387, 9.0305, 8.0315, -0.1387, 9.0305, 8.0315, -0.1387, 9.0305, 8.0315, -0.1387, 9.0305, 8.0315, -0.1387, 9.0305, 8.0315, -0.1387, 9.0305, 8.0315, -0.1387, 9.0305, 8.1145, -0.3633, 8.8548, 8.1145, -0.3633, 8.8548, 8.1145, -0.3633, 8.8548, 8.1145, -0.3633, 8.8548, 8.1145, -0.3633, 8.8548, 8.1145, -0.3633, 8.8548, 8.1145, -0.3633, 8.8548, 8.0540, -0.0772, 8.8196, 8.0540, -0.0772, 8.8196, 8.0540, -0.0772, 8.8196, 8.0540, -0.0772, 8.8196, 8.0540, -0.0772, 8.8196, 8.0540, -0.0772, 8.8196, 8.0540, -0.0772, 8.8196, 8.0081, -0.3555, 8.8704, 8.0081, -0.3555, 8.8704, 8.0081, -0.3555, 8.8704, 8.0081, -0.3555, 8.8704, 8.0081, -0.3555, 8.8704, 8.0081, -0.3555, 8.8704, 8.0081, -0.3555, 8.8704, 8.1936, -0.1689, 9.0013, 8.1936, -0.1689, 9.0013, 8.1936, -0.1689, 9.0013, 8.1936, -0.1689, 9.0013, 8.1936, -0.1689, 9.0013
// };

#define AXES 3
#define MPUREADLEN 32

static void read_1_second() {
    int t, axis;
    uint8_t buffer[MPUREADLEN];
    int16_t accelX, accelY, accelZ; //, gyroX, gyroY, gyroZ, temperature_regval;
    // float temperature = 0.0;
    float accelVals[AXES];
    // float gyroVals[AXES];

    for (t=0; t<200; t++) {
        read_sensors(buffer);
        
        // decode the buffer
        accelX = buffer[0] << 8 | buffer[1];
        accelY = buffer[2] << 8 | buffer[3];
        accelZ = buffer[4] << 8 | buffer[5];

        // Not used by this model
        // temperature_regval = (buffer[6]<<8 | buffer[7]);
        // temperature = temperature_regval/340.0+36.53;

        // gyroX = buffer[8] << 8 | buffer[9];
        // gyroY = buffer[10] << 8 | buffer[11];
        // gyroZ = buffer[12] << 8 | buffer[13];

        accelGravity(accelVals, accelX, accelY, accelZ, ACCEL_FS_16G);
        // gyroDegPerSec(gyroVals, gyroX, gyroY, gyroZ, GYRO_FS_500DPS);

        // Capture data in input_buf buffer
        for (axis = 0; axis<AXES; axis++) {
            input_buf[axis+t*AXES] = accelVals[axis];
        }

        ns_delay_us(5000);
    }
}


int main(int argc, char **argv) {
    
    signal_t signal;            // Wrapper for raw input buffer
    ei_impulse_result_t result; // Used to store inference output
    EI_IMPULSE_ERROR res;       // Return code from inference
    void *mpuHandle;

    am_hal_interrupt_master_enable();
    ns_malloc_init();
    ns_timer_init(0);
    ns_i2c_interface_init(1, MPU_I2CADDRESS_AD0_LOW, &mpuHandle);
    mpu6050_finish_init(mpuHandle);
    if(mpu6050_calibration()) {
        ns_printf("Calibration Failed!\n");
        return 1;
    }
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

    while(1) {
        // Perform DSP pre-processing and inference
        // Collect a 1s sample

        read_1_second();

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
    }
    return 0;
}

// Callback: fill a section of the out_ptr buffer when requested
static int get_signal_data(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        out_ptr[i] = (input_buf + offset)[i];
    }
    ns_printf("AccX[%d] = %f\n", offset, out_ptr[0]);
    return EIDSP_OK;
}