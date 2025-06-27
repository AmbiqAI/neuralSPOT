/**
 @file imc.cc

 @brief ICM45606 IMU Example

 HAR is a model trained using the scripts found here:
https://github.com/AmbiqAI/Human-Activity-Recognition

**/

#include <math.h>

#include "ns_spi.h"
/// NeuralSPOT Includes
#include "ns_ambiqsuite_harness.h"
#include "ns_core.h"
#include "ns_imu.h"
#include "imu/inv_imu_driver.h"


#include "ns_peripherals_power.h"


ns_imu_sensor_data_t imu_frame[20];

void my_frame_available_cb(void *arg) {
    ns_imu_config_t *cfg = (ns_imu_config_t *)arg;
    ns_lp_printf("Frame available\n");
    // Print out the frame
    for (int i = 0; i < cfg->frame_size; i++) {
        ns_lp_printf("Frame %d: Accel: %f %f %f, Gyro: %f %f %f, Temp: %f\n",
                     i,
                     cfg->frame_buffer[i].accel_g[0], cfg->frame_buffer[i].accel_g[1], cfg->frame_buffer[i].accel_g[2],
                     cfg->frame_buffer[i].gyro_dps[0], cfg->frame_buffer[i].gyro_dps[1], cfg->frame_buffer[i].gyro_dps[2],
                     cfg->frame_buffer[i].temp_degc);
    }
}

int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    ns_imu_config_t  imu_cfg = {
        .api = &ns_imu_V1_0_0,
        .sensor = NS_IMU_SENSOR_ICM45605,
        #ifdef AM_PART_APOLLO5B
        .iom    = 0,
        #else
        .iom    = 1, // IOM1 for Apollo4P
        #endif
        .accel_fsr = ACCEL_CONFIG0_ACCEL_UI_FS_SEL_4_G,
        .gyro_fsr  = GYRO_CONFIG0_GYRO_UI_FS_SEL_2000_DPS,
        .accel_odr = ACCEL_CONFIG0_ACCEL_ODR_50_HZ,
        .gyro_odr  = GYRO_CONFIG0_GYRO_ODR_50_HZ,
        .accel_ln_bw = IPREG_SYS2_REG_131_ACCEL_UI_LPFBW_DIV_4,
        .gyro_ln_bw  = IPREG_SYS1_REG_172_GYRO_UI_LPFBW_DIV_4,
        .calibrate = true,
        .frame_available_cb = my_frame_available_cb,
        .frame_size = sizeof(imu_frame) / sizeof(ns_imu_sensor_data_t),
        .frame_buffer = imu_frame
    };
    ns_imu_sensor_data_t imu_data;

    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed.\n");
    NS_TRY(ns_set_performance_mode(NS_MINIMUM_PERF), "Set CPU Perf mode failed.");
    ns_itm_printf_enable();
    ns_lp_printf("Ready to test IMC\n"); 
    NS_TRY(ns_imu_configure(&imu_cfg), "IMU Init Failed.\n");
    ns_lp_printf("IMU Init Success\n");

    // Main loop
    while (1) {
        ns_deep_sleep();
    }
    
}
