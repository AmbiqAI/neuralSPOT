

# File ns\_pmu\_utils.h

[**File List**](files.md) **>** [**includes-api**](dir_0f796f8be3b51b94a477512418b4fa0e.md) **>** [**ns\_pmu\_utils.h**](ns__pmu__utils_8h.md)

[Go to the documentation of this file](ns__pmu__utils_8h.md)


```C++

#ifndef NS_PMU_UTILS_H
#define NS_PMU_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#define NS_PMU_V0_0_1                                                                        \
    { .major = 0, .minor = 0, .revision = 1 }
#define NS_PMU_V1_0_0                                                                        \
    { .major = 1, .minor = 0, .revision = 0 }

#define NS_PMU_OLDEST_SUPPORTED_VERSION NS_PMU_V0_0_1
#define NS_PMU_CURRENT_VERSION NS_PMU_V1_0_0
#define NS_PMU_API_ID 0xCA000B

extern const ns_core_api_t ns_pmu_V0_0_1;
extern const ns_core_api_t ns_pmu_V1_0_0;
extern const ns_core_api_t ns_pmu_oldest_supported_version;
extern const ns_core_api_t ns_pmu_current_version;

typedef enum {
    NS_PMU_EVENT_COUNTER_SIZE_16 = 0,
    NS_PMU_EVENT_COUNTER_SIZE_32 = 1
} ns_pmu_event_counter_size_e;

typedef struct ns_pmu_event {
    bool enabled;
    uint32_t eventId;
    ns_pmu_event_counter_size_e counterSize;
} ns_pmu_event_t;

typedef struct ns_pmu_counter {
    bool added;            
    uint32_t mapIndex;     
    uint32_t counterValue; 
} ns_pmu_counter_t;

typedef struct {
    uint32_t counterValue[8]; 
} ns_pmu_counters_t; // just the counter values, used for snapshots

typedef struct ns_pmu_config {
    const ns_core_api_t *api;
    ns_pmu_event_t events[8];
    ns_pmu_counter_t counter[8];
} ns_pmu_config_t;

uint32_t ns_pmu_init(ns_pmu_config_t *cfg);
uint32_t ns_pmu_get_counters(ns_pmu_config_t *cfg);
void ns_delta_pmu(ns_pmu_counters_t *s, ns_pmu_counters_t *e, ns_pmu_counters_t *d);
void ns_pmu_get_name(ns_pmu_config_t *cfg, uint32_t i, char *name);
uint32_t ns_pmu_print_counters(ns_pmu_config_t *cfg);
void ns_pmu_event_create(ns_pmu_event_t *event, uint32_t eventId, ns_pmu_event_counter_size_e counterSize);
void ns_pmu_reset_counters();
void ns_pmu_reset_config(ns_pmu_config_t *cfg);

// Callback for tensorflow invoke
typedef int (*invoke_fp)();
// typedef TfLiteStatus (tflite::MicroInterpreter::*)()
void ns_pmu_characterize_function(invoke_fp func, ns_pmu_config_t *cfg);

#ifdef __cplusplus
}
#endif
#endif // NS_PMU_UTILS_H
```


