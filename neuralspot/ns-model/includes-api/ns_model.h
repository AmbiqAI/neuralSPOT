/**
 * @file ns_baseline.h
 * @author Generic AI Model wrapper
 * @brief
 * @version 0.1
 * @date 2023-03-13
 *
 * @copyright Copyright (c) 2023
 *
 * \addtogroup ns-model
 * @{
 *
 */

#ifndef NS_BASELINE
    #define NS_BASELINE
    #ifdef __cplusplus
        #include "tensorflow/lite/micro/kernels/micro_ops.h"
        #include "tensorflow/lite/micro/micro_interpreter.h"
        #include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
        #include "tensorflow/lite/micro/micro_profiler.h"
        #include "tensorflow/lite/micro/system_setup.h"
        #include "tensorflow/lite/schema/schema_generated.h"
#ifdef NS_TFSTRUCTURE_RECENT
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

    #define NS_MAX_INPUT_TENSORS 3
    #define NS_MAX_OUTPUT_TENSORS 3

typedef struct {
    ns_model_states_e state;

    // Configuration (init by application)
    ns_model_runtime_e runtime; ///< Future use
    const unsigned char *model_array;
    uint8_t *arena;            ///< Tensor Arena
    uint32_t arena_size;       ///< Size of tensor arena, in bytes
    uint8_t *rv_arena;         ///< ResourceVariable Arena
    uint32_t rv_arena_size;    ///< Size of RV arena, in bytes
    uint32_t rv_count;         ///< Number of resource variables
    uint32_t numInputTensors;  ///< Number of input tensors
    uint32_t numOutputTensors; ///< Number of output tensors

    #ifdef NS_MLPROFILE
    ns_timer_config_t *tickTimer;       ///< Optional, from tflm_profiler tool
    ns_perf_mac_count_t *mac_estimates; ///< Optional, from tflm_profiler tool
    #ifdef AM_PART_APOLLO5B
    ns_pmu_config_t *pmu;
    #else
    void *pmu;
    #endif
    #else
    void *tickTimer;
    void *mac_estimate;
    void *pmu;
    #endif
    // State (init by baseline code)
    const tflite::Model *model;                        ///< Model structure, initialized during init
    tflite::MicroInterpreter *interpreter;             ///< Interpreter, initialized during init
    TfLiteTensor *model_input[NS_MAX_INPUT_TENSORS];   ///< Input tensors, initialized during init
    TfLiteTensor *model_output[NS_MAX_OUTPUT_TENSORS]; ///< Output tensors, initialized during init
    tflite::MicroProfiler *profiler;                   ///< Profiler, initialized during init
    tflite::ErrorReporter *error_reporter;             ///< Error reporter, initialized during init

    // metadata
    uint32_t computed_arena_size;
} ns_model_state_t;

/**
 * @brief Initialize the model
 * @param ms Model state and configuration struct
 * @return int status
 */
extern int ns_model_init(ns_model_state_t *ms);

    #ifdef __cplusplus
}
    #endif
#endif
/** @}*/
