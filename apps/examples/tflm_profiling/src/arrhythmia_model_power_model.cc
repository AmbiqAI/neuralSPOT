/**
 * @file <arrhythmia_model_power>_model.c (generated from template.cc)
 * @author autodeploy.py
 * @brief Generated automatically from template code by NS autodeploy script
 * @version 0.1
 * @date 2023-03-15
 *
 * @copyright Copyright (c) 2025
 *
 */

#define LOCAL_MODEL_NAME arrhythmia_model_power
#define arrhythmia_model_power_COMPUTED_ARENA_SIZE 73

#define MODEL_LOCATION TCM
// #define MODEL_LOCATION MRAM
// #define MODEL_LOCATION SRAM
// #define MODEL_LOCATION PSRAM

#if (MODEL_LOCATION==TCM)
    #define arrhythmia_model_power_MODEL_LOCATION NS_AD_TCM
    #define MODEL_ATTRIB NS_PUT_IN_TCM
#elif (MODEL_LOCATION==MRAM)
    #define arrhythmia_model_power_MODEL_LOCATION NS_AD_MRAM
    #define MODEL_ATTRIB const
#elif (MODEL_LOCATION==SRAM)
    #define arrhythmia_model_power_MODEL_LOCATION NS_AD_SRAM
    #define MODEL_ATTRIB AM_SHARED_RW
#elif (MODEL_LOCATION==PSRAM)
    #define arrhythmia_model_power_MODEL_LOCATION NS_AD_PSRAM
    // Model will be copied to PSRAM at runtime
    #define MODEL_ATTRIB const
#else
    #error "Invalid MODEL_LOCATION"
#endif




#define arrhythmia_model_power_ARENA_LOCATION NS_AD_TCM
// #define arrhythmia_model_power_ARENA_LOCATION NS_AD_SRAM
// #define arrhythmia_model_power_ARENA_LOCATION NS_AD_PSRAM



#ifdef AM_PART_APOLLO5B
#define NS_PROFILER_PMU_EVENT_0 ARM_PMU_MVE_LDST_MULTI_RETIRED
#define NS_PROFILER_PMU_EVENT_1 ARM_PMU_MVE_INT_MAC_RETIRED
#define NS_PROFILER_PMU_EVENT_2 ARM_PMU_INST_RETIRED
#define NS_PROFILER_PMU_EVENT_3 ARM_PMU_MVE_LD_CONTIG_RETIRED
#endif

#include "arrhythmia_model_power_api.h"
#include "arrhythmia_model_power_model.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_core.h"
#if (arrhythmia_model_power_MODEL_LOCATION == NS_AD_PSRAM) or (arrhythmia_model_power_ARENA_LOCATION == NS_AD_PSRAM)
    #include "ns_peripherals_psram.h"
#endif
#include "arrhythmia_model_power_model_data.h"
#include "ns_model.h"

// Tensorflow Lite for Microcontroller includes (somewhat boilerplate)
// #include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_profiler.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/micro/tflite_bridge/micro_error_reporter.h"


#if (arrhythmia_model_power_MODEL_LOCATION == NS_AD_PSRAM)
    unsigned char *arrhythmia_model_power_model_psram;
#endif

#if (arrhythmia_model_power_ARENA_LOCATION == NS_AD_PSRAM)
    static uint8_t *arrhythmia_model_power_tensor_arena;
    static constexpr int arrhythmia_model_power_tensor_arena_size = 1024 * 1024 * 10; // 10MB
#else
    static constexpr int arrhythmia_model_power_tensor_arena_size = 1024 * arrhythmia_model_power_COMPUTED_ARENA_SIZE;
    #ifdef AM_PART_APOLLO3
        // Apollo3 doesn't have AM_SHARED_RW
        alignas(16) static uint8_t arrhythmia_model_power_tensor_arena[arrhythmia_model_power_tensor_arena_size];
    #else // not AM_PART_APOLLO3
        #if (arrhythmia_model_power_ARENA_LOCATION == NS_AD_SRAM)
            #ifdef keil6
            // Align to 16 bytes
            AM_SHARED_RW __attribute__((aligned(16))) static uint8_t arrhythmia_model_power_tensor_arena[arrhythmia_model_power_tensor_arena_size];
            #else
            AM_SHARED_RW alignas(16) static uint8_t arrhythmia_model_power_tensor_arena[arrhythmia_model_power_tensor_arena_size];
            #endif
        #else
            alignas(16) static uint8_t arrhythmia_model_power_tensor_arena[arrhythmia_model_power_tensor_arena_size];
        #endif
    #endif
