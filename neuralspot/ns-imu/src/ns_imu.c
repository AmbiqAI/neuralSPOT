/**
 * @file ns_imu.c
 * @author Ambiq
 * @brief Library for IMU sensors
 * @version 0.1
 * @date 2025-05-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "ns_imu.h"
#include "ns_imu_icm45605_driver.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_core.h"
#include <string.h>

const ns_core_api_t ns_imu_V0_0_1 = {.apiId = NS_IMU_API_ID, .version = NS_IMU_V0_0_1};
const ns_core_api_t ns_imu_V1_0_0 = {.apiId = NS_IMU_API_ID, .version = NS_IMU_V1_0_0};
const ns_core_api_t ns_imu_oldest_supported_version = {
    .apiId = NS_IMU_API_ID, .version = NS_IMU_V0_0_1};
const ns_core_api_t ns_imu_current_version = {
    .apiId = NS_IMU_API_ID, .version = NS_IMU_V1_0_0};

// Internal State
ns_spi_config_t static ns_imu_spi_config;
ns_imu_config_t static ns_imu_config; // config struct, needed for ISR


uint32_t ns_imu_configure(ns_imu_config_t *cfg) {

    // Check if the configuration is valid
    if (cfg == NULL) {
        ns_lp_printf("NS_IMU: Config cannot be null \n");
        return NS_STATUS_INVALID_CONFIG;
    }

    if (cfg->sensor != NS_IMU_SENSOR_ICM45605) {
        ns_lp_printf("NS_IMU: Sensor not supported\n");
        return NS_STATUS_INVALID_CONFIG;
    }

#ifdef AM_PART_APOLLO5B
    if (cfg->iom != 0) {
        ns_lp_printf("NS_IMU: SPI port must be 0 for AP510 EVB, %d not supported\n", cfg->iom);
        return NS_STATUS_INVALID_CONFIG;
    }
#endif

    ns_imu_spi_config.iom = cfg->iom;

#ifdef AM_PART_APOLLO5B
    // For AP510 Mikroe Board is IOM0, which is currently not initialized well in
    // the HAL. So we need to do it manually.

    // Configure and init SPI0  
    am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_IOM0);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_IOM0_SCK_CB,  g_AM_BSP_GPIO_IOM0_SCK_CB);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_IOM0_MISO_CB, g_AM_BSP_GPIO_IOM0_MISO_CB);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_IOM0_MOSI_CB, g_AM_BSP_GPIO_IOM0_MOSI_CB);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_IOM0_CS,      g_AM_BSP_GPIO_IOM0_CS);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_RST_CB,       g_AM_BSP_GPIO_RST_CB); 
    am_hal_gpio_state_write(AM_BSP_GPIO_RST_CB,     AM_HAL_GPIO_OUTPUT_SET);
#endif

    ns_spi_interface_init(&ns_imu_spi_config, AM_HAL_IOM_8MHZ, AM_HAL_IOM_SPI_MODE_3);
    cfg->spi_cfg = &ns_imu_spi_config;
    ns_delay_us(8000);

    // Call the IMU driver init function
    if (cfg->sensor == NS_IMU_SENSOR_ICM45605) {
        ns_imu_ICM45605_init(cfg);
    } else {
        ns_lp_printf("NS_IMU: Sensor not supported\n");
    }

    // Store the config in the global variable
    memcpy(&ns_imu_config, cfg, sizeof(ns_imu_config_t));
    return NS_STATUS_SUCCESS;
}


uint32_t ns_imu_get_data(ns_imu_config_t *cfg, ns_imu_sensor_data_t *data) {
    
    // Read data from the specified sensor
    if (cfg->sensor == NS_IMU_SENSOR_ICM45605) {
        ns_imu_ICM_45606_get_data(cfg, data);
        return NS_STATUS_SUCCESS;
    } else {
        ns_lp_printf("ICM45605: Sensor not supported\n");
    }
    return NS_STATUS_FAILURE;
}