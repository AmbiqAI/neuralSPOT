/**
 * @brief
 *
 */
#ifndef __TFLM_VALIDATOR_H
#define __TFLM_VALIDATOR_H
#include "ns_ambiqsuite_harness.h"
#include "ns_debug_log.h"

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

typedef struct {
    uint32_t computed_arena_size;
    uint32_t computed_stat_buffer_size;
    uint32_t computed_stat_per_event_size;
    uint32_t captured_events;
    ns_profiler_event_stats_t stat_buffer[NS_PROFILER_RPC_EVENTS_MAX];
} ns_mut_stats_t;

typedef union {
    ns_mut_stats_t stats;
    char bytes[sizeof(ns_mut_stats_t)];
} ns_outgoing_stats_t;

extern ns_incoming_config_t mut_cfg;
extern ns_outgoing_stats_t mut_stats;
#endif
