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

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "button.h"

int g_intButtonPressed = 0;

// Button stuff
//*****************************************************************************
//
// GPIO ISR
//
//*****************************************************************************
void
am_gpio0_001f_isr(void)
{
    uint32_t ui32IntStatus;

    //
    // Delay for button debounce.
    //
    am_hal_delay_us(20000);

    //
    // Clear the GPIO Interrupt (write to clear).
    //
    AM_CRITICAL_BEGIN
    am_hal_gpio_interrupt_irq_status_get(GPIO0_001F_IRQn, true, &ui32IntStatus);
    am_hal_gpio_interrupt_irq_clear(GPIO0_001F_IRQn, ui32IntStatus);
    AM_CRITICAL_END

    g_intButtonPressed = 1;
}

void
button_init(void)
{
    uint32_t ui32IntStatus;
    uint32_t ui32BUTTON0GpioNum = AM_BSP_GPIO_BUTTON0;

    //
    // Configure the button pin.
    //
    am_hal_gpio_pinconfig(AM_BSP_GPIO_BUTTON0, g_AM_BSP_GPIO_BUTTON0);

    //
    // Clear the GPIO Interrupt (write to clear).
    //
    AM_CRITICAL_BEGIN
    am_hal_gpio_interrupt_irq_status_get(GPIO0_001F_IRQn, false, &ui32IntStatus);
    am_hal_gpio_interrupt_irq_clear(GPIO0_001F_IRQn, ui32IntStatus);
    AM_CRITICAL_END

    //
    // Enable the GPIO/button interrupt.
    //
    am_hal_gpio_interrupt_control(AM_HAL_GPIO_INT_CHANNEL_0,
                                  AM_HAL_GPIO_INT_CTRL_INDV_ENABLE,
                                  (void *)&ui32BUTTON0GpioNum);
    
    NVIC_SetPriority(GPIO0_001F_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(GPIO0_001F_IRQn);
    am_hal_interrupt_master_enable();

    g_intButtonPressed = 0;
}