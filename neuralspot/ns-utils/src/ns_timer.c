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

const ns_core_api_t ns_timer_V0_0_1 = {.apiId = NS_TIMER_API_ID, .version = NS_TIMER_V0_0_1};

const ns_core_api_t ns_timer_V1_0_0 = {.apiId = NS_TIMER_API_ID, .version = NS_TIMER_V1_0_0};

const ns_core_api_t ns_timer_oldest_supported_version = {
    .apiId = NS_TIMER_API_ID, .version = NS_TIMER_V0_0_1};

const ns_core_api_t ns_timer_current_version = {
    .apiId = NS_TIMER_API_ID, .version = NS_TIMER_V1_0_0};

ns_timer_config_t *ns_timer_config[NS_TIMER_TEMPCO + 1];

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

extern bool ns_core_initialized(void) { return g_ns_state.initialized; }

uint32_t ns_timer_init(ns_timer_config_t *cfg) {
    uint32_t ui32Status = AM_HAL_STATUS_SUCCESS;

#ifndef NS_DISABLE_API_VALIDATION
    if (cfg == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }

    if (ns_core_check_api(
            cfg->api, &ns_timer_oldest_supported_version, &ns_timer_current_version)) {
        return NS_STATUS_INVALID_VERSION;
    }

    if (cfg->timer > NS_TIMER_TEMPCO) {
        return NS_STATUS_INVALID_CONFIG;
    }
    if ((cfg->enableInterrupt) && (cfg->callback == NULL)) {
        return NS_STATUS_INVALID_CONFIG;
    }
#endif

    ns_timer_config[cfg->timer] = cfg;

#if defined(AM_PART_APOLLO3) || defined(AM_PART_APOLLO3P)
    ui32Status = ns_timer_init_ap3(cfg);
#else
    ui32Status = ns_timer_init_ap4(cfg);
#endif

    return ui32Status;
}

#if defined(AM_PART_APOLLO3) || defined(AM_PART_APOLLO3P)
static am_hal_ctimer_config_t g_sTimer = {
    // Don't link timers.
    0,

    // Set up TimerA.
    (AM_HAL_CTIMER_FN_REPEAT | AM_HAL_CTIMER_INT_ENABLE | AM_HAL_CTIMER_HFRC_12KHZ),

    // No configuration for TimerB.
    0,
};

uint32_t ns_timer_init_ap3(ns_timer_config_t *cfg) {

    am_hal_ctimer_clear(0, AM_HAL_CTIMER_TIMERA);
    am_hal_ctimer_config(0, &g_sTimer);
    am_hal_ctimer_clear(1, AM_HAL_CTIMER_TIMERA);
    am_hal_ctimer_config(1, &g_sTimer);
}
#else
uint32_t ns_timer_init_ap4(ns_timer_config_t *cfg) {
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
#endif

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

#if defined(AM_PART_APOLLO3) || defined(AM_PART_APOLLO3P)

// Timer Interrupt Service Routine (ISR)
void am_ctimer_isr(void) {
    uint32_t ui32Status;
    ui32Status = am_hal_ctimer_int_status_get(false);

    am_hal_ctimer_int_clear(ui32Status);
    am_hal_ctimer_int_service(ui32Status);
}

// Emulate Sensor0 New Data
void timer0_handler(void) {

    // Inform main loop of sensor 0 Data availability
    g_bSensor0Data = true;
}

// Emulate Sensor1 New Data
void timer1_handler(void) {
    // Inform main loop of sensor 1 Data availability
    g_bSensor1Data = true;
}
#endif
