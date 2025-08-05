/**
 * @file <str_ww_ref_model_power>_model.c (generated from template.cc)
 * @author autodeploy.py
 * @brief Generated automatically from template code by NS autodeploy script
 * @version 0.1
 * @date 2023-03-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifdef AM_PART_APOLLO5B
#define NS_PROFILER_PMU_EVENT_0 ARM_PMU_MVE_LDST_MULTI_RETIRED
#define NS_PROFILER_PMU_EVENT_1 ARM_PMU_MVE_INT_MAC_RETIRED
#define NS_PROFILER_PMU_EVENT_2 ARM_PMU_INST_RETIRED
#define NS_PROFILER_PMU_EVENT_3 ARM_PMU_MVE_LD_CONTIG_RETIRED
#endif

#include "ns_debug_log.h"
#include "str_ww_ref_model_power_api.h"
#include "str_ww_ref_model_power_model.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_core.h"
#if (NS_AD_TCM == NS_AD_PSRAM) or (NS_AD_TCM == NS_AD_PSRAM)
    #include "ns_peripherals_psram.h"
#endif
#include "str_ww_ref_model_power_model_data.h"
#include "ns_model.h"

// Tensorflow Lite for Microcontroller includes (somewhat boilerplate)
// #include "tensorflow/lite/micro/all_ops_resolver.h"
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

// static constexpr int str_ww_ref_model_power_tensor_arena_size = 1024 * str_ww_ref_model_power_COMPUTED_ARENA_SIZE;


#if (str_ww_ref_model_power_MODEL_LOCATION == NS_AD_PSRAM)
    unsigned char *str_ww_ref_model_power_model_psram;
#elif (str_ww_ref_model_power_MODEL_LOCATION == NS_AD_SRAM)
    AM_SHARED_RW alignas(16) static unsigned char str_ww_ref_model_power_model[str_ww_ref_model_power_model_for_sram_LEN];
#endif

#if (str_ww_ref_model_power_ARENA_LOCATION == NS_AD_PSRAM)
    static uint8_t *str_ww_ref_model_power_tensor_arena;
    static constexpr int str_ww_ref_model_power_tensor_arena_size = 1024 * 1024 * 10; // 10MB
#else
    static constexpr int str_ww_ref_model_power_tensor_arena_size = 1024 * str_ww_ref_model_power_COMPUTED_ARENA_SIZE;
    #ifdef AM_PART_APOLLO3
        // Apollo3 doesn't have AM_SHARED_RW
        alignas(16) static uint8_t str_ww_ref_model_power_tensor_arena[str_ww_ref_model_power_tensor_arena_size];
    #else // not AM_PART_APOLLO3
        #if (str_ww_ref_model_power_ARENA_LOCATION == NS_AD_SRAM)
            #ifdef keil6
            // Align to 16 bytes
            AM_SHARED_RW __attribute__((aligned(16))) static uint8_t str_ww_ref_model_power_tensor_arena[str_ww_ref_model_power_tensor_arena_size];
            #else
            AM_SHARED_RW alignas(16) static uint8_t str_ww_ref_model_power_tensor_arena[str_ww_ref_model_power_tensor_arena_size];
            #endif
        #else
            alignas(16) static uint8_t str_ww_ref_model_power_tensor_arena[str_ww_ref_model_power_tensor_arena_size];
        #endif
    #endif
#endif

// alignas(16) static uint8_t str_ww_ref_model_power_tensor_arena[str_ww_ref_model_power_tensor_arena_size];

// Resource Variable Arena
static constexpr int str_ww_ref_model_power_resource_var_arena_size =
    4 * (0 + 1) * sizeof(tflite::MicroResourceVariables);
alignas(16) static uint8_t str_ww_ref_model_power_var_arena[str_ww_ref_model_power_resource_var_arena_size];

#ifdef NS_MLPROFILE
// Timer is used for TF profiling
ns_timer_config_t basic_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};
uint32_t str_ww_ref_model_power_mac_estimates[11] = {3360, 143360, 15360, 393216, 19200, 245760, 1920, 4096, 0, 96, 0};

const char* str_ww_ref_model_mac_strings[] = {"3*1*28*1*40", "1*1*28*1*128*40", "5*1*24*1*128", "1*1*24*1*128*128", "10*1*15*1*128", "1*1*15*1*128*128", "15*1*1*1*128", "1*1*1*1*32*128", "0", "32*1*3", "0", };
const char* str_ww_ref_model_output_shapes[] = {"1*28*1*40", "1*28*1*128", "1*24*1*128", "1*24*1*128", "1*15*1*128", "1*15*1*128", "1*1*1*128", "1*1*1*32", "1*32", "1*3", "1*3", };
const uint32_t str_ww_ref_model_output_magnitudes[] = {1120, 3584, 3072, 3072, 1920, 1920, 128, 32, 32, 3, 3, };
const uint32_t str_ww_ref_model_stride_h[] = {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, };
const uint32_t str_ww_ref_model_stride_w[] = {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, };
const uint32_t str_ww_ref_model_dilation_h[] = {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, };
const uint32_t str_ww_ref_model_dilation_w[] = {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, };
const char* str_ww_ref_model_mac_filter_shapes[] = {"1*3*1*40", "128*1*1*40", "1*5*1*128", "128*1*1*128", "1*10*1*128", "128*1*1*128", "1*15*1*128", "32*1*1*128", "0*0*0*0", "3*32", "0*0*0*0", };
const uint32_t str_ww_ref_model_write_estimate[] = {1120, 3584, 3072, 3072, 1920, 1920, 128, 32, 32, 3, 3, };
const uint32_t str_ww_ref_model_read_estimate[] = {3360, 1120, 15360, 3072, 19200, 1920, 1920, 128, 32, 96, 3, };

ns_perf_mac_count_t basic_mac = {
    .number_of_layers = 11, 
    .mac_count_map = str_ww_ref_model_power_mac_estimates,
    .output_magnitudes = (uint32_t *)str_ww_ref_model_output_magnitudes,
    .stride_h = (uint32_t *)str_ww_ref_model_stride_h,
    .stride_w = (uint32_t *)str_ww_ref_model_stride_w,
    .dilation_h = (uint32_t *)str_ww_ref_model_dilation_h,
    .dilation_w = (uint32_t *)str_ww_ref_model_dilation_w,
    .mac_compute_string = (const char **)str_ww_ref_model_mac_strings,
    .output_shapes = (const char **)str_ww_ref_model_output_shapes,
    .filter_shapes = (const char **)str_ww_ref_model_mac_filter_shapes
};

#ifdef AM_PART_APOLLO5B
ns_pmu_config_t basic_pmu_cfg;
#endif

#endif

#if (str_ww_ref_model_power_MODEL_LOCATION == NS_AD_PSRAM) or (str_ww_ref_model_power_ARENA_LOCATION == NS_AD_PSRAM)
extern ns_psram_config_t psram_cfg;
#endif

int str_ww_ref_model_power_init(ns_model_state_t *ms);

int str_ww_ref_model_power_minimal_init(ns_model_state_t *ms) {

#if (str_ww_ref_model_power_ARENA_LOCATION == NS_AD_PSRAM)
    str_ww_ref_model_power_tensor_arena = (uint8_t *)(psram_cfg.psram_base_address + 20*1024*1024);
#endif
    ms->runtime = TFLM;
    // ms->model_array = str_ww_ref_model_power_model;
    ms->arena = str_ww_ref_model_power_tensor_arena;
    ms->arena_size = str_ww_ref_model_power_tensor_arena_size;
    ms->rv_arena = str_ww_ref_model_power_var_arena;
    ms->rv_arena_size = str_ww_ref_model_power_resource_var_arena_size;
    ms->rv_count = 0;
    ms->numInputTensors = 1;
    ms->numOutputTensors = 1;

#if (str_ww_ref_model_power_MODEL_LOCATION == NS_AD_PSRAM)
    // Copy model to PSRAM
    str_ww_ref_model_power_model_psram = (unsigned char *)(psram_cfg.psram_base_address);
    memcpy((unsigned char *)(psram_cfg.psram_base_address), str_ww_ref_model_power_model, str_ww_ref_model_power_model_len);
    ms->model_array = str_ww_ref_model_power_model_psram;
#elif (str_ww_ref_model_power_MODEL_LOCATION == NS_AD_SRAM)
    // Copy to SRAM
    memcpy(str_ww_ref_model_power_model, str_ww_ref_model_power_model_for_sram, str_ww_ref_model_power_model_for_sram_len);
    ms->model_array = str_ww_ref_model_power_model;
#else
    ms->model_array = str_ww_ref_model_power_model;
#endif

#ifdef NS_MLPROFILE

    ms->tickTimer = &basic_tickTimer;
    ms->mac_estimates = &basic_mac;
    #ifdef AM_PART_APOLLO5B

    // PMU config for profiling
    basic_pmu_cfg.api = &ns_pmu_V1_0_0;
    ns_pmu_reset_config(&basic_pmu_cfg);

    // Add events
    ns_pmu_event_create(&basic_pmu_cfg.events[0], NS_PROFILER_PMU_EVENT_0, NS_PMU_EVENT_COUNTER_SIZE_32);
    ns_pmu_event_create(&basic_pmu_cfg.events[1], NS_PROFILER_PMU_EVENT_1, NS_PMU_EVENT_COUNTER_SIZE_32);
    ns_pmu_event_create(&basic_pmu_cfg.events[2], NS_PROFILER_PMU_EVENT_2, NS_PMU_EVENT_COUNTER_SIZE_32);
    ns_pmu_event_create(&basic_pmu_cfg.events[3], NS_PROFILER_PMU_EVENT_3, NS_PMU_EVENT_COUNTER_SIZE_32);   
    ns_pmu_init(&basic_pmu_cfg); // PMU config passed to model init, which passes it to debugLogInit
    ms->pmu = &basic_pmu_cfg;
    #endif
#else
    ms->tickTimer = NULL;
    ms->mac_estimates = NULL;
    #ifdef AM_PART_APOLLO5B
    ms->pmu = NULL;
    #endif
#endif

    int status = str_ww_ref_model_power_init(ms);
    return status;
}

int str_ww_ref_model_power_init(ns_model_state_t *ms) {
    ms->state = NOT_READY;

    tflite::MicroErrorReporter micro_error_reporter;
    ms->error_reporter = &micro_error_reporter;

#ifdef NS_MLPROFILE
    // Need a timer for the profiler to collect latencies
    NS_TRY(ns_timer_init(ms->tickTimer), "Timer init failed.\n");
    AM_SHARED_RW static tflite::MicroProfiler micro_profiler;
    ms->profiler = &micro_profiler;
    // Create the config struct for the debug log
    ns_debug_log_init_t cfg = {
        .t = ms->tickTimer,
        .m = ms->mac_estimates,
        #ifdef AM_PART_APOLLO5B
        .pmu = ms->pmu,
        #endif
    };
    ns_TFDebugLogInit(&cfg);

#else
    #ifdef NS_MLDEBUG
    ns_TFDebugLogInit(NULL);
    #endif
#endif

    tflite::InitializeTarget();

    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    ms->model = tflite::GetModel(ms->model_array);
    if (ms->model->version() != TFLITE_SCHEMA_VERSION) {
        TF_LITE_REPORT_ERROR(
            ms->error_reporter,
            "Model provided is schema version %d not equal "
            "to supported version %d.",
            ms->model->version(), TFLITE_SCHEMA_VERSION);
        return str_ww_ref_model_power_STATUS_FAILURE;
    }

#ifdef NS_TFSTRUCTURE_RECENT
    static tflite::MicroMutableOpResolver<5> resolver;
#else
    static tflite::MicroMutableOpResolver<5> resolver(error_reporter);
#endif
    resolver.AddDepthwiseConv2D();
resolver.AddConv2D();
resolver.AddReshape();
resolver.AddFullyConnected();
resolver.AddSoftmax();


    // Allocate ResourceVariable stuff if needed
    tflite::MicroResourceVariables *resource_variables;
    tflite::MicroAllocator *var_allocator;

    if (ms->rv_count != 0) {
        var_allocator = tflite::MicroAllocator::Create(ms->rv_arena, ms->rv_arena_size, nullptr);
        resource_variables = tflite::MicroResourceVariables::Create(var_allocator, ms->rv_count);
    } else {
        resource_variables = nullptr;
    }

    // Build an interpreter to run the model with.
#ifdef NS_TFSTRUCTURE_RECENT
    static tflite::MicroInterpreter static_interpreter(
        ms->model, resolver, ms->arena, ms->arena_size, resource_variables, ms->profiler);
#else
    static tflite::MicroInterpreter static_interpreter(
        ms->model, resolver, ms->arena, ms->arena_size, ms->error_reporter, nullptr, ms->profiler);
#endif
    ms->interpreter = &static_interpreter;

    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = ms->interpreter->AllocateTensors();

    if (allocate_status != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(ms->error_reporter, "AllocateTensors() failed");
        return str_ww_ref_model_power_STATUS_FAILURE;
    }
    ms->computed_arena_size = ms->interpreter->arena_used_bytes(); // prep to send back to PC
    #ifdef NS_MLPROFILE
    ns_lp_printf("Arena size computed., %d\n", ms->computed_arena_size);
    #endif
    // Obtain pointers to the model's input and output tensors.
    for (uint32_t t = 0; t < ms->numInputTensors; t++) {
        ms->model_input[t] = ms->interpreter->input(t);
    }

    for (uint32_t t = 0; t < ms->numOutputTensors; t++) {
        ms->model_output[t] = ms->interpreter->output(t);
    }

    ms->state = READY;
    return str_ww_ref_model_power_STATUS_SUCCESS;
}
