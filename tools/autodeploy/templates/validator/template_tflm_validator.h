/**
 * @brief
 *
 */
#ifndef __TFLM_VALIDATOR_H
#define __TFLM_VALIDATOR_H
#include "ns_ambiqsuite_harness.h"
#include "ns_debug_log.h"
#include "ns_pmu_map.h"

// #include <cstdlib>
// #include <cstring>

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

// Memory locations for model and arena
#define NS_AD_TCM   0
#define NS_AD_PSRAM 1
#define NS_AD_SRAM  2
#define NS_AD_MRAM  3

#define NS_AD_RPC_TRANSPORT_UART 0
#define NS_AD_RPC_TRANSPORT_USB 1

#ifdef AM_PART_APOLLO5B
#define NS_NUM_PMU_EVENTS NS_NUM_PMU_MAP_SIZE
#endif 

#define NS_MAX_INPUT_TENSORS 10
#define NS_MAX_OUTPUT_TENSORS 10

typedef struct {
    uint32_t computed_arena_size;
    uint32_t computed_stat_buffer_size;
    uint32_t computed_stat_per_event_size;
    uint32_t pmu_count;
    uint32_t pmu_events_per_layer;
    uint32_t captured_events;
    uint32_t platform; // dictates how to interpret the stats
    char csv_header[512]; // CSV header for the stats
    ns_profiler_event_stats_t stat_buffer[NS_PROFILER_RPC_EVENTS_MAX];
} ns_mut_stats_t;

#ifdef AM_PART_APOLLO5B
typedef struct {
    uint32_t layer;
    char csv_header[2048];
    uint32_t pmu_event_counters[73]; // CAREFUL: this is a bit more than needed, but C needs a fixed size
} ns_pmu_stats_t;
#endif

typedef union {
    ns_mut_stats_t stats;
    char bytes[sizeof(ns_mut_stats_t)];
#ifdef AM_PART_APOLLO5B
    ns_pmu_stats_t pmu_stats;
#endif
} ns_outgoing_stats_t;

extern ns_incoming_config_t mut_cfg;
extern ns_outgoing_stats_t mut_stats;
#endif