#endif

// Resource Variable Arena
static constexpr int arrhythmia_model_power_resource_var_arena_size =
    4 * (0 + 1) * sizeof(tflite::MicroResourceVariables);
alignas(16) static uint8_t arrhythmia_model_power_var_arena[arrhythmia_model_power_resource_var_arena_size];

#ifdef NS_MLPROFILE
// Timer is used for TF profiling
ns_timer_config_t basic_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};
uint32_t arrhythmia_model_power_mac_estimates[111] = {84000, 60000, 0, 0, 288, 288, 0, 0, 0, 192000, 40000, 0, 512, 512, 0, 0, 0, 256000, 0, 40000, 0, 512, 512, 0, 0, 0, 256000, 0, 24000, 0, 0, 256, 256, 0, 0, 0, 192000, 18000, 0, 576, 576, 0, 0, 0, 288000, 0, 18000, 0, 576, 576, 0, 0, 0, 288000, 0, 18000, 0, 0, 576, 576, 0, 0, 0, 193536, 12096, 0, 1024, 1024, 0, 0, 0, 258048, 0, 12096, 0, 1024, 1024, 0, 0, 0, 258048, 0, 12096, 0, 0, 1024, 1024, 0, 0, 0, 196608, 9216, 0, 2304, 2304, 0, 0, 0, 294912, 0, 9216, 0, 2304, 2304, 0, 0, 0, 294912, 0, 0, 192};

