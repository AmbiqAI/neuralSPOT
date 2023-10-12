/**
 * @file ns_timer.h
 * @author Carlos Morales
 * @brief Simple timer facility
 * @version 0.1
 * @date 2022-10-11
 *
 * @copyright Copyright (c) 2022
 *
 * \addtogroup ns-timer
 * @{
 * @ingroup ns-utils
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

    #define NS_TIMER_V0_0_1                                                                        \
        { .major = 0, .minor = 0, .revision = 1 }
    #define NS_TIMER_V1_0_0                                                                        \
        { .major = 1, .minor = 0, .revision = 0 }

    #define NS_TIMER_OLDEST_SUPPORTED_VERSION NS_TIMER_V0_0_1
    #define NS_TIMER_CURRENT_VERSION NS_TIMER_V1_0_0
    #define NS_TIMER_API_ID 0xCA0002

extern const ns_core_api_t ns_timer_V0_0_1;
extern const ns_core_api_t ns_timer_V1_0_0;
extern const ns_core_api_t ns_timer_oldest_supported_version;
extern const ns_core_api_t ns_timer_current_version;

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

/**
 * @brief ns-timer configuration struct
 *
 */
typedef struct ns_timer_config {
    const ns_core_api_t *api;      ///< API prefix
    ns_timers_e timer;             ///< NS_TIMER_COUNTER, NS_TIMER_INTERRUPT, or NS_TIMER_USB
    bool enableInterrupt;          ///< Will generate interrupts, needs callback
    uint32_t periodInMicroseconds; ///< For interrupts
    ns_timer_callback_cb callback; ///< Invoked when timer ticks
} ns_timer_config_t;

/**
 * @brief Initialize one of 3 timers supported by NeuralSPOT
 *
 * NS_TIMER_COUNTER     Intended use is reading timerticks
 * NS_TIMER_INTERRUPT   Calls a callback periodically
 * NS_TIMER_USB         Used by ns_usb to periodically service USB
 * NS_TIMER_TEMPCO      Used by ns_tempco to periodically collect temps
 *
 * @param cfg
 * @return uint32_t status
 */
extern uint32_t ns_timer_init(ns_timer_config_t *cfg);

/**
 * @brief Read current value of timer
 *
 * @param cfg
 * @return uint32_t timer if success, 0xDEADBEEF if bad handle
 */
extern uint32_t ns_us_ticker_read(ns_timer_config_t *cfg);

/**
 * @brief Clear timer
 *
 * @param cfg
 * @return uint32_t status
 */
extern uint32_t ns_timer_clear(ns_timer_config_t *cfg);

    #ifdef __cplusplus
}
    #endif
#endif
/** @}*/
