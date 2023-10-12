/**
 * @file ns_cache_profile.h
 * @author Carlos Morales
 * @brief Collection of cache profiling utilities
 * @version 0.1
 * @date 2022-10-10
 *
 * @copyright Copyright (c) 2022
 *
 * \addtogroup ns-cache-profile
 * @{
 * @ingroup ns-utils
 *
 */
#ifndef NS_CACHE_PROF
    #define NS_CACHE_PROF

    #ifdef __cplusplus
extern "C" {
    #endif

    #include "am_bsp.h"
    #include "am_mcu_apollo.h"
    #include "am_util.h"

/**
 * @brief Struct to capture a snapshot of the cache
 * performance counters
 *
 */
typedef struct {
    uint32_t daccess;
    uint32_t dtaglookup;
    uint32_t dhitslookup;
    uint32_t dhitsline;
    uint32_t iaccess;
    uint32_t itaglookup;
    uint32_t ihitslookup;
    uint32_t ihitsline;
} ns_cache_dump_t;

/**
 * @brief Struct to configure the cache profiler
 *
 */
typedef struct {
    bool enable;
} ns_cache_config_t;

/**
 * @brief Initialize the cache profiler
 *
 * @param cfg Configuration struct
 * @return uint8_t
 */
extern uint8_t ns_cache_profiler_init(ns_cache_config_t *cfg);

/**
 * @brief Capture the current value of the cache
 * performance counters in the provided struct
 *
 * @param dump
 */
extern void ns_capture_cache_stats(ns_cache_dump_t *dump);

/**
 * @brief Print the cache performance counters
 *
 * @param dump
 */
extern void ns_print_cache_stats(ns_cache_dump_t *dump);

/**
 * @brief Print the delta between two cache performance
 * counter snapshots
 *
 * @param start
 * @param end
 */
extern void ns_print_cache_stats_delta(ns_cache_dump_t *start, ns_cache_dump_t *end);

/**
 * @brief Capture the delta between two cache performance
 * counter snapshots
 *
 * @param s start snapshot
 * @param e end snapshot
 * @param d struct where delta will be stored
 */
extern void ns_delta_cache(ns_cache_dump_t *s, ns_cache_dump_t *e, ns_cache_dump_t *d);

typedef struct {
    uint32_t cyccnt;
    uint32_t cpicnt;
    uint32_t exccnt;
    uint32_t sleepcnt;
    uint32_t lsucnt;
    uint32_t foldcnt;
} ns_perf_counters_t;

/**
 * @brief Reset the HW performance counters
 *
 */
extern void ns_reset_perf_counters(void);

/**
 * @brief Configure the HW performance capture unit
 *
 */
extern void ns_init_perf_profiler(void);

/**
 * @brief Start the HW performance capture unit
 *
 */
extern void ns_start_perf_profiler(void);

/**
 * @brief Stop the HW performance capture unit
 *
 */
extern void ns_stop_perf_profiler(void);

/**
 * @brief Capture the current value of the HW performance
 * counters in the provided struct
 *
 * @param c
 */
extern void ns_capture_perf_profiler(ns_perf_counters_t *c);

/**
 * @brief Calculate the delta between two HW performance snapshots
 *
 * @param s starting snapshot
 * @param e ending snapshot
 * @param d Calculated delta
 */
extern void ns_delta_perf(ns_perf_counters_t *s, ns_perf_counters_t *e, ns_perf_counters_t *d);

/**
 * @brief Print a HW performance counter snapshot
 *
 * @param c
 */
extern void ns_print_perf_profile(ns_perf_counters_t *c);

    #ifdef __cplusplus
}
    #endif
#endif // NS_CACHE_PROF
/** @}*/
