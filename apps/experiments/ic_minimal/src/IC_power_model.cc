/**
 * @file <IC_power>_model.c (generated from template.cc)
 * @author autodeploy.py
 * @brief Generated automatically from template code by NS autodeploy script
 * @version 0.1
 * @date 2023-03-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#if defined(AM_PART_APOLLO5B) || defined(AM_PART_APOLLO510L)
#define NS_PROFILER_PMU_EVENT_0 ARM_PMU_MVE_LDST_MULTI_RETIRED
#define NS_PROFILER_PMU_EVENT_1 ARM_PMU_MVE_INT_MAC_RETIRED
#define NS_PROFILER_PMU_EVENT_2 ARM_PMU_INST_RETIRED
#define NS_PROFILER_PMU_EVENT_3 ARM_PMU_MVE_LD_CONTIG_RETIRED
#endif

#include "IC_power_api.h"
#include "IC_power_model.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_core.h"
#if (NS_AD_TCM == NS_AD_PSRAM) or (NS_AD_TCM == NS_AD_PSRAM)
    #include "ns_peripherals_psram.h"
#endif
#if (NS_AD_TCM == NS_AD_NVM)
    #include "ns_nvm.h"
#endif
#include "IC_power_model_data.h"
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

// static constexpr int IC_power_tensor_arena_size = 1024 * IC_power_COMPUTED_ARENA_SIZE;


#if (IC_power_MODEL_LOCATION == NS_AD_PSRAM)
    unsigned char *IC_power_model_psram;
#elif (IC_power_MODEL_LOCATION == NS_AD_NVM)
    // unsigned char *IC_power_model_nvm; // Model resides in NVM
    extern ns_nvm_config_t nvm_cfg;
#elif (IC_power_MODEL_LOCATION == NS_AD_SRAM)
    AM_SHARED_RW alignas(16) static unsigned char IC_power_model[IC_power_model_for_sram_LEN];
#endif

#if (IC_power_ARENA_LOCATION == NS_AD_PSRAM)
    static uint8_t *IC_power_tensor_arena;
    static constexpr int IC_power_tensor_arena_size = 1024 * 1024 * 10; // 10MB
#else
    static constexpr int IC_power_tensor_arena_size = 1024 * IC_power_COMPUTED_ARENA_SIZE;
    #ifdef AM_PART_APOLLO3
        // Apollo3 doesn't have AM_SHARED_RW
        alignas(16) static uint8_t IC_power_tensor_arena[IC_power_tensor_arena_size];
    #else // not AM_PART_APOLLO3
        #if (IC_power_ARENA_LOCATION == NS_AD_SRAM)
            #ifdef keil6
            // Align to 16 bytes
            AM_SHARED_RW __attribute__((aligned(16))) uint8_t IC_power_tensor_arena[IC_power_tensor_arena_size];
            #else
            AM_SHARED_RW alignas(16) static uint8_t IC_power_tensor_arena[IC_power_tensor_arena_size];
            #endif
        #else
            alignas(16) static uint8_t IC_power_tensor_arena[IC_power_tensor_arena_size];
        #endif
    #endif
#endif

// alignas(16) static uint8_t IC_power_tensor_arena[IC_power_tensor_arena_size];

// Resource Variable Arena
static constexpr int IC_power_resource_var_arena_size =
    4 * (0 + 1) * sizeof(tflite::MicroResourceVariables);
alignas(16) static uint8_t IC_power_var_arena[IC_power_resource_var_arena_size];

#ifdef NS_MLPROFILE
// Timer is used for TF profiling
ns_timer_config_t basic_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};
uint32_t IC_power_mac_estimates[16] = {442368, 2359296, 2359296, 0, 1179648, 2359296, 131072, 0, 1179648, 2359296, 131072, 0, 0, 0, 640, 0};

const char* IC_mac_strings[] = {"3*3*32*32*16*3", "3*3*32*32*16*16", "3*3*32*32*16*16", "0", "3*3*16*16*32*16", "3*3*16*16*32*32", "1*1*16*16*32*16", "0", "3*3*8*8*64*32", "3*3*8*8*64*64", "1*1*8*8*64*32", "0", "0", "0", "64*1*10", "0", };
const char* IC_output_shapes[] = {"1*32*32*16", "1*32*32*16", "1*32*32*16", "1*32*32*16", "1*16*16*32", "1*16*16*32", "1*16*16*32", "1*16*16*32", "1*8*8*64", "1*8*8*64", "1*8*8*64", "1*8*8*64", "1*1*1*64", "1*64", "1*10", "1*10", };
const uint32_t IC_output_magnitudes[] = {16384, 16384, 16384, 16384, 8192, 8192, 8192, 8192, 4096, 4096, 4096, 4096, 64, 64, 10, 10, };
const uint32_t IC_stride_h[] = {1, 1, 1, 0, 2, 1, 2, 0, 2, 1, 2, 0, 0, 0, 0, 0, };
const uint32_t IC_stride_w[] = {1, 1, 1, 0, 2, 1, 2, 0, 2, 1, 2, 0, 0, 0, 0, 0, };
const uint32_t IC_dilation_h[] = {1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, };
const uint32_t IC_dilation_w[] = {1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, };
const char* IC_mac_filter_shapes[] = {"16*3*3*3", "16*3*3*16", "16*3*3*16", "0*0*0*0", "32*3*3*16", "32*3*3*32", "32*1*1*16", "0*0*0*0", "64*3*3*32", "64*3*3*64", "64*1*1*32", "0*0*0*0", "0*0*0*0", "0*0*0*0", "10*64", "0*0*0*0", };
const uint32_t IC_write_estimate[] = {16384, 16384, 16384, 16384, 8192, 8192, 8192, 8192, 4096, 4096, 4096, 4096, 64, 64, 10, 10, };
const uint32_t IC_read_estimate[] = {27648, 147456, 147456, 16384, 36864, 73728, 4096, 8192, 18432, 36864, 2048, 4096, 4096, 64, 640, 10, };
const char* IC_in_dtype[] = {"int8", "int8", "int8", "int8", "int8", "int8", "int8", "int8", "int8", "int8", "int8", "int8", "int8", "int8", "int8", "int8", };
const char* IC_out_dtype[] = {"int8", "int8", "int8", "int8", "int8", "int8", "int8", "int8", "int8", "int8", "int8", "int8", "int8", "int8", "int8", "int8", };
const char* IC_w_dtype[] = {"int8", "int8", "int8", "NA", "int8", "int8", "int8", "NA", "int8", "int8", "int8", "NA", "NA", "NA", "int8", "NA", };
const char* IC_fused_act[] = {"RELU", "RELU", "NONE", "RELU", "RELU", "NONE", "NONE", "RELU", "RELU", "NONE", "NONE", "RELU", "NONE", "NONE", "NONE", "NONE", };
const uint32_t IC_elem_bits[] = {0, 0, 0, 8, 0, 0, 0, 8, 0, 0, 0, 8, 8, 8, 0, 8, };
const uint32_t IC_elem_vec_chunks[] = {0, 0, 0, 1024, 0, 0, 0, 512, 0, 0, 0, 256, 4, 4, 0, 1, };
const uint32_t IC_elem_predicated[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, };

ns_perf_mac_count_t basic_mac = {
    .number_of_layers = 16, 
    .mac_count_map = IC_power_mac_estimates,
    .output_magnitudes = (uint32_t *)IC_output_magnitudes,
    .stride_h = (uint32_t *)IC_stride_h,
    .stride_w = (uint32_t *)IC_stride_w,
    .dilation_h = (uint32_t *)IC_dilation_h,
    .dilation_w = (uint32_t *)IC_dilation_w,
    .mac_compute_string = (const char **)IC_mac_strings,
    .output_shapes = (const char **)IC_output_shapes,
    .filter_shapes = (const char **)IC_mac_filter_shapes
};

#if defined(AM_PART_APOLLO5B) || defined(AM_PART_APOLLO510L)
ns_pmu_config_t basic_pmu_cfg;
#endif

#endif

#if (IC_power_MODEL_LOCATION == NS_AD_PSRAM) or (IC_power_ARENA_LOCATION == NS_AD_PSRAM)
extern ns_psram_config_t psram_cfg;
#endif

int IC_power_init(ns_model_state_t *ms);

int IC_power_minimal_init(ns_model_state_t *ms) {

#if (IC_power_ARENA_LOCATION == NS_AD_PSRAM)
    IC_power_tensor_arena = (uint8_t *)(psram_cfg.psram_base_address + 20*1024*1024);
#endif
    ms->runtime = TFLM;
    // ms->model_array = IC_power_model;
    ms->arena = IC_power_tensor_arena;
    ms->arena_size = IC_power_tensor_arena_size;
    ms->rv_arena = IC_power_var_arena;
    ms->rv_arena_size = IC_power_resource_var_arena_size;
    ms->rv_count = 0;
    ms->numInputTensors = 1;
    ms->numOutputTensors = 1;

#if (IC_power_MODEL_LOCATION == NS_AD_PSRAM)
    // Copy model to PSRAM
    IC_power_model_psram = (unsigned char *)(psram_cfg.psram_base_address);
    memcpy((unsigned char *)(psram_cfg.psram_base_address), IC_power_model, IC_power_model_len);
    ms->model_array = IC_power_model_psram;
#elif (IC_power_MODEL_LOCATION == NS_AD_SRAM)
    // Copy to SRAM
    memcpy(IC_power_model, IC_power_model_for_sram, IC_power_model_for_sram_len);
    ms->model_array = IC_power_model;
#elif (IC_power_MODEL_LOCATION == NS_AD_NVM)
    // Flash the model image from MRAM (linked blob) into external NVM, then enable XIP.
    // Destination offset within XIP aperture (can be overridden at build time).
    #ifndef NS_AD_NVM_MODEL_OFFSET
    #define NS_AD_NVM_MODEL_OFFSET 0u
    #endif
    uint32_t const dst_base = (uint32_t)nvm_cfg.xip_base_address + (uint32_t)NS_AD_NVM_MODEL_OFFSET;

    // Safety: bail out if model cannot possibly fit.
    if (IC_power_model_len > nvm_cfg.size_bytes) {
        ns_lp_printf("NVM model too large: %u > %u bytes\n", (unsigned)IC_power_model_len, (unsigned)nvm_cfg.size_bytes);
        return IC_power_STATUS_FAILURE;
    }

    // Ensure XIP is disabled before erase/program.
    (void)ns_nvm_disable_xip();

    // Erase necessary sectors covering [dst_base, dst_base + model_len).
    #define NS_NVM_SECTOR_SIZE   (128u * 1024u)  // IS25WX064 sector size (128KB)
    uint32_t erase_start = dst_base & ~(NS_NVM_SECTOR_SIZE - 1u);
    uint32_t erase_end   = (dst_base + (uint32_t)IC_power_model_len + NS_NVM_SECTOR_SIZE - 1u) & ~(NS_NVM_SECTOR_SIZE - 1u);
    for (uint32_t a = erase_start; a < erase_end; a += NS_NVM_SECTOR_SIZE) {
        uint32_t rc = ns_nvm_sector_erase(a);
        if (rc) {
            ns_lp_printf("NVM sector erase failed at 0x%08x (rc=%u)\n", (unsigned)a, (unsigned)rc);
            return IC_power_STATUS_FAILURE;
        }
    }

    // Program the image in manageable chunks.
    const uint8_t *src = (const uint8_t *)IC_power_model;
    uint32_t       rem = (uint32_t)IC_power_model_len;
    uint32_t       off = 0;
    const uint32_t CHUNK = 4096u;
    while (rem) {
        uint32_t now = (rem > CHUNK) ? CHUNK : rem;
        uint32_t rc  = ns_nvm_write(dst_base + off, src + off, now, true);
        if (rc) {
            ns_lp_printf("NVM write failed at 0x%08x (rc=%u)\n", (unsigned)(dst_base + off), (unsigned)rc);
            return IC_power_STATUS_FAILURE;
        }
        off += now; rem -= now;
    }
    // Re-enable XIP so code can execute/read directly.
    if (ns_nvm_enable_xip()) { return IC_power_STATUS_FAILURE; }
    ms->model_array = (unsigned char *)(dst_base);
#else
    ms->model_array = IC_power_model;
#endif

    ns_lp_printf("Model array pointer: 0x%x\n", (uint32_t)ms->model_array);
#ifdef NS_MLPROFILE

    ms->tickTimer = &basic_tickTimer;
    ms->mac_estimates = &basic_mac;
    #if defined(AM_PART_APOLLO5B) || defined(AM_PART_APOLLO510L)

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
    #if defined(AM_PART_APOLLO5B) || defined(AM_PART_APOLLO510L)
    ms->pmu = NULL;
    #endif
#endif

    int status = IC_power_init(ms);
    return status;
}

int IC_power_init(ns_model_state_t *ms) {
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
        #if defined(AM_PART_APOLLO5B) || defined(AM_PART_APOLLO510L)
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
        ns_lp_printf("Model provided is schema version %d not equal to supported version %d.\n", ms->model->version(), TFLITE_SCHEMA_VERSION);
        return IC_power_STATUS_FAILURE;
    }

#ifdef NS_TFSTRUCTURE_RECENT
    static tflite::MicroMutableOpResolver<8> resolver;
#else
    static tflite::MicroMutableOpResolver<8> resolver(error_reporter);
#endif
    resolver.AddConv2D();
resolver.AddAdd();
resolver.AddAveragePool2D();
resolver.AddReshape();
resolver.AddFullyConnected();
resolver.AddSoftmax();
resolver.AddQuantize();
resolver.AddDequantize();


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
        ns_lp_printf("AllocateTensors() failed\n");
        return IC_power_STATUS_FAILURE;
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
    return IC_power_STATUS_SUCCESS;
}
