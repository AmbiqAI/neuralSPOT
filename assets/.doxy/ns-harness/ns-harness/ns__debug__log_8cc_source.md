

# File ns\_debug\_log.cc

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-harness**](dir_e0d7b3aff6df2cba2f05a768a095730e.md) **>** [**src**](dir_8df671c8e5b7eec7f2ec532421bc80bd.md) **>** [**ns\_debug\_log.cc**](ns__debug__log_8cc.md)

[Go to the documentation of this file](ns__debug__log_8cc.md)

```C++

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

#include "ns_debug_log.h"
#include "tensorflow/lite/micro/cortex_m_generic/debug_log_callback.h"

#include "ns_ambiqsuite_harness.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void
ns_TFDebugLog(const char *s) {
#if defined(NS_MLDEBUG) || defined(NS_MLPROFILE)
    ns_lp_printf("%s", s);
#endif
}

#ifdef NS_MLPROFILE
ns_timer_config_t *ns_microProfilerTimer;
ns_profiler_sidecar_t ns_microProfilerSidecar;
ns_cache_config_t ns_microProfiler_cache_config;
ns_profiler_event_stats_t ns_profiler_events_stats[NS_PROFILER_RPC_EVENTS_MAX];
#endif

void
ns_TFDebugLogInit(ns_timer_config_t *t, ns_perf_mac_count_t *m) {
#if defined(NS_MLDEBUG) || defined(NS_MLPROFILE)
    RegisterDebugLogCallback(ns_TFDebugLog);
#endif

#ifdef NS_MLPROFILE
    ns_microProfilerTimer = t;
    if (m != NULL) {
        ns_microProfilerSidecar.has_estimated_macs = true;
        ns_microProfilerSidecar.number_of_layers = m->number_of_layers;
        ns_microProfilerSidecar.mac_count_map = m->mac_count_map;
    } else {
        ns_microProfilerSidecar.has_estimated_macs = false;
    }
    ns_init_perf_profiler();
    ns_start_perf_profiler();
    ns_microProfiler_cache_config.enable = true;
    ns_cache_profiler_init(&ns_microProfiler_cache_config);
#endif
}

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

```

