/**
 * @file ns_timer_ap4.c
 * @author Carlos Morales
 * @brief Apollo4-specific timer implementation
 * @version 0.1
 * @date 2022-10-11
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ns_timer.h"
#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "ns_core.h"

extern ns_timer_config_t *ns_timer_config[NS_TIMER_TEMPCO + 1];

void am_timer01_isr(void) {
    ns_timers_e timerNum = ns_timer_config[1]->timer;
    //
    // Clear the timer Interrupt (write to clear).
    //
    am_hal_timer_interrupt_clear(AM_HAL_TIMER_MASK(timerNum, AM_HAL_TIMER_COMPARE1));
    am_hal_timer_clear(timerNum);

    ns_timer_config[1]->callback(ns_timer_config[1]);
}

void am_timer02_isr(void) {
    ns_timers_e timerNum = ns_timer_config[2]->timer;
    //
    // Clear the timer Interrupt (write to clear).
    //
    am_hal_timer_interrupt_clear(AM_HAL_TIMER_MASK(timerNum, AM_HAL_TIMER_COMPARE1));
    am_hal_timer_clear(timerNum);

    ns_timer_config[2]->callback(ns_timer_config[2]);
}

void am_timer03_isr(void) {
    ns_timers_e timerNum = ns_timer_config[3]->timer;
    //
    // Clear the timer Interrupt (write to clear).
    //
    am_hal_timer_interrupt_clear(AM_HAL_TIMER_MASK(timerNum, AM_HAL_TIMER_COMPARE1));
    am_hal_timer_clear(timerNum);

    ns_timer_config[2]->callback(ns_timer_config[3]);
}

uint32_t ns_timer_platform_init(ns_timer_config_t *cfg) {
    uint32_t ui32Status = AM_HAL_STATUS_SUCCESS;

#ifndef NS_DISABLE_API_VALIDATION
    // Apollo4-specific API validation
#endif

    am_hal_timer_config_t TimerConfig;

    // Set the timer configuration
    // The default timer configuration is HFRC_DIV16, EDGE, compares=0, no trig.
    am_hal_timer_default_config_set(&TimerConfig);

    // modify the default
    if (cfg->timer == NS_TIMER_TEMPCO) {
        TimerConfig.eInputClock = AM_HAL_TIMER_CLOCK_HFRC_DIV16;
    }

    if ((cfg->enableInterrupt)) {
        TimerConfig.eFunction = AM_HAL_TIMER_FN_UPCOUNT;
        TimerConfig.ui32Compare1 = cfg->periodInMicroseconds / 6; // 6 ticks per uS
    }

    ui32Status = am_hal_timer_config(cfg->timer, &TimerConfig);
    if (ui32Status != AM_HAL_STATUS_SUCCESS) {
        ns_lp_printf("Failed to configure TIMER%d, return value=%d\r\n", cfg->timer, ui32Status);
        return ui32Status;
    }

    //
    // Stop and clear the timer.
    //
    am_hal_timer_clear(cfg->timer);

    if ((cfg->enableInterrupt)) {
        am_hal_timer_interrupt_clear(AM_HAL_TIMER_MASK(cfg->timer, AM_HAL_TIMER_COMPARE1));
        am_hal_timer_interrupt_enable(AM_HAL_TIMER_MASK(cfg->timer, AM_HAL_TIMER_COMPARE1));
        //
        // Enable the timer interrupt in the NVIC.
        //
        if (cfg->timer == NS_TIMER_INTERRUPT) {
            NVIC_EnableIRQ(TIMER1_IRQn);
            NVIC_SetPriority(TIMER1_IRQn, AM_IRQ_PRIORITY_DEFAULT);
        } else if (cfg->timer == NS_TIMER_USB) {
            NVIC_SetPriority(TIMER2_IRQn, AM_IRQ_PRIORITY_DEFAULT);
            NVIC_EnableIRQ(TIMER2_IRQn);
        }
    }
    return ui32Status;
}

uint32_t ns_us_ticker_read(ns_timer_config_t *cfg) {
#ifndef NS_DISABLE_API_VALIDATION
    if (cfg == NULL) {
        return 0xDEADBEEF;
    }
#endif
    return am_hal_timer_read(cfg->timer) / 6; // 6 ticks per uS
}

uint32_t ns_timer_clear(ns_timer_config_t *cfg) {
#ifndef NS_DISABLE_API_VALIDATION
    if (cfg == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }
#endif
    am_hal_timer_clear(cfg->timer);
    return NS_STATUS_SUCCESS;
}
