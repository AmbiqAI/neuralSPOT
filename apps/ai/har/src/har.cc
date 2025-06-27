/**
 * @file har.cc
 *
 * @brief Continuous Human Activity Recognition using ICM-45605 IMU (NeuralSPOT)
 *
 * This example continuously samples the IMU and performs inference on 10s windows
 * with a 2s overlap (i.e., inference every 8s).
 **/


#include "ns_spi.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_core.h"
#include "ns_peripherals_power.h"
#include "ns_imu.h"
#include "imu/inv_imu_driver.h"
#include "ns_perf_profile.h"
#include "ns_peripherals_button.h"

#include "har_model.h"

/// State machine for HAR example
typedef enum { INFERENCE_PENDING, IDLE } myState_e;

/// IMU sampling parameters: 20Hz
#define SAMPLE_RATE_HZ      20
#define WINDOW_DURATION_S   10
#define OVERLAP_DURATION_S   2
#define WINDOW_SIZE        (SAMPLE_RATE_HZ * WINDOW_DURATION_S)   // 200 samples
#define STEP_SIZE          (SAMPLE_RATE_HZ * (WINDOW_DURATION_S - OVERLAP_DURATION_S)) // 160 samples (8s)
#define MPU_AXES             6

/// Circular buffer for sliding window
static float g_sensorData[WINDOW_SIZE][MPU_AXES];
static int buffer_head = 0;
static bool window_filled = false;

/// Chunk buffer for incoming STEP_SIZE samples
static ns_imu_sensor_data_t imu_chunk[STEP_SIZE];
volatile bool chunk_ready = false;

/// Stats for normalization
static float g_sensorMean[MPU_AXES];
static float g_sensorStd[MPU_AXES];

/// Labels
constexpr size_t kCategoryCount = 6;
const char *kCategoryLabels[kCategoryCount] = {
    "Walking", "Jogging", "Stairs", "Sitting", "Standing"
};

/// Callback when a chunk of STEP_SIZE samples is ready
void imu_frame_available_cb(void *arg) {
    ns_imu_config_t *cfg = (ns_imu_config_t *)arg;
    memcpy(imu_chunk, cfg->frame_buffer, sizeof(imu_chunk));
    chunk_ready = true;
}

/// Append new chunk into circular buffer and update head/index
void append_chunk_to_buffer(void) {
    for (int i = 0; i < STEP_SIZE; i++) {
        int idx = (buffer_head + i) % WINDOW_SIZE;
        // accel
        g_sensorData[idx][0] = imu_chunk[i].accel_g[0];
        g_sensorData[idx][1] = imu_chunk[i].accel_g[1];
        g_sensorData[idx][2] = imu_chunk[i].accel_g[2];
        // gyro
        g_sensorData[idx][3] = imu_chunk[i].gyro_dps[0];
        g_sensorData[idx][4] = imu_chunk[i].gyro_dps[1];
        g_sensorData[idx][5] = imu_chunk[i].gyro_dps[2];
    }
    buffer_head = (buffer_head + STEP_SIZE) % WINDOW_SIZE;
    // After enough samples, buffer is full
    if (!window_filled && buffer_head == 0) {
        window_filled = true;
    }
}

/// Compute mean and std deviation over WINDOW_SIZE samples
void compute_stats(void) {
    // reset
    for (int axis = 0; axis < MPU_AXES; axis++) {
        g_sensorMean[axis] = 0.0f;
    }
    // accumulate mean
    for (int i = 0; i < WINDOW_SIZE; i++) {
        for (int axis = 0; axis < MPU_AXES; axis++) {
            g_sensorMean[axis] += g_sensorData[i][axis];
        }
    }
    for (int axis = 0; axis < MPU_AXES; axis++) {
        g_sensorMean[axis] /= WINDOW_SIZE;
    }
    // std dev
    for (int axis = 0; axis < MPU_AXES; axis++) {
        float var = 0.0f;
        for (int i = 0; i < WINDOW_SIZE; i++) {
            float diff = g_sensorData[i][axis] - g_sensorMean[axis];
            var += diff * diff;
        }
        g_sensorStd[axis] = sqrtf(var / WINDOW_SIZE);
    }
}

/// Run inference on current buffer
void run_inference(void) {
    // prepare tensor input
    int tensorIndex = 0;
    for (int i = 0; i < WINDOW_SIZE; i++) {
        for (int axis = 0; axis < MPU_AXES; axis++) {
            float norm = (g_sensorData[i][axis] - g_sensorMean[axis]) / g_sensorStd[axis];
            norm = norm / model_input->params.scale + model_input->params.zero_point;
            norm = MAX(MIN(norm, 127), -128);
            model_input->data.int8[tensorIndex++] = (int8_t)norm;
        }
    }
    // invoke
    TfLiteStatus status = interpreter->Invoke();
    if (status != kTfLiteOk) {
        ns_lp_printf("Inference failed\n");
        return;
    }
    // output
    float max_val = 0.0f;
    uint8_t best = 0;
    for (uint8_t i = 0; i < kCategoryCount; i++) {
        float score = (model_output->data.int8[i] - model_output->params.zero_point) *
                      model_output->params.scale;
        if (score > 0.3f) {
            ns_lp_printf("[%s] with %d%% certainty\n", kCategoryLabels[i], (uint8_t)(score * 100));
        }
        if (score > max_val) {
            max_val = score;
            best = i;
        }
    }
    ns_lp_printf("**** Most probable: [%s]\n", kCategoryLabels[best]);
}

int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed.\n");
    NS_TRY(ns_set_performance_mode(NS_MINIMUM_PERF), "Set CPU Perf mode failed.\n");
    ns_itm_printf_enable();

    // configure IMU for STEP_SIZE-chunks
    ns_imu_config_t imu_cfg = {
        .api               = &ns_imu_V1_0_0,
        .sensor            = NS_IMU_SENSOR_ICM45605,
        #ifdef AM_PART_APOLLO4P
        .iom               = 1, // IOM1 for Apollo4P
        #else
        .iom               = 0, // IOM1 for Apollo4L
        #endif
        .accel_fsr         = ACCEL_CONFIG0_ACCEL_UI_FS_SEL_4_G,
        .gyro_fsr          = GYRO_CONFIG0_GYRO_UI_FS_SEL_2000_DPS,
        .accel_odr         = ACCEL_CONFIG0_ACCEL_ODR_50_HZ,
        .gyro_odr          = GYRO_CONFIG0_GYRO_ODR_50_HZ,
        .accel_ln_bw       = IPREG_SYS2_REG_131_ACCEL_UI_LPFBW_DIV_4,
        .gyro_ln_bw        = IPREG_SYS1_REG_172_GYRO_UI_LPFBW_DIV_4,
        .calibrate         = true,
        .frame_available_cb= imu_frame_available_cb,
        .frame_size        = STEP_SIZE,
        .frame_buffer      = imu_chunk
    };
    NS_TRY(ns_imu_configure(&imu_cfg), "IMU Init Failed.\n");
    ns_lp_printf("Continuous HAR: 10s windows with 2s overlap\n");

    // init model
    model_init();
    ns_interrupt_master_enable();

    myState_e state = IDLE;

    while (1) {
        if (chunk_ready) {
            chunk_ready = false;
            append_chunk_to_buffer();
            if (window_filled) {
                compute_stats();
                run_inference();
            }
        }
        ns_deep_sleep();
    }
    return 0;
}
