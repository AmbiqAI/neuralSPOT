

# File ns\_timer\_ap3.c

[**File List**](files.md) **>** [**apollo3**](dir_707bd67c77e0cdcc384b3fcaae8a053a.md) **>** [**ns\_timer\_ap3.c**](ns__timer__ap3_8c.md)

[Go to the documentation of this file](ns__timer__ap3_8c.md)


```C++

#include "ns_timer.h"
#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "ns_core.h"

extern ns_timer_config_t *ns_timer_config[NS_TIMER_TEMPCO + 1];

void timer0_handler(void) {
    if (ns_timer_config[0]->enableInterrupt) {
        ns_timer_config[0]->callback(ns_timer_config[0]);
    }
}

void timer1_handler(void) {
    if (ns_timer_config[1]->enableInterrupt) {
        ns_timer_config[1]->callback(ns_timer_config[1]);
    }
}

void timer2_handler(void) {
    if (ns_timer_config[2]->enableInterrupt) {
        ns_timer_config[2]->callback(ns_timer_config[2]);
    }
}

void timer3_handler(void) {
    if (ns_timer_config[3]->enableInterrupt) {
        ns_timer_config[3]->callback(ns_timer_config[3]);
    }
}

static am_hal_ctimer_config_t g_sTimer = {
    // Don't link timers.
    1,

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

    uint32_t ui32Period = cfg->periodInMicroseconds * AP3_CTIMER_FREQ_IN_MHZ;
    // Timer 0 int is 0x01 0000_0001
    // Timer 1 int is 0x04 0000_0100
    // Timer 2 int is 0x10 0001_0000
    // Timer 3 int is 0x40 0100_0000
    uint32_t ui32IntMask = (1 << cfg->timer * 2);

    am_hal_ctimer_clear(cfg->timer, AM_HAL_CTIMER_BOTH);
    am_hal_ctimer_config(cfg->timer, &g_sTimer);
    if (cfg->enableInterrupt) {
        am_hal_ctimer_period_set(cfg->timer, AM_HAL_CTIMER_BOTH, ui32Period, (ui32Period >> 1));
    }

    am_hal_ctimer_int_clear(ui32IntMask);
    if (cfg->enableInterrupt) {
        switch (cfg->timer) {
        case 0:
            am_hal_ctimer_int_register(ui32IntMask, timer0_handler);
            break;
        case 1:
            am_hal_ctimer_int_register(ui32IntMask, timer1_handler);
            break;
        case 2:
            am_hal_ctimer_int_register(ui32IntMask, timer2_handler);
            break;
        case 3:
            am_hal_ctimer_int_register(ui32IntMask, timer3_handler);
            break;
        }
        am_hal_ctimer_int_enable(ui32IntMask);
        NVIC_EnableIRQ(CTIMER_IRQn);
    }
    am_hal_ctimer_start(cfg->timer, AM_HAL_CTIMER_BOTH);

    am_hal_interrupt_master_enable();
    return NS_STATUS_SUCCESS;
}

uint32_t ns_us_ticker_read(ns_timer_config_t *cfg) {
#ifndef NS_DISABLE_API_VALIDATION
    if (cfg == NULL) {
        return 0xDEADBEEF;
    }
#endif
    return am_hal_ctimer_read(cfg->timer, AM_HAL_CTIMER_BOTH) / AP3_CTIMER_FREQ_IN_MHZ;
}

uint32_t ns_timer_clear(ns_timer_config_t *cfg) {
#ifndef NS_DISABLE_API_VALIDATION
    if (cfg == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }
#endif
    am_hal_ctimer_clear(cfg->timer, AM_HAL_CTIMER_BOTH);
    return NS_STATUS_SUCCESS;
}
```


