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

/**
 * @brief GPIO ISR handler
 *
 */
void am_gpio0_001f_isr(void) {
    uint32_t ui32IntStatus;
    // Clear the GPIO Interrupt (write to clear).
    AM_CRITICAL_BEGIN
    am_hal_gpio_interrupt_irq_status_get(GPIO0_001F_IRQn, true, &ui32IntStatus);
    am_hal_gpio_interrupt_irq_clear(GPIO0_001F_IRQn, ui32IntStatus);
    AM_CRITICAL_END
    am_hal_gpio_interrupt_service(GPIO0_001F_IRQn, ui32IntStatus);
}

// #ifndef NS_GPIO0_203F_IRQn
// #define NS_GPIO0_203F_IRQn GPIO0_203F_IRQn
// void am_gpio0_203f_isr(void) {
//     uint32_t ui32IntStatus;
//     // Clear the GPIO Interrupt (write to clear).
//     AM_CRITICAL_BEGIN
//     am_hal_gpio_interrupt_irq_status_get(GPIO0_203F_IRQn, true, &ui32IntStatus);
//     am_hal_gpio_interrupt_irq_clear(GPIO0_203F_IRQn, ui32IntStatus);
//     AM_CRITICAL_END
//     am_hal_gpio_interrupt_service(GPIO0_203F_IRQn, ui32IntStatus);
// }
// #endif

void am_gpio0_405f_isr(void) {
    uint32_t ui32IntStatus;
    // Clear the GPIO Interrupt (write to clear).
    AM_CRITICAL_BEGIN
    am_hal_gpio_interrupt_irq_status_get(GPIO0_405F_IRQn, true, &ui32IntStatus);
    am_hal_gpio_interrupt_irq_clear(GPIO0_405F_IRQn, ui32IntStatus);
    AM_CRITICAL_END
    am_hal_gpio_interrupt_service(GPIO0_405F_IRQn, ui32IntStatus);
}

extern int volatile *g_ns_peripheral_button0;
extern int volatile *g_ns_peripheral_button1;
extern int volatile *g_ns_peripheral_joulescope_trigger;
extern void ns_button_0_handler(void *pArg);
extern void ns_button_1_handler(void *pArg);
extern void ns_joulescope_trigger_handler(void *pArg);

uint32_t ns_button_platform_init(ns_button_config_t *cfg) {
    uint32_t ui32IntStatus;
    #if defined(apollo510b_evb)
    uint32_t ui32JoulescopeTriggerGpioNum = 5;
    #else
    uint32_t ui32JoulescopeTriggerGpioNum = 14;
    #endif
    // am_hal_gpio_mask_t GpioIntMask = AM_HAL_GPIO_MASK_DECLARE_ZERO;
    uint32_t GpioIntMask = 0;

    // APOLLO5A_TODO - Apollo5A EB does not have buttons

#if defined(apollo510_evb) || defined(apollo510b_evb)
    uint32_t ui32BUTTON0GpioNum = AM_BSP_GPIO_BUTTON0;
    uint32_t ui32BUTTON1GpioNum = AM_BSP_GPIO_BUTTON1;
    // Configure the button pin.
    if (cfg->button_0_enable) {
        am_hal_gpio_pinconfig(ui32BUTTON0GpioNum, g_AM_BSP_GPIO_BUTTON0);
        g_ns_peripheral_button0 = cfg->button_0_flag;
    }
    if (cfg->button_1_enable) {
        am_hal_gpio_pinconfig(ui32BUTTON1GpioNum, g_AM_BSP_GPIO_BUTTON1);
        g_ns_peripheral_button1 = cfg->button_1_flag;
    }
#endif
    if (cfg->joulescope_trigger_enable) {
        am_hal_gpio_pinconfig(ui32JoulescopeTriggerGpioNum, am_hal_gpio_pincfg_input);
        g_ns_peripheral_joulescope_trigger = cfg->joulescope_trigger_flag;
    }

    // // Clear the GPIO Interrupt (write to clear).
    AM_CRITICAL_BEGIN
    am_hal_gpio_interrupt_irq_status_get(GPIO0_001F_IRQn, false, &GpioIntMask);
    am_hal_gpio_interrupt_irq_clear(GPIO0_001F_IRQn, GpioIntMask);
    am_hal_gpio_interrupt_irq_status_get(GPIO0_203F_IRQn, false, &GpioIntMask);
    am_hal_gpio_interrupt_irq_clear(GPIO0_203F_IRQn, GpioIntMask);
    am_hal_gpio_interrupt_irq_status_get(GPIO0_405F_IRQn, false, &GpioIntMask);
    am_hal_gpio_interrupt_irq_clear(GPIO0_405F_IRQn, GpioIntMask);
    AM_CRITICAL_END

#if defined(apollo510_evb) || defined(apollo510b_evb)
    // Register interrupt handlers
    if (cfg->button_0_enable) {
        // uint32_t ui32IntStatus;
        ui32IntStatus = am_hal_gpio_interrupt_register(
            AM_HAL_GPIO_INT_CHANNEL_0, ui32BUTTON0GpioNum,
            (am_hal_gpio_handler_t)ns_button_0_handler, NULL);
        // ns_lp_printf("Button 0 interrupt registered. Status %d\n", ui32IntStatus);
        ui32IntStatus = am_hal_gpio_interrupt_control(
            AM_HAL_GPIO_INT_CHANNEL_0, AM_HAL_GPIO_INT_CTRL_INDV_ENABLE,
            (void *)&ui32BUTTON0GpioNum);
        // ns_lp_printf("Button 0 interrupt enabled. Status %d\n", ui32IntStatus);
    }
    if (cfg->button_1_enable) {
        am_hal_gpio_interrupt_register(
            AM_HAL_GPIO_INT_CHANNEL_0, ui32BUTTON1GpioNum,
            (am_hal_gpio_handler_t)ns_button_1_handler, NULL);
        am_hal_gpio_interrupt_control(
            AM_HAL_GPIO_INT_CHANNEL_0, AM_HAL_GPIO_INT_CTRL_INDV_ENABLE,
            (void *)&ui32BUTTON1GpioNum);
    }
#endif
    if (cfg->joulescope_trigger_enable) {
        am_hal_gpio_interrupt_register(
            AM_HAL_GPIO_INT_CHANNEL_0, ui32JoulescopeTriggerGpioNum, (am_hal_gpio_handler_t)ns_joulescope_trigger_handler,
            NULL);
        am_hal_gpio_interrupt_control(
            AM_HAL_GPIO_INT_CHANNEL_0, AM_HAL_GPIO_INT_CTRL_INDV_ENABLE,
            (void *)&ui32JoulescopeTriggerGpioNum);
    }

    NVIC_SetPriority(GPIO0_001F_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_SetPriority(GPIO0_203F_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_SetPriority(GPIO0_405F_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(GPIO0_001F_IRQn);
    NVIC_EnableIRQ(GPIO0_203F_IRQn);
    NVIC_EnableIRQ(GPIO0_405F_IRQn);

    am_hal_interrupt_master_enable();
    return NS_STATUS_SUCCESS;
}
