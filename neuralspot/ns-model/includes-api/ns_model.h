/**
 * @file ns_baseline.h
 * @author Generic TFLM Model baseline library
 * @brief
 * @version 0.1
 * @date 2023-03-13
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef NS_BASELINE
#define NS_BASELINE
#ifdef __cplusplus

    #include "tensorflow/lite/micro/all_ops_resolver.h"
    #include "tensorflow/lite/micro/kernels/micro_ops.h"
    #include "tensorflow/lite/micro/micro_interpreter.h"
    #include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
    #include "tensorflow/lite/micro/micro_profiler.h"
    #include "tensorflow/lite/micro/system_setup.h"
    #include "tensorflow/lite/schema/schema_generated.h"
    #ifdef NS_TF_VERSION_fecdd5d
        #include "tensorflow/lite/micro/tflite_bridge/micro_error_reporter.h"
    #else
        #include "tensorflow/lite/micro/micro_error_reporter.h"
    #endif

extern "C" {
#endif

#ifdef NS_MLPROFILE
    #include "ns_ambiqsuite_harness.h"
    #include "ns_debug_log.h"
#endif

typedef enum { READY, NOT_READY, ERROR } ns_model_states_e;
typedef enum { TFLM } ns_model_runtime_e;

typedef struct {
    ns_model_states_e state;

    // Configuration (init by application)
    ns_model_runtime_e runtime; ///< Future use
    const unsigned char *model_array;
    uint8_t *arena;
    uint32_t arena_size;
#ifdef NS_MLPROFILE
    ns_timer_config_t *tickTimer;
    ns_perf_mac_count_t *mac_estimate; ///< Optional, from tflm_profiler tool
#else
    void *tickTimer;
    void *mac_estimate; ///< Optional, from tflm_profiler tool
#endif
    // State (init by baseline code)
    const tflite::Model *model;
    tflite::MicroInterpreter *interpreter;
    TfLiteTensor *model_input;
    TfLiteTensor *model_output;
    tflite::MicroProfiler *profiler;
    tflite::ErrorReporter *error_reporter;

    // metadata
    uint32_t computed_arena_size;
} ns_model_state_t;

extern int
ns_model_init(ns_model_state_t *ms);

#ifdef __cplusplus
}
#endif
#endif
