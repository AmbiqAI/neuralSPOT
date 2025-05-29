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
#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
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
uint32_t static ns_imu_frame_buffer_index = 0;

// ISR for the IMU
void am_gpio0_203f_isr(void) {
    uint32_t ui32IntStatus;
    // Clear the GPIO Interrupt (write to clear).
    // ns_lp_printf("NS_IMU: GPIO ISR\n");
    AM_CRITICAL_BEGIN
    am_hal_gpio_interrupt_irq_status_get(GPIO0_203F_IRQn, true, &ui32IntStatus);
    am_hal_gpio_interrupt_irq_clear(GPIO0_203F_IRQn, ui32IntStatus);
    AM_CRITICAL_END
    am_hal_gpio_interrupt_service(GPIO0_203F_IRQn, ui32IntStatus);
}

void ns_imu_data_available_cb(void *pArg) {
    // Called when the IMU fires an interrupt
    ns_imu_sensor_data_t *data = &(ns_imu_config.frame_buffer[ns_imu_frame_buffer_index]);
    uint32_t data_available = ns_imu_ICM_45605_handle_interrupt();
    if (data_available == 0) {
        // ns_lp_printf("NS_IMU: No data available\n");
        return;
    }
    // ns_lp_printf("NS_IMU: Data available\n");
    // Read the data from the IMU
    if (ns_imu_get_data(&ns_imu_config, data) == NS_STATUS_SUCCESS) {
        // Accumulate the data in the buffer, and call the callback if buffer is full
        if (++ns_imu_frame_buffer_index >= ns_imu_config.frame_size) {
            ns_imu_frame_buffer_index = 0;
            ns_imu_config.frame_available_cb(&ns_imu_config);
        }
    } else {
        ns_lp_printf("NS_IMU: Failed to get data\n");
    }
}

uint32_t ns_imu_configure(ns_imu_config_t *cfg) {
    uint32_t imu_int_pin = 50; // GPIO pin for the IMU interrupt
    uint32_t GpioIntMask = 0;

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

    // If callback is set, configure int pin
    if (cfg->frame_available_cb != NULL) {
        ns_lp_printf("NS_IMU: Configuring GPIO interrupt\n");
        cfg->frame_size = cfg->frame_size ? cfg->frame_size : 1;
        am_hal_gpio_pinconfig(imu_int_pin,  am_hal_gpio_pincfg_input);
    }

    // Call the IMU driver init function
    if (cfg->sensor == NS_IMU_SENSOR_ICM45605) {
        if (NS_STATUS_SUCCESS != ns_imu_ICM45605_init(cfg)) {
            ns_lp_printf("NS_IMU: Failed to initialize ICM45605\n");
            return NS_STATUS_FAILURE;
        }
    } else {
        ns_lp_printf("NS_IMU: Sensor not supported\n");
    }

    // Enable the GPIO interrupt and register the data available callback
    if (cfg->frame_available_cb != NULL) {

        // Clear the GPIO Interrupt (write to clear).
        AM_CRITICAL_BEGIN
        am_hal_gpio_interrupt_irq_status_get(GPIO0_203F_IRQn, false, &GpioIntMask);
        am_hal_gpio_interrupt_irq_clear(GPIO0_203F_IRQn, GpioIntMask);
        AM_CRITICAL_END

        am_hal_gpio_interrupt_register(
            AM_HAL_GPIO_INT_CHANNEL_0, imu_int_pin,
            (am_hal_gpio_handler_t)ns_imu_data_available_cb, NULL);
        am_hal_gpio_interrupt_control(
            AM_HAL_GPIO_INT_CHANNEL_0, AM_HAL_GPIO_INT_CTRL_INDV_ENABLE,
            (void *)&imu_int_pin);

        NVIC_SetPriority(GPIO0_203F_IRQn, AM_IRQ_PRIORITY_DEFAULT);
        NVIC_EnableIRQ(GPIO0_203F_IRQn);
        am_hal_interrupt_master_enable();

    }
    // Store the config in the global variable
    memcpy(&ns_imu_config, cfg, sizeof(ns_imu_config_t));
    return NS_STATUS_SUCCESS;
}


uint32_t ns_imu_get_data(ns_imu_config_t *cfg, ns_imu_sensor_data_t *data) {
    
    // Read data from the specified sensor
    if (cfg->sensor == NS_IMU_SENSOR_ICM45605) {
        return ns_imu_ICM_45606_get_data(cfg, data);
    } else {
        ns_lp_printf("ICM45605: Sensor not supported\n");
    }
    return NS_STATUS_FAILURE;
}

uint32_t ns_imu_get_raw_data(ns_imu_config_t *cfg, ns_imu_sensor_data_t *data) {
    
    // Read data from the specified sensor
    if (cfg->sensor == NS_IMU_SENSOR_ICM45605) {
        return ns_imu_ICM_45606_get_raw_data(cfg, data);
    } else {
        ns_lp_printf("ICM45605: Sensor not supported\n");
    }
    return NS_STATUS_FAILURE;
}