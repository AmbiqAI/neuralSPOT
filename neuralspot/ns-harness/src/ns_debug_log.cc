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

// Implementation for the DebugLog() function that prints to the UART on the
// SparkFun Edge microcontroller. The same should work for other targets using
// the Ambiq Apollo 3.

#include "ns_debug_log.h"
#include "tensorflow/lite/micro/cortex_m_generic/debug_log_callback.h"

#include "ns_ambiqsuite_harness.h"
// #include "am_bsp.h"  // NOLINT
// #include "am_util.h" // NOLINT

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void
ns_TFDebugLog(const char *s) {
#ifdef NS_MLDEBUG
    ns_lp_printf("%s", s);
#endif
}

#ifdef NS_MLDEBUG
ns_timer_config_t *ns_microProfilerTimer;
#endif

void
ns_TFDebugLogInit(ns_timer_config_t *t) {
#ifdef NS_MLDEBUG
    RegisterDebugLogCallback(ns_TFDebugLog);
    ns_microProfilerTimer = t;
#endif
}

// void DebugLog(const char *s) {
// #ifndef TF_LITE_STRIP_ERROR_STRINGS
//     static bool is_initialized = false;
//     if (!is_initialized) {
//         ns_itm_printf_enable();
//         is_initialized = true;
//     }

//     ns_lp_printf("%s", s);
// #endif
// }

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
