/**
 * @file ns_pmu_map.h
 * @author Ambiq
 * @brief Convenient mapping of PMU registers to human-readable names
 * @version 0.1
 * @date 2024-08-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef NS_PMU_MAP_H
#define NS_PMU_MAP_H
#include "am_mcu_apollo.h"

typedef struct {
    uint32_t eventId;
    const char regname[50];
    const char description[120];
} ns_pmu_map_t;

#if defined(AM_PART_APOLLO5B) || defined(AM_PART_APOLLO510L)
extern uint32_t g_ns_pmu_map_length;
#define NS_NUM_PMU_MAP_SIZE (g_ns_pmu_map_length/sizeof(ns_pmu_map_t))
extern const ns_pmu_map_t ns_pmu_map[];
#define NS_PMU_MAP_SIZE 70 
// (sizeof(ns_pmu_map) / sizeof(ns_pmu_map_t))
#else
#define NS_NUM_PMU_MAP_SIZE 0
#endif
#endif // NS_PMU_MAP_H