const char* arrhythmia_model_mac_strings[] = {"1*7*1*500*24*1", "1*5*1*500*24", "0", "0", "1*1*1*1*12*24", "1*1*1*1*24*12", "0", "0", "0", "1*1*1*250*32*24", "1*5*1*250*32", "0", "1*1*1*1*16*32", "1*1*1*1*32*16", "0", "0", "0", "1*1*1*250*32*32", "0", "1*5*1*250*32", "0", "1*1*1*1*16*32", "1*1*1*1*32*16", "0", "0", "0", "1*1*1*250*32*32", "0", "1*3*1*250*32", "0", "0", "1*1*1*1*8*32", "1*1*1*1*32*8", "0", "0", "0", "1*1*1*125*48*32", "1*3*1*125*48", "0", "1*1*1*1*12*48", "1*1*1*1*48*12", "0", "0", "0", "1*1*1*125*48*48", "0", "1*3*1*125*48", "0", "1*1*1*1*12*48", "1*1*1*1*48*12", "0", "0", "0", "1*1*1*125*48*48", "0", "1*3*1*125*48", "0", "0", "1*1*1*1*12*48", "1*1*1*1*48*12", "0", "0", "0", "1*1*1*63*64*48", "1*3*1*63*64", "0", "1*1*1*1*16*64", "1*1*1*1*64*16", "0", "0", "0", "1*1*1*63*64*64", "0", "1*3*1*63*64", "0", "1*1*1*1*16*64", "1*1*1*1*64*16", "0", "0", "0", "1*1*1*63*64*64", "0", "1*3*1*63*64", "0", "0", "1*1*1*1*16*64", "1*1*1*1*64*16", "0", "0", "0", "1*1*1*32*96*64", "1*3*1*32*96", "0", "1*1*1*1*24*96", "1*1*1*1*96*24", "0", "0", "0", "1*1*1*32*96*96", "0", "1*3*1*32*96", "0", "1*1*1*1*24*96", "1*1*1*1*96*24", "0", "0", "0", "1*1*1*32*96*96", "0", "0", "96*1*2", };
const char* arrhythmia_model_output_shapes[] = {"1*1*500*24", "1*1*500*24", "1*1*250*24", "1*1*1*24", "1*1*1*12", "1*1*1*24", "1*1*1*24", "1*1*1*24", "1*1*250*24", "1*1*250*32", "1*1*250*32", "1*1*1*32", "1*1*1*16", "1*1*1*32", "1*1*1*32", "1*1*1*32", "1*1*250*32", "1*1*250*32", "1*1*250*32", "1*1*250*32", "1*1*1*32", "1*1*1*16", "1*1*1*32", "1*1*1*32", "1*1*1*32", "1*1*250*32", "1*1*250*32", "1*1*250*32", "1*1*250*32", "1*1*125*32", "1*1*1*32", "1*1*1*8", "1*1*1*32", "1*1*1*32", "1*1*1*32", "1*1*125*32", "1*1*125*48", "1*1*125*48", "1*1*1*48", "1*1*1*12", "1*1*1*48", "1*1*1*48", "1*1*1*48", "1*1*125*48", "1*1*125*48", "1*1*125*48", "1*1*125*48", "1*1*1*48", "1*1*1*12", "1*1*1*48", "1*1*1*48", "1*1*1*48", "1*1*125*48", "1*1*125*48", "1*1*125*48", "1*1*125*48", "1*1*63*48", "1*1*1*48", "1*1*1*12", "1*1*1*48", "1*1*1*48", "1*1*1*48", "1*1*63*48", "1*1*63*64", "1*1*63*64", "1*1*1*64", "1*1*1*16", "1*1*1*64", "1*1*1*64", "1*1*1*64", "1*1*63*64", "1*1*63*64", "1*1*63*64", "1*1*63*64", "1*1*1*64", "1*1*1*16", "1*1*1*64", "1*1*1*64", "1*1*1*64", "1*1*63*64", "1*1*63*64", "1*1*63*64", "1*1*63*64", "1*1*32*64", "1*1*1*64", "1*1*1*16", "1*1*1*64", "1*1*1*64", "1*1*1*64", "1*1*32*64", "1*1*32*96", "1*1*32*96", "1*1*1*96", "1*1*1*24", "1*1*1*96", "1*1*1*96", "1*1*1*96", "1*1*32*96", "1*1*32*96", "1*1*32*96", "1*1*32*96", "1*1*1*96", "1*1*1*24", "1*1*1*96", "1*1*1*96", "1*1*1*96", "1*1*32*96", "1*1*32*96", "1*1*32*96", "1*96", "1*2", };
const uint32_t arrhythmia_model_output_magnitudes[] = {12000, 12000, 6000, 24, 12, 24, 24, 24, 6000, 8000, 8000, 32, 16, 32, 32, 32, 8000, 8000, 8000, 8000, 32, 16, 32, 32, 32, 8000, 8000, 8000, 8000, 4000, 32, 8, 32, 32, 32, 4000, 6000, 6000, 48, 12, 48, 48, 48, 6000, 6000, 6000, 6000, 48, 12, 48, 48, 48, 6000, 6000, 6000, 6000, 3024, 48, 12, 48, 48, 48, 3024, 4032, 4032, 64, 16, 64, 64, 64, 4032, 4032, 4032, 4032, 64, 16, 64, 64, 64, 4032, 4032, 4032, 4032, 2048, 64, 16, 64, 64, 64, 2048, 3072, 3072, 96, 24, 96, 96, 96, 3072, 3072, 3072, 3072, 96, 24, 96, 96, 96, 3072, 3072, 3072, 96, 2, };
const uint32_t arrhythmia_model_stride_h[] = {1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, };
const uint32_t arrhythmia_model_stride_w[] = {2, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, };
const uint32_t arrhythmia_model_dilation_h[] = {1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, };
const uint32_t arrhythmia_model_dilation_w[] = {1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, };
const char* arrhythmia_model_mac_filter_shapes[] = {"24*1*7*1", "1*1*5*24", "0*0*0*0", "0*0*0*0", "12*1*1*24", "24*1*1*12", "0*0*0*0", "0*0*0*0", "0*0*0*0", "32*1*1*24", "1*1*5*32", "0*0*0*0", "16*1*1*32", "32*1*1*16", "0*0*0*0", "0*0*0*0", "0*0*0*0", "32*1*1*32", "0*0*0*0", "1*1*5*32", "0*0*0*0", "16*1*1*32", "32*1*1*16", "0*0*0*0", "0*0*0*0", "0*0*0*0", "32*1*1*32", "0*0*0*0", "1*1*3*32", "0*0*0*0", "0*0*0*0", "8*1*1*32", "32*1*1*8", "0*0*0*0", "0*0*0*0", "0*0*0*0", "48*1*1*32", "1*1*3*48", "0*0*0*0", "12*1*1*48", "48*1*1*12", "0*0*0*0", "0*0*0*0", "0*0*0*0", "48*1*1*48", "0*0*0*0", "1*1*3*48", "0*0*0*0", "12*1*1*48", "48*1*1*12", "0*0*0*0", "0*0*0*0", "0*0*0*0", "48*1*1*48", "0*0*0*0", "1*1*3*48", "0*0*0*0", "0*0*0*0", "12*1*1*48", "48*1*1*12", "0*0*0*0", "0*0*0*0", "0*0*0*0", "64*1*1*48", "1*1*3*64", "0*0*0*0", "16*1*1*64", "64*1*1*16", "0*0*0*0", "0*0*0*0", "0*0*0*0", "64*1*1*64", "0*0*0*0", "1*1*3*64", "0*0*0*0", "16*1*1*64", "64*1*1*16", "0*0*0*0", "0*0*0*0", "0*0*0*0", "64*1*1*64", "0*0*0*0", "1*1*3*64", "0*0*0*0", "0*0*0*0", "16*1*1*64", "64*1*1*16", "0*0*0*0", "0*0*0*0", "0*0*0*0", "96*1*1*64", "1*1*3*96", "0*0*0*0", "24*1*1*96", "96*1*1*24", "0*0*0*0", "0*0*0*0", "0*0*0*0", "96*1*1*96", "0*0*0*0", "1*1*3*96", "0*0*0*0", "24*1*1*96", "96*1*1*24", "0*0*0*0", "0*0*0*0", "0*0*0*0", "96*1*1*96", "0*0*0*0", "0*0*0*0", "2*96", };
const uint32_t arrhythmia_model_write_estimate[] = {12000, 12000, 6000, 24, 12, 24, 24, 24, 6000, 8000, 8000, 32, 16, 32, 32, 32, 8000, 8000, 8000, 8000, 32, 16, 32, 32, 32, 8000, 8000, 8000, 8000, 4000, 32, 8, 32, 32, 32, 4000, 6000, 6000, 48, 12, 48, 48, 48, 6000, 6000, 6000, 6000, 48, 12, 48, 48, 48, 6000, 6000, 6000, 6000, 3024, 48, 12, 48, 48, 48, 3024, 4032, 4032, 64, 16, 64, 64, 64, 4032, 4032, 4032, 4032, 64, 16, 64, 64, 64, 4032, 4032, 4032, 4032, 2048, 64, 16, 64, 64, 64, 2048, 3072, 3072, 96, 24, 96, 96, 96, 3072, 3072, 3072, 3072, 96, 24, 96, 96, 96, 3072, 3072, 3072, 96, 2, };
const uint32_t arrhythmia_model_read_estimate[] = {3500, 60000, 12000, 6000, 24, 12, 24, 24, 6000, 6000, 40000, 8000, 32, 16, 32, 32, 8000, 8000, 8000, 40000, 8000, 32, 16, 32, 32, 8000, 8000, 8000, 24000, 8000, 4000, 32, 8, 32, 32, 4000, 4000, 18000, 6000, 48, 12, 48, 48, 6000, 6000, 6000, 18000, 6000, 48, 12, 48, 48, 6000, 6000, 6000, 18000, 6000, 3024, 48, 12, 48, 48, 3024, 3024, 12096, 4032, 64, 16, 64, 64, 4032, 4032, 4032, 12096, 4032, 64, 16, 64, 64, 4032, 4032, 4032, 12096, 4032, 2048, 64, 16, 64, 64, 2048, 2048, 9216, 3072, 96, 24, 96, 96, 3072, 3072, 3072, 9216, 3072, 96, 24, 96, 96, 3072, 3072, 3072, 3072, 192, };

