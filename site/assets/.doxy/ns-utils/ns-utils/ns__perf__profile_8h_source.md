

# File ns\_perf\_profile.h

[**File List**](files.md) **>** [**includes-api**](dir_0f796f8be3b51b94a477512418b4fa0e.md) **>** [**ns\_perf\_profile.h**](ns__perf__profile_8h.md)

[Go to the documentation of this file](ns__perf__profile_8h.md)


```C++

#ifndef NS_CACHE_PROF
    #define NS_CACHE_PROF

    #ifdef __cplusplus
extern "C" {
    #endif

    #include "am_bsp.h"
    #include "am_mcu_apollo.h"
    #include "am_util.h"

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

typedef struct {
    bool enable;
} ns_cache_config_t;

extern uint8_t ns_cache_profiler_init(ns_cache_config_t *cfg);

extern void ns_capture_cache_stats(ns_cache_dump_t *dump);

extern void ns_print_cache_stats(ns_cache_dump_t *dump);

extern void ns_print_cache_stats_delta(ns_cache_dump_t *start, ns_cache_dump_t *end);

extern void ns_delta_cache(ns_cache_dump_t *s, ns_cache_dump_t *e, ns_cache_dump_t *d);

typedef struct {
    uint32_t cyccnt;
    uint32_t cpicnt;
    uint32_t exccnt;
    uint32_t sleepcnt;
    uint32_t lsucnt;
    uint32_t foldcnt;
} ns_perf_counters_t;

extern void ns_reset_perf_counters(void);

extern void ns_init_perf_profiler(void);

extern void ns_start_perf_profiler(void);

extern void ns_stop_perf_profiler(void);

extern void ns_capture_perf_profiler(ns_perf_counters_t *c);

extern void ns_delta_perf(ns_perf_counters_t *s, ns_perf_counters_t *e, ns_perf_counters_t *d);

extern void ns_print_perf_profile(ns_perf_counters_t *c);

#ifdef AM_PART_APOLLO5B
uint32_t ns_perf_enable_pcsamp(void);
int32_t ns_itm_pcsamp_enable(void);
#endif

#ifdef __cplusplus
}
#endif
#endif // NS_CACHE_PROF
```


