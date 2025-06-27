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

uint32_t ns_calibrate_icm45605(ns_imu_config_t *cfg);

uint32_t ns_imu_ICM45605_init(ns_imu_config_t *cfg) {
    uint8_t whoami;
    uint32_t status = NS_STATUS_SUCCESS;
    if (cfg == NULL) {
        ns_lp_printf("ns_imu_ICM45605_init: Invalid handle\n");
        return NS_STATUS_INVALID_HANDLE;
    }

    /* Transport layer initialization */
    cfg->imu_dev_handle = &ns_imu_icm45605_dev;
    ns_imu_icm45606_spi_config = cfg->spi_cfg;

    // Set up the transport layer
    ns_imu_icm45605_dev.transport.read_reg   = ns_imu_icm45605_read_reg;
    ns_imu_icm45605_dev.transport.write_reg  = ns_imu_icm45605_write_reg;
    ns_imu_icm45605_dev.transport.sleep_us   = ns_imu_icm45605_sleep_us;
    ns_imu_icm45605_dev.transport.serif_type = UI_SPI4;

    // Check that device is present
    status = inv_imu_get_who_am_i(&ns_imu_icm45605_dev, &whoami);
    if (whoami != INV_IMU_WHOAMI) {
        ns_lp_printf("IMU WHOAMI: 0x%02X (expected 0x%02X)\n", whoami, INV_IMU_WHOAMI);
        return NS_STATUS_FAILURE;
    }

    // Trigger soft-reset
    status |= inv_imu_soft_reset(&ns_imu_icm45605_dev);
    ns_delay_us(1000);

    // Set FSR
    status |= inv_imu_set_accel_fsr(&ns_imu_icm45605_dev, cfg->accel_fsr);
    status |= inv_imu_set_gyro_fsr(&ns_imu_icm45605_dev, cfg->gyro_fsr);
    status |= inv_imu_set_accel_frequency(&ns_imu_icm45605_dev, cfg->accel_odr);
    status |= inv_imu_set_gyro_frequency(&ns_imu_icm45605_dev, cfg->gyro_odr);
    status |= inv_imu_set_accel_ln_bw(&ns_imu_icm45605_dev, cfg->accel_ln_bw);
    status |= inv_imu_set_gyro_ln_bw(&ns_imu_icm45605_dev, cfg->gyro_ln_bw);
	status |= inv_imu_select_accel_lp_clk(&ns_imu_icm45605_dev, SMC_CONTROL_0_ACCEL_LP_CLK_RCOSC);
    status |= inv_imu_set_accel_mode(&ns_imu_icm45605_dev, PWR_MGMT0_ACCEL_MODE_LN); // LP?
    status |= inv_imu_set_gyro_mode(&ns_imu_icm45605_dev, PWR_MGMT0_GYRO_MODE_LN); // LP?

    if (cfg->calibrate) {
        ns_lp_printf("NS_IMU: Calibrating ICM-45605\n");
        status |= ns_calibrate_icm45605(cfg);
    } else {
        cfg->calibrated = 0; // set calibrated flag
    }

        // If callback is set, configure interrupt. Invoker is responsible for INT pin setup.
    if (cfg->frame_available_cb != NULL) {
        cfg->frame_size = cfg->frame_size ? cfg->frame_size : 1;
        status |= ns_imu_ICM45605_configure_interrupts(cfg);
    }

    return status;
};

float ns_imu_accel_map[] = {32, 16, 8, 4, 2};
float ns_imu_gyro_map[] = {4000, 2000, 1000, 500, 250, 125, 62.5, 31.25, 15.625};


/**
 * @brief Retrieve 6DOF data from configured IMU
 * 
 * @param cfg Config struct
 * @param data Data retrieved in floating point natural units (g, dps, degC)
 * @return uin32_t status
 */
uint32_t ns_imu_ICM_45606_get_data(ns_imu_config_t *cfg, ns_imu_sensor_data_t *data) {
    inv_imu_sensor_data_t d;
    uint32_t status = NS_STATUS_SUCCESS;
    if (cfg == NULL || data == NULL) {
        ns_lp_printf("Invalid handle or data pointer\n");
        return NS_STATUS_INVALID_HANDLE;
    }

    status = inv_imu_get_register_data(cfg->imu_dev_handle, &d);

    // Scale the data to natural units
    for (int i = 0; i < 3; i++) {
        data->accel_g[i]  = (float)(d.accel_data[i] * ns_imu_accel_map[cfg->accel_fsr]) / 32768;
        data->gyro_dps[i] = (float)(d.gyro_data[i] * ns_imu_gyro_map[cfg->gyro_fsr]) / 32768;
        if (cfg->calibrated) {
            data->accel_g[i]  -= cfg->accel_bias[i];
            data->gyro_dps[i] -= cfg->gyro_bias[i];
        }
    }
    data->temp_degc = (float)25 + ((float)d.temp_data / 128);
    return status;
}