ns_perf_mac_count_t basic_mac = {
    .number_of_layers = 111, 
    .mac_count_map = arrhythmia_model_power_mac_estimates,
    .output_magnitudes = (uint32_t *)arrhythmia_model_output_magnitudes,
    .stride_h = (uint32_t *)arrhythmia_model_stride_h,
    .stride_w = (uint32_t *)arrhythmia_model_stride_w,
    .dilation_h = (uint32_t *)arrhythmia_model_dilation_h,
    .dilation_w = (uint32_t *)arrhythmia_model_dilation_w,
    .mac_compute_string = (const char **)arrhythmia_model_mac_strings,
    .output_shapes = (const char **)arrhythmia_model_output_shapes,
    .filter_shapes = (const char **)arrhythmia_model_mac_filter_shapes
};

#ifdef AM_PART_APOLLO5B
ns_pmu_config_t basic_pmu_cfg;
#endif

#endif

#if (arrhythmia_model_power_MODEL_LOCATION == NS_AD_PSRAM) or (arrhythmia_model_power_ARENA_LOCATION == NS_AD_PSRAM)
extern ns_psram_config_t psram_cfg;
#endif

int arrhythmia_model_power_init(ns_model_state_t *ms);

int arrhythmia_model_power_minimal_init(ns_model_state_t *ms) {

#if (arrhythmia_model_power_ARENA_LOCATION == NS_AD_PSRAM)
    arrhythmia_model_power_tensor_arena = (uint8_t *)(psram_cfg.psram_base_address + 20*1024*1024);
#endif
    ms->runtime = TFLM;
    // ms->model_array = arrhythmia_model_power_model;
    ms->arena = arrhythmia_model_power_tensor_arena;
    ms->arena_size = arrhythmia_model_power_tensor_arena_size;
    ms->rv_arena = arrhythmia_model_power_var_arena;
    ms->rv_arena_size = arrhythmia_model_power_resource_var_arena_size;
    ms->rv_count = 0;
    ms->numInputTensors = 1;
    ms->numOutputTensors = 1;

#if (arrhythmia_model_power_MODEL_LOCATION == NS_AD_PSRAM)
    // Copy model to PSRAM
    arrhythmia_model_power_model_psram = (unsigned char *)(psram_cfg.psram_base_address);
    memcpy((unsigned char *)(psram_cfg.psram_base_address), arrhythmia_model_power_model, arrhythmia_model_power_model_len);
    ms->model_array = arrhythmia_model_power_model_psram;
#else
    ms->model_array = arrhythmia_model_power_model;
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

    int status = arrhythmia_model_power_init(ms);
    return status;
}

int arrhythmia_model_power_init(ns_model_state_t *ms) {
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
        return arrhythmia_model_power_STATUS_FAILURE;
    }

#ifdef NS_TFSTRUCTURE_RECENT
    static tflite::MicroMutableOpResolver<9> resolver;
#else
    static tflite::MicroMutableOpResolver<9> resolver(error_reporter);
#endif
    resolver.AddConv2D();
    resolver.AddDepthwiseConv2D();
    resolver.AddMaxPool2D();
    resolver.AddMean();
    resolver.AddMinimum();
    resolver.AddRelu();
    resolver.AddMul();
    resolver.AddAdd();
    resolver.AddFullyConnected();

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
    static tflite::MicroInterpreter static_interpreter(
        ms->model, resolver, ms->arena, ms->arena_size, resource_variables, ms->profiler);
    ms->interpreter = &static_interpreter;

    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = ms->interpreter->AllocateTensors();

    if (allocate_status != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(ms->error_reporter, "AllocateTensors() failed");
        ns_lp_printf("Arena size computed., %d\n", ms->interpreter->arena_used_bytes());

        return arrhythmia_model_power_STATUS_FAILURE;
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
    return arrhythmia_model_power_STATUS_SUCCESS;
}
