/**
 * @file basic_model.h
 * @author your name (you@domain.com)
 * @brief Basic TF Model init example
 * @version 0.1
 * @date 2022-10-26
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "basic_tf_stub.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_debug_log.h"

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

// TFLM model
// #include "model.h"
// #include "kws_input_data.h"
#include "kws_model_analysis.h"
#include "kws_model_data.h"
#include "kws_model_settings.h"

// Tensorflow Globals (somewhat boilerplate)
static tflite::ErrorReporter *error_reporter = nullptr;
static const tflite::Model *model = nullptr;
static tflite::MicroInterpreter *interpreter = nullptr;
static TfLiteTensor *model_input = nullptr;
static TfLiteTensor *model_output = nullptr;
static tflite::MicroProfiler *profiler = nullptr;

static constexpr int kTensorArenaSize = 1024 * 30;
alignas(16) static uint8_t tensor_arena[kTensorArenaSize];

#ifdef NS_MLPROFILE
// Timer is used for TF profiling
ns_timer_config_t basic_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};

#ifdef AM_PART_APOLLO5B
ns_pmu_config_t pmu_cfg;
#endif

#endif

/**
 * @brief Initialize TF with KWS model
 *
 * This code is fairly common across most TF-based models.
 * The major differences relate to input and output tensor
 * handling.
 *
 */
static void
model_init(void) {

    tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;
#ifdef NS_MLPROFILE
    static tflite::MicroProfiler micro_profiler;
    profiler = &micro_profiler;
    ns_perf_mac_count_t basic_mac = {.number_of_layers = kws_ref_model_number_of_estimates,
                                     .mac_count_map = kws_ref_model_mac_estimates};

    // Init the PMU config
    #ifdef AM_PART_APOLLO5B
    pmu_cfg.api = &ns_pmu_V1_0_0;
    ns_pmu_reset_config(&pmu_cfg);
    ns_pmu_event_create(&pmu_cfg.events[0], NS_PROFILER_PMU_EVENT_0, NS_PMU_EVENT_COUNTER_SIZE_32);
    ns_pmu_event_create(&pmu_cfg.events[1], NS_PROFILER_PMU_EVENT_1, NS_PMU_EVENT_COUNTER_SIZE_32);
    ns_pmu_event_create(&pmu_cfg.events[2], NS_PROFILER_PMU_EVENT_2, NS_PMU_EVENT_COUNTER_SIZE_32);
    ns_pmu_event_create(&pmu_cfg.events[3], NS_PROFILER_PMU_EVENT_3, NS_PMU_EVENT_COUNTER_SIZE_32);  
    ns_pmu_init(&pmu_cfg);
    #endif
    // Create the config struct for the debug log
    ns_debug_log_init_t cfg = {
        .t = &basic_tickTimer,
        .m = &basic_mac,
        #ifdef AM_PART_APOLLO5B
        .pmu = &pmu_cfg,
        #endif
    };

    ns_TFDebugLogInit(&cfg);
#elif NS_MLDEBUG
    ns_TFDebugLogInit(NULL);
#endif

    tflite::InitializeTarget();

    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    model = tflite::GetModel(g_kws_model_data);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        TF_LITE_REPORT_ERROR(error_reporter,
                             "Model provided is schema version %d not equal "
                             "to supported version %d.",
                             model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }

    // Build mutable resolver with minimum opset
#ifdef NS_TFSTRUCTURE_RECENT
    static tflite::MicroMutableOpResolver<6> resolver;
#else
    static tflite::MicroMutableOpResolver<6> resolver(error_reporter);
#endif
    resolver.AddFullyConnected();
    resolver.AddConv2D();
    resolver.AddDepthwiseConv2D();
    resolver.AddReshape();
    resolver.AddSoftmax();
    resolver.AddAveragePool2D();

    // static tflite::AllOpsResolver resolver;

    // Build an interpreter to run the model with.
#ifdef NS_TFSTRUCTURE_RECENT
    static tflite::MicroInterpreter static_interpreter(model, resolver, tensor_arena,
                                                       kTensorArenaSize, nullptr, profiler);
#else
    static tflite::MicroInterpreter static_interpreter(
        model, resolver, tensor_arena, kTensorArenaSize, error_reporter, nullptr, profiler);
#endif
    interpreter = &static_interpreter;

    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed\n");
        ns_lp_printf("AllocateTensors() failed with status %d\n", allocate_status);
        return;
    }

    ns_lp_printf("Arena size computed: %d\n", interpreter->arena_used_bytes());

    // Obtain pointers to the model's input and output tensors.
    model_input = interpreter->input(0);
    model_output = interpreter->output(0);
}
