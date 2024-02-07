//*****************************************************************************
//
//! @file button.c
//!
//! @brief Utility for reading EVB Buttons.
//!
//! Purpose: Reading EVB buttons
//!
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

extern int volatile *g_ns_peripheral_button0;
extern int volatile *g_ns_peripheral_button1;
extern int volatile *g_ns_peripheral_joulescope_trigger;
extern void ns_button_0_handler(void *pArg);
extern void ns_button_1_handler(void *pArg);

/**
 * @brief GPIO Button0 ISR handler
 *
 */
void am_gpio_isr(void) {
    // Read and clear the GPIO interrupt status.
#if defined(AM_PART_APOLLO3P)
    AM_HAL_GPIO_MASKCREATE(GpioIntStatusMask);
    am_hal_gpio_interrupt_status_get(false, pGpioIntStatusMask);
    am_hal_gpio_interrupt_clear(pGpioIntStatusMask);
    am_hal_gpio_interrupt_service(pGpioIntStatusMask);
#elif defined(AM_PART_APOLLO3)
    uint64_t ui64Status;
    am_hal_gpio_interrupt_status_get(false, &ui64Status);
    am_hal_gpio_interrupt_clear(ui64Status);
    am_hal_gpio_interrupt_service(ui64Status);
#else
    #error Unknown device.
#endif
}

uint32_t ns_button_platform_init(ns_button_config_t *cfg) {
    // uint32_t ui32IntStatus;
    // uint32_t ui32BUTTON0GpioNum = AM_BSP_GPIO_BUTTON0;
    // uint32_t ui32BUTTON1GpioNum = AM_BSP_GPIO_BUTTON1;
    // uint32_t ui32JoulescopeTriggerGpioNum = 24;

    // Configure the button pin.
    if (cfg->button_0_enable) {
        am_hal_gpio_pinconfig(AM_BSP_GPIO_BUTTON0, g_AM_BSP_GPIO_BUTTON0);
        g_ns_peripheral_button0 = cfg->button_0_flag;
    }
    if (cfg->button_1_enable) {
        am_hal_gpio_pinconfig(AM_BSP_GPIO_BUTTON1, g_AM_BSP_GPIO_BUTTON1);
        g_ns_peripheral_button1 = cfg->button_1_flag;
    }
    // if (cfg->joulescope_trigger_enable) {
    //     am_hal_gpio_pinconfig(24, am_hal_gpio_pincfg_input);
    //     g_ns_peripheral_joulescope_trigger = cfg->joulescope_trigger_flag;
    // }

    // Register interrupt handlers
    if (cfg->button_0_enable) {
        AM_HAL_GPIO_MASKCREATE(GpioIntMask0);
        am_hal_gpio_interrupt_register(
            AM_BSP_GPIO_BUTTON0, (am_hal_gpio_handler_t)ns_button_0_handler);
        am_hal_gpio_interrupt_clear(AM_HAL_GPIO_MASKBIT(pGpioIntMask0, AM_BSP_GPIO_BUTTON0));
        am_hal_gpio_interrupt_enable(AM_HAL_GPIO_MASKBIT(pGpioIntMask0, AM_BSP_GPIO_BUTTON0));
    }
    if (cfg->button_1_enable) {
        AM_HAL_GPIO_MASKCREATE(GpioIntMask1);
        am_hal_gpio_interrupt_register(
            AM_BSP_GPIO_BUTTON1, (am_hal_gpio_handler_t)ns_button_1_handler);
        am_hal_gpio_interrupt_clear(AM_HAL_GPIO_MASKBIT(pGpioIntMask1, AM_BSP_GPIO_BUTTON1));
        am_hal_gpio_interrupt_enable(AM_HAL_GPIO_MASKBIT(pGpioIntMask1, AM_BSP_GPIO_BUTTON1));
    }
    // if (cfg->joulescope_trigger_enable) {
    //     am_hal_gpio_interrupt_register(
    //         AM_HAL_GPIO_INT_CHANNEL_0, 24, (am_hal_gpio_handler_t)ns_joulescope_trigger_handler,
    //         NULL);
    //     am_hal_gpio_interrupt_control(
    //         AM_HAL_GPIO_INT_CHANNEL_0, AM_HAL_GPIO_INT_CTRL_INDV_ENABLE,
    //         (void *)&ui32JoulescopeTriggerGpioNum);
    // }

    NVIC_EnableIRQ(GPIO_IRQn);

    am_hal_interrupt_master_enable();
    return NS_STATUS_SUCCESS;
}
