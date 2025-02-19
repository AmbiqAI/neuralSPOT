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

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "ns_perf_profile.h"
#include "ns_timer.h"

// Match TFLM kMaxEvents
#define NS_PROFILER_MAX_EVENTS 4096
// #define NS_PROFILER_RPC_EVENTS_MAX 128
#define NS_PROFILER_TAG_SIZE 12
typedef struct {
    ns_cache_dump_t cache_start[NS_PROFILER_MAX_EVENTS];
    ns_cache_dump_t cache_end[NS_PROFILER_MAX_EVENTS];
    ns_perf_counters_t perf_start[NS_PROFILER_MAX_EVENTS];
    ns_perf_counters_t perf_end[NS_PROFILER_MAX_EVENTS];
    bool has_estimated_macs;
    int number_of_layers; ///< Number of layers for which we have mac estimates
    uint32_t *mac_count_map;
    uint32_t estimated_mac_count[NS_PROFILER_MAX_EVENTS];
    uint32_t captured_event_num; ///< How many events have been captured so far
} ns_profiler_sidecar_t;

/// Used by RPC validation functionality
typedef struct {
    ns_cache_dump_t cache_delta;
    ns_perf_counters_t perf_delta;
    uint32_t estimated_macs;
    uint32_t elapsed_us;
    char tag[NS_PROFILER_TAG_SIZE]; ///< Tag from TFLM microprofiler
} ns_profiler_event_stats_t;

typedef struct {
    uint32_t number_of_layers; ///< Number of layers for which we have mac estimates
    uint32_t *mac_count_map;
} ns_perf_mac_count_t;

#ifdef NS_MLPROFILE
extern ns_timer_config_t *ns_microProfilerTimer;
extern ns_profiler_sidecar_t ns_microProfilerSidecar;
extern ns_profiler_event_stats_t ns_profiler_events_stats[NS_PROFILER_RPC_EVENTS_MAX];
#endif

extern void ns_TFDebugLogInit(ns_timer_config_t *t, ns_perf_mac_count_t *m);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // TENSORFLOW_LITE_MICRO_DEBUG_LOG_H_
