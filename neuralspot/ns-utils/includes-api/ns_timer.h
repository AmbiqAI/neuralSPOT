/**
 * @file ns_timer.h
 * @author Carlos Morales
 * @brief Simple timer facility
 * @version 0.1
 * @date 2022-10-11
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef NS_TIMER
#define NS_TIMER

#ifdef __cplusplus
extern "C" {
#endif

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "ns_core.h"

#define NS_TIMER_VERSION "0.0.1"
#define NS_TIMER_MAGIC 0xCA0002
#define NS_TIMER_CHK_HANDLE(h)                                                                     \
    ((h) && ((am_hal_handle_prefix_t *)(h))->s.bInit &&                                            \
     (((am_hal_handle_prefix_t *)(h))->s.magic == NS_TIMER_MAGIC))

struct ns_timer_config;
typedef void (*ns_timer_callback_cb)(struct ns_timer_config *);

/**
 * @brief Supported Timers
 *
 */
typedef enum {
    NS_TIMER_COUNTER = 0,   ///< Intended use is reading timerticks
    NS_TIMER_INTERRUPT = 1, ///< Calls a callback periodically
    NS_TIMER_USB = 2,       ///< Used by ns_usb to periodically service USB
    NS_TIMER_TEMPCO = 3     ///< Used by ns_tempco to periodically collect temps
} ns_timers_e;

typedef struct ns_timer_config {
    am_hal_handle_prefix_t prefix;
    ns_timers_e timer;             ///< NS_TIMER_COUNTER, NS_TIMER_INTERRUPT, or NS_TIMER_USB
    bool enableInterrupt;          ///< Will generate interrupts, needs callback
    uint32_t periodInMicroseconds; ///< For interrupts
    ns_timer_callback_cb callback; ///< Invoked when timer ticks
} ns_timer_config_t;

extern uint32_t
ns_timer_init(ns_timer_config_t *cfg);
extern uint32_t
ns_us_ticker_read(ns_timer_config_t *cfg);

#ifdef __cplusplus
}
#endif

#endif
