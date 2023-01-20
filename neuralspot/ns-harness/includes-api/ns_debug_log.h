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
#ifndef TENSORFLOW_LITE_MICRO_DEBUG_LOG_H_
#define TENSORFLOW_LITE_MICRO_DEBUG_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "ns_perf_profile.h"
#include "ns_timer.h"

// Match TFLM kMaxEvents
#define NS_PROFILER_MAX_EVENTS 1024
typedef struct {
    ns_cache_dump_t cache_start[NS_PROFILER_MAX_EVENTS];
    ns_cache_dump_t cache_end[NS_PROFILER_MAX_EVENTS];
    ns_perf_counters_t perf_start[NS_PROFILER_MAX_EVENTS];
    ns_perf_counters_t perf_end[NS_PROFILER_MAX_EVENTS];
    bool has_estimated_macs;
    int number_of_layers; ///< Number of layers for which we have mac estimates
    uint32_t *mac_count_map;
    uint32_t estimated_mac_count[NS_PROFILER_MAX_EVENTS];
} ns_profiler_sidecar_t;

typedef struct {
    uint32_t number_of_layers; ///< Number of layers for which we have mac estimates
    uint32_t *mac_count_map;
} ns_perf_mac_count_t;

#ifdef NS_MLPROFILE
extern ns_timer_config_t *ns_microProfilerTimer;
extern ns_profiler_sidecar_t ns_microProfilerSidecar;
#endif

extern void
ns_TFDebugLogInit(ns_timer_config_t *t, ns_perf_mac_count_t *m);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // TENSORFLOW_LITE_MICRO_DEBUG_LOG_H_