uint32_t ns_imu_ICM_45606_get_raw_data(ns_imu_config_t *cfg, ns_imu_sensor_data_t *data) {
    inv_imu_sensor_data_t d;
    if (cfg == NULL || data == NULL) {
        ns_lp_printf("Invalid handle or data pointer\n");
        return NS_STATUS_INVALID_HANDLE;
    }
    if (NS_STATUS_SUCCESS != inv_imu_get_register_data(cfg->imu_dev_handle, &d)) {
        ns_lp_printf("NS_IMU: Failed to get raw data from ICM-45605\n");
        return NS_STATUS_FAILURE;
    }
    // Copy raw data to the output structure
    for (int i = 0; i < 3; i++) {
        data->accel_g[i]  = (float)d.accel_data[i]; // raw data in LSB
        data->gyro_dps[i] = (float)d.gyro_data[i];  // raw data in LSB
    }
    data->temp_degc = (float)d.temp_data; // raw data in LSB
    return NS_STATUS_SUCCESS;
}

uint32_t ns_imu_ICM45605_configure_interrupts(ns_imu_config_t *cfg) {
	inv_imu_int_pin_config_t int_pin_config;
	inv_imu_int_state_t      int_config;
    uint32_t status;
    if (cfg == NULL || cfg->imu_dev_handle == NULL) {
        ns_lp_printf("Invalid handle\n");
        return NS_STATUS_INVALID_HANDLE;
    }
    // ns_lp_printf("NS_IMU ICM: Configuring GPIO interrupt\n");
	int_pin_config.int_polarity = INTX_CONFIG2_INTX_POLARITY_HIGH;
	int_pin_config.int_mode     = INTX_CONFIG2_INTX_MODE_PULSE;
	int_pin_config.int_drive    = INTX_CONFIG2_INTX_DRIVE_PP;
	status = inv_imu_set_pin_config_int(cfg->imu_dev_handle, INV_IMU_INT2, &int_pin_config);
    if (status != NS_STATUS_SUCCESS) {
        ns_lp_printf("NS_IMU ICM: Failed to set pin config for INT2\n");
        return status;
    }
	/* Interrupts configuration */
	memset(&int_config, INV_IMU_DISABLE, sizeof(int_config));
	int_config.INV_UI_DRDY = INV_IMU_ENABLE;
	status = inv_imu_set_config_int(cfg->imu_dev_handle, INV_IMU_INT2, &int_config);

    return status;
}

uint32_t ns_imu_ICM_45605_handle_interrupt(void) {
    inv_imu_int_state_t int_state;
    inv_imu_get_int_status(&ns_imu_icm45605_dev, INV_IMU_INT2, &int_state);
    return int_state.INV_UI_DRDY ? 1 : 0;
}


/**
 *  Calibrate ICM-45605 by averaging 250 samples.
 *  While stationary, accel should read [0,0,1g] and gyro [0,0,0].
 *
 *  @param cfg    pointer to your ns_imu_config_t
 *  @param calib  out: computed biases
 *  @return NS_STATUS_SUCCESS or error
 */
uint32_t ns_calibrate_icm45605(ns_imu_config_t *cfg)
{

    uint32_t       count             = 0;
    double         sum_acc[3]        = {0,0,0};
    double         sum_gyro[3]       = {0,0,0};
    ns_imu_sensor_data_t d;

    // Get rid of some garbage data
    for (int i = 0; i < 10; i++) {
        if (ns_imu_ICM_45606_get_data(cfg, &d) != NS_STATUS_SUCCESS) {
            return NS_STATUS_FAILURE;
        }
        // ns_lp_printf("Data %d: Accel: %f, Gyro: %f\n", count, d.accel_g[2], d.gyro_dps[2]);
        ns_delay_us(20000); // 20ms delay to get 50Hz
    }

    while (count < 250) {
        if (ns_imu_ICM_45606_get_data(cfg, &d) != NS_STATUS_SUCCESS) {
            return NS_STATUS_FAILURE;
        }
        sum_acc[0]  += d.accel_g[0];
        sum_acc[1]  += d.accel_g[1];
        sum_acc[2]  += d.accel_g[2];
        sum_gyro[0] += d.gyro_dps[0];
        sum_gyro[1] += d.gyro_dps[1];
        sum_gyro[2] += d.gyro_dps[2];
        // ns_lp_printf("Data %d: Accel: %f, Gyro: %f\n", count, d.accel_g[2], d.gyro_dps[2]);
        count++;
        ns_delay_us(20000); // 20ms delay to get 50Hz
    }

    // compute and store biases
    for (int i = 0; i < 3; i++) {
        float avg_acc  = sum_acc[i]  / count;
        float avg_gyro = sum_gyro[i] / count;
        // X/Y accel bias = avg;  Z accel bias = (avg − 1 g)
        cfg->accel_bias[i] = avg_acc - (i==2 ? 1.0f : 0.0f);
        cfg->gyro_bias[i]  = avg_gyro;
        // ns_lp_printf("Bias %d: Accel: %f, Gyro: %f\n", i, cfg->accel_bias[i], cfg->gyro_bias[i]);
    }
    cfg->calibrated = 1; // set calibrated flag
    return NS_STATUS_SUCCESS;
}

