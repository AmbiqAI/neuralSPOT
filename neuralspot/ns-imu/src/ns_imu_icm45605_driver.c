/**
 * @file ns_imu_icm45605.c.dnc
 * @author Ambiq
 * @brief ICM45605 driver
 * @version 0.1
 * @date 2025-05-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "ns_imu.h"
#include "imu/inv_imu_driver.h"
#include "ns_imu_icm45605_driver.h"
#include "ns_ambiqsuite_harness.h"

// Internal State
inv_imu_device_t static ns_imu_icm45605_dev;
ns_spi_config_t static *ns_imu_icm45606_spi_config;

int static ns_imu_icm45605_read_reg(uint8_t reg, uint8_t *data, uint32_t len) {
    return ns_spi_read(ns_imu_icm45606_spi_config, data, len, reg | 0x80, 1, 0);
}
int static ns_imu_icm45605_write_reg(uint8_t reg, const uint8_t *data, uint32_t len) {
    return ns_spi_write(ns_imu_icm45606_spi_config, data, len, reg, 1, 0);
}
void static ns_imu_icm45605_sleep_us(uint32_t us) {
    ns_delay_us(us);
}

uint32_t ns_imu_ICM45605_init(ns_imu_config_t *cfg) {
    uint8_t whoami;

    /* Transport layer initialization */
    cfg->imu_dev_handle = &ns_imu_icm45605_dev;
    ns_imu_icm45606_spi_config = cfg->spi_cfg;

    // Set up the transport layer
    ns_imu_icm45605_dev.transport.read_reg   = ns_imu_icm45605_read_reg;
    ns_imu_icm45605_dev.transport.write_reg  = ns_imu_icm45605_write_reg;
    ns_imu_icm45605_dev.transport.sleep_us   = ns_imu_icm45605_sleep_us;
    ns_imu_icm45605_dev.transport.serif_type = UI_SPI4;

    // Check that device is present
    inv_imu_get_who_am_i(&ns_imu_icm45605_dev, &whoami);
    if (whoami != INV_IMU_WHOAMI) {
        ns_lp_printf("IMU WHOAMI: 0x%02X (expected 0x%02X)\n", whoami, INV_IMU_WHOAMI);
        return NS_STATUS_FAILURE;
    }

    // Trigger soft-reset
    inv_imu_soft_reset(&ns_imu_icm45605_dev);
    ns_delay_us(1000);

    // Set FSR
    inv_imu_set_accel_fsr(&ns_imu_icm45605_dev, cfg->accel_fsr);
    inv_imu_set_gyro_fsr(&ns_imu_icm45605_dev, cfg->gyro_fsr);
    inv_imu_set_accel_frequency(&ns_imu_icm45605_dev, cfg->accel_odr);
    inv_imu_set_gyro_frequency(&ns_imu_icm45605_dev, cfg->gyro_odr);
    inv_imu_set_accel_ln_bw(&ns_imu_icm45605_dev, cfg->accel_ln_bw);
    inv_imu_set_gyro_ln_bw(&ns_imu_icm45605_dev, cfg->gyro_ln_bw);
	inv_imu_select_accel_lp_clk(&ns_imu_icm45605_dev, SMC_CONTROL_0_ACCEL_LP_CLK_RCOSC);
    inv_imu_set_accel_mode(&ns_imu_icm45605_dev, PWR_MGMT0_ACCEL_MODE_LN); // LP?
    inv_imu_set_gyro_mode(&ns_imu_icm45605_dev, PWR_MGMT0_GYRO_MODE_LN); // LP?

    return NS_STATUS_SUCCESS;
};

/**
 * @brief Retrieve 6DOF data from configured IMU
 * 
 * @param cfg Config struct
 * @param data Data retrieved in floating point natural units (g, dps, degC)
 * @return uin32_t status
 */
uint32_t ns_imu_ICM_45606_get_data(ns_imu_config_t *cfg, ns_imu_sensor_data_t *data) {
    inv_imu_sensor_data_t d;
    
    inv_imu_get_register_data(cfg->imu_dev_handle, &d);
    data->accel_g[0]  = (float)(d.accel_data[0] * 4 /* gee */) / 32768;
    data->accel_g[1]  = (float)(d.accel_data[1] * 4 /* gee */) / 32768;
    data->accel_g[2]  = (float)(d.accel_data[2] * 4 /* gee */) / 32768;
    data->gyro_dps[0] = (float)(d.gyro_data[0] * 2000 /* dps */) / 32768;
    data->gyro_dps[1] = (float)(d.gyro_data[1] * 2000 /* dps */) / 32768;
    data->gyro_dps[2] = (float)(d.gyro_data[2] * 2000 /* dps */) / 32768;
    data->temp_degc   = (float)25 + ((float)d.temp_data / 128);
    return NS_STATUS_SUCCESS;
}