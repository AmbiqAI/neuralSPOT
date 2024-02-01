/**
 * @file ns_timer.c
 * @author Carlos Morales
 * @brief Simple timer facility
 * @version 0.1
 * @date 2022-10-11
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "ns_timer.h"
#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "ns_core.h"

static am_hal_ctimer_config_t g_sTimer = {
    // Don't link timers.
    0,

    // Set up TimerA.
    (AM_HAL_CTIMER_FN_REPEAT | AM_HAL_CTIMER_INT_ENABLE | AM_HAL_CTIMER_HFRC_12MHZ),

    // No configuration for TimerB.
    0,
};

#define AP3_CTIMER_FREQ_IN_MHZ 12

// Timer Interrupt Service Routine (ISR)
void am_ctimer_isr(void) {
    uint32_t ui32Status;
    ui32Status = am_hal_ctimer_int_status_get(false);

    am_hal_ctimer_int_clear(ui32Status);
    am_hal_ctimer_int_service(ui32Status);
}

uint32_t ns_timer_platform_init(ns_timer_config_t *cfg) {

    am_hal_ctimer_clear(cfg->timer, AM_HAL_CTIMER_TIMERA);

    // AP3 has a single timer, so we have to enable interrupts whether
    // the config calls for them or not. If interrupts are not asked for,
    // point the callback to a dummy function.

    am_hal_ctimer_config(cfg->timer, &g_sTimer);
    uint32_t ui32Period = cfg->periodInMicroseconds * AP3_CTIMER_FREQ_IN_MHZ;
    am_hal_ctimer_period_set(cfg->timer, AM_HAL_CTIMER_TIMERA, ui32Period, (ui32Period >> 1));

    // Timer 0 int is 0x01 0000_0001
    // Timer 1 int is 0x04 0000_0100
    // Timer 2 int is 0x10 0001_0000
    // Timer 3 int is 0x40 0100_0000
    uint32_t ui32IntMask = (1 << cfg->timer * 2);
    am_hal_ctimer_int_clear(ui32IntMask);
    if (cfg->enableInterrupt) {
        am_hal_ctimer_int_register(ui32IntMask, cfg->callback);
        am_hal_ctimer_int_enable(ui32IntMask);
    }
    NVIC_EnableIRQ(CTIMER_IRQn);
    am_hal_interrupt_master_enable();
}

uint32_t ns_us_ticker_read(ns_timer_config_t *cfg) {
#ifndef NS_DISABLE_API_VALIDATION
    if (cfg == NULL) {
        return 0xDEADBEEF;
    }
#endif
    return am_hal_ctimer_read(cfg->timer, AM_HAL_CTIMER_TIMERA) / AP3_CTIMER_FREQ_IN_MHZ;
}

uint32_t ns_timer_clear(ns_timer_config_t *cfg) {
#ifndef NS_DISABLE_API_VALIDATION
    if (cfg == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }
#endif
    am_hal_ctimer_clear(cfg->timer, AM_HAL_CTIMER_TIMERA);
    return NS_STATUS_SUCCESS;
}
