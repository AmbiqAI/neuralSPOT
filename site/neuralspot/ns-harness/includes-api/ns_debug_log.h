/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#ifndef NS_TENSORFLOW_LITE_MICRO_DEBUG_LOG_H_
#define NS_TENSORFLOW_LITE_MICRO_DEBUG_LOG_H_

#include "ns_perf_profile.h"
#include "ns_timer.h"
#ifdef AM_PART_APOLLO5B
#include "ns_pmu_utils.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Match TFLM kMaxEvents
#ifdef AM_PART_APOLLO5B
#define NS_PROFILER_MAX_EVENTS 4096
#else
#define NS_PROFILER_MAX_EVENTS 2048
#endif
// #define NS_PROFILER_RPC_EVENTS_MAX 128
#define NS_PROFILER_TAG_SIZE 20

#ifdef AM_PART_APOLLO5B
// Default PMU Events to capture - either all 4 are redefined or none
#ifndef NS_PROFILER_PMU_EVENT_0
#define NS_PROFILER_PMU_EVENT_0 ARM_PMU_MVE_INST_RETIRED
#define NS_PROFILER_PMU_EVENT_1 ARM_PMU_MVE_INT_MAC_RETIRED
#define NS_PROFILER_PMU_EVENT_2 ARM_PMU_INST_RETIRED
#define NS_PROFILER_PMU_EVENT_3 ARM_PMU_BUS_CYCLES
#endif
#endif


/// Used by RPC validation functionality
typedef struct {
    #ifdef AM_PART_APOLLO5B
    ns_pmu_counters_t pmu_delta;
    #else
    ns_cache_dump_t cache_delta;
    #endif
    ns_perf_counters_t perf_delta;
    uint32_t estimated_macs;
    uint32_t elapsed_us;
    char tag[NS_PROFILER_TAG_SIZE]; ///< Tag from TFLM microprofiler
} ns_profiler_event_stats_t;

typedef struct {
    uint32_t number_of_layers; ///< Number of layers for which we have mac estimates
    uint32_t *mac_count_map;
    uint32_t *output_magnitudes;
    uint32_t *stride_h;
    uint32_t *stride_w;
    uint32_t *dilation_h;
    uint32_t *dilation_w;
    
    // Arrays of Strings
    const char **mac_compute_string;
    const char **output_shapes;
    const char **filter_shapes;

} ns_perf_mac_count_t;


typedef struct {
    #ifdef AM_PART_APOLLO5B
    ns_pmu_counters_t pmu_snapshot[NS_PROFILER_MAX_EVENTS];
    #else
    ns_cache_dump_t cache_snapshot[NS_PROFILER_MAX_EVENTS];
    #endif
    ns_perf_counters_t perf_snapshot[NS_PROFILER_MAX_EVENTS];
    bool has_estimated_macs;
    int number_of_layers; ///< Number of layers for which we have mac estimates
    uint32_t *mac_count_map;
    const ns_perf_mac_count_t *m;
    uint32_t estimated_mac_count[NS_PROFILER_MAX_EVENTS];
    uint32_t captured_event_num; ///< How many events have been captured so far
} ns_profiler_sidecar_t;

#ifdef NS_MLPROFILE
extern ns_timer_config_t *ns_microProfilerTimer;
extern ns_profiler_sidecar_t ns_microProfilerSidecar;
extern char ns_profiler_csv_header[512];
extern ns_profiler_event_stats_t ns_profiler_events_stats[NS_PROFILER_RPC_EVENTS_MAX];
#if defined(AM_PART_APOLLO5B)
extern char ns_profiler_pmu_header[2048];
#endif // AM_PART_APOLLO5B
#endif // NS_MLPROFILE

typedef struct {
    ns_timer_config_t *t;
    const ns_perf_mac_count_t *m;
    #ifdef AM_PART_APOLLO5B
    ns_pmu_config_t *pmu;
    #endif
} ns_debug_log_init_t;

extern void
ns_TFDebugLogInit(ns_debug_log_init_t *cfg);
#ifdef AM_PART_APOLLO5B
extern uint32_t ns_characterize_model(invoke_fp func);
extern uint32_t ns_parse_pmu_stats(uint32_t num_layers, uint32_t rv);
extern uint32_t ns_set_pmu_header(void);
extern uint32_t ns_get_layer_counters(uint32_t layer,
                      uint32_t num_layers,
                      uint32_t rv,
                      uint32_t *out_counters);
#endif // AM_PART_APOLLO5B
#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // TENSORFLOW_LITE_MICRO_DEBUG_LOG_H_