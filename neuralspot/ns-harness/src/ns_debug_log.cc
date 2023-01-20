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
ns_cache_config_t cc;
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
    cc.enable = true;
    ns_cache_profiler_init(&cc);
#endif
}

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
