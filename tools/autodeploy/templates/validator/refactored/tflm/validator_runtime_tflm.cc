// ============================ validator_runtime_tflm.cc ============================
// C++ glue for the TFLM runtime. Implements ns_validator_rt_api_t via extern "C".

extern "C" {
    #include <stdint.h>
    #include <stddef.h>
    }
    
    #include "validator_runtime_iface.h"
    #include "validator_mem.h"
    #include "../tflm_validator.h"

    // neuralSPOT / TFLM includes
    #include "../mut_model_metadata.h"       // sizes, locations, RV count
    #include "ns_ambiqsuite_harness.h"    // ns_model_state_t, typedefs
    #include "ns_debug_log.h"
    #include "../tflm_ns_model.h"            // model helpers
    
    // Model-specific init function generated from template_tflm_model.cc
    extern int tflm_validator_model_init(ns_model_state_t *ms);
    
    // Resource Variable arena (always in TCM/SRAM per harness)
    #ifdef __cplusplus
    #include "tensorflow/lite/micro/micro_resource_variable.h"
    #endif
    extern const ns_perf_mac_count_t mac_estimates;
    #ifdef AM_PART_APOLLO5B
    extern ns_pmu_config_t s_pmu_cfg;
    #endif

    // Keep this aligned and in fast memory; size mirrors template logic
    static constexpr int kVarArenaSize =
        4 * (TFLM_VALIDATOR_MAX_RESOURCE_VARIABLES + 1) * sizeof(tflite::MicroResourceVariables);
    alignas(16) static uint8_t s_var_arena[kVarArenaSize];
    
    // Single runtime state
    static ns_model_state_t s_tflm;  // owned here
    
    // Minimal profiler hook: call LogCsv when available
    static void tflm_stats_hook(void){
    #ifdef NS_MLPROFILE
      if (s_tflm.profiler){
        s_tflm.profiler->LogCsv();
        ns_stop_perf_profiler();
      }
    #endif
    }
    
    // Helpers to get raw tensor pointers safely
    static inline uint8_t* tensor_data_u8(TfLiteTensor* t){
      // We treat as byte-addressable for transport; caller knows true size
      return reinterpret_cast<uint8_t*>(t->data.raw);
    }
    static inline const uint8_t* tensor_data_u8(const TfLiteTensor* t){
      return reinterpret_cast<const uint8_t*>(t->data.raw);
    }
    
    // ---- API impl ---------------------------------------------------------------
    static int rt_init(uint32_t num_inputs, uint32_t num_outputs, uint32_t profile, uint32_t warmup, ns_timer_config_t *tickTimer){
      (void)profile; (void)warmup; // handled by upper layer via mut_cfg
    
      // Fill required fields prior to init
      s_tflm.runtime = TFLM;
      s_tflm.model_array = ns_mem_model_ptr();
      s_tflm.arena       = ns_mem_arena_ptr();
      s_tflm.arena_size  = ns_mem_arena_size();
      s_tflm.rv_arena    = s_var_arena;
      s_tflm.rv_arena_size = kVarArenaSize;
      s_tflm.rv_count    = TFLM_VALIDATOR_MAX_RESOURCE_VARIABLES;
      s_tflm.numInputTensors  = num_inputs;
      s_tflm.numOutputTensors = num_outputs;
    #ifdef NS_MLPROFILE
      s_tflm.tickTimer = tickTimer;  // configured by app if profiling is enabled
      s_tflm.mac_estimates = &mac_estimates;
      #ifdef AM_PART_APOLLO5B
      s_tflm.pmu = &s_pmu_cfg;
      #endif
    #endif
    
      int rc = tflm_validator_model_init(&s_tflm);
      if (rc != 0){ ns_lp_printf("[ERROR] tflm_validator_model_init failed (%d)", rc); return -1; }
      return 0;
    }
    
    static int rt_set_input(uint32_t idx, const void* data, uint32_t len){
      if (idx >= s_tflm.numInputTensors) return -1;
      memcpy(tensor_data_u8(s_tflm.model_input[idx]), data, len);
      return 0;
    }
    
    static void* rt_map_input_writable(uint32_t idx, uint32_t* cap_out){
      if (idx >= s_tflm.numInputTensors) return nullptr;
      if (cap_out) *cap_out = 0xFFFFFFFFu; // caller knows exact size from config
      return (void*)tensor_data_u8(s_tflm.model_input[idx]);
    }
    
    static int rt_invoke(void){
      TfLiteStatus st = s_tflm.interpreter->Invoke();
      return (st == kTfLiteOk) ? 0 : -1;
    }
    
    static int rt_get_output(uint32_t idx, void* dst, uint32_t len){
      if (idx >= s_tflm.numOutputTensors) return -1;
      memcpy(dst, tensor_data_u8(s_tflm.model_output[idx]), len);
      return 0;
    }
    
    static const void* rt_map_output_readonly(uint32_t idx, uint32_t* size_out){
      if (idx >= s_tflm.numOutputTensors) return nullptr;
      if (size_out) *size_out = 0xFFFFFFFFu; // upper layer owns exact size
      return (const void*)tensor_data_u8(s_tflm.model_output[idx]);
    }
    
    static uint32_t rt_arena_used_bytes(void){
      return s_tflm.computed_arena_size;
    }
    
    static const ns_validator_rt_api_t kAPI = {
      rt_init,
      rt_set_input,
      rt_map_input_writable,
      rt_invoke,
      rt_get_output,
      rt_map_output_readonly,
      tflm_stats_hook,
      rt_arena_used_bytes
    };
    
    extern "C" const ns_validator_rt_api_t* ns_get_runtime_api(void){ return &kAPI; }