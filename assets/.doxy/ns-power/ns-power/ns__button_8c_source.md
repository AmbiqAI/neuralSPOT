

# File ns\_button.c

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-peripherals**](dir_62cbd78784261bb0d09981988628a167.md) **>** [**src**](dir_cf9eff0d6bf97258df730d615fa0f132.md) **>** [**ns\_button.c**](ns__button_8c.md)

[Go to the documentation of this file](ns__button_8c.md)

```C++

//*****************************************************************************
//
//
//*****************************************************************************

//*****************************************************************************
//
// ${copyright}
//
// This is part of revision ${version} of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "ns_core.h"
#include "ns_peripherals_button.h"

const ns_core_api_t ns_button_V0_0_1 = {.apiId = NS_BUTTON_API_ID, .version = NS_BUTTON_V0_0_1};

const ns_core_api_t ns_button_V1_0_0 = {.apiId = NS_BUTTON_API_ID, .version = NS_BUTTON_V1_0_0};

const ns_core_api_t ns_button_oldest_supported_version = {
    .apiId = NS_BUTTON_API_ID, .version = NS_BUTTON_V0_0_1};

const ns_core_api_t ns_button_current_version = {
    .apiId = NS_BUTTON_API_ID, .version = NS_BUTTON_V1_0_0};

int volatile *g_ns_peripheral_button0;
int volatile *g_ns_peripheral_button1;
int volatile *g_ns_peripheral_joulescope_trigger;

void am_gpio0_001f_isr(void) {
    uint32_t ui32IntStatus;

    // Clear the GPIO Interrupt (write to clear).
    AM_CRITICAL_BEGIN
    am_hal_gpio_interrupt_irq_status_get(GPIO0_001F_IRQn, true, &ui32IntStatus);
    am_hal_gpio_interrupt_irq_clear(GPIO0_001F_IRQn, ui32IntStatus);
    AM_CRITICAL_END
    am_hal_gpio_interrupt_service(GPIO0_001F_IRQn, ui32IntStatus);

    // *g_ns_peripheral_button0 = 1;
}

void ns_button_0_handler(void *pArg) { *g_ns_peripheral_button0 = 1; }

void ns_button_1_handler(void *pArg) { *g_ns_peripheral_button1 = 1; }

void ns_joulescope_trigger_handler(void *pArg) { *g_ns_peripheral_joulescope_trigger = 1; }

uint32_t ns_peripheral_button_init(ns_button_config_t *cfg) {
    uint32_t ui32IntStatus;
    uint32_t ui32BUTTON0GpioNum = AM_BSP_GPIO_BUTTON0;
    uint32_t ui32BUTTON1GpioNum = AM_BSP_GPIO_BUTTON1;
    uint32_t ui32JoulescopeTriggerGpioNum = 24;

#ifndef NS_DISABLE_API_VALIDATION
    if (cfg == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }

    if (ns_core_check_api(
            cfg->api, &ns_button_oldest_supported_version, &ns_button_current_version)) {
        return NS_STATUS_INVALID_VERSION;
    }
#endif

    // Configure the button pin.
    if (cfg->button_0_enable) {
        am_hal_gpio_pinconfig(AM_BSP_GPIO_BUTTON0, g_AM_BSP_GPIO_BUTTON0);
        g_ns_peripheral_button0 = cfg->button_0_flag;
    }
    if (cfg->button_1_enable) {
        am_hal_gpio_pinconfig(AM_BSP_GPIO_BUTTON1, g_AM_BSP_GPIO_BUTTON1);
        g_ns_peripheral_button1 = cfg->button_1_flag;
    }
    if (cfg->joulescope_trigger_enable) {
        am_hal_gpio_pinconfig(24, am_hal_gpio_pincfg_input);
        g_ns_peripheral_joulescope_trigger = cfg->joulescope_trigger_flag;
    }

    // Clear the GPIO Interrupt (write to clear).
    AM_CRITICAL_BEGIN
    am_hal_gpio_interrupt_irq_status_get(GPIO0_001F_IRQn, false, &ui32IntStatus);
    am_hal_gpio_interrupt_irq_clear(GPIO0_001F_IRQn, ui32IntStatus);
    AM_CRITICAL_END

    // Register interrupt handlers
    if (cfg->button_0_enable) {
        am_hal_gpio_interrupt_register(
            AM_HAL_GPIO_INT_CHANNEL_0, AM_BSP_GPIO_BUTTON0,
            (am_hal_gpio_handler_t)ns_button_0_handler, NULL);
        am_hal_gpio_interrupt_control(
            AM_HAL_GPIO_INT_CHANNEL_0, AM_HAL_GPIO_INT_CTRL_INDV_ENABLE,
            (void *)&ui32BUTTON0GpioNum);
    }
    if (cfg->button_1_enable) {
        am_hal_gpio_interrupt_register(
            AM_HAL_GPIO_INT_CHANNEL_0, AM_BSP_GPIO_BUTTON1,
            (am_hal_gpio_handler_t)ns_button_1_handler, NULL);
        am_hal_gpio_interrupt_control(
            AM_HAL_GPIO_INT_CHANNEL_0, AM_HAL_GPIO_INT_CTRL_INDV_ENABLE,
            (void *)&ui32BUTTON1GpioNum);
    }
    if (cfg->joulescope_trigger_enable) {
        am_hal_gpio_interrupt_register(
            AM_HAL_GPIO_INT_CHANNEL_0, 24, (am_hal_gpio_handler_t)ns_joulescope_trigger_handler,
            NULL);
        am_hal_gpio_interrupt_control(
            AM_HAL_GPIO_INT_CHANNEL_0, AM_HAL_GPIO_INT_CTRL_INDV_ENABLE,
            (void *)&ui32JoulescopeTriggerGpioNum);
    }

    NVIC_SetPriority(GPIO0_001F_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(GPIO0_001F_IRQn);

    am_hal_interrupt_master_enable();
    return NS_STATUS_SUCCESS;
}

```

