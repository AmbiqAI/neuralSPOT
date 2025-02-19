/**
 * @file ns_ambiqsuite_harness.h
 * @author Carlos Morales
 * @brief Abstract common AmbiqSuite function calls
 * @version 0.1
 * @date 2022-08-09
 *
 * @copyright Copyright (c) 2022
 *
 * \addtogroup harness
 * @{
 *
 */

#ifndef NS_AMBIQSUITE_HARNESS_H
    #define NS_AMBIQSUITE_HARNESS_H

    #ifdef __cplusplus
extern "C" {
    #endif

    #if defined(AM_PART_APOLLO3) || defined(AM_PART_APOLLO3P)
        #define am_hal_gpio_pincfg_output g_AM_HAL_GPIO_OUTPUT
    #endif

    #include "am_bsp.h"
    #include "am_mcu_apollo.h"
    #include "am_util.h"
    #include "ns_core.h"
    #include "ns_timer.h"
    #include <am_util_stdio.h>

    #define ns_debug_printf_enable am_bsp_debug_printf_enable
    #define ns_uart_printf_disable am_bsp_uart_printf_disable
    #define ns_itm_printf_disable                                                                  \
        g_ns_state.itmPrintCurrentlyEnabled = false;                                               \
        am_bsp_itm_printf_disable
    #define ns_debug_printf_disable am_bsp_debug_printf_disable
    #define ns_interrupt_master_enable am_hal_interrupt_master_enable
    #define ns_interrupt_master_disable am_hal_interrupt_master_disable

    #define ns_printf ns_lp_printf

    #if defined(AM_PART_APOLLO3) || defined(AM_PART_APOLLO3P) || defined(AM_PART_APOLLO5A) || defined(AM_PART_APOLLO5B)
        #define ns_itm_printf_enable am_bsp_itm_printf_enable
        #define ns_lp_printf am_util_stdio_printf
        #define ns_delay_us am_util_delay_us
        // SRAM is default for AP3
        #if defined(AM_PART_APOLLO3) || defined(AM_PART_APOLLO3P)
            #define NS_PUT_IN_TCM __attribute__((section(".tcm")))
            #define AM_SHARED_RW
        #else // AM_PART_APOLLO3
            #define NS_PUT_IN_TCM
        #endif
    #else
        #define ns_delay_us am_hal_delay_us
        // TCM is default for AP4
        #define NS_PUT_IN_TCM

/**
 * @brief Enable ITM prints without needed Crypto to be enabled
 *
 */
extern void ns_itm_printf_enable(void);

/**
 * @brief Low power print that enables only the needed hardware only while needed
 *
 * @param format
 * @param ...
 */
extern void ns_lp_printf(const char *format, ...);

    #endif // not AM_PART_APOLLO3, etc
/**
 * @brief Enable UART prints in power-sensitive way
 *
 */
extern void ns_uart_printf_enable(void);

/**
 * @brief Disable prints enabled by ns_itm_printf_enable()
 *
 * @return int32_t
 */
extern int32_t ns_cryptoless_itm_printf_disable(void);

    #ifdef __cplusplus
}
    #endif // __cplusplus
#endif // NS_AMBIQSUITE_HARNESS_H
    /** @} */
/** @} */
