

# File ns\_pmu\_map.h

[**File List**](files.md) **>** [**includes-api**](dir_0f796f8be3b51b94a477512418b4fa0e.md) **>** [**ns\_pmu\_map.h**](ns__pmu__map_8h.md)

[Go to the documentation of this file](ns__pmu__map_8h.md)


```C++

#ifndef NS_PMU_MAP_H
#define NS_PMU_MAP_H
#include "am_mcu_apollo.h"

typedef struct {
    uint32_t eventId;
    const char regname[50];
    const char description[120];
} ns_pmu_map_t;

#ifdef AM_PART_APOLLO5B
extern uint32_t g_ns_pmu_map_length;
#define NS_NUM_PMU_MAP_SIZE (g_ns_pmu_map_length/sizeof(ns_pmu_map_t))
extern const ns_pmu_map_t ns_pmu_map[];
#define NS_PMU_MAP_SIZE (sizeof(ns_pmu_map) / sizeof(ns_pmu_map_t))
#else
#define NS_NUM_PMU_MAP_SIZE 0
#endif
#endif // NS_PMU_MAP_H
```


