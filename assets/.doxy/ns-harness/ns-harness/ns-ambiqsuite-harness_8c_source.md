

# File ns-ambiqsuite-harness.c

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-harness**](dir_e0d7b3aff6df2cba2f05a768a095730e.md) **>** [**src**](dir_8df671c8e5b7eec7f2ec532421bc80bd.md) **>** [**ns-ambiqsuite-harness.c**](ns-ambiqsuite-harness_8c.md)

[Go to the documentation of this file](ns-ambiqsuite-harness_8c.md)

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
#include "ns_ambiqsuite_harness.h"

void
ns_itm_printf_enable(void) {
    // Enable crypto only as long as needed
    if (g_ns_state.cryptoCurrentlyEnabled == false) {
        am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_CRYPTO);
    }
    am_bsp_itm_printf_enable();

    if (g_ns_state.cryptoWantsToBeEnabled == false) {
        am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_CRYPTO_POWERDOWN, 0);
    }

    g_ns_state.itmPrintCurrentlyEnabled = true;
    g_ns_state.itmPrintWantsToBeEnabled = true;
}

void
ns_uart_printf_enable(void) {
    // Enable crypto only as long as needed
    am_bsp_uart_printf_enable();

    g_ns_state.uartPrintCurrentlyEnabled = true;
    g_ns_state.uartPrintWantsToBeEnabled = true;
}

//*****************************************************************************
//
//
//
//*****************************************************************************
int32_t
ns_cryptoless_itm_printf_enable(void) {
    // Enable the ITM interface and the SWO pin.
    am_hal_itm_enable();
    am_hal_tpiu_enable(AM_HAL_TPIU_BAUD_1M);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_ITM_SWO, g_AM_BSP_GPIO_ITM_SWO);
    am_util_stdio_printf_init(am_hal_itm_print);

    return 0;
}

//*****************************************************************************
//
//
//
//*****************************************************************************
int32_t
ns_cryptoless_itm_printf_disable(void) {
    // Disable the ITM/TPIU
    am_hal_itm_disable();

    // Detach the ITM interface from the STDIO driver.
    am_util_stdio_printf_init(0);

    // Disconnect the SWO pin
    am_hal_gpio_pinconfig(AM_BSP_GPIO_ITM_SWO, am_hal_gpio_pincfg_disabled);

    return 0;
}

void
ns_lp_printf(const char *format, ...) {
    va_list myargs;

    if ((g_ns_state.uartPrintWantsToBeEnabled == false) &&
        (g_ns_state.itmPrintWantsToBeEnabled == false)) {
        // No print interface has been enabled, ns_lp_printf is a no-op
        return;
    }

    if ((g_ns_state.uartPrintWantsToBeEnabled) && (g_ns_state.uartPrintCurrentlyEnabled == false)) {
        am_bsp_uart_printf_enable();
        g_ns_state.uartPrintCurrentlyEnabled = true;
    } else if ((g_ns_state.itmPrintWantsToBeEnabled == true) &&
               (g_ns_state.itmPrintCurrentlyEnabled == false)) {
        ns_cryptoless_itm_printf_enable();
        g_ns_state.itmPrintCurrentlyEnabled = true;
    }

    va_start(myargs, format);
    am_util_stdio_vprintf(format, myargs);
    va_end(myargs);
}

```
