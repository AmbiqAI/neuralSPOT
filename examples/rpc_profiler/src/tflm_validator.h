/**
 * @brief
 *
 */
#ifndef __TFLM_VALIDATOR_H
#define __TFLM_VALIDATOR_H
#include "ns_ambiqsuite_harness.h"
#include "ns_debug_log.h"
#include "ns_pmu_map.h"
#include "ns_pmu_utils.h"
#include "test_fn.h"

#include <cstdlib>
#include <cstring>

typedef struct {
    uint32_t profile_mut; ///> 1 -> enable ML Profiling and reporting
    // uint32_t num_resource_variables; ///> number of TFLM ResourceVariables to allocate
    uint32_t input_length;   ///> in bytes
    uint32_t output_length;  ///> in bytes
    uint32_t profile_warmup; ///> how many inferences to run before profiling
    uint32_t num_input_tensors;
    uint32_t num_output_tensors;
    uint32_t full_pmu_stats; ///> 1 -> enable full PMU stats
} ns_mut_config_t;

typedef union {
    ns_mut_config_t config;
    char bytes[sizeof(ns_mut_config_t)];
} ns_incoming_config_t;

typedef struct {
    uint32_t tensorSizeBytes;
} ns_tensor_details_t;

typedef union {
    ns_tensor_details_t details;
    char bytes[sizeof(ns_tensor_details_t)];
} ns_incoming_tensor_details_u;

#define NS_MUT_STATS_PLATFORM_AP3 3
#define NS_MUT_STATS_PLATFORM_AP4 4
#define NS_MUT_STATS_PLATFORM_AP5 5

#ifdef AM_PART_APOLLO5B
#define NS_NUM_PMU_EVENTS sizeof(ns_pmu_map) / sizeof(ns_pmu_map_t)
#endif 
#define NS_PROFILER_RPC_EVENTS_MAX 16 

typedef struct {
    uint32_t pmu_count;
    uint32_t platform; // dictates how to interpret the stats
    uint32_t captured_events; // uint
    uint32_t computed_stat_buffer_size; // uint
    uint32_t pmu_event_counters[NS_PMU_MAP_SIZE];
    char csv_header[2048]; // CSV header for the stats
} ns_mut_stats_t;

typedef union {
    ns_mut_stats_t stats;
    char bytes[sizeof(ns_mut_stats_t)];
} ns_outgoing_stats_t;

extern ns_incoming_config_t mut_cfg;
extern ns_outgoing_stats_t mut_stats;
#endif
